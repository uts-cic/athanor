
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
filename   : atanorbinmaps.cxx
Date       : 2017/04/01
Purpose    : map implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"

#include "atanorint.h"
//#prime
#include "atanorbinmaps.h"
#include "atanorlist.h"
#include "atanorivector.h"


//We need to declare once again our local definitions.
Exporting hmap<unsigned short, binmapsMethod>  Atanorbinmaps::methods;
Exporting hmap<string, string> Atanorbinmaps::infomethods;
Exporting bin_hash<unsigned long> Atanorbinmaps::exported;

Exporting short Atanorbinmaps::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorbinmaps::AddMethod(AtanorGlobal* global, string name,binmapsMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanorbinmaps::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Atanorbinmaps::idtype = global->Getid("binmaps");

    Atanorbinmaps::AddMethod(global, "clear", &Atanorbinmaps::MethodClear, P_NONE, "clear(): clear the container.");
    
    Atanorbinmaps::AddMethod(global, "invert", &Atanorbinmaps::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Atanorbinmaps::AddMethod(global, "find", &Atanorbinmaps::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Atanorbinmaps::AddMethod(global, "items", &Atanorbinmaps::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Atanorbinmaps::AddMethod(global, "join", &Atanorbinmaps::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Atanorbinmaps::AddMethod(global, "test", &Atanorbinmaps::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Atanorbinmaps::AddMethod(global, "keys", &Atanorbinmaps::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Atanorbinmaps::AddMethod(global, "values", &Atanorbinmaps::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Atanorbinmaps::AddMethod(global, "pop", &Atanorbinmaps::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Atanorbinmaps::AddMethod(global, "merge", &Atanorbinmaps::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Atanorbinmaps::idtype] = new Atanorbinmaps(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("isprimemap")] = new Atanorbinmaps(global);

    global->RecordMethods(global->Getid("isprimemap"), Atanorbinmaps::exported);
    #endif
    global->RecordMethods(Atanorbinmaps::idtype, Atanorbinmaps::exported);

    return true;
}


Exporting AtanorIteration* Atanorbinmaps::Newiteration(bool direction) {
    return new AtanorIterationbinmaps(this, direction);
}


Exporting Atanor* Atanorbinmaps::in(Atanor* context, Atanor* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    ushort val = a->Short();

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

Exporting Atanor* Atanorbinmaps::MethodFind(Atanor* context, short idthread, AtanorCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    basebin_hash<string>::iterator it;
    
    string val = callfunc->Evaluate(0, context, idthread)->String();
    
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



Exporting Atanor* Atanorbinmaps::Push(Atanor* k, Atanor* v) {
    Locking _lock(this);
    ushort s = k->Short();
    values[s] = v->String();
    return aTRUE;
}

Exporting Atanor* Atanorbinmaps::Pop(Atanor* kkey) {
    ushort k = kkey->Short();
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Atanorbinmaps::Clear() {
    Locking _lock(this);
    values.clear();
}

Exporting void Atanorbinmaps::Clean() {
    //Your cleaning code
}

Exporting string Atanorbinmaps::String() {
    Locking _lock(this);
    stringstream res;
    basebin_hash<string>::iterator it;
    res << "{";
    bool beg = true;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it->first << ":";
        stringing(res, it->second);
    }
    res << "}";
    return res.str();
}

Exporting string Atanorbinmaps::JSonString() {
    Locking _lock(this);
    stringstream res;
    basebin_hash<string>::iterator it;
    res << "{";
    bool beg = true;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        res << '"' << it->first << '"' << ":";
        jstringing(res, it->second);
    }
    res << "}";
    return res.str();
}


Exporting long Atanorbinmaps::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanorbinmaps::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanorbinmaps::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanorbinmaps::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Atanorbinmaps::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Atanor*  Atanorbinmaps::Put(Atanor* idx, Atanor* ke, short idthread) {
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
            values = ((Atanorbinmaps*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            values.clear();
            AtanorIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keyshort()] = itr->Valuestring();
            itr->Release();
            return aTRUE;
        }
        
        if (ke->isVectorContainer()) {
            values.clear();
            nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                values[nb] = ke->getstring(it);
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
                values[nb] = (*it)->String();
                nb++;
            }
            return aTRUE;
        }

        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalAtanor->Returnerror("Wrong map initialization", idthread);

        if (ke != this) {
            values.clear();
            if (ke->Type() == Atanorbinmaps::idtype) {
                Atanorbinmaps* kmap = (Atanorbinmaps*)ke;
                //We copy all values from ke to this
                basebin_hash<string>::iterator it;
                for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                    values[it->first] = it->second;
                kmap->Release();
            }
            else {
                AtanorIteration* itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                    values[itr->Keyshort()] = itr->Valuestring();
                itr->Release();
            }
            ke->Release();
        }
        return aTRUE;
    }

    values[idx->Short()] = ke->String();
    return aTRUE;
}

Exporting Atanor* Atanorbinmaps::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Atanor* vect = contextualpattern->Newinstance(idthread);
            basebin_hash<string>::iterator it;
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
        Atanorbinmaps* kmap = new Atanorbinmaps;
        key = ((AtanorIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Atanor* keyright = ((AtanorIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        ushort vleft = key->Short();
        ushort vright = keyright->Short();
        basebin_hash<string>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        basebin_hash<string>::iterator itr= values.end();
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

Exporting Atanor* Atanorbinmaps::same(Atanor* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Atanorbinmaps* m = (Atanorbinmaps*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    basebin_hash<string>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Atanor* Atanorbinmaps::xorset(Atanor* b, bool itself) {

    Doublelocking _lock(this, b);


    Atanorbinmaps* res;
    
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmaps;
        hmap<short, string> keys;

        basebin_hash<string>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;

        short v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyshort();
            if (keys.find(v) == keys.end())
                keys[v] = itr->Valuestring();
            else {
                if (values[v] == itr->Valuestring())
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

Exporting Atanor* Atanorbinmaps::orset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<string>::iterator it;
    Atanorbinmaps* res;
    if (itself)
        res = this;
    else
        res = (Atanorbinmaps*)Atom(true);

    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }

    return aNULL;
}

Exporting Atanor* Atanorbinmaps::andset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<string>::iterator it;
    Atanorbinmaps* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmaps;
        string v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            ikey = itr->Keyshort();
            if (values.check(ikey) && values[ikey] == v)
                res->values[ikey] = v;
        }
        itr->Release();
        return res;
    }

    return aNULL;
}

Exporting Atanor* Atanorbinmaps::plus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<string>::iterator it;
    Atanorbinmaps* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmaps;
        string v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
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
        res = (Atanorbinmaps*)Atom(true);

    string v = b->String();
    for (it = values.begin(); it != values.end(); it++)
        it->second += v;
    return res;
}

Exporting Atanor* Atanorbinmaps::minus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<string>::iterator it;
    Atanorbinmaps * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmaps;
        string v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
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
        res = (Atanorbinmaps*)Atom(true);

    string v = b->String();
    for (it = values.begin(); it != values.end(); it++)
        it->second = StringMinus(it->second,v);
    return res;
}




Exporting Atanor* Atanorbinmaps::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
    Locking _lock(this);
    Atanor* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    
    basebin_hash<string>::iterator it;
    
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
