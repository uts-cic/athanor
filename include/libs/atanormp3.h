
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
filename   : atanormp3.h
Date       : 2017/01/24
Purpose    : 
Programmer : Claude ROUX (claude.roux@xrce.xerox.com)
Reviewer   :
*/

#ifndef atanormp3_h
#define atanormp3_h


#ifdef MACSOUND
typedef unsigned int SystemSoundID;
typedef void(*AudioServicesSystemSoundCompletionProc)(SystemSoundID  mySSID, void* myself);

bool createSoundId(const char* thepath, SystemSoundID* _soundId);
void cleanSoundId(SystemSoundID*);
void playSoundId(SystemSoundID*);
void addSoundFunction(SystemSoundID* soundid, AudioServicesSystemSoundCompletionProc c, void* data);
void removeSoundFunction(SystemSoundID* soundid);
void completionCallback(SystemSoundID  mySSID, void* myself);
#endif

#ifdef WIN32
#include "Mp3.h"
#endif



//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanormp3;
//This typedef defines a type "mp3Method", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanormp3::*mp3Method)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanormp3 : public AtanorObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static hmap<short, mp3Method> methods;
    static hmap<string, string> infomethods;
    static bin_hash<unsigned long> exported;

    static short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
#ifdef MACSOUND
	SystemSoundID sound;
#endif
#ifdef WIN32
	Mp3 sound;
#endif

	string path;
	bool loaded;
	bool created;

#ifdef MACSOUND
	Atanor* function;
#endif
    //---------------------------------------------------------------------------------------------------------------------
    Atanormp3(AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
        //Do not forget your variable initialisation
		loaded = false;
#ifdef MACSOUND
		function = NULL;
#endif
		created = false;
	}


	~Atanormp3() {
#ifdef MACSOUND
		if (created) {
			if (function != NULL)
				removeSoundFunction(&sound);
			cleanSoundId(&sound);
		}
#endif
#ifdef WIN32
		sound.Cleanup();
#endif
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

#ifdef MACSOUND
	void Addfunction(Atanor* a) {
		function = a;
	}
#endif

    short Type() {
        return Atanormp3::idtype;
    }

    string Typename() {
        return "mp3";
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
		Atanormp3* a= new Atanormp3(NULL);
#ifdef MACSOUND
		a->function = f;
#endif
		return a;
    }

    AtanorIteration* Newiteration(bool direction) {
        return aITERNULL;
    }

    static void AddMethod(AtanorGlobal* g, string name, mp3Method func, unsigned long arity, string infos);
    static bool InitialisationModule(AtanorGlobal* g, string version);

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
	Atanor* MethodLoad(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodPlay(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodStop(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

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


