
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
filename   : atanoriterator.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanoriterator_h
#define atanoriterator_h

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanoriterator;
//This typedef defines a type "iteratorMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanoriterator::*iteratorMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanoriterator : public AtanorObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging hmap<unsigned short, iteratorMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    AtanorIteration* value;

    //---------------------------------------------------------------------------------------------------------------------
    Atanoriterator(AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
        //Do not forget your variable initialisation
		value = NULL;
    }

    Atanoriterator() {
        //Do not forget your variable initialisation
        value=NULL;
    }

	void Setreference(short r) {
		reference += r;
		if (value != NULL)
			value->Setreference(r);
	}

	void Resetreference(short r) {
		if (value != NULL)
			value->Resetreference(r);
		AtanorObject::Resetreference(r);
	}

    //----------------------------------------------------------------------------------------------------------------------
    Atanor* Put(Atanor* idx, Atanor* v, short idthread) {
		if (value != NULL)
			value->Resetreference(reference);
		value = v->Newiteration(false);
		value->Setreference(reference);
        return this;
    }
    
    Atanor* Get(Atanor* context, Atanor* v, short idthread) {        
        return this;
    }

    short Type() {
        return Atanoriterator::idtype;
    }

    string Typename() {
        return "iterator";
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
        return this;
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
        return new Atanoriterator(NULL);
    }

    AtanorIteration* Newiteration(bool direction) {
		return aITERNULL;
    }

    static void AddMethod(AtanorGlobal* g, string name, iteratorMethod func, unsigned long arity, string infos);
    
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
	Atanor* MethodBegin(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		if (value == NULL)
			return aNOELEMENT;
		return value->Begin();
	}

	Atanor* MethodKey(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		if (value == NULL)
			return aNOELEMENT;
		return value->Key();
	}

	Atanor* MethodValue(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		if (value == NULL)
			return aNOELEMENT;
		return value->Value();
	}

	Atanor* MethodNext(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		if (value == NULL)
			return aNOELEMENT;
		value->Next();
		return aTRUE;
	}

	Atanor* MethodEnd(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		if (value == NULL)
			return aTRUE;
		return value->End();
	}
	
    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
    //is returned by the Declaration method.
    Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods[idname])(contextualpattern, idthread, callfunc);
    }

	Atanor* Key() {
		if (value == NULL)
			return aNOELEMENT;
		return value->Key();
	}

	Atanor* Value() {
		if (value == NULL)
			return aNOELEMENT;
		return value->Value();
	}

	string Keystring() {
		if (value == NULL)
			return "";
		return value->Keystring();
	}

	wstring Valueustring() {
		if (value == NULL)
			return L"";
		return value->Valueustring();
	}

	string Valuestring() {
		if (value == NULL)
			return "";
		return value->Valuestring();
	}

	long Keyinteger() {
		if (value == NULL)
			return 0;
		return value->Keyinteger();
	}

	long Valueinteger() {
		if (value == NULL)
			return 0;
		return value->Valueinteger();
	}

	double Keyfloat() {
		if (value == NULL)
			return 0;
		return value->Keyfloat();
	}

	double Valuefloat() {
		if (value == NULL)
			return 0;
		return value->Valuefloat();
	}

	void Next() {
		if (value != NULL)
			value->Next();
	}

	Atanor* End() {
		if (value == NULL)
			return aTRUE;

		if (value->End() == aTRUE) {
			delete value;
			value = NULL;
			return aTRUE;
		}


		return aFALSE;
	}

	void Begin() {
		if (value != NULL)
			value->Begin();
	}

	Atanor* plusplus() {
		Next();
		return Key();
	}

    void Clear() {
        //To set a variable back to empty
		if (value != NULL)
			delete value;
		value = NULL;
    }

    void Clean() {
        //Your cleaning code
    }

    string String() {
        
        return "";
    }

    //wstring UString() {
    //   
    //  return L"";
    //	}

    long Integer() {
        
        return 0;
    }
    double Float() {
        
        return 0;
    }
    BLONG Long() {
        
        return 0;
    }

    bool Boolean() {
        
        return false;
    }

    
    //Basic operations
    long Size() {
        
        return 0;
    }
    
    Atanor* andset(Atanor* a, bool itself) {
        
        return this;
    }

    Atanor* orset(Atanor* a, bool itself) {
        
        return this;
    }

    Atanor* xorset(Atanor* a, bool itself) {
        
        return this;
    }

    
    Atanor* plus(Atanor* a, bool itself) {
        
        return this;
    }

    Atanor* minus(Atanor* a, bool itself) {
        
        return this;
    }

    Atanor* multiply(Atanor* a, bool itself) {
        
        return this;
    }

    Atanor* divide(Atanor* a, bool itself) {
        
        return this;
    }
    Atanor* power(Atanor* a, bool itself) {
        
        return this;
    }
    Atanor* shiftleft(Atanor* a, bool itself) {
        
        return this;
    }
    Atanor* shiftright(Atanor* a, bool itself) {
        
        return this;
    }
    Atanor* mod(Atanor* a, bool itself) {
        
        return this;
    }
    
    Atanor* less(Atanor* a) {
        
        return aFALSE;
    }
    
    Atanor* more(Atanor* a) {
        
        return aFALSE;
    }

    Atanor* same(Atanor* a) {
        
        if (a == this)
            return aTRUE;
        return aFALSE;
    }

    Atanor* different(Atanor* a) {
        
        if (same(a) == aFALSE)
            return aTRUE;
        return aFALSE;
    }

    Atanor* lessequal(Atanor* a) {
        
        return aFALSE;
    }
    
    Atanor* moreequal(Atanor* a) {
        
        return aFALSE;
    }

};

#endif







