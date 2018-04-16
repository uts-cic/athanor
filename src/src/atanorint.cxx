
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
filename   : atanorint.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorint.h"
#include "atanorstring.h"
#include "atanorustring.h"
#include "atanorivector.h"
#include "instructions.h"
#include "atanorlong.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, intMethod>  Atanorint::methods;
Exporting hmap<string, string> Atanorint::infomethods;
Exporting bin_hash<unsigned long> Atanorint::exported;

Exporting short Atanorint::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorint::AddMethod(AtanorGlobal* global, string name, intMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorint::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorint::idtype = global->Getid("int");

	Atanorint::AddMethod(global, "isemoji", &Atanorint::MethodIsemoji, P_NONE, "isemoji(): Test if a string only contains emoji characters");
	Atanorint::AddMethod(global, "emoji", &Atanorint::MethodEmoji, P_NONE, "emoji(): Return the textual description of an emoji");
	global->returntypes[global->Getid("emoji")] = a_string;

	Atanorint::AddMethod(global, "chr", &Atanorint::Methodchr, P_NONE, "chr(): return the character matching the unicode code");
	Atanorint::AddMethod(global, "succ", &Atanorint::MethodSucc, P_NONE, "succ(): return the successor of an integer");
	Atanorint::AddMethod(global, "pred", &Atanorint::MethodPred, P_NONE, "pred(): Return the predecessor of a byte.");

	Atanorint::AddMethod(global, "isprime", &Atanorint::MethodPrime, P_NONE, "prime(): return true is the number is a prime");
	Atanorint::AddMethod(global, "factors", &Atanorint::MethodPrimefactors, P_NONE, "factors(): return the list of prime factors");
	global->returntypes[global->Getid("factors")] = a_ivector;
	Atanorint::AddMethod(global, "bit", &Atanorint::MethodBit, P_ONE, "bit(i): check if the ith bit is 0 or 1");
	Atanorint::AddMethod(global, "invert", &Atanorint::MethodInvert, P_NONE, "");

	Atanorint::AddMethod(global, "format", &Atanorint::MethodFormat, P_ONE, "format(pattern): Return a string matching the C pattern.");

	Atanorint::AddMethod(global, "abs", &Atanorint::Methodabs, P_NONE, "abs(): call fabs on the value");
	Atanorint::AddMethod(global, "acos", &Atanorint::Methodacos, P_NONE, "acos(): call acos on the value");
	Atanorint::AddMethod(global, "acosh", &Atanorint::Methodacosh, P_NONE, "acosh(): call acosh on the value");
	Atanorint::AddMethod(global, "asin", &Atanorint::Methodasin, P_NONE, "asin(): call asin on the value");
	Atanorint::AddMethod(global, "asinh", &Atanorint::Methodasinh, P_NONE, "asinh(): call asinh on the value");
	Atanorint::AddMethod(global, "atan", &Atanorint::Methodatan, P_NONE, "atan(): call atan on the value");
	Atanorint::AddMethod(global, "atanh", &Atanorint::Methodatanh, P_NONE, "atanh(): call atanh on the value");
	Atanorint::AddMethod(global, "cbrt", &Atanorint::Methodcbrt, P_NONE, "cbrt(): call cbrt on the value");
	Atanorint::AddMethod(global, "cos", &Atanorint::Methodcos, P_NONE, "cos(): call cos on the value");
	Atanorint::AddMethod(global, "cosh", &Atanorint::Methodcosh, P_NONE, "cosh(): call cosh on the value");
	Atanorint::AddMethod(global, "erf", &Atanorint::Methoderf, P_NONE, "erf(): call erf on the value");
	Atanorint::AddMethod(global, "erfc", &Atanorint::Methoderfc, P_NONE, "erfc(): call erfc on the value");
	Atanorint::AddMethod(global, "exp", &Atanorint::Methodexp, P_NONE, "exp(): call exp on the value");
	Atanorint::AddMethod(global, "exp2", &Atanorint::Methodexp2, P_NONE, "exp2(): call exp2 on the value");
	Atanorint::AddMethod(global, "expm1", &Atanorint::Methodexpm1, P_NONE, "expm1(): call expm1 on the value");
	Atanorint::AddMethod(global, "floor", &Atanorint::Methodfloor, P_NONE, "floor(): call floor on the value");
	Atanorint::AddMethod(global, "lgamma", &Atanorint::Methodlgamma, P_NONE, "lgamma(): call lgamma on the value");
	Atanorint::AddMethod(global, "ln", &Atanorint::Methodln, P_NONE, "ln(): call log on the value");
	Atanorint::AddMethod(global, "log", &Atanorint::Methodlog, P_NONE, "log(): call log10 on the value");
	Atanorint::AddMethod(global, "log1p", &Atanorint::Methodlog1p, P_NONE, "log1p(): call log1p on the value");
	Atanorint::AddMethod(global, "log2", &Atanorint::Methodlog2, P_NONE, "log2(): call log2 on the value");
	Atanorint::AddMethod(global, "logb", &Atanorint::Methodlogb, P_NONE, "logb(): call logb on the value");
	Atanorint::AddMethod(global, "nearbyint", &Atanorint::Methodnearbyint, P_NONE, "nearbyint(): call nearbyint on the value");
	Atanorint::AddMethod(global, "rint", &Atanorint::Methodrint, P_NONE, "rint(): call rint on the value");
	Atanorint::AddMethod(global, "round", &Atanorint::Methodround, P_NONE, "round(): call round on the value");
	Atanorint::AddMethod(global, "sin", &Atanorint::Methodsin, P_NONE, "sin(): call sin on the value");
	Atanorint::AddMethod(global, "sinh", &Atanorint::Methodsinh, P_NONE, "sinh(): call sinh on the value");
	Atanorint::AddMethod(global, "sqrt", &Atanorint::Methodsqrt, P_NONE, "sqrt(): call sqrt on the value");
	Atanorint::AddMethod(global, "tan", &Atanorint::Methodtan, P_NONE, "tan(): call tan on the value");
	Atanorint::AddMethod(global, "tanh", &Atanorint::Methodtanh, P_NONE, "tanh(): call tanh on the value");
	Atanorint::AddMethod(global, "tgamma", &Atanorint::Methodtgamma, P_NONE, "tgamma(): call tgamma on the value");
	Atanorint::AddMethod(global, "trunc", &Atanorint::Methodtrunc, P_NONE, "trunc(): call trunc on the value");
	Atanorint::AddMethod(global, "even", &Atanorint::Methodeven, P_NONE, "even(): return true is the value is even");
	Atanorint::AddMethod(global, "odd", &Atanorint::Methododd, P_NONE, "odd(): return true is the value is odd");


	global->newInstance[Atanorint::idtype] = new Atanorint(0, global);
	global->newInstance[a_intthrough] = global->newInstance[Atanorint::idtype];
	global->RecordMethods(Atanorint::idtype, Atanorint::exported);
	global->RecordMethods(a_intthrough, Atanorint::exported);

	return true;
}


Atanor* Atanorint::MethodIsemoji(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (c_is_emoji((BLONG)value))
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorint::MethodEmoji(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	string s = Emoji((BLONG)value);
	return globalAtanor->Providestring(s);
}


Atanor* Atanorint::Methodchr(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanorustring* r = globalAtanor->Provideustring(L"");
	r->value = (wchar_t)value;
	return r;
}

Atanor* Atanorint::MethodFormat(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	char buffer[101];
	Atanor* kformat = callfunc->Evaluate(0, contextualpattern, idthread);
	string sformat = kformat->String();

#ifdef WIN32
	_set_invalid_parameter_handler(wrongSprintf);

	errorsprintf = false;
	sprintf_s(buffer, 100, STR(sformat), Integer());
	if (errorsprintf == true)
		return globalAtanor->Returnerror("Incorrect format specifier or size too long", idthread);
#else
	int spres;
	spres = sprintf_s(buffer, 100, STR(sformat), Integer());
	if (spres<0)
		return globalAtanor->Returnerror("KIF(124):Incorrect format specifier or size too long", idthread);
#endif

	return globalAtanor->Providestring(buffer);
}

Atanor* Atanorint::MethodBit(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	long ith = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	long v = value;
	if (ith < 0 || ith >= sizeof(long)* 8)
		return aFALSE;
	v &= 1 << ith;
	if (!v)
		return aFALSE;
	return aTRUE;
}


void AtanorLoopInteger::Callfunction() {

	AtanorCallFunction kfunc(function);

	Atanorint* ki = globalAtanor->Provideint(position);
	ki->Setreference();
	kfunc.arguments.push_back(this);
	kfunc.arguments.push_back(ki);
	Atanor* ke = kfunc.Get(aNULL, aNULL, globalAtanor->GetThreadid());
	ke->Release();
	ki->Resetreference();
}

Atanor* AtanorLoopInteger::Put(Atanor* context, Atanor* ke, short idthread) {
	if (ke->isVectorContainer()) {
		position = 0;
		interval.clear();
		for (int i = 0; i < ke->Size(); i++)
			interval.push_back(ke->getinteger(i));
		value = interval[0];
		return aTRUE;
	}

	if (ke->Type() == a_iloop) {
		AtanorLoopInteger* kl = (AtanorLoopInteger*)ke;
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


Atanor* AtanorLoopInteger::Vector(short idthread) {
	Atanorivector* kvect = globalAtanor->Provideivector();
	kvect->values = interval;
	return kvect;
}

Exporting Atanor* Atanorint::plus(Atanor* a, bool itself) {
	if (!itself && a->isFloat()) {
		double v = value;
		return globalAtanor->Providefloat(v + a->Float());
	}

	BLONG v = value;
	v += a->Long();

	if (IsLong(v))
		return new Atanorlong(v);

	if (itself) {
		value = v;
		return this;
	}


	return globalAtanor->Provideint(v);
}

Exporting Atanor* Atanorint::multiply(Atanor* a, bool itself) {
	if (!itself && a->isFloat()) {
		double v = value;
		return globalAtanor->Providefloat(v*a->Float());
	}

	BLONG v = value;
	v *= a->Long();

	if (IsLong(v))
		return new Atanorlong(v);


	if (itself) {
		value = v;
		return this;
	}

	return globalAtanor->Provideint(v);
}

Exporting Atanor* Atanorint::shiftleft(Atanor* a, bool itself) {
	BLONG v = value;
	v <<= a->Integer();
	if (IsLong(v))
		return new Atanorlong(v);

	if (itself) {
		value = v;
		return this;
	}


	return globalAtanor->Provideint(v);
}





