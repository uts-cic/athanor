
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
filename   : atanorlvector.h
Date       : 2017/04/01
Purpose    : vector implementation
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanorlvector_h
#define atanorlvector_h

#include "atanorint.h"
#include "atanorlong.h"
#include "atanorstring.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorlvector;
//This typedef defines a type "lvectorMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorlvector::*lvectorMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------


class Atanorlvector : public AtanorObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging hmap<unsigned short, lvectorMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    vector<BLONG> values;
	bool isconst;
    //---------------------------------------------------------------------------------------------------------------------
    Atanorlvector(AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
        //Do not forget your variable initialisation
		isconst = false;
    }

    Atanorlvector() {
        //Do not forget your variable initialisation
		isconst = false;
    }

    //----------------------------------------------------------------------------------------------------------------------

	void SetConst() {
		isconst = true;
	}

    Exporting Atanor* Put(Atanor* idx, Atanor* value, short idthread);Exporting Atanor* Get(Atanor* context, Atanor* value, short idthread);
    Exporting Atanor* Loophaskell(Atanor* recipient, Atanor* context, Atanor* env, AtanorFunctionLambda* bd, short idthread);
	Exporting Atanor* Filter(short idthread, Atanor* env, AtanorFunctionLambda* bd, Atanor* var, Atanor* kcont, Atanor* accu, Atanor* init, bool direct);
    short Type() {
        return Atanorlvector::idtype;
    }

    string Typename() {
        return "lvector";
    }

	bool isLong() {
		return true;
	}

	bool isNumber() {
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


    void Reserve(long d) {
        Locking _lock(this);
        if (d>values.size())
            values.reserve(d);
    }

	void storevalue(long v) {
		Locking _lock(this);
		values.push_back((BLONG)v);
	}

	void storevalue(short v) {
		Locking _lock(this);
		values.push_back((BLONG)v);
	}

	void storevalue(wchar_t v) {
		Locking _lock(this);
		values.push_back((BLONG)v);
	}

	void storevalue(BLONG v) {
		Locking _lock(this);
		values.push_back(v);
	}

	void storevalue(unsigned char v) {
		Locking _lock(this);
		values.push_back((BLONG)v);
	}

	void storevalue(float v) {
		Locking _lock(this);
		values.push_back((BLONG)v);
	}

	void storevalue(double v) {
		Locking _lock(this);
		values.push_back((BLONG)v);
	}

    Atanor* Atom(bool forced = false) {
        if (forced || !protect || reference) {
            Atanorlvector* v = new Atanorlvector;
            Locking _lock(this);
            v->values = values;
            return v;
        }
        return this;
    }

    bool duplicateForCall() {
        return isconst;
    }

    Exporting Atanor* getvalue(BLONG i);

	Atanor* Value(Atanor* a) {
		return getvalue(a->Long());
	}

    double getfloat(int i) {
        Locking _lock(this);
        if (i < 0 || i >= values.size())
            return 0;
        return values[i];
    }

    float getdecimal(int i) {
        Locking _lock(this);
        if (i < 0 || i >= values.size())
            return 0;
        return values[i];
    }

    long getinteger(int i) {
        Locking _lock(this);
        if (i < 0 || i >= values.size())
            return 0;
        return values[i];
    }

    BLONG getlong(int i) {
        Locking _lock(this);
        if (i < 0 || i >= values.size())
            return 0;
        return values[i];
    }

	short getshort(int i) {
		Locking _lock(this);
		if (i < 0 || i >= values.size())
			return 0;
		return values[i];
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
        return new Atanorlvector();
    }
    
    Atanor* Newvalue(Atanor* a, short idthread) {
        Atanorlvector* v = new Atanorlvector();
        if (a->isContainer()) {
            AtanorIteration* it = a->Newiteration(false);
            for (it->Begin(); it->End() == aFALSE; it->Next()) {
                v->values.push_back(it->Valuelong());
            }
            it->Release();
        }
        return v;
    }

	Exporting Atanor* Loopin(AtanorInstruction*, Atanor* context, short idthread);
    Exporting AtanorIteration* Newiteration(bool direction);

    static void AddMethod(AtanorGlobal* g, string name, lvectorMethod func, unsigned long arity, string infos);
    
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
    Atanor* MethodRemove(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        BLONG a = callfunc->Evaluate(0, contextualpattern, idthread)->Long();
        for (size_t i = 0; i < values.size(); i++) {
            if (values[i] == a) {
                values.erase(values.begin() + i);
                return aTRUE;
            }
        }
        return aFALSE;
    }



    Atanor* MethodClear(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Clear();
        return aTRUE;
    }


    Atanor* MethodPush(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Locking _lock(this);
        for (size_t i = 0; i < callfunc->Size(); i++)
            values.push_back(callfunc->Evaluate(i, contextualpattern, idthread)->Long());
        return aTRUE;
    }

    Atanor* MethodPop(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Atanor* pos = aMINUSONE;
        if (callfunc->Size() == 1)
            pos = callfunc->Evaluate(0, contextualpattern, idthread);
        return Pop(pos);
    }

    Atanor* MethodMerge(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Atanor* v = callfunc->Evaluate(0, contextualpattern, idthread);
        return Merging(v);
    }

    Atanor* MethodReserve(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        //The separator between values
        long sz = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        Reserve(sz);
        return aTRUE;
    }

    Atanor* MethodJoin(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        //The separator between values
        string sep = callfunc->Evaluate(0, contextualpattern, idthread)->String();
        Locking _lock(this);
        bool beg = true;
        stringstream v;
        for (size_t it = 0; it < values.size(); it++) {
            if (beg == false)
                v << sep;
            beg = false;
            v << values[it];
        }

        return globalAtanor->Providestring(v.str());
    }

    Atanor* MethodLast(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Locking _lock(this);
        if (values.size() == 0)
            return aNOELEMENT;
        return globalAtanor->Provideint(values.back());
    }

    Atanor* MethodEditDistance(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Atanor* v = callfunc->Evaluate(0, contextualpattern, idthread);
        Locking _lock(this);
        unsigned int dst = EditDistance(v);
        return globalAtanor->Provideint(dst);
    }

    Atanor* MethodSum(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        BLONG v = LSum();
        return new Atanorlong(v);
    }

    Atanor* MethodProduct(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        BLONG v = LProduct();
        return new Atanorlong(v);
    }

    Atanor* MethodInsert(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        BLONG i = callfunc->Evaluate(0, contextualpattern, idthread)->Long();
        Atanor* v = callfunc->Evaluate(1, contextualpattern, idthread);
        Insert(i, v);
        return aTRUE;
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

	Atanor* MethodSort(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	//---------------------------------------------------------------------------------------------------------------------

    Exporting Atanor* Push(Atanor*);	Atanor* Push(AtanorGlobal* g, Atanor* a, short idhtread) {
        Locking _lock(this);
        values.push_back(a->Long());
        return this;
    }


    Exporting Atanor* Pop(Atanor*);
    Exporting Atanor* Unique();Exporting void Sort(bool);Exporting void Shuffle();Exporting Atanor* Merging(Atanor*);Exporting bool Permute();Exporting unsigned int EditDistance(Atanor* e);
    
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

		BLONG v = 0;
		for (; i < j; i++)
			v += values[i];

		return new Atanorlong(v);
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

		BLONG v = 1;
		for (; i < j; i++)
			v *= values[i];

		return new Atanorlong(v);
	}

	BLONG LSum() {
        Locking _lock(this);
        BLONG v = 0;
        for (int i = 0; i < values.size(); i++)
            v += values[i];
        return v;
    }

    BLONG LProduct() {
        Locking _lock(this);
        if (values.size() == 0)
            return 0;
        BLONG v = values[0];
        for (int i = 1; i < values.size(); i++)
            v *= values[i];
        return v;
    }

    void Insert(int idx, Atanor* ke) {
        Locking _lock(this);
        values.insert(values.begin() + idx, ke->Long());
    }

    Atanor* Inverse() {
        Locking _lock(this);
        Atanorlvector* vect = new Atanorlvector;
        for (long i = values.size() - 1; i >= 0; i--) {
            vect->values.push_back(values[i]);
        }
        return vect;
    }


    Exporting Atanor* Map(short idthread);Exporting Atanor* Vector(short idthread);    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
    //is returned by the Declaration method.
    Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods[idname])(contextualpattern, idthread, callfunc);
    }

    Exporting void Clear();Exporting void Clean();
    Exporting string String();
    //wstring UString() {}

    Exporting long Integer();Exporting double Float();Exporting BLONG Long();Exporting bool Boolean();
    
    //Basic operations
    Exporting long Size();
    Exporting Atanor* in(Atanor* context, Atanor* a, short idthread);
    Exporting Atanor* andset(Atanor* a, bool itself);Exporting Atanor* orset(Atanor* a, bool itself);Exporting Atanor* xorset(Atanor* a, bool itself);Exporting Atanor* plus(Atanor* a, bool itself);Exporting Atanor* minus(Atanor* a, bool itself);Exporting Atanor* multiply(Atanor* a, bool itself);Exporting Atanor* divide(Atanor* a, bool itself);Exporting Atanor* power(Atanor* a, bool itself);Exporting Atanor* shiftleft(Atanor* a, bool itself);Exporting Atanor* shiftright(Atanor* a, bool itself);Exporting Atanor* mod(Atanor* a, bool itself);
    Exporting Atanor* same(Atanor* a);
};

class AtanorIterationlvector : public AtanorIteration {
    public:

    BLONG itx;
    Atanorlvector* ref;
    Locking _lock;

    AtanorIterationlvector(Atanorlvector* v, bool d, AtanorGlobal* g = NULL) : _lock(v), AtanorIteration(d, g) {
        ref = v;
        itx = 0;
    }

    Atanor* Key() {
        return globalAtanor->Provideint(itx);
    }

    Atanor* Value() {
        return globalAtanor->Provideint(ref->values[itx]);
    }

    string Keystring() {
        stringstream s;
        s << itx;
        return s.str();
    }

    string Valuestring() {
        stringstream s;
        s << ref->values[itx];
        return s.str();
    }

    long Keyinteger() {
        return itx;
    }

	BLONG Keylong() {
		return itx;
	}

    long Valueinteger() {
        return ref->values[itx];
    }

    BLONG Valuelong() {
        return ref->values[itx];
    }

    double Keyfloat() {
        return itx;
    }

    float Valuedecimal() {
        return ref->values[itx];
    }

    double Valuefloat() {
        return ref->values[itx];
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

#endif



