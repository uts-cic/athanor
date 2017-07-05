
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
filename   : atanoriterator.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanoriterator.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, iteratorMethod>  Atanoriterator::methods;
Exporting hmap<string, string> Atanoriterator::infomethods;
Exporting bin_hash<unsigned long> Atanoriterator::exported;

Exporting short Atanoriterator::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanoriterator::AddMethod(AtanorGlobal* global, string name, iteratorMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanoriterator::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanoriterator::idtype = global->Getid("iterator");

	Atanoriterator::AddMethod(global, "begin", &Atanoriterator::MethodBegin, P_NONE, "begin(): Start the iteration.");
	Atanoriterator::AddMethod(global, "key", &Atanoriterator::MethodKey, P_NONE, "key(): Return current key.");
	Atanoriterator::AddMethod(global, "value", &Atanoriterator::MethodValue, P_NONE, "value(): Return the current value.");
	Atanoriterator::AddMethod(global, "next", &Atanoriterator::MethodNext, P_NONE, "next(): next iteration.");
	Atanoriterator::AddMethod(global, "end", &Atanoriterator::MethodEnd, P_NONE, "end(): return true when the iteration is over.");
	
	global->newInstance[Atanoriterator::idtype] = new Atanoriterator(global);
	global->RecordMethods(Atanoriterator::idtype,Atanoriterator::exported);

	return true;
}








