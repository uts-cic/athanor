
/*
* Xerox Research Centre Europe - Grenoble Laboratory (XRCE)
*
* ATHANOR Language
* This file can only be used with the ATANOR library or the executable
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : ATHANOR
Version    : ATANOR 0.26
filename   : atanorfraction.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX (claude.roux@xrce.xerox.com)
Reviewer   :
*/

#ifndef atanorfraction_h
#define atanorfraction_h
#include "atanorint.h"
#include "atanorlong.h"
//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorfraction;
//This typedef defines a type "fractionMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorfraction::*fractionMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanorfraction : public AtanorObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging hmap<unsigned short, fractionMethod> methods;
	static Exchanging hmap<string, string> infomethods;
	static Exchanging bin_hash<unsigned long> exported;

	static Exchanging short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	BLONG numerator;
	BLONG denominator;

	//---------------------------------------------------------------------------------------------------------------------
	Atanorfraction(AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
		//Do not forget your variable initialisation
		numerator = 0;
		denominator = 1;
	}

	Atanorfraction(BLONG n, BLONG d) {
		//Do not forget your variable initialisation
		numerator = n;
		denominator = d;
	}

	Atanorfraction(double n) {
		Evaluatefraction(n);
	}

	//----------------------------------------------------------------------------------------------------------------------
	Atanor* Putvalue(Atanor* v, short idthread) {
		if (v->Type() == idtype) {
			Atanorfraction* val = (Atanorfraction*)v;
			numerator = val->numerator;
			denominator = val->denominator;
		}
		else
			Evaluatefraction(v->Float());
		return aTRUE;
	}

	Atanor* Put(Atanor* idx, Atanor* v, short idthread) {
		if (v->Type() == idtype) {
			Atanorfraction* val = (Atanorfraction*)v;
			numerator = val->numerator;
			denominator = val->denominator;
		}
		else
			Evaluatefraction(v->Float());
		return aTRUE;
	}

	Atanor* Invert(bool autoself) {
		if (numerator == 0)
			return new AtanorError("Cannot invert, numerator is 0");
		
		if (autoself)  {
			ND(denominator, numerator);
			return this;
		}

		return new Atanorfraction(denominator, numerator);
	}

	Atanor* Get(Atanor* context, Atanor* v, short idthread) {
		return this;
	}


	short Type() {
		return Atanorfraction::idtype;
	}

	string Typename() {
		return "fraction";
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
			return new  Atanorfraction(numerator, denominator);
		return this;
	}


	//---------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in aexportedmethods... See MethodInitialization below
	bool isDeclared(short n) {
		if (exported.find(n) != exported.end())
			return true;
		return false;
	}

	Atanor* Newinstance(short, Atanor* f = NULL) {
		return new Atanorfraction(0, 1);
	}

	AtanorIteration* Newiteration(bool direction) {
		return aITERNULL;
	}

	static void AddMethod(AtanorGlobal* g, string name, fractionMethod func, unsigned long arity, string infos);
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
	Atanor* MethodInitial(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		if (callfunc->Size() == 1)
			Evaluatefraction(callfunc->Evaluate(0, aNULL, idthread)->Float());
		else {
			numerator = callfunc->Evaluate(0, aNULL, idthread)->Long();
			denominator = callfunc->Evaluate(1, aNULL, idthread)->Long();
			if (denominator == 0)
				return globalAtanor->Returnerror("Denominator cannot be 0", idthread);
			Simplify();
		}
		return this;
	}

	Atanor* MethodSimplify(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		Simplify();
		return this;
	}

	//---------------------------------------------------------------------------------------------------------------------


	BLONG N() {
		return numerator;
	}

	BLONG D() {
		return denominator;
	}

	void ND(BLONG n, BLONG d) {
		numerator = n;
		denominator = d;
	}

	//---------------------------------------------------------------------------------------------------------------------
	BLONG euclidian(BLONG a, BLONG b) {
		if (b == 0)
			return 0;

		if (a < b)
			return 1;

		BLONG remain = a%b;
		if (remain == 0)
			return b;
		return euclidian(b, remain);
	}

	BLONG pureeuclidian(BLONG a, BLONG b) {
		if (b == 0)
			return 0;
		if (a<b)
			return 1;
		BLONG remain = a%b;
		if (remain == 0)
			return b;
		return pureeuclidian(b, remain);
	}

	BLONG computeEuclidian(BLONG a, BLONG b) {
		if (a>b)
			return pureeuclidian(a, b);
		return pureeuclidian(b, a);
	}

	bool Simplify() {
		if (denominator == 0)
			return false;

		if (denominator == 1 || numerator == 0)
			return true;

		if (denominator == numerator) {
			denominator = 1;
			numerator = 1;
			return true;
		}


		BLONG res;
		bool nneg = false;
		bool dneg = false;
		if (numerator < 0) {
			numerator *= -1;
			if (denominator < 0)
				denominator *= -1;
			else
				nneg = true;
		}
		else {
			if (denominator < 0) {
				denominator *= -1;
				dneg = true;
			}
		}

		if (denominator>numerator)
			res = euclidian(denominator, numerator);
		else
			res = euclidian(numerator, denominator);

		if (res == 1) {
			if (dneg)
				denominator *= -1;
			if (nneg)
				numerator *= -1;
			return true;
		}

		numerator /= res;
		denominator /= res;
		if (dneg)
			denominator *= -1;
		if (nneg)
			numerator *= -1;
		return true;
	}

	bool Evaluatefraction(double value) {
		BLONG denom = (long)value;
		if (denom == value) {
			ND(denom, 1);
			return true;
		}

		double mantissa = value;
		//nb contains the number of digits of value...
		short nb = log10(fabs(value));
		BLONG num = 1;
		//We limit the calculus to 29 digits after the point.
		//The mantissa of a double is encoded on 52 bits, which is roughly 15 digits...
		while (nb < 15 && mantissa != (long)mantissa)  {
			mantissa *= 10;
			nb++;
			num *= 10;
		}
		denom = mantissa;
		//In the case of a full mantissa, we use another version of the transformation
		//namely we try to approximate with the smallest fraction possible...
		char ddenom = 0;
		char dnum = 0;
		if (nb == 15) {
			BLONG p1 = computeEuclidian(denom, num);
			BLONG p2 = computeEuclidian(denom + 1, num);
			if (p1 < p2) {
				ddenom = 1;
				dnum = 0;
				p1 = p2;
			}
			p2 = computeEuclidian(denom, num + 1);
			if (p1 < p2) {
				ddenom = 0;
				dnum = 1;
				p1 = p2;
			}
			p2 = computeEuclidian(denom - 1, num);
			if (p1 < p2) {
				ddenom = -1;
				dnum = 0;
				p1 = p2;
			}
			p2 = computeEuclidian(denom, num - 1);
			if (p1 < p2) {
				ddenom = 0;
				dnum = -1;
			}
			denom += ddenom;
			num += dnum;
		}

		ND(denom, num);
		return Simplify();
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
	}

	void Clean() {
		//Your cleaning code
	}

	string String() {
		stringstream res;
		res << numerator << "/" << denominator;
		return res.str();
	}

	wstring UString() {
		std::wstringstream res;
		res << numerator << L"/" << denominator;
		return res.str();
	}

	long Integer() {
		double v = numerator / denominator;
		return (long)v;
	}

	double Float() {
		double v = numerator / denominator;
		return v;
	}
	BLONG Long() {
		double v = numerator / denominator;
		return (BLONG)v;
	}

	bool Boolean() {
		if (numerator == 0)
			return false;
		return true;
	}


	//Basic operations
	long Size() {
		return Integer();
	}

	Atanorfraction* Fraction() {
		return this;
	}

	Atanor* plus(Atanor* bb, bool autoself) {
		BLONG dea = denominator;
		BLONG nua = numerator;

		Atanorfraction* b = bb->Fraction();
		BLONG nub = b->N();
		BLONG deb = b->D();
		if (b != bb)
			b->Release();

		if (deb != dea) {
			nua *= deb;
			nub *= dea;
			dea *= deb;
		}

		nua += nub;
		if (autoself) {
			numerator = nua;
			denominator = dea;
			Simplify();
			return this;
		}

		return new Atanorfraction(nua, dea);
	}

	Atanor* minus(Atanor* bb, bool autoself) {
		BLONG dea = denominator;
		BLONG nua = numerator;

		Atanorfraction* b = bb->Fraction();
		BLONG nub = b->N();
		BLONG deb = b->D();
		if (b != bb)
			b->Release();

		if (deb != dea) {
			nua *= deb;
			nub *= dea;
			dea *= deb;
		}

		nua -= nub;
		if (autoself) {
			numerator = nua;
			denominator = dea;
			Simplify();
			return this;
		}

		return new Atanorfraction(nua, dea);
	}

	Atanor* multiply(Atanor* bb, bool autoself) {
		BLONG dea = denominator;
		BLONG nua = numerator;

		Atanorfraction* b = bb->Fraction();
		BLONG nub = b->N();
		BLONG deb = b->D();
		if (b != bb)
			b->Release();

		nua *= b->N();
		dea *= b->D();

		if (autoself) {
			numerator = nua;
			denominator = dea;
			Simplify();
			return this;
		}

		return new Atanorfraction(nua, dea);
	}

	Atanor* divide(Atanor* bb, bool autoself) {
		BLONG dea = denominator;
		BLONG nua = numerator;

		Atanorfraction* b = bb->Fraction();
		BLONG nub = b->N();
		BLONG deb = b->D();
		if (b != bb)
			b->Release();

		nua *= deb;
		dea *= b->N();

		if (dea == 0)
			return new AtanorError("MATH(100): Error wrong fraction: denominator is 0");

		if (autoself) {
			numerator = nua;
			denominator = dea;
			Simplify();
			return this;
		}

		return new Atanorfraction(nua, dea);
	}

	Atanor* mod(Atanor* b, bool autoself) {
		BLONG ib = b->Long();
		if (ib == 0)
			return new AtanorError("Error: Divided by 0");

		BLONG i = Long() % ib;
		return new Atanorlong(i);
	}

	Atanor* shiftleft(Atanor* bb, bool autoself) {
		BLONG i = Long() << bb->Long();
		return new Atanorlong(i);
	}

	Atanor* shiftright(Atanor* bb, bool autoself) {
		BLONG i = Long() >> bb->Long();
		return new Atanorlong(i);
	}

	Atanor* power(Atanor* b, bool autoself) {
		double i = pow(Float(), b->Float());
		return new Atanorint(i);
	}

	Atanor* less(Atanor* a) {
		if (Float()<a->Float())
			return aTRUE;
		return aFALSE;
	}
	Atanor* more(Atanor* a) {
		if (Float()>a->Float())
			return aTRUE;
		return aFALSE;
	}
	Atanor* same(Atanor* a) {
		if (Float() == a->Float())
			return aTRUE;
		return aFALSE;
	}
	Atanor* different(Atanor* a) {
		if (Float() != a->Float())
			return aTRUE;
		return aFALSE;
	}

	Atanor* lessequal(Atanor* a) {
		if (Float() <= a->Float())
			return aTRUE;
		return aFALSE;
	}
	Atanor* moreequal(Atanor* a) {
		if (Float() >= a->Float())
			return aTRUE;
		return aFALSE;
	}

};

#endif




