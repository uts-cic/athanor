
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
filename   : atanorprimemapsl.cxx
Date       : 2017/04/01
Purpose    : map implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorsvector.h"
#include "atanorstring.h"
//#prime
#include "atanorprimemapsl.h"
#include "atanorlist.h"
#include "atanorlvector.h"


//We need to declare once again our local definitions.
Exporting hmap<unsigned short, primemapslMethod>  Atanorprimemapsl::methods;
Exporting hmap<string, string> Atanorprimemapsl::infomethods;
Exporting bin_hash<unsigned long> Atanorprimemapsl::exported;

Exporting short Atanorprimemapsl::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorprimemapsl::AddMethod(AtanorGlobal* global, string name,primemapslMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanorprimemapsl::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Atanorprimemapsl::idtype = global->Getid("primemapsl");

    Atanorprimemapsl::AddMethod(global, "clear", &Atanorprimemapsl::MethodClear, P_NONE, "clear(): clear the container.");
    
    Atanorprimemapsl::AddMethod(global, "invert", &Atanorprimemapsl::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Atanorprimemapsl::AddMethod(global, "find", &Atanorprimemapsl::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Atanorprimemapsl::AddMethod(global, "items", &Atanorprimemapsl::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Atanorprimemapsl::AddMethod(global, "join", &Atanorprimemapsl::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Atanorprimemapsl::AddMethod(global, "test", &Atanorprimemapsl::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Atanorprimemapsl::AddMethod(global, "keys", &Atanorprimemapsl::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Atanorprimemapsl::AddMethod(global, "values", &Atanorprimemapsl::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Atanorprimemapsl::AddMethod(global, "sum", &Atanorprimemapsl::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Atanorprimemapsl::AddMethod(global, "product", &Atanorprimemapsl::MethodProduct, P_NONE, "product(): return the product of elements.");
    Atanorprimemapsl::AddMethod(global, "pop", &Atanorprimemapsl::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Atanorprimemapsl::AddMethod(global, "merge", &Atanorprimemapsl::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Atanorprimemapsl::idtype] = new Atanorprimemapsl(global);
    
    global->RecordMethods(Atanorprimemapsl::idtype, Atanorprimemapsl::exported);

    return true;
}


Exporting AtanorIteration* Atanorprimemapsl::Newiteration(bool direction) {
    return new AtanorIterationprimemapsl(this, direction);
}


Exporting Atanor* Atanorprimemapsl::in(Atanor* context, Atanor* a, short idthread) {
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

Exporting Atanor* Atanorprimemapsl::MethodFind(Atanor* context, short idthread, AtanorCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    prime_hash<string, BLONG>::iterator it;
    
    BLONG val = callfunc->Evaluate(0, context, idthread)->Long();
    
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



Exporting Atanor* Atanorprimemapsl::Push(Atanor* k, Atanor* v) {
    Locking _lock(this);
    string s = k->String();
    values[s] = v->Long();
    return aTRUE;
}

Exporting Atanor* Atanorprimemapsl::Pop(Atanor* kkey) {
    string k = kkey->String();
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Atanorprimemapsl::Clear() {
    Locking _lock(this);
    values.clear();
}

Exporting void Atanorprimemapsl::Clean() {
    //Your cleaning code
}

Exporting string Atanorprimemapsl::String() {
    Locking _lock(this);
    stringstream res;
    prime_hash<string,BLONG>::iterator it;
    res << "{";
    bool beg = true;
    string sx;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        sx = it->first;
        stringing(res, sx);
        res << ":" << it->second;
    }
    res << "}";
    return res.str();
}

Exporting string Atanorprimemapsl::JSonString() {
    Locking _lock(this);
    stringstream res;
    prime_hash<string,BLONG>::iterator it;
    res << "{";
    bool beg = true;
    string sx;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        sx = it->first;
        jstringing(res, sx);
        res << ":" << it->second;
    }
    res << "}";
    return res.str();
}


Exporting long Atanorprimemapsl::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanorprimemapsl::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanorprimemapsl::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanorprimemapsl::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Atanorprimemapsl::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Atanor*  Atanorprimemapsl::Put(Atanor* idx, Atanor* ke, short idthread) {
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
            values = ((Atanorprimemapsl*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            values.clear();
            AtanorIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keystring()] = itr->Valuelong();
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
            list<Atanor*>::iterator it;
            for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
                sprintf_s(buff, 20, "%d", nb);
                values[buff] = (*it)->Long();
                nb++;
            }
            return aTRUE;
        }

        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalAtanor->Returnerror("Wrong map initialization", idthread);

        if (ke != this) {
            values.clear();
            if (ke->Type() == Atanorprimemapsl::idtype) {
                Atanorprimemapsl* kmap = (Atanorprimemapsl*)ke;
                //We copy all values from ke to this
                prime_hash<string,BLONG>::iterator it;
                for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                    values[it->first] = it->second;
                kmap->Release();
            }
            else {
                AtanorIteration* itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                    values[itr->Keystring()] = itr->Valuelong();
                itr->Release();
            }
            ke->Release();
        }
        return aTRUE;
    }

    values[idx->String()] = ke->Long();
    return aTRUE;
}

Exporting Atanor* Atanorprimemapsl::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Atanor* vect = contextualpattern->Newinstance(idthread);
            prime_hash<string,BLONG>::iterator it;
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
        Atanorprimemapsl* kmap = new Atanorprimemapsl;
        key = ((AtanorIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Atanor* keyright = ((AtanorIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        string vleft = key->String();
        string vright = keyright->String();
        prime_hash<string,BLONG>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        prime_hash<string, BLONG>::iterator itr= values.end();
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

Exporting Atanor* Atanorprimemapsl::same(Atanor* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Atanorprimemapsl* m = (Atanorprimemapsl*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    prime_hash<string,BLONG>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Atanor* Atanorprimemapsl::xorset(Atanor* b, bool itself) {

    Doublelocking _lock(this, b);

    
    Atanorprimemapsl* res;
    
    
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapsl;
        hmap<string, BLONG> keys;

        prime_hash<string, BLONG>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;
            
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keystring();
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
        res = (Atanorprimemapsl*)Atom(true);

    prime_hash<string,BLONG>::iterator it;
    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second ^ v;
    return res;
}

Exporting Atanor* Atanorprimemapsl::orset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<string,BLONG>::iterator it;
    Atanorprimemapsl* res;
    if (itself)
        res = this;
    else
        res = (Atanorprimemapsl*)Atom(true);

    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second | v;
    return res;

}

Exporting Atanor* Atanorprimemapsl::andset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<string,BLONG>::iterator it;
    Atanorprimemapsl* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapsl;
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
            if (values.find(itr->Keystring()) != values.end() && values[itr->Keystring()]==v)
                res->values[itr->Keystring()] = v;
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorprimemapsl*)Atom(true);

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second & v;
    return res;
}

Exporting Atanor* Atanorprimemapsl::plus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<string,BLONG>::iterator it;
    Atanorprimemapsl* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapsl;
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
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
        res = (Atanorprimemapsl*)Atom(true);

    BLONG v = b->Long();
    for (it = values.begin(); it != values.end(); it++)
        it->second += v;
    return res;
}

Exporting Atanor* Atanorprimemapsl::minus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<string,BLONG>::iterator it;
    Atanorprimemapsl * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapsl;
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
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
        res = (Atanorprimemapsl*)Atom(true);

    BLONG v = b->Long();
    for (it = values.begin(); it != values.end(); it++)
        it->second -= v;
    return res;
}

Exporting Atanor* Atanorprimemapsl::multiply(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<string,BLONG>::iterator it;
    Atanorprimemapsl * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapsl;
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
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
        res = (Atanorprimemapsl*)Atom(true);

    BLONG v = b->Long();
    for (it = values.begin(); it != values.end(); it++)
        it->second *= v;
    return res;

}

Exporting Atanor* Atanorprimemapsl::divide(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<string,BLONG>::iterator it;
    Atanorprimemapsl * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapsl;
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
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
        res = (Atanorprimemapsl*)Atom(true);

    BLONG v = b->Long();
    if (v == 0) {
        res->Release();
        return new AtanorError("Error: Divided by 0");
    }
    for (it = values.begin(); it != values.end(); it++)
        it->second /= v;
    return res;

}

Exporting Atanor* Atanorprimemapsl::mod(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<string,BLONG>::iterator it;
    Atanorprimemapsl * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapsl;
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
        res = (Atanorprimemapsl*)Atom(true);

    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return new AtanorError("Error: Divided by 0");
    }
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second % v;
    return res;
}

Exporting Atanor* Atanorprimemapsl::shiftright(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<string,BLONG>::iterator it;
    Atanorprimemapsl * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapsl;
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
        res = (Atanorprimemapsl*)Atom(true);

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second >> v;
    return res;
}

Exporting Atanor* Atanorprimemapsl::shiftleft(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<string,BLONG>::iterator it;
    Atanorprimemapsl * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapsl;
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
        res = (Atanorprimemapsl*)Atom(true);

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second << v;
    return res;
}

Exporting Atanor* Atanorprimemapsl::power(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<string,BLONG>::iterator it;
    Atanorprimemapsl * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapsl;
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
        res = (Atanorprimemapsl*)Atom(true);

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second = pow(it->second, v);
    return res;
}

Exporting Atanor* Atanorprimemapsl::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
    Locking _lock(this);
    Atanor* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    
    prime_hash<string, BLONG>::iterator it;
    
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
