
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
filename   : atanorbvector.h
Date       : 2017/04/01
Purpose    : vector implementation
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanorbvector_h
#define atanorbvector_h

#include "atanorint.h"
#include "atanorlong.h"
#include "atanorbyte.h"
#include "atanorstring.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorbvector;
//This typedef defines a type "bvectorMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorbvector::*bvectorMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------


class Atanorbvector : public AtanorObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging hmap<unsigned short, bvectorMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    vector<unsigned char> values;
	bool isconst;
    //---------------------------------------------------------------------------------------------------------------------
    Atanorbvector(AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
        //Do not forget your variable initialisation
		isconst = false;
    }

    Atanorbvector() {
        //Do not forget your variable initialisation
		isconst = false;
    }

    //----------------------------------------------------------------------------------------------------------------------
	void SetConst() {
		isconst = true;
	}

    Exporting Atanor* Put(Atanor* context, Atanor* value, short idthread);
	Exporting Atanor* Get(Atanor* context, Atanor* value, short idthread);
    short Type() {
        return Atanorbvector::idtype;
    }

    string Typename() {
        return "bvector";
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
		values.push_back((unsigned char)v);
	}

	void storevalue(short v) {
		Locking _lock(this);
		values.push_back((unsigned char)v);
	}

	void storevalue(wchar_t v) {
		Locking _lock(this);
		values.push_back((unsigned char)v);
	}

	void storevalue(BLONG v) {
		Locking _lock(this);
		values.push_back((unsigned char)v);
	}

	void storevalue(unsigned char v) {
		Locking _lock(this);
		values.push_back(v);
	}

	void storevalue(float v) {
		Locking _lock(this);
		values.push_back((unsigned char)v);
	}

	void storevalue(double v) {
		Locking _lock(this);
		values.push_back((unsigned char)v);
	}

    Atanor* Atom(bool forced = false) {
        if (forced || !protect || reference) {
            Atanorbvector* v = new Atanorbvector;
            Locking _lock(this);
            v->values = values;
            return v;
        }
        return this;
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

    long getinteger(int i) {
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
        return new Atanorbvector();
    }

    bool duplicateForCall() {
        return isconst;
    }

    Atanor* Newvalue(Atanor* a, short idthread) {
        Atanorbvector* v = new Atanorbvector();
        if (a->isContainer()) {
            AtanorIteration* it = a->Newiteration(false);
            for (it->Begin(); it->End() == aFALSE; it->Next()) {
                v->values.push_back(it->IteratorValue()->Byte());
            }
            it->Release();
        }
        return v;
    }


	Exporting Atanor* Loopin(AtanorInstruction*, Atanor* context, short idthread);
    Exporting AtanorIteration* Newiteration(bool direction);

    static void AddMethod(AtanorGlobal* g, string name, bvectorMethod func, unsigned long arity, string infos);
    
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
    Atanor* MethodPush(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Locking _lock(this);
        for (size_t i = 0; i < callfunc->Size(); i++)
            values.push_back(callfunc->Evaluate(i, contextualpattern, idthread)->Byte());
        return aTRUE;
    }
    Atanor* MethodPop(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Atanor* pos = aMINUSONE;
        if (callfunc->Size() == 1)
            pos = callfunc->Evaluate(0, contextualpattern, idthread);
        return Pop(pos);
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
        return new Atanorbyte(values.back());
    }

    Atanor* MethodMerge(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Atanor* v = callfunc->Evaluate(0, contextualpattern, idthread);
        Locking _lock(this);
        return Merging(v);
    }

    Atanor* MethodEditDistance(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Atanor* v = callfunc->Evaluate(0, contextualpattern, idthread);
        Locking _lock(this);
        unsigned int dst = EditDistance(v);
        return globalAtanor->Provideint(dst);
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
        Locking _lock(this);
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

    Exporting Atanor* Push(Atanor*);Exporting Atanor* Pop(Atanor*);
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


    double Sum() {
        Locking _lock(this);
        double v = 0;
        for (int i = 0; i < values.size(); i++)
            v += values[i];
        return v;
    }

    double Product() {
        Locking _lock(this);
        if (values.size() == 0)
            return 0;
        double v = values[0];
        for (int i = 1; i < values.size(); i++)
            v *= values[i];
        return v;
    }

    void Insert(int idx, Atanor* ke) {
        Locking _lock(this);
        values.insert(values.begin() + idx, ke->Integer());
    }

    Atanor* Inverse() {
        Locking _lock(this);
        Atanorbvector* vect = new Atanorbvector;
        for (long i = values.size() - 1; i >= 0; i--) {
            vect->values.push_back(values[i]);
        }
        return vect;
    }

    Exporting Atanor* Map(short idthread);Exporting Atanor* Vector(short idthread);	//---------------------------------------------------------------------------------------------------------------------

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
    Exporting Atanor* in(Atanor* context, Atanor* a, short idthread);Exporting Atanor* andset(Atanor* a, bool itself);Exporting Atanor* orset(Atanor* a, bool itself);Exporting Atanor* xorset(Atanor* a, bool itself);Exporting Atanor* plus(Atanor* a, bool itself);Exporting Atanor* minus(Atanor* a, bool itself);Exporting Atanor* multiply(Atanor* a, bool itself);Exporting Atanor* divide(Atanor* a, bool itself);Exporting Atanor* power(Atanor* a, bool itself);Exporting Atanor* shiftleft(Atanor* a, bool itself);Exporting Atanor* shiftright(Atanor* a, bool itself);Exporting Atanor* mod(Atanor* a, bool itself);
    Exporting Atanor* same(Atanor* a);
};


class AtanorIterationbvector : public AtanorIteration {
    public:
    BLONG itx;
    Atanorbvector* ref;
    Locking _lock;

    AtanorIterationbvector(Atanorbvector* v, bool d, AtanorGlobal* g = NULL) : _lock(v), AtanorIteration(d, g) {
        ref = v;
        itx = 0;
    }

    Atanor* Key() {
        return globalAtanor->Provideint(itx);
    }

    Atanor* Value() {
        return new Atanorbyte(ref->values[itx]);
    }

    string Keystring() {
        stringstream s;
        s << itx;
        return s.str();
    }

    string Valuestring() {
        string s;
        s = ref->values[itx];
        return s;
    }

	long Keyinteger() {
		return itx;
	}

	BLONG Keylong() {
		return itx;
	}

	BLONG Valuelong() {
		return ref->values[itx];
	}

	long Valueinteger() {
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



