
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
filename   : atanorimage.cxx
Date       : 2017/04/01
Purpose    : Image display (based on FLTK)
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorfltk.h"
#include "atanorimage.h"

//We need to declare once again our local definitions.
hmap<short, imageMethod>  Atanorimage::methods;
hmap<string, string> Atanorimage::infomethods;
bin_hash<unsigned long> Atanorimage::exported;

short Atanorimage::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorimage::AddMethod(AtanorGlobal* global, string name, imageMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorimage::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorimage::idtype = global->Getid("image");

	Atanorimage::AddMethod(global, "loadgif", &Atanorimage::MethodLoadImageGIF, P_ONE, "loadgif(string filename): Load a GIF image");

#ifndef FLTKNOJPEG
	Atanorimage::AddMethod(global, "loadjpeg", &Atanorimage::MethodLoadImageJPEG, P_ONE, "loadjpeg(string filename): Load a JPEG image");
#endif



	global->newInstance[Atanorimage::idtype] = new Atanorimage(global);
	global->RecordMethods(Atanorimage::idtype,Atanorimage::exported);

	return true;
}


Atanor* Atanorimage::Put(Atanor* index, Atanor* value, short idthread) {
	return this;
}

Atanor* Atanorimage::Get(Atanor* context, Atanor* index, short idthread) {
	return this;
}

AtanorIteration* Atanorimage:: Newiteration(bool direction) {
	return aITERNULL;
}





