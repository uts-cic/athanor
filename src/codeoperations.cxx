
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
filename   : codeoperations.cxx
Date       : 2017/04/01
Purpose    :
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "instructions.h"
#include "atanorustring.h"
#include "atanordecimal.h"
#include "atanorvector.h"
#include "atanormap.h"
#ifndef NOFASTTYPE
#include <inttypes.h>
#endif

Atanor* Evaluatetype(uchar thetype, uchar ref, Atanor* a);

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifdef UNIX
#define swprintf_s swprintf
#endif

Atanor* AtanorPLUSPLUS::Get(Atanor* value, Atanor* variable, short idthread) {
	if (!globalAtanor->globalLOCK)
		return variable->plusplus();

	value = variable->plusplus();
	globalAtanor->Triggeronfalse(variable);
	return value;
}

Atanor* AtanorMINUSMINUS::Get(Atanor* value, Atanor* variable, short idthread) {
	if (!globalAtanor->globalLOCK)
		return variable->minusminus();

	value = variable->minusminus();
	globalAtanor->Triggeronfalse(variable);
	return value;
}

Atanor* AtanorSQUARE::Get(Atanor* value, Atanor* variable, short idthread) {
	value = variable->multiply(variable, false);
	globalAtanor->Triggeronfalse(variable);
	return value;
}

Atanor* AtanorCUBE::Get(Atanor* value, Atanor* variable, short idthread) {
	value = variable->multiply(variable, false);
	variable = value->multiply(variable, true);
	if (variable != value)
		value->Release();

	globalAtanor->Triggeronfalse(variable);
	return variable;
}


Atanor* AtanorInstructionAPPLYOPERATIONEQU::Get(Atanor* context, Atanor* value, short idthread) {
	_debuginit(idthread, this);


	_debugpush(recipient);
	Atanor* variable = value = recipient->Get(context, aNULL, idthread);
	_debugpop();

	Atanor* idx = recipient->Function();
	bool putback = false;
	bool relvar = false;

	if (idx != NULL) {
		idx = idx->Getindex();
		if (idx != recipient->Function())
			relvar = true;

		if (idx != NULL) {
			value = variable->Get(context, idx, idthread);
			if (value == aNOELEMENT) {
				if (variable->isValueContainer()) {
					value = idx->Put(variable, aNULL, idthread);
					if (value->isError()) {
						if (relvar)
							variable->Release();
						return value;
					}

					value = variable->Get(context, idx, idthread);
					putback = true;
				}
				else {
					if (relvar)
						variable->Release();
					_cleandebug;
					return globalAtanor->Returnerror("No value at this position in the container", idthread);
				}
			}
			else
			if (variable->isValueContainer())
				putback = true;
		}
	}

	//The position in the expression of our variable is the second from the bottom...
	Atanor* res;
	if (size == 1)
		res = instruction->instructions[0]->Get(context, aNULL, idthread);
	else
		res = instruction->Get(value, aNULL, idthread);

	Atanor* v;
	switch (action) {
	case a_plus:
		v = value->plus(res, true);
		break;
	case a_minus:
		v = value->minus(res, true);
		break;
	case a_multiply:
		v = value->multiply(res, true);
		break;
	case a_divide:
		v = value->divide(res, true);
		if (v->isError()) {
			value->Release();
			res->Release();
			return globalAtanor->Errorobject(idthread);
		}
		break;
	case a_power:
		v = value->power(res, true);
		break;
	case a_shiftleft:
		v = value->shiftleft(res, true);
		break;
	case a_shiftright:
		v = value->shiftright(res, true);
		break;
	case a_mod:
		v = value->mod(res, true);
		if (v->isError()) {
			value->Release();
			res->Release();
			return globalAtanor->Errorobject(idthread);
		}
		break;
	case a_or:
		v = value->orset(res, true);
		break;
	case a_xor:
		v = value->xorset(res, true);
		break;
	case a_and:
		v = value->andset(res, true);
		break;
	case a_merge:
		v = value->Merging(res);
		break;
	case a_add:
		if (!value->isVectorContainer())
			return globalAtanor->Returnerror("Cannot '::' these two elements", idthread);
		value->Merging(res);
		v = value;
	}

	if (v != value) {
		value->Put(aNULL, v, idthread);
		v->Release();
	}

	if (putback)
		idx->Put(variable, value, idthread);

	res->Release();

	if (globalAtanor->globalLOCK)
		globalAtanor->Triggeronfalse(variable);

	if (relvar)
		variable->Release();

	_cleandebug;
	if (globalAtanor->Error(idthread))
		return globalAtanor->Errorobject(idthread);

	return aTRUE;
}


Atanor* AtanorInstructionAPPLYOPERATIONROOT::ccompute(short idthread, uchar top, short& d) {
	Atanor* a = instructions.vecteur[d];
	bool itself = false;

	short act = a->Action();

	Atanor* r = aNULL;
	a = instructions.vecteur[--d];
	switch (a->Action()) {
	case a_const:
		r = a;
		break;
	case a_variable:
	case a_none:
		r = a->Get(aNULL, aNULL, idthread);
		break;
	default:
		r = ccompute(idthread, false, d);
	}

	if (top && thetype != 255)
		r = Evaluatetype(thetype, head, r);

	Atanor* v;

	//In this case, r is a novel object, which has never been used and can then be used for our own purpose...
	if (r->isProtected())
		itself = true;

	while (d > 0) {
		a = instructions.vecteur[--d];
		if (a->idtracker == 1)
			return r;

		switch (a->Action()) {
		case a_const:
			break;
		case a_variable:
		case a_none:
			a = a->Get(aNULL, aNULL, idthread);
			break;
		default:
			a = ccompute(idthread, false, d);
		}

		if (!r->Checkprecision(a)) {
			v = a->Newvalue(r, idthread);
			r->Release();
			r = v;
			itself = true;
		}

		switch (act) {
		case a_plus:
			v = r->plus(a, itself);
			break;
		case a_minus:
			v = r->minus(a, itself);
			break;
		case a_multiply:
			v = r->multiply(a, itself);
			break;
		case a_divide:
			v = r->divide(a, itself);
			if (v->isError()) {
				r->Release();
				a->Release();
				d = DIVIDEDBYZERO;
				return aRAISEERROR;
			}
			break;
		case a_power:
			v = r->power(a, itself);
			break;
		case a_shiftleft:
			v = r->shiftleft(a, itself);
			break;
		case a_shiftright:
			v = r->shiftright(a, itself);
			break;
		case a_mod:
			v = r->mod(a, itself);
			if (v->isError()) {
				r->Release();
				a->Release();
				d = DIVIDEDBYZERO;
				return aRAISEERROR;
			}
			break;
		case a_or:
			v = r->orset(a, itself);
			break;
		case a_xor:
			v = r->xorset(a, itself);
			break;
		case a_and:
			v = r->andset(a, itself);
			break;
		case a_merge:
			if (!itself && !r->isProtected())
				v = r->Atom(true)->Merging(a);
			else
				v = r->Merging(a);			
			break;
		case a_add:
			if (r->isVectorContainer())
				v = r->Merging(a);
			else {
				if (a->isVectorContainer()) {
					a->Insert(0, r);
					v = a;
					a = aNULL;
				}
				else {
					r->Release();
					a->Release();
					d = DIVIDEDBYZERO;
					return globalAtanor->Returnerror("Cannot '::' these two elements", idthread);
				}
			}
		}

		a->Release();

		itself = true;
		if (r != v) {
			r->Release();
			r = v;
		}
	}

	return r;
}

short AtanorInstructionSHORT::cshort(short idthread, short& d) {
#ifndef NOFASTTYPE
	int_fast16_t r;
#else
	short r;
#endif
	short act = instructions.vecteur[d]->Action();

	Atanor* a = instructions.vecteur[--d];
	switch (a->Action()) {
	case a_variable:
		r = a->Getshort(idthread);
		break;
	case a_const:
		r = a->Short();
		break;
	case a_none:
		a = a->Get(aNULL, aNULL, idthread);
		r = a->Short();
		a->Release();
		break;
	default:
		r = cshort(idthread, d);
	}

#ifndef NOFASTTYPE
	int_fast16_t v;
#else
	short v;
#endif

	while (d > 0) {
		a = instructions.vecteur[--d];
		if (a->idtracker == 1)
			return r;

		switch (a->Action()) {
		case a_variable:
			v = a->Getshort(idthread);
			break;
		case a_const:
			v = a->Short();
			break;
		case a_none:
			a = a->Get(aNULL, aNULL, idthread);
			v = a->Short();
			a->Release();
			break;
		default:
			v = cshort(idthread, d);
		}

		switch (act) {
		case a_plus:
			r += v;
			break;
		case a_minus:
			r -= v;
			break;
		case a_multiply:
			r *= v;
			break;
		case a_divide:
			if (v == 0) {
				globalAtanor->Returnerror("Error: Divided by 0", idthread);
				d = DIVIDEDBYZERO;
				return false;
			}
			r /= v;
			break;
		case a_power:
			r = pow((double)r, (double)v);
			break;
		case a_shiftleft:
			r <<= v;
			break;
		case a_shiftright:
			r >>= v;
			break;
		case a_mod:
			if (v == 0) {
				globalAtanor->Returnerror("Error: Divided by 0", idthread);
				d = DIVIDEDBYZERO;
				return false;
			}
			r %= v;
			break;
		case a_or:
			r |= v;
			break;
		case a_xor:
			r ^= v;
			break;
		case a_and:
			r &= v;
			break;
		}
	}

	return r;
}

long AtanorInstructionINTEGER::cinteger(short idthread, short& d) {
#ifndef NOFASTTYPE
	int_fast32_t r;
#else
	long r;
#endif
	short act = instructions.vecteur[d]->Action();

	Atanor* a = instructions.vecteur[--d];
	switch (a->Action()) {
	case a_variable:
		r = a->Getinteger(idthread);
		break;
	case a_const:
		r = a->Integer();
		break;
	case a_none:
		a = a->Get(aNULL, aNULL, idthread);
		r = a->Integer();
		a->Release();
		break;
	default:
		r = cinteger(idthread, d);
	}
#ifndef NOFASTTYPE
	int_fast32_t v;
#else
	long v;
#endif
	while (d > 0) {
		a = instructions.vecteur[--d];
		if (a->idtracker == 1)
			return r;

		switch (a->Action()) {
		case a_variable:
			v = a->Getinteger(idthread);
			break;
		case a_const:
			v = a->Integer();
			break;
		case a_none:
			a = a->Get(aNULL, aNULL, idthread);
			v = a->Integer();
			a->Release();
			break;
		default:
			v = cinteger(idthread, d);
		}

		switch (act) {
		case a_plus:
			r += v;
			break;
		case a_minus:
			r -= v;
			break;
		case a_multiply:
			r *= v;
			break;
		case a_divide:
			if (v == 0) {
				globalAtanor->Returnerror("Error: Divided by 0", idthread);
				d = DIVIDEDBYZERO;
				return false;
			}
			r /= v;
			break;
		case a_power:
			r = pow((double)r, (double)v);
			break;
		case a_shiftleft:
			r <<= v;
			break;
		case a_shiftright:
			r >>= v;
			break;
		case a_mod:
			if (v == 0) {
				globalAtanor->Returnerror("Error: Divided by 0", idthread);
				d = DIVIDEDBYZERO;
				return false;
			}
			r %= v;
			break;
		case a_or:
			r |= v;
			break;
		case a_xor:
			r ^= v;
			break;
		case a_and:
			r &= v;
			break;
		}
	}

	return r;
}

BLONG AtanorInstructionLONG::clong(short idthread, short& d) {
#ifndef NOFASTTYPE
	int_fast64_t r;
#else
	BLONG r;
#endif

	short act = instructions.vecteur[d]->Action();

	Atanor* a = instructions.vecteur[--d];
	switch (a->Action()) {
	case a_variable:
		r = a->Getlong(idthread);
		break;
	case a_const:
		r = a->Long();
		break;
	case a_none:
		a = a->Get(aNULL, aNULL, idthread);
		r = a->Long();
		a->Release();
		break;
	default:
		r = clong(idthread, d);
	}

#ifndef NOFASTTYPE
	int_fast64_t v;
#else
	BLONG r;
#endif

	while (d > 0) {
		a = instructions.vecteur[--d];
		if (a->idtracker == 1)
			return r;

		switch (a->Action()) {
		case a_variable:
			v = a->Getlong(idthread);
			break;
		case a_const:
			v = a->Long();
			break;
		case a_none:
			a = a->Get(aNULL, aNULL, idthread);
			v = a->Long();
			a->Release();
			break;
		default:
			v = clong(idthread, d);
		}

		switch (act) {
		case a_plus:
			r += v;
			break;
		case a_minus:
			r -= v;
			break;
		case a_multiply:
			r *= v;
			break;
		case a_divide:
			if (v == 0) {
				globalAtanor->Returnerror("Error: Divided by 0", idthread);
				d = DIVIDEDBYZERO;
				return false;
			}
			r /= v;
			break;
		case a_power:
			r = pow((double)r, (double)v);
			break;
		case a_shiftleft:
			r <<= v;
			break;
		case a_shiftright:
			r >>= v;
			break;
		case a_mod:
			if (v == 0) {
				globalAtanor->Returnerror("Error: Divided by 0", idthread);
				d = DIVIDEDBYZERO;
				return false;
			}
			r %= v;
			break;
		case a_or:
			r |= v;
			break;
		case a_xor:
			r ^= v;
			break;
		case a_and:
			r &= v;
			break;
		}
	}

	return r;
}

float AtanorInstructionDECIMAL::cdecimal(short idthread, short& d) {
	float r;
	short act = instructions.vecteur[d]->Action();

	float v;

	Atanor* a = instructions.vecteur[--d];
	switch (a->Action()) {
	case a_variable:
		r = a->Getdecimal(idthread);
		break;
	case a_const:
		r = a->Decimal();
		break;
	case a_none:
		a = a->Get(aNULL, aNULL, idthread);
		r = a->Decimal();
		a->Release();
		break;
	default:
		r = cdecimal(idthread, d);
	}

	while (d > 0) {
		a = instructions.vecteur[--d];
		if (a->idtracker == 1)
			return r;

		switch (a->Action()) {
		case a_variable:
			v = a->Getdecimal(idthread);
			break;
		case a_const:
			v = a->Decimal();
			break;
		case a_none:
			a = a->Get(aNULL, aNULL, idthread);
			v = a->Decimal();
			a->Release();
			break;
		default:
			v = cdecimal(idthread, d);
		}

		switch (act) {
		case a_plus:
			r += v;
			break;
		case a_minus:
			r -= v;
			break;
		case a_multiply:
			r *= v;
			break;
		case a_divide:
			if (v == 0) {
				globalAtanor->Returnerror("Error: Divided by 0", idthread);
				d = DIVIDEDBYZERO;
				return false;
			}
			r /= v;
			break;
		case a_power:
			r = pow((double)r, (double)v);
			break;
		case a_shiftleft:
			r = (long)r << (long)v;
			break;
		case a_shiftright:
			r = (long)r >> (long)v;
			break;
		case a_mod:
			if (v == 0) {
				globalAtanor->Returnerror("Error: Divided by 0", idthread);
				d = DIVIDEDBYZERO;
				return false;
			}
			r = (long)r % (long)v;
			break;
		case a_or:
			r = (long)r | (long)v;
			break;
		case a_xor:
			r = (long)r ^ (long)v;
			break;
		case a_and:
			r = (long)r & (long)v;
			break;
		}
	}

	return v;
}

double AtanorInstructionFLOAT::cfloat(short idthread, short& d) {
	double r;
	short act = instructions.vecteur[d]->Action();

	Atanor* a = instructions.vecteur[--d];
	switch (a->Action()) {
	case a_variable:
		r = a->Getfloat(idthread);
		break;
	case a_const:
		r = a->Float();
		break;
	case a_none:
		a = a->Get(aNULL, aNULL, idthread);
		r = a->Float();
		a->Release();
		break;
	default:
		r = cfloat(idthread, d);
	}

	double v;

	while (d > 0) {
		a = instructions.vecteur[--d];
		if (a->idtracker == 1)
			return r;

		switch (a->Action()) {
		case a_variable:
			v = a->Getfloat(idthread);
			break;
		case a_const:
			v = a->Float();
			break;
		case a_none:
			a = a->Get(aNULL, aNULL, idthread);
			v = a->Float();
			a->Release();
			break;
		default:
			v = cfloat(idthread, d);
		}

		switch (act) {
		case a_plus:
			r += v;
			break;
		case a_minus:
			r -= v;
			break;
		case a_multiply:
			r *= v;
			break;
		case a_divide:
			if (v == 0) {
				globalAtanor->Returnerror("Error: Divided by 0", idthread);
				d = DIVIDEDBYZERO;
				return false;
			}
			r /= v;
			break;
		case a_power:
			r = pow((double)r, (double)v);
			break;
		case a_shiftleft:
			r = (BLONG)r << (BLONG)v;
			break;
		case a_shiftright:
			r = (BLONG)r >> (BLONG)v;
			break;
		case a_mod:
			if (v == 0) {
				globalAtanor->Returnerror("Error: Divided by 0", idthread);
				d = DIVIDEDBYZERO;
				return false;
			}
			r = (BLONG)r % (BLONG)v;
			break;
		case a_or:
			r = (BLONG)r | (BLONG)v;
			break;
		case a_xor:
			r = (BLONG)r ^ (BLONG)v;
			break;
		case a_and:
			r = (BLONG)r & (BLONG)v;
			break;
		}
	}

	return r;
}

double AtanorInstructionFRACTION::cfloat(short idthread, short& d) {
	double r;
	short act = instructions.vecteur[d]->Action();

	Atanor* a = instructions.vecteur[--d];
	switch (a->Action()) {
	case a_variable:
		r = a->Getfloat(idthread);
		break;
	case a_const:
		r = a->Float();
		break;
	case a_none:
		a = a->Get(aNULL, aNULL, idthread);
		r = a->Float();
		a->Release();
		break;
	default:
		r = cfloat(idthread, d);
	}

	double v;

	while (d > 0) {
		a = instructions.vecteur[--d];
		if (a->idtracker == 1)
			return r;

		switch (a->Action()) {
		case a_variable:
			v = a->Getfloat(idthread);
			break;
		case a_const:
			v = a->Float();
			break;
		case a_none:
			a = a->Get(aNULL, aNULL, idthread);
			v = a->Float();
			a->Release();
			break;
		default:
			v = cfloat(idthread, d);
		}

		switch (act) {
		case a_plus:
			r += v;
			break;
		case a_minus:
			r -= v;
			break;
		case a_multiply:
			r *= v;
			break;
		case a_divide:
			if (v == 0) {
				globalAtanor->Returnerror("Error: Divided by 0", idthread);
				d = DIVIDEDBYZERO;
				return false;
			}
			r /= v;
			break;
		case a_power:
			r = pow((double)r, (double)v);
			break;
		case a_shiftleft:
			r = (BLONG)r << (BLONG)v;
			break;
		case a_shiftright:
			r = (BLONG)r >> (BLONG)v;
			break;
		case a_mod:
			if (v == 0) {
				globalAtanor->Returnerror("Error: Divided by 0", idthread);
				d = DIVIDEDBYZERO;
				return false;
			}
			r = (BLONG)r % (BLONG)v;
			break;
		case a_or:
			r = (BLONG)r | (BLONG)v;
			break;
		case a_xor:
			r = (BLONG)r ^ (BLONG)v;
			break;
		case a_and:
			r = (BLONG)r & (BLONG)v;
			break;
		}
	}

	return r;
}

Atanor* AtanorInstructionFRACTION::cfraction(short idthread, short& d) {
	Atanor* a = instructions.vecteur[d];
	bool itself = false;

	short act = a->Action();

	Atanor* r = aNULL;
	a = instructions.vecteur[--d];
	switch (a->Action()) {
	case a_const:
		r = a;
		break;
	case a_variable:
	case a_none:
		r = a->Get(aNULL, aNULL, idthread);
		break;
	default:
		r = cfraction(idthread, d);
	}	

	Atanor* v = (Atanor*)r->Fraction();

	if (v != r) {
		r->Release();
		r = v;
	}

	//In this case, r is a novel object, which has never been used and can then be used for our own purpose...
	if (r->isProtected())
		itself = true;

	while (d > 0) {
		a = instructions.vecteur[--d];
		if (a->idtracker == 1)
			return r;

		switch (a->Action()) {
		case a_const:
			break;
		case a_variable:
		case a_none:
			a = a->Get(aNULL, aNULL, idthread);
			break;
		default:
			a = cfraction(idthread, d);
		}

		v = (Atanor*)a->Fraction();
		if (v != a) {
			a->Release();
			a = v;
		}

		switch (act) {
		case a_plus:
			v = r->plus(a, itself);
			break;
		case a_minus:
			v = r->minus(a, itself);
			break;
		case a_multiply:
			v = r->multiply(a, itself);
			break;
		case a_divide:
			v = r->divide(a, itself);
			if (v->isError()) {
				r->Release();
				a->Release();
				d = DIVIDEDBYZERO;
				return aRAISEERROR;
			}
			break;
		case a_power:
			v = r->power(a, itself);
			break;
		case a_shiftleft:
			v = r->shiftleft(a, itself);
			break;
		case a_shiftright:
			v = r->shiftright(a, itself);
			break;
		case a_mod:
			v = r->mod(a, itself);
			if (v->isError()) {
				r->Release();
				a->Release();
				d = DIVIDEDBYZERO;
				return aRAISEERROR;
			}
			break;
		}

		a->Release();

		itself = true;
		if (r != v) {
			r->Release();
			r = (Atanor*)v->Fraction();
			if (r != v)
				v->Release();
		}
	}

	return r;
}

bool AtanorInstructionSTRING::cstring(short idthread, short& d, string& r) {
	short act = instructions.vecteur[d]->Action();

	Atanor* a = instructions.vecteur[--d];
	switch (a->Action()) {
	case a_variable:
		r = a->Getstring(idthread);
		break;
	case a_const:
		r = a->String();
		break;
	case a_none:
		a = a->Get(aNULL, aNULL, idthread);
		r = a->String();
		a->Release();
		break;
	default:
		if (!cstring(idthread, d, r))
			return false;
	}

	string v;

	while (d > 0) {
		a = instructions.vecteur[--d];
		if (a->idtracker == 1)
			return true;

		switch (a->Action()) {
		case a_variable:
			v = a->Getstring(idthread);
			break;
		case a_const:
			v = a->String();
			break;
		case a_none:
			a = a->Get(aNULL, aNULL, idthread);
			v = a->String();
			a->Release();
			break;
		default:
			if (!cstring(idthread, d, v))
				return false;
		}

		switch (act) {
		case a_plus:
			r += v;
			break;
		case a_minus:
			r = StringMinus(r, v);
			break;
		case a_or:
			r += v;
			break;
		case a_xor: {
						string u;
						short m = min(r.size(), v.size());
						for (short i = 0; i < m; i++) {
							if (r[i] != v[i])
								u += r[i];
						}
						r = u;
		}
			break;
		case a_and: {
						string u;
						short m = min(r.size(), v.size());
						for (short i = 0; i < m; i++) {
							if (r[i] == v[i])
								u += v[i];
						}
						r = u;
		}
			break;
		case a_merge:
			r += v;
			break;
		default:
			return false;
		}
	}
	return true;
}


bool AtanorInstructionUSTRING::custring(short idthread, short& d, wstring& r) {
	short act = instructions.vecteur[d]->Action();

	Atanor* a = instructions.vecteur[--d];
	switch (a->Action()) {
	case a_variable:
		r = a->Getustring(idthread);
		break;
	case a_const:
		r = a->UString();
		break;
	case a_none:
		a = a->Get(aNULL, aNULL, idthread);
		r = a->UString();
		a->Release();
		break;
	default:
		if (!custring(idthread, d, r))
			return false;
	}

	wstring v;

	while (d > 0) {
		a = instructions.vecteur[--d];
		if (a->idtracker == 1)
			return true;

		switch (a->Action()) {
		case a_variable:
			v = a->Getustring(idthread);
			break;
		case a_const:
			v = a->UString();
			break;
		case a_none:
			a = a->Get(aNULL, aNULL, idthread);
			v = a->UString();
			a->Release();
			break;
		default:
			if (!custring(idthread, d, v))
				return false;
		}

		switch (act) {
		case a_plus:
			r += v;
			break;
		case a_minus:
			r = StringMinus(r, v);
			break;
		case a_or:
			r += v;
			break;
		case a_xor: {
						wstring u;
						short m = min(r.size(), v.size());
						for (short i = 0; i < m; i++) {
							if (r[i] != v[i])
								u += r[i];
						}
						r = u;
		}
			break;
		case a_and: {
						wstring u;
						short m = min(r.size(), v.size());
						for (short i = 0; i < m; i++) {
							if (r[i] == v[i])
								u += v[i];
						}
						r = u;
		}
			break;
		case a_merge:
			r += v;
			break;
		default:
			return false;
		}
	}
	return true;
}


Atanor* AtanorInstructionAPPLYOPERATION::Get(Atanor* res, Atanor* inter, short idthread) {
	//This is an expression that escaped the compiler...
	if (root == NULL)
		//we create it on the fly...
		root = Compile(NULL);

	return root->Get(res, inter, idthread);
}


