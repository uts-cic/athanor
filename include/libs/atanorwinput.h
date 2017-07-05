
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
filename   : atanorwinput.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanorwinput_h
#define atanorwinput_h

#include "atanorwidget.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorwinput;
//This typedef defines a type "winputMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorwinput::*winputMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanorwinput : public Atanorwidget {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static hmap<short, winputMethod> methods;
    static hmap<string, string> infomethods;
    static bin_hash<unsigned long> exported;

    static short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    Fl_Input* input;
	char txt[1024];
	

	bool multiline;
	string buf;

    //---------------------------------------------------------------------------------------------------------------------
	Atanorwinput(AtanorGlobal* g, Atanor* parent = NULL) : Atanorwidget(g, parent) {
        //Do not forget your variable initialisation
		input = NULL;
		multiline = false;
		
	}

	Fl_Widget* Widget() {
		return input;
	}

	void itemclose() {
		if (function != NULL)
			return;
		if (input != NULL)
			delete input;
		input = NULL;
		multiline = false;
		clean();
	}

    //----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* index, Atanor* value, short idthread);
	Atanor* Get(Atanor* context, Atanor* index, short idthread);


    short Type() {
        return Atanorwinput::idtype;
    }

    string Typename() {
        return "winput";
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
		Atanorwinput* a = new Atanorwinput(globalAtanor);
		a->function = f;
		return a;
    }

	AtanorIteration* Newiteration(bool direction);

    static void AddMethod(AtanorGlobal* g, string name, winputMethod func, unsigned long arity, string infos);
    
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



	bool isString() {
		return false;
	}

	bool isNumber() {
		return false;
	}

	bool isContainer() {
		return false;
	}

	virtual bool isValueContainer() {
		return false;
	}

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
    //Atanor* MethodSize(AtanorGlobal* global,Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {return aZERO;}
	Atanor* MethodCreate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodValue(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodInsert(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodSelection(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodColor(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodWord(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodFont(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodFontSize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

    

    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
    //is returned by the Declaration method.
    Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		if (globalAtanor->Error(idthread))
			return globalAtanor->Errorobject(idthread);

		if (Stopall())
			return aNULL;

		///Specific section to handle threads... One exception the redraw function...
		if (idthread && callfunc->Size() != 0) {
			//In this case, we do not want to process the method, we postpone its activation
			RecordGUIObject(callfunc, idthread);
			return aNULL;
		}

		return (this->*methods[idname])(contextualpattern, idthread, callfunc);
    }

    void Clear() {
        //To set a variable back to empty
		
    }

    void Clean() {
        //Your cleaning code
    }

	long Size() {
		return String().size();
	}


	long Integer() {
		
		return String().size();
	}
	
	double Float() {
		
		return String().size();
	}
	
	string String() {
		
		return input->value();
	}

	
	bool Boolean() {		
		return true;
	}	
};

#endif








