
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
filename   : atanorprimemapi.cxx
Date       : 2017/04/01
Purpose    : map implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
//#prime
#include "atanorprimemapi.h"
#include "atanorlist.h"
#include "atanorivector.h"


//We need to declare once again our local definitions.
Exporting hmap<unsigned short, primemapiMethod>  Atanorprimemapi::methods;
Exporting hmap<string, string> Atanorprimemapi::infomethods;
Exporting bin_hash<unsigned long> Atanorprimemapi::exported;

Exporting short Atanorprimemapi::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorprimemapi::AddMethod(AtanorGlobal* global, string name, primemapiMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanorprimemapi::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();


    Atanorprimemapi::idtype = global->Getid("primemapi");

    Atanorprimemapi::AddMethod(global, "clear", &Atanorprimemapi::MethodClear, P_NONE, "clear(): clear the container.");
    
    Atanorprimemapi::AddMethod(global, "invert", &Atanorprimemapi::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Atanorprimemapi::AddMethod(global, "find", &Atanorprimemapi::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Atanorprimemapi::AddMethod(global, "items", &Atanorprimemapi::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Atanorprimemapi::AddMethod(global, "join", &Atanorprimemapi::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Atanorprimemapi::AddMethod(global, "test", &Atanorprimemapi::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Atanorprimemapi::AddMethod(global, "keys", &Atanorprimemapi::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Atanorprimemapi::AddMethod(global, "values", &Atanorprimemapi::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Atanorprimemapi::AddMethod(global, "sum", &Atanorprimemapi::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Atanorprimemapi::AddMethod(global, "product", &Atanorprimemapi::MethodProduct, P_NONE, "product(): return the product of elements.");
    Atanorprimemapi::AddMethod(global, "pop", &Atanorprimemapi::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Atanorprimemapi::AddMethod(global, "merge", &Atanorprimemapi::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Atanorprimemapi::idtype] = new Atanorprimemapi(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("iprimemap")] = new Atanorprimemapi(global);

    global->RecordMethods(global->Getid("iprimemap"), Atanorprimemapi::exported);
    #endif
    global->RecordMethods(Atanorprimemapi::idtype, Atanorprimemapi::exported);

    return true;
}


Exporting AtanorIteration* Atanorprimemapi::Newiteration(bool direction) {
    return new AtanorIterationprimemapi(this, direction);
}


Exporting Atanor* Atanorprimemapi::in(Atanor* context, Atanor* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    long val = a->Integer();

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

Exporting Atanor* Atanorprimemapi::MethodFind(Atanor* context, short idthread, AtanorCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    prime_hash<long, Atanor*>::iterator it;
    
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


Exporting void Atanorprimemapi::Setreference(short inc) {
    if (idtracker == CIRCULARDETECTION) {
        globalAtanor->Returnerror("Error: Circularity in container", globalAtanor->GetThreadid());
        return;
    }

    int id = idtracker;
    idtracker = CIRCULARDETECTION;
    reference += inc;
    Setprotect(0);
    prime_hash<long, Atanor*>::iterator itx;
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

static void resetMap(Atanorprimemapi* kmap, long inc) {
    Locking _lock(kmap);
    kmap->reference -= inc;

    prime_hash<long, Atanor*>& values = kmap->values;
    if (values.size() == 0)
        return;

    prime_hash<long, Atanor*>::iterator itx;
    for (itx = values.begin(); itx != values.end(); itx++)
        itx->second->Resetreference(inc);
}

Exporting void Atanorprimemapi::Resetreference(short inc) {
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


Exporting Atanor* Atanorprimemapi::Push(Atanor* k, Atanor* v) {
    Locking _lock(this);
    long s = k->Integer();
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

Exporting Atanor* Atanorprimemapi::Pop(Atanor* kkey) {
    long k = kkey->Integer();
    if (values.find(k) != values.end()) {
        kkey = values[k];
        values.erase(k);
        kkey->Resetreference(reference + 1);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Atanorprimemapi::Clear() {
    Locking _lock(this);
    prime_hash<long, Atanor*>::iterator itx;
    for (itx = values.begin(); itx != values.end(); itx++)
        itx->second->Resetreference(reference + 1);
    values.clear();
}

Exporting void Atanorprimemapi::Clean() {
    //Your cleaning code
}

Exporting string Atanorprimemapi::String() {
    Locking _lock(this);
    stringstream res;
    prime_hash<long, Atanor*>::iterator it;
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

Exporting string Atanorprimemapi::JSonString() {
    Locking _lock(this);
    stringstream res;
    prime_hash<long, Atanor*>::iterator it;
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


Exporting long Atanorprimemapi::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanorprimemapi::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanorprimemapi::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanorprimemapi::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Atanorprimemapi::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Atanor*  Atanorprimemapi::Put(Atanor* idx, Atanor* ke, short idthread) {
    if (ke == this)
        return aTRUE;

    Doublelocking _lock(this, ke);

    int nb;
    if (!idx->isIndex()) {
        if (ke == aNULL) {
            Clear();
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Clear();
            AtanorIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                Push(itr->Keyinteger(), itr->Value());
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
            if (ke->Type() == Atanorprimemapi::idtype) {
                Atanorprimemapi* kmap = (Atanorprimemapi*)ke;
                //We copy all values from ke to this
                prime_hash<long, Atanor*>::iterator it;
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
                    values[itr->Keyinteger()] = a;
                    a->Setreference(reference + 1);
                }
                itr->Release();
            }
            ke->Release();
        }
        return aTRUE;
    }

    Push(idx->Integer(), ke);
    return aTRUE;
}

Exporting Atanor* Atanorprimemapi::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Atanor* vect = contextualpattern->Newinstance(idthread);
            prime_hash<long, Atanor*>::iterator it;
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
        Atanorprimemapi* kmap = new Atanorprimemapi;
        key = ((AtanorIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Atanor* keyright = ((AtanorIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        long vleft = key->Integer();
        long vright = keyright->Integer();
        prime_hash<long, Atanor*>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        prime_hash<long, Atanor*>::iterator itr = values.end();
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

    long skey = key->Integer();
    key->Release();

    Atanor* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalAtanor->erroronkey)
            return globalAtanor->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }
    return kval;
}

Exporting Atanor* Atanorprimemapi::same(Atanor* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Atanorprimemapi* m = (Atanorprimemapi*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    prime_hash<long, Atanor*>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second->same(values[it->first]) == aFALSE)
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Atanor* Atanorprimemapi::xorset(Atanor* b, bool itself) {

    Doublelocking _lock(this, b);


    Atanorprimemapi* res;
    
    
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapi;
        hmap<long, Atanor*> keys;

        prime_hash<long, Atanor*>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;
            
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyinteger();
            if (keys.find(v) == keys.end())
                keys[v]=itr->IteratorValue();
            else {
                if (values[v]->same(itr->IteratorValue()) == aTRUE)
                    keys.erase(v);
            }
        }
        itr->Release();

        for (auto& a : keys)
            res->Push(a.first,a.second);

        return res;


    }


    if (itself)
        res = this;
    else
        res = (Atanorprimemapi*)Atom(true);

    prime_hash<long, Atanor*>::iterator it;
    for (it = values.begin(); it != values.end(); it++)
        it->second->xorset(b, true);
    return res;
}

Exporting Atanor* Atanorprimemapi::orset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long, Atanor*>::iterator it;
    Atanorprimemapi* res;
    if (itself)
        res = this;
    else
        res = (Atanorprimemapi*)Atom(true);

    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }

    for (it = values.begin(); it != values.end(); it++)
        it->second->orset(b, true);
    return res;

}

Exporting Atanor* Atanorprimemapi::andset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long, Atanor*>::iterator it;
    Atanorprimemapi* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapi;
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            if (values.find(itr->Keyinteger()) != values.end() && values[itr->Keyinteger()]->same(v) == aTRUE)
                res->Push(itr->Keyinteger(), v);
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorprimemapi*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->andset(b, true);
    return res;
}

Exporting Atanor* Atanorprimemapi::plus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long, Atanor*>::iterator it;
    Atanorprimemapi* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapi;
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyinteger());
            if (it != values.end()) {
                res->Push(it->first, it->second);
                res->values[it->first]->plus(v, true);
            }

        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorprimemapi*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->plus(b, true);
    return res;
}

Exporting Atanor* Atanorprimemapi::minus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long, Atanor*>::iterator it;
    Atanorprimemapi* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapi;
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyinteger());
            if (it != values.end()) {
                res->Push(it->first, it->second);
                res->values[it->first]->minus(v, true);
            }

        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorprimemapi*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->minus(b, true);
    return res;
}

Exporting Atanor* Atanorprimemapi::multiply(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long, Atanor*>::iterator it;
    Atanorprimemapi* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapi;
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyinteger());
            if (it != values.end()) {
                res->Push(it->first, it->second);
                res->values[it->first]->multiply(v, true);
            }

        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorprimemapi*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->multiply(b, true);
    return res;
}

Exporting Atanor* Atanorprimemapi::divide(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long, Atanor*>::iterator it;
    Atanorprimemapi* res;
    Atanor* v;
    Atanor* r;

    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapi;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyinteger());
            if (it != values.end()) {
                r = res->values[it->first]->divide(v, true);
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
        res = (Atanorprimemapi*)Atom(true);

    for (it = values.begin(); it != values.end(); it++) {
        r = it->second->divide(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Atanor* Atanorprimemapi::mod(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long, Atanor*>::iterator it;
    Atanorprimemapi* res;
    Atanor* v;
    Atanor* r;

    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapi;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyinteger());
            if (it != values.end()) {
                r = res->values[it->first]->mod(v, true);
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
        res = (Atanorprimemapi*)Atom(true);

    for (it = values.begin(); it != values.end(); it++) {
        r = it->second->mod(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Atanor* Atanorprimemapi::shiftright(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long, Atanor*>::iterator it;
    Atanorprimemapi* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapi;
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyinteger());
            if (it != values.end()) {
                res->Push(it->first, it->second);
                res->values[it->first]->shiftright(v, true);
            }

        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorprimemapi*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->shiftright(b, true);
    return res;
}

Exporting Atanor* Atanorprimemapi::shiftleft(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long, Atanor*>::iterator it;
    Atanorprimemapi* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapi;
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyinteger());
            if (it != values.end()) {
                res->Push(it->first, it->second);
                res->values[it->first]->shiftleft(v, true);
            }

        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorprimemapi*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->shiftleft(b, true);
    return res;
}

Exporting Atanor* Atanorprimemapi::power(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long, Atanor*>::iterator it;
    Atanorprimemapi* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapi;
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyinteger());
            if (it != values.end()) {
                res->Push(it->first, it->second);
                res->values[it->first]->power(v, true);
            }

        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorprimemapi*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->power(b, true);
    return res;
}




Exporting Atanor* Atanorprimemapi::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
    Locking _lock(this);
    Atanor* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    prime_hash<long, Atanor*>::iterator it;
    
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
