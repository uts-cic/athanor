
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
filename   : atanoratanor.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanoratanor_h
#define atanoratanor_h
#include "compilecode.h"
#include "instructions.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanoratanor;
//This typedef defines a type "atanorMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanoratanor::*atanorMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanoratanor : public AtanorObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging hmap<unsigned short, atanorMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;

	static Exchanging hmap<string, Atanoratanor*> recorded;

	bin_hash<unsigned long> declared;

    short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    AtanorCode* acode;
	string filename;

    //---------------------------------------------------------------------------------------------------------------------
    Atanoratanor(AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
        //Do not forget your variable initialisation
		acode = NULL;
    }

	Atanoratanor(string fn, AtanorCode* a, AtanorGlobal* g) : AtanorObject(g) {
		filename = fn;
		acode = a;
		Atanoratanor::recorded[filename] = this;
	}

	void Methods(Atanor* v) {

		for (auto& it : infomethods)
			v->storevalue(it.first);
	}

	string Info(string n) {

		if (infomethods.find(n) != infomethods.end())
			return infomethods[n];
		return "Unknown method";
	}


    //----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* v, Atanor* i, short idthread);
    
	Atanor* Get(Atanor* context, Atanor* v, short idthread);


	short Type() {
		return a_atanor;
	}

	short Typevariable() {
		return idtype;
	}

	string Typename() {
        return "atanor";
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

	//If it is too complex to duplicate an element (for instance when passed to a function)
	//then you can use return false...  Usually in that case, Atom should always be just: return this;
	bool duplicateForCall() {
		return false;
	}

	bool isDeclared(short id) {
		return acode->isDeclared(id);
	}

	Atanor* Declaration(short id) {
		return acode->Declaration(id);
	}


    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	Atanor* Newinstance(short, Atanor* f = NULL) {
		return new Atanoratanor(globalAtanor);
    }

    AtanorIteration* Newiteration(bool direction) {
        return aITERNULL;
    }

    static void AddMethod(AtanorGlobal* g, string name, atanorMethod func, unsigned long arity, string infos);
	static bool InitialisationModule(AtanorGlobal* g, string version);
	
	bool InitialisationModule(short name, AtanorGlobal* g);
	Atanor* Load();

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.

    Atanor* MethodMethod(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		Atanor* domain = acode->Mainframe();
		Atanor* func = domain->Declaration(callfunc->Name());
		if (func == NULL) {
			string message = "Unknown function: '";
			message += globalAtanor->Getsymbol(callfunc->Name());
			message += "'";
			return globalAtanor->Returnerror(message, idthread);
		}

		AtanorCallFunction cfunc(func);
		cfunc.arguments = callfunc->arguments;
		globalAtanor->Pushstack(domain, idthread);
		Atanor* a = cfunc.Get(domain, aNULL, idthread);
		globalAtanor->Popstack(idthread);
		return a;
	}

    

    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
    //is returned by the Declaration method.
    Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return MethodMethod(contextualpattern, idthread, callfunc);
    }

    void Clear() {
        //To set a variable back to empty
    }

    void Clean() {
        //Your cleaning code
    }

    string String() {        
		return filename;
    }

};

Atanor* AtanorRecordedFiles(string filename);

#endif




