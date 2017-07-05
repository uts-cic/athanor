
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
filename   : atanorbinmapu.cxx
Date       : 2017/04/01
Purpose    : map implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"

#include "atanorint.h"
//#prime
#include "atanorbinmapu.h"
#include "atanorlist.h"
#include "atanorivector.h"


//We need to declare once again our local definitions.
Exporting hmap<unsigned short, binmapuMethod>  Atanorbinmapu::methods;
Exporting hmap<string, string> Atanorbinmapu::infomethods;
Exporting bin_hash<unsigned long> Atanorbinmapu::exported;

Exporting short Atanorbinmapu::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorbinmapu::AddMethod(AtanorGlobal* global, string name, binmapuMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanorbinmapu::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();


    Atanorbinmapu::idtype = global->Getid("binmapu");

    Atanorbinmapu::AddMethod(global, "clear", &Atanorbinmapu::MethodClear, P_NONE, "clear(): clear the container.");
    
    Atanorbinmapu::AddMethod(global, "invert", &Atanorbinmapu::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Atanorbinmapu::AddMethod(global, "find", &Atanorbinmapu::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Atanorbinmapu::AddMethod(global, "items", &Atanorbinmapu::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Atanorbinmapu::AddMethod(global, "join", &Atanorbinmapu::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Atanorbinmapu::AddMethod(global, "test", &Atanorbinmapu::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Atanorbinmapu::AddMethod(global, "keys", &Atanorbinmapu::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Atanorbinmapu::AddMethod(global, "values", &Atanorbinmapu::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Atanorbinmapu::AddMethod(global, "pop", &Atanorbinmapu::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Atanorbinmapu::AddMethod(global, "merge", &Atanorbinmapu::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Atanorbinmapu::idtype] = new Atanorbinmapu(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("iuprimemap")] = new Atanorbinmapu(global);

    global->RecordMethods(global->Getid("iuprimemap"), Atanorbinmapu::exported);
    #endif
    global->RecordMethods(Atanorbinmapu::idtype, Atanorbinmapu::exported);

    return true;
}


Exporting AtanorIteration* Atanorbinmapu::Newiteration(bool direction) {
    return new AtanorIterationbinmapu(this, direction);
}


Exporting Atanor* Atanorbinmapu::in(Atanor* context, Atanor* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    ushort val = a->Short();

    if (context->isBoolean()) {
        Locking _lock(this);
        if (values.find(val) != values.end())
            return aTRUE;
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Atanorivector* v = (Atanorivector*)Selectaivector(context);
        Doublelocking _lock(this, v);
        if (values.find(val) != values.end())
            v->values.push_back(val);

        return v;
    }

    Locking _lock(this);
    if (values.find(val) != values.end())
        return globalAtanor->Provideint(val);

    return aNULL;

}

Exporting Atanor* Atanorbinmapu::MethodFind(Atanor* context, short idthread, AtanorCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    basebin_hash<wstring>::iterator it;

    wstring val = callfunc->Evaluate(0, context, idthread)->UString();

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



Exporting Atanor* Atanorbinmapu::Push(Atanor* k, Atanor* v) {
    Locking _lock(this);
    ushort s = k->Short();
    values[s] = v->UString();
    return aTRUE;
}

Exporting Atanor* Atanorbinmapu::Pop(Atanor* kkey) {
    ushort k = kkey->Short();
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Atanorbinmapu::Clear() {
    Locking _lock(this);
    values.clear();
}

Exporting void Atanorbinmapu::Clean() {
    //Your cleaning code
}

Exporting string Atanorbinmapu::String() {
    Locking _lock(this);
    stringstream res;
    basebin_hash<wstring>::iterator it;
    res << "{";
    bool beg = true;
    string sx;
    wstring w;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it->first << ":";
        w = it->second;
        sc_unicode_to_utf8(sx, w);
        stringing(res, sx);
    }
    res << "}";
    return res.str();
}

Exporting string Atanorbinmapu::JSonString() {
    Locking _lock(this);
    stringstream res;
    basebin_hash<wstring>::iterator it;
    res << "{";
    bool beg = true;
    string sx;
    wstring w;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it->first << ":";
        w = it->second;
        sc_unicode_to_utf8(sx, w);
        jstringing(res, sx);
    }
    res << "}";
    return res.str();
}


Exporting long Atanorbinmapu::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanorbinmapu::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanorbinmapu::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanorbinmapu::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Atanorbinmapu::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Atanor*  Atanorbinmapu::Put(Atanor* idx, Atanor* ke, short idthread) {
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
            values = ((Atanorbinmapu*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            values.clear();
            AtanorIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keyshort()] = itr->Valueustring();
            itr->Release();
            return aTRUE;
        }

        if (ke->isVectorContainer()) {
            values.clear();
            nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                values[nb] = ke->getustring(it);
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
                values[nb] = (*it)->UString();
                nb++;
            }
            return aTRUE;
        }

        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalAtanor->Returnerror("Wrong map initialization", idthread);

        if (ke != this) {
            values.clear();
            if (ke->Type() == Atanorbinmapu::idtype) {
                Atanorbinmapu* kmap = (Atanorbinmapu*)ke;
                //We copy all values from ke to this
                basebin_hash<wstring>::iterator it;
                for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                    values[it->first] = it->second;
                kmap->Release();
            }
            else {
                AtanorIteration* itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                    values[itr->Keyshort()] = itr->Valueustring();
                itr->Release();
            }
            ke->Release();
        }
        return aTRUE;
    }

    values[idx->Short()] = ke->UString();
    return aTRUE;
}

Exporting Atanor* Atanorbinmapu::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Atanor* vect = contextualpattern->Newinstance(idthread);
            basebin_hash<wstring>::iterator it;
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
        Atanorbinmapu* kmap = new Atanorbinmapu;
        key = ((AtanorIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Atanor* keyright = ((AtanorIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        ushort vleft = key->Short();
        ushort  vright = keyright->Short();
        basebin_hash<wstring>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        basebin_hash<wstring>::iterator itr = values.end();
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

    ushort skey = key->Short();
    key->Release();

    Atanor* kval = Value((long)skey);
    if (kval == aNOELEMENT) {
        if (globalAtanor->erroronkey)
            return globalAtanor->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }
    return kval;
}

Exporting Atanor* Atanorbinmapu::same(Atanor* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Atanorbinmapu* m = (Atanorbinmapu*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    basebin_hash<wstring>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Atanor* Atanorbinmapu::xorset(Atanor* b, bool itself) {

    Doublelocking _lock(this, b);


    Atanorbinmapu* res;
    
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmapu;
        hmap<short, wstring> keys;

        basebin_hash<wstring>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;

        short v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyshort();
            if (keys.find(v) == keys.end())
                keys[v] = itr->Valueustring();
            else {
                if (values[v] == itr->Valueustring())
                    keys.erase(v);
            }
        }
        itr->Release();

        for (auto& a : keys)
            res->values[a.first] = a.second;

        return res;
    }
    return aNULL;
}

Exporting Atanor* Atanorbinmapu::orset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<wstring>::iterator it;
    Atanorbinmapu* res;
    if (itself)
        res = this;
    else
        res = (Atanorbinmapu*)Atom(true);

    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }

    return aNULL;
}

Exporting Atanor* Atanorbinmapu::andset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<wstring>::iterator it;
    Atanorbinmapu* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmapu;
        wstring v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            ikey = itr->Keyshort();
            if (values.check(ikey) && values[ikey] == v)
                res->values[ikey] = v;
        }
        itr->Release();
        return res;
    }

    return aNULL;
}

Exporting Atanor* Atanorbinmapu::plus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<wstring>::iterator it;
    Atanorbinmapu* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmapu;
        wstring v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->values[ikey] = values[ikey] + v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorbinmapu*)Atom(true);

    wstring v = b->UString();
    for (it = values.begin(); it != values.end(); it++)
        it->second += v;
    return res;
}

Exporting Atanor* Atanorbinmapu::minus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<wstring>::iterator it;
    Atanorbinmapu * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmapu;
        wstring v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->values[ikey] = StringMinus(values[ikey], v);
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorbinmapu*)Atom(true);

    wstring v = b->UString();
    for (it = values.begin(); it != values.end(); it++)
        it->second = StringMinus(it->second, v);
    return res;
}




Exporting Atanor* Atanorbinmapu::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
    Locking _lock(this);
    Atanor* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    
    basebin_hash<wstring>::iterator it;
    
    Atanor* a;
    for (it=values.begin(); it != values.end(); it++) {
        var->storevalue(it->first);

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
