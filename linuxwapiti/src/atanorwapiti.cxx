#include "atanor.h"
#include "atanorversion.h"
#include "atanorwapiti.h"
#include "atanorstring.h"

#ifndef WIN32
#define _strdup strdup
#endif

//We need to declare once again our local definitions.
hmap<unsigned short, wapitiMethod>  Atanorwapiti::methods;
hmap<string, string> Atanorwapiti::infomethods;
bin_hash<unsigned long> Atanorwapiti::exported;

short Atanorwapiti::idtype = 0;

//------------------------------------------------------------------------------------------------------------------
//If you need to implement an external library... Otherwise remove it...
//When ATHANOR loads a library, it looks for this method in particular, which then make this object available into ATHANOR
extern "C" {
    Exporting bool libwapiti_InitialisationModule(AtanorGlobal* global, string version) {
        if (version != AtanorVersion())
            return false;

        global->Update();

        return Atanorwapiti::InitialisationModule(global, version);
    }
}
//------------------------------------------------------------------------------------------------------------------

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorwapiti::AddMethod(AtanorGlobal* global, string name, wapitiMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanorwapiti::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    //Each new object has a specific name, which will help recognize it in the code that will exploit wapiti...
    Atanorwapiti::idtype = global->Getid("wapiti");

    //You declare your methods here:
    // Argument 1 is a call to global, which will record your method into ATHANOR
    // Argument 2 is the name of your command
    // Argument 3 links your command with its implementation into the object class
    // Argument 4 is a combination of P_... which defines how many parameters this function can accept.
    //Argument 5 is a description of this method.
	Atanorwapiti::AddMethod(global, "loadmodel", &Atanorwapiti::MethodLoadModel, P_ONE, "loadmodel(string crfmodel): Loading a CRF model.");
	Atanorwapiti::AddMethod(global, "options", &Atanorwapiti::MethodOptions, P_ONE, "options(map options): Setting options.");
	Atanorwapiti::AddMethod(global, "train", &Atanorwapiti::MethodTraining, P_NONE, "train(): Launch training.");
	Atanorwapiti::AddMethod(global, "label", &Atanorwapiti::MethodLabelling, P_ONE, "label(vector words): Launch labelling.");
	Atanorwapiti::AddMethod(global, "lasterror", &Atanorwapiti::MethodLasterror, P_NONE, "lasterror(): Return the last error.");

    //We need this code, in order to create new instances of our wapiti object... DO NOT ALTER
    global->newInstance[Atanorwapiti::idtype] = new Atanorwapiti(global);
    global->RecordMethods(Atanorwapiti::idtype,Atanorwapiti::exported);
    
    return true;
}



Atanor* Atanorwapiti::MethodLoadModel(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (loadingmode)
		return globalAtanor->Returnerror("WPT(004): Model already loaded", idthread);
	string model = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	for (int i = 0; i < 100; i++)
		args[i][0] = 0;

	strcpy_s(args[0], 100, "wapiti");
	strcpy_s(args[1], 100, "label");
	strcpy_s(args[2], 100, "-m");
	strcpy_s(args[3], 100, STR(model));
	initopt(opt);
	opt_parse(4, args, &opt);
	char* erreur = CRFError();
	if (erreur[0] != 0) {
		model = "WPT(099): ";
		model += erreur;
		return globalAtanor->Returnerror(model, idthread);
	}

	loadingmode = 1;
	return aTRUE;
}

Atanor* Atanorwapiti::MethodOptions(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (loadingmode)
		return globalAtanor->Returnerror("WPT(004): Model already loaded", idthread);

	Atanor* koptions = callfunc->Evaluate(0, contextualpattern, idthread);
	if (!koptions->isVectorContainer())
		return globalAtanor->Returnerror("WPT(014): Expecting a vector container", idthread);

	int i;
	for (i = 0; i < 100; i++)
		args[i][0] = 0;

	AtanorIteration* itr = koptions->Newiteration(false);
	string s;
	i = 0;
	strcpy_s(args[i++], 100, "wapiti");
	for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
		s = itr->Valuestring();
		strcpy_s(args[i++], 100, STR(s));
	}
	itr->Release();

	initopt(opt);
	opt_parse(i, args, &opt);
	char* erreur = CRFError();
	if (erreur[0] != 0) {
		string model = "WPT(099): ";
		model += erreur;
		return globalAtanor->Returnerror(model, idthread);
	}
	loadingmode = 1;
	return aTRUE;
}

Atanor* Atanorwapiti::MethodTraining(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (!loadingmode || opt.mode != 0)
		return globalAtanor->Returnerror("WPT(021): No options provided", idthread);
	DoTrain(&opt);
	return aTRUE;
}

Atanor* Atanorwapiti::MethodLasterror(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	string s = WAPITILastError();
	return globalAtanor->Providestring(s);
}

Atanor* Atanorwapiti::MethodLabelling(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (!loadingmode || opt.mode == 0)
		return globalAtanor->Returnerror("WPT(022): No options provided", idthread);

	WAPITIInit();

	Atanor* kwords = callfunc->Evaluate(0, contextualpattern, idthread);
	if (!kwords->isVectorContainer())
		return globalAtanor->Returnerror("WPT(015): Expecting a vector container", idthread);

	int sz = kwords->Size();
	if (sz == 0)
		return aFALSE;

	if (crftagger == NULL) {
		crftagger = CreatePureTagger(&opt);
		loadingmode = 2;
		if (!InitPureTagger(crftagger))
			return aFALSE;
	}

	char** wordSequence = new char*[sz + 1];
	char** predictedTag = new char*[sz + 1];
	vector<string> wrds;
	int i;
	for (i = 0; i < sz; i++) {
		wrds.push_back(kwords->getstring(i));
		wordSequence[i] = _strdup(STR(wrds[i]));
		predictedTag[i] = NULL;
	}

	if (!LabelPureTagger(crftagger, wordSequence, predictedTag, sz))
		return aFALSE;

	kwords = Selectasvector(contextualpattern);
	string v;
	for (i = 0; i < sz; i++)
		kwords->storevalue(predictedTag[i]);

	delete[] wordSequence;
	delete[] predictedTag;
	return kwords;
}


