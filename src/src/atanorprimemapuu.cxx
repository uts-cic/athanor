
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
filename   : atanorprimemapuu.cxx
Date       : 2017/04/01
Purpose    : map implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanoruvector.h"
#include "atanorustring.h"
//#prime
#include "atanorprimemapuu.h"
#include "atanorlist.h"
#include "atanorivector.h"


//We need to declare once again our local definitions.
Exporting hmap<unsigned short, primemapuuMethod>  Atanorprimemapuu::methods;
Exporting hmap<string, string> Atanorprimemapuu::infomethods;
Exporting bin_hash<unsigned long> Atanorprimemapuu::exported;

Exporting short Atanorprimemapuu::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorprimemapuu::AddMethod(AtanorGlobal* global, string name,primemapuuMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanorprimemapuu::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Atanorprimemapuu::idtype = global->Getid("primemapuu");

    Atanorprimemapuu::AddMethod(global, "clear", &Atanorprimemapuu::MethodClear, P_NONE, "clear(): clear the container.");
    
    Atanorprimemapuu::AddMethod(global, "invert", &Atanorprimemapuu::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Atanorprimemapuu::AddMethod(global, "find", &Atanorprimemapuu::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Atanorprimemapuu::AddMethod(global, "items", &Atanorprimemapuu::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Atanorprimemapuu::AddMethod(global, "join", &Atanorprimemapuu::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Atanorprimemapuu::AddMethod(global, "test", &Atanorprimemapuu::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Atanorprimemapuu::AddMethod(global, "keys", &Atanorprimemapuu::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Atanorprimemapuu::AddMethod(global, "values", &Atanorprimemapuu::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Atanorprimemapuu::AddMethod(global, "pop", &Atanorprimemapuu::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Atanorprimemapuu::AddMethod(global, "merge", &Atanorprimemapuu::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Atanorprimemapuu::idtype] = new Atanorprimemapuu(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("uuprimemap")] = new Atanorprimemapuu(global);

    global->RecordMethods(global->Getid("uuprimemap"), Atanorprimemapuu::exported);
    #endif
    global->RecordMethods(Atanorprimemapuu::idtype, Atanorprimemapuu::exported);

    return true;
}


Exporting AtanorIteration* Atanorprimemapuu::Newiteration(bool direction) {
    return new AtanorIterationprimemapuu(this, direction);
}


Exporting Atanor* Atanorprimemapuu::in(Atanor* context, Atanor* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    wstring val = a->UString();

    if (context->isBoolean()) {
        Locking _lock(this);
        if (values.find(val)!=values.end())
            return aTRUE;
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Atanoruvector* v = (Atanoruvector*)Selectauvector(context);
        Doublelocking _lock(this, v);
        if (values.find(val)!=values.end())
            v->values.push_back(val);

        return v;
    }
    
    Locking _lock(this);
    if (values.find(val)!=values.end())
        return globalAtanor->Provideustring(val);

    return aNULL;

}

Exporting Atanor* Atanorprimemapuu::MethodFind(Atanor* context, short idthread, AtanorCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    prime_hash<wstring, wstring>::iterator it;
    
    wstring val = callfunc->Evaluate(0, context, idthread)->UString();
    
    if (context->isBoolean()) {
        Locking _lock(this);
        for (it = values.begin(); it != values.end(); it++) {
            if (it->second == val)
                return aTRUE;
        }
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Atanoruvector* v = (Atanoruvector*)Selectauvector(context);
        Doublelocking _lock(this, v);
        for (it = values.begin(); it != values.end(); it++) {
            if (it->second == val)
                v->values.push_back(it->first);
        }
        return v;
    }
    
    Locking _lock(this);
    for (it = values.begin(); it != values.end(); it++) {
        if (it->second == val)
            return globalAtanor->Provideustring(it->first);
    }

    return aNULL;

}



Exporting Atanor* Atanorprimemapuu::Push(Atanor* k, Atanor* v) {
    Locking _lock(this);
    wstring s = k->UString();
    values[s] = v->UString();
    return aTRUE;
}

Exporting Atanor* Atanorprimemapuu::Pop(Atanor* kkey) {
    wstring k = kkey->UString();
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Atanorprimemapuu::Clear() {
    Locking _lock(this);
    values.clear();
}

Exporting void Atanorprimemapuu::Clean() {
    //Your cleaning code
}

Exporting string Atanorprimemapuu::String() {
    Locking _lock(this);
    stringstream res;
    prime_hash<wstring,wstring>::iterator it;
    res << "{";
    bool beg = true;
    string sx;
    wstring w;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        w = it->first;
        sc_unicode_to_utf8(sx, w);
        stringing(res, sx);
        res << ":";
        w = it->second;
        sc_unicode_to_utf8(sx, w);
        stringing(res, sx);
    }

    res << "}";
    return res.str();
}

Exporting string Atanorprimemapuu::JSonString() {
    Locking _lock(this);
    stringstream res;
    prime_hash<wstring,wstring>::iterator it;
    res << "{";
    bool beg = true;
    string sx;
    wstring w;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        sc_unicode_to_utf8(sx, w);
        jstringing(res, sx);
        res << ":";
        w = it->second;
        sc_unicode_to_utf8(sx, w);
        jstringing(res, sx);
    }
    res << "}";
    return res.str();
}


Exporting long Atanorprimemapuu::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanorprimemapuu::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanorprimemapuu::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanorprimemapuu::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Atanorprimemapuu::Size() {
    Locking _lock(this);
    return values.size();
}

#ifndef swprintf_s
#define swprintf_s(a,b,c,d) swprintf(a,b,c,d)
    #endif


Exporting Atanor*  Atanorprimemapuu::Put(Atanor* idx, Atanor* ke, short idthread) {
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
            values = ((Atanorprimemapuu*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            values.clear();
            AtanorIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keyustring()] = itr->Valueustring();
            itr->Release();
            return aTRUE;
        }
        
        wchar_t buff[20];
        if (ke->isVectorContainer()) {
            values.clear();
            nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                swprintf_s(buff, 20, L"%d", nb);
                values[buff] = ke->getustring(it);
                nb++;
            }
            return aTRUE;
        }

        if (ke->Type() == a_list) {
            Atanorlist* kvect = (Atanorlist*)ke;
            values.clear();
            nb = 0;
            list<Atanor*>::iterator it;
            for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
                swprintf_s(buff, 20, L"%d", nb);
                values[buff] = (*it)->UString();
                nb++;
            }
            return aTRUE;
        }


        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalAtanor->Returnerror("Wrong map initialization", idthread);

        if (ke != this) {
            values.clear();
            if (ke->Type() == Atanorprimemapuu::idtype) {
                Atanorprimemapuu* kmap = (Atanorprimemapuu*)ke;
                //We copy all values from ke to this
                prime_hash<wstring,wstring>::iterator it;
                for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                    values[it->first] = it->second;
                kmap->Release();
            }
            else {
                AtanorIteration* itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                    values[itr->Keyustring()] = itr->Valueustring();
                itr->Release();
            }
            ke->Release();
        }
        return aTRUE;
    }

    values[idx->UString()] = ke->UString();
    return aTRUE;
}

Exporting Atanor* Atanorprimemapuu::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Atanor* vect = contextualpattern->Newinstance(idthread);
            prime_hash<wstring,wstring>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalAtanor->Provideustring(it->first));
            return vect;
        }

        if (contextualpattern->Type() == a_int || contextualpattern->Type() == a_float)
            return globalAtanor->Provideint(values.size());

        return this;
    }

    Atanor* key;
    if (idx->isInterval()) {
        Atanorprimemapuu* kmap = new Atanorprimemapuu;
        key = ((AtanorIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Atanor* keyright = ((AtanorIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        wstring vleft = key->UString();
        wstring vright = keyright->UString();
        prime_hash<wstring,wstring>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        prime_hash<wstring, wstring>::iterator itr= values.end();
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

    wstring skey = key->UString();
    key->Release();

    Atanor* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalAtanor->erroronkey)
            return globalAtanor->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }
    return kval;
}

Exporting Atanor* Atanorprimemapuu::same(Atanor* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Atanorprimemapuu* m = (Atanorprimemapuu*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    prime_hash<wstring,wstring>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Atanor* Atanorprimemapuu::xorset(Atanor* b, bool itself) {

    Doublelocking _lock(this, b);


    Atanorprimemapuu* res;
    
    
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapuu;
        hmap<wstring, wstring> keys;

        prime_hash<wstring, wstring>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;
            
        wstring v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyustring();
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

Exporting Atanor* Atanorprimemapuu::orset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,wstring>::iterator it;
    Atanorprimemapuu* res;
    if (itself)
        res = this;
    else
        res = (Atanorprimemapuu*)Atom(true);

    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }

    return aNULL;
}

Exporting Atanor* Atanorprimemapuu::andset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,wstring>::iterator it;
    Atanorprimemapuu* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapuu;
        wstring v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            if (values.find(itr->Keyustring()) != values.end() && values[itr->Keyustring()]==v)
                res->values[itr->Keyustring()] = v;
        }
        itr->Release();
        return res;
    }

    return aNULL;
}

Exporting Atanor* Atanorprimemapuu::plus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,wstring>::iterator it;
    Atanorprimemapuu* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapuu;
        wstring v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            it = values.find(itr->Keyustring());
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
        res = (Atanorprimemapuu*)Atom(true);

    wstring v = b->UString();
    for (it = values.begin(); it != values.end(); it++)
        it->second += v;
    return res;
}

Exporting Atanor* Atanorprimemapuu::minus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,wstring>::iterator it;
    Atanorprimemapuu * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapuu;
        wstring v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            it = values.find(itr->Keyustring());
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
        res = (Atanorprimemapuu*)Atom(true);

    wstring v = b->UString();
    for (it = values.begin(); it != values.end(); it++)
        it->second = StringMinus(it->second,v);
    return res;
}

Exporting Atanor* Atanorprimemapuu::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
    Locking _lock(this);
    Atanor* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    
    prime_hash<wstring, wstring>::iterator it;
    
    Atanor* a;
    vector<wstring> keys;

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
