
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
filename   : atanorshort.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorshort.h"
#include "atanorint.h"
#include "atanorlong.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, shortMethod>  Atanorshort::methods;
Exporting hmap<string, string> Atanorshort::infomethods;
Exporting bin_hash<unsigned long> Atanorshort::exported;

Exporting short Atanorshort::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorshort::AddMethod(AtanorGlobal* global, string name, shortMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorshort::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorshort::idtype = global->Getid("short");

	Atanorshort::AddMethod(global, "succ", &Atanorshort::MethodSucc, P_NONE, "succ(): return the successor of a short");
	Atanorshort::AddMethod(global, "pred", &Atanorshort::MethodPred, P_NONE, "pred(): Return the predecessor of a byte.");

	Atanorshort::AddMethod(global, "chr", &Atanorshort::Methodchr, P_NONE, "chr(): return the character matching the unicode code");
	Atanorshort::AddMethod(global, "format", &Atanorshort::MethodFormat, P_ONE, "format(pattern): Return a string matching the C pattern.");
	Atanorshort::AddMethod(global, "isprime", &Atanorshort::MethodPrime, P_NONE, "prime(): return true is the number is a prime");
	Atanorshort::AddMethod(global, "factors", &Atanorshort::MethodPrimefactors, P_NONE, "factors(): return the list of prime factors");

	Atanorshort::AddMethod(global, "abs", &Atanorshort::Methodabs, P_NONE, "abs(): call fabs on the value");
	Atanorshort::AddMethod(global, "acos", &Atanorshort::Methodacos, P_NONE, "acos(): call acos on the value");
	Atanorshort::AddMethod(global, "acosh", &Atanorshort::Methodacosh, P_NONE, "acosh(): call acosh on the value");
	Atanorshort::AddMethod(global, "asin", &Atanorshort::Methodasin, P_NONE, "asin(): call asin on the value");
	Atanorshort::AddMethod(global, "asinh", &Atanorshort::Methodasinh, P_NONE, "asinh(): call asinh on the value");
	Atanorshort::AddMethod(global, "atan", &Atanorshort::Methodatan, P_NONE, "atan(): call atan on the value");
	Atanorshort::AddMethod(global, "atanh", &Atanorshort::Methodatanh, P_NONE, "atanh(): call atanh on the value");
	Atanorshort::AddMethod(global, "cbrt", &Atanorshort::Methodcbrt, P_NONE, "cbrt(): call cbrt on the value");
	Atanorshort::AddMethod(global, "cos", &Atanorshort::Methodcos, P_NONE, "cos(): call cos on the value");
	Atanorshort::AddMethod(global, "cosh", &Atanorshort::Methodcosh, P_NONE, "cosh(): call cosh on the value");
	Atanorshort::AddMethod(global, "erf", &Atanorshort::Methoderf, P_NONE, "erf(): call erf on the value");
	Atanorshort::AddMethod(global, "erfc", &Atanorshort::Methoderfc, P_NONE, "erfc(): call erfc on the value");
	Atanorshort::AddMethod(global, "exp", &Atanorshort::Methodexp, P_NONE, "exp(): call exp on the value");
	Atanorshort::AddMethod(global, "exp2", &Atanorshort::Methodexp2, P_NONE, "exp2(): call exp2 on the value");
	Atanorshort::AddMethod(global, "expm1", &Atanorshort::Methodexpm1, P_NONE, "expm1(): call expm1 on the value");
	Atanorshort::AddMethod(global, "floor", &Atanorshort::Methodfloor, P_NONE, "floor(): call floor on the value");
	Atanorshort::AddMethod(global, "lgamma", &Atanorshort::Methodlgamma, P_NONE, "lgamma(): call lgamma on the value");
	Atanorshort::AddMethod(global, "ln", &Atanorshort::Methodln, P_NONE, "ln(): call log on the value");
	Atanorshort::AddMethod(global, "log", &Atanorshort::Methodlog, P_NONE, "log(): call log10 on the value");
	Atanorshort::AddMethod(global, "log1p", &Atanorshort::Methodlog1p, P_NONE, "log1p(): call log1p on the value");
	Atanorshort::AddMethod(global, "log2", &Atanorshort::Methodlog2, P_NONE, "log2(): call log2 on the value");
	Atanorshort::AddMethod(global, "logb", &Atanorshort::Methodlogb, P_NONE, "logb(): call logb on the value");
	Atanorshort::AddMethod(global, "nearbyint", &Atanorshort::Methodnearbyint, P_NONE, "nearbyint(): call nearbyint on the value");
	Atanorshort::AddMethod(global, "rint", &Atanorshort::Methodrint, P_NONE, "rint(): call rint on the value");
	Atanorshort::AddMethod(global, "round", &Atanorshort::Methodround, P_NONE, "round(): call round on the value");
	Atanorshort::AddMethod(global, "sin", &Atanorshort::Methodsin, P_NONE, "sin(): call sin on the value");
	Atanorshort::AddMethod(global, "sinh", &Atanorshort::Methodsinh, P_NONE, "sinh(): call sinh on the value");
	Atanorshort::AddMethod(global, "sqrt", &Atanorshort::Methodsqrt, P_NONE, "sqrt(): call sqrt on the value");
	Atanorshort::AddMethod(global, "tan", &Atanorshort::Methodtan, P_NONE, "tan(): call tan on the value");
	Atanorshort::AddMethod(global, "tanh", &Atanorshort::Methodtanh, P_NONE, "tanh(): call tanh on the value");
	Atanorshort::AddMethod(global, "tgamma", &Atanorshort::Methodtgamma, P_NONE, "tgamma(): call tgamma on the value");
	Atanorshort::AddMethod(global, "trunc", &Atanorshort::Methodtrunc, P_NONE, "trunc(): call trunc on the value");
	Atanorshort::AddMethod(global, "even", &Atanorshort::Methodeven, P_NONE, "even(): return true is the value is even");
	Atanorshort::AddMethod(global, "odd", &Atanorshort::Methododd, P_NONE, "odd(): return true is the value is odd");




	global->newInstance[Atanorshort::idtype] = new Atanorshort(0, global);
	global->RecordMethods(Atanorshort::idtype,Atanorshort::exported);

	return true;
}

Atanor* Atanorshort::MethodFormat(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	char buffer[101];
	Atanor* kformat = callfunc->Evaluate(0, contextualpattern, idthread);
	string sformat = kformat->String();

#ifdef WIN32
	_set_invalid_parameter_handler(wrongSprintf);

	errorsprintf = false;
	sprintf_s(buffer, 100, STR(sformat), Short());
	if (errorsprintf == true)
		return globalAtanor->Returnerror("Incorrect format specifier or size too long", idthread);
#else
	int spres;
	spres = sprintf_s(buffer, 100, STR(sformat), Short());
	if (spres<0)
		return globalAtanor->Returnerror("KIF(124):Incorrect format specifier or size too long", idthread);
#endif

	return globalAtanor->Providestring(buffer);
}



Atanor* Atanorshort::plus(Atanor* a, bool itself) {
	if (!itself && a->isFloat()) {
		double v = value;
		return globalAtanor->Providefloat(v + a->Float());
	}

	BLONG v = value;
	v += a->Long();

	if (IsLong(v))
		return new Atanorlong(v);

	if (!IsShort(v))
		return globalAtanor->Provideint(v);


	if (itself) {
		value = v;
		return this;
	}

	return new Atanorshort(v);
}

Atanor* Atanorshort::multiply(Atanor* a, bool itself) {
	if (!itself && a->isFloat()) {
		double v = value;
		return globalAtanor->Providefloat(v + a->Float());
	}

	BLONG v = value;
	v *= a->Long();

	if (IsLong(v))
		return new Atanorlong(v);

	if (!IsShort(v))
		return globalAtanor->Provideint(v);


	if (itself) {
		value = v;
		return this;
	}

	return new Atanorshort(v);
}

Atanor* Atanorshort::shiftleft(Atanor* a, bool itself) {
	if (!itself && a->isFloat()) {
		double v = value;
		return globalAtanor->Providefloat(v + a->Float());
	}

	BLONG v = value;
	v <<= a->Long();

	if (IsLong(v))
		return new Atanorlong(v);

	if (!IsShort(v))
		return globalAtanor->Provideint(v);

	if (itself) {
		value = v;
		return this;
	}

	return new Atanorshort(v);
}



