
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
filename   : atanorprimemapls.cxx
Date       : 2017/04/01
Purpose    : map implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"

#include "atanorint.h"
//#prime
#include "atanorprimemapls.h"
#include "atanorlist.h"
#include "atanorlvector.h"


//We need to declare once again our local definitions.
Exporting hmap<unsigned short, primemaplsMethod>  Atanorprimemapls::methods;
Exporting hmap<string, string> Atanorprimemapls::infomethods;
Exporting bin_hash<unsigned long> Atanorprimemapls::exported;

Exporting short Atanorprimemapls::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorprimemapls::AddMethod(AtanorGlobal* global, string name,primemaplsMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanorprimemapls::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Atanorprimemapls::idtype = global->Getid("primemapls");

    Atanorprimemapls::AddMethod(global, "clear", &Atanorprimemapls::MethodClear, P_NONE, "clear(): clear the container.");
    
    Atanorprimemapls::AddMethod(global, "invert", &Atanorprimemapls::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Atanorprimemapls::AddMethod(global, "find", &Atanorprimemapls::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Atanorprimemapls::AddMethod(global, "items", &Atanorprimemapls::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Atanorprimemapls::AddMethod(global, "join", &Atanorprimemapls::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Atanorprimemapls::AddMethod(global, "test", &Atanorprimemapls::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Atanorprimemapls::AddMethod(global, "keys", &Atanorprimemapls::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Atanorprimemapls::AddMethod(global, "values", &Atanorprimemapls::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Atanorprimemapls::AddMethod(global, "pop", &Atanorprimemapls::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Atanorprimemapls::AddMethod(global, "merge", &Atanorprimemapls::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Atanorprimemapls::idtype] = new Atanorprimemapls(global);
    
    global->RecordMethods(Atanorprimemapls::idtype, Atanorprimemapls::exported);

    return true;
}


Exporting AtanorIteration* Atanorprimemapls::Newiteration(bool direction) {
    return new AtanorIterationprimemapls(this, direction);
}


Exporting Atanor* Atanorprimemapls::in(Atanor* context, Atanor* a, short idthread) {
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

Exporting Atanor* Atanorprimemapls::MethodFind(Atanor* context, short idthread, AtanorCall* callfunc) {
    //Three cases aBLONG the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    prime_hash<BLONG, string>::iterator it;
    
    string val = callfunc->Evaluate(0, context, idthread)->String();
    
    if (context->isBoolean()) {
        Locking _lock(this);
        for (it = values.begin(); it != values.end(); it++) {
            if (it->second == val)
                return aTRUE;
        }
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Atanorlvector* v = (Atanorlvector*)Selectalvector(context);
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
            return new Atanorlong(it->first);
    }

    return aNULL;

}



Exporting Atanor* Atanorprimemapls::Push(Atanor* k, Atanor* v) {
    Locking _lock(this);
    BLONG s = k->Long();
    values[s] = v->String();
    return aTRUE;
}

Exporting Atanor* Atanorprimemapls::Pop(Atanor* kkey) {
    BLONG k = kkey->Long();
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Atanorprimemapls::Clear() {
    Locking _lock(this);
    values.clear();
}

Exporting void Atanorprimemapls::Clean() {
    //Your cleaning code
}

Exporting string Atanorprimemapls::String() {
    Locking _lock(this);
    stringstream res;
    prime_hash<BLONG,string>::iterator it;
    res << "{";
    bool beg = true;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it->first << ":";
        stringing(res, it->second);
    }
    res << "}";
    return res.str();
}

Exporting string Atanorprimemapls::JSonString() {
    Locking _lock(this);
    stringstream res;
    prime_hash<BLONG,string>::iterator it;
    res << "{";
    bool beg = true;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        res << '"' << it->first << '"' << ":";
        jstringing(res, it->second);
    }
    res << "}";
    return res.str();
}


Exporting long Atanorprimemapls::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanorprimemapls::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanorprimemapls::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanorprimemapls::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Atanorprimemapls::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Atanor*  Atanorprimemapls::Put(Atanor* idx, Atanor* ke, short idthread) {
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
            values = ((Atanorprimemapls*)ke)->values;
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
            list<Atanor*>::iterator it;
            for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
                values[nb] = (*it)->String();
                nb++;
            }
            return aTRUE;
        }

        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalAtanor->Returnerror("Wrong map initialization", idthread);

        if (ke != this) {
            values.clear();
            if (ke->Type() == Atanorprimemapls::idtype) {
                Atanorprimemapls* kmap = (Atanorprimemapls*)ke;
                //We copy all values from ke to this
                prime_hash<BLONG,string>::iterator it;
                for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                    values[it->first] = it->second;
                kmap->Release();
            }
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

Exporting Atanor* Atanorprimemapls::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Atanor* vect = contextualpattern->Newinstance(idthread);
            prime_hash<BLONG,string>::iterator it;
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
        Atanorprimemapls* kmap = new Atanorprimemapls;
        key = ((AtanorIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Atanor* keyright = ((AtanorIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        BLONG vleft = key->Long();
        BLONG vright = keyright->Long();
        prime_hash<BLONG,string>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        prime_hash<BLONG, string>::iterator itr= values.end();
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

Exporting Atanor* Atanorprimemapls::same(Atanor* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Atanorprimemapls* m = (Atanorprimemapls*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    prime_hash<BLONG,string>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Atanor* Atanorprimemapls::xorset(Atanor* b, bool itself) {

    Doublelocking _lock(this, b);


    Atanorprimemapls* res;
    
    
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapls;
        hmap<BLONG, string> keys;

        prime_hash<BLONG, string>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;
            
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

Exporting Atanor* Atanorprimemapls::orset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,string>::iterator it;
    Atanorprimemapls* res;
    if (itself)
        res = this;
    else
        res = (Atanorprimemapls*)Atom(true);

    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }

    return aNULL;
}

Exporting Atanor* Atanorprimemapls::andset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,string>::iterator it;
    Atanorprimemapls* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapls;
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

Exporting Atanor* Atanorprimemapls::plus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,string>::iterator it;
    Atanorprimemapls* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapls;
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
        res = (Atanorprimemapls*)Atom(true);

    string v = b->String();
    for (it = values.begin(); it != values.end(); it++)
        it->second += v;
    return res;
}

Exporting Atanor* Atanorprimemapls::minus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,string>::iterator it;
    Atanorprimemapls * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapls;
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
        res = (Atanorprimemapls*)Atom(true);

    string v = b->String();
    for (it = values.begin(); it != values.end(); it++)
        it->second = StringMinus(it->second,v);
    return res;
}

Exporting Atanor* Atanorprimemapls::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
    Locking _lock(this);
    Atanor* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    
    prime_hash<BLONG, string>::iterator it;
    
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
