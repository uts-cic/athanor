
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
filename   : atanordecimal.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef Atanordecimal_h
#define Atanordecimal_h

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanordecimal;
//This typedef defines a type "floatMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanordecimal::*decimalMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanordecimal : public AtanorObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging hmap<unsigned short, decimalMethod> methods;
	static Exchanging hmap<string, string> infomethods;
	static Exchanging bin_hash<unsigned long> exported;

	static Exchanging short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	float value;

	//---------------------------------------------------------------------------------------------------------------------
	Atanordecimal(float v, AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
		//Do not forget your variable initialisation
		value = v;
	}

	Atanordecimal(float v) {
		//Do not forget your variable initialisation
		value = v;
	}

	//----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* index, Atanor* v, short idthread) {
		value = v->Decimal();
		return this;
	}

	Atanor* Putvalue(Atanor* v, short idthread) {
		value = v->Decimal();
		return this;
	}


	Atanor* Get(Atanor* context, Atanor* value, short idthread) {

		return this;
	}

	bool Checkprecision(Atanor* r) {
		if (r->Type() == a_float)
			return false;
		return true;
	}


	short Type() {
		return Atanordecimal::idtype;
	}

	string Typename() {
		return "float";
	}

	bool isAtom() {
		return true;
	}

	bool isNumber() {
		return true;
	}

	bool isDecimal() {
		return true;
	}

	bool isFloat() {
		return true;
	}

	Atanor* Atom(bool forced = false) {
		if (forced || !protect || reference)
			return new  Atanordecimal(value);
		return this;
	}

	void storevalue(short u) {
		value = (short)u;
	}

	void storevalue(long u) {
		value = (float)u;
	}

	void storevalue(float u) {
		value = u;
	}

	void storevalue(double u) {
		value = (float)u;
	}

	void storevalue(unsigned char u) {
		value = (float)u;
	}

	void storevalue(string u) {
		value = convertfloat(u);
	}

	void storevalue(wstring u) {
		value = convertfloat(u);
	}

	void storevalue(wchar_t u) {
		wstring w;
		w = u;
		value = convertfloat(w);
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
		return new Atanordecimal(0);
	}

	Atanor* Newvalue(Atanor* a, short idthread) {
		return new Atanordecimal(a->Decimal());
	}

	AtanorIteration* Newiteration(bool direction) {
		return aITERNULL;
	}

	static void AddMethod(AtanorGlobal* g, string name, decimalMethod func, unsigned long arity, string infos, short typereturn);
	
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



	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.

	Atanor* Methodchr(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

	Atanor* MethodInvert(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return Invert(true);
	}


	Atanor* Methodeven(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		long v = value;
		if ((v % 2) == 0)
			return aTRUE;
		return aFALSE;
	}

	Atanor* Methododd(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		long v = value;
		if ((v % 2) == 0)
			return aFALSE;
		return aTRUE;
	}

	Atanor* Methodabs(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(fabs(value));
	}

	Atanor* Methodacos(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(acos(value));
	}

	Atanor* Methodacosh(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(acosh(value));
	}

	Atanor* Methodasin(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(asin(value));
	}

	Atanor* Methodasinh(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(asinh(value));
	}

	Atanor* Methodatan(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(atan(value));
	}

	Atanor* Methodatanh(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(atanh(value));
	}

	Atanor* Methodcbrt(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(cbrt(value));
	}

	Atanor* Methodcos(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(cos(value));
	}

	Atanor* Methodcosh(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(cosh(value));
	}

	Atanor* Methoderf(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(erf(value));
	}

	Atanor* Methoderfc(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(erfc(value));
	}

	Atanor* Methodexp(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(exp(value));
	}

	Atanor* Methodexp2(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(exp2(value));
	}

	Atanor* Methodexpm1(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(expm1(value));
	}

	Atanor* Methodfloor(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(floor(value));
	}

	Atanor* Methodlgamma(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(lgamma(value));
	}

	Atanor* Methodln(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(log(value));
	}

	Atanor* Methodlog(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(log10(value));
	}

	Atanor* Methodlog1p(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(log1p(value));
	}

	Atanor* Methodlog2(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(log2(value));
	}

	Atanor* Methodlogb(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(logb(value));
	}

	Atanor* Methodnearbyint(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(nearbyint(value));
	}

	Atanor* Methodrint(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(rint(value));
	}

	Atanor* Methodround(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(round(value));
	}

	Atanor* Methodsin(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(sin(value));
	}

	Atanor* Methodsinh(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(sinh(value));
	}

	Atanor* Methodsqrt(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(sqrt(value));
	}

	Atanor* Methodtan(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(tan(value));
	}

	Atanor* Methodtanh(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(tanh(value));
	}

	Atanor* Methodtgamma(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(tgamma(value));
	}

	Atanor* Methodtrunc(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return new Atanordecimal(trunc(value));
	}

	Atanor* MethodFormat(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodDegree(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodRadian(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

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

	wstring UString() {
		std::wstringstream s;
		s << value;
		return s.str();
	}

	string String() {
		stringstream s;
		s << value;
		return s.str();
	}

	long Integer() {
		return (long)value;
	}

	double Float() {
		return value;
	}

	float Decimal() {
		return value;
	}

	BLONG Long() {
		return (BLONG)value;
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
		return value;
	}

	float Getdecimal(short idthread) {
		return value;
	}

	BLONG Getlong(short idthread) {
		return (BLONG)value;
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

	//Basic operations
	virtual long Size() {
		return sizeof(value);
	}

	virtual Atanor* plusplus() {
		value++;
		return this;
	}

	virtual Atanor* minusminus() {
		value--;
		return this;
	}


	Atanor* andset(Atanor* a, bool itself) {
		if (itself) {
			value = (long)value & a->Integer();
			return this;
		}

		return new Atanordecimal((long)value & a->Integer());
	}

	Atanor* orset(Atanor* a, bool itself) {
		if (itself) {
			value = (long)value | a->Integer();
			return this;
		}
		return new Atanordecimal((long)value | a->Integer());
	}

	Atanor* xorset(Atanor* a, bool itself) {
		if (itself) {
			value = (long)value ^ a->Integer();
			return this;
		}
		return new Atanordecimal((long)value ^ a->Integer());
	}


	virtual Atanor* plus(Atanor* a, bool itself) {
		if (itself) {
			value += a->Decimal();
			return this;
		}
		return new Atanordecimal(value + a->Decimal());
	}

	virtual Atanor* minus(Atanor* a, bool itself) {
		if (itself) {
			value -= a->Decimal();
			return this;
		}
		return new Atanordecimal(value - a->Decimal());
	}

	Atanor* multiply(Atanor* a, bool itself) {
		if (itself) {
			value *= a->Decimal();
			return this;
		}
		return new Atanordecimal(value * a->Decimal());
	}

	Atanor* divide(Atanor* a, bool itself) {
		float v = a->Decimal();
		if (v == 0)
			return new AtanorError("Error: Divided by 0");
		if (itself) {
			value /= v;
			return this;
		}

		v = value / v;
		return new Atanordecimal(v);
	}

	Atanor* power(Atanor* a, bool itself) {		
		if (itself) {
			value = pow(value, a->Decimal());
			return this;
		}

		double v = pow(value, a->Decimal());
		return new Atanordecimal(v);
	}

	Atanor* shiftleft(Atanor* a, bool itself) {
		if (itself) {
			value = (long)value << a->Integer();
			return this;
		}
		return new Atanordecimal((long)value << a->Integer());
	}

	Atanor* shiftright(Atanor* a, bool itself) {
		if (itself) {
			value = (long)value >> a->Integer();
			return this;
		}
		return new Atanordecimal((long)value >> a->Integer());
	}

	Atanor* mod(Atanor* a, bool itself) {
		long v = a->Integer();
		if (v == 0)
			return new AtanorError("Error: Divided by 0");
		if (itself) {
			value = (long)value % v;
			return this;
		}

		v = (long)value % v;
		return new Atanordecimal(v);
	}

	Atanor* less(Atanor* a) {
		if (value < a->Decimal())
			return aTRUE;
		return aFALSE;
	}

	Atanor* more(Atanor* a) {
		if (value > a->Decimal())
			return aTRUE;
		return aFALSE;
	}

	Atanor* same(Atanor* a) {
		if (value == a->Decimal())
			return aTRUE;
		return aFALSE;
	}

	Atanor* different(Atanor* a) {
		if (value != a->Decimal())
			return aTRUE;
		return aFALSE;
	}

	Atanor* lessequal(Atanor* a) {
		if (value <= a->Decimal())
			return aTRUE;
		return aFALSE;
	}

	Atanor* moreequal(Atanor* a) {
		if (value >= a->Decimal())
			return aTRUE;
		return aFALSE;
	}
};

class AtanorLoopDecimal : public Atanordecimal {
public:

	vector<float> interval;
	long position;
	Atanor* function;

	AtanorLoopDecimal(AtanorGlobal* g) : Atanordecimal(0) {

		position = 0;
		function = NULL;
	}

	void Addfunction(Atanor* f) {
		function = f;
	}

	void Callfunction();

	short Type() {
		return a_dloop;
	}

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
		AtanorLoopDecimal* a= new AtanorLoopDecimal(globalAtanor);
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

		return Atanordecimal::plus(b, autoself);
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

		return Atanordecimal::minus(b, autoself);
	}
};

#endif








