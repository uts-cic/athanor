
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
filename   : atanorvector.h
Date       : 2017/04/01
Purpose    : vector implementation
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanorvector_h
#define atanorvector_h

#include "atanorint.h"
#include "atanorfloat.h"
#include "atanorstring.h"

class AtanorCallFunctionHaskell;
class AtanorFunctionLambda;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorvector;
//This typedef defines a type "vectorMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorvector::*vectorMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------
class AtanorIterationvector;

//---------------------------------------------------------------------------------------------------------------------

class Atanorvector : public AtanorObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging hmap<unsigned short, vectorMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    vector<Atanor*> values;
    bool merge;
	bool isconst;

    //---------------------------------------------------------------------------------------------------------------------
    Atanorvector(AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
        //Do not forget your variable initialisation
        merge = false;
		isconst = false;
    }

    Atanorvector() {
        //Do not forget your variable initialisation
        merge = false;
		isconst = false;
    }

    //----------------------------------------------------------------------------------------------------------------------
    Exporting virtual Atanor* Put(Atanor* index, Atanor* value, short idthread);
    Exporting virtual Atanor* Get(Atanor* context, Atanor* value, short idthread);

	void SetConst() {
		isconst = true;
	}

    void Setmerge() {
        merge = true;
    }

    virtual bool isMerge() {
        return merge;
    }

    Exporting Atanor* Getvalues(AtanorDeclaration* dom, bool duplicate);
    Exporting bool isUnified(AtanorDeclaration* dom);
    Exporting bool Unify(AtanorDeclaration* dom, Atanor* a);
    Exporting bool Insertvalue(Atanor* dom, Atanor* v, basebin_hash<Atanor*>&);
    Exporting Atanor* ExtractPredicateVariables(Atanor* contextualpattern, AtanorDeclaration* dom, Atanor* c, Atanor* e, short idthread, bool root);
    Exporting Atanor* EvaluePredicateVariables(Atanor* context, AtanorDeclaration* dom);

    Exporting Atanor* Loophaskell(Atanor* recipient, Atanor* context, Atanor* env, AtanorFunctionLambda* bd, short idthread);
    Exporting Atanor* Filter(short idthread, Atanor* env, AtanorFunctionLambda* bd, Atanor* var, Atanor* kcont, Atanor* accu, Atanor* init, bool direct);

    virtual short Type() {
        return Atanorvector::idtype;
    }

    string Typename() {
        return "vector";
    }

    bool isContainer() {
        return true;
    }

    bool isVectorContainer() {
        return true;
    }

    virtual Atanor* Atom(bool forced = false) {
        if (forced) {
            Atanorvector* v = globalAtanor->Providevector();
            Atanor* a;
            Locking _lock(this);
            for (size_t i = 0; i < values.size(); i++) {
                a = values[i]->Atom();
                a->Setreference(1);
                v->values.push_back(a);
            }
            
            return v;
        }
        return this;
    }

    virtual bool duplicateForCall() {
        return isconst;
    }

    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in kifexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        if (exported.find(n) != exported.end())
            return true;
        return false;
    }

    virtual Atanor* Newinstance(short idthread, Atanor* f = NULL) {
        return globalAtanor->Providevector();
    }

    Atanor* Newvalue(Atanor* a, short idthread) {
        Atanorvector* v = globalAtanor->Providevector();
        if (a->isContainer()) {
            AtanorIteration* it = a->Newiteration(false);
            for (it->Begin(); it->End() == aFALSE; it->Next()) {
                v->values.push_back(it->IteratorValue());
            }
            it->Release();
        }
        return v;
    }

    void AddInstruction(Atanor* a) {
        a->Setreference(1);
        values.push_back(a);
    }



    Exporting AtanorIteration* Newiteration(bool direction);


    static void AddMethod(AtanorGlobal* g, string name, vectorMethod func, unsigned long arity, string infos);
    
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
    Atanor* getvalue(BLONG i) {
        Locking _lock(this);
        if (i < 0 || i >= values.size())
            return aNOELEMENT;
        return values[i];
    }
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
    Exporting void Setreference(short r = 1);
    Exporting void Resetreference(short r = 1);
	void Setprotect(bool n) {
		Locking _lock(this);
		protect = n;
		for (size_t i = 0; i<values.size(); i++)
			values[i]->Setprotect(n);
	}

	void Protectcontainervalues() {
		Locking _lock(this);
		protect = true;
		for (size_t i = 0; i<values.size(); i++)
			values[i]->Setprotect(true);
	}


    void Popping() {
        Locking _lock(this);
        protect = false;
        if (reference <= 0)
            protect = true;
        for (size_t i = 0; i < values.size(); i++)
            values[i]->Popping();
    }
    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
    Atanor* MethodClear(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Clear();
        return aTRUE;
    }
    Atanor* MethodFlatten(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Atanor* res = Selectavector(contextualpattern);
        Flattening(res, this);
        return res;
    }
    
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

    Atanor* MethodLast(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        if (values.size() == 0)
            return aNOELEMENT;
        return values.back();
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

    Atanor* MethodSort(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        bool d = false;
        Atanor* comp = callfunc->Evaluate(0, contextualpattern, idthread);
        if (comp->isFunction()) {
            comp = comp->Body(idthread);
            if (comp == NULL || comp->Size() != 2)
                return globalAtanor->Returnerror("Expecting a comparison function with two parameters", idthread);
        }
        else {
            d = comp->Boolean();
            comp = NULL;
        }
        return Sort((AtanorFunction*)comp, idthread, d);
    }

    Atanor* MethodSortint(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        bool d = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
        return Sortint(idthread, d);
    }

    Atanor* MethodSortfloat(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        bool d = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
        return Sortfloat(idthread, d);
    }

    Atanor* MethodSortstring(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        bool d = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
        return Sortstring(idthread, d);
    }

    Atanor* MethodSortustring(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        bool d = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
        return Sortustring(idthread, d);
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
        string res;
        for (size_t it = 0; it<values.size();it++) {
            if (beg == false)
                res += sep;
            beg = false;
            res += values[it]->String();
        }

        return globalAtanor->Providestring(res);
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

    Atanor* MethodRemove(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Atanor* a = callfunc->Evaluate(0, contextualpattern, idthread);
        size_t found = -1;
        for (size_t i = 0; i < values.size(); i++) {
            if (values[i]->same(a)->Boolean()) {
                found = i;
                break;
            }
        }
        if (found != -1) {
            values[found]->Resetreference(reference + 1);
            values.erase(values.begin() + found);
            return aTRUE;
        }
        return aFALSE;
    }

    //---------------------------------------------------------------------------------------------------------

    //Raw push
    Atanor* push(Atanor* v) {
        values.push_back(v);
        v->Setreference();
        return this;
    }

    Exporting virtual Atanor* Push(Atanor*);
    Exporting Atanor* Push(AtanorGlobal* g, Atanor* a, short idhtread);

    Exporting virtual Atanor* Pop(Atanor*);

    void addstringto(string s, int i) {
        Locking _lock(this);
        if (values.size() == 0)
            return;

        Atanor* ke;
        if (i < 0) {
            ke = values.back();
        }
        else {
            if (i >= values.size())
                return;
            ke = values[i];
        }
        ke->addstringto(s);
    }

    void addustringto(wstring s, int i) {
        Locking _lock(this);
        if (values.size() == 0)
            return;

        Atanor* ke;
        if (i < 0) {
            ke = values.back();
        }
        else {
            if (i >= values.size())
                return;
            ke = values[i];
        }
        ke->addustringto(s);
    }

    void addstringto(wchar_t s, int i) {
        Locking _lock(this);
        if (values.size() == 0)
            return;

        Atanor* ke;
        if (i < 0) {
            ke = values.back();
        }
        else {
            if (i >= values.size())
                return;
            ke = values[i];
        }
        ke->addstringto(s);
    }
    
    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
    //is returned by the Declaration method.
    Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods[idname])(contextualpattern, idthread, callfunc);
    }

    Atanor* Last() {
        if (values.size() == 0)
            return aNULL;
        return values.back();
    }

    Exporting Atanor* Sort(AtanorFunction* comp, short idthread, bool d);Exporting Atanor* Sortstring(short idthread, bool d);Exporting Atanor* Sortustring(short idthread, bool d);Exporting Atanor* Sortint(short idthread, bool d);Exporting Atanor* Sortfloat(short idthread, bool d);Exporting Atanor* Unique();
    Atanor* Inverse() {
        Locking _lock(this);
        Atanorvector* vect = globalAtanor->Providevector();
        for (long i = values.size() - 1; i >= 0; i--) {
            vect->Push(values[i]);
        }
        return vect;
    }

    Exporting void Shuffle();Exporting Atanor* Merging(Atanor*);
    Exporting bool Permute();Exporting unsigned int EditDistance(Atanor* e);
    Exporting void Insert(int idx, Atanor* ke);

    Exporting Atanor* Map(short idthread);
    Atanor* Vector(short idthread) {
        return this;
    }

    Exporting Atanor* Thesum(long i, long j);
    Exporting Atanor* Theproduct(long i, long j);

    double Sum() {
        Locking _lock(this);
        double v = 0;
        for (int i = 0; i < values.size(); i++)
            v += values[i]->Float();
        return v;
    }

    double Product() {
        Locking _lock(this);
        if (values.size() == 0)
            return 0;
        double v = values[0]->Float();
        for (int i = 1; i < values.size(); i++)
            v *= values[i]->Float();
        return v;
    }


    Exporting void Clear();
    Exporting void Clean();

    Exporting string JSonString();
    Exporting string String();

    //wstring UString() {}

    Exporting long Integer();
    Exporting double Float();
    Exporting BLONG Long();
    Exporting bool Boolean();


    //Basic operations
    void Reserve(long d) {
        Locking _lock(this);
        if (d > values.size())
            values.reserve(d);
    }

    Exporting long Size();

    Exporting Atanor* in(Atanor* context, Atanor* a, short idthread);

    Exporting Atanor* andset(Atanor* a, bool itself);
    Exporting Atanor* orset(Atanor* a, bool itself);
    Exporting Atanor* xorset(Atanor* a, bool itself);
    Exporting Atanor* plus(Atanor* a, bool itself);
    Exporting Atanor* minus(Atanor* a, bool itself);
    Exporting Atanor* multiply(Atanor* a, bool itself);
    Exporting Atanor* divide(Atanor* a, bool itself);
    Exporting Atanor* power(Atanor* a, bool itself);
    Exporting Atanor* shiftleft(Atanor* a, bool itself);
    Exporting Atanor* shiftright(Atanor* a, bool itself);
    Exporting Atanor* mod(Atanor* a, bool itself);

    Exporting virtual Atanor* same(Atanor* a);

};
//---------------------------------------------------------------------------------
class AtanorIterationvector : public AtanorIteration {
    public:
    BLONG itx;
    Atanorvector* ref;
    Locking _lock;

    AtanorIterationvector(Atanorvector* v, bool d, AtanorGlobal* g = NULL) : _lock(v), AtanorIteration(d, g) {
        ref = v;
        itx = 0;
    }

    Atanor* Key() {
        return globalAtanor->Provideint(itx);
    }

    Atanor* Value() {
        return ref->values[itx];
    }

    string Keystring() {
        stringstream s;
        s << itx;
        return s.str();
    }

    string Valuestring() {
        return ref->values[itx]->String();
    }

    long Keyinteger() {
        return itx;
    }

    long Valueinteger() {
        return ref->values[itx]->Integer();
    }

    double Keyfloat() {
        return itx;
    }

    float Valuedecimal() {
        return ref->values[itx]->Decimal();
    }

    double Valuefloat() {
        return ref->values[itx]->Float();
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

//---------------------------------------------------------------------------------
class AtanorConstvector : public Atanorvector {
    public:
    bool evaluate;

    AtanorConstvector(AtanorGlobal* g, Atanor* parent = NULL) : Atanorvector(g, parent) {
        //Do not forget your variable initialisation
        evaluate = false;
    }

    AtanorConstvector() : Atanorvector() {
        //Do not forget your variable initialisation
        evaluate = false;
    }

    bool isAssignable() {
        return true;
    }

    Exporting void Prepare(Atanor* env, short idthread);
    
    bool baseValue() {
        if (evaluate)
            return false;
        return true;
    }

    short Type() {
        return a_constvector;
    }

    virtual Atanor* Atom(bool forced = false) {
        Atanorvector* v = globalAtanor->Providevector();
        Locking _lock(this);
        for (size_t i = 0; i < values.size(); i++)
            v->Push(values[i]);
        return v;
    }

    bool isConst() {
        return true;
    }

    bool duplicateForCall() {
        return true;
    }

    Exporting bool Checkvariable();
    Exporting bool Setvalue(Atanor* index, Atanor* value, short idthread, bool strict = false);

    Exporting Atanor* Put(Atanor* index, Atanor* value, short idthread);
    Exporting Atanor* Get(Atanor* index, Atanor* value, short idthread);
    Atanor* Newinstance(short idthread, Atanor* f = NULL) {
        return this;
    }


    bool isEvaluate() {
        return evaluate;
    }

    void Setevaluate(bool v) {
        evaluate = v;
    }

    Exporting Atanor* same(Atanor* a);
    void Resetreference(short inc) {}
    void Release() {}
};

class AtanorConstvectormerge : public AtanorConstvector {
    public:

    Exporting Atanor* ExtractPredicateVariables(Atanor* contextualpattern, AtanorDeclaration* dom, Atanor* c, Atanor* e, short idthread, bool root);	AtanorConstvectormerge(AtanorGlobal* g, Atanor* parent = NULL) : AtanorConstvector(g, parent) {}

    short Type() {
        return a_vectormerge;
    }

    Exporting string String();
    Atanor* Atom(bool forced = false) {
        Atanorvector* v = globalAtanor->Providevector();
        v->merge = true;
        Locking _lock(this);
        for (size_t i = 0; i < values.size(); i++)
            v->Push(values[i]);
        return v;
    }

    bool isMerge() {
        return true;
    }

};


//---------------------------------------------------------------------------------
class AtanorIterationInfinitevector : public AtanorIteration {
    public:
    long itx;
    int inc;

    AtanorIterationInfinitevector(bool d, AtanorGlobal* g = NULL) : AtanorIteration(d, g) {}


    Atanor* Key() {
        return globalAtanor->Provideint(itx);
    }

    Atanor* Value() {
        return globalAtanor->Provideint(itx);
    }

    long Keyinteger() {
        return itx;
    }

    long Valueinteger() {
        return itx;
    }

    double Keyfloat() {
        return itx;
    }

    double Valuefloat() {
        return itx;
    }

    void Next() {
        itx += inc;
    }

    long IteratorKeyInteger() {
        return itx;
    }

    double IteratorKeyFloat() {
        return itx;
    }

    Atanor* End() {
        return aFALSE;
    }

    Atanor* Begin() {
        return aTRUE;
    }

};

//---------------------------------------------------------------------------------
class AtanorIterationInfinitefloatvector : public AtanorIteration {
    public:
    double itx;
    double inc;

    AtanorIterationInfinitefloatvector(bool d, AtanorGlobal* g = NULL) : AtanorIteration(d, g) {}


    Atanor* Key() {
        return globalAtanor->Providefloat(itx);
    }

    Atanor* Value() {
        return globalAtanor->Providefloat(itx);
    }

    long Keyinteger() {
        return (long)itx;
    }

    long Valueinteger() {
        return (long)itx;
    }

    double Keyfloat() {
        return itx;
    }

    double Valuefloat() {
        return itx;
    }

    void Next() {
        itx += inc;
    }

    long IteratorKeyInteger() {
        return (long)itx;
    }

    double IteratorKeyFloat() {
        return itx;
    }

    Atanor* End() {
        return aFALSE;
    }

    Atanor* Begin() {
        return aTRUE;
    }

};

//---------------------------------------------------------------------------------
class AtanorInfinitevector : public AtanorTracked {
    public:

    Atanor* seed;
    Atanor* step;
    int direction;
    bool clean;
    bool compute;

    AtanorInfinitevector(int d, AtanorGlobal* global, Atanor* parent = NULL) : AtanorTracked(a_infinitive, global, parent) {
        clean = false;
        seed = aNULL;
        compute = false;
        if (d == 1)
            step = aONE;
        else
            step = aMINUSONE;
        direction = d;
    }

    bool isContainer() {
        return true;
    }

    Atanor* Newinstance(short idthread, Atanor* f = NULL) {
        return globalAtanor->Providevector();
    }

    Exporting AtanorIteration* Newiteration(bool direction);
    void AddInstruction(Atanor* s) {
        if (seed == aNULL)
            seed = s;
        else {
            step = s->Atom();
            step->Setreference();
            clean = true;
        }
    }

    Atanor* Put(Atanor* index, Atanor* value, short idthread) {
        return this;
    }

    Exporting string String();
    long Size() {
        return 0;
    }

    bool isInfinite() {
        return true;
    }

    void Resetreference(short inc) {}
    void Release() {}

};

class AtanorCycleVector : public AtanorTracked {
    public:

    Atanor* base;
    Atanor* value;
    bool repeat;

    AtanorCycleVector(Atanor* v, bool r, AtanorGlobal* g, Atanor* parent = NULL) : AtanorTracked(a_cycle, g, parent) {
        base = v;
        value = aNULL;
        repeat = r;
    }

    Exporting Atanor* Newinstance(short idthread, Atanor* f = NULL);
    Exporting AtanorIteration* Newiteration(bool direction);
    Exporting Atanor* Get(Atanor* context, Atanor* value, short idthread);
    bool isInfinite() {
        return true;
    }

    void Resetreference(short inc) {}
    void Release() {}

};

class AtanorReplicateVector : public AtanorTracked {
    public:

    Atanor* base;
    Atanor* nbbase;
    Atanor* value;
    Atanor* nb;

    AtanorReplicateVector(Atanor* v, AtanorGlobal* g, Atanor* parent = NULL) : AtanorTracked(a_replicate, g, parent) {
        base = v;
        nbbase = aNULL;
        value = aNULL;
        nb = aNULL;
    }

    Exporting Atanor* Get(Atanor* context, Atanor* value, short idthread);Exporting Atanor* Newiterator(bool);
    Exporting Atanor* Newinstance(short idthread, Atanor* f = NULL);
    Exporting AtanorIteration* Newiteration(bool direction);
    void AddInstruction(Atanor* v) {
        nbbase = v;
    }

    bool isInfinite() {
        return true;
    }

    void Clear() {
        value->Resetreference();
        value = aNULL;
        nb->Resetreference();
        nb = aNULL;
    }

    void Resetreference(short inc) {}
    void Release() {}

};

class AtanorIteratorCycleElement : public AtanorIteration {
    public:
    AtanorCycleVector* cycle;
    int itx;
    AtanorGlobal* global;

    AtanorIteratorCycleElement(AtanorCycleVector* k, AtanorGlobal* g = NULL) : AtanorIteration(false, g) {
        cycle = k;
        itx = 0;
        global = g;
    }

    ~AtanorIteratorCycleElement() {
        cycle->value->Resetreference();
        cycle->value = aNULL;
    }

    Atanor* Key() {
        return globalAtanor->Provideint(itx);
    }

    long Keyinteger() {
        return itx;
    }

    double Keyfloat() {
        return itx;
    }

    Atanor* Value() {
        return cycle->value;
    }

    void Next() {
        itx++;
    }

    Atanor* End() {
        return aFALSE;
    }

    Atanor* Begin() {
        itx = 0;
        return aTRUE;
    }
};

class AtanorIteratorCycleVector : public AtanorIteration {
    public:
    AtanorCycleVector* cycle;
    AtanorIteration* iter;
    AtanorGlobal* global;

    AtanorIteratorCycleVector(AtanorCycleVector* k, AtanorGlobal* g = NULL) : AtanorIteration(false, g) {
        cycle = k;
        global = g;
        iter = (AtanorIteration*)cycle->value->Newiteration(false);
    }

    ~AtanorIteratorCycleVector() {
        iter->Release();
        cycle->value->Resetreference();
        cycle->value = aNULL;
    }

    Atanor* Key() {
        return iter->Key();
    }

    Atanor* Value() {
        return iter->Value();
    }

    string Keystring() {
        return iter->Keystring();
    }

    string Valuestring() {
        return iter->Valuestring();
    }

    long Keyinteger() {
        return iter->Keyinteger();
    }

    long Valueinteger() {
        return iter->Valueinteger();
    }

    double Keyfloat() {
        return iter->Keyfloat();
    }

    double Valuefloat() {
        return iter->Valuefloat();
    }

    void Next() {
        return iter->Next();
    }

    Atanor* End() {
        return iter->End();
    }

    Atanor* Begin() {
        return iter->Begin();
    }
};

class AtanorIteratorReplicate : public AtanorIteration {
    public:
    AtanorReplicateVector* replicate;
    int itx;
    int mx;
    AtanorGlobal* global;

    AtanorIteratorReplicate(AtanorReplicateVector* k, AtanorGlobal* g = NULL) : AtanorIteration(false, g) {
        global = g;
        replicate = k;
        itx = 0;
        mx = k->nb->Integer();
    }

    ~AtanorIteratorReplicate() {
        replicate->Clear();
    }

    Atanor* Key() {
        return globalAtanor->Provideint(itx);
    }

    long Keyinteger() {
        return itx;
    }

    BLONG Keylong() {
        return itx;
    }

    double Keyfloat() {
        return itx;
    }

    Atanor* Value() {
        return replicate->value;
    }

    void Next() {
        itx++;
    }

    Atanor* End() {
        if (itx >= mx)
            return aTRUE;
        return aFALSE;
    }

    Atanor* Begin() {
        itx = 0;
        return aTRUE;
    }
};

//---------------------------------------------------------------------------------------------------------------------
class Atanorvectorbuff : public Atanorvector {
    public:
    long idx;
    bool used;

    Atanorvectorbuff(long i)  {
        //Do not forget your variable initialisation
        idx = i;
        used = false;
    }

    Exporting void Resetreference(short r);

};
//---------------------------------------------------------------------------------------------------------------------

#endif
