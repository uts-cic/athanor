
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
filename   : atanorframeinstance.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanorframeinstance_h
#define atanorframeinstance_h

#include "instructions.h"
//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorframeinstance;
//This typedef defines a type "frameinstanceMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorframeinstance::*frameinstanceMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------
class AtanorDeclarationForInstance : public Atanor {
public:

	VECTE<short> names;
	VECTE<Atanor*> declarations;
	basebin_hash<Atanor*> declared;
	short i;

	AtanorDeclarationForInstance()  {}

	long Size() {
		return names.size();
	}

	bool isDeclaration() {
		return true;
	}

	bool isEmpty() {
		if (!declarations.last)
			return true;
		return false;
	}

	bool isDeclared(short id) {
		return declared.check(id);
	}

	short Idposition(short id) {
		for (i = 0; i < names.last; i++) {
			if (names.vecteur[i] == id)
				return i;
		}
		return -1;
	}

	void Variables(vector<short>& vars) {
		for (i = 0; i < names.last; i++)
			vars.push_back(names[i]);
	}

	void Declare(short id, Atanor* a) {
		for (i = 0; i < names.last; i++) {
			if (names.vecteur[i] == id) {
				declarations.vecteur[i] = a;
				declared[id] = a;
				return;
			}
		}

		names.push_back(id);
		declarations.push_back(a);
		declared[id] = a;
	}

	Atanor* Declaration(short id) {
		return declared[id];
	}

	void Redeclare(short ipos, short id, Atanor* a) {
		declarations.vecteur[ipos] = a;
		declared[id] = a;
	}

	void erase(short id) {
		for (i = 0; i < names.last; i++) {
			if (names.vecteur[i] == id) {
				names.erase(i);
				declarations.erase(i);
				declared.erase(id);
				break;
			}
		}
	}

	void Cleaning(short idthread) {
		for (i = 0; i < names.last; i++) {
			declarations.vecteur[i]->Resetreference();
			globalAtanor->Removevariable(idthread, names.vecteur[i]);
		}

		names.last = 0;
		declarations.last = 0;
		declared.clear();
	}

	void Setreference(short r = 1) {
		for (i = 0; i < declarations.last; i++)
			declarations[i]->Setreference(r);
	}

	void Resetreference(short r = 1) {
		for (i = 0; i < declarations.last; i++)
			declarations[i]->Resetreference(r);
	}

	void Setprotect(bool n) {
		for (i = 0; i < declarations.last; i++)
			declarations[i]->Setprotect(n);
	}

	void Popping() {
		for (i = 0; i < declarations.last; i++)
			declarations[i]->Popping();
	}

	void clear() {
		names.clear();
		declarations.clear();
	}
};

//---------------------------------------------------------------------------------------------------------------------
class Atanorframeinstance : public AtanorObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging hmap<unsigned short, frameinstanceMethod> methods;
	static Exchanging hmap<string, string> infomethods;
	static Exchanging bin_hash<unsigned long> exported;

	static Exchanging short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...

	AtanorFrame* frame;
	AtanorDeclarationForInstance declared;
	bool declaring;
	//---------------------------------------------------------------------------------------------------------------------
	Atanorframeinstance(AtanorFrame* v, AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
		//Do not forget your variable initialisation
		frame = v;
		declaring = true;
	}

	Atanorframeinstance(AtanorFrame* v) {
		//Do not forget your variable initialisation
		frame = v;
		declaring = false;
	}

	//----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* index, Atanor* value, short idthread);
	Atanor* Get(Atanor* context, Atanor* value, short idthread);

	short Name() {
		return frame->name;
	}

	short Type() {
		if (frame == NULL)
			return idtype;
		return frame->Name();
	}

	string Typename() {
		return "frameinstance";
	}

	bool isFrame() {
		return true;
	}

	Atanor* Frame() {
		return frame;
	}

	bool duplicateForCall() {
		return false;
	}

	short Typevariable(short i) {
		return declared.declarations.vecteur[i]->Type();
	}

	void Postinstanciation(short idthread, bool setreference);

	void Variables(vector<short>& vars) {
		for (int it = 0; it < declared.declarations.last; it++) {
			if (declared.declarations[it]->isFunction() || declared.declarations[it]->isFrame() || declared.declarations[it]->isVariable())
				continue;
			vars.push_back(declared.names[it]);
		}
	}

	bool isDeclared(short id) {
		if (id == a_this)
			return true;

		if (declaring)
			return frame->isDeclared(id);
		
		return declared.isDeclared(id);
	}

	virtual void Declare(short id, Atanor* a) {
		if (declaring)
			frame->Declare(id, a);
		else
			declared.Declare(id, a);
	}

	Atanor* Declaration(short id) {
		if (declaring)
			return frame->Declaration(id);

		if (id == a_this)
			return this;
		
		return declared.Declaration(id);
	}

	Atanor* Atom(bool forced = false) {
		return this;
	}

	void setprotection() {
	}

	void resetprotection() {
	}

	void Cleaning(short idthread) {
		declared.Cleaning(idthread);
	}

	//---------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	Atanor* Newinstance(short, Atanor* f = NULL);

	AtanorIteration* Newiteration(bool direction) {
		return aITERNULL;
	}

	static void AddMethod(AtanorGlobal* g, string name, frameinstanceMethod func, unsigned long arity, string infos);

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
	static Atanorframeinstance* RecordFrame(short name, AtanorFrame* v, AtanorGlobal* g);
	//---------------------------------------------------------------------------------------------------------------------
	void Setreference(short r = 1) {
		Locking _lock(this);
		reference += r;
		protect = false;
		declared.Setreference(r);
	}

	void MethodFinal() {
		Atanor* func = frame->Declaration(a_final);
		if (func == NULL)
			return;
		
		short idthread = globalAtanor->GetThreadid();
		globalAtanor->Pushstack(this, idthread);
		func = Execute(func, idthread);
		globalAtanor->Popstack(idthread);

		func->Release();
	}

	void Resetreference(short r = 1) {
		bool deleted = false;
		{
			Locking _lock(this);

			if ((reference - r) <= 0)
				//We are about to delete our object... We call our final function then...
				MethodFinal();

			declared.Resetreference(r);
			reference -= r;
			if (reference <= 0) {
				if (protect)
					protect = false;
				else {
					if (idtracker != -1)
						globalAtanor->RemoveFromTracker(idtracker);
					deleted = true;
					declared.clear();
				}
			}
		}

		if (deleted)
			delete this;
	}

	void Setprotect(bool n) {
		Locking _lock(this);
		protect = n;
		declared.Setprotect(n);
	}

	void Popping() {
		Locking _lock(this);
		protect = false;
		if (reference <= 0)
			protect = true;
		declared.Popping();
	}

	long Setprotect() {
		return globalAtanor->RecordInTrackerProtected(this);
	}

	//---------------------------------------------------------------------------------------------------------------------
	Atanor* Execute(Atanor* body, vector<Atanor*>& arguments, short idthread);
	Atanor* Execute(Atanor* body, short idthread);
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	Atanor* MethodInitial(Atanor* domain, short idthread, AtanorCall* callfunc) {
		Atanor* func = frame->Declaration(a_initial);
		while (func != NULL && func->Size() != callfunc->Size())
			func = func->Nextfunction();

		if (func == NULL) {
			if (callfunc->Size() == 0)
				return aTRUE;
			return globalAtanor->Returnerror("Missing '_initial' function, check the arguments", idthread);
		}


		vector<Atanor*> arguments;
		Atanor* a;
		long i;
		for (i = 0; i < callfunc->arguments.size(); i++) {
			a = callfunc->arguments[i]->Get(domain, aNULL, idthread);
			a->Setreference();
			arguments.push_back(a);
		}

		globalAtanor->Pushstack(this, idthread);
		func = Execute(func, arguments, idthread);
		globalAtanor->Popstack(idthread);

		for (i = 0; i < arguments.size(); i++)
			arguments[i]->Resetreference();

		return func;
	}

	Atanor* MethodType(Atanor* domain, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providestring(globalAtanor->Getsymbol(frame->name));
	}

	//This is associated to all functions that have been declared within the frame...
	Atanor* MethodMethod(Atanor* domain, short idthread, AtanorCall* callfunc) {
		Atanor* func = frame->Declaration(callfunc->Name());
		if (func == NULL) {
			stringstream message;
			message << "Unknown method: '" << globalAtanor->Getsymbol(callfunc->Name()) << "' in frame: '" << globalAtanor->Getsymbol(frame->Name()) << "'";
			return globalAtanor->Returnerror(message.str(), idthread);
		}

		vector<Atanor*> arguments;
		Atanor* a;
		long i;

		setprotection();
		for (i = 0; i < callfunc->arguments.size(); i++) {
			a = callfunc->arguments[i]->Get(domain, aNULL, idthread);
			a->Setreference();
			arguments.push_back(a);
		}

		globalAtanor->Pushstack(this, idthread);
		func = Execute(func, arguments, idthread);
		globalAtanor->Popstack(idthread);

		for (i = 0; i < arguments.size(); i++)
			arguments[i]->Resetreference();

		resetprotection();

		return func;
	}

    //This is associated to all functions that have been declared within the frame...
    Atanor* MethodTopMethod(Atanor* domain, short idthread, AtanorCall* callfunc, AtanorFrame* topframe) {
        Atanor* func = topframe->Declaration(callfunc->Name());
        if (func == NULL) {
            stringstream message;
            message << "Unknown method: '" << globalAtanor->Getsymbol(callfunc->Name()) << "' in frame: '" << globalAtanor->Getsymbol(frame->Name()) << "'";
            return globalAtanor->Returnerror(message.str(), idthread);
        }
        
        vector<Atanor*> arguments;
        Atanor* a;
        long i;
        
        setprotection();
        for (i = 0; i < callfunc->arguments.size(); i++) {
            a = callfunc->arguments[i]->Get(domain, aNULL, idthread);
            a->Setreference();
            arguments.push_back(a);
        }
        
        globalAtanor->Pushstack(this, idthread);
        func = Execute(func, arguments, idthread);
        globalAtanor->Popstack(idthread);
        
        for (i = 0; i < arguments.size(); i++)
            arguments[i]->Resetreference();
        
        resetprotection();
        
        return func;
    }
    //---------------------------------------------------------------------------------------------------------------------
	Atanor* Callconversion(short id) {
		Atanor* func = frame->Declaration(id);
		if (func == NULL)
			return aNULL;

		setprotection();		
		short idthread = globalAtanor->GetThreadid();
		globalAtanor->Pushstack(this, idthread);
		func = Execute(func, idthread);
		globalAtanor->Popstack(idthread);

		resetprotection();
		return func;
	}

	Atanor* Calloperation(short id, Atanor* a) {
		Atanor* func = frame->Declaration(id);
		if (func == NULL)
			return aNULL;

		setprotection();
		vector<Atanor*> arguments;
		arguments.push_back(a);
		short idthread = globalAtanor->GetThreadid();
		globalAtanor->Pushstack(this, idthread);
		func = Execute(func, arguments, idthread);
		globalAtanor->Popstack(idthread);

		resetprotection();
		return func;
	}

	Atanor* in(Atanor* context, Atanor* a, short idthread) {
		Atanor* func = frame->Declaration(a_in);
		if (func == NULL)
			return aFALSE;


		vector<Atanor*> arguments;
		setprotection();
		arguments.push_back(context);
		arguments.push_back(a);

		func = Execute(func, arguments, idthread);
		resetprotection();

		return func;
	}

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
	//is returned by the Declaration method.
	Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		//In the case of a frame, either it is one of the predefined function
		if (exported.check(idname))
			return (this->*methods[idname])(contextualpattern, idthread, callfunc);

		//Or a basic function...
		return MethodMethod(contextualpattern, idthread, callfunc);
	}
    
    Atanor* CallTopMethod(short idname, AtanorFrame* topframe, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        return MethodTopMethod(contextualpattern, idthread, callfunc, topframe);
    }

	void Clear() {
		//To set a variable back to empty
	}

	void Clean() {
		//Your cleaning code
	}

	wstring UString() {
		Atanor* res = Callconversion(a_ustring);
		wstring s = res->UString();
		res->Release();
		return s;
	}


	string String() {
		Atanor* res = Callconversion(a_string);
		string s = res->String();
		res->Release();
		return s;
	}

	short Short() {
		Atanor* res = Callconversion(a_short);
		short s = res->Short();
		res->Release();
		return s;
	}

	long Integer() {
		Atanor* res = Callconversion(a_int);
		long s = res->Integer();
		res->Release();
		return s;
	}

	double Float() {
		Atanor* res = Callconversion(a_float);
		double s = res->Float();
		res->Release();
		return s;
	}

	BLONG Long() {
		Atanor* res = Callconversion(a_long);
		BLONG s = res->Long();
		res->Release();
		return s;
	}

	bool Boolean() {
		Atanor* res = Callconversion(a_boolean);
		bool s = res->Boolean();
		res->Release();
		return s;
	}

	uchar Byte() {
		Atanor* res = Callconversion(a_byte);
		uchar s = res->Byte();
		res->Release();
		return s;
	}

	//Basic operations
	long Size() {
		return declared.Size();
	}

	Atanor* andset(Atanor* a, bool itself) {
		return Calloperation(a_and, a);
	}

	Atanor* orset(Atanor* a, bool itself) {
		return Calloperation(a_or, a);
	}

	Atanor* xorset(Atanor* a, bool itself) {
		return Calloperation(a_xor, a);
	}


	Atanor* plus(Atanor* a, bool itself) {
		return Calloperation(a_plus, a);
	}

	Atanor* minus(Atanor* a, bool itself) {
		return Calloperation(a_minus, a);
	}

	Atanor* multiply(Atanor* a, bool itself) {
		return Calloperation(a_multiply, a);
	}

	Atanor* divide(Atanor* a, bool itself) {
		return Calloperation(a_divide, a);
	}

	Atanor* power(Atanor* a, bool itself) {
		return Calloperation(a_power, a);
	}

	Atanor* shiftleft(Atanor* a, bool itself) {
		return Calloperation(a_shiftleft, a);
	}
	Atanor* shiftright(Atanor* a, bool itself) {
		return Calloperation(a_shiftright, a);
	}

	Atanor* mod(Atanor* a, bool itself) {
		return Calloperation(a_mod, a);
	}

	Atanor* less(Atanor* a) {
		return Calloperation(a_less, a);
	}

	Atanor* more(Atanor* a) {
		return Calloperation(a_more, a);
	}

	Atanor* same(Atanor* a) {
		return Calloperation(a_same, a);
	}

	Atanor* different(Atanor* a) {
		return Calloperation(a_different, a);
	}

	Atanor* lessequal(Atanor* a) {
		return Calloperation(a_lessequal, a);
	}

	Atanor* moreequal(Atanor* a) {
		return Calloperation(a_moreequal, a);
	}


};

#endif







