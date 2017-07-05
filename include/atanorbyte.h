
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
filename   : atanorbyte.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanorbyte_h
#define atanorbyte_h

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorbyte;
//This typedef defines a type "byteMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorbyte::*byteMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanorbyte : public AtanorObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging hmap<unsigned short, byteMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    unsigned char value;

    //---------------------------------------------------------------------------------------------------------------------
    Atanorbyte(unsigned char v, AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
        //Do not forget your variable initialisation
        value=v;
    }

    Atanorbyte(unsigned char v) {
        //Do not forget your variable initialisation
        value=v;
    }

    //----------------------------------------------------------------------------------------------------------------------
    Atanor* Put(Atanor* idx, Atanor* v, short idthread) {
		value = v->Byte();
        return this;
    }
    
	Atanor* Putvalue(Atanor* v, short idthread) {
		value = v->Byte();
		return this;
	}


    Atanor* Get(Atanor* context, Atanor* v, short idthread) {        
        return this;
    }

	bool Checkprecision(Atanor* r) {
		if (r->Type() == a_byte)
			return true;

		return false;
	}

    short Type() {
        return Atanorbyte::idtype;
    }

    string Typename() {
        return "byte";
    }

    Atanor* Atom(bool forced=false) {
        if (forced || !protect || reference)
            return new  Atanorbyte(value);
        return this;
    }

	string Bytes() {
		char buff[] = { (char)value, 0 };
		return buff;
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
        return new Atanorbyte(0);
    }

    AtanorIteration* Newiteration(bool direction) {
		return aITERNULL;
    }

    static void AddMethod(AtanorGlobal* g, string name, byteMethod func, unsigned long arity, string infos);
    
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
	bool isString() {
		return false;
	}
	bool isAtom() {
		return true;
	}
	bool isNumber() {
		return true;
	}
    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
    //Atanor* MethodSize(AtanorGlobal* global,Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {return aZERO;}
    
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
	unsigned char Byte() {
		return value;
	}
	
	long Integer() {
		return value;
	}
	
	float Decimal() {
		return value;
	}
	
	double Float() {
		return value;
	}

	BLONG Long() {
		return value;
	}

	short Sort() {
		return value;
	}

	wstring UString() {
		wstring s(L" ");
		s[0] = value;
		return s;
	}

	wstring Getustring(short idthread) {
		wstring s(L" ");
		s[0] = value;
		return s;
	}

	string Getstring(short idthread) {
		string s(" ");
		s[0] = value;
		return s;
	}

	long Getinteger(short idthread) {
		return value;
	}

	double Getfloat(short idthread) {
		return value;
	}

	float Getdecimal(short idthread) {
		return value;
	}

	BLONG Getlong(short idthread) {
		return value;
	}

	string String() {
		string s(" ");
		s[0] = value;
		return s;
	}
	void storevalue(short u) {
		value = u;
	}
	void storevalue(long u) {
		value = (uchar)u;
	}
	
	void storevalue(double u) {
		value = (uchar)u;
	}
	void storevalue(unsigned char u) {
		value = u;
	}
	void storevalue(string u) {
		if (u.size())
			value = u[0];
		else
			value = 0;
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
	Atanor* andset(Atanor* a, bool autoself) {
		if (autoself) {
			value &= a->Byte();
			return this;
		}
		unsigned char i = value & a->Byte();
		return new Atanorbyte(i);
	}

	Atanor* orset(Atanor* a, bool autoself) {
		if (autoself) {
			value |= a->Byte();
			return this;
		}
		unsigned char i = value | a->Byte();
		return new Atanorbyte(i);

	}

	Atanor* xorset(Atanor* a, bool autoself) {
		if (autoself) {
			value ^= a->Byte();
			return this;
		}
		unsigned char i = value ^ a->Byte();
		return new Atanorbyte(i);

	}

	Atanor* plus(Atanor* a, bool autoself) {
		if (autoself) {
			value += a->Byte();
			return this;
		}
		unsigned char i = value + a->Byte();
		return new Atanorbyte(i);

	}
	Atanor* minus(Atanor* a, bool autoself) {
		if (autoself) {
			value -= a->Byte();
			return this;
		}
		unsigned char i = value - a->Byte();
		return new Atanorbyte(i);
	}

	Atanor* multiply(Atanor* a, bool autoself) {
		if (autoself) {
			value *= a->Byte();
			return this;
		}
		unsigned char i = value * a->Byte();
		return new Atanorbyte(i);
	}

	Atanor* divide(Atanor* a, bool autoself);
	Atanor* mod(Atanor* a, bool autoself);

	Atanor* shiftleft(Atanor* a, bool autoself) {
		if (autoself) {
			value <<= a->Byte();
			return this;
		}
		unsigned char i = value << a->Byte();
		return new Atanorbyte(i);
	}


	Atanor* shiftright(Atanor* a, bool autoself) {
		if (autoself) {
			value >>= a->Byte();
			return this;
		}
		unsigned char i = value >> a->Byte();
		return new Atanorbyte(i);
	}

	Atanor* power(Atanor* a, bool autoself);

	Atanor* less(Atanor* a) {
		if (value<a->Byte())
			return aTRUE;
		return aFALSE;
	}
	Atanor* more(Atanor* a) {
		if (value>a->Byte())
			return aTRUE;
		return aFALSE;
	}
	Atanor* same(Atanor* a) {
		if (value == a->Byte())
			return aTRUE;
		return aFALSE;
	}
	Atanor* different(Atanor* a) {
		if (value != a->Byte())
			return aTRUE;
		return aFALSE;
	}

	Atanor* lessequal(Atanor* a) {
		if (value <= a->Byte())
			return aTRUE;
		return aFALSE;
	}
	Atanor* moreequal(Atanor* a) {
		if (value >= a->Byte())
			return aTRUE;
		return aFALSE;
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
};

#endif







