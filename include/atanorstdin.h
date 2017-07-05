
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
filename   : atanorstdin.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanorstdin_h
#define atanorstdin_h
#include "atanor.h"
#include "atanorsvector.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorstdin;
//This typedef defines a type "stdinMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorstdin::*stdinMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanorstdin : public AtanorObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging hmap<unsigned short, stdinMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
	istream* is;

    //---------------------------------------------------------------------------------------------------------------------
    Atanorstdin(AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
        //Do not forget your variable initialisation
        is = NULL;
    }

    Atanorstdin() {
        //Do not forget your variable initialisation
		is = NULL;
    }

    //----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* v, Atanor* i, short idthread);
    
	Atanor* Get(Atanor* context, Atanor* v, short idthread);


    short Type() {
        return Atanorstdin::idtype;
    }

    string Typename() {
        return "stdin";
    }

	//The next two functions work in conjunction...
	//If your object is very atomic (such as a number or a string)
	//Then when it is passed to a function or stored into a container,
	//it might be worth duplicating it.
	// In that case: duplicateForCall should return true...
	
	//However if your object is complex and probably unique through out the code
	//The duplicateForCall should return false, and Atom should always reduce to a "return this;" only...
    Atanor* Atom(bool forced=false) {
        if (forced || !protect || reference)
            return new  Atanorstdin(NULL);
        return this;
    }

	bool duplicateForCall() {
		return true;
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
		return new Atanorstdin(NULL);
    }

    AtanorIteration* Newiteration(bool direction) {
		return aITERNULL;
    }

    static void AddMethod(AtanorGlobal* g, string name, stdinMethod func, unsigned long arity, string infos);
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
    //This is a sample  of a function that could be implemented for your needs.
	
	Atanor* MethodEof(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (is == NULL || is->eof())
			return aTRUE;
		return aFALSE;
	}

	Atanor* MethodInitial(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		is = &cin;
		return aTRUE;
	}

	Atanor* MethodGet(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (is == NULL || is->eof())
			return globalAtanor->Returnerror("Wrong access to the file", idthread);

		char c = is->get();
		if (context->isNumber())
			return globalAtanor->Provideint(c);
		string s;
		s = c;
		return globalAtanor->Providestring(s);
	}
	
	Atanor* MethodRead(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (is == NULL || is->eof())
			return globalAtanor->Returnerror("Wrong access to the file", idthread);

		string bf;
		long nb = -1;
		if (callfunc->Size() == 1) {
			nb = callfunc->Evaluate(0, aNULL, idthread)->Integer();
			if (nb != -1) {
				if (context->isVectorContainer()) {
					Atanor* vect = Selectasvector(context);
					readnb(((Atanorsvector*)vect)->values, nb);
					return vect;
				}

				readnb(bf, nb);
				return globalAtanor->Providestring(bf);
			}
		}


		if (context->isNumber())
			return globalAtanor->Provideint(is->get());

		if (context->isContainer()) {
			Atanor* vect = Selectasvector(context);
			readall(((Atanorsvector*)vect)->values);
			return vect;
		}

		readall(bf);
		return globalAtanor->Providestring(bf);

	}

    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
    //is returned by the Declaration method.
    Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods[idname])(contextualpattern, idthread, callfunc);
    }

	long readnb(string& v, long nb) {
		char fbuffer[4096];
		long i;
		long rd;
		long total = 0;
		while (total != nb) {
			if (is->eof())
				return total;
			is->read(fbuffer, 4096);
			rd = is->gcount();
			total += rd;
			for (i = 0; i < rd; i++)
				v += fbuffer[i];
		}

		return total;
	}

	long readnb(vector<string>& v, long nb) {
		string w;

		char fbuffer[4096];
		long i;
		long rd;
		long total = 0;

		while (total != nb) {
			if (is->eof())
				return total;
			is->read(fbuffer, 4095);
			rd = is->gcount();
			total += rd;
			for (i = 0; i < rd; i++) {
				w = fbuffer[i];
				v.push_back(w);
			}
		}

		return total;
	}

	void readall(string& v) {
		string line;
		while (!is->eof()) {
			getline(*is, line);
			v += line;
		}
	}


	void readall(vector<string>& v) {
		string line;
		while (!is->eof()) {
			getline(*is, line);
			v.push_back(line);
		}
	}


	void Clear() {
		//To set a variable back to empty
		is = NULL;
	}

};
#endif





