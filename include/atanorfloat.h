
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
filename   : atanorfloat.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanorfloat_h
#define atanorfloat_h

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorfloat;
//This typedef defines a type "floatMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorfloat::*floatMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanorfloat : public AtanorObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging hmap<unsigned short, floatMethod> methods;
	static Exchanging hmap<string, string> infomethods;
	static Exchanging bin_hash<unsigned long> exported;

	static Exchanging short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	double value;

	//---------------------------------------------------------------------------------------------------------------------
	Atanorfloat(double v, AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
		//Do not forget your variable initialisation
		value = v;
	}

	Atanorfloat(double v) {
		//Do not forget your variable initialisation
		value = v;
	}

	//----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* index, Atanor* v, short idthread) {
		value = v->Float();
		return this;
	}

	Atanor* Putvalue(Atanor* v, short idthread) {
		value = v->Float();
		return this;
	}

	Atanor* Get(Atanor* context, Atanor* value, short idthread) {

		return this;
	}


	short Type() {
		return Atanorfloat::idtype;
	}

	string Typename() {
		return "float";
	}

	bool isAtom() {
		return true;
	}

	short Typenumber() {
		return a_float;
	}

	bool isNumber() {
		return true;
	}

	bool isFloat() {
		return true;
	}

	Atanor* Atom(bool forced = false) {
		if (forced || !protect || reference)
			return globalAtanor->Providefloat(value);
		return this;
	}

	void storevalue(short u) {
		value = (short)u;
	}

	void storevalue(long u) {
		value = (double)u;
	}

	void storevalue(BLONG u) {
		value = (double)u;
	}
	
	void storevalue(double u) {
		value = u;
	}

	void storevalue(unsigned char u) {
		value = (double)u;
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

	Atanor* Succ() {
		return globalAtanor->Providefloat(value + 1);
	}

	Atanor* Pred() {
		return globalAtanor->Providefloat(value - 1);
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
		return globalAtanor->Providefloat(0);
	}

	Atanor* Newvalue(Atanor* a, short idthread) {
		return globalAtanor->Providefloat(a->Float());
	}

	AtanorIteration* Newiteration(bool direction) {
		return aITERNULL;
	}

	static void AddMethod(AtanorGlobal* g, string name, floatMethod func, unsigned long arity, string infos);
	
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

	Atanor* MethodSucc(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(value + 1);
	}

	Atanor* MethodPred(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(value - 1);
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
		return globalAtanor->Providefloat(fabs(value));
	}

	Atanor* Methodacos(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(acos(value));
	}

	Atanor* Methodacosh(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(acosh(value));
	}

	Atanor* Methodasin(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(asin(value));
	}

	Atanor* Methodasinh(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(asinh(value));
	}

	Atanor* Methodatan(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(atan(value));
	}

	Atanor* Methodatanh(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(atanh(value));
	}

	Atanor* Methodcbrt(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(cbrt(value));
	}

	Atanor* Methodcos(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(cos(value));
	}

	Atanor* Methodcosh(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(cosh(value));
	}

	Atanor* Methoderf(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(erf(value));
	}

	Atanor* Methoderfc(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(erfc(value));
	}

	Atanor* Methodexp(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(exp(value));
	}

	Atanor* Methodexp2(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(exp2(value));
	}

	Atanor* Methodexpm1(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(expm1(value));
	}

	Atanor* Methodfloor(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(floor(value));
	}

	Atanor* Methodlgamma(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(lgamma(value));
	}

	Atanor* Methodln(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(log(value));
	}

	Atanor* Methodlog(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(log10(value));
	}

	Atanor* Methodlog1p(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(log1p(value));
	}

	Atanor* Methodlog2(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(log2(value));
	}

	Atanor* Methodlogb(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(logb(value));
	}

	Atanor* Methodnearbyint(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(nearbyint(value));
	}

	Atanor* Methodrint(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(rint(value));
	}

	Atanor* Methodround(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(round(value));
	}

	Atanor* Methodsin(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(sin(value));
	}

	Atanor* Methodsinh(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(sinh(value));
	}

	Atanor* Methodsqrt(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(sqrt(value));
	}

	Atanor* Methodtan(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(tan(value));
	}

	Atanor* Methodtanh(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(tanh(value));
	}

	Atanor* Methodtgamma(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(tgamma(value));
	}

	Atanor* Methodtrunc(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providefloat(trunc(value));
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

	float Decimal() {
		return (float)value;
	}

	double Float() {
		return value;
	}

	BLONG Long() {
		return (long)value;
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

		return globalAtanor->Providefloat((long)value & a->Integer());
	}

	Atanor* orset(Atanor* a, bool itself) {
		if (itself) {
			value = (long)value | a->Integer();
			return this;
		}
		return globalAtanor->Providefloat((long)value | a->Integer());
	}

	Atanor* xorset(Atanor* a, bool itself) {
		if (itself) {
			value = (long)value ^ a->Integer();
			return this;
		}
		return globalAtanor->Providefloat((long)value ^ a->Integer());
	}


	virtual Atanor* plus(Atanor* a, bool itself) {
		if (itself) {
			value += a->Float();
			return this;
		}
		return globalAtanor->Providefloat(value + a->Float());
	}

	virtual Atanor* minus(Atanor* a, bool itself) {
		if (itself) {
			value -= a->Float();
			return this;
		}
		return globalAtanor->Providefloat(value - a->Float());
	}

	Atanor* multiply(Atanor* a, bool itself) {
		if (itself) {
			value *= a->Float();
			return this;
		}
		return globalAtanor->Providefloat(value * a->Float());
	}

	Atanor* divide(Atanor* a, bool itself) {
		double v = a->Float();
		if (v == 0)
			return new AtanorError("Error: Divided by 0");
		if (itself) {
			value /= v;
			return this;
		}

		v = value / v;
		return globalAtanor->Providefloat(v);
	}

	Atanor* power(Atanor* a, bool itself) {		
		if (itself) {
			value = pow(value, a->Float());
			return this;
		}

		double v = pow(value, a->Float());
		return globalAtanor->Providefloat(v);
	}

	Atanor* shiftleft(Atanor* a, bool itself) {
		if (itself) {
			value = (long)value << a->Integer();
			return this;
		}
		return globalAtanor->Providefloat((long)value << a->Integer());
	}

	Atanor* shiftright(Atanor* a, bool itself) {
		if (itself) {
			value = (long)value >> a->Integer();
			return this;
		}
		return globalAtanor->Providefloat((long)value >> a->Integer());
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
		return globalAtanor->Providefloat(v);
	}

	Atanor* less(Atanor* a) {
		if (value < a->Float())
			return aTRUE;
		return aFALSE;
	}

	Atanor* more(Atanor* a) {
		if (value > a->Float())
			return aTRUE;
		return aFALSE;
	}

	Atanor* same(Atanor* a) {
		if (value == a->Float())
			return aTRUE;
		return aFALSE;
	}

	Atanor* different(Atanor* a) {
		if (value != a->Float())
			return aTRUE;
		return aFALSE;
	}

	Atanor* lessequal(Atanor* a) {
		if (value <= a->Float())
			return aTRUE;
		return aFALSE;
	}

	Atanor* moreequal(Atanor* a) {
		if (value >= a->Float())
			return aTRUE;
		return aFALSE;
	}
};

//---------------------------------------------------------------------------------------------------------------------
class Atanorfloatbuff : public Atanorfloat {
public:
	long idx;
	bool used;

	Atanorfloatbuff(long i) : Atanorfloat(0) {
		//Do not forget your variable initialisation
		idx = i;
		used = false;
	}

	void Setreference(short r) {
		Locking _lock(this);
		reference += r;
		protect = false;		
	}

	void Resetreference(short r) {
		bool clr = false;
		{
			Locking _lock(this);
			reference -= r;
			if (reference <= 0) {
				if (!protect) {
					value = 0;
					reference = 0;
					clr = true;
					protect = true;
				}
			}
		}

		if (clr) {
			Locking _lock(globalAtanor->floatlock);
			used = false;
			globalAtanor->fempties.push_back(idx);
		}
	}
};

class AtanorLoopFloat : public Atanorfloat {
public:
	
	vector<double> interval;
	long position;
	Atanor* function;

	AtanorLoopFloat(AtanorGlobal* g) : Atanorfloat(0) {
		
		position = 0;
		function = NULL;
	}

	void Addfunction(Atanor* f) {
		function = f;
	}

	void Callfunction();

	short Type() {
		return a_floop;
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
		AtanorLoopFloat* a = new AtanorLoopFloat(globalAtanor);
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

		return Atanorfloat::plus(b, autoself);
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

		return Atanorfloat::minus(b, autoself);
	}
};

#endif








