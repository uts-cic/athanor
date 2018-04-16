
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
filename   : atanortreemapfl.cxx
Date       : 2017/04/01
Purpose    : map implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorfvector.h"
#include "atanorfloat.h"
//#prime
#include "atanortreemapfl.h"
#include "atanorlist.h"
#include "atanorlvector.h"


//We need to declare once again our local definitions.
Exporting hmap<unsigned short, treemapflMethod>  Atanortreemapfl::methods;
Exporting hmap<string, string> Atanortreemapfl::infomethods;
Exporting bin_hash<unsigned long> Atanortreemapfl::exported;

Exporting short Atanortreemapfl::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanortreemapfl::AddMethod(AtanorGlobal* global, string name,treemapflMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanortreemapfl::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Atanortreemapfl::idtype = global->Getid("treemapfl");

    Atanortreemapfl::AddMethod(global, "clear", &Atanortreemapfl::MethodClear, P_NONE, "clear(): clear the container.");
    
    Atanortreemapfl::AddMethod(global, "invert", &Atanortreemapfl::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Atanortreemapfl::AddMethod(global, "find", &Atanortreemapfl::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Atanortreemapfl::AddMethod(global, "items", &Atanortreemapfl::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Atanortreemapfl::AddMethod(global, "join", &Atanortreemapfl::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Atanortreemapfl::AddMethod(global, "test", &Atanortreemapfl::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Atanortreemapfl::AddMethod(global, "keys", &Atanortreemapfl::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Atanortreemapfl::AddMethod(global, "values", &Atanortreemapfl::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Atanortreemapfl::AddMethod(global, "sum", &Atanortreemapfl::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Atanortreemapfl::AddMethod(global, "product", &Atanortreemapfl::MethodProduct, P_NONE, "product(): return the product of elements.");
    Atanortreemapfl::AddMethod(global, "pop", &Atanortreemapfl::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Atanortreemapfl::AddMethod(global, "merge", &Atanortreemapfl::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Atanortreemapfl::idtype] = new Atanortreemapfl(global);
    
    global->RecordMethods(Atanortreemapfl::idtype, Atanortreemapfl::exported);

    return true;
}


Exporting AtanorIteration* Atanortreemapfl::Newiteration(bool direction) {
    return new AtanorIterationtreemapfl(this, direction);
}


Exporting Atanor* Atanortreemapfl::in(Atanor* context, Atanor* a, short idthread) {
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

Exporting Atanor* Atanortreemapfl::MethodFind(Atanor* context, short idthread, AtanorCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    
    BLONG val = callfunc->Evaluate(0, context, idthread)->Long();
    
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



Exporting Atanor* Atanortreemapfl::Push(Atanor* k, Atanor* v) {
    Locking _lock(this);
    double s = k->Float();
    values[s] = v->Long();
    return aTRUE;
}

Exporting Atanor* Atanortreemapfl::Pop(Atanor* kkey) {
    double k = kkey->Float();
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Atanortreemapfl::Clear() {
    Locking _lock(this);
    values.clear();
}

Exporting void Atanortreemapfl::Clean() {
    //Your cleaning code
}

Exporting string Atanortreemapfl::String() {
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

Exporting string Atanortreemapfl::JSonString() {
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


Exporting long Atanortreemapfl::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanortreemapfl::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanortreemapfl::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanortreemapfl::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Atanortreemapfl::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Atanor*  Atanortreemapfl::Put(Atanor* idx, Atanor* ke, short idthread) {
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
            values = ((Atanortreemapfl*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            values.clear();
            AtanorIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keyfloat()] = itr->Valuelong();
            itr->Release();
            return aTRUE;
        }
        if (ke->isVectorContainer()) {
            values.clear();
            nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                values[nb] = ke->getinteger(it);
                nb++;
            }
            return aTRUE;
        }
        if (ke->Type() == a_list) {
            Atanorlist* kvect = (Atanorlist*)ke;
            values.clear();
            nb = 0;

            for (auto& it : kvect->values) {
                values[nb] = it->Long();
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalAtanor->Returnerror("Wrong map initialization", idthread);
        if (ke != this) {
            values.clear();
            if (ke->Type() == Atanortreemapfl::idtype)
                values = ((Atanortreemapfl*)ke)->values;
            else {
                AtanorIteration* itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                    values[itr->Keyfloat()] = itr->Valuelong();
                itr->Release();
            }
            ke->Release();
        }
        return aTRUE;
    }
    values[idx->Float()] = ke->Long();
    return aTRUE;
}

Exporting Atanor* Atanortreemapfl::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Atanor* vect = contextualpattern->Newinstance(idthread);
            map<double,BLONG>::iterator it;
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
        Atanortreemapfl* kmap = new Atanortreemapfl;
        key = ((AtanorIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Atanor* keyright = ((AtanorIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        double vleft = key->Float();
        double vright = keyright->Float();
        map<double,BLONG>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        map<double, BLONG>::iterator itr= values.end();
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

Exporting Atanor* Atanortreemapfl::same(Atanor* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Atanortreemapfl* m = (Atanortreemapfl*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    map<double,BLONG>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Atanor* Atanortreemapfl::xorset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Atanortreemapfl* res;
    
    
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapfl;
        hmap<double, BLONG> keys;

        for (auto& it : values)
            keys[it.first] = it.second;
            
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyfloat();
            if (keys.find(v) == keys.end())
                keys[v]=itr->Valuelong();
            else {
                if (values[v]==itr->Valuelong())
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
        res = (Atanortreemapfl*)Atom(true);

    long v = b->Integer();
    for (auto& it : values)
        it.second = (long)it.second ^ v;
    return res;
}

Exporting Atanor* Atanortreemapfl::orset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapfl* res;
    if (itself)
        res = this;
    else
        res = (Atanortreemapfl*)Atom(true);
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = (long)it.second | v;
    return res;
}

Exporting Atanor* Atanortreemapfl::andset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapfl* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        res = new Atanortreemapfl;
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
            if (values.find(itr->Keyfloat()) != values.end() && values[itr->Keyfloat()]==v)
                res->values[itr->Keyfloat()] = v;
        }
        itr->Release();
        return res;
    }
    if (itself)
        res = this;
    else
        res = (Atanortreemapfl*)Atom(true);
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = (long)it.second & v;
    return res;
}

Exporting Atanor* Atanortreemapfl::plus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapfl* res;
    if (b->isMapContainer()) {
        map<double,BLONG>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapfl;
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
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
        res = (Atanortreemapfl*)Atom(true);

    BLONG v = b->Long();
    for (auto& it : res->values)
        it.second += v;
    return res;
}

Exporting Atanor* Atanortreemapfl::minus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapfl * res;
    if (b->isMapContainer()) {
        map<double,BLONG>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapfl;
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
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
        res = (Atanortreemapfl*)Atom(true);

    BLONG v = b->Long();
    for (auto& it : res->values)
        it.second -= v;
    return res;
}

Exporting Atanor* Atanortreemapfl::multiply(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapfl * res;
    if (b->isMapContainer()) {
        map<double,BLONG>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapfl;
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
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
        res = (Atanortreemapfl*)Atom(true);

    BLONG v = b->Long();
    for (auto& it : res->values)
        it.second *= v;
    return res;

}

Exporting Atanor* Atanortreemapfl::divide(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapfl * res;
    if (b->isMapContainer()) {
        map<double,BLONG>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapfl;
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
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
        res = (Atanortreemapfl*)Atom(true);

    BLONG v = b->Long();
    if (v == 0) {
        res->Release();
        return new AtanorError("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second /= v;
    return res;

}

Exporting Atanor* Atanortreemapfl::mod(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapfl * res;
    if (b->isMapContainer()) {
        map<double,BLONG>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapfl;
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
        res = (Atanortreemapfl*)Atom(true);

    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return new AtanorError("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second = (long)it.second % v;
    return res;
}

Exporting Atanor* Atanortreemapfl::shiftright(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapfl * res;
    if (b->isMapContainer()) {
        map<double,BLONG>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapfl;
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
        res = (Atanortreemapfl*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second = (long)it.second >> v;
    return res;
}

Exporting Atanor* Atanortreemapfl::shiftleft(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapfl * res;
    if (b->isMapContainer()) {
        map<double,BLONG>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapfl;
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
        res = (Atanortreemapfl*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second = (long)it.second << v;
    return res;
}

Exporting Atanor* Atanortreemapfl::power(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapfl * res;
    if (b->isMapContainer()) {
        map<double,BLONG>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapfl;
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
        res = (Atanortreemapfl*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second = pow(it.second, v);
    return res;
}

Exporting Atanor* Atanortreemapfl::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
    Locking _lock(this);
    Atanor* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    
    map<double, BLONG>::iterator it;
    
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
