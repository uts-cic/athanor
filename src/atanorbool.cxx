
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
filename   : atanorbool.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorbool.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, boolMethod>  Atanorbool::methods;
Exporting hmap<string, string> Atanorbool::infomethods;
Exporting bin_hash<unsigned long> Atanorbool::exported;

Exporting short Atanorbool::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorbool::AddMethod(AtanorGlobal* global, string name, boolMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;	
}

bool Atanorbool::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorbool::idtype = global->Getid("bool");

	//Atanorbool::AddMethod(global, "size", &Atanorbool::MethodSize, P_NONE, "size(): Return the size of the string.");



	global->newInstance[Atanorbool::idtype] = new Atanorbool(0, global);
	global->RecordMethods(Atanorbool::idtype,Atanorbool::exported);

	return true;
}








