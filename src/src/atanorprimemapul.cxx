
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
filename   : atanorprimemapul.cxx
Date       : 2017/04/01
Purpose    : map implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanoruvector.h"
#include "atanorustring.h"
//#prime
#include "atanorprimemapul.h"
#include "atanorlist.h"
#include "atanorlvector.h"


//We need to declare once again our local definitions.
Exporting hmap<unsigned short, primemapulMethod>  Atanorprimemapul::methods;
Exporting hmap<string, string> Atanorprimemapul::infomethods;
Exporting bin_hash<unsigned long> Atanorprimemapul::exported;

Exporting short Atanorprimemapul::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorprimemapul::AddMethod(AtanorGlobal* global, string name,primemapulMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanorprimemapul::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Atanorprimemapul::idtype = global->Getid("primemapul");

    Atanorprimemapul::AddMethod(global, "clear", &Atanorprimemapul::MethodClear, P_NONE, "clear(): clear the container.");
    
    Atanorprimemapul::AddMethod(global, "invert", &Atanorprimemapul::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Atanorprimemapul::AddMethod(global, "find", &Atanorprimemapul::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Atanorprimemapul::AddMethod(global, "items", &Atanorprimemapul::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Atanorprimemapul::AddMethod(global, "join", &Atanorprimemapul::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Atanorprimemapul::AddMethod(global, "test", &Atanorprimemapul::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Atanorprimemapul::AddMethod(global, "keys", &Atanorprimemapul::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Atanorprimemapul::AddMethod(global, "values", &Atanorprimemapul::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Atanorprimemapul::AddMethod(global, "sum", &Atanorprimemapul::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Atanorprimemapul::AddMethod(global, "product", &Atanorprimemapul::MethodProduct, P_NONE, "product(): return the product of elements.");
    Atanorprimemapul::AddMethod(global, "pop", &Atanorprimemapul::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Atanorprimemapul::AddMethod(global, "merge", &Atanorprimemapul::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Atanorprimemapul::idtype] = new Atanorprimemapul(global);
    
    global->RecordMethods(Atanorprimemapul::idtype, Atanorprimemapul::exported);

    return true;
}


Exporting AtanorIteration* Atanorprimemapul::Newiteration(bool direction) {
    return new AtanorIterationprimemapul(this, direction);
}


Exporting Atanor* Atanorprimemapul::in(Atanor* context, Atanor* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    wstring val = a->UString();

    if (context->isBoolean()) {
        Locking _lock(this);
        if (values.find(val)!=values.end())
            return aTRUE;
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Atanoruvector* v = (Atanoruvector*)Selectauvector(context);
        Doublelocking _lock(this, v);
        if (values.find(val)!=values.end())
            v->values.push_back(val);

        return v;
    }
    
    Locking _lock(this);
    if (values.find(val)!=values.end())
        return globalAtanor->Provideustring(val);

    return aNULL;

}

Exporting Atanor* Atanorprimemapul::MethodFind(Atanor* context, short idthread, AtanorCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    prime_hash<wstring, BLONG>::iterator it;
    
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
        Atanoruvector* v = (Atanoruvector*)Selectauvector(context);
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
            return globalAtanor->Provideustring(it->first);
    }

    return aNULL;

}



Exporting Atanor* Atanorprimemapul::Push(Atanor* k, Atanor* v) {
    Locking _lock(this);
    wstring s = k->UString();
    values[s] = v->Long();
    return aTRUE;
}

Exporting Atanor* Atanorprimemapul::Pop(Atanor* kkey) {
    wstring k = kkey->UString();
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Atanorprimemapul::Clear() {
    Locking _lock(this);
    values.clear();
}

Exporting void Atanorprimemapul::Clean() {
    //Your cleaning code
}

Exporting string Atanorprimemapul::String() {
    Locking _lock(this);
    stringstream res;
    prime_hash<wstring,BLONG>::iterator it;
    res << "{";
    bool beg = true;
    string sx;
    wstring w;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        w = it->first;
        sc_unicode_to_utf8(sx, w);
        stringing(res, sx);
        res << ":" << it->second;
    }
    res << "}";
    return res.str();
}

Exporting string Atanorprimemapul::JSonString() {
    Locking _lock(this);
    stringstream res;
    prime_hash<wstring,BLONG>::iterator it;
    res << "{";
    bool beg = true;
    string sx;
    wstring w;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        w = it->first;
        sc_unicode_to_utf8(sx, w);
        jstringing(res, sx);
        res << ":" << it->second;
    }
    res << "}";
    return res.str();
}


Exporting long Atanorprimemapul::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanorprimemapul::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanorprimemapul::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanorprimemapul::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Atanorprimemapul::Size() {
    Locking _lock(this);
    return values.size();
}

#ifndef swprintf_s
#define swprintf_s(a,b,c,d) swprintf(a,b,c,d)
    #endif


Exporting Atanor*  Atanorprimemapul::Put(Atanor* idx, Atanor* ke, short idthread) {
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
            values = ((Atanorprimemapul*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            values.clear();
            AtanorIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keyustring()] = itr->Valuelong();
            itr->Release();
            return aTRUE;
        }

        wchar_t buff[20];
        if (ke->isVectorContainer()) {
            values.clear();
            nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                swprintf_s(buff, 20, L"%d", nb);
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
                swprintf_s(buff, 20, L"%d", nb);
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
            if (ke->Type() == Atanorprimemapul::idtype) {
                Atanorprimemapul* kmap = (Atanorprimemapul*)ke;
                //We copy all values from ke to this
                prime_hash<wstring,BLONG>::iterator it;
                for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                    values[it->first] = it->second;
                kmap->Release();
            }
            else {
                AtanorIteration* itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                    values[itr->Keyustring()] = itr->Valuelong();
                itr->Release();
            }
            ke->Release();
        }
        return aTRUE;
    }

    values[idx->UString()] = ke->Long();
    return aTRUE;
}

Exporting Atanor* Atanorprimemapul::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Atanor* vect = contextualpattern->Newinstance(idthread);
            prime_hash<wstring,BLONG>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalAtanor->Provideustring(it->first));
            return vect;
        }

        if (contextualpattern->Type() == a_int || contextualpattern->Type() == a_float)
            return globalAtanor->Provideint(values.size());

        return this;
    }

    Atanor* key;
    if (idx->isInterval()) {
        Atanorprimemapul* kmap = new Atanorprimemapul;
        key = ((AtanorIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Atanor* keyright = ((AtanorIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        wstring vleft = key->UString();
        wstring vright = keyright->UString();
        prime_hash<wstring,BLONG>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        prime_hash<wstring, BLONG>::iterator itr= values.end();
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

    wstring skey = key->UString();
    key->Release();

    Atanor* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalAtanor->erroronkey)
            return globalAtanor->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }
    return kval;
}

Exporting Atanor* Atanorprimemapul::same(Atanor* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Atanorprimemapul* m = (Atanorprimemapul*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    prime_hash<wstring,BLONG>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Atanor* Atanorprimemapul::xorset(Atanor* b, bool itself) {

    Doublelocking _lock(this, b);

    
    Atanorprimemapul* res;
    
    
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapul;
        hmap<wstring, BLONG> keys;

        prime_hash<wstring, BLONG>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;
            
        wstring v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyustring();
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
        res = (Atanorprimemapul*)Atom(true);

    prime_hash<wstring,BLONG>::iterator it;
    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second ^ v;
    return res;
}

Exporting Atanor* Atanorprimemapul::orset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,BLONG>::iterator it;
    Atanorprimemapul* res;
    if (itself)
        res = this;
    else
        res = (Atanorprimemapul*)Atom(true);

    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second | v;
    return res;

}

Exporting Atanor* Atanorprimemapul::andset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,BLONG>::iterator it;
    Atanorprimemapul* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapul;
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
            if (values.find(itr->Keyustring()) != values.end() && values[itr->Keyustring()]==v)
                res->values[itr->Keyustring()] = v;
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorprimemapul*)Atom(true);

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second & v;
    return res;
}

Exporting Atanor* Atanorprimemapul::plus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,BLONG>::iterator it;
    Atanorprimemapul* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapul;
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
            it = values.find(itr->Keyustring());
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
        res = (Atanorprimemapul*)Atom(true);

    BLONG v = b->Long();
    for (it = values.begin(); it != values.end(); it++)
        it->second += v;
    return res;
}

Exporting Atanor* Atanorprimemapul::minus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,BLONG>::iterator it;
    Atanorprimemapul * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapul;
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
            it = values.find(itr->Keyustring());
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
        res = (Atanorprimemapul*)Atom(true);

    BLONG v = b->Long();
    for (it = values.begin(); it != values.end(); it++)
        it->second -= v;
    return res;
}

Exporting Atanor* Atanorprimemapul::multiply(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,BLONG>::iterator it;
    Atanorprimemapul * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapul;
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
            it = values.find(itr->Keyustring());
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
        res = (Atanorprimemapul*)Atom(true);

    BLONG v = b->Long();
    for (it = values.begin(); it != values.end(); it++)
        it->second *= v;
    return res;

}

Exporting Atanor* Atanorprimemapul::divide(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,BLONG>::iterator it;
    Atanorprimemapul * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapul;
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
            if (v == 0) {
                res->Release();
                return new AtanorError("Error: Divided by 0");
            }

            it = values.find(itr->Keyustring());
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
        res = (Atanorprimemapul*)Atom(true);

    BLONG v = b->Long();
    if (v == 0) {
        res->Release();
        return new AtanorError("Error: Divided by 0");
    }
    for (it = values.begin(); it != values.end(); it++)
        it->second /= v;
    return res;

}

Exporting Atanor* Atanorprimemapul::mod(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,BLONG>::iterator it;
    Atanorprimemapul * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapul;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            if (v == 0) {
                res->Release();
                return new AtanorError("Error: Divided by 0");
            }

            it = values.find(itr->Keyustring());
            if (it != values.end()) {
                res->values[it->first] = (long)it->second  % v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorprimemapul*)Atom(true);

    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return new AtanorError("Error: Divided by 0");
    }
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second % v;
    return res;
}

Exporting Atanor* Atanorprimemapul::shiftright(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,BLONG>::iterator it;
    Atanorprimemapul * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapul;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keyustring());
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
        res = (Atanorprimemapul*)Atom(true);

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second >> v;
    return res;
}

Exporting Atanor* Atanorprimemapul::shiftleft(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,BLONG>::iterator it;
    Atanorprimemapul * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapul;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keyustring());
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
        res = (Atanorprimemapul*)Atom(true);

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second << v;
    return res;
}

Exporting Atanor* Atanorprimemapul::power(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,BLONG>::iterator it;
    Atanorprimemapul * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapul;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keyustring());
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
        res = (Atanorprimemapul*)Atom(true);

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second = pow(it->second, v);
    return res;
}

Exporting Atanor* Atanorprimemapul::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
    Locking _lock(this);
    Atanor* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    
    prime_hash<wstring, BLONG>::iterator it;
    
    Atanor* a;
    vector<wstring> keys;

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
