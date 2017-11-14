
/*
*  Athanor: Le langage des Alpages mède à Grenoble
*
*  Copyright (C) 2017: ATHANOR Language
* This file can only be used with the ATHANOR library or the executable
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : ATHANOR
Version    : ATANOR 0.26
filename   : procedures.cxx
Date       : 2017/04/01
Purpose    :
Programmer : Claude ROUX
Reviewer   :
*/

//Definition of procedures
#ifdef WIN32
#include "Windows.h"
#define _USE_MATH_DEFINES
#include <math.h>
#endif

#include "atanor.h"
#include "atanortreemapls.h"
#include "atanorversion.h"
#include "instructions.h"
#include "atanorstring.h"
#include "atanorvector.h"
#include "atanorivector.h"
#include "atanordvector.h"
#include "atanorfvector.h"
#include "atanorsvector.h"
#include "atanoruvector.h"
#include "atanorbvector.h"
#include "atanormap.h"
#include "constobjects.h"
#include "atanorprimemap.h"
#include "compilecode.h"
#include "atanoratanor.h"
#include "atanorlvector.h"
#include "atanormapss.h"
#include "atanorframeinstance.h"

#ifdef UNIX
#include <sys/time.h>
#endif

//---------------------------------------------------------
Atanor* ProcThreadhandle(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

Atanor* ProcThreadid(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	return globalAtanor->Provideint(idthread);
}

//---------------------------------------------------------
Atanor* ProcRandom(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	long mx = 100;
	if (callfunc->Size() == 1)
		mx = callfunc->Evaluate(0, aNULL, idthread)->Float();
	return globalAtanor->Providefloat(localrandom(mx));
}
//------------------------------------------------------------------------------------------------------------------------
Atanor* ProcCreate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	short n = callfunc->Name();
	
	if (callfunc->Size() == 1) {
		Atanor* value = callfunc->Evaluate(0, aNULL, idthread);
		if (value->isError())
			return value;

		Atanor* object = globalAtanor->newInstance[n]->Newinstance(idthread);
		if (n == a_atanor) {
			object = AtanorRecordedFiles(value->String());
			if (object != NULL)
				return object;
		}
		object->Put(contextualpattern, value, idthread);
		return object;
	}

	return globalAtanor->newInstance[n]->Newinstance(idthread);
}

Atanor* ProcCreateFrame(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* object = globalAtanor->newInstance[callfunc->Name()]->Newinstance(idthread);
	if (object->Frame()->isDeclared(a_initial)) {
		//callfunc->name = a_initial;
		object->Setreference();

		Atanor* a = ((Atanorframeinstance*)object)->MethodInitial(contextualpattern, idthread, callfunc);
		//Atanor* a = object->CallMethod(a_initial, contextualpattern, idthread, callfunc);

		if (globalAtanor->Error(idthread)) {
			object->Resetreference();
			a->Release();
			return globalAtanor->Errorobject(idthread);
		}

		object->Protect();
		a->Release();
	}
	else {
		if (callfunc->Size() != 0) {
			object->Release();
			return globalAtanor->Returnerror("Wrong frame initialization", idthread);
		}
	}

	return object;
}
//------------------------------------------------------------------------------------------------------------------------
Atanor* ProcAllObjects(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanorsvector* vs = (Atanorsvector*)Selectasvector(contextualpattern);
	globalAtanor->AtanorAllObjects(vs->values);
	map<string, bool> inter;
	for (long i = 0; i < vs->values.size(); i++)
		inter[vs->values[i]] = true;


	vs->values.clear();
	for (auto& it : inter)
		vs->values.push_back(it.first);
	return vs;
}

//------------------------------------------------------------------------------------------------------------------------

Atanor* ProcBreakPoint(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	long i = callfunc->Evaluate(0, aNULL, idthread)->Integer();

	return globalAtanor->Provideint(i);
}
//------------------------------------------------------------------------------------------------------------------------

Atanor* ProcMax(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* current = callfunc->Evaluate(0, aNULL, idthread);
	Atanor* right;

	for (size_t i = 1; i < callfunc->Size(); i++) {
		right = callfunc->Evaluate(i, aNULL, idthread);

		if (current->less(right)->Boolean())
			current = right;
	}

	return current;
}

Atanor* ProcMin(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* current = callfunc->Evaluate(0, aNULL, idthread);
	Atanor* right;

	for (size_t i = 1; i < callfunc->Size(); i++) {
		right = callfunc->Evaluate(i, aNULL, idthread);

		if (current->more(right)->Boolean())
			current = right;
	}

	return current;
}
//------------------------------------------------------------------------------------------------------------------------
Atanor* ProcVersion(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	return globalAtanor->Providestring(AtanorVersion());
}
//------------------------------------------------------------------------------------------------------------------------
Atanor* ProcMirrorDisplay(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	globalAtanor->doubledisplay = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
	return aTRUE;
}
//------------------------------------------------------------------------------------------------------------------------
Atanor* ProcErrorOnKey(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	bool v = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
	globalAtanor->erroronkey = v;
	return aTRUE;
}
//------------------------------------------------------------------------------------------------------------------------
Atanor* ProcExit(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	globalAtanor->executionbreak = true;
	return aEND;
}

//------------------------------------------------------------------------------------------------------------------------
//Initialize the official valid features for synodes
Atanor* ProcSetValidFeatures(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	short idname = globalAtanor->Getid("definitions");
	AtanorCallMethod call(idname);
	return globalAtanor->newInstance[a_synode]->CallMethod(idname, contextualpattern, idthread, callfunc);
}
//------------------------------------------------------------------------------------------------------------------------
Atanor* ProcForceLocks(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	globalAtanor->globalLOCK = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
	return aTRUE;
}
//------------------------------------------------------------------------------------------------------------------------
void SetEnvironmentVariable(string command, string val);
Atanor* ProcSetEnv(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	string key = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	string val = callfunc->Evaluate(1, contextualpattern, idthread)->String();
	SetEnvironmentVariable(key, val);
	return aTRUE;
}
//------------------------------------------------------------------------------------------------------------------------
Atanor* ProcStackSize(Atanor* domain, short idthread, AtanorCall* callfunc) {
	long sz = callfunc->Evaluate(0, domain, idthread)->Integer();
	if (sz > globalAtanor->maxstack)
		globalAtanor->maxstack = sz;
	return aTRUE;
}
//------------------------------------------------------------------------------------------------------------------------
Atanor* ProcMaxThreads(Atanor* domain, short idthread, AtanorCall* callfunc) {
	long sz = callfunc->Evaluate(0, domain, idthread)->Integer();
	globalAtanor->Reallocatemaxthreads(sz);
	return aTRUE;
}
//------------------------------------------------------------------------------------------------------------------------
Atanor* ProcMaxJoined(Atanor* domain, short idthread, AtanorCall* callfunc) {
	long sz = callfunc->Evaluate(0, domain, idthread)->Integer();
	globalAtanor->Reallocatemaxjoined(sz);
	return aTRUE;
}
//------------------------------------------------------------------------------------------------------------------------
Atanor* ProcMaxMaxRange(Atanor* domain, short idthread, AtanorCall* callfunc) {
	BLONG sz = callfunc->Evaluate(0, domain, idthread)->Long();
	globalAtanor->maxrange = sz;
	return aTRUE;
}
//------------------------------------------------------------------------------------------------------------------------
Atanor* ProcUse(Atanor* domain, short idthread, AtanorCall* callfunc) {
	string filename = callfunc->Evaluate(0, domain, idthread)->String();
	AtanorCode* acode = globalAtanor->Getcurrentcode();
	string name;
	acode->Loadlibrary(filename, name);
	return aTRUE;
}


//------------------------------------------------------------------------------------------------------------------------
//Loading a file into the current atanor code... See AtanorCode::C_regularcall to check the call to this procedure
Atanor* ProcLoadin(Atanor* domain, short idthread, AtanorCall* callfunc) {

	string filename = callfunc->Evaluate(0, domain, idthread)->String();
	//It has already been loaded once for all...
	for (int i = 0; i < globalAtanor->filenames.size(); i++) {
		if (globalAtanor->filenames[i] == filename)
			return aTRUE;
	}

	string code;
	ifstream file(filename, openMode);
	if (file.fail()) {
		stringstream message;
		message << "Cannot open file: " << filename;
		AtanorError* err = new AtanorError(message.str());
		return err;
	}

	string line;
	char c = file.get();
	if (!file.eof()) {
		do {
			code += c;
			c = file.get();

		} while (!file.eof());
	}

	file.close();

	//We will store our content into the current code space
	AtanorCode* acode = globalAtanor->Getcurrentcode();

	//A loadin takes place into the loading of another file, whose index we save
	short id = acode->currentfileid;
	string currentfilename = acode->filename;
	//Our new filename is pushed into the current code space
	acode->SetFilename(filename);
	//We protect the current parse tree
	bnf_atanor* current = globalAtanor->currentbnf;
	size_t previousfirstinstruction = acode->firstinstruction;
	//We compile our code
	acode->Compile(code);

	acode->firstinstruction = previousfirstinstruction;

	//we reset our our parse tree
	globalAtanor->currentbnf = current;
	//and we put back the current filename id...
	acode->currentfileid = id;
	acode->filename = currentfilename;
	if (globalAtanor->Error(idthread)) {
		stringstream err;
		err << globalAtanor->Errorstring(idthread);
		if (globalAtanor->lineerror < 0)
			globalAtanor->lineerror = 0;

		throw new AtanorRaiseError(err, filename, globalAtanor->lineerror, globalAtanor->lineerror);
	}
	return aTRUE;
}
//------------------------------------------------------------------------------------------------------------------------
Atanor* ProcEvalFunction(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	string code = callfunc->Evaluate(0, contextualpattern, idthread)->String();

	Locking _lock(globalAtanor);
	AtanorCode* acode = globalAtanor->Getcurrentcode();
	long lastinstruction = acode->InstructionSize();
	Atanor* ci = globalAtanor->threads[idthread].currentinstruction;
	long lastrecorded;
	
	lastrecorded = globalAtanor->Trackedsize();
	Atanor* compiled = acode->Compilefunction(code);
	if (compiled == NULL || !compiled->isFunction()) {
		globalAtanor->threads[idthread].currentinstruction = callfunc;
		if (compiled != NULL) {
			if (compiled->Type() == a_callhaskell)
				return compiled->Body(idthread);
			if (compiled->isError())
				return compiled;
		}
		if (globalAtanor->Error(idthread))
			return globalAtanor->Errorobject(idthread);
		return globalAtanor->Returnerror("Cannot compile this function declaration", idthread);
	}

	return compiled;
}

Atanor* ProcEval(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (contextualpattern->Action() == a_call)
		return ProcEvalFunction(contextualpattern, idthread, callfunc);

	string code = callfunc->Evaluate(0, contextualpattern, idthread)->String();

	Locking _lock(globalAtanor);
	AtanorCode* acode = globalAtanor->Getcurrentcode();
	long lastinstruction = acode->InstructionSize();
	Atanor* ci = globalAtanor->threads[idthread].currentinstruction;
	long lastrecorded;
	
	lastrecorded = globalAtanor->Trackedsize();
	Atanor* compiled = acode->Compilefunction(code);
	if (compiled == NULL || compiled->isError() || globalAtanor->Error(idthread)) {
		globalAtanor->threads[idthread].currentinstruction = callfunc;

		if (compiled != NULL && compiled->isError())
			return compiled;
		if (globalAtanor->Error(idthread))
			return globalAtanor->Errorobject(idthread);		
		return globalAtanor->Returnerror("Cannot compile this code", idthread);
	}

	if (compiled->isFunction() || compiled->Type() == a_callhaskell)
		return compiled;

	acode->compilemode = true;
	Atanor* a = acode->Execute(lastinstruction, idthread);

	globalAtanor->Cleanfrom(lastrecorded);
	globalAtanor->threads[idthread].currentinstruction = ci;
	
	if (a->isReturned())
		return a->Returned(idthread);

	return a;
}

//------------------------------------------------------------------------------------------------------------------------
Atanor* ProcRedictectOutput(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	string filename = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	bool iserr = callfunc->Evaluate(1, contextualpattern, idthread)->Boolean();
	int output = 1;
	if (iserr)
		output = 2;
	output = redirect_output(filename, output);
	if (output == -2)
		return globalAtanor->Returnerror("SYS(010): Stream has already been redirected", idthread);
	if (output == -1)
		return globalAtanor->Returnerror("SYS(001): Cannot open output file", idthread);
	return globalAtanor->Provideint(output);
}

Atanor* ProcRestateOutput(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	long output = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	bool iserr = callfunc->Evaluate(1, contextualpattern, idthread)->Boolean();
	if (iserr)
		output = restate_output(output, 2);
	else
		output = restate_output(output, 1);
	if (output == -1)
		return globalAtanor->Returnerror("SYS(011): Stream not opened", idthread);
	return aTRUE;
}


//------------------------------------------------------------------------------------------------------------------------
Atanor* ProcWaitOnJoin(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	{
		Locking _lock(globalAtanor->_join);
		if (globalAtanor->threads[idthread].joined.size() == 0)
			return aFALSE;
	}

	Atanor* func = NULL;
	Atanor* object = NULL;
	long idobject = -1;
	if (callfunc->Size() != 0) {
		func = callfunc->Evaluate(0, aNULL, idthread);
		if (!func->isFunction())
			return globalAtanor->Returnerror("Argument shoud be a function", idthread);
		if (callfunc->Size() == 2) {
			object = callfunc->Evaluate(1, aNULL, idthread);
			object->Setreference();
		}
	}

	VECTE<THREADPTR>& v = globalAtanor->threads[idthread].joined;
	bool alive = true;

	std::thread* t = NULL;

	while (alive) {
		alive = false;

		if (func != NULL) {
			AtanorCallFunction call(func);
			if (object != NULL)
				call.arguments.push_back(object);

			Atanor* a = call.Get(aNULL, aNULL, idthread);
			a->Release();
		}

		for (int i = 0; i < globalAtanor->maxjoined; i++) {
			if (globalAtanor->Error(idthread)) {
				alive = false;
				break;
			}

			t = NULL;
			{
				Locking _lock(globalAtanor->_join);
				if (i >= v.size())
					continue;

				if (v.vecteur[i] != NULL) {
					t = v.vecteur[i];
					alive = true;
				}
			}
			if (t != NULL)
				t->join();
		}
	}

	if (object != NULL)
		object->Resetreference();
	v.clear();
	return aTRUE;
}

Atanor* ProcSleep(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	long v = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	std::this_thread::sleep_for(std::chrono::milliseconds(v));
	return aTRUE;
}

Atanor* ProcPause(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* kfirst = callfunc->Evaluate(0, aNULL, idthread);
	double res = kfirst->Float() * 1000;
	char car = 8;
	bool display = false;
	if (callfunc->Size() == 2)
		display = callfunc->Evaluate(1, aNULL, idthread)->Boolean();
	timeval tempsfinal;
	gettimeofday(&tempsfinal, NULL);
	double init = (((unsigned long)tempsfinal.tv_sec) * 1000 + ((unsigned long)tempsfinal.tv_usec) / 1000.0);

	while (timeminus(init, tempsfinal) < res) {
		if (display) {
			cerr << "/" << car << "/" << car << "/" << car;
			cerr << "-" << car << "-" << car << "-" << car;
			cerr << "\\" << car << "\\" << car << "\\" << car;
		}
		gettimeofday(&tempsfinal, NULL);
	}
	return aTRUE;
}

//___________________________________________________________________________________________________

Atanor* ProcSum(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* v = callfunc->Evaluate(0, contextualpattern, idthread);
	if (!v->isVectorContainer())
		return globalAtanor->Returnerror("Expecting a vector container", idthread);

	long sz = v->Size();
	if (!sz)
		return aZERO;

	long i = 0;
	long j = sz;
	if (callfunc->Size() > 1) {
		i = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
		if (callfunc->Size() == 3)
			j = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
	}

	return v->Thesum(i, j);
}

Atanor* ProcProduct(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* v = callfunc->Evaluate(0, contextualpattern, idthread);
	if (!v->isVectorContainer())
		return globalAtanor->Returnerror("Expecting a vector container", idthread);

	long sz = v->Size();
	if (!sz)
		return aZERO;

	long i = 0;
	long j = sz;
	if (callfunc->Size() > 1) {
		i = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
		if (callfunc->Size() == 3)
			j = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
	}

	return v->Theproduct(i, j);
}

//___________________________________________________________________________________________________

Atanor* ProcCatch(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (globalAtanor->Error(idthread) == false)
		return aFALSE;

	if (callfunc->Size() != 1) {
		globalAtanor->Cleanerror(idthread);
		return aTRUE;
	}
	string resstr = globalAtanor->Errorstring(idthread);
	globalAtanor->Cleanerror(idthread);
	Atanor* val = globalAtanor->Providestring(resstr);
	Atanor* res = callfunc->Evaluate(0, contextualpattern, idthread);

	res->Put(aNULL, val, idthread);
	val->Release();
	return aTRUE;

}
//___________________________________________________________________________________________________
Atanor* ProcEmojis(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanortreemapls* amap = new Atanortreemapls;
	l_emojis(amap->values);
	return amap;
}

//___________________________________________________________________________________________________

Atanor* ProcPrint(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* res = aNULL;
	Atanorstring* kval = (Atanorstring*)globalAtanor->stringbuffer;
	string todisplay;
#ifdef DOSOUTPUT
	bool convert = false;
	if (globalAtanor->os == &cout)
		convert = true;
#endif

	for (int i = 0; i < callfunc->Size(); i++) {
		res = callfunc->Evaluate(i, aNULL, idthread);
		if (globalAtanor->Error(idthread))
			return globalAtanor->Errorobject(idthread);
		todisplay += res->StringToDisplay();
	}

	Locking _lock(globalAtanor->_printlock);
	if (kval == NULL) {
		if (globalAtanor->displayfunction == NULL) {
#ifdef DOSOUTPUT
			if (convert) {
				string sdos = conversion2dos(todisplay);
				*globalAtanor->os << sdos;
			}
			else
				*globalAtanor->os << todisplay;
#else
			*globalAtanor->os << todisplay;
#endif
		}
		else
			globalAtanor->displayfunction(todisplay, globalAtanor->displayobject);
		if (globalAtanor->doubledisplay) {
#ifdef DOSOUTPUT
			string sdos = conversion2dos(todisplay);
			cout << sdos;
#else
			cout << todisplay;
#endif
		}
	}
	else
		kval->value += todisplay;

	return aTRUE;
}


Atanor* ProcPrintLN(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* res = aNULL;
	Atanorstring* kval = (Atanorstring*)globalAtanor->stringbuffer;
	string todisplay;
#ifdef DOSOUTPUT
	bool convert = false;
	if (globalAtanor->os == &cout)
		convert = true;
#endif

	for (int i = 0; i < callfunc->Size(); i++) {
		if (i)
			todisplay += " ";
		res = callfunc->Evaluate(i, contextualpattern, idthread);
		if (globalAtanor->Error(idthread))
			return globalAtanor->Errorobject(idthread);

		todisplay += res->StringToDisplay();
	}

	todisplay += Endl;

	Locking _lock(globalAtanor->_printlock);
	if (kval == NULL) {
		if (globalAtanor->doubledisplay) {
#ifdef DOSOUTPUT
			string sdos = conversion2dos(todisplay);
			cout << sdos;
#else
			cout << todisplay;
#endif
		}
		if (globalAtanor->displayfunction == NULL) {
#ifdef DOSOUTPUT
			if (convert) {
				string sdos = conversion2dos(todisplay);
				*globalAtanor->os << sdos;
			}
			else
				*globalAtanor->os << todisplay;
#else
			*globalAtanor->os << todisplay;
#endif
		}
		else
			globalAtanor->displayfunction(todisplay, globalAtanor->displayobject);
	}
	else
		kval->value += todisplay;

	return aTRUE;
}


Atanor* ProcPrintFlush(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	globalAtanor->os->flush();
	return aTRUE;
}

Atanor* ProcPrinterr(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* res = aNULL;
	Atanorstring* kval = (Atanorstring*)globalAtanor->stringbuffererror;
	string todisplay;

	for (int i = 0; i < callfunc->Size(); i++) {
		res = callfunc->Evaluate(i, aNULL, idthread);
		if (globalAtanor->Error(idthread))
			return globalAtanor->Errorobject(idthread);
		todisplay += res->StringToDisplay();
	}

	Locking _lock(globalAtanor->_printlock);
	if (kval == NULL) {
#ifdef DOSOUTPUT
		todisplay = conversion2dos(todisplay);
#endif
		cerr << todisplay;
	}
	else
		kval->value += todisplay;

	return aTRUE;
}

Atanor* ProcPrinterrLN(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* res = aNULL;
	Atanorstring* kval = (Atanorstring*)globalAtanor->stringbuffererror;

	string todisplay;
	for (int i = 0; i < callfunc->Size(); i++) {
		if (i)
			todisplay += " ";
		res = callfunc->Evaluate(i, aNULL, idthread);
		if (globalAtanor->Error(idthread))
			return globalAtanor->Errorobject(idthread);
		todisplay += res->StringToDisplay();
	}

	todisplay += Endl;

	Locking _lock(globalAtanor->_printlock);
	if (kval == NULL) {
#ifdef DOSOUTPUT
		todisplay = conversion2dos(todisplay);
#endif
		cerr << todisplay;
	}
	else
		kval->value += todisplay;
	return aTRUE;
}


void toDisplayJN(string& todisplay, Atanor* context, AtanorCall* callfunc, short idthread) {
	//First the container...
	Atanor* kcont = callfunc->Evaluate(0, context, idthread);
	string sep = Endl;
	if (callfunc->Size() > 1)
		sep = callfunc->Evaluate(1, context, idthread)->String();

	if (kcont->isContainer() == false) {
		todisplay = kcont->String();
		todisplay += sep;
	}
	else {
		bool beg = true;
		if (kcont->isVectorContainer()) {
			int it;
			for (it = 0; it < kcont->Size(); it++) {
				if (beg == false)
					todisplay += sep;
				beg = false;
				todisplay += kcont->getstring(it);
			}
		}
		else {
			AtanorIteration* itr;
			if (kcont->isMapContainer() && callfunc->Size() == 3) {
				itr = kcont->Newiteration(false);
				string valsep = callfunc->Evaluate(2, context, idthread)->String();
				for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
					if (beg == false)
						todisplay += valsep;
					beg = false;
					todisplay += itr->Keystring();
					todisplay += sep;
					todisplay += itr->Valuestring();
				}
				itr->Release();
			}
			else {
				itr = kcont->Newiteration(false);
				for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
					if (beg == false)
						todisplay += sep;
					beg = false;
					todisplay += itr->Valuestring();
				}
				itr->Release();
			}
		}
	}
}

Atanor* ProcPrintJoin(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanorstring* kval = (Atanorstring*)globalAtanor->kstd;
	string todisplay;
#ifdef DOSOUTPUT
	bool convert = false;
	if (globalAtanor->os == &cout)
		convert = true;
#endif


	toDisplayJN(todisplay, contextualpattern, callfunc, idthread);


	Locking _lock(globalAtanor->_printlock);
	if (kval == NULL) {
		if (globalAtanor->doubledisplay) {
#ifdef DOSOUTPUT
			string sdos = conversion2dos(todisplay);
			cout << sdos;
#else
			cout << todisplay;
#endif
		}
		if (globalAtanor->displayfunction == NULL) {
#ifdef DOSOUTPUT
			if (convert) {
				string sdos = conversion2dos(todisplay);
				*globalAtanor->os << sdos;
			}
			else
				*globalAtanor->os << todisplay;
#else
			*globalAtanor->os << todisplay;
#endif
		}
		else
			globalAtanor->displayfunction(todisplay, globalAtanor->displayobject);
	}
	else
		kval->value += todisplay;

	return aTRUE;
}

Atanor* ProcPrintJoinErr(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanorstring* kval = (Atanorstring*)globalAtanor->kerr;
	string todisplay;

	toDisplayJN(todisplay, contextualpattern, callfunc, idthread);

	Locking _lock(globalAtanor->_printlock);
	if (kval == NULL) {
#ifdef DOSOUTPUT
		todisplay = conversion2dos(todisplay);
#endif
		cerr << todisplay;
	}
	else
		kval->value += todisplay;

	return aTRUE;
}

Atanor* ProcPrintJoinLN(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanorstring* kval = (Atanorstring*)globalAtanor->kstd;
	string todisplay;
#ifdef DOSOUTPUT
	bool convert = false;
	if (globalAtanor->os == &cout)
		convert = true;
#endif

	toDisplayJN(todisplay, contextualpattern, callfunc, idthread);

	todisplay += Endl;
	Locking _lock(globalAtanor->_printlock);
	if (kval == NULL) {
		if (globalAtanor->doubledisplay) {
#ifdef DOSOUTPUT
			string sdos = conversion2dos(todisplay);
			cout << sdos;
#else
			cout << todisplay;
#endif
		}
		if (globalAtanor->displayfunction == NULL) {
#ifdef DOSOUTPUT
			if (convert) {
				string sdos = conversion2dos(todisplay);
				*globalAtanor->os << sdos;
			}
			else
				*globalAtanor->os << todisplay;
#else
			*globalAtanor->os << todisplay;
#endif
		}
		else
			globalAtanor->displayfunction(todisplay, globalAtanor->displayobject);
	}
	else
		kval->value += todisplay;

	return aTRUE;
}

Atanor* ProcPrintJoinErrLN(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanorstring* kval = (Atanorstring*)globalAtanor->kerr;
	string todisplay;

	toDisplayJN(todisplay, contextualpattern, callfunc, idthread);

	todisplay += Endl;

	Locking _lock(globalAtanor->_printlock);
	if (kval == NULL) {
#ifdef DOSOUTPUT
		todisplay = conversion2dos(todisplay);
#endif
		cerr << todisplay;
	}
	else
		kval->value += todisplay;

	return aTRUE;
}

//___________________________________________________________________________________________________

Atanor* ProcRange(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	Atanor* first = callfunc->Evaluate(0, contextualpattern, idthread);
	//whenever a aNOELEMENT is returned, we can assume that a variable is missing to fullfil the range...
	if (first == aNOELEMENT)
		return aNOELEMENT;

	Atanor* second = callfunc->Evaluate(1, contextualpattern, idthread);
	if (second == aNOELEMENT)
		return aNOELEMENT;

	Atanor* kvect;
	Atanor* incr = aONE;

	if (callfunc->Size() == 3) {
		incr = callfunc->Evaluate(2, contextualpattern, idthread);
		if (incr == aNOELEMENT)
			return aNOELEMENT;
	}

	if (first->isInteger() && incr->isInteger()) {
		if (contextualpattern == aNULL)
			kvect = globalAtanor->Provideivector();
		else
			kvect = Selectavector(contextualpattern);

		if (kvect->Type() == a_lvector) {
			BLONG l = first->Long();
			BLONG r = second->Long();
			BLONG inc = incr->Long();

			BLONG d = (r - l) / inc;
			if (d<0)
				d *= -1;

			if (l>r && inc > 0)
				inc *= -1;

			if (d <= globalAtanor->maxrange) {
				if (inc == 0)
					return kvect;

				kvect->Reserve((long)d);

				if (inc > 0) {
					for (BLONG i = l; i <= r; i += inc)
						kvect->storevalue(i);
				}
				else {
					for (BLONG i = l; i >= r; i += inc)
						kvect->storevalue(i);
				}
				return kvect;
			}
			kvect->Release();
			return globalAtanor->Returnerror("RANGE over actual capacity", idthread);
		}

		long l = first->Integer();
		long r = second->Integer();
		long d = r - l + 1;
		long inc = incr->Integer();

		if (d<0)
			d *= -1;
		if (l>r && inc > 0)
			inc *= -1;

		if (d <= globalAtanor->maxrange) {
			if (inc == 0)
				return kvect;

			kvect->Reserve(d);

			if (inc > 0) {
				for (long i = l; i <= r; i += inc)
					kvect->storevalue(i);
			}
			else {
				for (long i = l; i >= r; i += inc)
					kvect->storevalue(i);
			}
			return kvect;
		}

		kvect->Release();
		return globalAtanor->Returnerror("RANGE over actual capacity", idthread);
	}

	if (first->isFloat() || incr->isFloat()) {
		if (contextualpattern == aNULL)
			kvect = globalAtanor->Providefvector();
		else
			kvect = Selectavector(contextualpattern);

		if (kvect->Type() == a_dvector) {
			float l = first->Decimal();
			float r = second->Decimal();
			float inc = incr->Decimal();

			float d = (r - l) / inc;
			if (d<0)
				d *= -1;

			if (l>r && inc > 0)
				inc *= -1;

			if (d <= globalAtanor->maxrange) {
				if (inc == 0)
					return kvect;

				kvect->Reserve((long)d);

				if (inc > 0) {
					for (float i = l; i <= r; i += inc)
						kvect->storevalue(i);
				}
				else {
					for (float i = l; i >= r; i += inc)
						kvect->storevalue(i);
				}
				return kvect;
			}
			kvect->Release();
			return globalAtanor->Returnerror("RANGE over actual capacity", idthread);
		}

		double l = first->Float();
		double r = second->Float();
		double inc = incr->Float();

		double d = (r - l) / inc;
		if (d<0)
			d *= -1;
		if (l>r && inc > 0)
			inc *= -1;

		if (d <= globalAtanor->maxrange) {
			if (inc == 0)
				return kvect;

			kvect->Reserve((long)d);
			if (inc > 0) {
				for (double i = l; i <= r; i += inc)
					kvect->storevalue(i);
			}
			else {
				for (double i = l; i >= r; i += inc)
					kvect->storevalue(i);
			}
			return kvect;
		}
		kvect->Release();
		return globalAtanor->Returnerror("RANGE over actual capacity", idthread);
	}

	if (first->Typevariable() == a_ustring) {
		wstring l = first->UString();
		wstring r = second->UString();
		if (l.size() != 1 || r.size() != 1)
			return globalAtanor->Returnerror("String should be one character long in RANGE", idthread);
		long inc = incr->Integer();

		char cl = l[0];
		char cr = r[0];
		int d = cr - cl;
		if (d<0)
			d *= -1;

		if (cl>cr && inc > 0)
			inc *= -1;

		kvect = Selectauvector(contextualpattern);
		if (inc == 0)
			return kvect;

		kvect->Reserve((long)d);
		if (inc > 0) {
			for (uchar i = cl; i <= cr; i += inc) {
				l = (char)i;
				kvect->storevalue(l);
			}
		}
		else {
			for (uchar i = cl; i >= cr; i += inc) {
				l = (char)i;
				kvect->storevalue(l);
			}
		}
		return kvect;
	}

	string l = first->String();
	string r = second->String();
	if (l.size() != 1 || r.size() != 1)
		return globalAtanor->Returnerror("String should be one character long in RANGE", idthread);
	long inc = incr->Integer();

	char cl = l[0];
	char cr = r[0];
	int d = cr - cl;
	if (d<0)
		d *= -1;

	if (cl>cr && inc > 0)
		inc *= -1;

	kvect = Selectasvector(contextualpattern);
	if (inc == 0)
		return kvect;

	kvect->Reserve((long)d);
	if (inc > 0) {
		for (uchar i = cl; i <= cr; i += inc) {
			l = (char)i;
			kvect->storevalue(l);
		}
	}
	else {
		for (uchar i = cl; i >= cr; i += inc) {
			l = (char)i;
			kvect->storevalue(l);
		}
	}
	return kvect;
}

Atanor* ProcOrd(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	wstring s = callfunc->Evaluate(0, contextualpattern, idthread)->UString();
	if (s.size() >= 1) {
		if (contextualpattern->isNumber())
			return globalAtanor->Provideint((int)s[0]);

		if (contextualpattern->isVectorContainer() || s.size() > 1) {
			Atanor* kvect = Selectaivector(contextualpattern);
			if (kvect->Type() == a_ivector) {
				Locking _lock((AtanorObject*)kvect);
				for (int i = 0; i < s.size(); i++)
					((Atanorivector*)kvect)->values.push_back((long)s[i]);
			}
			else {
				AtanorConstInt loc(0);
				for (int i = 0; i < s.size(); i++) {
					loc.value = (long)s[i];
					kvect->Push(&loc);
				}
			}
			return kvect;
		}

		return globalAtanor->Provideint((int)s[0]);
	}

	return aNULL;
}

//----------------------------------------------------------------------------------------------------------
Atanor* ProcLock(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//we create our lock
	if (globalAtanor->Error(idthread))
		return aFALSE;

	ThreadLock* ktl;
	Atanor* klock = callfunc->Evaluate(0, aNULL, idthread);
	string lock = klock->String();
	{
		Locking _lock(globalAtanor->_call);
		if (globalAtanor->locktables.find(lock) == globalAtanor->locktables.end()) {
			ktl = new ThreadLock(NULL, false, false);
			globalAtanor->locktables[lock] = ktl;
		}
		else
			ktl = globalAtanor->locktables[lock];
	}

	globalAtanor->threads[idthread].locks[lock] = ktl;
	ktl->Locking();
	ktl->idthread = idthread;
	return aTRUE;
}

Atanor* ProcUnlock(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//we create our lock
	if (globalAtanor->Error(idthread))
		return aFALSE;

	ThreadLock* ktl;
	Atanor* klock = callfunc->Evaluate(0, aNULL, idthread);
	string lock = klock->String();
	{
		Locking _lock(globalAtanor->_call);
		if (globalAtanor->locktables.find(lock) == globalAtanor->locktables.end())
			return globalAtanor->Returnerror("Unknown lock", idthread);

		ktl = globalAtanor->locktables[lock];
	}

	if (globalAtanor->threads[idthread].locks.find(lock) != globalAtanor->threads[idthread].locks.end())
		globalAtanor->threads[idthread].locks.erase(lock);

	ktl->idthread = 0;
	ktl->Unlocking();
	return aTRUE;
}

//----------------------------------------------------------------------------------------------------------
Atanor* ProcWaiton(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//we create our lock
	if (globalAtanor->Error(idthread))
		return aFALSE;

	LockedThread* var;
	Atanor* varlock = callfunc->Evaluate(0, aNULL, idthread);
	string lock = varlock->String();
	{
		Locking _lock(globalAtanor->_call);
		if (globalAtanor->waitstrings.find(lock) == globalAtanor->waitstrings.end()) {
			var = new LockedThread();
			globalAtanor->waitstrings[lock] = var;
		}
		else
			var = globalAtanor->waitstrings[lock];
	}

	var->Blocked();
	return aTRUE;
}

//----------------------------------------------------------------------------------------------------------
// We wait for a given variable to return FALSE as value for Protectedboolean...
Atanor* ProcWaitonfalse(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//we create our lock
	if (globalAtanor->Error(idthread))
		return aFALSE;

	LockedThread* var;
	Atanor* varlock = callfunc->Evaluate(0, aNULL, idthread);
	long id = globalAtanor->RecordInTrackerProtected(varlock);

	{
		Locking _lock(globalAtanor->_call);
		if (globalAtanor->threadvariables.find(id) == globalAtanor->threadvariables.end()) {
			var = new LockedThread();
			globalAtanor->threadvariables[id] = var;
		}
		else
			return aFALSE;
	}

	var->Blocked();
	return aTRUE;
}
//----------------------------------------------------------------------------------------------------------
Atanor* ProcCast(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//we create our lock
	if (globalAtanor->Error(idthread))
		return aFALSE;

	LockedThread* var;
	if (callfunc->Size() == 0) {
		Locking _lock(globalAtanor->_call);

		for (auto& v : globalAtanor->waitstrings)
			v.second->Released();

		return aTRUE;
	}

	Atanor* varlock = callfunc->Evaluate(0, aNULL, idthread);
	string lock = varlock->String();
	{
		Locking _lock(globalAtanor->_call);
		if (globalAtanor->waitstrings.find(lock) == globalAtanor->waitstrings.end())
			return globalAtanor->Returnerror("Unknown variable", idthread);

		var = globalAtanor->waitstrings[lock];
	}

	var->Released();
	return aTRUE;
}

//----------------------------------------------------------------------------------------------------------
Atanor* ProcKeyboardGet(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (callfunc->Size()) {
		string label = callfunc->Evaluate(0, aNULL, idthread)->String();
		cerr << label;
	}

	string s;
	getline(cin, s);
	return globalAtanor->Providestring(s);
}
//----------------------------------------------------------------------------------------------------------
Atanor* ProcRaise(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (globalAtanor->Error(idthread))
		return aFALSE;

	string resstr = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	return globalAtanor->Returnerror(resstr, idthread);
}
//----------------------------------------------------------------------------------------------------------
Atanor* ProcBase(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	static vector<wstring> caracs;
	static hash_bin<wchar_t, long> mcaracs;
	wstring w;
	long n, b = -1;

	Atanor* ke = callfunc->Evaluate(1, contextualpattern, idthread);
	//We can reset the chararacter list...
	if (ke->isVectorContainer()) {
		caracs.clear();
		mcaracs.clear();
		b = ke->Size();
		for (n = 0; n < b; n++) {
			w = ke->getustring(n);
			mcaracs[w[0]] = caracs.size();
			caracs.push_back(w);
		}
		if (b)
			return globalAtanor->Provideint(b);
	}

	if (caracs.size() == 0) {
		w = L"0";
		for (n = 0; n < 10; n++) {
			mcaracs[w[0]] = caracs.size();
			caracs.push_back(w);
			w[0]++;
		}
		w = L"A";
		for (n = 10; n < 36; n++) {
			mcaracs[w[0]] = caracs.size();
			caracs.push_back(w);
			w[0]++;
		}
		w = L"a";
		for (n = 36; n < 62; n++) {
			mcaracs[w[0]] = caracs.size();
			caracs.push_back(w);
			w[0]++;
		}
		w = L"#";
		mcaracs[w[0]] = caracs.size();
		caracs.push_back(w);
		w = L"@";
		mcaracs[w[0]] = caracs.size();
		caracs.push_back(w);
		w = L"";
		if (!b)
			return aTRUE;
	}

	b = ke->Integer();
	ke = callfunc->Evaluate(0, contextualpattern, idthread);

	if (b > caracs.size()) {
		char message[1024];
		sprintf_s(message, 1024, "KIF(510): base too large, max is: %d", caracs.size());
		return globalAtanor->Returnerror(message, idthread);
	}

	if (ke->isString()) {
		w = ke->UString();
		long v = 0;
		wchar_t wc;
		for (n = 0; n < w.size(); n++) {
			wc = w[n];
			if (mcaracs.check(wc) == false || mcaracs[wc] >= b)
				return globalAtanor->Returnerror("KIF(511): Cannot analyze this string in this base.", idthread);

			v *= b;
			v += mcaracs[wc];
		}
		return globalAtanor->Provideint(v);
	}

	n = ke->Integer();

	while (n) {
		w = caracs[n%b] + w;
		n /= b;
	}

	return globalAtanor->Provideustring(w);
}

//----------------------------------------------------------------------------------------------------------
//<distance(L1,l1,L2,l2) = r | let a=L1.radian(), let b=L2.radian(), let c=l1.radian(), let d=l2.radian(), let r= acos(cos(a)*cos(b)*cos(abs(c-d))+sin(a)*sin(b))*6371>

static inline double Radian(double num) {
	return(M_PI*(num / 180));
}

Atanor* ProcGPSDistance(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	double a, b, c, d;
	double R = 6371;

	a = Radian(callfunc->Evaluate(0, contextualpattern, idthread)->Float());
	c = Radian(callfunc->Evaluate(1, contextualpattern, idthread)->Float());
	b = Radian(callfunc->Evaluate(2, contextualpattern, idthread)->Float());
	d = Radian(callfunc->Evaluate(3, contextualpattern, idthread)->Float());

	if (callfunc->Size() == 5)
		R = callfunc->Evaluate(4, contextualpattern, idthread)->Float();

	R = acos(cos(a)*cos(b)*cos(abs(c - d)) + sin(a)*sin(b)) * R;
	return globalAtanor->Providefloat(R);
}


Atanor* ProcMath(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* av = callfunc->Evaluate(0, contextualpattern, idthread);
	double v;

	if (av->isContainer()) {
		short n = callfunc->Name();
		AtanorIteration* it = av->Newiteration(false);
		av = av->Newinstance(idthread);
		for (it->Begin(); !it->End()->Boolean(); it->Next()) {
			v = globalAtanor->mathFunctions[n](it->Valuefloat());
			av->storevalue(v);
		}
		it->Release();
		return av;
	}

	v = av->Float();
	return globalAtanor->Providefloat(globalAtanor->mathFunctions[callfunc->Name()](v));
}

//----------------------------------------------------------------------------------------------------------
Atanor* CommonJSon(Atanor* object, short idthread, AtanorCall* callfunc) {
	return globalAtanor->Providestring(object->JSonString());
}

Atanor* CommonString(Atanor* object, short idthread, AtanorCall* callfunc) {
	return globalAtanor->Providestring(object->String());
}

Atanor* CommonUString(Atanor* object, short idthread, AtanorCall* callfunc) {
	return globalAtanor->Provideustring(object->UString());
}

Atanor* CommonInt(Atanor* object, short idthread, AtanorCall* callfunc) {
	return globalAtanor->Provideint(object->Integer());
}

Atanor* CommonFloat(Atanor* object, short idthread, AtanorCall* callfunc) {
	return globalAtanor->Providefloat(object->Float());
}

Atanor* CommonShort(Atanor* object, short idthread, AtanorCall* callfunc) {
	return new Atanorshort(object->Short());
}

Atanor* CommonLong(Atanor* object, short idthread, AtanorCall* callfunc) {
	return new Atanorlong(object->Long());
}

Atanor* CommonDecimal(Atanor* object, short idthread, AtanorCall* callfunc) {
	return new Atanordecimal(object->Decimal());
}


Atanor* CommonVector(Atanor* object, short idthread, AtanorCall* callfunc) {
	return object->Vector(idthread);
}

Atanor* CommonMap(Atanor* object, short idthread, AtanorCall* callfunc) {
	return object->Map(idthread);
}

Atanor* CommonSize(Atanor* object, short idthread, AtanorCall* callfunc) {
	return globalAtanor->Provideint(object->Size());
}

Atanor* CommonType(Atanor* object, short idthread, AtanorCall* callfunc) {
	return globalAtanor->Providestring(globalAtanor->Getsymbol(object->Type()));
}

Atanor* CommonISA(Atanor* object, short idthread, AtanorCall* callfunc) {
	string ty = callfunc->Evaluate(0, aNULL, idthread)->String();
	string cp = globalAtanor->Getsymbol(object->Type());
	if (ty == cp)
		return aTRUE;
	return aFALSE;
}

Atanor* CommonIsstring(Atanor* object, short idthread, AtanorCall* callfunc) {
	if (object->isString())
		return aTRUE;
	return aFALSE;
}

Atanor* CommonIscontainer(Atanor* object, short idthread, AtanorCall* callfunc) {
	if (object->isContainer())
		return aTRUE;
	return aFALSE;
}

Atanor* CommonIsnumber(Atanor* object, short idthread, AtanorCall* callfunc) {
	if (object->isNumber())
		return aTRUE;
	return aFALSE;
}

Atanor* CommonIsvector(Atanor* object, short idthread, AtanorCall* callfunc) {
	if (object->isVectorContainer())
		return aTRUE;
	return aFALSE;
}

Atanor* CommonIsmap(Atanor* object, short idthread, AtanorCall* callfunc) {
	if (object->isMapContainer())
		return aTRUE;
	return aFALSE;
}


Atanor* CommonInfo(Atanor* object, short idthread, AtanorCall* callfunc) {
	string s = callfunc->Evaluate(0, aNULL, idthread)->String();
	s = object->Info(s);
	return globalAtanor->Providestring(s);
}

Atanor* CommonMethods(Atanor* object, short idthread, AtanorCall* callfunc) {
	Atanorsvector* vs = globalAtanor->Providesvector();
	object->Methods(vs);
	vs->Sort(false);
	return vs;
}

Atanor* CommonExtension(Atanor* object, short idthread, AtanorCall* callfunc) {
	//First we need to find the right function from the right frame...
	short name = callfunc->Name();
	short t = object->Type();
	AtanorFrame* frame = globalAtanor->extensions[t];

	//We have two cases of error
	if (frame == NULL) {
		//One no extension exists for this type, however, the variable might have been accessed through a self or a let variable
		//we check if the Extension was attributed by error...
		//The "Execute" here has been implemented to call the AtanorCallMethod::Get...
		if (globalAtanor->methods.check(t) && Arity(globalAtanor->methods[t][name], callfunc->Size()))
			return callfunc->Execute(aNULL, object, idthread);

		stringstream message;
		message << "No 'extension' exists for a '" << globalAtanor->Getsymbol(t) << "' object";
		return globalAtanor->Returnerror(message.str(), idthread);
	}

	if (frame->Declaration(name) == NULL) {
		//Second an extension exists for this type, however, the variable might have been accessed through a self or a let variable
		//we check if actually there is a method with that name, which is part of the object itself...
		//Again, the CommonExtension could have been attributed by error...
		//The "Execute" here has been implemented to call the AtanorCallMethod::Get...
		if (globalAtanor->methods.check(t) && Arity(globalAtanor->methods[t][name], callfunc->Size()))
			return callfunc->Execute(aNULL, object, idthread);

		stringstream message;
		message << "The method '" << globalAtanor->Getsymbol(name)
			<< "' does not exist for a '" << globalAtanor->Getsymbol(t) << "' object";
		return globalAtanor->Returnerror(message.str(), idthread);
	}

	//We create a temporary place for THE variable
	AtanorDeclaration environment;

	//Then we intialize this variable with object
	environment.Declare(frame->Name(), object);
	environment.Declare(a_this, object);

	AtanorCallFunction call(frame->Declaration(name));
	call.arguments = callfunc->arguments;

	globalAtanor->Pushstack(&environment, idthread);
	object = call.Get(&environment, aNULL, idthread);
	globalAtanor->Popstack(idthread);

	//We have returned our value in object, which we protect...
	return object;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------

Exporting void AtanorGlobal::RecordOneProcedure(string name, AtanorProcedure p, unsigned long arity, short retype) {
	short idname = Getid(name);
	procedures[idname] = p;
	arities[idname] = arity;

	if (retype != a_null)
		returntypes[idname] = retype;
}

Exporting void AtanorGlobal::RecordCommon(string name, AtanorProcedure p, unsigned long arity, short retype) {
	short idname = Getid(name);
	commons[idname] = p;
	arities[idname] = arity;
	if (allmethods.find(idname) == allmethods.end())
		allmethods[idname] = arity;
	else
		allmethods[idname] |= arity;

	if (retype != a_null)
		returntypes[idname] = retype;
}

Exporting void AtanorGlobal::RecordMethods(short type, bin_hash<unsigned long>& exported) {
	bin_hash<unsigned long>::iterator it;
	for (it = exported.begin(); it != exported.end(); it++) {
		methods[type][it->first] = it->second;
		if (allmethods.check(it->first))
			allmethods[it->first] |= it->second;
		else
			allmethods[it->first] = it->second;
	}

	compatibilities[type][type] = true;
	strictcompatibilities[type][type] = true;

	compatibilities[a_call][type] = true;
	compatibilities[a_universal][type] = true;
	compatibilities[a_let][type] = true;
	compatibilities[a_self][type] = true;

	strictcompatibilities[a_call][type] = true;
	strictcompatibilities[a_universal][type] = true;
	strictcompatibilities[a_let][type] = true;
	strictcompatibilities[a_self][type] = true;
}

Exporting void AtanorGlobal::RecordMethods(short type, short name, unsigned long arity) {
	methods[type][name] = arity;
	if (allmethods.check(name))
		allmethods[name] |= arity;
	else
		allmethods[name] = arity;
	RecordCommon(Getsymbol(name), CommonExtension, arity);
	extensionmethods[name] = true;

	compatibilities[type][type] = true;
	strictcompatibilities[type][type] = true;

	compatibilities[a_call][type] = true;
	compatibilities[a_universal][type] = true;
	compatibilities[a_let][type] = true;
	compatibilities[a_self][type] = true;

	strictcompatibilities[a_call][type] = true;
	strictcompatibilities[a_universal][type] = true;
	strictcompatibilities[a_let][type] = true;
	strictcompatibilities[a_self][type] = true;
}

Exporting void AtanorGlobal::RecordProcedures() {
	systemfunctions["_erroronkey"] = true;
	systemfunctions["_setstacksize"] = true;
	systemfunctions["_setthreadlimit"] = true;
	systemfunctions["_setjoinedlimit"] = true;
	systemfunctions["_setenv"] = true;
	systemfunctions["_setmaxrange"] = true;
	systemfunctions["use"] = true;
	systemfunctions["loadin"] = true;
	systemfunctions["_setvalidfeatures"] = true;

	RecordOneProcedure("_setstacksize", ProcStackSize, P_ONE);
	RecordOneProcedure("_setmaxthreads", ProcMaxThreads, P_ONE);
	RecordOneProcedure("_setmaxjoined", ProcMaxJoined, P_ONE);
	RecordOneProcedure("_setmaxrange", ProcMaxMaxRange, P_ONE);
	RecordOneProcedure("_exit", ProcExit, P_NONE);
	RecordOneProcedure("_erroronkey", ProcErrorOnKey, P_ONE);
	RecordOneProcedure("_setenv", ProcSetEnv, P_TWO);
	RecordOneProcedure("_forcelocks", ProcForceLocks, P_ONE);

	RecordOneProcedure("_setvalidfeatures", ProcSetValidFeatures, P_ONE);

	RecordOneProcedure("_breakpoint", ProcBreakPoint, P_ONE);

	RecordOneProcedure("use", ProcUse, P_ONE);
	RecordOneProcedure("loadin", ProcLoadin, P_ONE);

	RecordOneProcedure("allobjects", ProcAllObjects, P_NONE);

	RecordOneProcedure("version", ProcVersion, P_NONE);
	RecordOneProcedure("_mirrordisplay", ProcMirrorDisplay, P_NONE);

	RecordOneProcedure("emojis", ProcEmojis, P_NONE);

	RecordOneProcedure("print", ProcPrint, P_FULL);
	RecordOneProcedure("println", ProcPrintLN, P_FULL);
	RecordOneProcedure("printerr", ProcPrinterr, P_FULL);
	RecordOneProcedure("printlnerr", ProcPrinterrLN, P_FULL);
	RecordOneProcedure("printj", &ProcPrintJoin, P_ONE | P_TWO | P_THREE);
	RecordOneProcedure("printjerr", &ProcPrintJoinErr, P_ONE | P_TWO | P_THREE);
	RecordOneProcedure("printjln", &ProcPrintJoinLN, P_ONE | P_TWO | P_THREE);
	RecordOneProcedure("printjlnerr", &ProcPrintJoinErrLN, P_ONE | P_TWO | P_THREE);

	RecordOneProcedure("_eval", ProcEval, P_ONE);
	RecordOneProcedure("_evalfunction", ProcEvalFunction, P_ONE);

	RecordOneProcedure("abs", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("acos", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("acosh", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("asin", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("asinh", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("atan", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("atanh", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("cbrt", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("∛", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("cos", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("cosh", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("erf", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("erfc", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("exp", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("exp2", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("expm1", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("floor", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("lgamma", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("ln", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("log", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("log1p", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("log2", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("logb", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("nearbyint", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("rint", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("round", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("sin", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("sinh", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("sqrt", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("√", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("tan", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("tanh", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("tgamma", &ProcMath, P_ONE, a_float);
	RecordOneProcedure("trunc", &ProcMath, P_ONE, a_float);

	RecordOneProcedure("∑", &ProcSum, P_ONE | P_TWO | P_THREE);
	RecordOneProcedure("∏", &ProcProduct, P_ONE | P_TWO | P_THREE);

	RecordOneProcedure("sum", &ProcSum, P_ONE | P_TWO | P_THREE);
	RecordOneProcedure("product", &ProcProduct, P_ONE | P_TWO | P_THREE);

	//Error management...
	RecordOneProcedure("catch", ProcCatch, P_ONE);
	RecordOneProcedure("raise", ProcRaise, P_ONE);

	RecordOneProcedure("GPSdistance", &ProcGPSDistance, P_FOUR | P_FIVE);

	//Range including interval definition
	RecordOneProcedure("range", ProcRange, P_TWO | P_THREE);
	RecordOneProcedure("ord", ProcOrd, P_ONE); //this specific one is used with range...

	RecordOneProcedure("random", ProcRandom, P_NONE | P_ONE, a_float);

	RecordOneProcedure("pause", ProcPause, P_ONE);

	RecordOneProcedure("kget", ProcKeyboardGet, P_NONE | P_ONE);

	RecordOneProcedure("redirectoutput", ProcRedictectOutput, P_TWO);
	RecordOneProcedure("restateoutput", ProcRestateOutput, P_TWO);

	RecordOneProcedure("max", ProcMax, P_ATLEASTTWO);
	RecordOneProcedure("min", ProcMin, P_ATLEASTTWO);

	RecordOneProcedure("base", ProcBase, P_TWO);

	//Threading
	RecordOneProcedure("_threadhandle", ProcThreadhandle, P_NONE);
	RecordOneProcedure("_threadid", ProcThreadid, P_NONE);
	RecordOneProcedure("waitonjoined", ProcWaitOnJoin, P_NONE | P_ONE | P_TWO);
	RecordOneProcedure("lock", ProcLock, P_ONE);
	RecordOneProcedure("unlock", ProcUnlock, P_ONE);
	RecordOneProcedure("wait", ProcWaiton, P_ONE);
	RecordOneProcedure("waitonfalse", ProcWaitonfalse, P_ONE);
	RecordOneProcedure("cast", ProcCast, P_NONE | P_ONE);
	RecordOneProcedure("sleep", ProcSleep, P_ONE);


	//Common methods to all objects...
	RecordCommon("json", CommonJSon, P_NONE, a_string);
	RecordCommon("string", CommonString, P_NONE, a_string);
	RecordCommon("ustring", CommonUString, P_NONE, a_ustring);
	RecordCommon("int", CommonInt, P_NONE, a_int);
	RecordCommon("float", CommonFloat, P_NONE, a_float);
	RecordCommon("long", CommonLong, P_NONE, a_long);
	RecordCommon("decimal", CommonDecimal, P_NONE, a_decimal);
	RecordCommon("short", CommonShort, P_NONE, a_short);
	RecordCommon("vector", CommonVector, P_NONE);
	RecordCommon("map", CommonMap, P_NONE);

	RecordCommon("size", CommonSize, P_NONE, a_int);
	RecordCommon("info", CommonInfo, P_ONE, a_string);
	RecordCommon("methods", CommonMethods, P_NONE);
	RecordCommon("type", CommonType, P_NONE, a_string);
	RecordCommon("isa", CommonISA, P_ONE);

	RecordCommon("isstring", CommonIsstring, P_NONE);
	RecordCommon("isvector", CommonIsvector, P_NONE);
	RecordCommon("ismap", CommonIsmap, P_NONE);
	RecordCommon("iscontainer", CommonIscontainer, P_NONE);
	RecordCommon("isnumber", CommonIsnumber, P_NONE);

}



