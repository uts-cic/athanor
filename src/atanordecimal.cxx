
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
filename   : atanordecimal.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanordecimal.h"
#include "atanorstring.h"
#ifdef WIN32
#define _USE_MATH_DEFINES
#include <math.h>
#endif 

#include "atanordvector.h"
#include "instructions.h"
#include "constobjects.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, decimalMethod>  Atanordecimal::methods;
Exporting hmap<string, string> Atanordecimal::infomethods;
Exporting bin_hash<unsigned long> Atanordecimal::exported;

Exporting short Atanordecimal::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanordecimal::AddMethod(AtanorGlobal* global, string name, decimalMethod func, unsigned long arity, string infos, short typereturn) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
	if (typereturn != a_null)
		global->returntypes[idname] = typereturn;
}

bool Atanordecimal::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanordecimal::idtype = global->Getid("decimal");

	Atanordecimal::AddMethod(global, "chr", &Atanordecimal::Methodchr, P_NONE, "chr(): return the character matching the unicode code", a_ustring);
	Atanordecimal::AddMethod(global, "invert", &Atanordecimal::MethodInvert, P_NONE, "", a_float);
	Atanordecimal::AddMethod(global, "succ", &Atanordecimal::MethodSucc, P_NONE, "succ(): Return a successor of the current value", a_float);
	Atanordecimal::AddMethod(global, "pred", &Atanordecimal::MethodPred, P_NONE, "pred(): Return the predecessor of a byte.", a_float);

	Atanordecimal::AddMethod(global, "format", &Atanordecimal::MethodFormat, P_ONE, "format(pattern): Return a string matching the C pattern.", a_string);
	Atanordecimal::AddMethod(global, "radian", &Atanordecimal::MethodRadian, P_NONE, "abs(): call fabs on the value", a_float);
	Atanordecimal::AddMethod(global, "degree", &Atanordecimal::MethodDegree, P_NONE, "abs(): call fabs on the value", a_float);

	Atanordecimal::AddMethod(global, "abs", &Atanordecimal::Methodabs, P_NONE, "abs(): call fabs on the value", a_float);
	Atanordecimal::AddMethod(global, "acos", &Atanordecimal::Methodacos, P_NONE, "acos(): call acos on the value", a_float);
	Atanordecimal::AddMethod(global, "acosh", &Atanordecimal::Methodacosh, P_NONE, "acosh(): call acosh on the value", a_float);
	Atanordecimal::AddMethod(global, "asin", &Atanordecimal::Methodasin, P_NONE, "asin(): call asin on the value", a_float);
	Atanordecimal::AddMethod(global, "asinh", &Atanordecimal::Methodasinh, P_NONE, "asinh(): call asinh on the value", a_float);
	Atanordecimal::AddMethod(global, "atan", &Atanordecimal::Methodatan, P_NONE, "atan(): call atan on the value", a_float);
	Atanordecimal::AddMethod(global, "atanh", &Atanordecimal::Methodatanh, P_NONE, "atanh(): call atanh on the value", a_float);
	Atanordecimal::AddMethod(global, "cbrt", &Atanordecimal::Methodcbrt, P_NONE, "cbrt(): call cbrt on the value", a_float);
	Atanordecimal::AddMethod(global, "cos", &Atanordecimal::Methodcos, P_NONE, "cos(): call cos on the value", a_float);
	Atanordecimal::AddMethod(global, "cosh", &Atanordecimal::Methodcosh, P_NONE, "cosh(): call cosh on the value", a_float);
	Atanordecimal::AddMethod(global, "erf", &Atanordecimal::Methoderf, P_NONE, "erf(): call erf on the value", a_float);
	Atanordecimal::AddMethod(global, "erfc", &Atanordecimal::Methoderfc, P_NONE, "erfc(): call erfc on the value", a_float);
	Atanordecimal::AddMethod(global, "exp", &Atanordecimal::Methodexp, P_NONE, "exp(): call exp on the value", a_float);
	Atanordecimal::AddMethod(global, "exp2", &Atanordecimal::Methodexp2, P_NONE, "exp2(): call exp2 on the value", a_float);
	Atanordecimal::AddMethod(global, "expm1", &Atanordecimal::Methodexpm1, P_NONE, "expm1(): call expm1 on the value", a_float);
	Atanordecimal::AddMethod(global, "floor", &Atanordecimal::Methodfloor, P_NONE, "floor(): call floor on the value", a_float);
	Atanordecimal::AddMethod(global, "lgamma", &Atanordecimal::Methodlgamma, P_NONE, "lgamma(): call lgamma on the value", a_float);
	Atanordecimal::AddMethod(global, "ln", &Atanordecimal::Methodln, P_NONE, "ln(): call log on the value", a_float);
	Atanordecimal::AddMethod(global, "log", &Atanordecimal::Methodlog, P_NONE, "log(): call log10 on the value", a_float);
	Atanordecimal::AddMethod(global, "log1p", &Atanordecimal::Methodlog1p, P_NONE, "log1p(): call log1p on the value", a_float);
	Atanordecimal::AddMethod(global, "log2", &Atanordecimal::Methodlog2, P_NONE, "log2(): call log2 on the value", a_float);
	Atanordecimal::AddMethod(global, "logb", &Atanordecimal::Methodlogb, P_NONE, "logb(): call logb on the value", a_float);
	Atanordecimal::AddMethod(global, "nearbyint", &Atanordecimal::Methodnearbyint, P_NONE, "nearbyint(): call nearbyint on the value", a_float);
	Atanordecimal::AddMethod(global, "rint", &Atanordecimal::Methodrint, P_NONE, "rint(): call rint on the value", a_float);
	Atanordecimal::AddMethod(global, "round", &Atanordecimal::Methodround, P_NONE, "round(): call round on the value", a_float);
	Atanordecimal::AddMethod(global, "sin", &Atanordecimal::Methodsin, P_NONE, "sin(): call sin on the value", a_float);
	Atanordecimal::AddMethod(global, "sinh", &Atanordecimal::Methodsinh, P_NONE, "sinh(): call sinh on the value", a_float);
	Atanordecimal::AddMethod(global, "sqrt", &Atanordecimal::Methodsqrt, P_NONE, "sqrt(): call sqrt on the value", a_float);
	Atanordecimal::AddMethod(global, "tan", &Atanordecimal::Methodtan, P_NONE, "tan(): call tan on the value", a_float);
	Atanordecimal::AddMethod(global, "tanh", &Atanordecimal::Methodtanh, P_NONE, "tanh(): call tanh on the value", a_float);
	Atanordecimal::AddMethod(global, "tgamma", &Atanordecimal::Methodtgamma, P_NONE, "tgamma(): call tgamma on the value", a_float);
	Atanordecimal::AddMethod(global, "trunc", &Atanordecimal::Methodtrunc, P_NONE, "trunc(): call trunc on the value", a_float);
	Atanordecimal::AddMethod(global, "even", &Atanordecimal::Methodeven, P_NONE, "even(): return true is the value is even", a_float);
	Atanordecimal::AddMethod(global, "odd", &Atanordecimal::Methododd, P_NONE, "odd(): return true is the value is odd", a_float);



	global->newInstance[Atanordecimal::idtype] = new Atanordecimal(0, global);
	global->newInstance[a_decimalthrough] = global->newInstance[Atanordecimal::idtype];
	global->RecordMethods(Atanordecimal::idtype, Atanordecimal::exported);
	global->RecordMethods(a_decimalthrough, Atanordecimal::exported);
	return true;
}

Atanor* Atanordecimal::Methodchr(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanorustring* r = globalAtanor->Provideustring(L"");
	r->value = (wchar_t)value;
	return r;
}


Atanor* Atanordecimal::MethodFormat(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

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


Atanor* Atanordecimal::MethodRadian(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	float num = value;
	num = M_PI*(num / 180);
	return new Atanordecimal(num);
}

Atanor* Atanordecimal::MethodDegree(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	float num = value;
	num = (num * 180) / M_PI;
	return new Atanordecimal(num);
}

void AtanorLoopDecimal::Callfunction() {

	AtanorCallFunction kfunc(function);

	Atanorint* ki = globalAtanor->Provideint(position);
	ki->Setreference();
	kfunc.arguments.push_back(this);
	kfunc.arguments.push_back(ki);
	Atanor* ke = kfunc.Get(aNULL, aNULL, globalAtanor->GetThreadid());
	ke->Release();
	ki->Resetreference();
}

Atanor* AtanorLoopDecimal::Put(Atanor* context, Atanor* ke, short idthread) {
	if (ke->isVectorContainer()) {
		position = 0;
		interval.clear();
		for (int i = 0; i < ke->Size(); i++)
			interval.push_back(ke->getdecimal(i));
		value = interval[0];
		return aTRUE;
	}

	if (ke->Type() == a_dloop) {
		AtanorLoopDecimal* kl = (AtanorLoopDecimal*)ke;
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


Atanor* AtanorLoopDecimal::Vector(short idthread) {
	Atanordvector* kvect = new Atanordvector;
	kvect->values = interval;
	return kvect;
}



