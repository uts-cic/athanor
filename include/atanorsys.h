
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
filename   : atanorsys.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX (claude.roux@xrce.xerox.com)
Reviewer   :
*/

#ifndef atanorsys_h
#define atanorsys_h

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorsys;
//This typedef defines a type "sysMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorsys::*sysMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanorsys : public AtanorObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging hmap<unsigned short, sysMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
	string lastcommand;

    //---------------------------------------------------------------------------------------------------------------------
    Atanorsys(AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
        //Do not forget your variable initialisation
        
    }

    Atanorsys() {
        //Do not forget your variable initialisation
        
    }

    //----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* v, Atanor* i, short idthread);
    
	Atanor* Get(Atanor* context, Atanor* v, short idthread);


    short Type() {
        return Atanorsys::idtype;
    }

    string Typename() {
        return "sys";
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
            return new  Atanorsys;
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
		return new Atanorsys;
    }

    AtanorIteration* Newiteration(bool direction) {
		return aITERNULL;
    }

    static void AddMethod(AtanorGlobal* g, string name, sysMethod func, unsigned long arity, string infos);
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

	Atanor* MethodCommand(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodisDirectory(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodCreateDirectory(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodListDirectory(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodRealPath(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodFileInfo(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodEnv(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);


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

    string String() {       
        return lastcommand;
    }

};

class AtanorConstsys : public Atanorsys {
public:

	AtanorConstsys(AtanorGlobal* g) : Atanorsys(g) {}

	void Resetreference(long inc) {}
	void Release() {}

};
#endif



