
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
filename   : atanorbitmap.cxx
Date       : 2017/04/01
Purpose    : bitmap implementation (based on FLTK)
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorbitmap.h"

//We need to declare once again our local definitions.
hmap<short, bitmapMethod>  Atanorbitmap::methods;
hmap<string, string> Atanorbitmap::infomethods;
bin_hash<unsigned long> Atanorbitmap::exported;

short Atanorbitmap::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorbitmap::AddMethod(AtanorGlobal* global, string name, bitmapMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorbitmap::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorbitmap::idtype = global->Getid("bitmap");

	Atanorbitmap::AddMethod(global, "_initial", &Atanorbitmap::MethodInitial, P_THREE, "_initial(ivector,int l,int h): Loading of a bitmap from a vector. 'l' is the lenght, 'h' the hight.");
	Atanorbitmap::AddMethod(global, "load", &Atanorbitmap::MethodLoad, P_THREE, "load(vector bitmaps,int w,int h): Load a bitmap");


	global->newInstance[Atanorbitmap::idtype] = new Atanorbitmap(NULL, global);
	global->RecordMethods(Atanorbitmap::idtype,Atanorbitmap::exported);

	return true;
}





