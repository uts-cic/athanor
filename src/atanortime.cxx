
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
filename   : atanortime.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanortime.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, timeMethod>  Atanortime::methods;
Exporting hmap<string, string> Atanortime::infomethods;
Exporting bin_hash<unsigned long> Atanortime::exported;

Exporting short Atanortime::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanortime::AddMethod(AtanorGlobal* global, string name, timeMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanortime::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanortime::idtype = global->Getid("time");

	Atanortime::AddMethod(global, "reset", &Atanortime::MethodReset, P_NONE, "reset(): reset the time");

	global->newInstance[Atanortime::idtype] = new Atanortime(global);
	global->RecordMethods(Atanortime::idtype,Atanortime::exported);

	return true;
}



Atanor* Atanortime::Put(Atanor* index, Atanor* value, short idthread) {
	return this;
}

Atanor* Atanortime::Get(Atanor* context, Atanor* index, short idthread) {
	return this;
}

string Atanortime::String() {
	char buffer[100];
	double timeval = Float() / 1000000UL;
	sprintf_s(buffer, 100, "%fs", timeval);
	return buffer;
}


Atanor* Atanortime::MethodReset(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a time
	gettimeofday(&value, NULL);
	return this;
}





