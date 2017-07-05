
/*
*  Athanor: Le langage des Alpages m�de � Grenoble
*
*  Copyright (C) 2017: ATHANOR Language
* This file can only be used with the ATHANOR library or the executable
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : ATHANOR
Version    : ATANOR 0.26
filename   : atanorslider.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanorslider_h
#define atanorslider_h
#include "atanorwidget.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorslider;
//This typedef defines a type "sliderMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorslider::*sliderMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanorslider : public Atanorwidget {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static hmap<short, sliderMethod> methods;
    static hmap<string, string> infomethods;
    static bin_hash<unsigned long> exported;

    static short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
	Fl_Slider* slider;
	char txt[1024];
	

    //---------------------------------------------------------------------------------------------------------------------
	Atanorslider(AtanorGlobal* g, Atanor* parent = NULL) : Atanorwidget(g, parent) {
        //Do not forget your variable initialisation
		slider = NULL;
		
    }

	void itemclose() {
		if (slider != NULL)
			delete slider;
		slider = NULL;
		clean();
	}

	Fl_Widget* Widget() {
		return slider;
	}

	void ResetWidget() {		
		slider = NULL;
	}


    //----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* index, Atanor* value, short idthread);
	Atanor* Get(Atanor* context, Atanor* index, short idthread);


    short Type() {
        return Atanorslider::idtype;
    }

    string Typename() {
        return "slider";
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
		Atanorslider* a = new Atanorslider(globalAtanor);
		a->function = f;
		return a;
    }

	AtanorIteration* Newiteration(bool direction);

    static void AddMethod(AtanorGlobal* g, string name, sliderMethod func, unsigned long arity, string infos);
    
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
	Atanor* MethodResize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodBoundary(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodType(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodBoxType(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodStep(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodValue(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

    

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
		return 0;
	}

	uchar Byte() {
		
		return 0;
	}

	long Integer() {
		
		return 0;
	}
	
	double Float() {
		
		return 0;
	}
	
	string String() {
		
		return "";
	}

	
	bool Boolean() {
		
		return true;
	}

    
    //Basic operations    
	Atanor* andset(Atanor* a, bool autoself) {
		
	    return this;
	}

	Atanor* orset(Atanor* a, bool autoself) {
		
	    return this;
	}

	Atanor* xorset(Atanor* a, bool autoself) {
		
	    return this;
	}

	Atanor* plus(Atanor* a, bool autoself) {
		
	    return this;
	}
	Atanor* minus(Atanor* a, bool autoself) {
		
	    return this;
	}

	Atanor* multiply(Atanor* a, bool autoself) {
		
	    return this;
	}

	Atanor* divide(Atanor* a, bool autoself) {
		
	    return this;

	}
	Atanor* mod(Atanor* a, bool autoself) {
		
	    return this;
	}

	Atanor* shiftleft(Atanor* a, bool autoself) {
		
	    return this;
	}


	Atanor* shiftright(Atanor* a, bool autoself) {
		
	    return this;
	}

	Atanor* power(Atanor* a, bool autoself) {
		
	    return this;
	}

	Atanor* less(Atanor* a) {
		return aFALSE;
	}
	Atanor* more(Atanor* a) {
		return aFALSE;
	}
	Atanor* same(Atanor* a) {
		return aFALSE;
	}
	Atanor* different(Atanor* a) {
		return aFALSE;
	}

	Atanor* lessequal(Atanor* a) {
		return aFALSE;
	}

	Atanor* moreequal(Atanor* a) {
		return aFALSE;
	}

	Atanor* plusplus() {
		return this;
	}

	Atanor* minusminus() {
		return this;
	}
};

#endif








