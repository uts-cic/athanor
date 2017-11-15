
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
filename   : atanorprimemapfs.cxx
Date       : 2017/04/01
Purpose    : map implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorfvector.h"
#include "atanorfloat.h"
//#prime
#include "atanorprimemapfs.h"
#include "atanorlist.h"
#include "atanorivector.h"


//We need to declare once again our local definitions.
Exporting hmap<unsigned short, primemapfsMethod>  Atanorprimemapfs::methods;
Exporting hmap<string, string> Atanorprimemapfs::infomethods;
Exporting bin_hash<unsigned long> Atanorprimemapfs::exported;

Exporting short Atanorprimemapfs::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorprimemapfs::AddMethod(AtanorGlobal* global, string name,primemapfsMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanorprimemapfs::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Atanorprimemapfs::idtype = global->Getid("primemapfs");

    Atanorprimemapfs::AddMethod(global, "clear", &Atanorprimemapfs::MethodClear, P_NONE, "clear(): clear the container.");
    
    Atanorprimemapfs::AddMethod(global, "invert", &Atanorprimemapfs::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Atanorprimemapfs::AddMethod(global, "find", &Atanorprimemapfs::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Atanorprimemapfs::AddMethod(global, "items", &Atanorprimemapfs::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Atanorprimemapfs::AddMethod(global, "join", &Atanorprimemapfs::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Atanorprimemapfs::AddMethod(global, "test", &Atanorprimemapfs::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Atanorprimemapfs::AddMethod(global, "keys", &Atanorprimemapfs::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Atanorprimemapfs::AddMethod(global, "values", &Atanorprimemapfs::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Atanorprimemapfs::AddMethod(global, "pop", &Atanorprimemapfs::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Atanorprimemapfs::AddMethod(global, "merge", &Atanorprimemapfs::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Atanorprimemapfs::idtype] = new Atanorprimemapfs(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("fsprimemap")] = new Atanorprimemapfs(global);

    global->RecordMethods(global->Getid("fsprimemap"), Atanorprimemapfs::exported);
    #endif
    global->RecordMethods(Atanorprimemapfs::idtype, Atanorprimemapfs::exported);

    return true;
}


Exporting AtanorIteration* Atanorprimemapfs::Newiteration(bool direction) {
    return new AtanorIterationprimemapfs(this, direction);
}


Exporting Atanor* Atanorprimemapfs::in(Atanor* context, Atanor* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    double val = a->Float();

    if (context->isBoolean()) {
        Locking _lock(this);
        if (values.find(val)!=values.end())
            return aTRUE;
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Atanorfvector* v = (Atanorfvector*)Selectafvector(context);
        Doublelocking _lock(this, v);
        if (values.find(val)!=values.end())
            v->values.push_back(val);

        return v;
    }
    
    Locking _lock(this);
    if (values.find(val)!=values.end())
        return globalAtanor->Providefloat(val);

    return aNULL;

}

Exporting Atanor* Atanorprimemapfs::MethodFind(Atanor* context, short idthread, AtanorCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    prime_hash<double, string>::iterator it;
    
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
        Atanorfvector* v = (Atanorfvector*)Selectafvector(context);
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
            return globalAtanor->Providefloat(it->first);
    }

    return aNULL;

}



Exporting Atanor* Atanorprimemapfs::Push(Atanor* k, Atanor* v) {
    Locking _lock(this);
    double s = k->Float();
    values[s] = v->String();
    return aTRUE;
}

Exporting Atanor* Atanorprimemapfs::Pop(Atanor* kkey) {
    double k = kkey->Float();
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Atanorprimemapfs::Clear() {
    Locking _lock(this);
    values.clear();
}

Exporting void Atanorprimemapfs::Clean() {
    //Your cleaning code
}

Exporting string Atanorprimemapfs::String() {
    Locking _lock(this);
    stringstream res;
    prime_hash<double,string>::iterator it;
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

Exporting string Atanorprimemapfs::JSonString() {
    Locking _lock(this);
    stringstream res;
    prime_hash<double,string>::iterator it;
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


Exporting long Atanorprimemapfs::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanorprimemapfs::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanorprimemapfs::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanorprimemapfs::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Atanorprimemapfs::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Atanor*  Atanorprimemapfs::Put(Atanor* idx, Atanor* ke, short idthread) {
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
            values = ((Atanorprimemapfs*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            values.clear();
            AtanorIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keyfloat()] = itr->Valuestring();
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
            if (ke->Type() == Atanorprimemapfs::idtype) {
                Atanorprimemapfs* kmap = (Atanorprimemapfs*)ke;
                //We copy all values from ke to this
                prime_hash<double,string>::iterator it;
                for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                    values[it->first] = it->second;
                kmap->Release();
            }
            else {
                AtanorIteration* itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                    values[itr->Keyfloat()] = itr->Valuestring();
                itr->Release();
            }
            ke->Release();
        }
        return aTRUE;
    }

    values[idx->Float()] = ke->String();
    return aTRUE;
}

Exporting Atanor* Atanorprimemapfs::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Atanor* vect = contextualpattern->Newinstance(idthread);
            prime_hash<double,string>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalAtanor->Providefloat(it->first));
            return vect;
        }

        if (contextualpattern->Type() == a_int || contextualpattern->Type() == a_float)
            return globalAtanor->Provideint(values.size());

        return this;
    }

    Atanor* key;
    if (idx->isInterval()) {
        Atanorprimemapfs* kmap = new Atanorprimemapfs;
        key = ((AtanorIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Atanor* keyright = ((AtanorIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        double vleft = key->Float();
        double vright = keyright->Float();
        prime_hash<double,string>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        prime_hash<double, string>::iterator itr= values.end();
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

    double skey = key->Float();
    key->Release();

    Atanor* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalAtanor->erroronkey)
            return globalAtanor->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }
    return kval;
}

Exporting Atanor* Atanorprimemapfs::same(Atanor* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Atanorprimemapfs* m = (Atanorprimemapfs*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    prime_hash<double,string>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Atanor* Atanorprimemapfs::xorset(Atanor* b, bool itself) {

    Doublelocking _lock(this, b);


    Atanorprimemapfs* res;
    
    
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapfs;
        hmap<double, string> keys;

        prime_hash<double, string>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;
            
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyfloat();
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

Exporting Atanor* Atanorprimemapfs::orset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<double,string>::iterator it;
    Atanorprimemapfs* res;
    if (itself)
        res = this;
    else
        res = (Atanorprimemapfs*)Atom(true);

    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }

    return aNULL;
}

Exporting Atanor* Atanorprimemapfs::andset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<double,string>::iterator it;
    Atanorprimemapfs* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapfs;
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            if (values.find(itr->Keyfloat()) != values.end() && values[itr->Keyfloat()]==v)
                res->values[itr->Keyfloat()] = v;
        }
        itr->Release();
        return res;
    }

    return aNULL;
}

Exporting Atanor* Atanorprimemapfs::plus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<double,string>::iterator it;
    Atanorprimemapfs* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapfs;
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            it = values.find(itr->Keyfloat());
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
        res = (Atanorprimemapfs*)Atom(true);

    string v = b->String();
    for (it = values.begin(); it != values.end(); it++)
        it->second += v;
    return res;
}

Exporting Atanor* Atanorprimemapfs::minus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<double,string>::iterator it;
    Atanorprimemapfs * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapfs;
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            it = values.find(itr->Keyfloat());
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
        res = (Atanorprimemapfs*)Atom(true);

    string v = b->String();
    for (it = values.begin(); it != values.end(); it++)
        it->second = StringMinus(it->second,v);
    return res;
}

Exporting Atanor* Atanorprimemapfs::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
    Locking _lock(this);
    Atanor* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    
    prime_hash<double, string>::iterator it;
    
    Atanor* a;
    vector<double> keys;

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
