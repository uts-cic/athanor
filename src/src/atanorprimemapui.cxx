
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
filename   : atanorprimemapui.cxx
Date       : 2017/04/01
Purpose    : map implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanoruvector.h"
#include "atanorustring.h"
//#prime
#include "atanorprimemapui.h"
#include "atanorlist.h"
#include "atanorivector.h"


//We need to declare once again our local definitions.
Exporting hmap<unsigned short, primemapuiMethod>  Atanorprimemapui::methods;
Exporting hmap<string, string> Atanorprimemapui::infomethods;
Exporting bin_hash<unsigned long> Atanorprimemapui::exported;

Exporting short Atanorprimemapui::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorprimemapui::AddMethod(AtanorGlobal* global, string name,primemapuiMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanorprimemapui::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Atanorprimemapui::idtype = global->Getid("primemapui");

    Atanorprimemapui::AddMethod(global, "clear", &Atanorprimemapui::MethodClear, P_NONE, "clear(): clear the container.");
    
    Atanorprimemapui::AddMethod(global, "invert", &Atanorprimemapui::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Atanorprimemapui::AddMethod(global, "find", &Atanorprimemapui::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Atanorprimemapui::AddMethod(global, "items", &Atanorprimemapui::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Atanorprimemapui::AddMethod(global, "join", &Atanorprimemapui::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Atanorprimemapui::AddMethod(global, "test", &Atanorprimemapui::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Atanorprimemapui::AddMethod(global, "keys", &Atanorprimemapui::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Atanorprimemapui::AddMethod(global, "values", &Atanorprimemapui::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Atanorprimemapui::AddMethod(global, "sum", &Atanorprimemapui::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Atanorprimemapui::AddMethod(global, "product", &Atanorprimemapui::MethodProduct, P_NONE, "product(): return the product of elements.");
    Atanorprimemapui::AddMethod(global, "pop", &Atanorprimemapui::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Atanorprimemapui::AddMethod(global, "merge", &Atanorprimemapui::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Atanorprimemapui::idtype] = new Atanorprimemapui(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("uiprimemap")] = new Atanorprimemapui(global);

    global->RecordMethods(global->Getid("uiprimemap"), Atanorprimemapui::exported);
    #endif
    global->RecordMethods(Atanorprimemapui::idtype, Atanorprimemapui::exported);

    return true;
}


Exporting AtanorIteration* Atanorprimemapui::Newiteration(bool direction) {
    return new AtanorIterationprimemapui(this, direction);
}


Exporting Atanor* Atanorprimemapui::in(Atanor* context, Atanor* a, short idthread) {
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

Exporting Atanor* Atanorprimemapui::MethodFind(Atanor* context, short idthread, AtanorCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    prime_hash<wstring, long>::iterator it;
    
    long val = callfunc->Evaluate(0, context, idthread)->Integer();
    
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



Exporting Atanor* Atanorprimemapui::Push(Atanor* k, Atanor* v) {
    Locking _lock(this);
    wstring s = k->UString();
    values[s] = v->Integer();
    return aTRUE;
}

Exporting Atanor* Atanorprimemapui::Pop(Atanor* kkey) {
    wstring k = kkey->UString();
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Atanorprimemapui::Clear() {
    Locking _lock(this);
    values.clear();
}

Exporting void Atanorprimemapui::Clean() {
    //Your cleaning code
}

Exporting string Atanorprimemapui::String() {
    Locking _lock(this);
    stringstream res;
    prime_hash<wstring,long>::iterator it;
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

Exporting string Atanorprimemapui::JSonString() {
    Locking _lock(this);
    stringstream res;
    prime_hash<wstring,long>::iterator it;
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


Exporting long Atanorprimemapui::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanorprimemapui::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanorprimemapui::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanorprimemapui::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Atanorprimemapui::Size() {
    Locking _lock(this);
    return values.size();
}

#ifndef swprintf_s
#define swprintf_s(a,b,c,d) swprintf(a,b,c,d)
    #endif


Exporting Atanor*  Atanorprimemapui::Put(Atanor* idx, Atanor* ke, short idthread) {
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
            values = ((Atanorprimemapui*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            values.clear();
            AtanorIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keyustring()] = itr->Valueinteger();
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
                values[buff] = (*it)->Integer();
                nb++;
            }
            return aTRUE;
        }

        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalAtanor->Returnerror("Wrong map initialization", idthread);

        if (ke != this) {
            values.clear();
            if (ke->Type() == Atanorprimemapui::idtype) {
                Atanorprimemapui* kmap = (Atanorprimemapui*)ke;
                //We copy all values from ke to this
                prime_hash<wstring,long>::iterator it;
                for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                    values[it->first] = it->second;
                kmap->Release();
            }
            else {
                AtanorIteration* itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                    values[itr->Keyustring()] = itr->Valueinteger();
                itr->Release();
            }
            ke->Release();
        }
        return aTRUE;
    }

    values[idx->UString()] = ke->Integer();
    return aTRUE;
}

Exporting Atanor* Atanorprimemapui::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Atanor* vect = contextualpattern->Newinstance(idthread);
            prime_hash<wstring,long>::iterator it;
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
        Atanorprimemapui* kmap = new Atanorprimemapui;
        key = ((AtanorIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Atanor* keyright = ((AtanorIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        wstring vleft = key->UString();
        wstring vright = keyright->UString();
        prime_hash<wstring,long>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        prime_hash<wstring, long>::iterator itr= values.end();
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

Exporting Atanor* Atanorprimemapui::same(Atanor* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Atanorprimemapui* m = (Atanorprimemapui*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    prime_hash<wstring,long>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Atanor* Atanorprimemapui::xorset(Atanor* b, bool itself) {

    Doublelocking _lock(this, b);

    
    Atanorprimemapui* res;
    
    
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapui;
        hmap<wstring, long> keys;

        prime_hash<wstring, long>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;
            
        wstring v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyustring();
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
        res = (Atanorprimemapui*)Atom(true);

    prime_hash<wstring,long>::iterator it;
    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second ^ v;
    return res;
}

Exporting Atanor* Atanorprimemapui::orset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,long>::iterator it;
    Atanorprimemapui* res;
    if (itself)
        res = this;
    else
        res = (Atanorprimemapui*)Atom(true);

    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second | v;
    return res;

}

Exporting Atanor* Atanorprimemapui::andset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,long>::iterator it;
    Atanorprimemapui* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapui;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            if (values.find(itr->Keyustring()) != values.end() && values[itr->Keyustring()]==v)
                res->values[itr->Keyustring()] = v;
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorprimemapui*)Atom(true);

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second & v;
    return res;
}

Exporting Atanor* Atanorprimemapui::plus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,long>::iterator it;
    Atanorprimemapui* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapui;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
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
        res = (Atanorprimemapui*)Atom(true);

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second += v;
    return res;
}

Exporting Atanor* Atanorprimemapui::minus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,long>::iterator it;
    Atanorprimemapui * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapui;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
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
        res = (Atanorprimemapui*)Atom(true);

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second -= v;
    return res;
}

Exporting Atanor* Atanorprimemapui::multiply(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,long>::iterator it;
    Atanorprimemapui * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapui;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
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
        res = (Atanorprimemapui*)Atom(true);

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second *= v;
    return res;

}

Exporting Atanor* Atanorprimemapui::divide(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,long>::iterator it;
    Atanorprimemapui * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapui;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
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
        res = (Atanorprimemapui*)Atom(true);

    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return new AtanorError("Error: Divided by 0");
    }
    for (it = values.begin(); it != values.end(); it++)
        it->second /= v;
    return res;

}

Exporting Atanor* Atanorprimemapui::mod(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,long>::iterator it;
    Atanorprimemapui * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapui;
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
        res = (Atanorprimemapui*)Atom(true);

    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return new AtanorError("Error: Divided by 0");
    }
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second % v;
    return res;
}

Exporting Atanor* Atanorprimemapui::shiftright(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,long>::iterator it;
    Atanorprimemapui * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapui;
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
        res = (Atanorprimemapui*)Atom(true);

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second >> v;
    return res;
}

Exporting Atanor* Atanorprimemapui::shiftleft(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,long>::iterator it;
    Atanorprimemapui * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapui;
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
        res = (Atanorprimemapui*)Atom(true);

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second << v;
    return res;
}

Exporting Atanor* Atanorprimemapui::power(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,long>::iterator it;
    Atanorprimemapui * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorprimemapui;
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
        res = (Atanorprimemapui*)Atom(true);

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second = pow(it->second, v);
    return res;
}

Exporting Atanor* Atanorprimemapui::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
    Locking _lock(this);
    Atanor* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    
    prime_hash<wstring, long>::iterator it;
    
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
