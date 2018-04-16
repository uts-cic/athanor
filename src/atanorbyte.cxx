
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
filename   : atanorbyte.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorbyte.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, byteMethod>  Atanorbyte::methods;
Exporting hmap<string, string> Atanorbyte::infomethods;
Exporting bin_hash<unsigned long> Atanorbyte::exported;

Exporting short Atanorbyte::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorbyte::AddMethod(AtanorGlobal* global, string name, byteMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorbyte::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorbyte::idtype = global->Getid("byte");

	Atanorbyte::AddMethod(global, "succ", &Atanorbyte::MethodSucc, P_NONE, "succ(): Return the successor of a byte.");
	Atanorbyte::AddMethod(global, "pred", &Atanorbyte::MethodPred, P_NONE, "pred(): Return the predecessor of a byte.");
	

	global->newInstance[Atanorbyte::idtype] = new Atanorbyte(0, global);
	global->RecordMethods(Atanorbyte::idtype,Atanorbyte::exported);

	return true;
}


Atanor* Atanorbyte::divide(Atanor* a, bool autoself) {
	unsigned char ib = a->Byte();
	if (ib == 0)
		return new AtanorError("Error: Divided by 0");
	if (autoself) {
		value /= ib;
		return this;
	}
	unsigned char i = value / ib;
	return new Atanorbyte(i);
}



Atanor* Atanorbyte::mod(Atanor* a, bool autoself) {
	unsigned char ib = a->Byte();
	if (ib == 0)
		return new AtanorError("Error: Divided by 0");
	if (autoself) {
		value %= ib;
		return this;
	}
	unsigned char i = value % ib;
	return new Atanorbyte(i);
}

Atanor* Atanorbyte::power(Atanor* a, bool autoself) {
	if (autoself) {
		value = pow(Float(), a->Float());
		return this;
	}
	double i = pow(Float(), a->Float());
	return new Atanorbyte(i);
}





