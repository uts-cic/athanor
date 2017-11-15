
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
filename   : atanortreemapss.cxx
Date       : 2017/04/01
Purpose    : map implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorsvector.h"
#include "atanorstring.h"
#include "atanortreemapss.h"
#include "atanorlist.h"
#include "atanorivector.h"


//We need to declare once again our local definitions.
Exporting hmap<unsigned short, treemapssMethod>  Atanortreemapss::methods;
Exporting hmap<string, string> Atanortreemapss::infomethods;
Exporting bin_hash<unsigned long> Atanortreemapss::exported;

Exporting short Atanortreemapss::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanortreemapss::AddMethod(AtanorGlobal* global, string name,treemapssMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanortreemapss::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Atanortreemapss::idtype = global->Getid("treemapss");

    Atanortreemapss::AddMethod(global, "clear", &Atanortreemapss::MethodClear, P_NONE, "clear(): clear the container.");
    
    Atanortreemapss::AddMethod(global, "invert", &Atanortreemapss::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Atanortreemapss::AddMethod(global, "find", &Atanortreemapss::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Atanortreemapss::AddMethod(global, "items", &Atanortreemapss::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Atanortreemapss::AddMethod(global, "join", &Atanortreemapss::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Atanortreemapss::AddMethod(global, "test", &Atanortreemapss::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Atanortreemapss::AddMethod(global, "keys", &Atanortreemapss::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Atanortreemapss::AddMethod(global, "values", &Atanortreemapss::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Atanortreemapss::AddMethod(global, "pop", &Atanortreemapss::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Atanortreemapss::AddMethod(global, "merge", &Atanortreemapss::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Atanortreemapss::idtype] = new Atanortreemapss(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("sstreemap")] = new Atanortreemapss(global);

    global->RecordMethods(global->Getid("sstreemap"), Atanortreemapss::exported);
    #endif
    global->RecordMethods(Atanortreemapss::idtype, Atanortreemapss::exported);

    return true;
}


Exporting AtanorIteration* Atanortreemapss::Newiteration(bool direction) {
    return new AtanorIterationtreemapss(this, direction);
}


Exporting Atanor* Atanortreemapss::in(Atanor* context, Atanor* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    string val = a->String();

    if (context->isBoolean()) {
        Locking _lock(this);
        if (values.find(val)!=values.end())
            return aTRUE;
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Atanorsvector* v = (Atanorsvector*)Selectasvector(context);
        Doublelocking _lock(this, v);
        if (values.find(val)!=values.end())
            v->values.push_back(val);

        return v;
    }
    
    Locking _lock(this);
    if (values.find(val)!=values.end())
        return globalAtanor->Providestring(val);

    return aNULL;

}

Exporting Atanor* Atanortreemapss::MethodFind(Atanor* context, short idthread, AtanorCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    
    string val = callfunc->Evaluate(0, context, idthread)->String();
    
    if (context->isBoolean()) {
        Locking _lock(this);
        for (auto& it : values) {
            if (it.second == val)
                return aTRUE;
        }
        return aFALSE;
    }
    if (context->isVectorContainer()) {
        Atanorsvector* v = (Atanorsvector*)Selectasvector(context);
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
            return globalAtanor->Providestring(it.first);
    }
    return aNULL;
}



Exporting Atanor* Atanortreemapss::Push(Atanor* k, Atanor* v) {
    Locking _lock(this);
    string s = k->String();
    values[s] = v->String();
    return aTRUE;
}

Exporting Atanor* Atanortreemapss::Pop(Atanor* kkey) {
    string k = kkey->String();
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Atanortreemapss::Clear() {
    Locking _lock(this);
    values.clear();
}

Exporting void Atanortreemapss::Clean() {
    //Your cleaning code
}

Exporting string Atanortreemapss::String() {
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
        sx = it.second;
        stringing(res, sx);
    }
    res << "}";
    return res.str();
}

Exporting string Atanortreemapss::JSonString() {
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
        res << ":";
        sx = it.second;
        jstringing(res, sx);
    }
    res << "}";
    return res.str();
}


Exporting long Atanortreemapss::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanortreemapss::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanortreemapss::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanortreemapss::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Atanortreemapss::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Atanor*  Atanortreemapss::Put(Atanor* idx, Atanor* ke, short idthread) {
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
            values = ((Atanortreemapss*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            values.clear();
            AtanorIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keystring()] = itr->Valuestring();
            itr->Release();
            return aTRUE;
        }
        
        char buff[20];
        if (ke->isVectorContainer()) {
            values.clear();
            nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                sprintf_s(buff, 20, "%d", nb);
                values[buff] = ke->getstring(it);
                nb++;
            }
            return aTRUE;
        }
        if (ke->Type() == a_list) {
            Atanorlist* kvect = (Atanorlist*)ke;
            values.clear();
            nb = 0;

            for (auto& it : kvect->values) {
                sprintf_s(buff, 20, "%d", nb);
                values[buff] = it->String();
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalAtanor->Returnerror("Wrong map initialization", idthread);
        if (ke != this) {
            values.clear();
            if (ke->Type() == Atanortreemapss::idtype)
                values = ((Atanortreemapss*)ke)->values;
            else {
                AtanorIteration* itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                    values[itr->Keystring()] = itr->Valuestring();
                itr->Release();
            }
            ke->Release();
        }
        return aTRUE;
    }
    values[idx->String()] = ke->String();
    return aTRUE;
}

Exporting Atanor* Atanortreemapss::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Atanor* vect = contextualpattern->Newinstance(idthread);
            map<string,string>::iterator it;
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
        Atanortreemapss* kmap = new Atanortreemapss;
        key = ((AtanorIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Atanor* keyright = ((AtanorIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        string vleft = key->String();
        string vright = keyright->String();
        map<string,string>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        map<string, string>::iterator itr= values.end();
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

Exporting Atanor* Atanortreemapss::same(Atanor* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Atanortreemapss* m = (Atanortreemapss*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    map<string,string>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Atanor* Atanortreemapss::xorset(Atanor* b, bool itself) {

    Doublelocking _lock(this, b);


    Atanortreemapss* res;
    
    
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapss;
        hmap<string, string> keys;

        for (auto& it : values)
            keys[it.first] = it.second;
            
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keystring();
            if (keys.find(v) == keys.end())
                keys[v]=itr->Valuestring();
            else {
                if (values[v]==itr->Valuestring())
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

Exporting Atanor* Atanortreemapss::orset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapss* res;
    if (itself)
        res = this;
    else
        res = (Atanortreemapss*)Atom(true);

    if (b->isMapContainer()) {
        map<string,string>::iterator it;

        res->Merging(b);
        return res;
    }

    return aNULL;
}

Exporting Atanor* Atanortreemapss::andset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapss* res;
    if (b->isMapContainer()) {
        map<string,string>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapss;
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            if (values.find(itr->Keystring()) != values.end() && values[itr->Keystring()]==v)
                res->values[itr->Keystring()] = v;
        }
        itr->Release();
        return res;
    }

    return aNULL;
}

Exporting Atanor* Atanortreemapss::plus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapss* res;
    if (b->isMapContainer()) {
        map<string,string>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapss;
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            it = values.find(itr->Keystring());
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
        res = (Atanortreemapss*)Atom(true);

    string v = b->String();
    for (auto& it : res->values)
        it.second += v;
    return res;
}

Exporting Atanor* Atanortreemapss::minus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapss * res;
    if (b->isMapContainer()) {
        map<string,string>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapss;
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            it = values.find(itr->Keystring());
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
        res = (Atanortreemapss*)Atom(true);

    string v = b->String();
    for (auto& it : res->values)
        it.second = StringMinus(it.second,v);
    return res;
}

Exporting Atanor* Atanortreemapss::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
    Locking _lock(this);
    Atanor* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    
    map<string, string>::iterator it;
    
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
