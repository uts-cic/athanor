
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
filename   : atanorbvector.cxx
Date       : 2017/04/01
Purpose    : vector implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorbvector.h"
#include "atanorbyte.h"
#include "atanorvector.h"
#include "atanorivector.h"
#include "atanorfvector.h"
#include "atanormap.h"
#include "instructions.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, bvectorMethod>  Atanorbvector::methods;
Exporting hmap<string, string> Atanorbvector::infomethods;
Exporting bin_hash<unsigned long> Atanorbvector::exported;

Exporting short Atanorbvector::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorbvector::AddMethod(AtanorGlobal* global, string name, bvectorMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanorbvector::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Atanorbvector::idtype = global->Getid("bvector");

    Atanorbvector::AddMethod(global, "clear", &Atanorbvector::MethodClear, P_NONE, "clear(): clear the container.");

    Atanorbvector::AddMethod(global, "reverse", &Atanorbvector::MethodReverse, P_NONE, "reverse(): reverse a vector.");
    Atanorbvector::AddMethod(global, "reserve", &Atanorbvector::MethodReserve, P_ONE, "reserve(int sz): Reserve a size of 'sz' potential element in the vector.");
    Atanorbvector::AddMethod(global, "unique", &Atanorbvector::MethodUnique, P_NONE, "unique(): remove duplicate elements.");

    Atanorbvector::AddMethod(global, "last", &Atanorbvector::MethodLast, P_NONE, "last(): return the last element.");
    Atanorbvector::AddMethod(global, "join", &Atanorbvector::MethodJoin, P_ONE, "join(string sep): Produce a string representation for the container.");
    Atanorbvector::AddMethod(global, "shuffle", &Atanorbvector::MethodShuffle, P_NONE, "shuffle(): shuffle the values in the vector.");
    Atanorbvector::AddMethod(global, "sort", &Atanorbvector::MethodSort, P_ONE, "sort(bool reverse): sort the elements within.");
    Atanorbvector::AddMethod(global, "sum", &Atanorbvector::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Atanorbvector::AddMethod(global, "product", &Atanorbvector::MethodProduct, P_NONE, "product(): return the product of elements.");
    Atanorbvector::AddMethod(global, "push", &Atanorbvector::MethodPush, P_ATLEASTONE, "push(v): Push a value into the vector.");
    Atanorbvector::AddMethod(global, "pop", &Atanorbvector::MethodPop, P_NONE | P_ONE, "pop(i): Erase an element from the vector");
    Atanorbvector::AddMethod(global, "merge", &Atanorbvector::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");
    Atanorbvector::AddMethod(global, "editdistance", &Atanorbvector::MethodEditDistance, P_ONE, "editdistance(v): Compute the edit distance with vector 'v'.");
    Atanorbvector::AddMethod(global, "insert", &Atanorbvector::MethodInsert, P_TWO, "insert(i,v): Insert v at position i.");


    global->newInstance[Atanorbvector::idtype] = new Atanorbvector(global);
    global->RecordMethods(Atanorbvector::idtype, Atanorbvector::exported);

    return true;
}

Exporting Atanor* Atanorbvector::getvalue(BLONG i) {
    Locking _lock(this);
    if (i<0 || i>values.size())
        return aNOELEMENT;
    return new Atanorbyte(values[i]);
}

Exporting AtanorIteration* Atanorbvector::Newiteration(bool direction) {
    return new AtanorIterationbvector(this, direction);
}

Exporting Atanor* Atanorbvector::in(Atanor* context, Atanor* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    if (a->Size() == 0)
        return aNULL;

    uchar val = a->String()[0];
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


Exporting Atanor* Atanorbvector::Push(Atanor* a) {
    Locking _lock(this);
    string v = a->String();
    if (v.size() > 0)
        values.push_back(v[0]);
    else
        values.push_back(0);
    return this;
}

Exporting Atanor* Atanorbvector::Pop(Atanor* idx) {
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

Exporting void Atanorbvector::Clear() {
    //To set a variable back to empty
    Locking _lock(this);
    values.clear();
}

Exporting void Atanorbvector::Clean() {
    //Your cleaning code
}

Exporting string Atanorbvector::String() {
    Locking _lock(this);
    size_t i;
    stringstream r;
    r << "[";
    bool first = true;
    for (i = 0; i < values.size(); i++) {
        if (first)
            first = false;
        else
            r << ",";
        r << '"' << values[i] << '"';
    }
    r << "]";
    return r.str();
}


Exporting long Atanorbvector::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanorbvector::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanorbvector::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanorbvector::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Atanorbvector::Size() {
    Locking _lock(this);
    return values.size();
}

Exporting Atanor* Atanorbvector::Merging(Atanor* ke) {
    if (!ke->isContainer()) {
        values.push_back(ke->Integer());
        return this;
    }

    if (ke->Size() == 0)
        return this;

    Doublelocking _lock(this, ke);
    //Three cases:
    if (ke->Type() == idtype) {
        Atanorbvector* kvect = (Atanorbvector*)ke;
        values.insert(values.end(), kvect->values.begin(), kvect->values.end());
        return this;
    }

    if (ke->Type() == a_ivector) {
        Atanorivector* kvect = (Atanorivector*)ke;
        values.insert(values.end(), kvect->values.begin(), kvect->values.end());
        return this;
    }

    if (ke->Type() == a_fvector) {
        Atanorfvector* kvect = (Atanorfvector*)ke;
        values.insert(values.end(), kvect->values.begin(), kvect->values.end());
        return this;
    }

    AtanorIteration* itr = (AtanorIteration*)ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next())
        values.push_back(itr->Valueinteger());
    itr->Release();
    return this;
}

static bool comp(uchar s1, uchar s2) {
    if (s1<s2)
        return true;
    return false;
}

static bool icomp(uchar s1, uchar s2) {
    if (s1>s2)
        return true;
    return false;
}

Exporting void Atanorbvector::Sort(bool direction) {
    Locking _lock(this);

    if (direction == false)
        sort(values.begin(), values.end(), comp);
    else
        sort(values.begin(), values.end(), icomp);
}

Exporting Atanor* Atanorbvector::same(Atanor* a) {
    if (!a->isVectorContainer())
        return aFALSE;

    Doublelocking _lock(this, a);

    if (a->Type() != idtype) {
        if (a->isVectorContainer()) {
            if (a->Size() != values.size())
                return aFALSE;
            for (int i = 0; i < values.size(); i++) {
                if (values[i] != ((Atanorvector*)a)->values[i]->Byte())
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
        if (a->Type() == a_fvector) {
            if (a->Size() != values.size())
                return aFALSE;
            for (int i = 0; i < values.size(); i++) {
                if (((Atanorfvector*)a)->values[i] != values[i])
                    return aFALSE;
            }
            return aTRUE;
        }
        return aFALSE;
    }

    Atanorbvector* m = (Atanorbvector*)a;
    if (m->values.size() != values.size())
        return aFALSE;

    for (int i = 0; i < values.size(); i++) {
        if (m->values[i] != values[i])
            return aFALSE;
    }
    return aTRUE;
}


Exporting bool Atanorbvector::Permute() {
    Locking _lock(this);

    return next_permutation(values.begin(), values.end());
}

Exporting void Atanorbvector::Shuffle() {
    Locking _lock(this);

    size_t sz = values.size();
    size_t i, f;
    int mx = sz;
    uchar v;
    for (i = 0; i < sz; i++) {
        f = localrandom(mx--);
        if (mx != f) {
            v = values[mx];
            values[mx] = values[f];
            values[f] = v;
        }
    }
}

Exporting Atanor* Atanorbvector::Unique() {
    Locking _lock(this);
    Atanorbvector* kvect = new Atanorbvector;
    map<uchar, bool> inter;
    for (int i = 0; i < values.size(); i++) {
        if (inter.find(values[i]) == inter.end()) {
            inter[values[i]] = true;
            kvect->values.push_back(values[i]);
        }
    }
    return kvect;
}

Exporting unsigned int Atanorbvector::EditDistance(Atanor* e) {
    unsigned int s1len, s2len, x, y, lastdiag, olddiag;
    s1len = Size();
    s2len = e->Size();

    long* v1 = new long[s1len + 1];
    long* v2 = new long[s2len + 1];

    y = max(s1len, s2len);
    for (x = 0; x < y; x++) {
        if (x < s1len)
            v1[x] = values[x];
        if (x < s2len)
            v2[x] = e->getinteger(x);
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


Exporting Atanor*  Atanorbvector::Put(Atanor* idx, Atanor* ke, short idthread) {
    if (ke == this)
        return aTRUE;

    Doublelocking _lock(this, ke);

    if (!idx->isIndex()) {
        if (ke == aNULL) {
            values.clear();
            return aTRUE;
        }

        if (ke->Type() == a_bvector) {
            values = ((Atanorbvector*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            values.clear();
            values.reserve(ke->Size());
            AtanorIteration* itr = (AtanorIteration*)ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values.push_back((uchar)itr->Keyinteger());
            itr->Release();
            return aTRUE;
        }

        if (ke->isContainer()) {
            values.clear();
            values.reserve(ke->Size());
            AtanorIteration* itr = (AtanorIteration*)ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values.push_back((uchar)itr->Valueinteger());
            itr->Release();
            return aTRUE;
        }
        if (ke->isString()) {
            values.clear();
            string sv = ke->String();
            if (!v_comma_split_byte(sv, values))
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
            values.push_back((uchar)ke->getinteger(it));
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
                    values.insert(values.begin() + lkey, (uchar)ke->getinteger(i));
                }
                return aTRUE;
            }

            if (ke->isContainer()) {
                AtanorIteration* itr = (AtanorIteration*)ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    values.insert(values.begin() + lkey, (uchar)itr->Valueinteger());
                itr->Release();
                return aTRUE;
            }

            values.insert(values.begin() + lkey, ke->Byte());
        }
        else {
            register long ikey = idx->Integer();
            int sz = values.size();
            int nb;
            uchar n = ke->Byte();
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

Exporting Atanor* Atanorbvector::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {
    Locking _lock(this);

    if (!idx->isIndex()) {
        Atanor* kv = NULL;
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer()) {
            kv = Selectabvector(contextualpattern);
            if (kv->Type() == a_bvector)
                ((Atanorbvector*)kv)->values = values;
            else
                for (int i = 0; i < values.size(); i++)
                    kv->Push(new Atanorbyte(values[i]));
            return kv;
        }

        if (contextualpattern->Type() == a_int || contextualpattern->Type() == a_float)
            return globalAtanor->Provideint(values.size());
        return this;
    }

    register long ikey;
    Atanor* keyright = NULL;
    ikey = idx->Getinteger(idthread);
    if (idx->isInterval())
        keyright = ((AtanorIndex*)idx)->right->Get(aNULL, aNULL, idthread);

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
        return new Atanorbyte(values[ikey]);

    long iright = keyright->Getinteger(idthread);
    Atanorbvector* kvect;
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
    kvect = new Atanorbvector;
    for (int i = ikey; i < iright; i++)
        kvect->values.push_back(values[i]);
    return kvect;
}


Exporting Atanor* Atanorbvector::Map(short idthread) {
    Locking _lock(this);
    Atanormap* kmap = globalAtanor->Providemap();

    char buff[100];
    for (int it = 0; it < values.size(); it++) {
        sprintf_s(buff, 100, "%d", it);
        kmap->Push(buff, new Atanorbyte(values[it]));
    }
    return kmap;
}

Exporting Atanor* Atanorbvector::Vector(short idthread) {
    Locking _lock(this);
    Atanorvector* kvect = globalAtanor->Providevector();
    kvect->values.reserve(values.size());
    for (int i = 0; i < values.size(); i++)
        kvect->Push(new Atanorbyte(values[i]));
    return kvect;
}


Exporting Atanor* Atanorbvector::andset(Atanor* b, bool itself) {
    Atanorbvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Atanorbvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] &= (uchar)itr->Valueinteger();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    uchar v = (uchar)b->Integer();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] &= v;
    return ref;
}

Exporting Atanor* Atanorbvector::orset(Atanor* b, bool itself) {
    Atanorbvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Atanorbvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] |= (uchar)itr->Valueinteger();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    uchar v = (uchar)b->Integer();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] |= v;
    return ref;
}

Exporting Atanor* Atanorbvector::xorset(Atanor* b, bool itself) {
    Atanorbvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Atanorbvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isVectorContainer()) {
        long size = Size();

        Atanorbvector* ref = new Atanorbvector;
        Locking _lock(this);
        hmap<short, bool> store;
        for (it = 0; it < b->Size(); it++)
            store[b->getshort(it)] = true;

        for (it = 0; it < size; it++) {
            if (store.find(values[it]) == store.end())
                store[values[it]] = true;
        }

        for (auto& i : store)
            ref->values.push_back((uchar)i.first);

        return ref;
    }

    uchar v = (uchar)b->Short();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] ^= v;
    return ref;
}


Exporting Atanor* Atanorbvector::plus(Atanor* b, bool itself) {
    Atanorbvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Atanorbvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] += (uchar)itr->Valueinteger();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    uchar v = (uchar)b->Integer();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] += v;
    return ref;
}

Exporting Atanor* Atanorbvector::minus(Atanor* b, bool itself) {
    Atanorbvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Atanorbvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] -= (uchar)itr->Valueinteger();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    uchar v = (uchar)b->Integer();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] -= v;
    return ref;
}

Exporting Atanor* Atanorbvector::multiply(Atanor* b, bool itself) {
    Atanorbvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Atanorbvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] *= (uchar)itr->Valueinteger();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    uchar v = (uchar)b->Integer();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] *= v;
    return ref;
}

Exporting Atanor* Atanorbvector::divide(Atanor* b, bool itself) {
    Atanorbvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Atanorbvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;
    uchar v;
    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            v = (uchar)itr->Valueinteger();
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

    v = (uchar)b->Integer();
    if (v == 0) {
        ref->Release();
        return new AtanorError("Error: Divided by 0");
    }

    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] /= v;
    return ref;
}

Exporting Atanor* Atanorbvector::power(Atanor* b, bool itself) {
    Atanorbvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Atanorbvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] = pow((double)ref->values[it], itr->Valuefloat());
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    double v = b->Float();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] = pow((double)ref->values[it], v);

    return ref;
}

Exporting Atanor* Atanorbvector::shiftleft(Atanor* b, bool itself) {
    Atanorbvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Atanorbvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] <<= (uchar)itr->Valueinteger();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    uchar v = (uchar)b->Integer();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] <<= v;
    return ref;
}

Exporting Atanor* Atanorbvector::shiftright(Atanor* b, bool itself) {
    Atanorbvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Atanorbvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] >>= (uchar)itr->Valueinteger();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    uchar v = (uchar)b->Integer();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] <<= v;
    return ref;
}

Exporting Atanor* Atanorbvector::mod(Atanor* b, bool itself) {
    Atanorbvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Atanorbvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;
    uchar v;

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            v = (uchar)itr->Valueinteger();
            if (v == 0) {
                ref->Release();
                return new AtanorError("Error: Divided by 0");
            }

            ref->values[it] %= v;
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    v = (uchar)b->Integer();
    if (v == 0) {
        ref->Release();
        return new AtanorError("Error: Divided by 0");
    }

    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] %= v;
    return ref;
}

class BComp {
    public:
    AtanorCallFunction compare;
    short idthread;
    Atanorbyte p;
    Atanorbyte s;

    BComp(AtanorFunction* comp, short idt) : compare(comp), p(0), s(0), idthread(idt) {
        p.Setreference();
        s.Setreference();
        compare.arguments.push_back(&p);
        compare.arguments.push_back(&s);
    }

    bool get() {
        return compare.Get(aNULL, aNULL, idthread)->Boolean();
    }
};

class BComparison {
    public:
    BComp* compare;

    BComparison(BComp* c)  {
        compare = c;
    }

    bool operator() (uchar i, uchar j) {
        compare->p.value = i;
        compare->s.value = j;
        return compare->get();
    }
};

Atanor* Atanorbvector::MethodSort(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
    Atanor* comp= callfunc->Evaluate(0, contextualpattern, idthread);
    if (comp->isFunction()) {
        if (comp->Size() != 2)
            return globalAtanor->Returnerror("Expecting a comparison function with two parameters", idthread);

        BComp kcomp((AtanorFunction*)comp->Body(idthread), idthread);
        BComparison kfcomp(&kcomp);
        sort(values.begin(), values.end(), kfcomp);
        return this;
    }
    Sort(comp->Boolean());
    return this;
}



Exporting Atanor* Atanorbvector::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
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
