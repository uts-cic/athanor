
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
filename   : atanorfloat.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorfloat.h"
#include "atanorstring.h"
#ifdef WIN32
#define _USE_MATH_DEFINES
#include <math.h>
#endif 

#include "atanorfvector.h"
#include "instructions.h"
#include "constobjects.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, floatMethod>  Atanorfloat::methods;
Exporting hmap<string, string> Atanorfloat::infomethods;
Exporting bin_hash<unsigned long> Atanorfloat::exported;

Exporting short Atanorfloat::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorfloat::AddMethod(AtanorGlobal* global, string name, floatMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

static const double M_GOLDEN = 1.61803398874989484820458683436563811772030917980576286213544862270526046281890244970720720418939113748475;

bool Atanorfloat::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorfloat::idtype = global->Getid("float");

	Atanorfloat::AddMethod(global, "chr", &Atanorfloat::Methodchr, P_NONE, "chr(): return the character matching the unicode code");
	Atanorfloat::AddMethod(global, "invert", &Atanorfloat::MethodInvert, P_NONE, "");

	Atanorfloat::AddMethod(global, "format", &Atanorfloat::MethodFormat, P_ONE, "format(pattern): Return a string matching the C pattern.");
	Atanorfloat::AddMethod(global, "radian", &Atanorfloat::MethodRadian, P_NONE, "abs(): call fabs on the value");
	Atanorfloat::AddMethod(global, "degree", &Atanorfloat::MethodDegree, P_NONE, "abs(): call fabs on the value");

	Atanorfloat::AddMethod(global, "abs", &Atanorfloat::Methodabs, P_NONE, "abs(): call fabs on the value");
	Atanorfloat::AddMethod(global, "acos", &Atanorfloat::Methodacos, P_NONE, "acos(): call acos on the value");
	Atanorfloat::AddMethod(global, "acosh", &Atanorfloat::Methodacosh, P_NONE, "acosh(): call acosh on the value");
	Atanorfloat::AddMethod(global, "asin", &Atanorfloat::Methodasin, P_NONE, "asin(): call asin on the value");
	Atanorfloat::AddMethod(global, "asinh", &Atanorfloat::Methodasinh, P_NONE, "asinh(): call asinh on the value");
	Atanorfloat::AddMethod(global, "atan", &Atanorfloat::Methodatan, P_NONE, "atan(): call atan on the value");
	Atanorfloat::AddMethod(global, "atanh", &Atanorfloat::Methodatanh, P_NONE, "atanh(): call atanh on the value");
	Atanorfloat::AddMethod(global, "cbrt", &Atanorfloat::Methodcbrt, P_NONE, "cbrt(): call cbrt on the value");
	Atanorfloat::AddMethod(global, "cos", &Atanorfloat::Methodcos, P_NONE, "cos(): call cos on the value");
	Atanorfloat::AddMethod(global, "cosh", &Atanorfloat::Methodcosh, P_NONE, "cosh(): call cosh on the value");
	Atanorfloat::AddMethod(global, "erf", &Atanorfloat::Methoderf, P_NONE, "erf(): call erf on the value");
	Atanorfloat::AddMethod(global, "erfc", &Atanorfloat::Methoderfc, P_NONE, "erfc(): call erfc on the value");
	Atanorfloat::AddMethod(global, "exp", &Atanorfloat::Methodexp, P_NONE, "exp(): call exp on the value");
	Atanorfloat::AddMethod(global, "exp2", &Atanorfloat::Methodexp2, P_NONE, "exp2(): call exp2 on the value");
	Atanorfloat::AddMethod(global, "expm1", &Atanorfloat::Methodexpm1, P_NONE, "expm1(): call expm1 on the value");
	Atanorfloat::AddMethod(global, "floor", &Atanorfloat::Methodfloor, P_NONE, "floor(): call floor on the value");
	Atanorfloat::AddMethod(global, "lgamma", &Atanorfloat::Methodlgamma, P_NONE, "lgamma(): call lgamma on the value");
	Atanorfloat::AddMethod(global, "ln", &Atanorfloat::Methodln, P_NONE, "ln(): call log on the value");
	Atanorfloat::AddMethod(global, "log", &Atanorfloat::Methodlog, P_NONE, "log(): call log10 on the value");
	Atanorfloat::AddMethod(global, "log1p", &Atanorfloat::Methodlog1p, P_NONE, "log1p(): call log1p on the value");
	Atanorfloat::AddMethod(global, "log2", &Atanorfloat::Methodlog2, P_NONE, "log2(): call log2 on the value");
	Atanorfloat::AddMethod(global, "logb", &Atanorfloat::Methodlogb, P_NONE, "logb(): call logb on the value");
	Atanorfloat::AddMethod(global, "nearbyint", &Atanorfloat::Methodnearbyint, P_NONE, "nearbyint(): call nearbyint on the value");
	Atanorfloat::AddMethod(global, "rint", &Atanorfloat::Methodrint, P_NONE, "rint(): call rint on the value");
	Atanorfloat::AddMethod(global, "round", &Atanorfloat::Methodround, P_NONE, "round(): call round on the value");
	Atanorfloat::AddMethod(global, "sin", &Atanorfloat::Methodsin, P_NONE, "sin(): call sin on the value");
	Atanorfloat::AddMethod(global, "sinh", &Atanorfloat::Methodsinh, P_NONE, "sinh(): call sinh on the value");
	Atanorfloat::AddMethod(global, "sqrt", &Atanorfloat::Methodsqrt, P_NONE, "sqrt(): call sqrt on the value");
	Atanorfloat::AddMethod(global, "tan", &Atanorfloat::Methodtan, P_NONE, "tan(): call tan on the value");
	Atanorfloat::AddMethod(global, "tanh", &Atanorfloat::Methodtanh, P_NONE, "tanh(): call tanh on the value");
	Atanorfloat::AddMethod(global, "tgamma", &Atanorfloat::Methodtgamma, P_NONE, "tgamma(): call tgamma on the value");
	Atanorfloat::AddMethod(global, "trunc", &Atanorfloat::Methodtrunc, P_NONE, "trunc(): call trunc on the value");
	Atanorfloat::AddMethod(global, "even", &Atanorfloat::Methodeven, P_NONE, "even(): return true is the value is even");
	Atanorfloat::AddMethod(global, "odd", &Atanorfloat::Methododd, P_NONE, "odd(): return true is the value is odd");

	short idreal = global->Getid("real");

	global->newInstance[Atanorfloat::idtype] = new Atanorfloat(0, global);
	global->newInstance[idreal] = new Atanorfloat(0, global);
	global->newInstance[a_floatthrough] = global->newInstance[Atanorfloat::idtype];

	global->RecordMethods(Atanorfloat::idtype, Atanorfloat::exported);
	global->RecordMethods(idreal, Atanorfloat::exported);
	global->RecordMethods(a_floatthrough, Atanorfloat::exported);

	Atanor* a = new AtanorSystemVariable(global, new AtanorConstFloat(M_PI), global->Createid("_pi"), a_float);

	a = new AtanorSystemVariable(global, new AtanorConstFloat(M_PI), global->Createid("π"), a_float);
	a = new AtanorSystemVariable(global, new AtanorConstFloat(M_E), global->Createid("_e"), a_float);
	a = new AtanorSystemVariable(global, new AtanorConstFloat(M_E), global->Createid("ℯ"), a_float);

	a = new AtanorSystemVariable(global, new AtanorConstFloat(M_GOLDEN), global->Createid("_phi"), a_float);
	a = new AtanorSystemVariable(global, new AtanorConstFloat(M_GOLDEN), global->Createid("φ"), a_float);
	
	return true;
}

Atanor* Atanorfloat::Methodchr(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanorustring* r = globalAtanor->Provideustring(L"");
	r->value = (wchar_t)value;
	return r;
}


Atanor* Atanorfloat::MethodFormat(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	char buffer[101];
	Atanor* kformat = callfunc->Evaluate(0, contextualpattern, idthread);
	string sformat = kformat->String();

#ifdef WIN32
	_set_invalid_parameter_handler(wrongSprintf);

	errorsprintf = false;
	sprintf_s(buffer, 100, STR(sformat), Float());
	if (errorsprintf == true)
		return globalAtanor->Returnerror("Incorrect format specifier or size too long", idthread);
#else
	int spres;
	spres = sprintf_s(buffer, 100, STR(sformat), Float());
	if (spres<0)
		return globalAtanor->Returnerror("KIF(124):Incorrect format specifier or size too long",idthread);
#endif

	return globalAtanor->Providestring(buffer);
}


Atanor* Atanorfloat::MethodRadian(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	double num = value;
	num = M_PI*(num / 180);
	return globalAtanor->Providefloat(num);
}

Atanor* Atanorfloat::MethodDegree(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	double num = value;
	num = (num * 180) / M_PI;
	return globalAtanor->Providefloat(num);
}

void AtanorLoopFloat::Callfunction() {

	AtanorCallFunction kfunc(function);

	Atanorint* ki = globalAtanor->Provideint(position);
	ki->Setreference();
	kfunc.arguments.push_back(this);
	kfunc.arguments.push_back(ki);
	Atanor* ke = kfunc.Get(aNULL, aNULL, globalAtanor->GetThreadid());
	ke->Release();
	ki->Resetreference();
}

Atanor* AtanorLoopFloat::Put(Atanor* context, Atanor* ke, short idthread) {
	if (ke->isVectorContainer()) {
		position = 0;
		interval.clear();
		for (int i = 0; i < ke->Size(); i++)
			interval.push_back(ke->getfloat(i));
		value = interval[0];
		return aTRUE;
	}

	if (ke->Type() == a_floop) {
		AtanorLoopFloat* kl = (AtanorLoopFloat*)ke;
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


Atanor* AtanorLoopFloat::Vector(short idthread) {
	Atanorfvector* kvect = globalAtanor->Providefvector();
	kvect->values = interval;
	return kvect;
}



