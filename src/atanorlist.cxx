
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
filename   : atanorlist.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorlist.h"
#include "atanorvector.h"
#include "atanormap.h"
#include "atanorlist.h"
#include "instructions.h"
#include "constobjects.h"
#include "atanorivector.h"
#include "atanorbyte.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, listMethod>  Atanorlist::methods;
Exporting hmap<string, string> Atanorlist::infomethods;
Exporting bin_hash<unsigned long> Atanorlist::exported;

Exporting short Atanorlist::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorlist::AddMethod(AtanorGlobal* global, string name, listMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanorlist::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    Atanorlist::idtype = global->Getid("list");

    Atanorlist::AddMethod(global, "clear", &Atanorlist::MethodClear, P_NONE, "clear(): clear the container.");
    Atanorlist::AddMethod(global, "flatten", &Atanorlist::MethodFlatten, P_NONE, "flatten(): flatten a vector structure.");

    Atanorlist::AddMethod(global, "remove", &Atanorlist::MethodRemove, P_ONE, "remove(e): remove 'e' from the vector.");

    Atanorlist::AddMethod(global, "reverse", &Atanorlist::MethodReverse, P_NONE, "reverse(): reverse a vector.");
    Atanorlist::AddMethod(global, "unique", &Atanorlist::MethodUnique, P_NONE, "unique(): remove duplicate elements.");

    Atanorlist::AddMethod(global, "join", &Atanorlist::MethodJoin, P_ONE, "join(string sep): Produce a string representation for the container.");
    Atanorlist::AddMethod(global, "shuffle", &Atanorlist::MethodShuffle, P_NONE, "shuffle(): shuffle the values in the list.");
    Atanorlist::AddMethod(global, "last", &Atanorlist::MethodLast, P_NONE, "last(): return the last element.");
    Atanorlist::AddMethod(global, "first", &Atanorlist::MethodFirst, P_NONE, "first(): return the first element.");
    Atanorlist::AddMethod(global, "sum", &Atanorlist::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Atanorlist::AddMethod(global, "product", &Atanorlist::MethodProduct, P_NONE, "product(): return the product of elements.");
    Atanorlist::AddMethod(global, "push", &Atanorlist::MethodPush, P_ATLEASTONE, "push(v): Push a value into the list.");
    Atanorlist::AddMethod(global, "pushfirst", &Atanorlist::MethodPushfirst, P_ONE, "pushfirst(v): Push a value into the list in first position.");
    Atanorlist::AddMethod(global, "pop", &Atanorlist::MethodPop, P_NONE | P_ONE, "pop(i): Erase an element from the list at position i.");
    Atanorlist::AddMethod(global, "popfirst", &Atanorlist::MethodPopfirst, P_NONE, "popfirst(i): Erase the first element.");
    Atanorlist::AddMethod(global, "poplast", &Atanorlist::MethodPoplast, P_NONE, "poplast(i): Erase the last element.");
    Atanorlist::AddMethod(global, "merge", &Atanorlist::MethodMerge, P_ONE, "merge(v): Merge v into the list.");
    Atanorlist::AddMethod(global, "insert", &Atanorlist::MethodInsert, P_TWO, "insert(i,v): Insert v at position i.");


    global->newInstance[Atanorlist::idtype] = new Atanorlist(global);
    global->RecordMethods(Atanorlist::idtype, Atanorlist::exported);

    return true;
}

Exporting AtanorIteration* Atanorlist::Newiteration(bool direction) {
    return new AtanorIterationlist(this, direction);
}

Exporting Atanor* Atanorlist::in(Atanor* context, Atanor* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...


    if (context->isBoolean()) {
        Locking _lock(this);
        for (auto& it : values) {
            if (it->same(a) == aTRUE)
                return aTRUE;
        }
        return aFALSE;
    }

    long i = 0;
    if (context->isVectorContainer()) {
        Atanorivector* v = (Atanorivector*)Selectaivector(context);
        Doublelocking _lock(this, v);
        for (auto& it : values) {
            if (it->same(a) == aTRUE)
                v->values.push_back(i);
            i++;
        }
        return v;
    }

    Locking _lock(this);
	for (auto& it : values) {
        if (it->same(a) == aTRUE)
			return globalAtanor->Provideint(i);
        i++;
    }

    return aNULL;
}


Exporting Atanor* Atanorlist::getvalue(BLONG i) {
    Locking _lock(this);
    if (i < 0 || i >= values.size())
        return aNOELEMENT;


    for (auto& it : values) {
        if (!i)
            return (it);
        i--;
    }
    return aNOELEMENT;
}

Exporting void Atanorlist::Setreference(short r) {
    if (idtracker == CIRCULARDETECTION) {
        globalAtanor->Returnerror("Error: Circularity in container", globalAtanor->GetThreadid());
        return;
    }

    int id = idtracker;
    idtracker = CIRCULARDETECTION;

    Locking _lock(this);
    protect = false;
	reference += r;

    for (auto& it : values)
        it->Setreference(r);
    idtracker = id;
}

static void resetList(Atanorlist* klist, long inc) {
    Locking _lock(klist);
    klist->reference -= inc;

    list<Atanor*>& values = klist->values;
    if (values.size() == 0)
        return;
    

	for (auto& itx : values)
        itx->Resetreference(inc);
}

Exporting void Atanorlist::Resetreference(short inc) {
    if ((reference - inc) > 0)
        resetList(this, inc);
    else {
        resetList(this, inc + 1 - protect);
		if (!protect) {
			if (idtracker != -1)
                globalAtanor->RemoveFromTracker(idtracker);
            delete this;
        }
    }
}

Exporting Atanor* Atanorlist::Pushfirst(Atanor* a) {
    Locking _lock(this);
    a = a->Atom();
    values.push_front(a);
    a->Setreference(reference + 1);
    return this;
}

Exporting Atanor* Atanorlist::Push(Atanor* a) {
    Locking _lock(this);
    a = a->Atom();
    values.push_back(a);
    a->Setreference(reference + 1);
    return this;
}

Exporting Atanor* Atanorlist::Pop(Atanor* v) {
    Locking _lock(this);
    if (!values.size())
        return aFALSE;

    list<Atanor*>::iterator it;
    for (it = values.begin(); it != values.end(); ++it) {
        if ((*it)->same(v)) {
            v = *it;
            values.erase(it);
            v->Resetreference(reference + 1);
            return aTRUE;
        }
    }
    return aFALSE;
}

Exporting void Atanorlist::Clear() {
    //To set a variable back to empty
    Locking _lock(this);

    for (auto& it : values) {
        it->Resetreference(reference + 1);
    }
    values.clear();
}

Exporting void Atanorlist::Clean() {
    //Your cleaning code
}

Exporting string Atanorlist::String() {
    Locking _lock(this);
    string res;

    res = "[";
    bool beg = true;
    string sx;
    for (auto& it : values) {
        if (beg == false)
            res += ",";
        beg = false;
        sx = it->StringToDisplay();
        if (!it->isString() || it->isContainer())
            res += sx;
        else
            stringing(res, sx);
    }
    res += "]";
    return res;
}

Exporting string Atanorlist::JSonString() {
    Locking _lock(this);
    string res;

    res = "[";
    bool beg = true;
    string sx;
    for (auto& it : values) {
        if (beg == false)
            res += ",";
        beg = false;
        sx = it->JSonString();
		if (!it->isString() || it->isContainer())
            res += sx;
        else
            jstringing(res, sx);
    }
    res += "]";
    return res;
}



Exporting long Atanorlist::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanorlist::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanorlist::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanorlist::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Atanorlist::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting void Atanorlist::Insert(int idx, Atanor* ke) {
    ke = ke->Atom();
    Locking _lock(this);
    list<Atanor*>::iterator itl = values.begin();
    for (int nb = 0; nb < idx; nb++)
        itl++;
    values.insert(itl, ke);
    ke->Setreference(reference + 1);
}

Exporting bool Atanorlist::Permute() {
    Locking _lock(this);
    return next_permutation(values.begin(), values.end());
}

Exporting void Atanorlist::Shuffle() {
    Locking _lock(this);

    vector<Atanor*> vb;
    for (auto& it : values)
        vb.push_back(it);
    size_t sz = vb.size();
    size_t i, f;
    int mx = sz;
    Atanor* v;
    for (i = 0; i < sz; i++) {
        f = localrandom(mx--);
        if (mx != f) {
            v = vb[mx];
            vb[mx] = vb[f];
            vb[f] = v;
        }
    }

    values.clear();
    for (i = 0; i < sz; i++)
        values.push_back(vb[i]);
}

Exporting Atanor* Atanorlist::Unique() {
    Locking _lock(this);
    Atanorlist* klist = new Atanorlist;

    map<string, Atanor*> inter;
    string k;
    for (auto& it : values) {
        k = it->String();
        if (inter.find(k) != inter.end()) {
            if (inter[k]->same(it)->Boolean() == false)
                klist->Push(it);
        }
        else {
            inter[k] = it;
            klist->Push(it);
        }
    }
    return klist;
}

Exporting void Atanorlist::Popfirst() {
    Locking _lock(this);
    if (values.size() == 0)
        return;
    Atanor* kres = values.front();
    kres->Resetreference(reference + 1);
    values.pop_front();
}

Exporting void Atanorlist::Poplast() {
    Locking _lock(this);
    if (values.size() == 0)
        return;
    Atanor* kres = values.back();
    kres->Resetreference(reference + 1);
    values.pop_back();
}

Exporting Atanor* Atanorlist::Merging(Atanor* ke) {
    if (!ke->isContainer()) {
        Push(ke);
        return this;
    }

    if (ke->Size() == 0)
        return this;

    Doublelocking _lock(this, ke);
    //Three cases:
    if (ke->Type() == a_list) {
        Atanorlist* klist = (Atanorlist*)ke;

        for (auto& it : klist->values)
            Push(it);
        return this;
    }

    AtanorIteration* itr = ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next())
        Push(itr->Value());
    itr->Release();

    return this;
}

Exporting Atanor* Atanorlist::Map(short idthread) {
    Locking _lock(this);
	Atanormap* kmap = globalAtanor->Providemap();


    int nb = 0;
    char ch[20];
    for (auto& it : values) {
        sprintf_s(ch, 20, "%d", nb);
        kmap->Push(ch, it);
        nb++;
    }
    return kmap;
}

Exporting Atanor* Atanorlist::Vector(short idthread) {
    Locking _lock(this);
	Atanorvector* kvect = globalAtanor->Providevector();
    //We copy all values from ke to this
    kvect->values.reserve(values.size());

    for (auto& it : values)
        kvect->Push(it);
    return kvect;
}

Exporting Atanor*  Atanorlist::Put(Atanor* idx, Atanor* ke, short idthread) {
    if (ke == this)
        return aTRUE;

    Doublelocking _lock(this, ke);

    if (!idx->isIndex()) {
        if (ke == aNULL) {
            Clear();
            return aTRUE;
        }

        if (ke->Type() == a_list) {
            Atanorlist* kvect = (Atanorlist*)ke;
            //We copy all values from ke to this
            Clear();

            for (auto& it : kvect->values)
                Push(it);
            return aTRUE;
        }

        if (ke->isVectorContainer()) {
            //We copy all values from ke to this
            Clear();
            for (int it = 0; it < ke->Size(); ++it)
                Push(ke->getvalue(it));
            return aTRUE;
        }
        //We gather all the keys from the MAP
        if (ke->isMapContainer()) {
            Clear();
            AtanorIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                Push(itr->Key());
            itr->Release();
            return aTRUE;
        }
        ke = ke->Vector(idthread);
        if (!ke->isVectorContainer())
            return globalAtanor->Returnerror("Cannot set this value", idthread);

        Clear();
        //We copy all values from ke to this
		for (long it = 0; it < ke->Size(); ++it)
			Push(ke->getvalue(it));
        ke->Release();
        return aTRUE;
    }

    if (idx->isInterval()) {

        //First we remove the values...
        long lkey = ((AtanorIndex*)idx)->left->Integer();
        if (lkey < 0)
            lkey = values.size() + lkey;

        long rkey = ((AtanorIndex*)idx)->right->Integer();
        if (rkey < 0)
            rkey = values.size() + rkey;

        if (rkey < lkey || rkey >= values.size() || lkey >= values.size()) {
            if (globalAtanor->erroronkey)
                globalAtanor->Returnerror("Wrong index", idthread);
            return aTRUE;
        }

        list<Atanor*>::iterator it;
        listValue(it, lkey);
        vector<list<Atanor*>::iterator > vit;
        Atanor* krkey;
        while (rkey > lkey) {
            krkey = *it;
            vit.push_back(it);
            krkey->Resetreference(reference + 1);
            rkey--;
            ++it;
        }

        for (int i = vit.size() - 1; i >= 0; i--)
            values.erase(vit[i]);

        listValue(it, lkey);
        if (ke->isVectorContainer()) {
            for (int i = 0; i < ke->Size(); i++) {
                krkey = ke->getvalue(i);
                krkey = krkey->Atom();
                values.insert(it, krkey);
                krkey->Setreference(reference + 1);
            }
            return aTRUE;
        }

        if (ke->Type() == a_list) {
			for (auto& iti : ((Atanorlist*)ke)->values) {
                krkey = iti;
                krkey = krkey->Atom();
                values.insert(it, krkey);
                krkey->Setreference(reference + 1);
            }
            return aTRUE;
        }

        if (ke->isContainer()) {
            AtanorIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
                krkey = itr->Value();
                krkey = krkey->Atom();
                values.insert(it, krkey);
                krkey->Setreference(reference + 1);
            }
            itr->Release();
            return aTRUE;
        }

        krkey = ke->Atom();
        values.insert(it, krkey);
        krkey->Setreference(reference + 1);
        return aTRUE;
    }

    register int ikey = idx->Integer();
    list<Atanor*>::iterator it;
    int mx = values.size();

    if (ikey == mx)
        Push(ke);
    else {
        if (ikey > mx) {
            for (; mx < ikey; mx++)
                values.push_back(aNULL);
            Push(ke);
        }
        else {
            if (ikey < 0) {
                ikey = mx + ikey;
                if (ikey < 0)
                    return globalAtanor->Returnerror("Cannot set this value", idthread);
            }
            ke = ke->Atom();
            listValue(it, ikey);
            if (*it != NULL)
                (*it)->Resetreference(reference + 1);
            *it = ke;
            ke->Setreference(reference + 1);
        }
    }

    return aTRUE;
}

Exporting Atanor* Atanorlist::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {

        Atanor* ke;
        //In this case, we copy the elements from the vector to the map, using the position as index
        if (contextualpattern->isMapContainer()) {
            Atanormap* map = (Atanormap*)Selectamap(contextualpattern);
            size_t i = 0;

            char ch[20];
            for (auto& it : values) {
                sprintf_s(ch, 20, "%d", i);
                ke = it;
                map->Push(ch, ke);
                i++;
            }
            return map;
        }

        if (contextualpattern->Type() == a_int || contextualpattern->Type() == a_float)
			return globalAtanor->Provideint(values.size());

        //Then some values must be replaced with their evaluation
        if (evaluate == true) {
            Atanorlist* kvect = new Atanorlist;

            for (auto& it : values) {
                ke = it->Get(aNULL, aNULL, idthread);
                if (ke == aRAISEERROR) {
                    kvect->Release();
                    return aRAISEERROR;
                }
                kvect->Push(ke);
            }
            return kvect;
        }

        return this;
    }

	Atanor* keyright = NULL;
	
	long ikey = ((AtanorIndex*)idx)->left->Getinteger(idthread);
	if (idx->isInterval())
		keyright = ((AtanorIndex*)idx)->right->Get(aNULL, aNULL, idthread);	

    list<Atanor*>::iterator it;
    register int i;
    Atanorlist* kvect;
    if (ikey < 0)
        ikey = values.size() + ikey;
    if (ikey < 0 || ikey >= values.size()) {
        if (ikey != values.size() || keyright == NULL) {
            if (globalAtanor->erroronkey)
                return globalAtanor->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    it = values.begin();
    for (i = 0; i < ikey; i++) ++it;


    if (keyright == NULL)
        return *it;

	long iright;

	if (keyright != NULL) {
		iright = keyright->Integer();
		keyright->Release();
	}

	if (iright < 0 || keyright == aNULL) {
        iright = values.size() + iright;
        if (iright<ikey) {
            if (globalAtanor->erroronkey)
                return globalAtanor->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    else {
        if (iright>values.size()) {
            if (globalAtanor->erroronkey)
                return globalAtanor->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    //In this case, we must create a new list
    kvect = new Atanorlist;
    for (; i < iright; i++, ++it)
        kvect->Push(*it);

    return kvect;
}


Exporting Atanor* Atanorlist::andset(Atanor* b, bool itself) {
    Atanorlist* ref;


    Atanor* ke;
    if (!b->isContainer()) {
        if (itself)
            ref = this;
        else
            ref = (Atanorlist*)Atom(true);
        Doublelocking _lock(ref, b);
        for (auto& itl : ref->values) {
            ke = itl->andset(b, true);
            if (ke->isError()) {
                ref->Release();
                return ke;
            }
        }
        return ref;
    }

    ref = new Atanorlist;
    Locking _lock(this);
    AtanorIteration* itr = b->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        for (auto& itl : ref->values) {
            ke = itr->IteratorValue();
            if (itl->same(ke) == aTRUE) {
                ref->Push((itl));
                break;
            }
        }
    }
    itr->Release();
    return ref;

}

Exporting Atanor* Atanorlist::orset(Atanor* b, bool itself) {
    Atanorlist* ref;

    long size = Size();

    Atanor* ke;
    if (itself)
        ref = this;
    else
        ref = (Atanorlist*)Atom(true);
    if (!b->isContainer()) {
        Doublelocking _lock(ref, b);
        for (auto& itl : ref->values) {
            ke = itl->orset(b, true);
            if (ke->isError()) {
                ref->Release();
                return ke;
            }
        }
        return ref;
    }

    ref->Merging(b);
    return ref;
}

Exporting Atanor* Atanorlist::xorset(Atanor* b, bool itself) {
    Atanorlist* ref;


    Atanor* ke;
    if (!b->isContainer()) {
        if (itself)
            ref = this;
        else
            ref = (Atanorlist*)Atom(true);

        Doublelocking _lock(ref, b);
        for (auto& itl : ref->values) {
            ke = itl->xorset(b, true);
            if (ke->isError()) {
                ref->Release();
                return ke;
            }
        }
        return ref;
    }

    ref = new Atanorlist;
    Locking _lock(this);
    AtanorIteration* itr = b->Newiteration(false);
    bool found;
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        found = false;
		for (auto& itl : ref->values) {			
            ke = itr->IteratorValue();
            if (itl->same(ke) == aTRUE) {
                found = true;
                break;
            }
        }

        if (!found) {
            ke = itr->IteratorValue();
            ref->Push(ke);
        }
    }
    itr->Release();
    return ref;
}


Exporting Atanor* Atanorlist::plus(Atanor* b, bool itself) {
    Atanorlist* ref;
    if (itself)
        ref = this;
    else
        ref = (Atanorlist*)Atom(true);


    Atanor* ke;
    Atanor* kv;
    Doublelocking _lock(ref, b);

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (auto& itl : ref->values) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = itl->plus(kv, true);
            if (ke->isError()) {
                ref->Release();
                return ke;
            }
            itr->Next();
        }
        itr->Release();
        return ref;
    }


	for (auto& itl : ref->values) {
        ke = itl->plus(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Atanor* Atanorlist::minus(Atanor* b, bool itself) {
    Atanorlist* ref;
    if (itself)
        ref = this;
    else
        ref = (Atanorlist*)Atom(true);


    Atanor* ke;
    Atanor* kv;

    Doublelocking _lock(ref, b);

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (auto& itl : ref->values) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = itl->minus(kv, true);
            if (ke->isError()) {
                ref->Release();
                return ke;
            }
            itr->Next();
        }
        itr->Release();
        return ref;
    }


	for (auto& itl : ref->values) {
        ke = itl->minus(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Atanor* Atanorlist::multiply(Atanor* b, bool itself) {
    Atanorlist* ref;
    if (itself)
        ref = this;
    else
        ref = (Atanorlist*)Atom(true);


    Atanor* ke;
    Atanor* kv;

    Doublelocking _lock(ref, b);

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (auto& itl : ref->values) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = itl->multiply(kv, true);
            if (ke->isError()) {
                ref->Release();
                return ke;
            }
            itr->Next();
        }
        itr->Release();
        return ref;
    }


	for (auto& itl : ref->values) {
        ke = itl->multiply(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Atanor* Atanorlist::divide(Atanor* b, bool itself) {
    Atanorlist* ref;
    if (itself)
        ref = this;
    else
        ref = (Atanorlist*)Atom(true);


    Atanor* ke;
    Atanor* kv;

    Doublelocking _lock(ref, b);

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (auto& itl : ref->values) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = itl->divide(kv, true);
            if (ke->isError()) {
                ref->Release();
                return ke;
            }
            itr->Next();
        }
        itr->Release();
        return ref;
    }


	for (auto& itl : ref->values) {
        ke = itl->divide(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Atanor* Atanorlist::power(Atanor* b, bool itself) {
    Atanorlist* ref;
    if (itself)
        ref = this;
    else
        ref = (Atanorlist*)Atom(true);


    Atanor* ke;
    Atanor* kv;

    Doublelocking _lock(ref, b);

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (auto& itl : ref->values) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = itl->power(kv, true);
            if (ke->isError()) {
                ref->Release();
                return ke;
            }
            itr->Next();
        }
        itr->Release();
        return ref;
    }


	for (auto& itl : ref->values) {
        ke = itl->power(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Atanor* Atanorlist::shiftleft(Atanor* b, bool itself) {
    Atanorlist* ref;
    if (itself)
        ref = this;
    else
        ref = (Atanorlist*)Atom(true);


    Atanor* ke;
    Atanor* kv;

    Doublelocking _lock(ref, b);

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (auto& itl : ref->values) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = itl->shiftleft(kv, true);
            if (ke->isError()) {
                ref->Release();
                return ke;
            }
            itr->Next();
        }
        itr->Release();
        return ref;
    }


	for (auto& itl : ref->values) {
        ke = itl->shiftleft(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Atanor* Atanorlist::shiftright(Atanor* b, bool itself) {
    Atanorlist* ref;
    if (itself)
        ref = this;
    else
        ref = (Atanorlist*)Atom(true);


    Atanor* kv;

    Atanor* ke;
    Doublelocking _lock(ref, b);

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (auto& itl : ref->values) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = itl->shiftright(kv, true);
            if (ke->isError()) {
                ref->Release();
                return ke;
            }
            itr->Next();
        }
        itr->Release();
        return ref;
    }


	for (auto& itl : ref->values) {
        ke = itl->shiftright(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Atanor* Atanorlist::mod(Atanor* b, bool itself) {
    Atanorlist* ref;
    if (itself)
        ref = this;
    else
        ref = (Atanorlist*)Atom(true);


    Atanor* ke;
    Atanor* kv;

    Doublelocking _lock(ref, b);

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (auto& itl : ref->values) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = itl->mod(kv, true);
            if (ke->isError()) {
                ref->Release();
                return ke;
            }
            itr->Next();
        }
        itr->Release();
        return ref;
    }


	for (auto& itl : ref->values) {
        ke = itl->mod(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}



Exporting Atanor* Atanorlist::same(Atanor* a) {

    if (a->Type() != a_list)
        return aFALSE;
    Atanorlist* v = (Atanorlist*)a;
    Doublelocking _lock(this, v);

    if (v->values.size() != values.size())
        return aFALSE;

    list<Atanor*>::iterator it;
    list<Atanor*>::iterator itv;
    for (it = values.begin(), itv = v->values.begin(); it != values.end(); ++it, ++itv) {
        if ((*it)->same(*itv) == aFALSE)
            return aFALSE;
    }
    return aTRUE;
}
//----------------------------------------------------------------------------------
Exporting void Atanorlist::storevalue(string u) {
	Atanor* a = globalAtanor->Providestring(u);
	a->Setreference(reference + 1);
	Locking _lock(this);
	values.push_back(a);
}

Exporting void Atanorlist::storevalue(wstring u) {
	Atanor* a = globalAtanor->Provideustring(u);
	a->Setreference(reference + 1);
	Locking _lock(this);
	values.push_back(a);
}

Exporting void Atanorlist::storevalue(long u) {
	Atanor* a = globalAtanor->Provideint(u);
	a->Setreference(reference + 1);
	Locking _lock(this);
	values.push_back(a);
}

Exporting void Atanorlist::storevalue(short u) {
	Atanor* a = new Atanorshort(u);
	a->Setreference(reference + 1);
	Locking _lock(this);
	values.push_back(a);
}

Exporting void Atanorlist::storevalue(BLONG u) {
	Atanor* a = new Atanorlong(u);
	a->Setreference(reference + 1);
	Locking _lock(this);
	values.push_back(a);
}

Exporting void Atanorlist::storevalue(float u) {
	Atanor* a = new Atanordecimal(u);
	a->Setreference(reference + 1);
	Locking _lock(this);
	values.push_back(a);
}

Exporting void Atanorlist::storevalue(double u) {
	Atanor* a = globalAtanor->Providefloat(u);
	a->Setreference(reference + 1);
	Locking _lock(this);
	values.push_back(a);
}

Exporting void Atanorlist::storevalue(unsigned char u) {
	Atanor* a = new Atanorbyte(u);
	a->Setreference(reference + 1);
	Locking _lock(this);
	values.push_back(a);
}

Exporting void Atanorlist::storevalue(wchar_t u) {
	wstring w;
	w = u;
	Atanor* a = globalAtanor->Provideustring(w);
	a->Setreference(reference + 1);
	Locking _lock(this);
	values.push_back(a);
}



