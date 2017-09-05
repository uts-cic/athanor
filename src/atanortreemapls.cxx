
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
filename   : atanortreemapls.cxx
Date       : 2017/04/01
Purpose    : map implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"

#include "atanorint.h"
//#prime
#include "atanortreemapls.h"
#include "atanorlist.h"
#include "atanorlvector.h"


//We need to declare once again our local definitions.
Exporting hmap<unsigned short, treemaplsMethod>  Atanortreemapls::methods;
Exporting hmap<string, string> Atanortreemapls::infomethods;
Exporting bin_hash<unsigned long> Atanortreemapls::exported;

Exporting short Atanortreemapls::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanortreemapls::AddMethod(AtanorGlobal* global, string name,treemaplsMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanortreemapls::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Atanortreemapls::idtype = global->Getid("treemapls");

    Atanortreemapls::AddMethod(global, "clear", &Atanortreemapls::MethodClear, P_NONE, "clear(): clear the container.");
    
    Atanortreemapls::AddMethod(global, "invert", &Atanortreemapls::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Atanortreemapls::AddMethod(global, "find", &Atanortreemapls::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Atanortreemapls::AddMethod(global, "items", &Atanortreemapls::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Atanortreemapls::AddMethod(global, "join", &Atanortreemapls::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Atanortreemapls::AddMethod(global, "test", &Atanortreemapls::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Atanortreemapls::AddMethod(global, "keys", &Atanortreemapls::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Atanortreemapls::AddMethod(global, "values", &Atanortreemapls::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Atanortreemapls::AddMethod(global, "pop", &Atanortreemapls::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Atanortreemapls::AddMethod(global, "merge", &Atanortreemapls::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Atanortreemapls::idtype] = new Atanortreemapls(global);
    
    global->RecordMethods(Atanortreemapls::idtype, Atanortreemapls::exported);

    return true;
}


Exporting AtanorIteration* Atanortreemapls::Newiteration(bool direction) {
    return new AtanorIterationtreemapls(this, direction);
}


Exporting Atanor* Atanortreemapls::in(Atanor* context, Atanor* a, short idthread) {
    //Three cases aBLONG the container type...
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
        if (values.find(val)!=values.end())
            v->values.push_back(val);

        return v;
    }
    
    Locking _lock(this);
    if (values.find(val)!=values.end())
        return new Atanorlong(val);

    return aNULL;

}

Exporting Atanor* Atanortreemapls::MethodFind(Atanor* context, short idthread, AtanorCall* callfunc) {
    //Three cases aBLONG the container type...
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
        Atanorlvector* v = (Atanorlvector*)Selectalvector(context);
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
            return new Atanorlong(it.first);
    }
    return aNULL;
}



Exporting Atanor* Atanortreemapls::Push(Atanor* k, Atanor* v) {
    Locking _lock(this);
    BLONG s = k->Long();
    values[s] = v->String();
    return aTRUE;
}

Exporting Atanor* Atanortreemapls::Pop(Atanor* kkey) {
    BLONG k = kkey->Long();
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Atanortreemapls::Clear() {
    Locking _lock(this);
    values.clear();
}

Exporting void Atanortreemapls::Clean() {
    //Your cleaning code
}

Exporting string Atanortreemapls::String() {
    Locking _lock(this);
    stringstream res;

    res << "{";
    bool beg = true;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it.first << ":";
        stringing(res, it.second);
    }
    res << "}";
    return res.str();
}

Exporting string Atanortreemapls::JSonString() {
    Locking _lock(this);
    stringstream res;

    res << "{";
    bool beg = true;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        res << '"' << it.first << '"' << ":";
        jstringing(res, it.second);
    }
    res << "}";
    return res.str();
}


Exporting long Atanortreemapls::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanortreemapls::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanortreemapls::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanortreemapls::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Atanortreemapls::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Atanor*  Atanortreemapls::Put(Atanor* idx, Atanor* ke, short idthread) {
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
            values = ((Atanortreemapls*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            values.clear();
            AtanorIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keylong()] = itr->Valuestring();
            itr->Release();
            return aTRUE;
        }
        
        if (ke->isVectorContainer()) {
            values.clear();
            nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                values[nb] = ke->getstring(it);
                nb++;
            }
            return aTRUE;
        }
        if (ke->Type() == a_list) {
            Atanorlist* kvect = (Atanorlist*)ke;
            values.clear();
            nb = 0;

            for (auto& it : kvect->values) {
                values[nb] = it->String();
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalAtanor->Returnerror("Wrong map initialization", idthread);
        if (ke != this) {
            values.clear();
            if (ke->Type() == Atanortreemapls::idtype)
                values = ((Atanortreemapls*)ke)->values;
            else {
                AtanorIteration* itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                    values[itr->Keylong()] = itr->Valuestring();
                itr->Release();
            }
            ke->Release();
        }
        return aTRUE;
    }
    values[idx->Long()] = ke->String();
    return aTRUE;
}

Exporting Atanor* Atanortreemapls::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Atanor* vect = contextualpattern->Newinstance(idthread);
            map<BLONG,string>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(new Atanorlong(it->first));
            return vect;
        }

        if (contextualpattern->Type() == a_int || contextualpattern->Type() == a_float)
            return new Atanorlong(values.size());

        return this;
    }

    Atanor* key;
    if (idx->isInterval()) {
        Atanortreemapls* kmap = new Atanortreemapls;
        key = ((AtanorIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Atanor* keyright = ((AtanorIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        BLONG vleft = key->Long();
        BLONG vright = keyright->Long();
        map<BLONG,string>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        map<BLONG, string>::iterator itr= values.end();
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

Exporting Atanor* Atanortreemapls::same(Atanor* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Atanortreemapls* m = (Atanortreemapls*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    map<BLONG,string>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Atanor* Atanortreemapls::xorset(Atanor* b, bool itself) {

    Doublelocking _lock(this, b);


    Atanortreemapls* res;
    
    
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapls;
        hmap<BLONG, string> keys;

        for (auto& it : values)
            keys[it.first] = it.second;
            
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keylong();
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

Exporting Atanor* Atanortreemapls::orset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapls* res;
    if (itself)
        res = this;
    else
        res = (Atanortreemapls*)Atom(true);

    if (b->isMapContainer()) {
        map<BLONG,string>::iterator it;

        res->Merging(b);
        return res;
    }

    return aNULL;
}

Exporting Atanor* Atanortreemapls::andset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapls* res;
    if (b->isMapContainer()) {
        map<BLONG,string>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapls;
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            if (values.find(itr->Keylong()) != values.end() && values[itr->Keylong()]==v)
                res->values[itr->Keylong()] = v;
        }
        itr->Release();
        return res;
    }

    return aNULL;
}

Exporting Atanor* Atanortreemapls::plus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapls* res;
    if (b->isMapContainer()) {
        map<BLONG,string>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapls;
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            it = values.find(itr->Keylong());
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
        res = (Atanortreemapls*)Atom(true);

    string v = b->String();
    for (auto& it : res->values)
        it.second += v;
    return res;
}

Exporting Atanor* Atanortreemapls::minus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapls * res;
    if (b->isMapContainer()) {
        map<BLONG,string>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapls;
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            it = values.find(itr->Keylong());
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
        res = (Atanortreemapls*)Atom(true);

    string v = b->String();
    for (auto& it : res->values)
        it.second = StringMinus(it.second,v);
    return res;
}

Exporting Atanor* Atanortreemapls::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
    Locking _lock(this);
    Atanor* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    
    map<BLONG, string>::iterator it;
    
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