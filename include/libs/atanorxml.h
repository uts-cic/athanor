
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
filename   : atanorxml.h
Date       : 2016/09/01
Purpose    : 
Programmer : Claude ROUX (claude.roux@xrce.xerox.com)
Reviewer   :
*/

#ifndef atanorxml_h
#define atanorxml_h

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>


//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorxml;
//This typedef defines a type "xmlMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorxml::*xmlMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanorxml : public AtanorObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static hmap<short, xmlMethod> methods;
	static hmap<string, string> infomethods;
	static bin_hash<unsigned long> exported;

	static short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	xmlNodePtr node;

	//---------------------------------------------------------------------------------------------------------------------
	Atanorxml(AtanorGlobal* g, xmlNodePtr v)  {
		//Do not forget your variable initialisation
		node = v;
		g->RecordInTrackerProtected(this);
	}

	//----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* v, Atanor* i, short idthread);

	Atanor* Get(Atanor* context, Atanor* v, short idthread);


	short Type() {
		return Atanorxml::idtype;
	}

	string Typename() {
		return "xml";
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

	Atanor* Atom(bool forced = false) {
		return this;
	}

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
		return new Atanorxml(globalAtanor, NULL);
	}

	AtanorIteration* Newiteration(bool direction) {
		return aITERNULL;
	}

	static void AddMethod(AtanorGlobal* g, string name, xmlMethod func, unsigned long arity, string infos);
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
	void TraverseXML(xmlNodePtr n, string& contenu, bool first) {
		if (n == NULL)
			return;
		if (n->content != NULL)
			contenu += (char*)n->content;
		TraverseXML(n->children, contenu, false);
		if (!first)
			TraverseXML(n->next, contenu, false);
	}


	Atanor* MethodRoot(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodDocument(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodNext(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodUnlink(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodDelete(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodPrevious(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodParent(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodChild(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodName(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodLine(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodId(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodxmlType(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodProperties(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodContent(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodNamespace(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodNew(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* Methodxmlstring(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);


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
		if (node == NULL)
			return "";
		return (char*)node->name;
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


