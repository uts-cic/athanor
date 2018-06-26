
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
filename   : atanortreemapl.cxx
Date       : 2017/04/01
Purpose    : map implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
//#prime
#include "atanortreemapl.h"
#include "atanorlist.h"
#include "atanorlvector.h"


//We need to declare once again our local definitions.
Exporting hmap<unsigned short, treemaplMethod>  Atanortreemapl::methods;
Exporting hmap<string, string> Atanortreemapl::infomethods;
Exporting bin_hash<unsigned long> Atanortreemapl::exported;

Exporting short Atanortreemapl::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanortreemapl::AddMethod(AtanorGlobal* global, string name, treemaplMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanortreemapl::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();


    Atanortreemapl::idtype = global->Getid("treemapl");

    Atanortreemapl::AddMethod(global, "clear", &Atanortreemapl::MethodClear, P_NONE, "clear(): clear the container.");
    
    Atanortreemapl::AddMethod(global, "invert", &Atanortreemapl::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Atanortreemapl::AddMethod(global, "find", &Atanortreemapl::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Atanortreemapl::AddMethod(global, "items", &Atanortreemapl::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Atanortreemapl::AddMethod(global, "join", &Atanortreemapl::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Atanortreemapl::AddMethod(global, "test", &Atanortreemapl::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Atanortreemapl::AddMethod(global, "keys", &Atanortreemapl::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Atanortreemapl::AddMethod(global, "values", &Atanortreemapl::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Atanortreemapl::AddMethod(global, "sum", &Atanortreemapl::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Atanortreemapl::AddMethod(global, "product", &Atanortreemapl::MethodProduct, P_NONE, "product(): return the product of elements.");
    Atanortreemapl::AddMethod(global, "pop", &Atanortreemapl::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Atanortreemapl::AddMethod(global, "merge", &Atanortreemapl::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Atanortreemapl::idtype] = new Atanortreemapl(global);
    
    global->RecordMethods(Atanortreemapl::idtype, Atanortreemapl::exported);

    return true;
}


Exporting AtanorIteration* Atanortreemapl::Newiteration(bool direction) {
    return new AtanorIterationtreemapl(this, direction);
}


Exporting Atanor* Atanortreemapl::in(Atanor* context, Atanor* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    BLONG val = a->Long();

    if (context->isBoolean()) {
        Locking _lock(this);
        if (values.find(val)!=values.end())
            return aTRUE;
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Atanorlvector* v = (Atanorlvector*)Selectalvector(context);
        Doublelocking _lock(this, v);
        if (values.find(val) != values.end())
            v->values.push_back(val);

        return v;
    }
    
    Locking _lock(this);
    if (values.find(val)!=values.end())
        return new Atanorlong(val);

    return aNULL;

}

Exporting Atanor* Atanortreemapl::MethodFind(Atanor* context, short idthread, AtanorCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    
    Atanor* a = callfunc->Evaluate(0,context,idthread);
    if (context->isBoolean()) {
        Locking _lock(this);
        for (auto& it : values) {
            if (it.second->same(a) == aTRUE)
                return aTRUE;
        }
        return aFALSE;
    }
    if (context->isVectorContainer()) {
        Atanorlvector* v = (Atanorlvector*)Selectalvector(context);
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
            return new Atanorlong(it.first);
    }
    return aNULL;
}


Exporting void Atanortreemapl::Setreference(short inc) {
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
static void resetMap(Atanortreemapl* kmap, long inc) {
    Locking _lock(kmap);
    kmap->reference -= inc;
    map<BLONG, Atanor*>& values = kmap->values;
    if (values.size() == 0)
        return;

    for (auto& itx : values)
        itx.second->Resetreference(inc);
}

Exporting void Atanortreemapl::Resetreference(short inc) {
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


Exporting Atanor* Atanortreemapl::Push(Atanor* k, Atanor* v) {
    Locking _lock(this);
    BLONG s = k->Long();
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

Exporting Atanor* Atanortreemapl::Pop(Atanor* kkey) {
    BLONG k = kkey->Long();
    if (values.find(k) != values.end()) {
        kkey = values[k];
        values.erase(k);
        kkey->Resetreference(reference + 1);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Atanortreemapl::Clear() {
    Locking _lock(this);

    for (auto& itx : values)
        itx.second->Resetreference(reference + 1);
    values.clear();
}

Exporting void Atanortreemapl::Clean() {
    //Your cleaning code
}

Exporting string Atanortreemapl::String() {
    Locking _lock(this);
    stringstream res;

    res << "{";
    bool beg = true;
    string sx;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it.first << ":";
        sx = it.second->StringToDisplay();
        if (!it.second->isString() || it.second->isContainer())
            res << sx;
        else
            stringing(res, sx);
    }
    res << "}";
    return res.str();
}

Exporting string Atanortreemapl::JSonString() {
    Locking _lock(this);
    stringstream res;

    res << "{";
    bool beg = true;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        res << '"' << it.first << '"' << ":" << it.second->JSonString();
    }
    res << "}";
    return res.str();
}


Exporting long Atanortreemapl::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanortreemapl::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanortreemapl::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanortreemapl::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Atanortreemapl::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Atanor*  Atanortreemapl::Put(Atanor* idx, Atanor* ke, short idthread) {
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
                Push(itr->Keylong(), itr->Value());
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

            for (auto& it : kvect->values) {
                Push(nb, it);
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalAtanor->Returnerror("Wrong map initialization", idthread);
        if (ke != this) {
            Clear();
            if (ke->Type() == Atanortreemapl::idtype) {
                Atanortreemapl* kmap = (Atanortreemapl*)ke;
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
                    values[itr->Keylong()] = a;
                    a->Setreference(reference + 1);
                }
                itr->Release();
            }
            ke->Release();
        }
        return aTRUE;
    }
    Push(idx->Long(), ke);
    return aTRUE;
}

Exporting Atanor* Atanortreemapl::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Atanor* vect = contextualpattern->Newinstance(idthread);
            map<BLONG, Atanor*>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(new Atanorlong(it->first));
            return vect;
        }

        if (contextualpattern->Type() == a_int || contextualpattern->Type() == a_float)
            return globalAtanor->Provideint(values.size());

        return this;
    }

    Atanor* key;
    if (idx->isInterval()) {
        Atanortreemapl* kmap = new Atanortreemapl;
        key = ((AtanorIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Atanor* keyright = ((AtanorIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        BLONG vleft = key->Long();
        BLONG vright = keyright->Long();
        map<BLONG, Atanor*>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        map<BLONG, Atanor*>::iterator itr = values.end();
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

    BLONG skey = key->Long();
    key->Release();

    Atanor* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalAtanor->erroronkey)
            return globalAtanor->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }
    return kval;
}

Exporting Atanor* Atanortreemapl::same(Atanor* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Atanortreemapl* m = (Atanortreemapl*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    map<BLONG, Atanor*>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second->same(values[it->first]) == aFALSE)
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Atanor* Atanortreemapl::xorset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);
    Atanortreemapl* res;
    
    
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapl;
        hmap<BLONG, Atanor*> keys;

        for (auto& it : values)
            keys[it.first] = it.second;
            
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keylong();
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
        res = (Atanortreemapl*)Atom(true);

    for (auto& it : values)
        it.second->xorset(b, true);
    return res;
}

Exporting Atanor* Atanortreemapl::orset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapl* res;
    if (itself)
        res = this;
    else
        res = (Atanortreemapl*)Atom(true);
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    for (auto& it : res->values)
        it.second->orset(b, true);
    return res;
}

Exporting Atanor* Atanortreemapl::andset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapl* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        res = new Atanortreemapl;
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            if (values.find(itr->Keylong()) != values.end() && values[itr->Keylong()]->same(v) == aTRUE)
                res->Push(itr->Keylong(), v);
        }
        itr->Release();
        return res;
    }
    if (itself)
        res = this;
    else
        res = (Atanortreemapl*)Atom(true);
    for (auto& it : res->values)
        it.second->andset(b, true);
    return res;
}

Exporting Atanor* Atanortreemapl::plus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapl* res;
    if (b->isMapContainer()) {
        map<BLONG, Atanor*>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapl;
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keylong());
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
        res = (Atanortreemapl*)Atom(true);

    for (auto& it : res->values)
        it.second->plus(b, true);
    return res;
}

Exporting Atanor* Atanortreemapl::minus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapl* res;
    if (b->isMapContainer()) {
        map<BLONG, Atanor*>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapl;
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keylong());
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
        res = (Atanortreemapl*)Atom(true);

    for (auto& it : res->values)
        it.second->minus(b, true);
    return res;
}

Exporting Atanor* Atanortreemapl::multiply(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapl* res;
    if (b->isMapContainer()) {
        map<BLONG, Atanor*>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapl;
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keylong());
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
        res = (Atanortreemapl*)Atom(true);

    for (auto& it : res->values)
        it.second->multiply(b, true);
    return res;
}

Exporting Atanor* Atanortreemapl::divide(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapl* res;
    Atanor* v;
    Atanor* r;

    if (b->isMapContainer()) {
        map<BLONG, Atanor*>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapl;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keylong());
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
        res = (Atanortreemapl*)Atom(true);

    for (auto& it : res->values) {
        r = it.second->divide(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Atanor* Atanortreemapl::mod(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapl* res;
    Atanor* v;
    Atanor* r;

    if (b->isMapContainer()) {
        map<BLONG, Atanor*>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapl;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keylong());
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
        res = (Atanortreemapl*)Atom(true);

    for (auto& it : res->values) {
        r = it.second->mod(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Atanor* Atanortreemapl::shiftright(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapl* res;
    if (b->isMapContainer()) {
        map<BLONG, Atanor*>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapl;
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keylong());
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
        res = (Atanortreemapl*)Atom(true);

    for (auto& it : res->values)
        it.second->shiftright(b, true);
    return res;
}

Exporting Atanor* Atanortreemapl::shiftleft(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapl* res;
    if (b->isMapContainer()) {
        map<BLONG, Atanor*>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapl;
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keylong());
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
        res = (Atanortreemapl*)Atom(true);

    for (auto& it : res->values)
        it.second->shiftleft(b, true);
    return res;
}

Exporting Atanor* Atanortreemapl::power(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapl* res;
    if (b->isMapContainer()) {
        map<BLONG, Atanor*>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapl;
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keylong());
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
        res = (Atanortreemapl*)Atom(true);

    for (auto& it : res->values)
        it.second->power(b, true);
    return res;
}




Exporting Atanor* Atanortreemapl::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
    Locking _lock(this);
    Atanor* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    map<BLONG, Atanor*>::iterator it;
    
    Atanor* a;
    vector<BLONG> keys;

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
