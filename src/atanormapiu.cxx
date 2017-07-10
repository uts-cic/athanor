
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
filename   : atanormapiu.cxx
Date       : 2017/04/01
Purpose    : map implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"

#include "atanorint.h"
//#prime
#include "atanormapiu.h"
#include "atanorlist.h"
#include "atanorivector.h"


//We need to declare once again our local definitions.
Exporting hmap<unsigned short, mapiuMethod>  Atanormapiu::methods;
Exporting hmap<string, string> Atanormapiu::infomethods;
Exporting bin_hash<unsigned long> Atanormapiu::exported;

Exporting short Atanormapiu::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanormapiu::AddMethod(AtanorGlobal* global, string name,mapiuMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanormapiu::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Atanormapiu::idtype = global->Getid("mapiu");

    Atanormapiu::AddMethod(global, "clear", &Atanormapiu::MethodClear, P_NONE, "clear(): clear the container.");
    
    Atanormapiu::AddMethod(global, "invert", &Atanormapiu::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Atanormapiu::AddMethod(global, "find", &Atanormapiu::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Atanormapiu::AddMethod(global, "items", &Atanormapiu::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Atanormapiu::AddMethod(global, "join", &Atanormapiu::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Atanormapiu::AddMethod(global, "test", &Atanormapiu::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Atanormapiu::AddMethod(global, "keys", &Atanormapiu::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Atanormapiu::AddMethod(global, "values", &Atanormapiu::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Atanormapiu::AddMethod(global, "pop", &Atanormapiu::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Atanormapiu::AddMethod(global, "merge", &Atanormapiu::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Atanormapiu::idtype] = new Atanormapiu(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("iumap")] = new Atanormapiu(global);

    global->RecordMethods(global->Getid("iumap"), Atanormapiu::exported);
    #endif
    global->RecordMethods(Atanormapiu::idtype, Atanormapiu::exported);

    return true;
}


Exporting AtanorIteration* Atanormapiu::Newiteration(bool direction) {
    return new AtanorIterationmapiu(this, direction);
}


Exporting Atanor* Atanormapiu::in(Atanor* context, Atanor* a, short idthread) {
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
        if (values.find(val)!=values.end())
            v->values.push_back(val);

        return v;
    }
    
    Locking _lock(this);
    if (values.find(val)!=values.end())
        return globalAtanor->Provideint(val);

    return aNULL;

}

Exporting Atanor* Atanormapiu::MethodFind(Atanor* context, short idthread, AtanorCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    
    wstring val = callfunc->Evaluate(0, context, idthread)->UString();
    
    if (context->isBoolean()) {
        Locking _lock(this);
        for (auto& it : values) {
            if (it.second == val)
                return aTRUE;
        }
        return aFALSE;
    }
    if (context->isVectorContainer()) {
        Atanorivector* v = (Atanorivector*)Selectaivector(context);
        Doublelocking _lock(this, v);
        for (auto& it : values) {
            if (it.second == val)
                v->values.push_back(it.first);
        }
        return v;
    }
    
    Locking _lock(this);
    for (auto& it : values) {
        if (it.second == val)
            return globalAtanor->Provideint(it.first);
    }
    return aNULL;
}



Exporting Atanor* Atanormapiu::Push(Atanor* k, Atanor* v) {
    Locking _lock(this);
    long s = k->Integer();
    values[s] = v->UString();
    return aTRUE;
}

Exporting Atanor* Atanormapiu::Pop(Atanor* kkey) {
    long k = kkey->Integer();
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Atanormapiu::Clear() {
    Locking _lock(this);
    values.clear();
}

Exporting void Atanormapiu::Clean() {
    //Your cleaning code
}

Exporting string Atanormapiu::String() {
    Locking _lock(this);
    stringstream res;

    res << "{";
    bool beg = true;
    string sx;
    wstring w;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it.first << ":";
        w = it.second;
        sc_unicode_to_utf8(sx, w);
        stringing(res, sx);
    }
    res << "}";
    return res.str();
}

Exporting string Atanormapiu::JSonString() {
    Locking _lock(this);
    stringstream res;

    res << "{";
    bool beg = true;
    string sx;
    wstring w;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        res << '"' << it.first << '"' << ":";
        w = it.second;
        sc_unicode_to_utf8(sx, w);
        jstringing(res, sx);
    }
    res << "}";
    return res.str();
}


Exporting long Atanormapiu::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanormapiu::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanormapiu::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanormapiu::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Atanormapiu::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Atanor*  Atanormapiu::Put(Atanor* idx, Atanor* ke, short idthread) {
    if (ke == this)
        return aTRUE;
    Doublelocking _lock(this, ke);
    int nb;
    if (!idx->isIndex()) {
        if (ke == aNULL) {
            values.clear();
            return aTRUE;
        }
        
        if (ke->Type() == Type()) {
            values = ((Atanormapiu*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            values.clear();
            AtanorIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keyinteger()] = itr->Valueustring();
            itr->Release();
            return aTRUE;
        }
        
        if (ke->isVectorContainer()) {
            values.clear();
            nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                values[nb] = ke->getustring(it);
                nb++;
            }
            return aTRUE;
        }
        if (ke->Type() == a_list) {
            Atanorlist* kvect = (Atanorlist*)ke;
            values.clear();
            nb = 0;

            for (auto& it : kvect->values) {
                values[nb] = it->UString();
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalAtanor->Returnerror("Wrong map initialization", idthread);
        if (ke != this) {
            values.clear();
            if (ke->Type() == Atanormapiu::idtype)
                values = ((Atanormapiu*)ke)->values;
            else {
                AtanorIteration* itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                    values[itr->Keyinteger()] = itr->Valueustring();
                itr->Release();
            }
            ke->Release();
        }
        return aTRUE;
    }
    values[idx->Integer()] = ke->UString();
    return aTRUE;
}

Exporting Atanor* Atanormapiu::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Atanor* vect = contextualpattern->Newinstance(idthread);
            hmap<long,wstring>::iterator it;
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
        Atanormapiu* kmap = new Atanormapiu;
        key = ((AtanorIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Atanor* keyright = ((AtanorIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        long vleft = key->Integer();
        long vright = keyright->Integer();
        hmap<long,wstring>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        hmap<long, wstring>::iterator itr= values.end();
        if (keyright != aNULL) {
            itr = values.find(vright);
            if (itr == values.end()) {
                keyright->Release();
                return kmap;
            }
        }
        keyright->Release();
        for (; it != values.end(); it++) {
            kmap->values[it->first] = it->second;
            if (it == itr)
                return kmap;
        }

        if (itr != values.end())
            kmap->values.clear();
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

Exporting Atanor* Atanormapiu::same(Atanor* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Atanormapiu* m = (Atanormapiu*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    hmap<long,wstring>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Atanor* Atanormapiu::xorset(Atanor* b, bool itself) {

    Doublelocking _lock(this, b);


    Atanormapiu* res;
    
    
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanormapiu;
        hmap<long, wstring> keys;

        for (auto& it : values)
            keys[it.first] = it.second;
            
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyinteger();
            if (keys.find(v) == keys.end())
                keys[v]=itr->Valueustring();
            else {
                if (values[v]==itr->Valueustring())
                    keys.erase(v);
            }
        }
        itr->Release();

        for (auto& a : keys)
            res->values[a.first]= a.second;

        return res;


    }

    return aNULL;
}

Exporting Atanor* Atanormapiu::orset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanormapiu* res;
    if (itself)
        res = this;
    else
        res = (Atanormapiu*)Atom(true);

    if (b->isMapContainer()) {
        hmap<long,wstring>::iterator it;

        res->Merging(b);
        return res;
    }

    return aNULL;
}

Exporting Atanor* Atanormapiu::andset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanormapiu* res;
    if (b->isMapContainer()) {
        hmap<long,wstring>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanormapiu;
        wstring v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            if (values.find(itr->Keyinteger()) != values.end() && values[itr->Keyinteger()]==v)
                res->values[itr->Keyinteger()] = v;
        }
        itr->Release();
        return res;
    }

    return aNULL;
}

Exporting Atanor* Atanormapiu::plus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanormapiu* res;
    if (b->isMapContainer()) {
        hmap<long,wstring>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanormapiu;
        wstring v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            it = values.find(itr->Keyinteger());
            if (it != values.end()) {
                res->values[it->first] = it->second + v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanormapiu*)Atom(true);

    wstring v = b->UString();
    for (auto& it : res->values)
        it.second += v;
    return res;
}

Exporting Atanor* Atanormapiu::minus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanormapiu * res;
    if (b->isMapContainer()) {
        hmap<long,wstring>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanormapiu;
        wstring v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            it = values.find(itr->Keyinteger());
            if (it != values.end()) {
                res->values[it->first] = StringMinus(it->second, v);
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanormapiu*)Atom(true);

    wstring v = b->UString();
    for (auto& it : res->values)
        it.second = StringMinus(it.second,v);
    return res;
}

Exporting Atanor* Atanormapiu::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
    Locking _lock(this);
    Atanor* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    
    hmap<long, wstring>::iterator it;
    
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
