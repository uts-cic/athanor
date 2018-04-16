
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
filename   : atanorprimemapss.cxx
Date       : 2017/04/01
Purpose    : map implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorsvector.h"
#include "atanorstring.h"
#include "atanorprimemapss.h"
#include "atanorlist.h"
#include "atanorivector.h"


//We need to declare once again our local definitions.
Exporting hmap<unsigned short, primemapssMethod>  Atanorprimemapss::methods;
Exporting hmap<string, string> Atanorprimemapss::infomethods;
Exporting bin_hash<unsigned long> Atanorprimemapss::exported;

Exporting short Atanorprimemapss::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorprimemapss::AddMethod(AtanorGlobal* global, string name,primemapssMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanorprimemapss::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Atanorprimemapss::idtype = global->Getid("primemapss");

    Atanorprimemapss::AddMethod(global, "clear", &Atanorprimemapss::MethodClear, P_NONE, "clear(): clear the container.");
    
    Atanorprimemapss::AddMethod(global, "invert", &Atanorprimemapss::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Atanorprimemapss::AddMethod(global, "find", &Atanorprimemapss::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Atanorprimemapss::AddMethod(global, "items", &Atanorprimemapss::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Atanorprimemapss::AddMethod(global, "join", &Atanorprimemapss::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Atanorprimemapss::AddMethod(global, "test", &Atanorprimemapss::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Atanorprimemapss::AddMethod(global, "keys", &Atanorprimemapss::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Atanorprimemapss::AddMethod(global, "values", &Atanorprimemapss::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Atanorprimemapss::AddMethod(global, "pop", &Atanorprimemapss::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Atanorprimemapss::AddMethod(global, "merge", &Atanorprimemapss::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Atanorprimemapss::idtype] = new Atanorprimemapss(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("ssprimemap")] = new Atanorprimemapss(global);

    global->RecordMethods(global->Getid("ssprimemap"), Atanorprimemapss::exported);
    #endif
    global->RecordMethods(Atanorprimemapss::idtype, Atanorprimemapss::exported);

    return true;
}


Exporting AtanorIteration* Atanorprimemapss::Newiteration(bool direction) {
    return new AtanorIterationprimemapss(this, direction);
}


Exporting Atanor* Atanorprimemapss::in(Atanor* context, Atanor* a, short idthread) {
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

Exporting Atanor* Atanorprimemapss::MethodFind(Atanor* context, short idthread, AtanorCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    prime_hash<string, string>::iterator it;
    
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
        Atanorsvector* v = (Atanorsvector*)Selectasvector(context);
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
            return globalAtanor->Providestring(it->first);
    }

    return aNULL;

}



Exporting Atanor* Atanorprimemapss::Push(Atanor* k, Atanor* v) {
    Locking _lock(this);
    string s = k->String();
    values[s] = v->String();
    return aTRUE;
}

Exporting Atanor* Atanorprimemapss::Pop(Atanor* kkey) {
    string k = kkey->String();
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Atanorprimemapss::Clear() {
    Locking _lock(this);
    values.clear();
}

Exporting void Atanorprimemapss::Clean() {
    //Your cleaning code
}

Exporting string Atanorprimemapss::String() {
    Locking _lock(this);
    stringstream res;
    prime_hash<string,string>::iterator it;
    res << "{";
    bool beg = true;
    string sx;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        sx = it->first;
        stringing(res, sx);
        res << ":";
        sx = it->second;
        stringing(res, sx);
    }
    res << "}";
    return res.str();
}

Exporting string Atanorprimemapss::JSonString() {
    Locking _lock(this);
    stringstream res;
    prime_hash<string,string>::iterator it;
    res << "{";
    bool beg = true;
    string sx;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        sx = it->first;
        jstringing(res, sx);
        res << ":";
        sx = it->second;
        jstringing(res, sx);
    }
    res << "}";
    return res.str();
}


Exporting long Atanorprimemapss::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanorprimemapss::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanorprimemapss::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanorprimemapss::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Atanorprimemapss::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Atanor*  Atanorprimemapss::Put(Atanor* idx, Atanor* ke, short idthread) {
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
            values = ((Atanorprimemapss*)ke)->values;
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
            list<Atanor*>::iterator it;
            for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
                sprintf_s(buff, 20, "%d", nb);
                values[buff] = (*it)->String();
                nb++;
            }
            return aTRUE;
        }


        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalAtanor->Returnerror("Wrong map initialization", idthread);

        if (ke != this) {
            values.clear();
            if (ke->Type() == Atanorprimemapss::idtype) {
                Atanorprimemapss* kmap = (Atanorprimemapss*)ke;
                //We copy all values from ke to this
                prime_hash<string,string>::iterator it;
                for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                    values[it->first] = it->second;
                kmap->Release();
            }
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

Exporting Atanor* Atanorprimemapss::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Atanor* vect = contextualpattern->Newinstance(idthread);
            prime_hash<string,string>::iterator it;
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
        Atanorprimemapss* kmap = new Atanorprimemapss;
        key = ((AtanorIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Atanor* keyright = ((AtanorIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        string vleft = key->String();
        string vright = keyright->String();
        prime_hash<string,string>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        prime_hash<string, string>::iterator itr= values.end();
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

Exporting Atanor* Atanorprimemapss::same(Atanor* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Atanorprimemapss* m = (Atanorprimemapss*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    prime_hash<string,string>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Atanor* Atanorprimemapss::xorset(Atanor* b, bool itself) {

    Doublelocking _lock(this, b);


    Atanorprimemapss* res;
    
    
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapss;
        hmap<string, string> keys;

        prime_hash<string, string>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;
            
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

Exporting Atanor* Atanorprimemapss::orset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<string,string>::iterator it;
    Atanorprimemapss* res;
    if (itself)
        res = this;
    else
        res = (Atanorprimemapss*)Atom(true);

    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }

    return aNULL;
}

Exporting Atanor* Atanorprimemapss::andset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<string,string>::iterator it;
    Atanorprimemapss* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapss;
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

Exporting Atanor* Atanorprimemapss::plus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<string,string>::iterator it;
    Atanorprimemapss* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapss;
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
        res = (Atanorprimemapss*)Atom(true);

    string v = b->String();
    for (it = values.begin(); it != values.end(); it++)
        it->second += v;
    return res;
}

Exporting Atanor* Atanorprimemapss::minus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<string,string>::iterator it;
    Atanorprimemapss * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapss;
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
        res = (Atanorprimemapss*)Atom(true);

    string v = b->String();
    for (it = values.begin(); it != values.end(); it++)
        it->second = StringMinus(it->second,v);
    return res;
}

Exporting Atanor* Atanorprimemapss::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
    Locking _lock(this);
    Atanor* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    
    prime_hash<string, string>::iterator it;
    
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
