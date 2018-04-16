
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
filename   : atanorstdin.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorversion.h"
#include "atanorstdin.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, stdinMethod>  Atanorstdin::methods;
Exporting hmap<string, string> Atanorstdin::infomethods;
Exporting bin_hash<unsigned long> Atanorstdin::exported;

Exporting short Atanorstdin::idtype = 0;

//------------------------------------------------------------------------------------------------------------------

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorstdin::AddMethod(AtanorGlobal* global, string name, stdinMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorstdin::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorstdin::idtype = global->Getid("stdin");

	Atanorstdin::AddMethod(global, "_initial", &Atanorstdin::MethodInitial, P_NONE | P_ONE | P_TWO, "_initial(name,op): load a file in read/write/append mode. Default is read.");
	Atanorstdin::AddMethod(global, "get", &Atanorstdin::MethodGet, P_NONE, "get(): get a character.");
	Atanorstdin::AddMethod(global, "eof", &Atanorstdin::MethodEof, P_NONE, "eof(): return true, if the end has been reached.");

	global->newInstance[Atanorstdin::idtype] = new Atanorstdin(global);
	global->RecordMethods(Atanorstdin::idtype,Atanorstdin::exported);
	
	return true;
}



Atanor* Atanorstdin::Put(Atanor* idx, Atanor* kval, short idthread) {
	return aTRUE;
}

Atanor* Atanorstdin::Get(Atanor* context, Atanor* idx, short idthread) {
	return this;
}



