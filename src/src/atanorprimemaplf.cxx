
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
filename   : atanorprimemaplf.cxx
Date       : 2017/04/01
Purpose    : map implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"

#include "atanorint.h"
//#prime
#include "atanorprimemaplf.h"
#include "atanorlist.h"
#include "atanorlvector.h"


//We need to declare once again our local definitions.
Exporting hmap<unsigned short, primemaplfMethod>  Atanorprimemaplf::methods;
Exporting hmap<string, string> Atanorprimemaplf::infomethods;
Exporting bin_hash<unsigned long> Atanorprimemaplf::exported;

Exporting short Atanorprimemaplf::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorprimemaplf::AddMethod(AtanorGlobal* global, string name,primemaplfMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanorprimemaplf::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Atanorprimemaplf::idtype = global->Getid("primemaplf");

    Atanorprimemaplf::AddMethod(global, "clear", &Atanorprimemaplf::MethodClear, P_NONE, "clear(): clear the container.");
    
    Atanorprimemaplf::AddMethod(global, "invert", &Atanorprimemaplf::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Atanorprimemaplf::AddMethod(global, "find", &Atanorprimemaplf::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Atanorprimemaplf::AddMethod(global, "items", &Atanorprimemaplf::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Atanorprimemaplf::AddMethod(global, "join", &Atanorprimemaplf::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Atanorprimemaplf::AddMethod(global, "test", &Atanorprimemaplf::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Atanorprimemaplf::AddMethod(global, "keys", &Atanorprimemaplf::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Atanorprimemaplf::AddMethod(global, "values", &Atanorprimemaplf::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Atanorprimemaplf::AddMethod(global, "sum", &Atanorprimemaplf::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Atanorprimemaplf::AddMethod(global, "product", &Atanorprimemaplf::MethodProduct, P_NONE, "product(): return the product of elements.");
    Atanorprimemaplf::AddMethod(global, "pop", &Atanorprimemaplf::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Atanorprimemaplf::AddMethod(global, "merge", &Atanorprimemaplf::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Atanorprimemaplf::idtype] = new Atanorprimemaplf(global);
    
    global->RecordMethods(Atanorprimemaplf::idtype, Atanorprimemaplf::exported);

    return true;
}


Exporting AtanorIteration* Atanorprimemaplf::Newiteration(bool direction) {
    return new AtanorIterationprimemaplf(this, direction);
}


Exporting Atanor* Atanorprimemaplf::in(Atanor* context, Atanor* a, short idthread) {
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

Exporting Atanor* Atanorprimemaplf::MethodFind(Atanor* context, short idthread, AtanorCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    prime_hash<BLONG, double>::iterator it;
    
    double val = callfunc->Evaluate(0, context, idthread)->Float();
    
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



Exporting Atanor* Atanorprimemaplf::Push(Atanor* k, Atanor* v) {
    Locking _lock(this);
    BLONG s = k->Long();
    values[s] = v->Float();
    return aTRUE;
}

Exporting Atanor* Atanorprimemaplf::Pop(Atanor* kkey) {
    BLONG k = kkey->Long();
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Atanorprimemaplf::Clear() {
    Locking _lock(this);
    values.clear();
}

Exporting void Atanorprimemaplf::Clean() {
    //Your cleaning code
}

Exporting string Atanorprimemaplf::String() {
    Locking _lock(this);
    stringstream res;
    prime_hash<BLONG,double>::iterator it;
    res << "{";
    bool beg = true;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it->first << ":" << it->second;
    }
    res << "}";
    return res.str();
}

Exporting string Atanorprimemaplf::JSonString() {
    Locking _lock(this);
    stringstream res;
    prime_hash<BLONG,double>::iterator it;
    res << "{";
    bool beg = true;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        res << '"' << it->first << '"' << ":" << it->second;
    }
    res << "}";
    return res.str();
}


Exporting long Atanorprimemaplf::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanorprimemaplf::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanorprimemaplf::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanorprimemaplf::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Atanorprimemaplf::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Atanor*  Atanorprimemaplf::Put(Atanor* idx, Atanor* ke, short idthread) {
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
            values = ((Atanorprimemaplf*)ke)->values;
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
            list<Atanor*>::iterator it;
            for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
                values[nb] = (*it)->Float();
                nb++;
            }
            return aTRUE;
        }

        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalAtanor->Returnerror("Wrong map initialization", idthread);

        if (ke != this) {
            values.clear();
            if (ke->Type() == Atanorprimemaplf::idtype) {
                Atanorprimemaplf* kmap = (Atanorprimemaplf*)ke;
                //We copy all values from ke to this
                prime_hash<BLONG,double>::iterator it;
                for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                    values[it->first] = it->second;
                kmap->Release();
            }
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

Exporting Atanor* Atanorprimemaplf::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Atanor* vect = contextualpattern->Newinstance(idthread);
            prime_hash<BLONG,double>::iterator it;
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
        Atanorprimemaplf* kmap = new Atanorprimemaplf;
        key = ((AtanorIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Atanor* keyright = ((AtanorIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        BLONG vleft = key->Long();
        BLONG vright = keyright->Long();
        prime_hash<BLONG,double>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        prime_hash<BLONG, double>::iterator itr= values.end();
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

Exporting Atanor* Atanorprimemaplf::same(Atanor* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Atanorprimemaplf* m = (Atanorprimemaplf*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    prime_hash<BLONG,double>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Atanor* Atanorprimemaplf::xorset(Atanor* b, bool itself) {

    Doublelocking _lock(this, b);

    
    Atanorprimemaplf* res;
    
    
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemaplf;
        hmap<BLONG, double> keys;

        prime_hash<BLONG, double>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;
            
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
        res = (Atanorprimemaplf*)Atom(true);

    prime_hash<BLONG,double>::iterator it;
    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second ^ v;
    return res;
}

Exporting Atanor* Atanorprimemaplf::orset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,double>::iterator it;
    Atanorprimemaplf* res;
    if (itself)
        res = this;
    else
        res = (Atanorprimemaplf*)Atom(true);

    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second | v;
    return res;

}

Exporting Atanor* Atanorprimemaplf::andset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,double>::iterator it;
    Atanorprimemaplf* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemaplf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            if (values.find(itr->Keylong()) != values.end() && values[itr->Keylong()]==v)
                res->values[itr->Keylong()] = v;
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorprimemaplf*)Atom(true);

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second & v;
    return res;
}

Exporting Atanor* Atanorprimemaplf::plus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,double>::iterator it;
    Atanorprimemaplf* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemaplf;
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
        res = (Atanorprimemaplf*)Atom(true);

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second += v;
    return res;
}

Exporting Atanor* Atanorprimemaplf::minus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,double>::iterator it;
    Atanorprimemaplf * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemaplf;
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
        res = (Atanorprimemaplf*)Atom(true);

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second -= v;
    return res;
}

Exporting Atanor* Atanorprimemaplf::multiply(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,double>::iterator it;
    Atanorprimemaplf * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemaplf;
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
        res = (Atanorprimemaplf*)Atom(true);

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second *= v;
    return res;

}

Exporting Atanor* Atanorprimemaplf::divide(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,double>::iterator it;
    Atanorprimemaplf * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemaplf;
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
        res = (Atanorprimemaplf*)Atom(true);

    double v = b->Float();
    if (v == 0) {
        res->Release();
        return new AtanorError("Error: Divided by 0");
    }
    for (it = values.begin(); it != values.end(); it++)
        it->second /= v;
    return res;

}

Exporting Atanor* Atanorprimemaplf::mod(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,double>::iterator it;
    Atanorprimemaplf * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemaplf;
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
        res = (Atanorprimemaplf*)Atom(true);

    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return new AtanorError("Error: Divided by 0");
    }
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second % v;
    return res;
}

Exporting Atanor* Atanorprimemaplf::shiftright(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,double>::iterator it;
    Atanorprimemaplf * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemaplf;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keylong());
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
        res = (Atanorprimemaplf*)Atom(true);

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second >> v;
    return res;
}

Exporting Atanor* Atanorprimemaplf::shiftleft(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,double>::iterator it;
    Atanorprimemaplf * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemaplf;
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
        res = (Atanorprimemaplf*)Atom(true);

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second << v;
    return res;
}

Exporting Atanor* Atanorprimemaplf::power(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,double>::iterator it;
    Atanorprimemaplf * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemaplf;
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
        res = (Atanorprimemaplf*)Atom(true);

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second = pow(it->second, v);
    return res;
}

Exporting Atanor* Atanorprimemaplf::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
    Locking _lock(this);
    Atanor* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    
    prime_hash<BLONG, double>::iterator it;
    
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
