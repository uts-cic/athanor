
/*
* Xerox Research Centre Europe - Grenoble Laboratory (XRCE)
*
* Copyright (C) 2016 - 2017 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
* This file can only be used with the ATANOR library or the executable
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : ATHANOR
Version    : ATANOR 0.26
filename   : atanorcurl.h
Date       : 2016/09/21
Purpose    : 
Programmer : Claude ROUX (claude.roux@xrce.xerox.com)
Reviewer   :
*/

#ifndef atanorcurl_h
#define atanorcurl_h

#include <curl/curl.h>

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorcurl;
//This typedef defines a type "curlMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorcurl::*curlMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanorcurl : public AtanorObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static hmap<short, curlMethod> methods;
    static hmap<string, string> infomethods;
    static bin_hash<unsigned long> exported;

    static short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
	
	Atanor* function;
	Atanor* object;
	string urlstr;
	CURL *curl;

	char* urlbuffer;
	int urlsize;

    //---------------------------------------------------------------------------------------------------------------------
    Atanorcurl(AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
        //Do not forget your variable initialisation
		curl = curl_easy_init();
		function = NULL;
		object = aNULL;
		urlsize = 2048;
		urlbuffer = (char*)malloc(urlsize);		
    }

	~Atanorcurl() {
		free(urlbuffer);
		if (curl != NULL)
			curl_easy_cleanup(curl);
	}
	
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


    //----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* v, Atanor* i, short idthread);
    
	Atanor* Get(Atanor* context, Atanor* v, short idthread);


    short Type() {
        return Atanorcurl::idtype;
    }

    string Typename() {
        return "curl";
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


    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in kifexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        if (exported.find(n) != exported.end())
            return true;
        return false;
    }

    Atanor* Newinstance(short, Atanor* f = NULL) {
		Atanorcurl* a = new Atanorcurl(globalAtanor);
		a->function = f;
		return a;
    }

    AtanorIteration* Newiteration(bool direction) {
        return aITERNULL;
    }

	void Addfunction(Atanor* a) {
		function = a;
	}

    static void AddMethod(AtanorGlobal* g, string name, curlMethod func, unsigned long arity, string infos);
    static bool InitialisationModule(AtanorGlobal* g, string version);

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
    //Atanor* MethodSize(AtanorGlobal* global,Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {return aZERO;}

	Atanor* MethodInitial(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		//We are interested by the variable...
		object = callfunc->arguments[0];
		if (!object->isCallVariable())
			object = callfunc->Evaluate(0, aNULL, idthread);
		return aTRUE;
	}

	Atanor* MethodProxy(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodPWD(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodURL(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodExecute(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodOptions(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);


    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
    //is returned by the Declaration method.
    Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods[idname])(contextualpattern, idthread, callfunc);
    }

	Atanor* errormsg(short idthread, CURLcode res) {
		const char* errmsg = curl_easy_strerror(res);
		char ch[1024];
		sprintf_s(ch, 1024, "URL(%d): %s", res, errmsg);
		return globalAtanor->Returnerror(ch, idthread);
	}

    void Clear() {
        //To set a variable back to empty
    }

    void Clean() {
        //Your cleaning code
    }

    string String() {
        Locking _lock(this);
        return "";
    }

    //wstring UString() {
    //   Locking _lock(this);
    //  return L"";
    //	}

    long Integer() {
        Locking _lock(this);
        return 0;
    }
    double Float() {
        Locking _lock(this);
        return 0;
    }
    BLONG Long() {
        Locking _lock(this);
        return 0;
    }

    bool Boolean() {
        Locking _lock(this);
        return false;
    }

    
    //Basic operations
    long Size() {
        Locking _lock(this);
        return 0;
    }
    
    Atanor* andset(Atanor* a, bool itself) {
        Locking _lock(this);
        return this;
    }

    Atanor* orset(Atanor* a, bool itself) {
        Locking _lock(this);
        return this;
    }

    Atanor* xorset(Atanor* a, bool itself) {
        Locking _lock(this);
        return this;
    }

    
    Atanor* plus(Atanor* a, bool itself) {
        Locking _lock(this);
        return this;
    }

    Atanor* minus(Atanor* a, bool itself) {
        Locking _lock(this);
        return this;
    }

    Atanor* multiply(Atanor* a, bool itself) {
        Locking _lock(this);
        return this;
    }

    Atanor* divide(Atanor* a, bool itself) {
        Locking _lock(this);
        return this;
    }
    Atanor* power(Atanor* a, bool itself) {
        Locking _lock(this);
        return this;
    }
    Atanor* shiftleft(Atanor* a, bool itself) {
        Locking _lock(this);
        return this;
    }
    Atanor* shiftright(Atanor* a, bool itself) {
        Locking _lock(this);
        return this;
    }
    Atanor* mod(Atanor* a, bool itself) {
        Locking _lock(this);
        return this;
    }
    
    Atanor* less(Atanor* a) {
        Locking _lock(this);
        return aFALSE;
    }
    
    Atanor* more(Atanor* a) {
        Locking _lock(this);
        return aFALSE;
    }

    Atanor* same(Atanor* a) {
        Locking _lock(this);
        if (a == this)
            return aTRUE;
        return aFALSE;
    }

    Atanor* different(Atanor* a) {
        Locking _lock(this);
        if (same(a) == aFALSE)
            return aTRUE;
        return aFALSE;
    }

    Atanor* lessequal(Atanor* a) {
        Locking _lock(this);
        return aFALSE;
    }
    
    Atanor* moreequal(Atanor* a) {
        Locking _lock(this);
        return aFALSE;
    }

};

#endif


