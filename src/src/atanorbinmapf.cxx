
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
filename   : atanorbinmapf.cxx
Date       : 2017/04/01
Purpose    : map implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"

#include "atanorint.h"
//#prime
#include "atanorbinmapf.h"
#include "atanorlist.h"
#include "atanorivector.h"


//We need to declare once again our local definitions.
Exporting hmap<unsigned short, binmapfMethod>  Atanorbinmapf::methods;
Exporting hmap<string, string> Atanorbinmapf::infomethods;
Exporting bin_hash<unsigned long> Atanorbinmapf::exported;

Exporting short Atanorbinmapf::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorbinmapf::AddMethod(AtanorGlobal* global, string name, binmapfMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanorbinmapf::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();


    Atanorbinmapf::idtype = global->Getid("binmapf");

    Atanorbinmapf::AddMethod(global, "clear", &Atanorbinmapf::MethodClear, P_NONE, "clear(): clear the container.");
    
    Atanorbinmapf::AddMethod(global, "invert", &Atanorbinmapf::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Atanorbinmapf::AddMethod(global, "find", &Atanorbinmapf::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Atanorbinmapf::AddMethod(global, "items", &Atanorbinmapf::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Atanorbinmapf::AddMethod(global, "join", &Atanorbinmapf::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Atanorbinmapf::AddMethod(global, "test", &Atanorbinmapf::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Atanorbinmapf::AddMethod(global, "keys", &Atanorbinmapf::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Atanorbinmapf::AddMethod(global, "values", &Atanorbinmapf::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Atanorbinmapf::AddMethod(global, "sum", &Atanorbinmapf::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Atanorbinmapf::AddMethod(global, "product", &Atanorbinmapf::MethodProduct, P_NONE, "product(): return the product of elements.");
    Atanorbinmapf::AddMethod(global, "pop", &Atanorbinmapf::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Atanorbinmapf::AddMethod(global, "merge", &Atanorbinmapf::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Atanorbinmapf::idtype] = new Atanorbinmapf(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("ifprimemap")] = new Atanorbinmapf(global);

    global->RecordMethods(global->Getid("ifprimemap"), Atanorbinmapf::exported);
    #endif
    global->RecordMethods(Atanorbinmapf::idtype, Atanorbinmapf::exported);

    return true;
}


Exporting AtanorIteration* Atanorbinmapf::Newiteration(bool direction) {
    return new AtanorIterationbinmapf(this, direction);
}


Exporting Atanor* Atanorbinmapf::in(Atanor* context, Atanor* a, short idthread) {
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

Exporting Atanor* Atanorbinmapf::MethodFind(Atanor* context, short idthread, AtanorCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    basebin_hash<double>::iterator it;

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



Exporting Atanor* Atanorbinmapf::Push(Atanor* k, Atanor* v) {
    Locking _lock(this);
    ushort s = k->Short();
    values[s] = v->Float();
    return aTRUE;
}

Exporting Atanor* Atanorbinmapf::Pop(Atanor* kkey) {
    ushort k = kkey->Short();
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Atanorbinmapf::Clear() {
    Locking _lock(this);
    values.clear();
}

Exporting void Atanorbinmapf::Clean() {
    //Your cleaning code
}

Exporting string Atanorbinmapf::String() {
    Locking _lock(this);
    stringstream res;
    basebin_hash<double>::iterator it;
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

Exporting string Atanorbinmapf::JSonString() {
    Locking _lock(this);
    stringstream res;
    basebin_hash<double>::iterator it;
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


Exporting long Atanorbinmapf::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanorbinmapf::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanorbinmapf::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanorbinmapf::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Atanorbinmapf::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Atanor*  Atanorbinmapf::Put(Atanor* idx, Atanor* ke, short idthread) {
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
            values = ((Atanorbinmapf*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            values.clear();
            AtanorIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keyshort()] = itr->Valuefloat();
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
            if (ke->Type() == Atanorbinmapf::idtype) {
                Atanorbinmapf* kmap = (Atanorbinmapf*)ke;
                //We copy all values from ke to this
                basebin_hash<double>::iterator it;
                for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                    values[it->first] = it->second;
                kmap->Release();
            }
            else {
                AtanorIteration* itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                    values[itr->Keyshort()] = itr->Valuefloat();
                itr->Release();
            }
            ke->Release();
        }
        return aTRUE;
    }

    values[idx->Short()] = ke->Float();
    return aTRUE;
}

Exporting Atanor* Atanorbinmapf::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Atanor* vect = contextualpattern->Newinstance(idthread);
            basebin_hash<double>::iterator it;
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
        Atanorbinmapf* kmap = new Atanorbinmapf;
        key = ((AtanorIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Atanor* keyright = ((AtanorIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        ushort vleft = key->Short();
        ushort vright = keyright->Short();
        basebin_hash<double>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        basebin_hash<double>::iterator itr = values.end();
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

Exporting Atanor* Atanorbinmapf::same(Atanor* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Atanorbinmapf* m = (Atanorbinmapf*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    basebin_hash<double>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Atanor* Atanorbinmapf::xorset(Atanor* b, bool itself) {

    Doublelocking _lock(this, b);


    Atanorbinmapf* res;
    
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmapf;
        hmap<short, double> keys;

        basebin_hash<double>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;

        short v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyfloat();
            if (keys.find(v) == keys.end())
                keys[v] = itr->Valuefloat();
            else {
                if (values[v] == itr->Valuefloat())
                    keys.erase(v);
            }
        }
        itr->Release();

        for (auto& a : keys)
            res->values[a.first] = a.second;
        return res;
    }


    if (itself)
        res = this;
    else
        res = (Atanorbinmapf*)Atom(true);

    basebin_hash<double>::iterator it;
    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second ^ v;
    return res;
}

Exporting Atanor* Atanorbinmapf::orset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<double>::iterator it;
    Atanorbinmapf* res;
    if (itself)
        res = this;
    else
        res = (Atanorbinmapf*)Atom(true);

    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second | v;
    return res;

}

Exporting Atanor* Atanorbinmapf::andset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<double>::iterator it;
    Atanorbinmapf* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmapf;
        double v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            ikey = itr->Keyshort();
            if (values.check(ikey) && values[ikey] == v)
                res->values[ikey] = v;
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorbinmapf*)Atom(true);

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second & v;
    return res;
}

Exporting Atanor* Atanorbinmapf::plus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<double>::iterator it;
    Atanorbinmapf* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmapf;
        double v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
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
        res = (Atanorbinmapf*)Atom(true);

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second += v;
    return res;
}

Exporting Atanor* Atanorbinmapf::minus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<double>::iterator it;
    Atanorbinmapf * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmapf;
        double v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->values[ikey] = values[ikey] - v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorbinmapf*)Atom(true);

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second -= v;
    return res;
}

Exporting Atanor* Atanorbinmapf::multiply(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<double>::iterator it;
    Atanorbinmapf * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmapf;
        double v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->values[ikey] = values[ikey] * v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorbinmapf*)Atom(true);

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second *= v;
    return res;

}

Exporting Atanor* Atanorbinmapf::divide(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<double>::iterator it;
    Atanorbinmapf * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmapf;
        double v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            if (v == 0) {
                res->Release();
                return new AtanorError("Error: Divided by 0");
            }

            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->values[ikey] = values[ikey] / v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorbinmapf*)Atom(true);

    double v = b->Float();
    if (v == 0) {
        res->Release();
        return new AtanorError("Error: Divided by 0");
    }
    for (it = values.begin(); it != values.end(); it++)
        it->second /= v;
    return res;

}

Exporting Atanor* Atanorbinmapf::mod(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<double>::iterator it;
    Atanorbinmapf * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmapf;
        long v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            if (v == 0) {
                res->Release();
                return new AtanorError("Error: Divided by 0");
            }

            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->values[ikey] = (long)values[ikey] % v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorbinmapf*)Atom(true);

    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return new AtanorError("Error: Divided by 0");
    }
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second % v;
    return res;
}

Exporting Atanor* Atanorbinmapf::shiftright(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<double>::iterator it;
    Atanorbinmapf * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmapf;
        long v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->values[ikey] = (long)values[ikey] >> v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorbinmapf*)Atom(true);

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second >> v;
    return res;
}

Exporting Atanor* Atanorbinmapf::shiftleft(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<double>::iterator it;
    Atanorbinmapf * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmapf;
        long v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->values[ikey] = (long)values[ikey] << v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorbinmapf*)Atom(true);

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second << v;
    return res;
}

Exporting Atanor* Atanorbinmapf::power(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<double>::iterator it;
    Atanorbinmapf * res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = new Atanorbinmapf;
        double v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->values[ikey] = pow(values[ikey], v);
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanorbinmapf*)Atom(true);

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second = pow(it->second, v);
    return res;
}




Exporting Atanor* Atanorbinmapf::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
    Locking _lock(this);
    Atanor* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    
    basebin_hash<double>::iterator it;
    
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
