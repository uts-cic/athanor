
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
filename   : atanortransducer.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanortransducer.h"
#include "atoms.h"
#include "atanormapss.h"
#include "instructions.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, transducerMethod>  Atanortransducer::methods;
Exporting hmap<string, string> Atanortransducer::infomethods;
Exporting bin_hash<unsigned long> Atanortransducer::exported;

Exporting short Atanortransducer::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanortransducer::AddMethod(AtanorGlobal* global, string name, transducerMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanortransducer::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanortransducer::idtype = global->Getid("transducer");

	Atanortransducer::AddMethod(global, "_initial", &Atanortransducer::MethodInitial, P_ONE, "_initial(file): load an automaton file.");

	Atanortransducer::AddMethod(global, "load", &Atanortransducer::MethodLoad, P_ONE, "load(filename): load a transducer");
	Atanortransducer::AddMethod(global, "add", &Atanortransducer::MethodAdd, P_ONE | P_TWO | P_THREE, "add(string|map, bool normalize, int latintable): add a string or a mapss to the automaton");
	Atanortransducer::AddMethod(global, "build", &Atanortransducer::MethodBuild, P_ONE | P_TWO | P_THREE | P_FOUR, "buld(input,output, norm, latintable): Build a transducer file out of a text file containing on the first line surface form, then on next line lemma+features.");
	Atanortransducer::AddMethod(global, "store", &Atanortransducer::MethodStore, P_ONE | P_TWO | P_THREE, "store(name, norm, latintable): store an automaton.");
	Atanortransducer::AddMethod(global, "compilergx", &Atanortransducer::MethodCompilergx, P_TWO | P_THREE, "compilergx(expression, vector, name): Compile a regular expression combined with a vector and store it with name.");
	Atanortransducer::AddMethod(global, "lookup", &Atanortransducer::MethodLookup, P_THREE | P_ONE, "lookup(word, threshold, flags): lookup for a word with a threshold and flags");
	Atanortransducer::AddMethod(global, "lookdown", &Atanortransducer::MethodLookdown, P_ONE, "lookdown(word_pos_feat): lookdow for the surface form matching a word+pos+features");
	Atanortransducer::AddMethod(global, "process", &Atanortransducer::MethodProcess, P_ONE, "process(word): Return the word readings against the automaton");


	global->newInstance[Atanortransducer::idtype] = new Atanortransducer(global);
	global->RecordMethods(Atanortransducer::idtype,Atanortransducer::exported);

	Atanor* a = new AtanorSystemVariable(global, new AtanorConstInt(1), global->Createid("a_first"), a_int);
	a = new AtanorSystemVariable(global, new AtanorConstInt(2), global->Createid("a_change"), a_int);
	a = new AtanorSystemVariable(global, new AtanorConstInt(4), global->Createid("a_delete"), a_int);
	a = new AtanorSystemVariable(global, new AtanorConstInt(8), global->Createid("a_insert"), a_int);
	a = new AtanorSystemVariable(global, new AtanorConstInt(16), global->Createid("a_switch"), a_int);
	a = new AtanorSystemVariable(global, new AtanorConstInt(32), global->Createid("a_nocase"), a_int);

	return true;
}

Atanor* Atanortransducer::Put(Atanor* index, Atanor* value, short idthread) {
	return this;
}

Atanor* Atanortransducer::Get(Atanor* context, Atanor* index, short idthread) {
	return this;
}

Atanor* Atanortransducer::MethodInitial(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	name = callfunc->Evaluate(0, contextualpattern, idthread)->String();

	automaton = new AtanorAutomaton;
	if (!automaton->load(name))
		return globalAtanor->Returnerror("XTR(223): Cannot load the file", idthread);

	automaton->fillencoding(false);
	automaton->start.shuffle();
	return aTRUE;
}


Atanor* Atanortransducer::MethodStore(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	if (automaton == NULL)
		return aFALSE;

	string name = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	if (callfunc->Size() >= 2) {
		automaton->normalize = callfunc->Evaluate(1, contextualpattern, idthread)->Boolean();
		if (callfunc->Size() == 3)
			automaton->encoding_table = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
	}

	if (automaton->store(name))
		return aTRUE;

	return aFALSE;

}

Atanor* Atanortransducer::MethodLoad(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	Atanor* filename = callfunc->Evaluate(0, contextualpattern, idthread);
	if (automaton != NULL)
		delete automaton;
	automaton = new AtanorAutomaton;
	string sname = filename->String();
	if (!automaton->load(sname))
		return aFALSE;

	automaton->fillencoding(false);
	automaton->start.shuffle();
	return aTRUE;
}

Atanor* Atanortransducer::MethodAdd(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		if (automaton == NULL)
			automaton = new AtanorAutomaton;

		Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);

		if (callfunc->Size() >= 2) {
			automaton->normalize = callfunc->Evaluate(1, contextualpattern, idthread)->Boolean();
			if (callfunc->Size() == 3)
				automaton->encoding_table = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
		}

		if (ke->Type() == Atanormapss::idtype)
			automaton->addmap(((Atanormapss*)ke)->values);
		else {
			hmap<string, string> values;
			if (ke->isVectorContainer()) {
				if ((ke->Size() % 2) != 0)
					return globalAtanor->Returnerror("XTR(010): Expecting a vector with an even number of values.", idthread);
				string s;
				string l;
				for (int i = 0; i < ke->Size(); i += 2) {
					s = ke->getstring(i);
					l = ke->getstring(i + 1);
					values[s] = l;
				}

			}
			else {
				if (ke->isMapContainer()) {
					AtanorIteration* itr = ke->Newiteration(false);
					for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
						values[itr->Keystring()] = itr->Valuestring();
					}
					itr->Release();
				}
			}
			automaton->addmap(values);
		}

		return aTRUE;
}

Atanor* Atanortransducer::MethodBuild(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	string input = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	string output = callfunc->Evaluate(1, contextualpattern, idthread)->String();
	bool norm = callfunc->Evaluate(2, contextualpattern, idthread)->Boolean();
	int latinencoding = 1;
	if (callfunc->Size() == 4)
		latinencoding = callfunc->Evaluate(3, contextualpattern, idthread)->Integer();

	if (automaton != NULL)
		delete automaton;

	automaton = new AtanorAutomaton();
	if (compileAutomaton(*automaton, input, output, latinencoding, norm))
		return aTRUE;

	return aFALSE;
}

Atanor* Atanortransducer::MethodProcess(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (automaton == NULL)
		return aNULL;
	Atanor* kvect = Selectavector(contextualpattern);
	string words = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	charReadString currentreader(words);
	currentreader.addoffsets = true;
	currentreader.begin();
	Atanorsvector* kvs = globalAtanor->Providesvector();
	while (!currentreader.end()) {
		if (automaton->process(currentreader, kvs->values, false)) {
			kvect->Push(kvs);
			kvs = globalAtanor->Providesvector();
		}
	}
	kvs->Release();
	return kvect;
}

Atanor* Atanortransducer::MethodLookup(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (automaton == NULL)
		return aNULL;

	wstring word = callfunc->Evaluate(0, contextualpattern, idthread)->UString();
	int threshold = 0;
	short flags = 0;
	if (callfunc->Size() == 3) {
		threshold = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
		flags = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
	}

	if (contextualpattern->isContainer()) {
		Atanor* kvs = Selectasvector(contextualpattern);

		if (kvs->Type() == a_svector) {
			automaton->up(word, ((Atanorsvector*)kvs)->values, threshold, flags);
		}
		else {
			vector<string> readings;
			automaton->up(word, readings, threshold, flags);
			for (int i = 0; i < readings.size(); i++)
				kvs->storevalue(readings[i]);
		}
		return kvs;
	}

	vector<string> readings;
	automaton->up(word, readings, threshold, flags);

	if (readings.size())
		return globalAtanor->Providestring(readings[0]);
	return aNULL;
}

Atanor* Atanortransducer::MethodLookdown(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	if (automaton == NULL)
		return aNULL;

	wstring word = callfunc->Evaluate(0, contextualpattern, idthread)->UString();

	if (contextualpattern->isContainer()) {
		Atanor* kvs = Selectasvector(contextualpattern);

		if (kvs->Type() == a_svector)
			automaton->down(word, ((Atanorsvector*)kvs)->values);
		else {
			vector<string> readings;
			automaton->down(word, readings);
			for (int i = 0; i < readings.size(); i++)
				kvs->storevalue(readings[i]);
		}
		return kvs;
	}

	vector<string> readings;
	automaton->down(word, readings);

	if (readings.size())
		return globalAtanor->Providestring(readings[0]);
	return aNULL;
}

Atanor* Atanortransducer::MethodCompilergx(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring rgx(callfunc->Evaluate(0, contextualpattern, idthread)->String());
	Atanor* kfeat = callfunc->Evaluate(1, contextualpattern, idthread);
	if (!kfeat->isVectorContainer())
		return globalAtanor->Returnerror("XTR(001): We expect the second argument to be a vector.", idthread);

	if (automaton == NULL)
		automaton = new AtanorAutomaton();

	//we first transform each of our features into indexes...
	vector<unsigned long> indexes;
	string s;
	for (int i = 0; i < kfeat->Size(); i++) {
		s = kfeat->getstring(i);
		if (s[0] != '\t')
			s = "\t" + s;
		indexes.push_back(automaton->index(s));
	}

	if (!automaton->start.parse(*automaton, rgx, indexes))
		return globalAtanor->Returnerror("XTR(002): Wrong regular expression", idthread);

	if (callfunc->Size() == 3) {
		string name = callfunc->Evaluate(2, contextualpattern, idthread)->String();
		automaton->store(name);
	}

	return aTRUE;
}





