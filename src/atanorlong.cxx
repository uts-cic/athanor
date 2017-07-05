
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
filename   : atanorlong.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorlong.h"
#include "instructions.h"
#include "atanorlvector.h"
#include "atanorustring.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, longMethod>  Atanorlong::methods;
Exporting hmap<string, string> Atanorlong::infomethods;
Exporting bin_hash<unsigned long> Atanorlong::exported;

Exporting short Atanorlong::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorlong::AddMethod(AtanorGlobal* global, string name, longMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorlong::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorlong::idtype = global->Getid("long");

	Atanorlong::AddMethod(global, "chr", &Atanorlong::Methodchr, P_NONE, "chr(): return the character matching the unicode code");
	Atanorlong::AddMethod(global, "format", &Atanorlong::MethodFormat, P_ONE, "format(pattern): Return a string matching the C pattern.");
	Atanorlong::AddMethod(global, "isprime", &Atanorlong::MethodPrime, P_NONE, "prime(): return true is the number is a prime");
	Atanorlong::AddMethod(global, "factors", &Atanorlong::MethodPrimefactors, P_NONE, "factors(): return the list of prime factors");

	Atanorlong::AddMethod(global, "abs", &Atanorlong::Methodabs, P_NONE, "abs(): call fabs on the value");
	Atanorlong::AddMethod(global, "acos", &Atanorlong::Methodacos, P_NONE, "acos(): call acos on the value");
	Atanorlong::AddMethod(global, "acosh", &Atanorlong::Methodacosh, P_NONE, "acosh(): call acosh on the value");
	Atanorlong::AddMethod(global, "asin", &Atanorlong::Methodasin, P_NONE, "asin(): call asin on the value");
	Atanorlong::AddMethod(global, "asinh", &Atanorlong::Methodasinh, P_NONE, "asinh(): call asinh on the value");
	Atanorlong::AddMethod(global, "atan", &Atanorlong::Methodatan, P_NONE, "atan(): call atan on the value");
	Atanorlong::AddMethod(global, "atanh", &Atanorlong::Methodatanh, P_NONE, "atanh(): call atanh on the value");
	Atanorlong::AddMethod(global, "cbrt", &Atanorlong::Methodcbrt, P_NONE, "cbrt(): call cbrt on the value");
	Atanorlong::AddMethod(global, "cos", &Atanorlong::Methodcos, P_NONE, "cos(): call cos on the value");
	Atanorlong::AddMethod(global, "cosh", &Atanorlong::Methodcosh, P_NONE, "cosh(): call cosh on the value");
	Atanorlong::AddMethod(global, "erf", &Atanorlong::Methoderf, P_NONE, "erf(): call erf on the value");
	Atanorlong::AddMethod(global, "erfc", &Atanorlong::Methoderfc, P_NONE, "erfc(): call erfc on the value");
	Atanorlong::AddMethod(global, "exp", &Atanorlong::Methodexp, P_NONE, "exp(): call exp on the value");
	Atanorlong::AddMethod(global, "exp2", &Atanorlong::Methodexp2, P_NONE, "exp2(): call exp2 on the value");
	Atanorlong::AddMethod(global, "expm1", &Atanorlong::Methodexpm1, P_NONE, "expm1(): call expm1 on the value");
	Atanorlong::AddMethod(global, "floor", &Atanorlong::Methodfloor, P_NONE, "floor(): call floor on the value");
	Atanorlong::AddMethod(global, "lgamma", &Atanorlong::Methodlgamma, P_NONE, "lgamma(): call lgamma on the value");
	Atanorlong::AddMethod(global, "ln", &Atanorlong::Methodln, P_NONE, "ln(): call log on the value");
	Atanorlong::AddMethod(global, "log", &Atanorlong::Methodlog, P_NONE, "log(): call log10 on the value");
	Atanorlong::AddMethod(global, "log1p", &Atanorlong::Methodlog1p, P_NONE, "log1p(): call log1p on the value");
	Atanorlong::AddMethod(global, "log2", &Atanorlong::Methodlog2, P_NONE, "log2(): call log2 on the value");
	Atanorlong::AddMethod(global, "logb", &Atanorlong::Methodlogb, P_NONE, "logb(): call logb on the value");
	Atanorlong::AddMethod(global, "nearbyint", &Atanorlong::Methodnearbyint, P_NONE, "nearbyint(): call nearbyint on the value");
	Atanorlong::AddMethod(global, "rint", &Atanorlong::Methodrint, P_NONE, "rint(): call rint on the value");
	Atanorlong::AddMethod(global, "round", &Atanorlong::Methodround, P_NONE, "round(): call round on the value");
	Atanorlong::AddMethod(global, "sin", &Atanorlong::Methodsin, P_NONE, "sin(): call sin on the value");
	Atanorlong::AddMethod(global, "sinh", &Atanorlong::Methodsinh, P_NONE, "sinh(): call sinh on the value");
	Atanorlong::AddMethod(global, "sqrt", &Atanorlong::Methodsqrt, P_NONE, "sqrt(): call sqrt on the value");
	Atanorlong::AddMethod(global, "tan", &Atanorlong::Methodtan, P_NONE, "tan(): call tan on the value");
	Atanorlong::AddMethod(global, "tanh", &Atanorlong::Methodtanh, P_NONE, "tanh(): call tanh on the value");
	Atanorlong::AddMethod(global, "tgamma", &Atanorlong::Methodtgamma, P_NONE, "tgamma(): call tgamma on the value");
	Atanorlong::AddMethod(global, "trunc", &Atanorlong::Methodtrunc, P_NONE, "trunc(): call trunc on the value");
	Atanorlong::AddMethod(global, "even", &Atanorlong::Methodeven, P_NONE, "even(): return true is the value is even");
	Atanorlong::AddMethod(global, "odd", &Atanorlong::Methododd, P_NONE, "odd(): return true is the value is odd");



	global->newInstance[Atanorlong::idtype] = new Atanorlong(0, global);
	global->newInstance[a_longthrough] = global->newInstance[Atanorlong::idtype];
	global->RecordMethods(Atanorlong::idtype,Atanorlong::exported);
	global->RecordMethods(a_longthrough, Atanorlong::exported);
	return true;
}

Atanor* Atanorlong::MethodFormat(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	char buffer[101];
	Atanor* kformat = callfunc->Evaluate(0, contextualpattern, idthread);
	string sformat = kformat->String();

#ifdef WIN32
	_set_invalid_parameter_handler(wrongSprintf);

	errorsprintf = false;
	sprintf_s(buffer, 100, STR(sformat), Long());
	if (errorsprintf == true)
		return globalAtanor->Returnerror("Incorrect format specifier or size too long", idthread);
#else
	int spres;
	spres = sprintf_s(buffer, 100, STR(sformat), Long());
	if (spres<0)
		return globalAtanor->Returnerror("KIF(124):Incorrect format specifier or size too long", idthread);
#endif

	return globalAtanor->Providestring(buffer);
}



Atanor* Atanorlong::Methodchr(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanorustring* r = globalAtanor->Provideustring(L"");
	r->value = (wchar_t)value;
	return r;
}

//--------------------------------------------------------------------------------------------

void AtanorLoopLong::Callfunction() {

	AtanorCallFunction kfunc(function);

	Atanorint* ki = globalAtanor->Provideint(position);
	ki->Setreference();
	kfunc.arguments.push_back(this);
	kfunc.arguments.push_back(ki);
	Atanor* ke = kfunc.Get(aNULL, aNULL, globalAtanor->GetThreadid());
	ke->Release();
	ki->Resetreference();
}

Atanor* AtanorLoopLong::Put(Atanor* context, Atanor* ke, short idthread) {
	if (ke->isVectorContainer()) {
		position = 0;
		interval.clear();
		for (int i = 0; i < ke->Size(); i++)
			interval.push_back(ke->getlong(i));
		value = interval[0];
		return aTRUE;
	}

	if (ke->Type() == a_lloop) {
		AtanorLoopLong* kl = (AtanorLoopLong*)ke;
		interval = kl->interval;
		value = kl->value;
		position = kl->position;
		return aTRUE;
	}

	if (interval.size() == 0) {
		position = 0;
		value = 0;
		return aTRUE;
	}

	position = ke->Integer();
	if (position >= interval.size())
		position = position % interval.size();
	else
	if (position<0)
		position = (interval.size() + position) % interval.size();
	value = interval[position];
	return aTRUE;
}


Atanor* AtanorLoopLong::Vector(short idthread) {
	Atanorlvector* kvect = new Atanorlvector;
	kvect->values = interval;
	return kvect;
}



