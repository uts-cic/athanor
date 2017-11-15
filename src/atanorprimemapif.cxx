
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
filename   : atanorprimemapif.cxx
Date       : 2017/04/01
Purpose    : map implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"

#include "atanorint.h"
//#prime
#include "atanorprimemapif.h"
#include "atanorlist.h"
#include "atanorivector.h"


//We need to declare once again our local definitions.
Exporting hmap<unsigned short, primemapifMethod>  Atanorprimemapif::methods;
Exporting hmap<string, string> Atanorprimemapif::infomethods;
Exporting bin_hash<unsigned long> Atanorprimemapif::exported;

Exporting short Atanorprimemapif::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorprimemapif::AddMethod(AtanorGlobal* global, string name,primemapifMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanorprimemapif::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Atanorprimemapif::idtype = global->Getid("primemapif");

    Atanorprimemapif::AddMethod(global, "clear", &Atanorprimemapif::MethodClear, P_NONE, "clear(): clear the container.");
    
    Atanorprimemapif::AddMethod(global, "invert", &Atanorprimemapif::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Atanorprimemapif::AddMethod(global, "find", &Atanorprimemapif::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Atanorprimemapif::AddMethod(global, "items", &Atanorprimemapif::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Atanorprimemapif::AddMethod(global, "join", &Atanorprimemapif::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Atanorprimemapif::AddMethod(global, "test", &Atanorprimemapif::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Atanorprimemapif::AddMethod(global, "keys", &Atanorprimemapif::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Atanorprimemapif::AddMethod(global, "values", &Atanorprimemapif::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Atanorprimemapif::AddMethod(global, "sum", &Atanorprimemapif::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Atanorprimemapif::AddMethod(global, "product", &Atanorprimemapif::MethodProduct, P_NONE, "product(): return the product of elements.");
    Atanorprimemapif::AddMethod(global, "pop", &Atanorprimemapif::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Atanorprimemapif::AddMethod(global, "merge", &Atanorprimemapif::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Atanorprimemapif::idtype] = new Atanorprimemapif(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("ifprimemap")] = new Atanorprimemapif(global);

    global->RecordMethods(global->Getid("ifprimemap"), Atanorprimemapif::exported);
    #endif
    global->RecordMethods(Atanorprimemapif::idtype, Atanorprimemapif::exported);

    return true;
}


Exporting AtanorIteration* Atanorprimemapif::Newiteration(bool direction) {
    return new AtanorIterationprimemapif(this, direction);
}


Exporting Atanor* Atanorprimemapif::in(Atanor* context, Atanor* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    long val = a->Integer();

    if (context->isBoolean()) {
        Locking _lock(this);
        if (values.find(val)!=values.end())
            return aTRUE;
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Atanorivector* v = (Atanorivector*)Selectaivector(context);
        Doublelocking _lock(this, v);
        if (values.find(val)!=values.end())
            v->values.push_back(val);

        return v;
    }
    
    Locking _lock(this);
    if (values.find(val)!=values.end())
        return globalAtanor->Provideint(val);

    return aNULL;

}

Exporting Atanor* Atanorprimemapif::MethodFind(Atanor* context, short idthread, AtanorCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    prime_hash<long, double>::iterator it;
    
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
        Atanorivector* v = (Atanorivector*)Selectaivector(context);
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
            return globalAtanor->Provideint(it->first);
    }

    return aNULL;

}



Exporting Atanor* Atanorprimemapif::Push(Atanor* k, Atanor* v) {
    Locking _lock(this);
    long s = k->Integer();
    values[s] = v->Float();
    return aTRUE;
}

Exporting Atanor* Atanorprimemapif::Pop(Atanor* kkey) {
    long k = kkey->Integer();
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Atanorprimemapif::Clear() {
    Locking _lock(this);
    values.clear();
}

Exporting void Atanorprimemapif::Clean() {
    //Your cleaning code
}

Exporting string Atanorprimemapif::String() {
    Locking _lock(this);
    stringstream res;
    prime_hash<long,double>::iterator it;
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

Exporting string Atanorprimemapif::JSonString() {
    Locking _lock(this);
    stringstream res;
    prime_hash<long,double>::iterator it;
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


Exporting long Atanorprimemapif::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanorprimemapif::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanorprimemapif::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanorprimemapif::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Atanorprimemapif::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Atanor*  Atanorprimemapif::Put(Atanor* idx, Atanor* ke, short idthread) {
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
            values = ((Atanorprimemapif*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            values.clear();
            AtanorIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keyinteger()] = itr->Valuefloat();
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
            if (ke->Type() == Atanorprimemapif::idtype) {
                Atanorprimemapif* kmap = (Atanorprimemapif*)ke;
                //We copy all values from ke to this
                prime_hash<long,double>::iterator it;
                for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                    values[it->first] = it->second;
                kmap->Release();
            }
            else {
                AtanorIteration* itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                    values[itr->Keyinteger()] = itr->Valuefloat();
                itr->Release();
            }
            ke->Release();
        }
        return aTRUE;
    }

    values[idx->Integer()] = ke->Float();
    return aTRUE;
}

Exporting Atanor* Atanorprimemapif::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Atanor* vect = contextualpattern->Newinstance(idthread);
            prime_hash<long,double>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalAtanor->Provideint(it->first));
            return vect;
        }

        if (contextualpattern->Type() == a_int || contextualpattern->Type() == a_float)
            return globalAtanor->Provideint(values.size());

        return this;
    }

    Atanor* key;
    if (idx->isInterval()) {
        Atanorprimemapif* kmap = new Atanorprimemapif;
        key = ((AtanorIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Atanor* keyright = ((AtanorIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        long vleft = key->Integer();
        long vright = keyright->Integer();
        prime_hash<long,double>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        prime_hash<long, double>::iterator itr= values.end();
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

    long skey = key->Integer();
    key->Release();

    Atanor* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalAtanor->erroronkey)
            return globalAtanor->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }
    return kval;
}

Exporting Atanor* Atanorprimemapif::same(Atanor* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Atanorprimemapif* m = (Atanorprimemapif*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    prime_hash<long,double>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Atanor* Atanorprimemapif::xorset(Atanor* b, bool itself) {

    Doublelocking _lock(this, b);

    
    Atanorprimemapif* res;
    
    
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapif;
        hmap<long, double> keys;

        prime_hash<long, double>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;
            
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyinteger();
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
        res = (Atanorprimemapif*)Atom(true);

    prime_hash<long,double>::iterator it;
    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second ^ v;
    return res;
}

Exporting Atanor* Atanorprimemapif::orset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long,double>::iterator it;
    Atanorprimemapif* res;
    if (itself)
        res = this;
    else
        res = (Atanorprimemapif*)Atom(true);

    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second | v;
    return res;

}

Exporting Atanor* Atanorprimemapif::andset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long,double>::iterator it;
    Atanorprimemapif* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapif;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            if (values.find(itr->Keyinteger()) != values.end() && values[itr->Keyinteger()]==v)
                res->values[itr->Keyinteger()] = v;
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorprimemapif*)Atom(true);

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second & v;
    return res;
}

Exporting Atanor* Atanorprimemapif::plus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long,double>::iterator it;
    Atanorprimemapif* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapif;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keyinteger());
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
        res = (Atanorprimemapif*)Atom(true);

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second += v;
    return res;
}

Exporting Atanor* Atanorprimemapif::minus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long,double>::iterator it;
    Atanorprimemapif * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapif;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keyinteger());
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
        res = (Atanorprimemapif*)Atom(true);

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second -= v;
    return res;
}

Exporting Atanor* Atanorprimemapif::multiply(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long,double>::iterator it;
    Atanorprimemapif * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapif;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keyinteger());
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
        res = (Atanorprimemapif*)Atom(true);

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second *= v;
    return res;

}

Exporting Atanor* Atanorprimemapif::divide(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long,double>::iterator it;
    Atanorprimemapif * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapif;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            if (v == 0) {
                res->Release();
                return new AtanorError("Error: Divided by 0");
            }

            it = values.find(itr->Keyinteger());
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
        res = (Atanorprimemapif*)Atom(true);

    double v = b->Float();
    if (v == 0) {
        res->Release();
        return new AtanorError("Error: Divided by 0");
    }
    for (it = values.begin(); it != values.end(); it++)
        it->second /= v;
    return res;

}

Exporting Atanor* Atanorprimemapif::mod(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long,double>::iterator it;
    Atanorprimemapif * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapif;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            if (v == 0) {
                res->Release();
                return new AtanorError("Error: Divided by 0");
            }

            it = values.find(itr->Keyinteger());
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
        res = (Atanorprimemapif*)Atom(true);

    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return new AtanorError("Error: Divided by 0");
    }
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second % v;
    return res;
}

Exporting Atanor* Atanorprimemapif::shiftright(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long,double>::iterator it;
    Atanorprimemapif * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapif;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keyinteger());
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
        res = (Atanorprimemapif*)Atom(true);

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second >> v;
    return res;
}

Exporting Atanor* Atanorprimemapif::shiftleft(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long,double>::iterator it;
    Atanorprimemapif * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapif;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keyinteger());
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
        res = (Atanorprimemapif*)Atom(true);

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second << v;
    return res;
}

Exporting Atanor* Atanorprimemapif::power(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long,double>::iterator it;
    Atanorprimemapif * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapif;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keyinteger());
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
        res = (Atanorprimemapif*)Atom(true);

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second = pow(it->second, v);
    return res;
}

Exporting Atanor* Atanorprimemapif::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
    Locking _lock(this);
    Atanor* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    
    prime_hash<long, double>::iterator it;
    
    Atanor* a;
    vector<long> keys;

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
