
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
filename   : atanorbool.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanorbool_h
#define atanorbool_h

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorbool;
//This typedef defines a type "boolMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorbool::*boolMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanorbool : public AtanorObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging hmap<unsigned short, boolMethod> methods;
	static Exchanging hmap<string, string> infomethods;
	static Exchanging bin_hash<unsigned long> exported;

	static Exchanging short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	bool value;

	//---------------------------------------------------------------------------------------------------------------------
	Atanorbool(bool v, AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
		//Do not forget your variable initialisation
		value = v;
	}

	Atanorbool(bool v) {
		//Do not forget your variable initialisation
		value = v;
	}

	//----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* index, Atanor* v, short idthread) {
		value = v->Boolean();
		return this;
	}

	Atanor* Putvalue(Atanor* v, short idthread) {
		value = v->Boolean();
		return this;
	}


	Atanor* Get(Atanor* context, Atanor* value, short idthread) {
		return this;
	}


	short Type() {
		return Atanorbool::idtype;
	}

	string Typename() {
		return "bool";
	}

	bool isString() {
		return false;
	}

	bool isNumber() {
		return false;
	}

	bool isAtom() {
		return true;
	}

	bool isBoolean() {
		return false;
	}

	bool isFrame() {
		return false;
	}

	Atanor* Atom(bool forced = false) {
		if (forced || !protect || reference)
			return new  Atanorbool(value);
		return this;
	}

	Atanor* Succ() {
		if (value)
			return aFALSE;
		return aTRUE;
	}

	Atanor* Pred() {
		if (value)
			return aFALSE;
		return aTRUE;
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
		return new Atanorbool(0);
	}

	Atanor* Newvalue(Atanor* a, short idthread) {
		return new Atanorbool(a->Boolean());
	}


	AtanorIteration* Newiteration(bool direction) {
		return aITERNULL;
	}

	static void AddMethod(AtanorGlobal* g, string name, boolMethod func, unsigned long arity, string infos);
	
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
	Atanor* MethodInvert(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		if (value)
			return aFALSE;
		return aTRUE;
	}

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
		value = false;
	}

	void Clean() {
		//Your cleaning code
	}

	string String() {
		if (value)
			return "true";
		return "false";
	}

	long Integer() {
		return (long)value;
	}

	long Size() {
		return (long)value;
	}

	bool Boolean() {
		return value;
	}

	float Decimal() {
		return (float)value;
	}

	double Float() {
		return (double)value;
	}

	BLONG Long() {
		return (BLONG)value;
	}

	Atanor* same(Atanor* a) {
		if (value == a->Boolean())
			return aTRUE;
		return aFALSE;
	}

	Atanor* different(Atanor* a) {
		if (value != a->Boolean())
			return aTRUE;
		return aFALSE;
	}


	Atanor* plus(Atanor* a, bool itself) {
		if (a->Boolean() || value)
			return aTRUE;

		return aFALSE;
	}

	Atanor* minus(Atanor* a, bool itself) {
		if (value) {
			if (a->Boolean())
				return aFALSE;
			return aTRUE;
		}

		if (a->Boolean())
			return aTRUE;

		return aFALSE;
	}

};


class AtanorLoopBoolean : public Atanorbool {
public:

	AtanorLoopBoolean() : Atanorbool(true) {}

	Atanor* Put(Atanor* context, Atanor* ke, short) {
		value = ke->Boolean();
		return aTRUE;
	}

	short Type() {
		return a_bloop;
	}

	void Next() {
		value = true - value;
	}

	Atanor* plusplus() {
		value = true - value;
		return this;
	}

	Atanor* minusminus() {
		value = true - value;
		return this;
	}

	Atanor* Newinstance(short, Atanor* f = NULL) {
		return new AtanorLoopBoolean;
	}

};

#endif









