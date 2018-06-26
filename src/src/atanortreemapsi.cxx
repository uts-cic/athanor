
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
filename   : atanortreemapsi.cxx
Date       : 2017/04/01
Purpose    : map implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorsvector.h"
#include "atanorstring.h"
//#prime
#include "atanortreemapsi.h"
#include "atanorlist.h"
#include "atanorivector.h"


//We need to declare once again our local definitions.
Exporting hmap<unsigned short, treemapsiMethod>  Atanortreemapsi::methods;
Exporting hmap<string, string> Atanortreemapsi::infomethods;
Exporting bin_hash<unsigned long> Atanortreemapsi::exported;

Exporting short Atanortreemapsi::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanortreemapsi::AddMethod(AtanorGlobal* global, string name,treemapsiMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanortreemapsi::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Atanortreemapsi::idtype = global->Getid("treemapsi");

    Atanortreemapsi::AddMethod(global, "clear", &Atanortreemapsi::MethodClear, P_NONE, "clear(): clear the container.");
    
    Atanortreemapsi::AddMethod(global, "invert", &Atanortreemapsi::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Atanortreemapsi::AddMethod(global, "find", &Atanortreemapsi::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Atanortreemapsi::AddMethod(global, "items", &Atanortreemapsi::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Atanortreemapsi::AddMethod(global, "join", &Atanortreemapsi::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Atanortreemapsi::AddMethod(global, "test", &Atanortreemapsi::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Atanortreemapsi::AddMethod(global, "keys", &Atanortreemapsi::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Atanortreemapsi::AddMethod(global, "values", &Atanortreemapsi::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Atanortreemapsi::AddMethod(global, "sum", &Atanortreemapsi::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Atanortreemapsi::AddMethod(global, "product", &Atanortreemapsi::MethodProduct, P_NONE, "product(): return the product of elements.");
    Atanortreemapsi::AddMethod(global, "pop", &Atanortreemapsi::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Atanortreemapsi::AddMethod(global, "merge", &Atanortreemapsi::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Atanortreemapsi::idtype] = new Atanortreemapsi(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("sitreemap")] = new Atanortreemapsi(global);

    global->RecordMethods(global->Getid("sitreemap"), Atanortreemapsi::exported);
    #endif
    global->RecordMethods(Atanortreemapsi::idtype, Atanortreemapsi::exported);

    return true;
}


Exporting AtanorIteration* Atanortreemapsi::Newiteration(bool direction) {
    return new AtanorIterationtreemapsi(this, direction);
}


Exporting Atanor* Atanortreemapsi::in(Atanor* context, Atanor* a, short idthread) {
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

Exporting Atanor* Atanortreemapsi::MethodFind(Atanor* context, short idthread, AtanorCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    
    long val = callfunc->Evaluate(0, context, idthread)->Integer();
    
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



Exporting Atanor* Atanortreemapsi::Push(Atanor* k, Atanor* v) {
    Locking _lock(this);
    string s = k->String();
    values[s] = v->Integer();
    return aTRUE;
}

Exporting Atanor* Atanortreemapsi::Pop(Atanor* kkey) {
    string k = kkey->String();
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Atanortreemapsi::Clear() {
    Locking _lock(this);
    values.clear();
}

Exporting void Atanortreemapsi::Clean() {
    //Your cleaning code
}

Exporting string Atanortreemapsi::String() {
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
        res << ":" << it.second;
    }
    res << "}";
    return res.str();
}

Exporting string Atanortreemapsi::JSonString() {
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
        res << ":" << it.second;
    }
    res << "}";
    return res.str();
}


Exporting long Atanortreemapsi::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanortreemapsi::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanortreemapsi::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanortreemapsi::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Atanortreemapsi::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Atanor*  Atanortreemapsi::Put(Atanor* idx, Atanor* ke, short idthread) {
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
            values = ((Atanortreemapsi*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            values.clear();
            AtanorIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keystring()] = itr->Valueinteger();
            itr->Release();
            return aTRUE;
        }
        char buff[20];
        if (ke->isVectorContainer()) {
            values.clear();
            nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                sprintf_s(buff, 20, "%d", nb);
                values[buff] = ke->getinteger(it);
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
                values[buff] = it->Integer();
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalAtanor->Returnerror("Wrong map initialization", idthread);
        if (ke != this) {
            values.clear();
            if (ke->Type() == Atanortreemapsi::idtype)
                values = ((Atanortreemapsi*)ke)->values;
            else {
                AtanorIteration* itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                    values[itr->Keystring()] = itr->Valueinteger();
                itr->Release();
            }
            ke->Release();
        }
        return aTRUE;
    }
    values[idx->String()] = ke->Integer();
    return aTRUE;
}

Exporting Atanor* Atanortreemapsi::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Atanor* vect = contextualpattern->Newinstance(idthread);
            map<string,long>::iterator it;
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
        Atanortreemapsi* kmap = new Atanortreemapsi;
        key = ((AtanorIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Atanor* keyright = ((AtanorIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        string vleft = key->String();
        string vright = keyright->String();
        map<string,long>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        map<string, long>::iterator itr= values.end();
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

Exporting Atanor* Atanortreemapsi::same(Atanor* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Atanortreemapsi* m = (Atanortreemapsi*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    map<string,long>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Atanor* Atanortreemapsi::xorset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Atanortreemapsi* res;
    
    
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapsi;
        hmap<string, long> keys;

        for (auto& it : values)
            keys[it.first] = it.second;
            
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keystring();
            if (keys.find(v) == keys.end())
                keys[v]=itr->Valueinteger();
            else {
                if (values[v]==itr->Valueinteger())
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
        res = (Atanortreemapsi*)Atom(true);

    long v = b->Integer();
    for (auto& it : values)
        it.second = (long)it.second ^ v;
    return res;
}

Exporting Atanor* Atanortreemapsi::orset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapsi* res;
    if (itself)
        res = this;
    else
        res = (Atanortreemapsi*)Atom(true);
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = (long)it.second | v;
    return res;
}

Exporting Atanor* Atanortreemapsi::andset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapsi* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        res = new Atanortreemapsi;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            if (values.find(itr->Keystring()) != values.end() && values[itr->Keystring()]==v)
                res->values[itr->Keystring()] = v;
        }
        itr->Release();
        return res;
    }
    if (itself)
        res = this;
    else
        res = (Atanortreemapsi*)Atom(true);
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = (long)it.second & v;
    return res;
}

Exporting Atanor* Atanortreemapsi::plus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapsi* res;
    if (b->isMapContainer()) {
        map<string,long>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapsi;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
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
        res = (Atanortreemapsi*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second += v;
    return res;
}

Exporting Atanor* Atanortreemapsi::minus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapsi * res;
    if (b->isMapContainer()) {
        map<string,long>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapsi;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keystring());
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
        res = (Atanortreemapsi*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second -= v;
    return res;
}

Exporting Atanor* Atanortreemapsi::multiply(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapsi * res;
    if (b->isMapContainer()) {
        map<string,long>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapsi;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keystring());
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
        res = (Atanortreemapsi*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second *= v;
    return res;

}

Exporting Atanor* Atanortreemapsi::divide(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapsi * res;
    if (b->isMapContainer()) {
        map<string,long>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapsi;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            if (v == 0) {
                res->Release();
                return new AtanorError("Error: Divided by 0");
            }

            it = values.find(itr->Keystring());
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
        res = (Atanortreemapsi*)Atom(true);

    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return new AtanorError("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second /= v;
    return res;

}

Exporting Atanor* Atanortreemapsi::mod(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapsi * res;
    if (b->isMapContainer()) {
        map<string,long>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapsi;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            if (v == 0) {
                res->Release();
                return new AtanorError("Error: Divided by 0");
            }

            it = values.find(itr->Keystring());
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
        res = (Atanortreemapsi*)Atom(true);

    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return new AtanorError("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second = (long)it.second % v;
    return res;
}

Exporting Atanor* Atanortreemapsi::shiftright(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapsi * res;
    if (b->isMapContainer()) {
        map<string,long>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapsi;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keystring());
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
        res = (Atanortreemapsi*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second = (long)it.second >> v;
    return res;
}

Exporting Atanor* Atanortreemapsi::shiftleft(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapsi * res;
    if (b->isMapContainer()) {
        map<string,long>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapsi;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keystring());
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
        res = (Atanortreemapsi*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second = (long)it.second << v;
    return res;
}

Exporting Atanor* Atanortreemapsi::power(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanortreemapsi * res;
    if (b->isMapContainer()) {
        map<string,long>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanortreemapsi;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keystring());
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
        res = (Atanortreemapsi*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second = pow(it.second, v);
    return res;
}

Exporting Atanor* Atanortreemapsi::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
    Locking _lock(this);
    Atanor* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    
    map<string, long>::iterator it;
    
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
