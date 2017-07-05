
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
filename   : atanordate.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX (claude.roux@xrce.xerox.com)
Reviewer   :
*/

#ifndef atanordate_h
#define atanordate_h

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanordate;
//This typedef defines a type "dateMethod", which expose the typical parameters of a new a method implementation
typedef Atanor* (Atanordate::*dateMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanordate : public AtanorObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging hmap<unsigned short, dateMethod> methods;
	static Exchanging hmap<string, string> infomethods;
	static Exchanging bin_hash<unsigned long> exported;

	static Exchanging short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	time_t value;

	//---------------------------------------------------------------------------------------------------------------------
	Atanordate(AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
		//Do not forget your variable initialisation
		value = NULL;
	}

	Atanordate(time_t v)  {
		//Do not forget your variable initialisation
		if (v == 0)
			time(&value);
		else
			value = v;

	}

	//----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* v, Atanor* i, short idthread);

	Atanor* Get(Atanor* context, Atanor* v, short idthread);


	short Type() {
		return Atanordate::idtype;
	}

	string Typename() {
		return "date";
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
		if (forced || !protect || reference)
			return new  Atanordate(value);
		return this;
	}

	bool duplicateForCall() {
		return false;
	}


	//---------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in aexportedmethods... See MethodInitialization below
	bool isDeclared(short n) {
		if (exported.find(n) != exported.end())
			return true;
		return false;
	}

	Atanor* Newinstance(short, Atanor* f = NULL) {
		return new Atanordate((time_t)0);
	}

	AtanorIteration* Newiteration(bool direction) {
		return aITERNULL;
	}

	static void AddMethod(AtanorGlobal* g, string name, dateMethod func, unsigned long arity, string infos);
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
	Atanor* MethodInitial(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodDate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodYear(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodMonth(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodDay(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodYearday(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodWeekday(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodHour(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodMinute(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodFormat(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodSecond(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);


	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new a methods. This method is called when a AtanorCallMethodMethod object
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

	string String();

	//wstring UString() {
	//   
	//  return L"";
	//	}

	long Integer() {

		return value;
	}
	double Float() {

		return value;
	}
	BLONG Long() {

		return value;
	}

	bool Boolean() {
		if (value == 0)
			return false;
		return true;
	}

	Atanor* plus(Atanor* b, bool autoself) {
		if (autoself) {
			value += b->Integer();
			return this;
		}

		long i = value + b->Integer();
		return new Atanordate(i);
	}

	Atanor* minus(Atanor* b, bool autoself) {
		if (autoself) {
			value = difftime(value, b->Integer());
			return this;
		}
		time_t i = difftime(value, b->Integer());
		return new Atanordate(i);
	}

	//Basic operations

	Atanor* less(Atanor* a) {
		if (value<a->Integer())
			return aTRUE;
		return aFALSE;
	}
	Atanor* more(Atanor* a) {
		if (value>a->Integer())
			return aTRUE;
		return aFALSE;
	}
	Atanor* same(Atanor* a) {
		if (value == a->Integer())
			return aTRUE;
		return aFALSE;
	}
	Atanor* different(Atanor* a) {
		if (value != a->Integer())
			return aTRUE;
		return aFALSE;
	}

	Atanor* lessequal(Atanor* a) {
		if (value <= a->Integer())
			return aTRUE;
		return aFALSE;
	}
	Atanor* moreequal(Atanor* a) {
		if (value >= a->Integer())
			return aTRUE;
		return aFALSE;
	}
	long Size() {
		return value;
	}
};

#endif




