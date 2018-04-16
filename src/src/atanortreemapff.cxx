
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
filename   : atanortreemapff.cxx
Date       : 2017/04/01
Purpose    : map implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorfvector.h"
#include "atanorfloat.h"
//#prime
#include "atanortreemapff.h"
#include "atanorlist.h"
#include "atanorivector.h"


//We need to declare once again our local definitions.
Exporting hmap<unsigned short, treemapffMethod>  Atanortreemapff::methods;
Exporting hmap<string, string> Atanortreemapff::infomethods;
Exporting bin_hash<unsigned long> Atanortreemapff::exported;

Exporting short Atanortreemapff::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanortreemapff::AddMethod(AtanorGlobal* global, string name,treemapffMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanortreemapff::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Atanortreemapff::idtype = global->Getid("treemapff");

    Atanortreemapff::AddMethod(global, "clear", &Atanortreemapff::MethodClear, P_NONE, "clear(): clear the container.");
    
    Atanortreemapff::AddMethod(global, "invert", &Atanortreemapff::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Atanortreemapff::AddMethod(global, "find", &Atanortreemapff::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Atanortreemapff::AddMethod(global, "items", &Atanortreemapff::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Atanortreemapff::AddMethod(global, "join", &Atanortreemapff::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Atanortreemapff::AddMethod(global, "test", &Atanortreemapff::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Atanortreemapff::AddMethod(global, "keys", &Atanortreemapff::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Atanortreemapff::AddMethod(global, "values", &Atanortreemapff::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Atanortreemapff::AddMethod(global, "sum", &Atanortreemapff::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Atanortreemapff::AddMethod(global, "product", &Atanortreemapff::MethodProduct, P_NONE, "product(): return the product of elements.");
    Atanortreemapff::AddMethod(global, "pop", &Atanortreemapff::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Atanortreemapff::AddMethod(global, "merge", &Atanortreemapff::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Atanortreemapff::idtype] = new Atanortreemapff(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("fftreemap")] = new Atanortreemapff(global);

    global->RecordMethods(global->Getid("fftreemap"), Atanortreemapff::exported);
    #endif
    global->RecordMethods(Atanortreemapff::idtype, Atanortreemapff::exported);

    return true;
}


Exporting AtanorIteration* Atanortreemapff::Newiteration(bool direction) {
    return new AtanorIterationtreemapff(this, direction);
}


Exporting Atanor* Atanortreemapff::in(Atanor* context, Atanor* a, short idthread) {
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

Exporting Atanor* Atanortreemapff::MethodFind(Atanor* context, short idthread, AtanorCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    
    double val = callfunc->Evaluate(0, context, idthread)->Float();
    
    if (context->isBoolean()) {
        Locking _lock(this);
        for (auto& it : values) {
            if (it.second == val)
                return aTRUE;
        }
        return aFALSE;
    }
    if (context->isVectorContainer()) {
        Atanorfvector* v = (Atanorfvector*)Selectafvector(context);
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
            return globalAtanor->Providefloat(it.first);
    }
    return aNULL;
}



Exporting Atanor* Atanortreemapff::Push(Atanor* k, Atanor* v) {
    Locking _lock(this);
    double s = k->Float();
    values[s] = v->Float();
    return aTRUE;
}

Exporting Atanor* Atanortreemapff::Pop(Atanor* kkey) {
    double k = kkey->Float();
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Atanortreemapff::Clear() {
    Locking _lock(this);
    values.clear();
}

Exporting void Atanortreemapff::Clean() {
    //Your cleaning code
}

Exporting string Atanortreemapff::String() {
    Locking _lock(this);
    stringstream res;

    res << "{";
    bool beg = true;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it.first << ":" << it.second;
    }
    res << "}";
    return res.str();
}

Exporting string Atanortreemapff::JSonString() {
    Locking _lock(this);
    stringstream res;

    res << "{";
    bool beg = true;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        res << '"' << it.first << '"' << ":" << it.second;
    }
    res << "}";
    return res.str();
}


Exporting long Atanortreemapff::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanortreemapff::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanortreemapff::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanortreemapff::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Atanortreemapff::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Atanor*  Atanortreemapff::Put(Atanor* idx, Atanor* ke, short idthread) {
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
            values = ((Atanortreemapff*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            values.clear();
            AtanorIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keyfloat()] = itr->Valuefloat();
            itr->Release();
            return aTRUE;
        }
        if (ke->isVectorContainer()) {
            values.clear();
            nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                values[nb] = ke->getfloat(it);
                nb++;
            }
            return aTRUE;
        }
        if (ke->Type() == a_list) {
            Atanorlist* kvect = (Atanorlist*)ke;
            values.clear();
            nb = 0;

            for (auto& it : kvect->values) {
                values[nb] = it->Float();
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalAtanor->Returnerror("Wrong map initialization", idthread);
        if (ke != this) {
            values.clear();
            if (ke->Type() == Atanortreemapff::idtype)
                values = ((Atanortreemapff*)ke)->values;
            else {
                AtanorIteration* itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                    values[itr->Keyfloat()] = itr->Valuefloat();
                itr->Release();
            }
            ke->Release();
        }
        return aTRUE;
    }
    values[idx->Float()] = ke->Float();
    return aTRUE;
}

Exporting Atanor* Atanortreemapff::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Atanor* vect = contextualpattern->Newinstance(idthread);
            map<double,double>::iterator it;
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
        Atanortreemapff* kmap = new Atanortreemapff;
        key = ((AtanorIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Atanor* keyright = ((AtanorIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        double vleft = key->Float();
        double vright = keyright->Float();
        map<double,double>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        map<double, double>::iterator itr= values.end();
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

Exporting Atanor* Atanortreemapff::same(Atanor* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Atanortreemapff* m = (Atanortreemapff*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    map<double,double>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Atanor* Atanortreemapff::xorset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Atanortreemapff* res;
    
    
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapff;
        hmap<double, double> keys;

        for (auto& it : values)
            keys[it.first] = it.second;
            
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyfloat();
            if (keys.find(v) == keys.end())
                keys[v]=itr->Valuefloat();
            else {
                if (values[v]==itr->Valuefloat())
                    keys.erase(v);
            }
        }
        itr->Release();

        for (auto& a : keys)
            res->values[a.first]= a.second;

        return res;


    }
    if (itself)
        res = this;
    else
        res = (Atanortreemapff*)Atom(true);

    long v = b->Integer();
    for (auto& it : values)
        it.second = (long)it.second ^ v;
    return res;
}

Exporting Atanor* Atanortreemapff::orset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapff* res;
    if (itself)
        res = this;
    else
        res = (Atanortreemapff*)Atom(true);
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = (long)it.second | v;
    return res;
}

Exporting Atanor* Atanortreemapff::andset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapff* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        res = new Atanortreemapff;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            if (values.find(itr->Keyfloat()) != values.end() && values[itr->Keyfloat()]==v)
                res->values[itr->Keyfloat()] = v;
        }
        itr->Release();
        return res;
    }
    if (itself)
        res = this;
    else
        res = (Atanortreemapff*)Atom(true);
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = (long)it.second & v;
    return res;
}

Exporting Atanor* Atanortreemapff::plus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapff* res;
    if (b->isMapContainer()) {
        map<double,double>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapff;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
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
        res = (Atanortreemapff*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second += v;
    return res;
}

Exporting Atanor* Atanortreemapff::minus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapff * res;
    if (b->isMapContainer()) {
        map<double,double>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapff;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keyfloat());
            if (it != values.end()) {
                res->values[it->first] = it->second - v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanortreemapff*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second -= v;
    return res;
}

Exporting Atanor* Atanortreemapff::multiply(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapff * res;
    if (b->isMapContainer()) {
        map<double,double>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapff;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keyfloat());
            if (it != values.end()) {
                res->values[it->first] = it->second * v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanortreemapff*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second *= v;
    return res;

}

Exporting Atanor* Atanortreemapff::divide(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapff * res;
    if (b->isMapContainer()) {
        map<double,double>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapff;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            if (v == 0) {
                res->Release();
                return new AtanorError("Error: Divided by 0");
            }

            it = values.find(itr->Keyfloat());
            if (it != values.end()) {
                res->values[it->first] = it->second / v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanortreemapff*)Atom(true);

    double v = b->Float();
    if (v == 0) {
        res->Release();
        return new AtanorError("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second /= v;
    return res;

}

Exporting Atanor* Atanortreemapff::mod(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapff * res;
    if (b->isMapContainer()) {
        map<double,double>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapff;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            if (v == 0) {
                res->Release();
                return new AtanorError("Error: Divided by 0");
            }

            it = values.find(itr->Keyfloat());
            if (it != values.end()) {
                res->values[it->first] = (long)it->second % v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanortreemapff*)Atom(true);

    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return new AtanorError("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second = (long)it.second % v;
    return res;
}

Exporting Atanor* Atanortreemapff::shiftright(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapff * res;
    if (b->isMapContainer()) {
        map<double,double>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapff;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keyfloat());
            if (it != values.end()) {
                res->values[it->first] = (long)it->second>> v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanortreemapff*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second = (long)it.second >> v;
    return res;
}

Exporting Atanor* Atanortreemapff::shiftleft(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapff * res;
    if (b->isMapContainer()) {
        map<double,double>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapff;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keyfloat());
            if (it != values.end()) {
                res->values[it->first] = (long)it->second << v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanortreemapff*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second = (long)it.second << v;
    return res;
}

Exporting Atanor* Atanortreemapff::power(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapff * res;
    if (b->isMapContainer()) {
        map<double,double>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapff;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keyfloat());
            if (it != values.end()) {
                res->values[it->first] = pow(it->second, v);
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanortreemapff*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second = pow(it.second, v);
    return res;
}

Exporting Atanor* Atanortreemapff::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
    Locking _lock(this);
    Atanor* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    
    map<double, double>::iterator it;
    
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
