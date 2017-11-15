
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
filename   : atanormaplf.cxx
Date       : 2017/04/01
Purpose    : map implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"

#include "atanorint.h"
//#prime
#include "atanormaplf.h"
#include "atanorlist.h"
#include "atanorlvector.h"


//We need to declare once again our local definitions.
Exporting hmap<unsigned short, maplfMethod>  Atanormaplf::methods;
Exporting hmap<string, string> Atanormaplf::infomethods;
Exporting bin_hash<unsigned long> Atanormaplf::exported;

Exporting short Atanormaplf::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanormaplf::AddMethod(AtanorGlobal* global, string name, maplfMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanormaplf::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();


    Atanormaplf::idtype = global->Getid("maplf");

    Atanormaplf::AddMethod(global, "clear", &Atanormaplf::MethodClear, P_NONE, "clear(): clear the container.");
    
    Atanormaplf::AddMethod(global, "invert", &Atanormaplf::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Atanormaplf::AddMethod(global, "find", &Atanormaplf::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Atanormaplf::AddMethod(global, "items", &Atanormaplf::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Atanormaplf::AddMethod(global, "join", &Atanormaplf::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Atanormaplf::AddMethod(global, "test", &Atanormaplf::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Atanormaplf::AddMethod(global, "keys", &Atanormaplf::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Atanormaplf::AddMethod(global, "values", &Atanormaplf::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Atanormaplf::AddMethod(global, "sum", &Atanormaplf::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Atanormaplf::AddMethod(global, "product", &Atanormaplf::MethodProduct, P_NONE, "product(): return the product of elements.");
    Atanormaplf::AddMethod(global, "pop", &Atanormaplf::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Atanormaplf::AddMethod(global, "merge", &Atanormaplf::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Atanormaplf::idtype] = new Atanormaplf(global);
    
    global->RecordMethods(Atanormaplf::idtype, Atanormaplf::exported);

    return true;
}


Exporting AtanorIteration* Atanormaplf::Newiteration(bool direction) {
    return new AtanorIterationmaplf(this, direction);
}


Exporting Atanor* Atanormaplf::in(Atanor* context, Atanor* a, short idthread) {
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
        if (values.find(val)!=values.end())
            v->values.push_back(val);

        return v;
    }
    
    Locking _lock(this);
    if (values.find(val)!=values.end())
        return new Atanorlong(val);

    return aNULL;

}

Exporting Atanor* Atanormaplf::MethodFind(Atanor* context, short idthread, AtanorCall* callfunc) {
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



Exporting Atanor* Atanormaplf::Push(Atanor* k, Atanor* v) {
    Locking _lock(this);
    BLONG s = k->Long();
    values[s] = v->Float();
    return aTRUE;
}

Exporting Atanor* Atanormaplf::Pop(Atanor* kkey) {
    BLONG k = kkey->Long();
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Atanormaplf::Clear() {
    Locking _lock(this);
    values.clear();
}

Exporting void Atanormaplf::Clean() {
    //Your cleaning code
}

Exporting string Atanormaplf::String() {
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

Exporting string Atanormaplf::JSonString() {
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


Exporting long Atanormaplf::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanormaplf::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanormaplf::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanormaplf::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Atanormaplf::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Atanor*  Atanormaplf::Put(Atanor* idx, Atanor* ke, short idthread) {
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
            values = ((Atanormaplf*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            values.clear();
            AtanorIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keylong()] = itr->Valuefloat();
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
            if (ke->Type() == Atanormaplf::idtype)
                values = ((Atanormaplf*)ke)->values;
            else {
                AtanorIteration* itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                    values[itr->Keylong()] = itr->Valuefloat();
                itr->Release();
            }
            ke->Release();
        }
        return aTRUE;
    }
    values[idx->Long()] = ke->Float();
    return aTRUE;
}

Exporting Atanor* Atanormaplf::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Atanor* vect = contextualpattern->Newinstance(idthread);
            hmap<BLONG, double>::iterator it;
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
        Atanormaplf* kmap = new Atanormaplf;
        key = ((AtanorIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Atanor* keyright = ((AtanorIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        BLONG vleft = key->Long();
        BLONG vright = keyright->Long();
        hmap<BLONG, double>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        hmap<BLONG, double>::iterator itr = values.end();
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

Exporting Atanor* Atanormaplf::same(Atanor* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Atanormaplf* m = (Atanormaplf*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    hmap<BLONG, double>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Atanor* Atanormaplf::xorset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);
    Atanormaplf* res;
    
    
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanormaplf;
        hmap<BLONG, double> keys;

        for (auto& it : values)
            keys[it.first] = it.second;
            
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keylong();
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
        res = (Atanormaplf*)Atom(true);

    long v = b->Integer();
    for (auto& it : values)
        it.second = (long)it.second ^ v;
    return res;
}

Exporting Atanor* Atanormaplf::orset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanormaplf* res;
    if (itself)
        res = this;
    else
        res = (Atanormaplf*)Atom(true);
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = (long)it.second | v;
    return res;
}

Exporting Atanor* Atanormaplf::andset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanormaplf* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        res = new Atanormaplf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            if (values.find(itr->Keylong()) != values.end() && values[itr->Keylong()] == v)
                res->values[itr->Keylong()] = v;
        }
        itr->Release();
        return res;
    }
    if (itself)
        res = this;
    else
        res = (Atanormaplf*)Atom(true);
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = (long)it.second & v;
    return res;
}

Exporting Atanor* Atanormaplf::plus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanormaplf* res;
    if (b->isMapContainer()) {
        hmap<BLONG, double>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanormaplf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
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
        res = (Atanormaplf*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second += v;
    return res;
}

Exporting Atanor* Atanormaplf::minus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanormaplf * res;
    if (b->isMapContainer()) {
        hmap<BLONG, double>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanormaplf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keylong());
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
        res = (Atanormaplf*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second -= v;
    return res;
}

Exporting Atanor* Atanormaplf::multiply(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanormaplf * res;
    if (b->isMapContainer()) {
        hmap<BLONG, double>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanormaplf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keylong());
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
        res = (Atanormaplf*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second *= v;
    return res;

}

Exporting Atanor* Atanormaplf::divide(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanormaplf * res;
    if (b->isMapContainer()) {
        hmap<BLONG, double>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanormaplf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            if (v == 0) {
                res->Release();
                return new AtanorError("Error: Divided by 0");
            }

            it = values.find(itr->Keylong());
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
        res = (Atanormaplf*)Atom(true);

    double v = b->Float();
    if (v == 0) {
        res->Release();
        return new AtanorError("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second /= v;
    return res;

}

Exporting Atanor* Atanormaplf::mod(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanormaplf * res;
    if (b->isMapContainer()) {
        hmap<BLONG, double>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanormaplf;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            if (v == 0) {
                res->Release();
                return new AtanorError("Error: Divided by 0");
            }

            it = values.find(itr->Keylong());
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
        res = (Atanormaplf*)Atom(true);

    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return new AtanorError("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second = (long)it.second % v;
    return res;
}

Exporting Atanor* Atanormaplf::shiftright(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanormaplf * res;
    if (b->isMapContainer()) {
        hmap<BLONG, double>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanormaplf;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keylong());
            if (it != values.end()) {
                res->values[it->first] = (long)it->second >> v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanormaplf*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second = (long)it.second >> v;
    return res;
}

Exporting Atanor* Atanormaplf::shiftleft(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanormaplf * res;
    if (b->isMapContainer()) {
        hmap<BLONG, double>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanormaplf;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keylong());
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
        res = (Atanormaplf*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second = (long)it.second << v;
    return res;
}

Exporting Atanor* Atanormaplf::power(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanormaplf * res;
    if (b->isMapContainer()) {
        hmap<BLONG, double>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanormaplf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keylong());
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
        res = (Atanormaplf*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second = pow(it.second, v);
    return res;
}

Exporting Atanor* Atanormaplf::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
    Locking _lock(this);
    Atanor* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    
    hmap<BLONG, double>::iterator it;
    
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
