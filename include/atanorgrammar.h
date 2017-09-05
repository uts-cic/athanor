
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
filename   : atanorgrammar.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX (claude.roux@xrce.xerox.com)
Reviewer   :
*/

#ifndef atanorgrammar_h
#define atanorgrammar_h
#include "atanor.h"
#include "instructions.h"

class gramstate;
class Atanorgrammar;
class GrammarBaseAutomaton;
class GrammarAutomaton;

class gramarc {
public:
	gramstate* state;
	gramstate* from;
	wstring wlabel;
	short idrule;
	unsigned char status;

	gramarc();

	virtual char test(wstring& c);
	virtual int test(wstring& l, wstring& r, int i);
	char test(wchar_t c);
};

class gramstate {
public:
	unsigned char status;
	vector<gramarc*> arcs;

	gramstate() {
		status = 0;
	}

	char comparemap(short idthread, wstring& lkey, GrammarBaseAutomaton* gram, vector<wstring>& labels, int& i, Atanor* m, bool);
	char compare(short idthread, GrammarBaseAutomaton* gram, vector<wstring>& labels, int& i, Atanor* v, bool);


	char comparemap(short idthread, wstring& lkey, GrammarBaseAutomaton* gram, wstring& labels, int& i, Atanor* m, bool);
	char compare(short idthread, GrammarBaseAutomaton* gram, wstring& labels, int& i, Atanor* v, bool);
};

class GrammarBaseAutomaton {
public:
	

	vector<gramstate*> states;
	vector<gramarc*> arcs;
	vector<gramstate*> rules;

	Atanor* function;
	unsigned char skipblanks;

	GrammarBaseAutomaton() {		
		function = NULL;
		skipblanks = 0;
	}

	~GrammarBaseAutomaton() {
		int i;

		for (i = 0; i < states.size(); i++)
			delete states[i];

		for (i = 0; i < arcs.size(); i++)
			delete arcs[i];
	}
	bool callfunction(short idthread, string&, Atanor*, int pos);
	bool callfunction(short idthread, wstring&, Atanor*, int pos);
	bool callfunction(Atanor* func, short idthread, Atanor*, int pos);

};

class subgrammar : public gramarc {
public:

	gramstate substate;
	GrammarBaseAutomaton gram;

	subgrammar(GrammarBaseAutomaton* ga)  {}

	char test(wstring& c);
	int test(wstring& l, wstring& r, int i);
};


class GrammarAutomaton : public GrammarBaseAutomaton {
public:

	hmap<wstring, int> dictionary;
	hmap<int, wstring> rdictionary;

	Atanor* Apply(short, Atanor* res, wstring& labels, bool asstring);
	Atanor* Apply(short, Atanor* res, vector<wstring>& labels, bool asstring);
};


//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorgrammar;
//This typedef defines a type "grammarMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorgrammar::*grammarMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanorgrammar : public AtanorObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging hmap<unsigned short, grammarMethod> methods;
	static Exchanging hmap<string, string> infomethods;
	static Exchanging bin_hash<unsigned long> exported;

	static Exchanging short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	
	GrammarAutomaton* automaton;
	Atanor* function;
	unsigned char skipblanks;
	bool clean;


	//---------------------------------------------------------------------------------------------------------------------
	Atanorgrammar(AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
		//Do not forget your variable initialisation
		automaton = NULL;
		clean = false;
		skipblanks = 0;
	}

	void Methods(Atanor* v) {

		for (auto& it : infomethods)
			v->storevalue(it.first);
	}

	string Info(string n) {

		if (infomethods.find(n) != infomethods.end())
			return infomethods[n];
		return "Unknown method";
	}

	//----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* v, Atanor* i, short idthread);

	Atanor* Get(Atanor* context, Atanor* v, short idthread);

	Atanor* in(Atanor* context, Atanor* a, short idthread);

	short Type() {
		return Atanorgrammar::idtype;
	}

	string Typename() {
		return "grammar";
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

	//If it is too complex to duplicate an element (for instance when passed to a function)
	//then you can use return false...  Usually in that case, Atom should always be just: return this;
	bool duplicateForCall() {
		return false;
	}

	void Addfunction(Atanor* a) {
		function = a;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in atanexportedmethods... See MethodInitialization below
	bool isDeclared(short n) {
		if (exported.find(n) != exported.end())
			return true;
		return false;
	}

	Atanor* Newinstance(short, Atanor* f = NULL) {
		Atanorgrammar* a = new Atanorgrammar(NULL);
		a->function = f;
		return a;
	}

	AtanorIteration* Newiteration(bool direction) {
		return aITERNULL;
	}

	static void AddMethod(AtanorGlobal* g, string name, grammarMethod func, unsigned long arity, string infos);
	static bool InitialisationModule(AtanorGlobal* g, string version);

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	//Atanor* MethodSize(AtanorGlobal* global,Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {return aZERO;}

	Atanor* MethodInitial(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodApply(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
	//is returned by the Declaration method.
	Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		Locking _lock(this);
		return (this->*methods[idname])(contextualpattern, idthread, callfunc);
	}

	//------------------------------------------------------------------------------------------------------------------------
	Atanor* Apply(short, Atanor* v, vector<wstring>& labels, bool asstring);
	Atanor* Apply(short, Atanor* v, wstring& labels, bool asstring);
	Atanor* Loader(string& v, short idthread);

	void Clear() {
		//To set a variable back to empty
	}

	void Clean() {
		//Your cleaning code
	}

};

#endif




