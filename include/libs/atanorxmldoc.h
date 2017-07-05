
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
filename   : atanorxmldoc.h
Date       : 2017/01/24
Purpose    : 
Programmer : Claude ROUX (claude.roux@xrce.xerox.com)
Reviewer   :
*/

#ifndef atanorxmldoc_h
#define atanorxmldoc_h

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorxmldoc;
//This typedef defines a type "xmldocMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorxmldoc::*xmldocMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanorxmldoc : public AtanorObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static hmap<short, xmldocMethod> methods;
    static hmap<string, string> infomethods;
    static bin_hash<unsigned long> exported;

    static short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
	
	string filename;
	char fname[1024];

	xmlSAXHandler* sax;
	xmlDocPtr doc;
	Atanor* function;
	Atanor* object;

	Atanor* functionend;
	Atanor* objectend;
	int idnode;
	bool stop;

	xmlXPathContextPtr xpathcontext;

    //---------------------------------------------------------------------------------------------------------------------
    Atanorxmldoc(AtanorGlobal* g, Atanor* parent) : AtanorObject(g, parent) {
        //Do not forget your variable initialisation
		doc = NULL;
		object = aNULL;
		function = NULL;
		functionend = NULL;
		objectend = aNULL;
		sax = NULL;
		idnode = 1;
		xpathcontext = NULL;
		stop = false;
	}

    Atanorxmldoc(xmlDocPtr v) : AtanorObject() {
        //Do not forget your variable initialisation
		doc = v;
		function = NULL;
		object = aNULL;
		functionend = NULL;
		objectend = aNULL;
		sax = NULL;
		idnode = 1;
		xpathcontext = NULL;
		stop = false;
	}

    //----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* v, Atanor* i, short idthread);
    
	Atanor* Get(Atanor* context, Atanor* v, short idthread);


    short Type() {
        return Atanorxmldoc::idtype;
    }

    string Typename() {
        return "xmldoc";
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

	bool duplicateForCall() {
		return false;
	}

	void Addfunction(Atanor* a) {
		function = a;
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
		Atanorxmldoc* a = new Atanorxmldoc(NULL);
		a->function = f;
		return a;
    }

    AtanorIteration* Newiteration(bool direction) {
        return aITERNULL;
    }

    static void AddMethod(AtanorGlobal* g, string name, xmldocMethod func, unsigned long arity, string infos);
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
    //This is an example of a function that could be implemented for your needs.
    //Atanor* MethodSize(AtanorGlobal* global,Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {return aZERO;}

	Atanor* MethodLoadXML(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodOnClosing(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodLoadXMLBuffer(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodCreate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodXPath(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodSave(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodClose(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodNode(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodxmlEncoding(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodSerialize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodSerializeString(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

	Atanor* MethodInitial(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		//We are interested by the variable...
		object = callfunc->arguments[0];
		if (!object->isCallVariable())
			object = callfunc->Evaluate(0, aNULL, idthread);
		return aTRUE;
	}

	xmlNodePtr buildnode(xmlNodePtr r, Atanor* e);
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
		if (doc == NULL)
			return "";
		xmlChar* buff = NULL;
		int longueur;
		xmlDocDumpMemoryEnc(doc, &buff, &longueur, "UTF-8");
		string s((char*)buff);
		xmlFree(buff);
		return s;
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


};

#endif


