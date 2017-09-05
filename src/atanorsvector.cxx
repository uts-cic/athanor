
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
filename   : atanorsvector.cxx
Date       : 2017/04/01
Purpose    : vector implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorhaskell.h"
#include "atanorsvector.h"
#include "atanorstring.h"
#include "atanormap.h"
#include "atanorvector.h"
#include "atanorivector.h"
#include "atanorfvector.h"
#include "constobjects.h"
#include "instructions.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, svectorMethod>  Atanorsvector::methods;
Exporting hmap<string, string> Atanorsvector::infomethods;
Exporting bin_hash<unsigned long> Atanorsvector::exported;

Exporting short Atanorsvector::idtype = 0;

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorsvector::AddMethod(AtanorGlobal* global, string name, svectorMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanorsvector::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Atanorsvector::idtype = global->Getid("svector");

    Atanorsvector::AddMethod(global, "clear", &Atanorsvector::MethodClear, P_NONE, "clear(): clear the container.");

    Atanorsvector::AddMethod(global, "remove", &Atanorsvector::MethodRemove, P_ONE, "remove(e): remove 'e' from the vector.");

    Atanorsvector::AddMethod(global, "sum", &Atanorsvector::MethodSum, P_NONE, "sum(): concatenate the strings in the vector.");
    Atanorsvector::AddMethod(global, "reverse", &Atanorsvector::MethodReverse, P_NONE, "reverse(): reverse a vector.");
    Atanorsvector::AddMethod(global, "unique", &Atanorsvector::MethodUnique, P_NONE, "unique(): remove duplicate elements.");
    Atanorsvector::AddMethod(global, "reserve", &Atanorsvector::MethodReserve, P_ONE, "reserve(int sz): Reserve a size of 'sz' potential element in the vector.");
    Atanorsvector::AddMethod(global, "last", &Atanorsvector::MethodLast, P_NONE, "last(): return the last element.");
    Atanorsvector::AddMethod(global, "join", &Atanorsvector::MethodJoin, P_ONE, "join(string sep): Produce a string representation for the container.");
    Atanorsvector::AddMethod(global, "shuffle", &Atanorsvector::MethodShuffle, P_NONE, "shuffle(): shuffle the values in the vector.");
    Atanorsvector::AddMethod(global, "sort", &Atanorsvector::MethodSort, P_ONE, "sort(bool reverse): sort the elements within.");
    Atanorsvector::AddMethod(global, "push", &Atanorsvector::MethodPush, P_ATLEASTONE, "push(v): Push a value into the vector.");
    Atanorsvector::AddMethod(global, "pop", &Atanorsvector::MethodPop, P_NONE | P_ONE, "pop(i): Erase an element from the vector");
    Atanorsvector::AddMethod(global, "merge", &Atanorsvector::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");
    Atanorsvector::AddMethod(global, "editdistance", &Atanorsvector::MethodEditDistance, P_ONE, "editdistance(v): Compute the edit distance with vector 'v'.");
    Atanorsvector::AddMethod(global, "insert", &Atanorsvector::MethodInsert, P_TWO, "insert(i,v): Insert v at position i.");




    global->newInstance[Atanorsvector::idtype] = new Atanorsvector(global);
    global->newInstance[a_vectorthrough] = global->newInstance[Atanorsvector::idtype];
    global->RecordMethods(Atanorsvector::idtype, Atanorsvector::exported);
    global->RecordMethods(a_vectorthrough, Atanorsvector::exported);

    return true;
}

Exporting AtanorIteration* Atanorsvector::Newiteration(bool direction) {
    return new AtanorIterationsvector(this, direction);
}

Exporting Atanor* Atanorsvector::in(Atanor* context, Atanor* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    string val = a->String();
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


Exporting Atanor* Atanorsvector::getvalue(BLONG i) {
    Locking _lock(this);
    if (i < 0 || i >= values.size())
        return aNOELEMENT;
    return globalAtanor->Providestring(values[i]);
}


Exporting Atanor* Atanorsvector::Push(Atanor* a) {
    Locking _lock(this);
    values.push_back(a->String());
    return aTRUE;
}

Exporting Atanor* Atanorsvector::Pop(Atanor* idx) {
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

Exporting void Atanorsvector::Clear() {
    //To set a variable back to empty
    Locking _lock(this);
    values.clear();
}

Exporting void Atanorsvector::Clean() {
    //Your cleaning code
}

Exporting string Atanorsvector::JSonString() {
    string res;
    res = "[";
    bool beg = true;
    string sx;
    for (int i = 0; i < values.size(); i++) {
        if (beg == false)
            res += ",";
        beg = false;
        sx = values[i];
        jstringing(res, sx);
    }
    res += "]";
    return res;
}


Exporting string Atanorsvector::String() {
    string res;
    res = "[";
    bool beg = true;
    string sx;
    for (int i = 0; i < values.size(); i++) {
        if (beg == false)
            res += ",";
        beg = false;
        sx = values[i];
        stringing(res, sx);
    }
    res += "]";
    return res;
}


Exporting long Atanorsvector::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanorsvector::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanorsvector::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanorsvector::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}

Exporting void Atanorsvector::addustringto(wstring ws, int i) {
    Locking _lock(this);
    if (values.size() == 0)
        return;

    string s;
    s_unicode_to_utf8(s, ws);
    if (i < 0)
        values.back() += s;
    else {
        if (i >= values.size())
            return;
        values[i] += s;
    }
}

Exporting void Atanorsvector::addstringto(wchar_t wc, int i) {
    Locking _lock(this);
    if (values.size() == 0)
        return;

    string s = c_unicode_to_utf8(wc);
    if (i < 0)
        values.back() += s;
    else {
        if (i >= values.size())
            return;
        values[i] += s;
    }
}

Exporting void Atanorsvector::addstringto(string s, int i) {
    Locking _lock(this);
    if (values.size() == 0)
        return;

    if (i < 0)
        values.back() += s;
    else {
        if (i >= values.size())
            return;
        values[i] += s;
    }
}


//Basic operations
Exporting long Atanorsvector::Size() {
    Locking _lock(this);
    return values.size();
}

Exporting unsigned int Atanorsvector::EditDistance(Atanor* e) {
    unsigned int s1len, s2len, x, y, lastdiag, olddiag;
    s1len = Size();
    s2len = e->Size();

    string* v1 = new string[s1len + 1];
    string* v2 = new string[s2len + 1];

    y = max(s1len, s2len);
    for (x = 0; x < y; x++) {
        if (x < s1len)
            v1[x] = values[x];
        if (x < s2len)
            v2[x] = e->getstring(x);
    }

    size_t* column = new size_t[s1len + 1];
    for (y = 1; y <= s1len; y++)
        column[y] = y;

    unsigned int ed;
    for (x = 1; x <= s2len; x++) {
        column[0] = x;
        for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
            olddiag = column[y];
            ed = StringEditDistance(v1[y - 1], v2[x - 1]);
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


Exporting void Atanorsvector::Shuffle() {
    Locking _lock(this);

    size_t sz = values.size();
    size_t i, f;
    int mx = sz;
    string v;
    for (i = 0; i < sz; i++) {
        f = localrandom(mx--);
        if (mx != f) {
            v = values[mx];
            values[mx] = values[f];
            values[f] = v;
        }
    }
}

Exporting Atanor* Atanorsvector::Unique() {
    Locking _lock(this);
    Atanorsvector* kvect = globalAtanor->Providesvector();
    map<string, bool> inter;
    for (long i = 0; i < values.size(); i++) {
        if (inter.find(values[i]) == inter.end()) {
            inter[values[i]] = true;
            kvect->values.push_back(values[i]);
        }
    }
    return kvect;
}

Exporting bool Atanorsvector::Permute() {
    Locking _lock(this);
    return next_permutation(values.begin(), values.end());
}

Exporting Atanor* Atanorsvector::same(Atanor* a) {
    Doublelocking _lock(this, a);

    if (a->Type() != idtype) {
        if (a->isVectorContainer()) {
            if (a->Size() != values.size())
                return aFALSE;
            AtanorIteration* itr = a->Newiteration(false);
            for (int i = 0; i < values.size(); i++) {
                if (values[i] != itr->Valuestring()) {
                    itr->Release();
                    return aFALSE;
                }
            }
            itr->Release();
            return aTRUE;
        }
        return aFALSE;
    }

    Atanorsvector* m = (Atanorsvector*)a;
    if (m->values.size() != values.size())
        return aFALSE;

    for (int i = 0; i < values.size(); i++) {
        if (m->values[i] != values[i])
            return aFALSE;
    }
    return aTRUE;
}

Exporting Atanor* Atanorsvector::Map(short idthread) {
    Locking _lock(this);
    Atanormap* kmap = globalAtanor->Providemap();

    char buff[100];
    for (int it = 0; it < values.size(); it++) {
        sprintf_s(buff, 100, "%d", it);
        kmap->Push(buff, globalAtanor->Providestring(values[it]));
    }
    return kmap;
}

Exporting Atanor* Atanorsvector::Vector(short idthread) {
    Locking _lock(this);
    Atanorvector* kvect = globalAtanor->Providevector();
    kvect->values.reserve(values.size());
    for (int i = 0; i < values.size(); i++)
        kvect->Push(globalAtanor->Providestring(values[i]));
    return kvect;
}


Exporting Atanor* Atanorsvector::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {
    Locking _lock(this);

    if (!idx->isIndex()) {
        Atanor* kv = NULL;
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer()) {
            kv = Selectasvector(contextualpattern);
            if (kv->Type() == a_svector)
                ((Atanorsvector*)kv)->values = values;
            else
                for (int i = 0; i < values.size(); i++)
                    kv->Push(globalAtanor->Providestring(values[i]));
            return kv;
        }

        if (contextualpattern->Type() == a_int || contextualpattern->Type() == a_float)
            return globalAtanor->Provideint(values.size());
        return this;
    }

    Atanor* key;
    Atanor* keyright = NULL;

    AtanorIndex* kind = (AtanorIndex*)idx;
    key = kind->left->Get(aNULL, aNULL, idthread);
    if (kind->interval == true)
        keyright = kind->right->Get(aNULL, aNULL, idthread);

    register long ikey;
    bool stringkey = false;
    if (key->Type() == a_string) {
        stringkey = true;
        string sf = key->String();
        bool found = false;
        if (kind->signleft) {
            for (ikey = values.size() - 1; ikey >= 0; ikey--) {
                if (sf == values[ikey]) {
                    found = true;
                    break;
                }
            }
        }
        else {
            for (ikey = 0; ikey < values.size(); ikey++) {
                if (sf == values[ikey]) {
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            if (globalAtanor->erroronkey)
                return globalAtanor->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    else
        ikey = key->Integer();

    key->Release();
    
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
        return globalAtanor->Providestring(values[ikey]);

    Atanorsvector* kvect;
    long iright;
    if (keyright->Type() == a_string) {
        string sf = keyright->String();
        bool found = false;
        if (kind->signright) {
            for (iright = values.size() - 1; iright >= 0; iright--) {
                if (sf == values[iright]) {
                    found = true;
                    iright++;
                    break;
                }
            }
        }
        else {
            for (iright = 0; iright < values.size(); iright++) {
                if (sf == values[iright]) {
                    found = true;
                    iright++;
                    break;
                }
            }
        }
        if (!found) {
            if (globalAtanor->erroronkey)
                return globalAtanor->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    else {
        if (keyright == aNULL)
            iright = 0;
        else {
            iright = keyright->Integer();
            if (stringkey && iright >= 0)
                iright = ikey + iright + 1;
        }
    }

    keyright->Release();
    if (iright < 0 || keyright == aNULL) {
        iright = values.size() + iright;
        if (iright<ikey) {
            if (globalAtanor->erroronkey)
                return globalAtanor->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    else  {
        if (iright>values.size()) {
            if (globalAtanor->erroronkey)
                return globalAtanor->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }

    //In this case, we must create a new vector
    kvect = globalAtanor->Providesvector();
    for (int i = ikey; i < iright; i++)
        kvect->values.push_back(values[i]);
    return kvect;
}

Exporting Atanor*  Atanorsvector::Put(Atanor* idx, Atanor* ke, short idthread) {
    if (ke == this)
        return aTRUE;

    Doublelocking _lock(this, ke);

    if (!idx->isIndex()) {
        if (ke == aNULL) {
            values.clear();
            return aTRUE;
        }

        if (ke->Type() == a_svector) {
            values = ((Atanorsvector*)ke)->values;
            return aTRUE;
        }
        if (ke->isMapContainer()) {
            values.clear();
            values.reserve(ke->Size());
            AtanorIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values.push_back(itr->Keystring());
            itr->Release();
            return aTRUE;
        }
        if (ke->isContainer()) {
            values.clear();
            values.reserve(ke->Size());
            AtanorIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values.push_back(itr->Valuestring());
            itr->Release();
            return aTRUE;
        }
        if (ke->isString()) {
            values.clear();
            string sv = ke->String();
            if (!v_comma_split_string(sv, values))
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
            values.push_back(ke->getstring(it));
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
                    values.insert(values.begin() + lkey, ke->getstring(i));
                }
                return aTRUE;
            }

            if (ke->isContainer()) {
                AtanorIteration* itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    values.insert(values.begin() + lkey, itr->Valuestring());
                itr->Release();
                return aTRUE;
            }

            values.insert(values.begin() + lkey, ke->String());
        }
        else {
            register long ikey = idx->Integer();
            int sz = values.size();
            int nb;
            string n = ke->String();
            if (ikey < 0) {
                ikey = sz + ikey;
                if (ikey < 0)
                    return globalAtanor->Returnerror("Cannot set this value", idthread);
            }

            if (ikey >= sz) {
                for (nb = values.size(); nb < ikey; nb++)
                    values.push_back("");
                values.push_back(n);
            }
            else
                values[ikey] = n;
        }
    }
    return aTRUE;
}

Exporting Atanor* Atanorsvector::Merging(Atanor* ke) {
    if (!ke->isContainer()) {
        values.push_back(ke->String());
        return this;
    }

    if (ke->Size() == 0)
        return this;

    Doublelocking _lock(this, ke);
    //Three cases:
    if (ke->Type() == idtype) {
        Atanorsvector* kvect = (Atanorsvector*)ke;
        values.insert(values.end(), kvect->values.begin(), kvect->values.end());
        return this;
    }

    AtanorIteration* itr = ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next())
        values.push_back(itr->Valuestring());
    itr->Release();
    return this;
}


Exporting Atanor* Atanorsvector::andset(Atanor *b, bool itself) {
    if (!b->isContainer())
        return aNULL;

    Atanorsvector* ref;

    long size = Size();
    size_t it;

    ref = globalAtanor->Providesvector();
    Locking _lock(this);
    AtanorIteration* itr = b->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        for (it = 0; it < size; it++) {
            if (values[it] == itr->Valuestring()) {
                ref->values.push_back(values[it]);
                break;
            }
        }
    }
    itr->Release();
    return ref;

}

Exporting Atanor* Atanorsvector::orset(Atanor *b, bool itself) {
    Atanorsvector* ref;
    if (itself)
        ref = this;
    else
        ref = (Atanorsvector*)Atom(true);

    ref->Merging(b);
    return ref;
}

Exporting Atanor* Atanorsvector::xorset(Atanor *b, bool itself) {
    if (!b->isVectorContainer())
        return aNULL;


    long size = Size();
    size_t it;

    Atanorsvector* ref = globalAtanor->Providesvector();
    Locking _lock(this);
    string s;
    hmap<string, bool> store;
    for (it = 0; it < b->Size(); it++)
        store[b->getstring(it)] = true;

    for (it = 0; it < size; it++) {
        if (store.find(values[it]) == store.end())
            store[values[it]] = true;
    }

    for (auto& i : store)
        ref->values.push_back(i.first);

    return ref;
}

Exporting Atanor* Atanorsvector::plus(Atanor* b, bool itself) {
    Atanorsvector* ref;
    if (itself)
        ref = this;
    else
        ref = (Atanorsvector*)Atom(true);

    int it;
    Doublelocking _lock(ref, b);
    long size = values.size();

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] += itr->Valuestring();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    string v = b->String();
    for (it = 0; it < size; it++) {
        ref->values[it] += v;
    }

    return ref;
}

Exporting Atanor* Atanorsvector::minus(Atanor *b, bool itself) {
    Atanorsvector* ref;
    if (itself)
        ref = this;
    else
        ref = (Atanorsvector*)Atom(true);

    int it;
    Locking _lock(ref);
    long size = values.size();

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] = StringMinus(ref->values[it], itr->Valuestring());
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    string v = b->String();
    for (it = 0; it < size; it++) {
        ref->values[it] = StringMinus(ref->values[it], v);
    }

    return ref;
}


static bool comp(string s1, string s2) {
    if (s1<s2)
        return true;
    return false;
}

static bool icomp(string s1, string s2) {
    if (s1>s2)
        return true;
    return false;
}

Exporting void Atanorsvector::Sort(bool direction) {
    Locking _lock(this);
    if (direction == false)
        sort(values.begin(), values.end(), comp);
    else
        sort(values.begin(), values.end(), icomp);
}


Exporting Atanor* Atanorsvector::Loophaskell(Atanor* recipient, Atanor* context, Atanor* environment, AtanorFunctionLambda* bd, short idthread) {
    Atanor* a;
    char addvalue = 0;

    if (context != aNOELEMENT)
        addvalue = Selecttype(context);

    Atanorstring* it = globalAtanor->Providestring("");
    it->Setreference();

    recipient->Putvalue( it, idthread);

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

Exporting Atanor* Atanorsvector::Filter(short idthread, Atanor* env, AtanorFunctionLambda* bd, Atanor* var, Atanor* kcont, Atanor* accu, Atanor* init, bool direct) {

    Atanor* returnval;

    bool first = false;
    Atanorstring* key = globalAtanor->Providestring("");
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

class SComp {
    public:
    AtanorCallFunction compare;
    short idthread;
    AtanorConstString p;
    AtanorConstString s;

    SComp(AtanorFunction* comp, short idt) : compare(comp), p(""), s(""), idthread(idt) {
        compare.arguments.push_back(&p);
        compare.arguments.push_back(&s);
    }

    bool get() {
        return compare.Get(aNULL, aNULL, idthread)->Boolean();
    }
};

class SComparison {
    public:
    SComp* compare;

    SComparison(SComp* c)  {
        compare = c;
    }

    bool operator() (const string& i, const string& j) {
        compare->p.value = i;
        compare->s.value = j;
        return compare->get();
    }
};

Atanor* Atanorsvector::MethodSort(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
    Atanor* comp = callfunc->Evaluate(0, contextualpattern, idthread);
    if (comp->isFunction()) {
        if (comp->Size() != 2)
            return globalAtanor->Returnerror("Expecting a comparison function with two parameters", idthread);
        SComp kcomp((AtanorFunction*)comp->Body(idthread), idthread);
        SComparison kfcomp(&kcomp);
        sort(values.begin(), values.end(), kfcomp);
        return this;
    }
    Sort(comp->Boolean());
    return this;
}


Exporting Atanor* Atanorsvector::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
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
