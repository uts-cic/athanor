
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
filename   : atanorfvector.cxx
Date       : 2017/04/01
Purpose    : vector implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorhaskell.h"
#include "atanorfvector.h"
#include "atanorivector.h"
#include "atanorconstants.h"
#include "atanormap.h"
#include "atanorvector.h"
#include "constobjects.h"
#include "instructions.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, fvectorMethod>  Atanorfvector::methods;
Exporting hmap<string, string> Atanorfvector::infomethods;
Exporting bin_hash<unsigned long> Atanorfvector::exported;

Exporting short Atanorfvector::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorfvector::AddMethod(AtanorGlobal* global, string name, fvectorMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanorfvector::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();



    Atanorfvector::idtype = global->Getid("fvector");

    Atanorfvector::AddMethod(global, "reverse", &Atanorfvector::MethodReverse, P_NONE, "reverse(): reverse a vector.");
    Atanorfvector::AddMethod(global, "reserve", &Atanorfvector::MethodReserve, P_ONE, "reserve(int sz): Reserve a size of 'sz' potential element in the vector.");
    Atanorfvector::AddMethod(global, "unique", &Atanorfvector::MethodUnique, P_NONE, "unique(): remove duplicate elements.");

    Atanorfvector::AddMethod(global, "remove", &Atanorfvector::MethodRemove, P_ONE, "remove(e): remove 'e' from the vector.");

    Atanorfvector::AddMethod(global, "last", &Atanorfvector::MethodLast, P_NONE, "last(): return the last element.");
    Atanorfvector::AddMethod(global, "join", &Atanorfvector::MethodJoin, P_ONE, "join(string sep): Produce a string representation for the container.");

    Atanorfvector::AddMethod(global, "shuffle", &Atanorfvector::MethodShuffle, P_NONE, "shuffle(): shuffle the values in the vector.");
    Atanorfvector::AddMethod(global, "sort", &Atanorfvector::MethodSort, P_ONE, "sort(bool reverse): sort the elements within.");
    Atanorfvector::AddMethod(global, "sum", &Atanorfvector::MethodSum, P_NONE, "sum(): return the sum of elements.");

    Atanorfvector::AddMethod(global, "product", &Atanorfvector::MethodProduct, P_NONE, "product(): return the product of elements.");
    Atanorfvector::AddMethod(global, "push", &Atanorfvector::MethodPush, P_ATLEASTONE, "push(v): Push a value into the vector.");
    Atanorfvector::AddMethod(global, "pop", &Atanorfvector::MethodPop, P_NONE | P_ONE, "pop(i): Erase an element from the vector");
    Atanorfvector::AddMethod(global, "merge", &Atanorfvector::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");
    Atanorfvector::AddMethod(global, "editdistance", &Atanorfvector::MethodEditDistance, P_ONE, "editdistance(v): Compute the edit distance with vector 'v'.");
    Atanorfvector::AddMethod(global, "insert", &Atanorfvector::MethodInsert, P_TWO, "insert(i,v): Insert v at position i.");


    global->newInstance[Atanorfvector::idtype] = new Atanorfvector(global);
    global->RecordMethods(Atanorfvector::idtype,Atanorfvector::exported);

    return true;
}

Exporting AtanorIteration* Atanorfvector::Newiteration(bool direction) {
    return new AtanorIterationfvector(this, direction);
}


Exporting Atanor* Atanorfvector::in(Atanor* context, Atanor* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    double val = a->Float();
    if (context->isBoolean()) {
        Locking _lock(this);
        for (size_t i = 0; i < values.size(); i++) {
            if (values[i] == val)
                return aTRUE;
        }
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Atanorivector* v = (Atanorivector*)Selectaivector(context);
        Doublelocking _lock(this, v);

        for (size_t i = 0; i < values.size(); i++) {
            if (values[i] == val)
                v->values.push_back(i);
        }
        return v;
    }

    Locking _lock(this);
    for (size_t i = 0; i < values.size(); i++) {
        if (values[i] == val)
            return globalAtanor->Provideint(i);
    }

    return aNULL;
}

Exporting Atanor* Atanorfvector::getvalue(BLONG i) {
    Locking _lock(this);
    if (i<0 || i>=values.size())
        return aNOELEMENT;
    return globalAtanor->Providefloat(values[i]);
}

Exporting Atanor* Atanorfvector::Push(Atanor* a) {
    Locking _lock(this);
    values.push_back(a->Float());
    return aTRUE;
}

Exporting Atanor* Atanorfvector::Pop(Atanor* idx) {
    Locking _lock(this);
    if (!values.size())
        return aFALSE;
        
    BLONG v = idx->Integer();
    if (v == -1) {
        v = values.size() - 1;
        values.pop_back();
    }
    else {
        if (v < 0 || v >= (BLONG)values.size())
            return aFALSE;
        values.erase(values.begin() + v);
    }
    return aTRUE;
}

Exporting void Atanorfvector::Clear() {
    //To set a variable back to empty
    Locking _lock(this);
    values.clear();
}

Exporting void Atanorfvector::Clean() {
    //Your cleaning code
}

Exporting string Atanorfvector::String() {
    Locking _lock(this);
    size_t i;
    stringstream r;
    r<<"[";
    bool first=true;
    for (i=0;i<values.size();i++) {
        if (first)
            first=false;
        else
            r<<",";
        r<<values[i];
    }
    r<<"]";
    return r.str();
}


Exporting long Atanorfvector::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanorfvector::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanorfvector::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanorfvector::Boolean() {
    Locking _lock(this);
    if (values.size()==0)
        return false;
    return true;
}


//Basic operations
Exporting long Atanorfvector::Size() {
    Locking _lock(this);
    return values.size();
}

static bool compFloat(double s1, double s2) {
    if (s1<s2)
        return true;
    return false;
}

static bool icompFloat(double s1, double s2) {
    if (s1>s2)
        return true;
    return false;
}
Exporting void Atanorfvector::Sort(bool direction) {
    Locking _lock(this);
    if (direction == false)
        sort(values.begin(), values.end(), compFloat);
    else
        sort(values.begin(), values.end(), icompFloat);
}

Exporting Atanor* Atanorfvector::Merging(Atanor* ke) {
    if (!ke->isContainer()) {
        Locking _lock(this);
        values.push_back(ke->Float());
        return this;
    }

    if (ke->Size() == 0)
        return this;

    Doublelocking _lock(this, ke);
    //Three cases:
    if (ke->Type() == idtype) {
        Atanorfvector* kvect = (Atanorfvector*)ke;
        values.insert(values.end(), kvect->values.begin(), kvect->values.end());
        return this;
    }

    if (ke->Type() == a_ivector) {
        Atanorivector* kvect = (Atanorivector*)ke;
        values.insert(values.end(), kvect->values.begin(), kvect->values.end());
        return this;
    }

    AtanorIteration* itr = (AtanorIteration*)ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next())
        values.push_back(itr->Valuefloat());
    itr->Release();

    return this;
}

Exporting unsigned int Atanorfvector::EditDistance(Atanor* e) {
    unsigned int s1len, s2len, x, y, lastdiag, olddiag;
    s1len = Size();
    s2len = e->Size();

    double* v1 = new double[s1len + 1];
    double* v2 = new double[s2len + 1];

    y = max(s1len, s2len);
    for (x = 0; x < y; x++) {
        if (x < s1len)
            v1[x] = values[x];
        if (x < s2len)
            v2[x] = e->getfloat(x);
    }

    size_t* column = new size_t[s1len + 1];
    for (y = 1; y <= s1len; y++)
        column[y] = y;

    unsigned int ed;
    for (x = 1; x <= s2len; x++) {
        column[0] = x;
        for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
            olddiag = column[y];
            if (v1[y - 1] == v2[x - 1])
                ed = 0;
            else
                ed = 1;
            column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + ed);
            lastdiag = olddiag;
        }
    }

    delete[] v1;
    delete[] v2;

    s2len = column[s1len];
    delete[] column;
    return s2len;
}

Exporting Atanor* Atanorfvector::Put(Atanor* idx, Atanor* ke, short idthread) {
    Doublelocking _lock(this, ke);

    if (ke == this)
        return aTRUE;

    if (!idx->isIndex()) {
        if (ke == aNULL) {
            values.clear();
            return aTRUE;
        }

        if (ke->Type() == a_fvector) {
            //We copy all values from ke to this
            values = ((Atanorfvector*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            values.clear();
            values.reserve(ke->Size());
            AtanorIteration* itr = (AtanorIteration*)ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values.push_back(itr->Keyfloat());
            itr->Release();
            return aTRUE;
        }

        if (ke->isContainer()) {
            values.clear();
            values.reserve(ke->Size());
            AtanorIteration* itr = (AtanorIteration*)ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values.push_back(itr->Valuefloat());
            itr->Release();
            return aTRUE;
        }
        if (ke->isString()) {
            values.clear();
            string sv = ke->String();
            if (!v_comma_split_float(sv, values))
                return globalAtanor->Returnerror("Cannot set this value", idthread);
            return aTRUE;
        }
        ke = ke->Vector(idthread);
        if (!ke->isVectorContainer())
            return globalAtanor->Returnerror("Cannot set this value", idthread);

        values.clear();
        values.reserve(ke->Size());
        //We copy all values from ke to this
        for (long it = 0; it < ke->Size(); ++it)
            values.push_back(ke->getfloat(it));
        ke->Release();
        return aTRUE;
    }
    else {
        //In this specific case, we try to replace a bloc of values with a new bloc
        if (idx->isInterval()) {
            //First we remove the values...
            long lkey = idx->Integer();
            if (lkey < 0)
                lkey = values.size() + lkey;

            long rkey = ((AtanorIndex*)idx)->right->Integer();
            if (rkey < 0)
                rkey = values.size() + rkey;

            if (rkey < lkey || rkey >= values.size() || lkey >= values.size()) {
                if (globalAtanor->erroronkey)
                    globalAtanor->Returnerror("Wrong index", idthread);
                return aTRUE;
            }

            if (rkey != lkey)
                rkey--;

            while (rkey >= lkey) {
                values.erase(values.begin() + rkey);
                rkey--;
            }
            if (ke->isVectorContainer()) {
                for (int i = ke->Size() - 1; i >= 0; i--) {
                    values.insert(values.begin() + lkey, ke->getfloat(i));
                }
                return aTRUE;
            }

            if (ke->isContainer()) {
                AtanorIteration* itr = (AtanorIteration*)ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    values.insert(values.begin() + lkey, itr->Valuefloat());
                itr->Release();
                return aTRUE;
            }

            values.insert(values.begin() + lkey, ke->Float());
        }
        else {
            register long ikey = idx->Integer();
            int sz = values.size();
            int nb;
            double n = ke->Float();
            if (ikey < 0) {
                ikey = sz + ikey;
                if (ikey < 0)
                    return globalAtanor->Returnerror("Cannot set this value", idthread);
            }

            if (ikey >= sz) {
                for (nb = values.size(); nb < ikey; nb++)
                    values.push_back(0);
                values.push_back(n);
            }
            else
                values[ikey] = n;
        }
    }
    return aTRUE;
}

Exporting bool Atanorfvector::Permute() {
    Locking _lock(this);

    return next_permutation(values.begin(), values.end());
}

Exporting void Atanorfvector::Shuffle() {
    Locking _lock(this);

    size_t sz = values.size();
    size_t i, f;
    int mx = sz;
    double v;
    for (i = 0; i < sz; i++) {
        f = localrandom(mx--);
        if (mx != f) {
            v = values[mx];
            values[mx] = values[f];
            values[f] = v;
        }
    }
}

Exporting Atanor* Atanorfvector::Unique() {
    Locking _lock(this);
    Atanorfvector* kvect = globalAtanor->Providefvector();
    map<double, bool> inter;
    for (int i = 0; i < values.size(); i++) {
        if (inter.find(values[i]) == inter.end()) {
            inter[values[i]] = true;
            kvect->values.push_back(values[i]);
        }
    }
    return kvect;
}


Exporting Atanor* Atanorfvector::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {
    Locking _lock(this);

    if (!idx->isIndex()) {
        Atanor* kv = NULL;
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer()) {
            kv = Selectafvector(contextualpattern);
            if (kv->Type() == a_fvector)
                ((Atanorfvector*)kv)->values = values;
            else
                for (int i = 0; i < values.size(); i++)
                    kv->Push(globalAtanor->Providefloat(values[i]));
            return kv;
        }

        if (contextualpattern->Type() == a_int || contextualpattern->Type() == a_float)
            return globalAtanor->Provideint(values.size());
        return this;
    }

    register long ikey;
    Atanor* keyright = NULL;
    if (idx->isIndex()) {
        AtanorIndex* kind = (AtanorIndex*)idx;
        ikey = kind->left->Getinteger(idthread);
        if (kind->interval == true)
            keyright = kind->right->Get(aNULL, aNULL, idthread);
    }
    else
        ikey = idx->Getinteger(idthread);

    int iright;
    if (keyright != NULL)
        iright = keyright->Integer();

    Atanorfvector* kvect;
    if (ikey < 0)
        ikey = values.size() + ikey;

    if (ikey < 0 || ikey >= values.size()) {
        if (ikey != values.size() || keyright == NULL) {
            if (globalAtanor->erroronkey)
                return globalAtanor->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }

    if (keyright == NULL)
        return globalAtanor->Providefloat(values[ikey]);

    keyright->Release();
    if (iright < 0 || keyright == aNULL) {
        iright = values.size() + iright;
        if (iright<ikey) {
            if (globalAtanor->erroronkey)
                return globalAtanor->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    else {
        if (iright>values.size()) {
            if (globalAtanor->erroronkey)
                return globalAtanor->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    //In this case, we must create a new vector
    kvect = globalAtanor->Providefvector();
    for (int i = ikey; i < iright; i++)
        kvect->values.push_back(values[i]);
    return kvect;
}

Exporting Atanor* Atanorfvector::Map(short idthread) {
    Locking _lock(this);
    Atanormap* kmap = globalAtanor->Providemap();

    char buff[100];
    for (int it = 0; it < values.size(); it++) {
        sprintf_s(buff, 100, "%d", it);
        kmap->Push(buff, globalAtanor->Providefloat(values[it]));
    }
    return kmap;
}

Exporting Atanor* Atanorfvector::Vector(short idthread) {
    Locking _lock(this);
    Atanorvector* kvect = globalAtanor->Providevector();
    kvect->values.reserve(values.size());
    for (int i = 0; i < values.size(); i++)
        kvect->Push(globalAtanor->Providefloat(values[i]));
    return kvect;
}



Exporting Atanor* Atanorfvector::same(Atanor* a) {
    if (!a->isVectorContainer())
        return aFALSE;


    Doublelocking _lock(this, a);

    if (a->Type() != idtype) {
        if (a->Type() == a_vector) {
            if (a->Size() != values.size())
                return aFALSE;
            for (int i = 0; i < values.size(); i++) {
                if (values[i] != ((Atanorvector*)a)->values[i]->Float())
                    return aFALSE;
            }
            return aTRUE;
        }
        if (a->Type() == a_ivector) {
            if (a->Size() != values.size())
                return aFALSE;
            for (int i = 0; i < values.size(); i++) {
                if (((Atanorivector*)a)->values[i] != values[i])
                    return aFALSE;
            }
            return aTRUE;
        }
        return aFALSE;
    }

    Atanorfvector* m = (Atanorfvector*)a;
    if (m->values.size() != values.size())
        return aFALSE;

    for (int i = 0; i < values.size(); i++) {
        if (m->values[i] != values[i])
            return aFALSE;
    }
    return aTRUE;
}


Exporting Atanor* Atanorfvector::andset(Atanor* b, bool itself) {
    Atanorfvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Atanorfvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;
    long v;
    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            v = ref->values[it];
            v &= itr->Valueinteger();
            ref->values[it] = v;
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    v = b->Integer();
    long vbis;
    for (it = 0; it < ref->values.size(); it++) {
        vbis = ref->values[it];
        vbis &= v;
        ref->values[it] = vbis;
    }
    return ref;
}

Exporting Atanor* Atanorfvector::orset(Atanor* b, bool itself) {
    Atanorfvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Atanorfvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;
    long v;
    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            v = ref->values[it];
            v |= itr->Valueinteger();
            ref->values[it] = v;
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    v = b->Integer();
    long vbis;
    for (it = 0; it < ref->values.size(); it++) {
        vbis = ref->values[it];
        vbis |= v;
        ref->values[it] = vbis;
    }
    return ref;
}

Exporting Atanor* Atanorfvector::xorset(Atanor* b, bool itself) {
    Atanorfvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Atanorfvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;
    long v;
    if (b->isVectorContainer()) {
        long size = Size();

        Atanorfvector* ref = globalAtanor->Providefvector();
        Locking _lock(this);
        hmap<double, bool> store;
        for (it = 0; it < b->Size(); it++)
            store[b->getfloat(it)] = true;

        for (it = 0; it < size; it++) {
            if (store.find(values[it]) == store.end())
                store[values[it]] = true;
        }

        for (auto& i : store)
            ref->values.push_back(i.first);

        return ref;
    }

    v = b->Integer();
    long vbis;
    for (it = 0; it < ref->values.size(); it++) {
        vbis = ref->values[it];
        vbis ^= v;
        ref->values[it] = vbis;
    }
    return ref;
}


Exporting Atanor* Atanorfvector::plus(Atanor* b, bool itself) {
    Atanorfvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Atanorfvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] += itr->Valuefloat();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    double v = b->Float();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] += v;
    return ref;
}

Exporting Atanor* Atanorfvector::minus(Atanor* b, bool itself) {
    Atanorfvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Atanorfvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] -= itr->Valuefloat();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    double v = b->Float();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] -= v;
    return ref;
}

Exporting Atanor* Atanorfvector::multiply(Atanor* b, bool itself) {
    Atanorfvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Atanorfvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] *= itr->Valuefloat();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    double v = b->Float();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] *= v;
    return ref;
}

Exporting Atanor* Atanorfvector::divide(Atanor* b, bool itself) {
    Atanorfvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Atanorfvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;
    double v;
    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            v = itr->Valuefloat();
            if (v == 0) {
                ref->Release();
                return new AtanorError("Error: Divided by 0");
            }
            ref->values[it] /= v;
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    v = b->Float();
    if (v == 0) {
        ref->Release();
        return new AtanorError("Error: Divided by 0");
    }

    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] /= v;
    return ref;
}

Exporting Atanor* Atanorfvector::power(Atanor* b, bool itself) {
    Atanorfvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Atanorfvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] = pow(ref->values[it], itr->Valuefloat());
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    double v = b->Float();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] = pow(ref->values[it], v);

    return ref;
}

Exporting Atanor* Atanorfvector::shiftleft(Atanor* b, bool itself) {
    Atanorfvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Atanorfvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;
    long v;
    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            v = ref->values[it];
            v <<= itr->Valueinteger();
            ref->values[it] = v;
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    v = b->Integer();
    long vint;
    for (it = 0; it < ref->values.size(); it++) {
        vint = ref->values[it];
        vint <<= v;
        ref->values[it] = vint;
    }
    return ref;
}

Exporting Atanor* Atanorfvector::shiftright(Atanor* b, bool itself) {
    Atanorfvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Atanorfvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;
    long v;
    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            v = ref->values[it];
            v >>= itr->Valueinteger();
            ref->values[it] = v;
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    v = b->Integer();
    long vint;
    for (it = 0; it < ref->values.size(); it++) {
        vint = ref->values[it];
        vint >>= v;
        ref->values[it] = vint;
    }
    return ref;
}

Exporting Atanor* Atanorfvector::mod(Atanor* b, bool itself) {
    Atanorfvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Atanorfvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;
    long v;
    long vbis;
    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            v = itr->Valueinteger();
            if (v == 0) {
                ref->Release();
                return new AtanorError("Error: Divided by 0");
            }
            vbis = ref->values[it];
            vbis %= v;
            ref->values[it] = vbis;
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    v = b->Integer();
    if (v == 0) {
        ref->Release();
        return new AtanorError("Error: Divided by 0");
    }

    for (it = 0; it < ref->values.size(); it++) {
        vbis = ref->values[it];
        vbis %= v;
        ref->values[it] = vbis;

    }
    return ref;
}

Exporting Atanor* Atanorfvector::Loophaskell(Atanor* recipient, Atanor* context, Atanor* environment, AtanorFunctionLambda* bd, short idthread) {
    Atanor* a;
    Atanorfloat* it = globalAtanor->Providefloat();
    it->Setreference();
    
    char addvalue = 0;

    if (context != aNOELEMENT)
        addvalue = Selecttype(context);

    recipient->Putvalue(it, idthread);

    for (size_t i = 0; i < values.size(); i++) {
        it->value = values[i];

        a = bd->Get(environment, aNULL, idthread);

        if (globalAtanor->Error(idthread) || a == aBREAK) {
            if (a == aBREAK)
                break;

            it->Resetreference();
            recipient->Forcedclean();
            a->Release();
            context->Release();
            return globalAtanor->Errorobject(idthread);
        }

        if (a != aNULL) {
            context = Storealongtype(context, a, idthread, addvalue);
            a->Release();
        }
    }

    it->Resetreference();
    recipient->Forcedclean();
    return context;
}

Exporting Atanor* Atanorfvector::Filter(short idthread, Atanor* env, AtanorFunctionLambda* bd, Atanor* var, Atanor* kcont, Atanor* accu, Atanor* init, bool direct) {

    Atanor* returnval;

    bool first = false;
    
    Atanorfloat* key = globalAtanor->Providefloat();
    key->Setreference();

    if (init != aNOELEMENT) {
        accu->Putvalue(init, idthread);
        if (kcont != NULL) {
            if (direct)
                kcont->Insert(0, init);
            else
                kcont->Push(init);
        }
    }
    else
        first = true; //we are dealing with a foldr1 or a foldl1
        
    for (size_t i = 0; i < values.size(); i++) {
        key->value = values[i];
        if (first) {
            returnval = key->Atom();//We use the first value of the list to seed our accumulator variable
            first = false;
        }
        else {
            var->Putvalue(key, idthread);
            returnval = bd->Get(env, aNULL, idthread);

            if (returnval == aBREAK) {
                accu = returnval;
                break;
            }

            if (globalAtanor->Error(idthread)) {
                key->Resetreference();
                var->Forcedclean();
                if (kcont != NULL)
                    kcont->Release();
                return globalAtanor->Errorobject(idthread);
            }
        }

        if (returnval != aNULL) {
            
            accu->Putvalue(returnval, idthread);
            if (kcont != NULL) {
                if (direct)
                    kcont->Insert(0, returnval);
                else
                    kcont->Push(returnval);
            }
            returnval->Release();
        }
    }

    key->Resetreference();
    var->Forcedclean();
    if (kcont != NULL)
        return kcont;
    return accu->Value();
}

class FComp {
    public:
    AtanorCallFunction compare;
    short idthread;
    AtanorConstFloat p;
    AtanorConstFloat s;

    FComp(AtanorFunction* comp, short idt) : compare(comp), p(0), s(0), idthread(idt) {
        compare.arguments.push_back(&p);
        compare.arguments.push_back(&s);
    }

    bool get() {
        return compare.Get(aNULL, aNULL, idthread)->Boolean();
    }
};

class FComparison {
    public:
    FComp* compare;

    FComparison(FComp* c)  {
        compare = c;
    }

    bool operator() (double i, double j) {
        compare->p.value = i;
        compare->s.value = j;
        return compare->get();
    }
};

Atanor* Atanorfvector::MethodSort(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
    Atanor* comp = callfunc->Evaluate(0, contextualpattern, idthread);
    if (comp->isFunction()) {
        if (comp->Size() != 2)
            return globalAtanor->Returnerror("Expecting a comparison function with two parameters", idthread);

        FComp kcomp((AtanorFunction*)comp->Body(idthread), idthread);
        FComparison kfcomp(&kcomp);
        sort(values.begin(), values.end(), kfcomp);
        return this;
    }
    Sort(comp->Boolean());
    return this;
}


Exporting Atanor* Atanorfvector::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
    Locking _lock(this);
    Atanor* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    
    Atanor* a;
    for (long i = 0; i < values.size(); i++) {
        var->storevalue(values[i]);

        a = ins->instructions.vecteur[1]->Get(context, aNULL, idthread);

        //Continue does not trigger needInvestigate
        if (a->needInvestigate()) {
            if (a == aBREAK)
                break;
            return a;
        }

        a->Release();
    }

    return this;
}
