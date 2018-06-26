
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
filename   : atanortime.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX (claude.roux@xrce.xerox.com)
Reviewer   :
*/

#ifndef atanortime_h
#define atanortime_h
#ifdef WIN32
#include <Windows.h>
#endif

#ifdef UNIX
#include <sys/time.h>
#endif

#include "atanorfloat.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanortime;
//This typedef defines a type "timeMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanortime::*timeMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanortime : public AtanorObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging hmap<unsigned short, timeMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...

	timeval value;

    //---------------------------------------------------------------------------------------------------------------------
	Atanortime(AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
        //Do not forget your variable initialisation
		gettimeofday(&value, NULL);
    }

	Atanortime() {
		//Do not forget your variable initialisation
		gettimeofday(&value, NULL);
	}

	Atanortime(timeval& t) {
		//Do not forget your variable initialisation
		value = t;
	}

	//----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* v, Atanor* i, short idthread);
    
	Atanor* Get(Atanor* context, Atanor* v, short idthread);


    short Type() {
        return Atanortime::idtype;
    }

    string Typename() {
        return "time";
    }

    bool isString() {
        return false;
    }

    bool isNumber() {
        return false;
    }

    bool isBoolean() {
        return false;
    }

    bool isFrame() {
        return false;
    }

    Atanor* Atom(bool forced=false) {
        if (forced || !protect || reference)
            return new  Atanortime(value);
        return this;
    }

	bool duplicateForCall() {
		return false;
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
        return new Atanortime;
    }

    AtanorIteration* Newiteration(bool direction) {
		return aITERNULL;
    }

    static void AddMethod(AtanorGlobal* g, string name, timeMethod func, unsigned long arity, string infos);
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
    //Atanor* MethodSize(AtanorGlobal* global,Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {return aZERO;}

	Atanor* MethodReset(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);


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

	string String();

	double Float() {
		double mt = (((unsigned long)value.tv_sec) * 1000 + ((unsigned long)value.tv_usec) / 1000.0) + 0.5;
		return mt;
	}

	long Integer() {
		return Float();
	}

	bool Boolean() {
		return true;
	}

	Atanor* plus(Atanor* bb, bool autoself) {
		if (bb->Type() != idtype)
			return aNULL;

		Atanortime* b = (Atanortime*)bb;

		return new Atanorfloat(plustime(value, b->value));

	}

	Atanor* minus(Atanor* bb, bool autoself) {
		if (bb->Type() != idtype)
			return aNULL;

		Atanortime* b = (Atanortime*)bb;

		return new Atanorfloat(minustime(value, b->value));
	}


	Atanor* less(Atanor* a) {
		double mt = (((unsigned long)value.tv_sec) * 1000 + ((unsigned long)value.tv_usec) / 1000.0) + 0.5;
		if (mt<a->Float())
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
	long Size() {
		return Integer();
	}
};

#endif



