
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
filename   : atanorbinmap.cxx
Date       : 2017/04/01
Purpose    : map implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
//#prime
#include "atanorbinmap.h"
#include "atanorlist.h"
#include "atanorivector.h"


//We need to declare once again our local definitions.
Exporting hmap<unsigned short, binmapMethod>  Atanorbinmap::methods;
Exporting hmap<string, string> Atanorbinmap::infomethods;
Exporting bin_hash<unsigned long> Atanorbinmap::exported;

Exporting short Atanorbinmap::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorbinmap::AddMethod(AtanorGlobal* global, string name, binmapMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanorbinmap::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();


    Atanorbinmap::idtype = global->Getid("binmap");

    Atanorbinmap::AddMethod(global, "clear", &Atanorbinmap::MethodClear, P_NONE, "clear(): clear the container.");
    
    Atanorbinmap::AddMethod(global, "invert", &Atanorbinmap::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Atanorbinmap::AddMethod(global, "find", &Atanorbinmap::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Atanorbinmap::AddMethod(global, "items", &Atanorbinmap::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Atanorbinmap::AddMethod(global, "join", &Atanorbinmap::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Atanorbinmap::AddMethod(global, "test", &Atanorbinmap::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Atanorbinmap::AddMethod(global, "keys", &Atanorbinmap::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Atanorbinmap::AddMethod(global, "values", &Atanorbinmap::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Atanorbinmap::AddMethod(global, "sum", &Atanorbinmap::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Atanorbinmap::AddMethod(global, "product", &Atanorbinmap::MethodProduct, P_NONE, "product(): return the product of elements.");
    Atanorbinmap::AddMethod(global, "pop", &Atanorbinmap::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Atanorbinmap::AddMethod(global, "merge", &Atanorbinmap::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Atanorbinmap::idtype] = new Atanorbinmap(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("iprimemap")] = new Atanorbinmap(global);

    global->RecordMethods(global->Getid("iprimemap"), Atanorbinmap::exported);
    #endif
    global->RecordMethods(Atanorbinmap::idtype, Atanorbinmap::exported);

    return true;
}


Exporting AtanorIteration* Atanorbinmap::Newiteration(bool direction) {
    return new AtanorIterationbinmap(this, direction);
}


Exporting Atanor* Atanorbinmap::in(Atanor* context, Atanor* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    ushort val = a->Short();

    if (context->isBoolean()) {
        Locking _lock(this);
        if (values.find(val)!=values.end())
            return aTRUE;
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Atanorivector* v = (Atanorivector*)Selectaivector(context);
        Doublelocking _lock(this, v);
        if (values.find(val) != values.end())
            v->values.push_back(val);

        return v;
    }
    
    Locking _lock(this);
    if (values.find(val)!=values.end())
        return globalAtanor->Provideint(val);

    return aNULL;

}

Exporting Atanor* Atanorbinmap::MethodFind(Atanor* context, short idthread, AtanorCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    basebin_hash<Atanor*>::iterator it;
    
    Atanor* a = callfunc->Evaluate(0,context,idthread);

    if (context->isBoolean()) {
        Locking _lock(this);
        for (it = values.begin(); it != values.end(); it++) {
            if (it->second->same(a) == aTRUE)
                return aTRUE;
        }
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Atanorivector* v = (Atanorivector*)Selectaivector(context);
        Doublelocking _lock(this, v);

        for (it = values.begin(); it != values.end(); it++) {
            if (it->second->same(a) == aTRUE)
                v->values.push_back(it->first);
        }
        return v;
    }

    Locking _lock(this);
    for (it = values.begin(); it != values.end(); it++) {
        if (it->second->same(a) == aTRUE)
            return globalAtanor->Provideint(it->first);
    }

    return aNULL;

}


Exporting void Atanorbinmap::Setreference(short inc) {
    if (idtracker == CIRCULARDETECTION) {
        globalAtanor->Returnerror("Error: Circularity in container", globalAtanor->GetThreadid());
        return;
    }

    int id = idtracker;
    idtracker = CIRCULARDETECTION;
    reference += inc;
    Setprotect(0);
    basebin_hash<Atanor*>::iterator itx;
    vector<Atanor*> doubles;
    {
        Locking _lock(this);
        for (itx = values.begin(); itx != values.end(); itx++) {
            if (itx->second != NULL)
                doubles.push_back(itx->second);
        }
    }
    for (int itx = 0; itx < doubles.size(); itx++)
        doubles[itx]->Setreference(inc);
    idtracker = id;
}

static void resetMap(Atanorbinmap* kmap, long inc) {
    Locking _lock(kmap);
    kmap->reference -= inc;

    basebin_hash<Atanor*>& values = kmap->values;
    if (values.size() == 0)
        return;

    basebin_hash<Atanor*>::iterator itx;
    for (itx = values.begin(); itx != values.end(); itx++)
        itx->second->Resetreference(inc);
}

Exporting void Atanorbinmap::Resetreference(short inc) {
    if ((reference - inc) > 0)
        resetMap(this, inc);
    else {
        resetMap(this, inc + 1 - protect);
        if (!protect) {
            if (idtracker != -1)
                globalAtanor->RemoveFromTracker(idtracker);
            delete this;
        }
    }
}


Exporting Atanor* Atanorbinmap::Push(Atanor* k, Atanor* v) {
    Locking _lock(this);
    ushort s = k->Short();
    if (values.find(s) != values.end()) {
        Atanor* kval = values[s];
        values.erase(s);
        kval->Resetreference(reference + 1);
    }

    v = v->Atom();
    values[s] = v;
    v->Setreference(reference + 1);
    return aTRUE;
}

Exporting Atanor* Atanorbinmap::Pop(Atanor* kkey) {
    ushort k = kkey->Short();
    if (values.find(k) != values.end()) {
        kkey = values[k];
        values.erase(k);
        kkey->Resetreference(reference + 1);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Atanorbinmap::Clear() {
    Locking _lock(this);
    basebin_hash<Atanor*>::iterator itx;
    for (itx = values.begin(); itx != values.end(); itx++)
        itx->second->Resetreference(reference + 1);
    values.clear();
}

Exporting void Atanorbinmap::Clean() {
    //Your cleaning code
}

Exporting string Atanorbinmap::String() {
    Locking _lock(this);
    stringstream res;
    basebin_hash<Atanor*>::iterator it;
    res << "{";
    bool beg = true;
    string sx;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it->first << ":";
        sx = it->second->StringToDisplay();
        if (!it->second->isString())
            res << sx;
        else
            stringing(res, sx);
    }
    res << "}";
    return res.str();
}

Exporting string Atanorbinmap::JSonString() {
    Locking _lock(this);
    stringstream res;
    basebin_hash<Atanor*>::iterator it;
    res << "{";
    bool beg = true;

    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        res << '"' << it->first << '"' << ":" << it->second->JSonString();
    }
    res << "}";
    return res.str();
}


Exporting long Atanorbinmap::Integer() {
    Locking _lock(this);
    return values.size();
}

short Atanorbinmap::Short() {
    Locking _lock(this);
    return values.size();
}

float Atanorbinmap::Decimal() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanorbinmap::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanorbinmap::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanorbinmap::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Atanorbinmap::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Atanor*  Atanorbinmap::Put(Atanor* idx, Atanor* ke, short idthread) {
    if (ke == this)
        return aTRUE;

    Doublelocking _lock(this, ke);

    long nb;
    if (!idx->isIndex()) {
        if (ke == aNULL) {
            Clear();
            return aTRUE;
        }
        
        if (ke->isMapContainer()) {
            Clear();
            AtanorIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                Push((ushort)itr->Keyshort(), itr->Value());
            itr->Release();
            return aTRUE;
        }

        if (ke->isVectorContainer()) {
            Clear();
            nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                Push(nb, ke->getvalue(it));
                nb++;
            }
            return aTRUE;
        }

        if (ke->Type() == a_list) {
            Atanorlist* kvect = (Atanorlist*)ke;
            Clear();
            nb = 0;
            list<Atanor*>::iterator it;
            for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
                Push(nb, *it);
                nb++;
            }
            return aTRUE;
        }

        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalAtanor->Returnerror("Wrong map initialization", idthread);

        if (ke != this) {
            Clear();
            if (ke->Type() == Atanorbinmap::idtype) {
                Atanorbinmap* kmap = (Atanorbinmap*)ke;
                //We copy all values from ke to this
                basebin_hash<Atanor*>::iterator it;
                for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                    Push(it->first, it->second);
                kmap->Release();
            }
            else {
                AtanorIteration* itr = ke->Newiteration(false);
                Atanor* a;
                for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
                    a=itr->IteratorValue();
                    a=a->Atom();
                    values[itr->Keyshort()] = a;
                    a->Setreference(reference + 1);
                }
                itr->Release();
            }
            ke->Release();
        }
        return aTRUE;
    }

    Push(idx->Short(), ke);
    return aTRUE;
}

Exporting Atanor* Atanorbinmap::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Atanor* vect = contextualpattern->Newinstance(idthread);
            basebin_hash<Atanor*>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalAtanor->Provideint(it->first));
            return vect;
        }

        if (contextualpattern->Type() == a_int || contextualpattern->Type() == a_float)
            return globalAtanor->Provideint(values.size());

        return this;
    }

    Atanor* key;
    if (idx->isInterval()) {
        Atanorbinmap* kmap = new Atanorbinmap;
        key = ((AtanorIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Atanor* keyright = ((AtanorIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        ushort vleft = key->Short();
        ushort vright = keyright->Short();
        basebin_hash<Atanor*>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        basebin_hash<Atanor*>::iterator itr = values.end();
        if (keyright != aNULL) {
            itr = values.find(vright);
            if (itr == values.end()) {
                keyright->Release();
                return kmap;
            }
        }
        keyright->Release();
        for (; it != values.end(); it++) {
            kmap->Push(it->first, it->second);
            if (it == itr)
                return kmap;
        }

        if (itr != values.end())
            kmap->Clear();
        return kmap;

    }

    key = ((AtanorIndex*)idx)->left->Get(aNULL, aNULL, idthread);
    
    if (key == aNULL) {
        if (globalAtanor->erroronkey)
            return globalAtanor->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }

    ushort skey = key->Short();
    key->Release();

    Atanor* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalAtanor->erroronkey)
            return globalAtanor->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }
    return kval;
}

Exporting Atanor* Atanorbinmap::same(Atanor* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Atanorbinmap* m = (Atanorbinmap*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    basebin_hash<Atanor*>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second->same(values[it->first]) == aFALSE)
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Atanor* Atanorbinmap::xorset(Atanor* b, bool itself) {

    Doublelocking _lock(this, b);


    Atanorbinmap* res;
    
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmap;
        hmap<short, Atanor*> keys;

        basebin_hash<Atanor*>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;
            
        short v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyshort();
            if (keys.find(v) == keys.end())
                keys[v] = itr->IteratorValue();
            else {
                if (values[v]->same(itr->IteratorValue()) == aTRUE)
                    keys.erase(v);
            }
        }
        itr->Release();

        for (auto& a : keys)
            res->Push(a.first, a.second);

        return res;

    }


    if (itself)
        res = this;
    else
        res = (Atanorbinmap*)Atom(true);

    basebin_hash<Atanor*>::iterator it;
    for (it = values.begin(); it != values.end(); it++)
        it->second->xorset(b, true);
    return res;
}

Exporting Atanor* Atanorbinmap::orset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<Atanor*>::iterator it;
    Atanorbinmap* res;
    if (itself)
        res = this;
    else
        res = (Atanorbinmap*)Atom(true);

    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }

    for (it = values.begin(); it != values.end(); it++)
        it->second->orset(b, true);
    return res;

}

Exporting Atanor* Atanorbinmap::andset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<Atanor*>::iterator it;
    Atanorbinmap* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmap;
        Atanor* v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            ikey=itr->Keyshort();
            if (values.check(ikey) && values[ikey]->same(v) == aTRUE)
                res->Push(ikey, v);
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorbinmap*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->andset(b, true);
    return res;
}

Exporting Atanor* Atanorbinmap::plus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<Atanor*>::iterator it;
    Atanorbinmap* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmap;
        Atanor* v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->Push(ikey, values[ikey]);
                res->values[ikey]->plus(v, true);
            }

        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorbinmap*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->plus(b, true);
    return res;
}

Exporting Atanor* Atanorbinmap::minus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<Atanor*>::iterator it;
    Atanorbinmap* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmap;
        Atanor* v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->Push(ikey, values[ikey]);
                res->values[ikey]->minus(v, true);
            }

        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorbinmap*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->minus(b, true);
    return res;
}

Exporting Atanor* Atanorbinmap::multiply(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<Atanor*>::iterator it;
    Atanorbinmap* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmap;
        Atanor* v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->Push(ikey, values[ikey]);
                res->values[ikey]->multiply(v, true);
            }

        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorbinmap*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->multiply(b, true);
    return res;
}

Exporting Atanor* Atanorbinmap::divide(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<Atanor*>::iterator it;
    Atanorbinmap* res;
    Atanor* v;
    Atanor* r;

    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmap;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->Push(ikey, values[ikey]);
                r = res->values[ikey]->divide(v, true);
                if (r->isError()) {
                    res->Release();
                    itr->Release();
                    return r;
                }
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorbinmap*)Atom(true);

    for (it = values.begin(); it != values.end(); it++) {
        r = it->second->divide(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Atanor* Atanorbinmap::mod(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<Atanor*>::iterator it;
    Atanorbinmap* res;
    Atanor* v;
    Atanor* r;

    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmap;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->Push(ikey, values[ikey]);
                r = res->values[ikey]->mod(v, true);
                if (r->isError()) {
                    res->Release();
                    itr->Release();
                    return r;
                }
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorbinmap*)Atom(true);

    for (it = values.begin(); it != values.end(); it++) {
        r = it->second->mod(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Atanor* Atanorbinmap::shiftright(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<Atanor*>::iterator it;
    Atanorbinmap* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmap;
        Atanor* v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->Push(ikey, values[ikey]);
                res->values[ikey]->shiftright(v, true);
            }

        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorbinmap*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->shiftright(b, true);
    return res;
}

Exporting Atanor* Atanorbinmap::shiftleft(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<Atanor*>::iterator it;
    Atanorbinmap* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmap;
        Atanor* v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->Push(ikey, values[ikey]);
                res->values[ikey]->shiftleft(v, true);
            }

        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorbinmap*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->shiftleft(b, true);
    return res;
}

Exporting Atanor* Atanorbinmap::power(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<Atanor*>::iterator it;
    Atanorbinmap* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmap;
        Atanor* v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->Push(ikey, values[ikey]);
                res->values[ikey]->power(v, true);
            }

        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorbinmap*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->power(b, true);
    return res;
}

Exporting Atanor* Atanorbinmap::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
    Locking _lock(this);
    Atanor* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    basebin_hash<Atanor*>::iterator it;
    
    Atanor* a;
    for (it=values.begin(); it != values.end(); it++) {
        var->storevalue(it->first);

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
