
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
filename   : atanorfraction.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorfraction.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, fractionMethod>  Atanorfraction::methods;
Exporting hmap<string, string> Atanorfraction::infomethods;
Exporting bin_hash<unsigned long> Atanorfraction::exported;

Exporting short Atanorfraction::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorfraction::AddMethod(AtanorGlobal* global, string name, fractionMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorfraction::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorfraction::idtype = global->Getid("fraction");

	Atanorfraction::AddMethod(global, "_initial", &Atanorfraction::MethodInitial, P_ONE | P_TWO, "_initial(n,d): initialize a fraction");
	Atanorfraction::AddMethod(global, "nd", &Atanorfraction::MethodInitial, P_ONE | P_TWO, "nd(n,d): initialize a fraction");
	Atanorfraction::AddMethod(global, "simplify", &Atanorfraction::MethodSimplify, P_NONE, "simplify(): simplify a fraction");

	global->newInstance[Atanorfraction::idtype] = new Atanorfraction(global);
	global->RecordMethods(Atanorfraction::idtype,Atanorfraction::exported);

	return true;
}





