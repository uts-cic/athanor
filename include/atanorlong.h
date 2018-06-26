
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
filename   : atanorlong.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanorlong_h
#define atanorlong_h

#include "atanorfloat.h"
#include "atanorstring.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorlong;
//This typedef defines a type "longMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorlong::*longMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanorlong : public AtanorObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging hmap<unsigned short, longMethod> methods;
	static Exchanging hmap<string, string> infomethods;
	static Exchanging bin_hash<unsigned long> exported;

	static Exchanging short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	BLONG value;

	//---------------------------------------------------------------------------------------------------------------------
	Atanorlong(BLONG v, AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
		//Do not forget your variable initialisation
		value = v;
	}

	Atanorlong(BLONG v) {
		//Do not forget your variable initialisation
		value = v;
	}

	//----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* index, Atanor* v, short idthread) {
		value = v->Long();
		return this;
	}

	Atanor* Putvalue(Atanor* v, short idthread) {
		value = v->Long();
		return this;
	}


	Atanor* Get(Atanor* context, Atanor* v, short idthread) {

		return this;
	}

	bool Checkprecision(Atanor* r) {
		if (r->Type() >= a_float)
			return false;
		return true;
	}

	short Type() {
		return Atanorlong::idtype;
	}

	string Typename() {
		return "long";
	}

	bool isInteger() {
		return true;
	}

	bool isAtom() {
		return true;
	}

	short Typenumber() {
		return a_long;
	}

	bool isNumber() {
		return true;
	}

	bool isLong() {
		return true;
	}

	Atanor* Atom(bool forced = false) {
		if (forced || !protect || reference)
			return new  Atanorlong(value);
		return this;
	}

	string Bytes() {
		u_kiflong x(value);
		unsigned char buff[u_kiflongsz + 1];
		memcpy(buff, x.t, u_kiflongsz);
		buff[u_kiflongsz] = 0;
		return (char*)buff;
	}

	void storevalue(short u) {
		value = u;
	}

	void storevalue(long u) {
		value = u;
	}

	void storevalue(BLONG u) {
		value = u;
	}

	void storevalue(double u) {
		value = (BLONG)u;
	}

	void storevalue(unsigned char u) {
		value = (BLONG)u;
	}

	void storevalue(string u) {
		value = convertinteger(u);
	}

	void storevalue(wstring u) {
		value = convertinteger(u);
	}

	void storevalue(wchar_t u) {
		wstring w;
		w = u;
		value = convertinteger(w);
	}

	//---------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	bool isDeclared(short n) {
		if (exported.find(n) != exported.end())
			return true;
		return false;
	}

	Atanor* Newinstance(short, Atanor* f = NULL) {
		return new Atanorlong(0);
	}

	Atanor* Newvalue(Atanor* a, short idthread) {
		return new Atanorlong(a->Long());
	}


	AtanorIteration* Newiteration(bool direction) {
		return aITERNULL;
	}

	static void AddMethod(AtanorGlobal* g, string name, longMethod func, unsigned long arity, string infos);
	
	static bool InitialisationModule(AtanorGlobal* g, string version);

	
	void Methods(Atanor* v) {

		for (auto& it : infomethods)
			v->storevalue(it.first);
	}

	string Info(string n) {

		if (infomethods.find(n) != infomethods.end())
			return infomethods[n];
		return "Unknown method";
	}


	Atanor* Succ() {
		return new Atanorlong(value + 1);
	}

	Atanor* Pred() {
		return new Atanorlong(value - 1);
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	Atanor* Methodchr(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

	Atanor* MethodSucc(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanorlong(value + 1);
	}

	Atanor* MethodPred(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanorlong(value - 1);
	}

	Atanor* MethodPrimefactors(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		Atanor* kvect = Selectaivector(contextualpattern);
		BLONG n = value;
		long dec = 2;
		BLONG ni = n;
		while (n > 1 && (dec*dec) <= ni) {
			if (n%dec == 0) {
				kvect->storevalue(dec);
				n /= dec;
			}
			else
				dec++;
		}

		if (n > 1)
			kvect->storevalue((long)n);
		return kvect;
	}

	Atanor* MethodPrime(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		BLONG n = value;
		if (!n)
			return aFALSE;
		if (n == 2 || n == 3 || n == 5 || n == 7 || n == 11)
			return aTRUE;

		if (!n || (n % 2) == 0 || (n % 3) == 0 || (n % 5) == 0 || (n % 7) == 0 || (n % 11) == 0)
			return aFALSE;

		BLONG dec = 13;
		BLONG ni = n;

		while (n > 1 && (dec*dec) <= ni) {
			if (n%dec == 0)
				return aFALSE;
			dec += 4;
			if (n%dec == 0)
				return aFALSE;
			dec += 2;
			if (n%dec == 0)
				return aFALSE;
			dec += 2;
			if (n%dec == 0)
				return aFALSE;
			dec += 2;
		}

		return aTRUE;
	}

	Atanor* Methodeven(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		if ((value % 2) == 0)
			return aTRUE;
		return aFALSE;
	}

	Atanor* Methododd(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		if ((value % 2) == 0)
			return aFALSE;
		return aTRUE;
	}


	Atanor* MethodFormat(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

	Atanor* Methodabs(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(fabs(v));
	}

	Atanor* Methodacos(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(acos(v));
	}

	Atanor* Methodacosh(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(acosh(v));
	}

	Atanor* Methodasin(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(asin(v));
	}

	Atanor* Methodasinh(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(asinh(v));
	}

	Atanor* Methodatan(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(atan(v));
	}

	Atanor* Methodatanh(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(atanh(v));
	}

	Atanor* Methodcbrt(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(cbrt(v));
	}

	Atanor* Methodcos(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(cos(v));
	}

	Atanor* Methodcosh(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(cosh(v));
	}

	Atanor* Methoderf(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(erf(v));
	}

	Atanor* Methoderfc(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(erfc(v));
	}

	Atanor* Methodexp(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(exp(v));
	}

	Atanor* Methodexp2(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(exp2(v));
	}

	Atanor* Methodexpm1(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(expm1(v));
	}

	Atanor* Methodfloor(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(floor(v));
	}

	Atanor* Methodlgamma(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(lgamma(v));
	}

	Atanor* Methodln(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(log(v));
	}

	Atanor* Methodlog(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(log10(v));
	}

	Atanor* Methodlog1p(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(log1p(v));
	}

	Atanor* Methodlog2(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(log2(v));
	}

	Atanor* Methodlogb(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(logb(v));
	}

	Atanor* Methodnearbyint(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(nearbyint(v));
	}

	Atanor* Methodrint(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(rint(v));
	}

	Atanor* Methodround(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(round(v));
	}

	Atanor* Methodsin(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(sin(v));
	}

	Atanor* Methodsinh(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(sinh(v));
	}

	Atanor* Methodsqrt(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(sqrt(v));
	}

	Atanor* Methodtan(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(tan(v));
	}

	Atanor* Methodtanh(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(tanh(v));
	}

	Atanor* Methodtgamma(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(tgamma(v));
	}

	Atanor* Methodtrunc(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		double v = (double)value;
		return globalAtanor->Providefloat(trunc(v));
	}

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
	//is returned by the Declaration method.
	Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		return (this->*methods[idname])(contextualpattern, idthread, callfunc);
	}

	void Clear() {
		//To set a variable back to empty
		value = 0;
	}

	void Clean() {
		//Your cleaning code
	}


	string String() {
		stringstream v;
		v << value;
		return v.str();
	}

	long Integer() {
		return (long)value;
	}

	long Size() {
		return sizeof(value);
	}

	Atanor* plusplus() {
		value++;
		return this;
	}

	Atanor* minusminus() {
		value--;
		return this;
	}

	wstring Getustring(short idthread) {
		std::wstringstream s;
		s << value;
		return s.str();
	}

	string Getstring(short idthread) {
		stringstream s;
		s << value;
		return s.str();
	}

	long Getinteger(short idthread) {
		return (long)value;
	}

	double Getfloat(short idthread) {
		return (double)value;
	}

	float Getdecimal(short idthread) {
		return (float)value;
	}

	BLONG Getlong(short idthread) {
		return (BLONG)value;
	}

	wstring UString() {
		std::wstringstream s;
		s << value;
		return s.str();
	}

	float Decimal() {
		return (float)value;
	}

	double Float() {
		return (double)value;
	}

	bool Boolean() {
		if (value == 0)
			return false;
		return true;
	}

	bool Protectedboolean() {
		Locking _lock(this);
		if (value == 0)
			return false;
		return true;
	}

	BLONG Long() {
		return value;
	}

	//we add the current value with a
	Atanor* andset(Atanor* a, bool itself) {
		if (itself) {
			value &= a->Long();
			return this;
		}
		return new Atanorlong(value & a->Long());
	}

	Atanor* orset(Atanor* a, bool itself) {
		if (itself) {
			value |= a->Long();
			return this;
		}
		return new Atanorlong(value | a->Long());
	}

	Atanor* xorset(Atanor* a, bool itself) {
		if (itself) {
			value ^= a->Long();
			return this;
		}
		return new Atanorlong(value ^ a->Long());
	}


	Atanor* plus(Atanor* a, bool itself) {
		if (itself) {
			value += a->Long();
			return this;
		}

		if (a->isFloat()) {
			double v = value;
			return globalAtanor->Providefloat(v + a->Float());
		}

		return new Atanorlong(value + a->Long());
	}

	Atanor* minus(Atanor* a, bool itself) {
		if (itself) {
			value -= a->Long();
			return this;
		}
		return new Atanorlong(value - a->Long());
	}

	Atanor* multiply(Atanor* a, bool itself) {
		if (itself) {
			value *= a->Long();
			return this;
		}
		
		if (a->isFloat()) {
			double v = value;
			return globalAtanor->Providefloat(v * a->Float());
		}

		return new Atanorlong(value * a->Long());
	}

	Atanor* divide(Atanor* a, bool itself) {
		double v = a->Float();
		if (v == 0)
			return new AtanorError("Error: Divided by 0");
		v = (double)value / v;
		
		if (itself) {
			value = v;
			return this;
		}

		return globalAtanor->Providefloat(v);
	}

	Atanor* power(Atanor* a, bool itself) {
		double v = (double)value;
		v = pow(v, a->Float());
		if (itself) {
			value = v;
			return this;
		}
		return globalAtanor->Providefloat(v);
	}

	Atanor* shiftleft(Atanor* a, bool itself) {
		if (itself) {
			value <<= a->Long();
			return this;
		}
		return new Atanorlong(value << a->Long());
	}

	Atanor* shiftright(Atanor* a, bool itself) {
		if (itself) {
			value >>= a->Long();
			return this;
		}
		return new Atanorlong(value >> a->Long());
	}

	Atanor* mod(Atanor* a, bool itself) {
		BLONG v = a->Long();
		if (v == 0)
			return new AtanorError("Error: Divided by 0");
		if (itself) {
			value %= v;
			return this;
		}

		v = value % v;
		return new Atanorlong(v);
	}

	Atanor* less(Atanor* a) {
		if (value < a->Long())
			return aTRUE;
		return aFALSE;
	}

	Atanor* more(Atanor* a) {
		if (value > a->Long())
			return aTRUE;
		return aFALSE;
	}

	Atanor* same(Atanor* a) {
		if (value == a->Long())
			return aTRUE;
		return aFALSE;
	}

	Atanor* different(Atanor* a) {
		if (value != a->Long())
			return aTRUE;
		return aFALSE;
	}

	Atanor* lessequal(Atanor* a) {
		if (value <= a->Long())
			return aTRUE;
		return aFALSE;
	}

	Atanor* moreequal(Atanor* a) {
		if (value >= a->Long())
			return aTRUE;
		return aFALSE;
	}
};


class AtanorLoopLong : public Atanorlong {
public:

	vector<BLONG> interval;
	long position;
	Atanor* function;

	AtanorLoopLong(AtanorGlobal* g) : Atanorlong(0) {

		position = 0;
		function = NULL;
	}

	short Type() {
		return a_lloop;
	}


	void Addfunction(Atanor* f) {
		function = f;
	}

	void Callfunction();


	Atanor* Put(Atanor*, Atanor*, short);
	Atanor* Vector(short idthread);

	long Size() {
		return interval.size();
	}

	void Next() {
		if (interval.size() == 0)
			return;

		position++;
		if (position >= interval.size()) {
			if (function != NULL)
				Callfunction();
			position = 0;
		}
		value = interval[position];
	}

	Atanor* plusplus() {
		if (interval.size() == 0)
			return this;
		position++;
		if (position >= interval.size()) {
			if (function != NULL)
				Callfunction();
			position = 0;
		}
		value = interval[position];
		return this;
	}

	Atanor* minusminus() {
		if (interval.size() == 0)
			return this;
		position--;
		if (position < 0) {
			if (function != NULL)
				Callfunction();
			position = interval.size() - 1;
		}
		value = interval[position];
		return this;
	}


	Atanor* Newinstance(short idthread, Atanor* f = NULL) {
		AtanorLoopLong* a=new AtanorLoopLong(globalAtanor);
		a->function = f;
		return a;
	}

	Atanor* plus(Atanor* b, bool autoself) {
		if (interval.size() == 0)
			return this;

		if (autoself) {
			position += b->Integer();

			position = abs(position) % interval.size();

			value = interval[position];
			return this;
		}

		return Atanorlong::plus(b, autoself);
	}

	Atanor* minus(Atanor* b, bool autoself) {
		if (interval.size() == 0)
			return this;

		if (autoself) {
			position -= b->Integer();

			position = (interval.size() - abs(position)) % interval.size();

			value = interval[position];
			return this;
		}

		return Atanorlong::minus(b, autoself);
	}
};

#endif









