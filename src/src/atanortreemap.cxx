
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
filename   : atanortreemap.cxx
Date       : 2017/04/01
Purpose    : map implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanortreemap.h"
#include "atanorlist.h"
#include "atanorivector.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, treemapMethod>  Atanortreemap::methods;
Exporting hmap<string, string> Atanortreemap::infomethods;
Exporting bin_hash<unsigned long> Atanortreemap::exported;

Exporting short Atanortreemap::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanortreemap::AddMethod(AtanorGlobal* global, string name, treemapMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanortreemap::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();



    Atanortreemap::idtype = global->Getid("treemap");

    Atanortreemap::AddMethod(global, "items", &Atanortreemap::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");
    
    Atanortreemap::AddMethod(global, "invert", &Atanortreemap::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Atanortreemap::AddMethod(global, "find", &Atanortreemap::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");



    Atanortreemap::AddMethod(global, "join", &Atanortreemap::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Atanortreemap::AddMethod(global, "test", &Atanortreemap::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Atanortreemap::AddMethod(global, "keys", &Atanortreemap::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Atanortreemap::AddMethod(global, "values", &Atanortreemap::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Atanortreemap::AddMethod(global, "sum", &Atanortreemap::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Atanortreemap::AddMethod(global, "product", &Atanortreemap::MethodProduct, P_NONE, "product(): return the product of elements.");
    Atanortreemap::AddMethod(global, "pop", &Atanortreemap::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Atanortreemap::AddMethod(global, "merge", &Atanortreemap::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    
    global->newInstance[Atanortreemap::idtype] = new Atanortreemap(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("streemap")] = new Atanortreemap(global);
    global->newInstance[global->Getid("treemaps")] = new Atanortreemap(global);

    global->RecordMethods(global->Getid("treemaps"),Atanortreemap::exported);
    global->RecordMethods(global->Getid("streemap"),Atanortreemap::exported);
    #endif
    global->RecordMethods(Atanortreemap::idtype,Atanortreemap::exported);

    return true;
}

Exporting AtanorIteration* Atanortreemap::Newiteration(bool direction) {
    return new AtanorIterationtreemap(this, direction);
}

Exporting Atanor* Atanortreemap::MethodFind(Atanor* context, short idthread, AtanorCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    Atanor* a = callfunc->Evaluate(0, context, idthread);
    if (context->isBoolean()) {
        Locking _lock(this);
        for (auto& it : values) {
            if (it.second->same(a) == aTRUE)
                return aTRUE;
        }
        return aFALSE;
    }
    if (context->isVectorContainer()) {
        Atanorsvector* v = (Atanorsvector*)Selectasvector(context);
        Doublelocking _lock(this, v);
        for (auto& it : values) {
            if (it.second->same(a) == aTRUE)
                v->values.push_back(it.first);
        }
        return v;
    }
    Locking _lock(this);
    for (auto& it : values) {
        if (it.second->same(a) == aTRUE)
            return globalAtanor->Providestring(it.first);
    }
    return aNULL;
}

Exporting Atanor* Atanortreemap::in(Atanor* context, Atanor* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    string val = a->String();

    if (context->isBoolean()) {
        Locking _lock(this);
        if (values.find(val) != values.end())
            return aTRUE;
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Atanorsvector* v = (Atanorsvector*)Selectasvector(context);
        Doublelocking _lock(this, v);
        if (values.find(val) != values.end())
            v->values.push_back(val);

        return v;
    }

    Locking _lock(this);
    if (values.find(val) != values.end())
        return globalAtanor->Providestring(val);

    return aNULL;
}

Exporting void Atanortreemap::Setreference(short inc) {
    if (idtracker == CIRCULARDETECTION) {
        globalAtanor->Returnerror("Error: Circularity in container", globalAtanor->GetThreadid());
        return;
    }
    int id = idtracker;
    idtracker = CIRCULARDETECTION;
    reference += inc;
    Setprotect(0);

    vector<Atanor*> doubles;
    {
        Locking _lock(this);
        for (auto& itx : values) {
            if (itx.second != NULL)
                doubles.push_back(itx.second);
        }
    }
    for (int itx = 0; itx < doubles.size(); itx++)
        doubles[itx]->Setreference(inc);
    idtracker = id;
}
static void resetMap(Atanortreemap* kmap, long inc) {
    Locking _lock(kmap);
    kmap->reference -= inc;
    map<string, Atanor*>& values = kmap->values;
    if (values.size() == 0)
        return;

    for (auto& itx : values)
        itx.second->Resetreference(inc);
}

Exporting void Atanortreemap::Resetreference(short inc) {
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

Exporting Atanor* Atanortreemap::Push(Atanor* k, Atanor* v) {
    Locking _lock(this);
    string s = k->String();
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

Exporting Atanor* Atanortreemap::Pop(Atanor* kkey) {
    string k = kkey->String();
    if (values.find(k) != values.end()) {
        kkey = values[k];
        values.erase(k);
        kkey->Resetreference(reference + 1);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Atanortreemap::Clear() {
    Locking _lock(this);

    for (auto& itx : values)
        itx.second->Resetreference(reference + 1);
    values.clear();
}

Exporting void Atanortreemap::Clean() {
    //Your cleaning code
}

Exporting string Atanortreemap::String() {
    Locking _lock(this);
    stringstream res;

    res << "{";
    bool beg = true;
    string sx;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        sx = it.first;
        stringing(res, sx);
        res << ":";
        sx = it.second->StringToDisplay();
        if (!it.second->isString() || it.second->isContainer())
            res << sx;
        else
            stringing(res, sx);
    }
    res << "}";
    return res.str();
}

Exporting string Atanortreemap::JSonString() {
    Locking _lock(this);
    stringstream res;

    res << "{";
    bool beg = true;
    string sx;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        sx = it.first;
        jstringing(res, sx);
        res << ":" << it.second->JSonString();
    }
    res << "}";
    return res.str();
}

//Basic operations
Exporting long Atanortreemap::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting long Atanortreemap::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanortreemap::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanortreemap::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanortreemap::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}



Exporting Atanor*  Atanortreemap::Put(Atanor* idx, Atanor* ke, short idthread) {
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
                Push(itr->Keystring(), itr->Value());
            itr->Release();
            return aTRUE;
        }
        char ch[20];
        if (ke->isVectorContainer()) {
            Clear();
            nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                sprintf_s(ch, 20, "%d", nb);
                Push(ch, ke->getvalue(it));
                nb++;
            }
            return aTRUE;
        }
        if (ke->Type() == a_list) {
            Atanorlist* kvect = (Atanorlist*)ke;
            Clear();
            nb = 0;

            for (auto& it : kvect->values) {
                sprintf_s(ch, 20, "%d", nb);
                Push(ch, it);
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalAtanor->Returnerror("Wrong map initialization", idthread);
        if (ke != this) {
            Clear();
            if (ke->Type() == Atanortreemap::idtype) {
                Atanortreemap* kmap = (Atanortreemap*)ke;
                //We copy all values from ke to this

                for (auto& it : kmap->values)
                    Push(it.first, it.second);
                kmap->Release();
            }
            else {
                AtanorIteration* itr = ke->Newiteration(false);
                Atanor* a;
                for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
                    a=itr->IteratorValue();
                    a=a->Atom();
                    values[itr->Keystring()] = a;
                    a->Setreference(reference + 1);
                }
                itr->Release();
            }
            ke->Release();
        }
        return aTRUE;
    }
    Push(STR(idx->String()), ke);
    return aTRUE;
}

Exporting Atanor* Atanortreemap::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Atanor* vect = contextualpattern->Newinstance(idthread);
            map<string, Atanor*>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalAtanor->Providestring(it->first));
            return vect;
        }

        if (contextualpattern->Type() == a_int || contextualpattern->Type() == a_float)
            return globalAtanor->Provideint(values.size());

        return this;
    }

    Atanor* key;
    if (idx->isInterval()) {
        Atanortreemap* kmap = new Atanortreemap;
        key = ((AtanorIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Atanor* keyright = ((AtanorIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        string vleft = key->String();
        string vright = keyright->String();
        map<string, Atanor*>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        map<string, Atanor*>::iterator itr= values.end();
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

    string skey = key->String();
    key->Release();

    Atanor* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalAtanor->erroronkey)
            return globalAtanor->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }
    return kval;
}

Exporting Atanor* Atanortreemap::same(Atanor* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Atanortreemap* m = (Atanortreemap*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    map<string, Atanor*>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second->same(values[it->first]) == aFALSE)
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Atanor* Atanortreemap::xorset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);
    Atanortreemap* res;
    
    
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemap;
        hmap<string, Atanor*> keys;

        for (auto& it : values)
            keys[it.first] = it.second;
            
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keystring();
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
        res = (Atanortreemap*)Atom(true);

    for (auto& it : values)
        it.second->xorset(b, true);
    return res;
}

Exporting Atanor* Atanortreemap::orset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemap* res;
    if (itself)
        res = this;
    else
        res = (Atanortreemap*)Atom(true);
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    for (auto& it : res->values)
        it.second->orset(b, true);
    return res;
}

Exporting Atanor* Atanortreemap::andset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemap* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        res = new Atanortreemap;
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            if (values.find(itr->Keystring()) != values.end() && values[itr->Keystring()]->same(v) == aTRUE)
                res->Push(itr->Keystring(), v);
        }
        itr->Release();
        return res;
    }
    if (itself)
        res = this;
    else
        res = (Atanortreemap*)Atom(true);
    for (auto& it : res->values)
        it.second->andset(b, true);
    return res;
}

Exporting Atanor* Atanortreemap::plus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemap* res;
    if (b->isMapContainer()) {
        map<string, Atanor*>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemap;
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keystring());
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
        res = (Atanortreemap*)Atom(true);

    for (auto& it : res->values)
        it.second->plus(b, true);
    return res;
}

Exporting Atanor* Atanortreemap::minus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemap* res;
    if (b->isMapContainer()) {
        map<string, Atanor*>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemap;
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keystring());
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
        res = (Atanortreemap*)Atom(true);

    for (auto& it : res->values)
        it.second->minus(b, true);
    return res;
}

Exporting Atanor* Atanortreemap::multiply(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemap* res;
    if (b->isMapContainer()) {
        map<string, Atanor*>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemap;
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keystring());
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
        res = (Atanortreemap*)Atom(true);

    for (auto& it : res->values)
        it.second->multiply(b, true);
    return res;
}

Exporting Atanor* Atanortreemap::divide(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemap* res;
    Atanor* v;
    Atanor* r;

    if (b->isMapContainer()) {
        map<string, Atanor*>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemap;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keystring());
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
        res = (Atanortreemap*)Atom(true);

    for (auto& it : res->values) {
        r = it.second->divide(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Atanor* Atanortreemap::mod(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemap* res;
    Atanor* v;
    Atanor* r;

    if (b->isMapContainer()) {
        map<string, Atanor*>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemap;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keystring());
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
        res = (Atanortreemap*)Atom(true);

    for (auto& it : res->values) {
        r = it.second->mod(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Atanor* Atanortreemap::shiftright(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemap* res;
    if (b->isMapContainer()) {
        map<string, Atanor*>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemap;
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keystring());
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
        res = (Atanortreemap*)Atom(true);

    for (auto& it : res->values)
        it.second->shiftright(b, true);
    return res;
}

Exporting Atanor* Atanortreemap::shiftleft(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemap* res;
    if (b->isMapContainer()) {
        map<string, Atanor*>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemap;
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keystring());
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
        res = (Atanortreemap*)Atom(true);

    for (auto& it : res->values)
        it.second->shiftleft(b, true);
    return res;
}

Exporting Atanor* Atanortreemap::power(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemap* res;
    if (b->isMapContainer()) {
        map<string, Atanor*>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemap;
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keystring());
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
        res = (Atanortreemap*)Atom(true);

    for (auto& it : res->values)
        it.second->power(b, true);
    return res;
}


Exporting Atanor* Atanortreemap::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
    Locking _lock(this);
    Atanor* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    map<string, Atanor*>::iterator it;
    
    Atanor* a;
    vector<string> keys;

    for (it=values.begin(); it != values.end(); it++)
        keys.push_back(it->first);

    for (long i = 0; i < keys.size(); i++) {

        var->storevalue(keys[i]);

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
