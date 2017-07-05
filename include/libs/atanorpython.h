
/*
* Xerox Research Centre Europe - Grenoble Laboratory (XRCE)
*
* Copyright (C) 2010 - 2017 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
* This file can only be used with the ATANOR library or the executable
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : ATHANOR
Version    : ATANOR 0.26
filename   : atanorpython.h
Date       : 2016/11/19
Purpose    : 
Programmer : Claude ROUX (claude.roux@xrce.xerox.com)
Reviewer   :
*/

#ifndef atanorpython_h
#define atanorpython_h

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorpython;
//This typedef defines a type "pythonMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorpython::*pythonMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanorpython : public AtanorObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    Exchanging static hmap<unsigned short, pythonMethod> methods;
    static hmap<string, string> infomethods;
    static bin_hash<unsigned long> exported;

    static short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
	string pythonfilename;
	PyObject* pModule;


    //---------------------------------------------------------------------------------------------------------------------
    Atanorpython(AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
        //Do not forget your variable initialisation
        pModule = NULL;
		pythonfilename = "";
    }

    Atanorpython() {
        //Do not forget your variable initialisation
		pModule = NULL;
		pythonfilename = "";
    }

    //----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* v, Atanor* i, short idthread);
    
	Atanor* Get(Atanor* context, Atanor* v, short idthread);


    short Type() {
        return Atanorpython::idtype;
    }

    string Typename() {
        return "python";
    }

	//The next two functions work in conjunction...
	//If your object is very atomic (such as a number or a string)
	//Then when it is passed to a function or stored into a container,
	//it might be worth duplicating it.
	// In that case: needNewInstance should return true...
	
	//However if your object is complex and probably unique through out the code
	//The needNewInstance should return false, and Atom should always reduce to a "return this;" only...
    Atanor* Atom(bool forced=false) {
		if (forced || !protect)
			return new  Atanorpython;
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
		return new Atanorpython;
    }

    AtanorIteration* Newiteration(bool direction) {
		return aITERNULL;
    }

    static void AddMethod(AtanorGlobal* g, string name, pythonMethod func, unsigned long arity, string infos);
    static bool InitialisationModule(AtanorGlobal* g, string version);

	void Methods(Atanor* v) {
		hmap<string, string>::iterator it;
		for (it = infomethods.begin(); it != infomethods.end(); it++)
			v->storevalue(it->first);
	}

	string Info(string n) {

		if (infomethods.find(n) != infomethods.end())
			return infomethods[n];
		return "Unknown method";
	}


    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is a sample  of a function that could be implemented for your needs.
	
	Atanor* MethodSetpath(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

	Atanor* MethodClose(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);


	Atanor* MethodRun(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

	Atanor* MethodImport(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

	Atanor* MethodExecute(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
    //is returned by the Declaration method.
    Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods[idname])(contextualpattern, idthread, callfunc);
    }

    void Clear(AtanorGlobal* g) {
        //To set a variable back to empty
    }

    void Clean() {
        //Your cleaning code
    }


};
#endif


