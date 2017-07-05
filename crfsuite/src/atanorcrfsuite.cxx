#include "atanor.h"
#include "atanorversion.h"
#include "option.h"
#include <crfsuite.h>

#include "atanorcrfsuite.h"

extern "C" {
	int crfsuite_call(int argc, char *argv[], tagger_option_t* opt);
	int crfsuite_tagging(char* filename, char* outputfilename, tagger_option_t* opt);
	void crf_suite_closing_model(tagger_option_t* opt);
	void tagger_option_init(tagger_option_t* opt);
	void tagger_option_finish(tagger_option_t* opt);
	int crfsuite_tagging_string(char* astream, int sz, tagger_option_t* opt);
}

extern "C" {
	void atan_storing_double(void* call, const char* label, double v);
	void atan_storing_label(void* call, const char* label);
	void atan_storing_label_sep(void* call, const char* label);
	void atan_storing_next(void* call);
	void atan_storing_next_final(void* call);
}

void atan_storing_double(void* call, const char* label, double v) {
	Atanorcrfsuite* kcrf = (Atanorcrfsuite*)call;
	Atanorstring* kstr = globalAtanor->Providestring(label);
	Atanorfloat* kfloat = globalAtanor->Providefloat(v);
	kcrf->kvectoutput->Push(kstr);
	kcrf->kvectoutput->Push(kfloat);
}

void atan_storing_label_sep(void* call, const char* label) {
	Atanorcrfsuite* kcrf = (Atanorcrfsuite*)call;
	Atanorstring* kstr = globalAtanor->Providestring(label);
	kstr->value += "\t";
	kcrf->kvectoutput->Push(kstr);
}

void atan_storing_label(void* call, const char* label) {
	Atanorcrfsuite* kcrf = (Atanorcrfsuite*)call;
	Atanorstring* kstr = globalAtanor->Providestring(label);
	kcrf->kvectoutput->Push(kstr);
}

void atan_storing_next(void* call) {
	Atanorcrfsuite* kcrf = (Atanorcrfsuite*)call;
	Atanorstring* kstr = globalAtanor->Providestring(kcrf->separator);
	kcrf->kvectoutput->Push(kstr);
}

void atan_storing_next_final(void* call) {
	Atanorcrfsuite* kcrf = (Atanorcrfsuite*)call;
	Atanorstring* kstr = globalAtanor->Providestring(kcrf->endoftagging);
	kcrf->kvectoutput->Push(kstr);
}

//We need to declare once again our local definitions.
hmap<unsigned short, crfsuiteMethod>  Atanorcrfsuite::methods;
hmap<string, string> Atanorcrfsuite::infomethods;
bin_hash<unsigned long> Atanorcrfsuite::exported;

short Atanorcrfsuite::idtype = 0;

//------------------------------------------------------------------------------------------------------------------
//If you want to implement an external library... Otherwise remove it...
extern "C" {
	Exporting bool libcrfsuite_InitialisationModule(AtanorGlobal* global, string version) {
		if (version != AtanorVersion())
			return false;

		global->Update();

		return Atanorcrfsuite::InitialisationModule(global, version);
	}
}
//------------------------------------------------------------------------------------------------------------------
Atanorcrfsuite::Atanorcrfsuite(AtanorGlobal* g, Atanor* parent) : AtanorObject(g, parent) {
	//Do not forget your variable initialisation
	separator = "#";
	endoftagging = "$";
	kvectoutput = NULL;
	tagger_option_init(&opt);
	opt.caller = (void*)this;
	for (int i = 0; i < 100; i++)
		args[i] = (char*)malloc(1024);
}

Atanorcrfsuite::~Atanorcrfsuite() {
	crf_suite_closing_model(&opt);
	tagger_option_finish(&opt);
	for (int i = 0; i < 100; i++)
		free(args[i]);
}

//------------------------------------------------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorcrfsuite::AddMethod(AtanorGlobal* global, string name, crfsuiteMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorcrfsuite::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorcrfsuite::idtype = global->Getid("crfsuite");

	Atanorcrfsuite::AddMethod(global, "learn", &Atanorcrfsuite::MethodLearn, P_ONE, "learn(svector args): Launching learning");
	Atanorcrfsuite::AddMethod(global, "tagfile", &Atanorcrfsuite::MethodTag, P_NONE | P_ONE | P_TWO, "tagfile(string filename,string outputfile): tagging a file.");
	Atanorcrfsuite::AddMethod(global, "tag", &Atanorcrfsuite::MethodSTag, P_ONE, "tag(svector tokens): tagging a vector of tokens.");
	Atanorcrfsuite::AddMethod(global, "load", &Atanorcrfsuite::MethodLoad, P_ONE, "load(svector args): loading a model.");
	Atanorcrfsuite::AddMethod(global, "close", &Atanorcrfsuite::MethodClose, P_NONE, "close(): closing a model.");
	Atanorcrfsuite::AddMethod(global, "separators", &Atanorcrfsuite::MethodSeps, P_TWO, "separators(string separator,string endoftagging): setting how the fields will be separated.");

	global->newInstance[Atanorcrfsuite::idtype] = new Atanorcrfsuite(global);
	global->RecordMethods(Atanorcrfsuite::idtype,Atanorcrfsuite::exported);
	
	return true;
}

AtanorIteration* Atanorcrfsuite::Newiteration(bool direction) {
	return aITERNULL;
}


Atanor* Atanorcrfsuite::Put(Atanor* idx, Atanor* kval, short idthread) {
	return aTRUE;
}

Atanor* Atanorcrfsuite::Get(Atanor* context, Atanor* idx, short idthread) {
	return this;
}



Atanor* Atanorcrfsuite::MethodLearn(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	Atanor* first = callfunc->Evaluate(0, contextualpattern, idthread);
	if (first->isVectorContainer() == false)
		return globalAtanor->Returnerror("CST(001): Expecting a vector for options as input", idthread);


	for (int i = 0; i < 100; i++)
		args[i][0] = 0;

	int argc = 2;
	strcpy_s(args[0], 100, "crfsuite");
	strcpy_s(args[1], 100, "learn");
	string val;
	for (int i = 0; i < first->Size(); i++) {
		val = first->getstring(i);
		strcpy_s(args[i + 2], 100, STR(val));
		++argc;
	}

	crfsuite_call(argc, args, &opt);
	//you may return any value of course...
	return aTRUE;
}

Atanor* Atanorcrfsuite::MethodLoad(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	Atanor* first = callfunc->Evaluate(0, contextualpattern, idthread);
	if (first->isVectorContainer() == false)
		return globalAtanor->Returnerror("CST(001): Expecting a vector for options as input", idthread);


	for (int i = 0; i < 100; i++)
		args[i][0] = 0;

	int argc = 2;
	strcpy_s(args[0], 100, "crfsuite");
	strcpy_s(args[1], 100, "tag");
	string val;
	for (int i = 0; i < first->Size(); i++) {
		val = first->getstring(i);
		strcpy_s(args[i + 2], 100, STR(val));
		++argc;
	}

	if (!crfsuite_call(argc, args, &opt))
		return aTRUE;

	//you may return any value of course...
	return globalAtanor->Returnerror("CST(005): Error while processing model", idthread);
}

Atanor* Atanorcrfsuite::MethodTag(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (opt.themodel == NULL)
		return globalAtanor->Returnerror("CST(004): Missing model", idthread);

	opt.store_in_struct = 1;
	if (callfunc->Size() >= 1) {
		string name = callfunc->Evaluate(0, contextualpattern, idthread)->String();
		strcpy_s(args[0], 1024, STR(name));
		if (callfunc->Size() == 2) {
			opt.store_in_struct = 0;
			name = callfunc->Evaluate(1, contextualpattern, idthread)->String();
			strcpy_s(args[1], 1024, STR(name));
			if (!crfsuite_tagging(args[0], args[1], &opt))
				return aTRUE;
			return globalAtanor->Returnerror("CST(006): Error while processing tagging file", idthread);
		}
		kvectoutput = globalAtanor->Providevector();
		crfsuite_tagging(args[0], NULL, &opt);
	}
	else {
		kvectoutput = globalAtanor->Providevector();
		crfsuite_tagging(NULL, NULL, &opt);
	}
	return kvectoutput;
}

Atanor* Atanorcrfsuite::MethodSTag(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (opt.themodel == NULL)
		return globalAtanor->Returnerror("CST(004): Missing model", idthread);

	opt.store_in_struct = 1;
	kvectoutput = globalAtanor->Providevector();
	Atanor* kinput = callfunc->Evaluate(0, contextualpattern, idthread);
	if (kinput->isVectorContainer() == false)
		return globalAtanor->Returnerror("CST(003): Expecting a vector input", idthread);
	string content;
	for (int i = 0; i < kinput->Size(); i++) {
		content += kinput->getstring(i);
		content += "\n";
	}
	crfsuite_tagging_string(STR(content), content.size(), &opt);
	return kvectoutput;
}

Atanor* Atanorcrfsuite::MethodClose(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	crf_suite_closing_model(&opt);
	opt.themodel = NULL;
	return aTRUE;
}

Atanor* Atanorcrfsuite::MethodSeps(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	separator = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	endoftagging = callfunc->Evaluate(1, contextualpattern, idthread)->String();
	return aTRUE;
}

