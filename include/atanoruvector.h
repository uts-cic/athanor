
/*
*  Athanor: Le langage des Alpages m�de � Grenoble
*
*  Copyright (C) 2017: ATHANOR Language
* This file can only be used with the ATHANOR library or the executable
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : ATHANOR
Version    : ATANOR 0.26
filename   : atanoruvector.h
Date       : 2017/04/01
Purpose    : vector implementation
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanoruvector_h
#define atanoruvector_h

#include "atanorint.h"
#include "atanorustring.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanoruvector;
//This typedef defines a type "uvectorMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanoruvector::*uvectorMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------

class Atanoruvector : public AtanorObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging hmap<unsigned short, uvectorMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    vector<wstring> values;
	bool isconst;

    //---------------------------------------------------------------------------------------------------------------------
    Atanoruvector(AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
        //Do not forget your variable initialisation
		isconst = false;
    }

    Atanoruvector() {
        //Do not forget your variable initialisation
		isconst = false;
    }

    //----------------------------------------------------------------------------------------------------------------------

	void SetConst() {
		isconst = true;
	}

    Exporting Atanor* Put(Atanor* value, Atanor* v, short idthread);Exporting Atanor* Get(Atanor* context, Atanor* value, short idthread);
    Exporting Atanor* Loophaskell(Atanor* recipient, Atanor* context, Atanor* env, AtanorFunctionLambda* bd, short idthread);
	Exporting Atanor* Filter(short idthread, Atanor* env, AtanorFunctionLambda* bd, Atanor* var, Atanor* kcont, Atanor* accu, Atanor* init, bool direct);
    short Type() {
        return Atanoruvector::idtype;
    }

    string Typename() {
        return "uvector";
    }

	bool isString() {
		return true;
	}

    bool isContainer() {
        return true;
    }

    bool isValueContainer() {
        return true;
    }

    bool isVectorValueContainer() {
        return true;
    }

    bool isVectorContainer() {
        return true;
    }

    bool isMapContainer() {
        return false;
    }

    bool duplicateForCall() {
        return isconst;
    }

    void Reserve(long d) {
        Locking _lock(this);
        if (d > values.size())
            values.reserve(d);
    }

    void storevalue(wstring v) {
        Locking _lock(this);
        values.push_back(v);
    }

    void storevalue(wchar_t v) {
        Locking _lock(this);
        wstring w;
        w = v;
        values.push_back(w);
    }

    Atanor* Atom(bool forced = false) {
        if (forced || !protect || reference) {
			Atanoruvector* v = globalAtanor->Provideuvector();
            Locking _lock(this);
            v->values = values;
            return v;
        }
        return this;
    }

    Exporting Atanor* getvalue(BLONG i);
    wstring getustring(int i) {
        Locking _lock(this);
        if (i < 0 || i >= values.size())
            return 0;
        return values[i];
    }


	Atanor* Value(Atanor* a) {
		return getvalue(a->Long());
	}


    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in kifexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        if (exported.find(n) != exported.end())
            return true;
        return false;
    }

    Atanor* Newinstance(short idthread, Atanor* f = NULL) {
		return globalAtanor->Provideuvector();
    }

    Atanor* Newvalue(Atanor* a, short idthread) {
		Atanoruvector* v = globalAtanor->Provideuvector();
        if (a->isContainer()) {
            AtanorIteration* it = a->Newiteration(false);
            for (it->Begin(); it->End() == aFALSE; it->Next()) {
                v->values.push_back(it->Valueustring());
            }
            it->Release();
        }
        return v;
    }

	Exporting Atanor* Loopin(AtanorInstruction*, Atanor* context, short idthread);
    Exporting AtanorIteration* Newiteration(bool direction);

    static void AddMethod(AtanorGlobal* g, string name, uvectorMethod func, unsigned long arity, string infos);
    
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
    Atanor* MethodClear(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Clear();
        return aTRUE;
    }
    Atanor* MethodRemove(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        wstring a = callfunc->Evaluate(0, contextualpattern, idthread)->UString();
        for (size_t i = 0; i < values.size(); i++) {
            if (values[i] == a) {
                values.erase(values.begin() + i);
                return aTRUE;
            }
        }
        return aFALSE;
    }

    Atanor* MethodPush(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Locking _lock(this);
        for (size_t i = 0; i < callfunc->Size(); i++)
            values.push_back(callfunc->Evaluate(i, contextualpattern, idthread)->UString());
        return aTRUE;
    }

    Atanor* MethodPop(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Atanor* pos = aMINUSONE;
        if (callfunc->Size() == 1)
            pos = callfunc->Evaluate(0, contextualpattern, idthread);
        return Pop(pos);
    }

    Atanor* MethodLast(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Locking _lock(this);
        if (values.size() == 0)
            return aNOELEMENT;
        return globalAtanor->Provideustring(values.back());
    }

	Atanor* MethodSum(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return Thesum(0, values.size());

	}

    Atanor* MethodMerge(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Atanor* v = callfunc->Evaluate(0, contextualpattern, idthread);
        return Merging(v);
    }

    Atanor* MethodEditDistance(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Atanor* v = callfunc->Evaluate(0, contextualpattern, idthread);
        Locking _lock(this);
        unsigned int dst = EditDistance(v);
        return globalAtanor->Provideint(dst);
    }

    Atanor* MethodInsert(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        long i = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        Atanor* v = callfunc->Evaluate(1, contextualpattern, idthread);
        Insert(i, v);
        return aTRUE;
    }

    Atanor* MethodUnique(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        return Unique();
    }

    Atanor* MethodReserve(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        //The separator between values
        long sz = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        Reserve(sz);
        return aTRUE;
    }

    Atanor* MethodJoin(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        //The separator between values
        wstring sep = callfunc->Evaluate(0, contextualpattern, idthread)->UString();
        Locking _lock(this);
        bool beg = true;
        wstring res;
        for (size_t it = 0; it<values.size(); it++) {
            if (beg == false)
                res += sep;
            beg = false;
            res += values[it];
        }

        return globalAtanor->Provideustring(res);
    }

    Atanor* MethodShuffle(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Shuffle();
        return this;
    }

    Atanor* MethodReverse(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        return Inverse();
    }

	Atanor* MethodSort(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	//---------------------------------------------------------------------------------------------------------------------
    Exporting void addstringto(string s, int i);
	Exporting void addstringto(wchar_t s, int i);
	Exporting void addustringto(wstring ws, int i);
    Exporting Atanor* Push(Atanor*);	Atanor* Push(AtanorGlobal* g, Atanor* a, short idhtread) {
        Locking _lock(this);
        values.push_back(a->UString());
        return this;
    }
    
    Exporting Atanor* Pop(Atanor*);
    Exporting Atanor* Unique();
	Exporting void Sort(bool);
	Exporting void Shuffle();
	Exporting Atanor* Merging(Atanor*);
	Exporting bool Permute();
	Exporting unsigned int EditDistance(Atanor* e);

    void Insert(int idx, Atanor* ke) {
        Locking _lock(this);
        values.insert(values.begin() + idx, ke->UString());
    }

    Atanor* Inverse() {
        Locking _lock(this);
		Atanoruvector* vect = globalAtanor->Provideuvector(); 
        for (long i = values.size() - 1; i >= 0; i--) {
            vect->values.push_back(values[i]);
        }
        return vect;
    }

    Exporting Atanor* Map(short idthread);Exporting Atanor* Vector(short idthread);
    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
    //is returned by the Declaration method.
    Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods[idname])(contextualpattern, idthread, callfunc);
    }

	Atanor* Thesum(long i, long j) {
		long sz = values.size();
		if (!sz)
			return aNULL;

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

		wstring v;
		for (; i < j; i++)
			v += values[i];

		return globalAtanor->Provideustring(v);
	}

    Exporting void Clear();Exporting void Clean();
    Exporting string JSonString();Exporting string String();Exporting wstring UString();
    Exporting long Integer();Exporting double Float();Exporting BLONG Long();Exporting bool Boolean();

    //Basic operations
    Exporting long Size();
    Exporting Atanor* in(Atanor* context, Atanor* a, short idthread);
    Exporting Atanor* andset(Atanor* a, bool itself);Exporting Atanor* orset(Atanor* a, bool itself);Exporting Atanor* xorset(Atanor* a, bool itself);Exporting Atanor* plus(Atanor* a, bool itself);Exporting Atanor* minus(Atanor* a, bool itself);
    Exporting Atanor* same(Atanor* a);
};

class AtanorIterationuvector : public AtanorIteration {
    public:

    BLONG itx;
    Atanoruvector* ref;
    Locking _lock;

    AtanorIterationuvector(Atanoruvector* v, bool d, AtanorGlobal* g = NULL) : _lock(v), AtanorIteration(d, g) {
        ref = v;
        itx = 0;
    }

    Atanor* Key() {
        return globalAtanor->Provideint(itx);
    }

    Atanor* Value() {
        return globalAtanor->Provideustring(ref->values[itx]);
    }

    string Keystring() {
        stringstream s;
        s << itx;
        return s.str();
    }

    wstring Valueustring() {
        return ref->values[itx];
    }

    string Valuestring() {
        string s;
        s_unicode_to_utf8(s, ref->values[itx]);
        return s;
    }

	long Keyinteger() {
		return itx;
	}

	long Valueinteger() {
		string v;
		sc_unicode_to_utf8(v, ref->values[itx]);
		return conversionintegerhexa(STR(v));
	}

	BLONG Keylong() {
		return itx;
	}

	BLONG Valuelong() {
		string v;
		sc_unicode_to_utf8(v, ref->values[itx]);
		return conversionintegerhexa(STR(v));
	}

    double Keyfloat() {
        return itx;
    }

    float Valuedecimal() {
        string v;
        sc_unicode_to_utf8(v, ref->values[itx]);
        return convertfloat(STR(v));
    }

    double Valuefloat() {
        string v;
        sc_unicode_to_utf8(v, ref->values[itx]);
        return convertfloat(STR(v));
    }

    void Next() {
        if (reverse)
            itx--;
        else
            itx++;
    }

    Atanor* End() {
        if (reverse) {
            if (itx < 0)
                return aTRUE;
        }
        else {
            if (itx >= ref->values.size())
                return aTRUE;
        }
        return aFALSE;
    }

    Atanor* Begin() {
        if (reverse) {
            itx = ref->values.size() - 1;
        }
        else
            itx = 0;
        return aTRUE;
    }


};
//---------------------------------------------------------------------------------------------------------------------
class Atanoruvectorbuff : public Atanoruvector {
public:
	long idx;
	bool used;

	Atanoruvectorbuff(long i)  {
		//Do not forget your variable initialisation
		idx = i;
		used = false;
	}

	void Setreference(short r) {
		Locking _lock(this);
		reference += r;
		protect = false;
	}

	void Resetreference(short r) {
		bool clr = false;
		{
			Locking _lock(this);
			reference -= r;
			if (reference <= 0) {
				if (!protect) {
					values.clear();
					reference = 0;
					protect = true;
					clr = true;
				}
			}
		}

		if (clr) {
			Locking _lock(globalAtanor->uvectorlock);
			used = false;
			globalAtanor->uvectorempties.push_back(idx);
		}
	}

};
//---------------------------------------------------------------------------------------------------------------------


#endif



