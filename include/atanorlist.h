
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
filename   : atanorlist.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanorlist_h
#define atanorlist_h

#include "atanorint.h"
#include "atanorstring.h"
#include "atanorfvector.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorlist;
//This typedef defines a type "listMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorlist::*listMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------


class Atanorlist : public AtanorObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging hmap<unsigned short, listMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    list<Atanor*> values;
    bool evaluate;

    //---------------------------------------------------------------------------------------------------------------------
    Atanorlist(AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
        //Do not forget your variable initialisation
        evaluate = false;
    }

    Atanorlist() {
        //Do not forget your variable initialisation
        evaluate = false;
    }

    //----------------------------------------------------------------------------------------------------------------------
    Exporting Atanor* Put(Atanor* index, Atanor* value, short idthread);Exporting Atanor* Get(Atanor* context, Atanor* value, short idthread);
    short Type() {
        return Atanorlist::idtype;
    }

    string Typename() {
        return "list";
    }

    bool isContainer() {
        return true;
    }

    bool isVectorContainer() {
        return false;
    }

    bool isMapContainer() {
        return false;
    }

    bool duplicateForCall() {
        return false;
    }

    Exporting Atanor* getvalue(BLONG i);
	Exporting void storevalue(string u);
	Exporting void storevalue(float u);
	Exporting void storevalue(short u);
	Exporting void storevalue(wstring u);
	Exporting void storevalue(long u);
	Exporting void storevalue(BLONG u);
	Exporting void storevalue(double u);
	Exporting void storevalue(unsigned char u);
	Exporting void storevalue(wchar_t u);

    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in kifexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        if (exported.find(n) != exported.end())
            return true;
        return false;
    }

    Atanor* Newinstance(short idthread, Atanor* f = NULL) {
        return new Atanorlist();
    }

    Atanor* Newvalue(Atanor* a, short idthread) {
        Atanorlist* v = new Atanorlist();
        if (a->isContainer()) {
            AtanorIteration* it = a->Newiteration(false);
            for (it->Begin(); it->End() == aFALSE; it->Next()) {
                v->values.push_back(it->IteratorValue());
            }
            it->Release();
        }
        return v;
    }


    Exporting AtanorIteration* Newiteration(bool direction);

    static void AddMethod(AtanorGlobal* g, string name, listMethod func, unsigned long arity, string infos);
    
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

    Atanor* Atom(bool forced = false) {
        if (forced) {
            Atanorlist* v = new Atanorlist;
            Locking _lock(this);

            for (auto& it : values)
                v->Push(it);
            return v;
        }
        return this;
    }


    //---------------------------------------------------------------------------------------------------------------------
	Exporting void Setreference(short r = 1);
	Exporting void Resetreference(short r = 1);
    void Setprotect(bool n) {
        Locking _lock(this);
        protect = n;

        for (auto& it : values)
            it->Setprotect(n);
    }

	void Protectcontainervalues() {
		Locking _lock(this);
		protect = true;

		for (auto& it : values)
			it->Setprotect(true);
	}

    void Popping() {
        Locking _lock(this);
        protect = false;
        if (reference <= 0)
            protect = true;


        for (auto& it : values)
            it->Popping();
    }

    bool listValue(list<Atanor*>::iterator& it, int itx) {
        Locking _lock(this);
        if (itx >= values.size())
            return false;
        it = values.begin();
        while (itx > 0) {
            it++;
            itx--;
        }
        return true;
    }
    //---------------------------------------------------------------------------------------------------------------------
    Atanor* MethodPush(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Atanor* v;
        for (size_t i = 0; i < callfunc->Size(); i++) {
            v = callfunc->Evaluate(i, contextualpattern, idthread);
			if (v->isError())
				return v;
            Push(v);
            v->Release();
        }
        return aTRUE;
    }

    Atanor* MethodRemove(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Atanor* a = callfunc->Evaluate(0, contextualpattern, idthread);
        list<Atanor*>::iterator it;
        for (it = values.begin(); it != values.end(); it++) {
            if ((*it)->same(a)->Boolean()) {
                values.erase(it);
                return aTRUE;
            }
        }
        return aFALSE;
    }

    Atanor* MethodFlatten(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Atanor* res = Selectavector(contextualpattern);
        Flattening(res, this);
        return res;
    }

    Atanor* MethodClear(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Clear();
        return aTRUE;
    }

    Atanor* MethodPushfirst(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Atanor* v = callfunc->Evaluate(0, contextualpattern, idthread);
        return Pushfirst(v);
    }

    Atanor* MethodLast(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Locking _lock(this);
        if (values.size() == 0)
            return aNOELEMENT;
        return values.back();
    }

    Atanor* MethodFirst(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Locking _lock(this);
        if (values.size() == 0)
            return aNOELEMENT;
        return values.back();
    }

    Atanor* MethodJoin(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        //The separator between values
        string sep = callfunc->Evaluate(0, contextualpattern, idthread)->String();
        Locking _lock(this);
        bool beg = true;
        string res;

        for (auto& it : values) {
            if (beg == false)
                res += sep;
            beg = false;
            res += it->String();
        }

		return globalAtanor->Providestring(res);
    }

    Atanor* MethodMerge(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Atanor* v = callfunc->Evaluate(0, contextualpattern, idthread);
        Merging(v);
        return this;
    }

    Atanor* MethodSum(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        double v = Sum();
		return globalAtanor->Providefloat(v);
    }

    Atanor* MethodProduct(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        double v = Product();
		return globalAtanor->Providefloat(v);
    }

    Atanor* MethodInsert(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        long i = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        Atanor* v = callfunc->Evaluate(1, contextualpattern, idthread);
        Insert(i, v);
        return aTRUE;
    }

    Atanor* MethodPop(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Atanor* pos = aMINUSONE;
        if (callfunc->Size() == 1)
            pos = callfunc->Evaluate(0, contextualpattern, idthread);
        return Pop(pos);
    }

    Atanor* MethodPopfirst(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Popfirst();
        return aTRUE;
    }

    Atanor* MethodPoplast(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        return Pop(aONE);
    }


    Atanor* MethodUnique(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        return Unique();
    }

    Atanor* MethodShuffle(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Shuffle();
        return this;
    }

    Atanor* MethodReverse(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        return Inverse();
    }

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
    Exporting Atanor* Pushfirst(Atanor*);Exporting Atanor* Push(Atanor*);
    
    Atanor* push(Atanor* v) {
        values.push_back(v);
        v->Setreference();
        return this;
    }

    Exporting Atanor* Pop(Atanor*);Exporting void Popfirst();Exporting void Poplast();
    Atanor* Last() {
        Locking _lock(this);
        if (values.size() == 0)
            return aNOELEMENT;
        return values.back();
    }

    Atanor* First() {
        Locking _lock(this);
        if (values.size() == 0)
            return aNOELEMENT;
        return values.front();
    }

    Exporting Atanor* Unique();Exporting void Shuffle();Exporting Atanor* Merging(Atanor*);Exporting bool Permute();Exporting void Insert(int idx, Atanor* ke);
    Atanor* Inverse() {
        Locking _lock(this);
        Atanorlist* vect = new Atanorlist;
        list<Atanor*>::reverse_iterator it;
        for (it = values.rbegin(); it != values.rend(); it++)
            vect->Push(*it);
        return vect;
    }

    Exporting Atanor* Map(short idthread);Exporting Atanor* Vector(short idthread);
    
	Atanor* Thesum(long i, long j) {
		long sz = values.size();
		if (!sz)
			return aZERO;

		if (i < 0)
			i += sz;
		if (j < 0)
			j += sz;

		if (i < 0)
			i = 0;
		else
		if (i >= sz)
			i = sz - 1;

		if (j <= i)
			j = i + 1;
		
		j++;
		if (j>sz)
			j = sz;

		long n = 0;
		list<Atanor*>::iterator it = values.begin();	
		if ((*it)->isContainer()) {
			Atanorfvector* sub = globalAtanor->Providefvector();
			Atanor* res;
			for (; it != values.end(); it++) {
				if (n == j)
					break;
				if (n >= i) {
					res = (*it)->Thesum(0, (*it)->Size());
					sub->values.push_back(res->Float());
					res->Release();
				}
				n++;
			}

			return res;
		}
		
		if ((*it)->isNumber()) {
			double v = 0;
			for (; it != values.end(); it++) {
				if (n == j)
					break;
				if (n >= i)
					v += (*it)->Float();
				n++;
			}

			return globalAtanor->Providefloat(v);
		}

		string v;
		for (; it != values.end(); it++) {
			if (n == j)
				break;
			if (n >= i)
				v += (*it)->String();
			n++;
		}

		return globalAtanor->Providestring(v);
	}

	Atanor* Theproduct(long i, long j) {
		long sz = values.size();
		if (!sz)
			return aZERO;

		if (i < 0)
			i += sz;
		if (j < 0)
			j += sz;

		if (i < 0)
			i = 0;
		else
		if (i >= sz)
			i = sz - 1;

		if (j <= i)
			j = i + 1;
		
		j++;
		if (j>sz)
			j = sz;


		long n = 0;
		list<Atanor*>::iterator it = values.begin();
		if ((*it)->isContainer()) {
			Atanorfvector* sub = globalAtanor->Providefvector();
			Atanor* res;
			for (; it != values.end(); it++) {
				if (n == j)
					break;
				if (n >= i) {
					res = (*it)->Theproduct(0, (*it)->Size());
					sub->values.push_back(res->Float());
					res->Release();
				}
				n++;
			}

			return res;
		}
		
		double v = 1;

		for (auto& it : values) {
			if (n == j)
				break;
			if (n >= i)
				v *= it->Float();
			n++;
		}

		return globalAtanor->Providefloat(v);
	}

	double Sum() {
        Locking _lock(this);
        if (values.size() == 0)
            return 0;
        double v = 0;

        for (auto& it : values)
            v += it->Float();
        return v;
    }

    double Product() {
        Locking _lock(this);
        if (values.size() == 0)
            return 0;
        double v = 1;

        for (auto& it : values)
            v += it->Float();
        return v;
    }
    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
    //is returned by the Declaration method.
    Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods[idname])(contextualpattern, idthread, callfunc);
    }

    Exporting void Clear();Exporting void Clean();
    Exporting string JSonString();Exporting string String();
    //wstring UString() {}

    Exporting long Integer();Exporting double Float();Exporting BLONG Long();Exporting bool Boolean();

    //Basic operations
    Exporting long Size();
    Exporting Atanor* in(Atanor* context, Atanor* a, short idthread);
    Exporting Atanor* andset(Atanor* a, bool itself);Exporting Atanor* orset(Atanor* a, bool itself);Exporting Atanor* xorset(Atanor* a, bool itself);Exporting Atanor* plus(Atanor* a, bool itself);Exporting Atanor* minus(Atanor* a, bool itself);Exporting Atanor* multiply(Atanor* a, bool itself);Exporting Atanor* divide(Atanor* a, bool itself);Exporting Atanor* power(Atanor* a, bool itself);Exporting Atanor* shiftleft(Atanor* a, bool itself);Exporting Atanor* shiftright(Atanor* a, bool itself);Exporting Atanor* mod(Atanor* a, bool itself);
    Exporting Atanor* same(Atanor* a);
};

class AtanorIterationlist : public AtanorIteration {
    public:

    long itx;
    list<Atanor*>::iterator it;
    list<Atanor*>::reverse_iterator rit;
    Atanorlist* ref;
    Locking _lock;

    AtanorIterationlist(Atanorlist* v, bool d, AtanorGlobal* g = NULL) : _lock(v), AtanorIteration(d, g) {
        ref = v;
        itx = 0;
    }

    Atanor* Key() {
		return globalAtanor->Provideint(itx);
    }

    Atanor* Value() {
        if (reverse)
            return (*rit);
        return (*it);
    }

    string Keystring() {
        stringstream s;
        s << itx;
        return s.str();
    }

    string Valuestring() {
        if (reverse)
            return (*rit)->String();
        return (*it)->String();
    }

    long Keyinteger() {
        return itx;
    }

	BLONG Keylong() {
		return itx;
	}

    long Valueinteger() {
        if (reverse)
            return (*rit)->Integer();
        return (*it)->Integer();
    }

    double Keyfloat() {
        return itx;
    }

    float Valuedecimal() {
        if (reverse)
            return (*rit)->Decimal();
        return (*it)->Decimal();
    }

    double Valuefloat() {
        if (reverse)
            return (*rit)->Float();
        return (*it)->Float();
    }

    void Next() {
        if (reverse) {
            itx--;
            rit++;
        }
        else {
            itx++;
            it++;
        }
    }

    Atanor* End() {
        if (reverse) {
            if (rit == ref->values.rend())
                return aTRUE;
        }
        else {
            if (it == ref->values.end())
                return aTRUE;
        }
        return aFALSE;
    }

    Atanor* Begin() {
        if (reverse) {
            itx = ref->values.size() - 1;
            rit = ref->values.rbegin();
        }
        else {
            itx = 0;
            it = ref->values.begin();
        }
        return aTRUE;
    }
};
//---------------------------------------------------------------------------------------------------------------------

#endif



