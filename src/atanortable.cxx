
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
filename   : Atanortable.cxx
Date       : 2017/04/01
Purpose    : vector implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorhaskell.h"
#include "atanortable.h"
#include "atanorvector.h"
#include "atanorivector.h"
#include "atanormap.h"
#include "atanorlist.h"
#include "instructions.h"
#include "constobjects.h"
#include "atanoruvector.h"
#include "atanorfvector.h"
#include "atanorbyte.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, tableMethod>  Atanortable::methods;
Exporting hmap<string, string> Atanortable::infomethods;
Exporting bin_hash<unsigned long> Atanortable::exported;

Exporting short Atanortable::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanortable::AddMethod(AtanorGlobal* global, string name, tableMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanortable::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();


    Atanortable::idtype = global->Getid("table");

    Atanortable::AddMethod(global, "clear", &Atanortable::MethodClear, P_NONE, "clear(): clear the container.");
    Atanortable::AddMethod(global, "flatten", &Atanortable::MethodFlatten, P_NONE, "flatten(): flatten a table structure.");

    Atanortable::AddMethod(global, "remove", &Atanortable::MethodRemove, P_ONE, "remove(e): remove 'e' from the table.");

    Atanortable::AddMethod(global, "reverse", &Atanortable::MethodReverse, P_NONE, "reverse(): reverse a table.");
    Atanortable::AddMethod(global, "unique", &Atanortable::MethodUnique, P_NONE, "unique(): remove duplicate elements.");
	Atanortable::AddMethod(global, "_initial", &Atanortable::MethodReserve, P_ONE, "_initial(int sz): Reserve a size of 'sz' potential element in the table.");
	Atanortable::AddMethod(global, "reserve", &Atanortable::MethodReserve, P_ONE, "reserve(int sz): Reserve a size of 'sz' potential element in the table.");
	Atanortable::AddMethod(global, "resize", &Atanortable::MethodReserve, P_ONE, "resize(int sz): Resize a table.");
	Atanortable::AddMethod(global, "join", &Atanortable::MethodJoin, P_ONE, "join(string sep): Produce a string representation for the container.");
    Atanortable::AddMethod(global, "shuffle", &Atanortable::MethodShuffle, P_NONE, "shuffle(): shuffle the values in the table.");
    Atanortable::AddMethod(global, "last", &Atanortable::MethodLast, P_NONE, "last(): return the last element.");
    Atanortable::AddMethod(global, "sum", &Atanortable::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Atanortable::AddMethod(global, "product", &Atanortable::MethodProduct, P_NONE, "product(): return the product of elements.");
    Atanortable::AddMethod(global, "push", &Atanortable::MethodPush, P_ATLEASTONE, "push(v): Push a value into the table.");
    Atanortable::AddMethod(global, "pop", &Atanortable::MethodPop, P_NONE | P_ONE, "pop(i): Erase an element from the table");
    Atanortable::AddMethod(global, "merge", &Atanortable::MethodMerge, P_ONE, "merge(v): Merge v into the table.");
    Atanortable::AddMethod(global, "editdistance", &Atanortable::MethodEditDistance, P_ONE, "editdistance(v): Compute the edit distance with table 'v'.");
    Atanortable::AddMethod(global, "insert", &Atanortable::MethodInsert, P_TWO, "insert(i,v): Insert v at position i.");

    global->newInstance[Atanortable::idtype] = new Atanortable(global);
    global->RecordMethods(Atanortable::idtype, Atanortable::exported);

    return true;
}

Exporting AtanorIteration* Atanortable::Newiteration(bool direction) {
    return new AtanorIterationtable(this, direction);
}

Exporting void Atanortable::Insert(int idx, Atanor* ke) {
	if (idx < 0 || (idx + 1) >= size) {
		globalAtanor->Returnerror("table is full or wrong index", globalAtanor->GetThreadid());
		return;
	}

    ke = ke->Atom();
    ke->Setreference(reference + 1);
	if (values[idx] == aNOELEMENT) {
		values[idx] = ke;
		return;
	}

	for (long i = size - 1; i >= idx; i++)
		values[i] = values[i - 1];

	values[idx] = ke;
}

Exporting void Atanortable::Setreference(short r) {
    if (idtracker == CIRCULARDETECTION) {
        globalAtanor->Returnerror("Error: Circularity in container", globalAtanor->GetThreadid());
        return;
    }

    int id = idtracker;
    idtracker = CIRCULARDETECTION;

    
    protect = false;
    reference += r;
    for (size_t i = 0; i < size; i++)
        values[i]->Setreference(r);

    idtracker = id;
}

static void resetTable(Atanortable* kvect, long inc) {
    Locking _lock(kvect);
    kvect->reference -= inc;

    if (kvect->size == 0)
        return;

    for (int itx = 0; itx < kvect->size; itx++)
        kvect->values[itx]->Resetreference(inc);
}

Exporting void Atanortable::Resetreference(short inc) {
    if ((reference - inc) > 0)
		resetTable(this, inc);
    else {
		resetTable(this, inc + 1 - protect);
		if (!protect) {
			if (idtracker != -1)
                globalAtanor->RemoveFromTracker(idtracker);
            delete this;
        }
    }
}


Exporting Atanor* Atanortable::in(Atanor* context, Atanor* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...


    if (context->isBoolean()) {
        for (size_t i = 0; i < size; i++) {
            if (a->same(values[i]) == aTRUE)
                return aTRUE;
        }
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Atanorivector* v = (Atanorivector*)Selectaivector(context);
        Locking _lock(v);
        for (size_t i = 0; i < size; i++) {
            if (a->same(values[i]) == aTRUE)
                v->values.push_back(i);
        }
        return v;
    }

    
    for (size_t i = 0; i < size; i++) {
        if (a->same(values[i]) == aTRUE)
            return globalAtanor->Provideint(i);
    }

    return aNULL;
}

Exporting Atanor* Atanortable::Push(Atanor* a) {
	if (push(a) == aNOELEMENT)
		return globalAtanor->Returnerror("Could not push this value into the table", globalAtanor->GetaThreadId());

    return aTRUE;
}

Exporting Atanor* Atanortable::Push(AtanorGlobal* g, Atanor* a, short idhtread) {
	if (push(a) == aNOELEMENT)
		return globalAtanor->Returnerror("Could not push this value into the table", globalAtanor->GetaThreadId());
	return aTRUE;
}

Exporting Atanor* Atanortable::Pop(Atanor* idx) {
    
    if (!size)
        return aFALSE;

    BLONG v = idx->Integer();

	long i;
    if (v == -1) {
		idx = Last(i);
		if (idx != aNOELEMENT)
			values[i] = aNOELEMENT;
		else
			return aFALSE;
    }
    else {
        if (v < 0 || v >= (BLONG)size)
            return aFALSE;
        idx = values[v];
		values[v] = aNOELEMENT;
    }

    idx->Resetreference(reference + 1);
    return aTRUE;
}

Exporting void Atanortable::Clean() {
    //Your cleaning code
}

Exporting string Atanortable::String() {
    
    string res;
    int it;
    res = "[";
    bool beg = true;
    string sx;
    Atanor* element;
    for (it = 0; it < size; it++) {
        element = values[it];
        sx = element->StringToDisplay();
		if (!element->isString() || element->isContainer()) {
            if (sx == "")
                sx = "''";
            if (beg == false) {
                if (sx[0] != '|')
                    res += ",";
            }
            res += sx;
        }
        else {
            if (beg == false)
                res += ",";
            stringing(res, sx);
        }
        beg = false;

    }
    res += "]";
    return res;
}

Exporting Atanor* Atanortable::Map(short idthread) {
    
	Atanormap* kmap = globalAtanor->Providemap();
    char ch[20];
    for (int it = 0; it < size; it++) {
        sprintf_s(ch, 20, "%d", it);
        kmap->Push(ch, values[it]);
    }
    return kmap;
}

Exporting string Atanortable::JSonString() {
    
    string res;
    int it;
    res = "[";
    bool beg = true;
    string sx;
    Atanor* element;
    for (it = 0; it < size; it++) {
        element = values[it];
        sx = element->JSonString();
		if (!element->isString() || element->isContainer()) {
            if (beg == false) {
                if (sx[0] != '|')
                    res += ",";
            }
        }
        else {
            if (beg == false)
                res += ",";
        }
        res += sx;
        beg = false;
    }
    res += "]";
    return res;
}

Exporting long Atanortable::Integer() {
    
    return size;
}

Exporting double Atanortable::Float() {
    
    return size;
}

Exporting BLONG Atanortable::Long() {
    
    return size;
}

Exporting bool Atanortable::Boolean() {
	if (!size)
		return false;
	return true;
}


//Basic operations
Exporting long Atanortable::Size() {
    
    return size;
}

Exporting Atanor* Atanortable::andset(Atanor *b, bool itself) {
    Atanortable* ref;

    long size = Size();
    size_t it;
    Atanor* ke;
    if (!b->isContainer()) {
        if (itself)
            ref = this;
        else
            ref = (Atanortable*)Atom(true);

        

        for (it = 0; it < size; it++) {
            ke = ref->values[it]->andset(b, true);
            if (ke->isError()) {
                ref->Release();
                return ke;
            }
        }
        return ref;
    }

    ref = new Atanortable(b->Size());
	Locking((AtanorObject*)b);

    AtanorIteration* itr = b->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        for (it = 0; it < size; it++) {
            ke = itr->IteratorValue();
            if (values[it]->same(ke) == aTRUE) {
                ref->Push(ke);
                break;
            }
        }
    }
    itr->Release();
    return ref;

}

Exporting Atanor* Atanortable::orset(Atanor *b, bool itself) {
    Atanortable* ref;

    long size = Size();
    size_t it;
    Atanor* ke;
    if (itself)
        ref = this;
    else
        ref = (Atanortable*)Atom(true);

    if (!b->isContainer()) {
        

        for (it = 0; it < size; it++) {
            ke = ref->values[it]->orset(b, true);
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

Exporting Atanor* Atanortable::xorset(Atanor *b, bool itself) {
    Atanortable* ref;

    long size = Size();
    size_t it;
    Atanor* ke;
    if (!b->isContainer()) {
        if (itself)
            ref = this;
        else
            ref = (Atanortable*)Atom(true);

		for (it = 0; it < size; it++) {
            ke = ref->values[it]->xorset(b, true);
            if (ke->isError()) {
                ref->Release();
                return ke;
            }
        }
        return ref;
    }

	ref = new Atanortable(b->Size());
    Locking((AtanorObject*)b);

    AtanorIteration* itr = b->Newiteration(false);
    bool found;
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        found = false;
        for (it = 0; it < size; it++) {
            ke = itr->IteratorValue();
            if (values[it]->same(ke) == aTRUE) {
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


Exporting Atanor* Atanortable::plus(Atanor* b, bool itself) {
    Atanortable* ref;
    if (itself)
        ref = this;
    else
        ref = (Atanortable*)Atom(true);

    int it;
    Atanor* ke;
    Atanor* kv;
    Doublelocking _lock(ref, b);
    

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = ref->values[it]->plus(kv, true);
            if (ke->isError()) {

                ref->Release();
                return ke;
            }

            itr->Next();
        }
        itr->Release();
        return ref;
    }


    for (it = 0; it < size; it++) {
        ke = ref->values[it]->plus(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Atanor* Atanortable::minus(Atanor *b, bool itself) {
    Atanortable* ref;
    if (itself)
        ref = this;
    else
        ref = (Atanortable*)Atom(true);

    int it;
    Atanor* ke;
    Atanor* kv;

    
    

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = ref->values[it]->minus(kv, true);
            if (ke->isError()) {

                ref->Release();
                return ke;
            }

            itr->Next();
        }
        itr->Release();
        return ref;
    }


    for (it = 0; it < size; it++) {
        ke = ref->values[it]->minus(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Atanor* Atanortable::multiply(Atanor *b, bool itself) {
    Atanortable* ref;
    if (itself)
        ref = this;
    else
        ref = (Atanortable*)Atom(true);

    int it;
    Atanor* ke;
    Atanor* kv;

    
    

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = ref->values[it]->multiply(kv, true);
            if (ke->isError()) {
                ref->Release();

                return ke;
            }

            itr->Next();
        }
        itr->Release();
        return ref;
    }


    for (it = 0; it < size; it++) {
        ke = ref->values[it]->multiply(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Atanor* Atanortable::divide(Atanor *b, bool itself) {
    Atanortable* ref;
    if (itself)
        ref = this;
    else
        ref = (Atanortable*)Atom(true);

    int it;
    Atanor* ke;
    Atanor* kv;

    
    

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = ref->values[it]->divide(kv, true);
            if (ke->isError()) {
                ref->Release();

                return ke;
            }

            itr->Next();
        }
        itr->Release();
        return ref;
    }


    for (it = 0; it < size; it++) {
        ke = ref->values[it]->divide(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Atanor* Atanortable::power(Atanor *b, bool itself) {
    Atanortable* ref;
    if (itself)
        ref = this;
    else
        ref = (Atanortable*)Atom(true);

    int it;
    Atanor* ke;
    Atanor* kv;

    
    

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = ref->values[it]->power(kv, true);
            if (ke->isError()) {
                ref->Release();

                return ke;
            }

            itr->Next();
        }
        itr->Release();
        return ref;
    }


    for (it = 0; it < size; it++) {
        ke = ref->values[it]->power(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Atanor* Atanortable::shiftleft(Atanor *b, bool itself) {
    Atanortable* ref;
    if (itself)
        ref = this;
    else
        ref = (Atanortable*)Atom(true);

    int it;
    Atanor* ke;
    Atanor* kv;

    
    

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = ref->values[it]->shiftleft(kv, true);
            if (ke->isError()) {

                ref->Release();
                return ke;
            }

            itr->Next();
        }
        itr->Release();
        return ref;
    }


    for (it = 0; it < size; it++) {
        ke = ref->values[it]->shiftleft(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Atanor* Atanortable::shiftright(Atanor *b, bool itself) {
    Atanortable* ref;
    if (itself)
        ref = this;
    else
        ref = (Atanortable*)Atom(true);

    int it;
    Atanor* kv;

    Atanor* ke;
    
    

    if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = ref->values[it]->shiftright(kv, true);
            if (ke->isError()) {

                ref->Release();
                return ke;
            }

            itr->Next();
        }
        itr->Release();
        return ref;
    }


    for (it = 0; it < size; it++) {
        ke = ref->values[it]->shiftright(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Atanor* Atanortable::mod(Atanor *b, bool itself) {
    Atanortable* ref;
    if (itself)
        ref = this;
    else
        ref = (Atanortable*)Atom(true);

    int it;
    Atanor* ke;
    Atanor* kv;

	if (b->isContainer()) {
        AtanorIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = ref->values[it]->mod(kv, true);
            if (ke->isError()) {
                ref->Release();

                return ke;
            }

            itr->Next();
        }
        itr->Release();
        return ref;
    }


    for (it = 0; it < size; it++) {
        ke = ref->values[it]->mod(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Atanor*  Atanortable::Put(Atanor* idx, Atanor* value, short idthread) {
    if (value == this)
        return aTRUE;
    Doublelocking _lock(this, value);
    if (!idx->isIndex()) {
        if (value == aNULL) {
            Clear();
            return aTRUE;
        }
        if (value->Type() == a_list) {
            Atanorlist* kvect = (Atanorlist*)value;
            //We copy all values from value to this
            Clear();
			Resize(value->Size());				
            {
				long i = 0;
                for (auto& it : kvect->values)
					Pushing(it, i++);
            }
            return aTRUE;
        }
        if (value->isVectorContainer()) {
            //We copy all values from value to this
            Clear();
            long sz = value->Size();
            Resize(sz);
            for (long it = 0; it < sz; it++)
				Pushing(value->getvalue(it), it);
            return aTRUE;
        }
        //We gather all the keys from the MAP
        if (value->isMapContainer()) {
            Clear();
            Resize(value->Size());
			long i = 0;
            AtanorIteration* itr = value->Newiteration(false);
			for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
				if (!Pushing(itr->Key(), i++))
					break;
			}
            itr->Release();
            return aTRUE;
		}
		
		value = value->Vector(idthread);
		if (!value->isVectorContainer())
			return globalAtanor->Returnerror("Cannot set this value", idthread);

		Clear();
		Resize(value->Size());
		//We copy all values from ke to this
		for (long it = 0; it < value->Size(); ++it) {
			if (!Pushing(value->getvalue(it), it)) {
				value->Release();
				return globalAtanor->Returnerror("Maximum size of table reached", idthread);
			}
		}
		value->Release();
		return aTRUE;
    }
    //In this specific case, we try to replace a bloc of values with a new bloc
    if (idx->isInterval()) {
        //First we remove the values...
		long lkey = idx->Integer();
        if (lkey < 0)
            lkey = size + lkey;
		long rkey = ((AtanorIndex*)idx)->right->Integer();
        if (rkey < 0)
            rkey = size + rkey;
        if (rkey < lkey || rkey >= size || lkey >= size) {
            if (globalAtanor->erroronkey)
                globalAtanor->Returnerror("Wrong index", idthread);
            return aTRUE;
        }
        if (rkey != lkey)
            rkey--;
        Atanor* krkey;
        while (rkey >= lkey) {
            krkey = values[rkey];
			values[rkey] = aNOELEMENT;
            krkey->Resetreference(reference + 1);
            rkey--;
        }
        if (value->isVectorContainer()) {
            for (int i = value->Size() - 1; i >= 0; i--) {
                krkey = value->getvalue(i);
				Insert(lkey, krkey);                
            }
            return aTRUE;
        }
        if (value->isContainer()) {
            AtanorIteration* itr = value->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
                krkey = itr->Value();
				Insert(lkey, krkey);				
            }
            itr->Release();
            return aTRUE;
        }

		Insert(lkey, krkey);
        return aTRUE;
    }
	register long ikey = idx->Integer();
    if (ikey >= size)
		return globalAtanor->Returnerror("Table is full", idthread);

	if (ikey < 0) {
		ikey = size + ikey;
		if (ikey < 0)
			return globalAtanor->Returnerror("Cannot set this value", idthread);
	}
	value = value->Atom();
	if (values[ikey] != aNOELEMENT)
		values[ikey]->Resetreference(reference + 1);
	values[ikey] = value;
	value->Setreference(reference + 1);

    return aTRUE;
}


Exporting Atanor* Atanortable::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {
    

    if (!idx->isIndex()) {

        Atanor* ke;
        //In this case, we copy the elements from the vector to the map, using the position as index
        if (contextualpattern->isMapContainer()) {
            Atanor* map = Selectamap(contextualpattern);
            size_t i = 0;
            char ch[20];
            for (int it = 0; it < size; it++) {
                sprintf_s(ch, 20, "%d", i);
                ke = values[it];
                map->Push(ch, ke);
                i++;
            }
            return map;
        }

        if (contextualpattern->Type() == a_int || contextualpattern->Type() == a_float)
            return globalAtanor->Provideint(size);
        return this;
    }

    Atanor* key;
    Atanor* keyright = NULL;

	AtanorIndex* kind = (AtanorIndex*)idx;
	key = kind->left->Get(aNULL, aNULL, idthread);
	if (kind->interval == true)
		keyright = kind->right->Get(aNULL, aNULL, idthread);

    register long ikey;
    bool stringkey = false;
    if (key->Type() == a_string) {
        string sf = key->String();
        stringkey = true;
        bool found = false;
		if (kind->signleft) {
            for (ikey = size - 1; ikey >= 0; ikey--) {
                if (sf == values[ikey]->String()) {
                    found = true;
                    break;
                }
            }

        }
        else {
            for (ikey = 0; ikey < size; ikey++) {
                if (sf == values[ikey]->String()) {
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            if (globalAtanor->erroronkey)
                return globalAtanor->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    else
        ikey = key->Integer();

    key->Release();

    if (ikey < 0)
        ikey = size + ikey;

    if (ikey < 0 || ikey >= size) {
        if (ikey != size || keyright == NULL) {
            if (globalAtanor->erroronkey)
                return globalAtanor->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }

    if (keyright == NULL)
        return values[ikey];

	Atanortable* kvect;
	long iright;
	if (keyright->Type() == a_string) {
		string sf = keyright->String();
		bool found = false;
		if (kind->signright) {
			for (iright = size - 1; iright >= 0; iright--) {
				if (sf == values[iright]->String()) {
					found = true;
					iright++;
					break;
				}
			}
		}
		else {
			for (iright = 0; iright < size; iright++) {
				if (sf == values[iright]->String()) {
					found = true;
					iright++;
					break;
				}
			}
		}
		if (!found) {
			if (globalAtanor->erroronkey)
				return globalAtanor->Returnerror("Wrong index", idthread);
			return aNOELEMENT;
		}
	}
	else {
		if (keyright == aNULL)
			iright = 0;
		else {
			iright = keyright->Integer();
			if (stringkey && iright >= 0)
				iright = ikey + iright + 1;
		}
	}

    keyright->Release();

    if (iright < 0 || keyright == aNULL) {
        iright = size + iright;
        if (iright<ikey) {
            if (globalAtanor->erroronkey)
                return globalAtanor->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    else {
        if (iright>size) {
            if (globalAtanor->erroronkey)
                return globalAtanor->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }

    //In this case, we must create a new vector
	kvect = new Atanortable(size);
	for (int i = ikey; i < iright; i++)
		kvect->values[i] = values[i];

    return kvect;
}

Exporting void Atanortable::Shuffle() {
    size_t sz = size;
    size_t i, f;
    int mx = sz;
    Atanor* v;
    for (i = 0; i < sz; i++) {
        f = localrandom(mx--);
        if (mx != f) {
            v = values[mx];
            values[mx] = values[f];
            values[f] = v;
        }
    }
}


Exporting Atanor* Atanortable::Unique() {
    
    Atanortable* kvect = new Atanortable(size);
    map<string, Atanor*> inter;
    string k;
    for (int i = 0; i < size; i++) {
        k = values[i]->String();
        if (inter.find(k) != inter.end()) {
            if (inter[k]->same(values[i])->Boolean() == false)
                kvect->Push(values[i]);
        }
        else {
            inter[k] = values[i];
            kvect->Push(values[i]);
        }
    }
    return kvect;
}


Exporting void Atanortable::Clear() {
    
	for (int itx = 0; itx < size; itx++) {
		values[itx]->Resetreference(reference + 1);
		values[itx] = aNOELEMENT;
	}    
}

Exporting Atanor* Atanortable::Merging(Atanor* ke) {
    if (!ke->isContainer()) {
        Push(ke);
        return this;
    }

    if (ke->Size() == 0)
        return this;

    Doublelocking _lock(this, ke);
    //Three cases:
    if (ke->Type() == idtype) {
        Atanortable* kvect = (Atanortable*)ke;
        for (long i = 0; i < kvect->size; i++)
            Push(kvect->values[i]);
        return this;
    }

    AtanorIteration* itr = ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next())
        Push(itr->Value());
    itr->Release();
    return this;
}


Exporting Atanor* Atanortable::same(Atanor* a) {
    Doublelocking _lock(this, a);

    if (a->Type() != idtype) {
        if (a->isVectorContainer()) {
            if (a->Size() != size)
                return aFALSE;
            Atanor* v;
            for (int i = 0; i < size; i++) {
                v = a->getvalue(i);
                if (!v->same(values[i])->Boolean()) {
                    v->Release();
                    return aFALSE;
                }
                v->Release();
            }
            return aTRUE;
        }
        return aFALSE;
    }

    Atanortable* v = (Atanortable*)a;

    if (size != v->size)
        return aFALSE;

    for (int i = 0; i < size; i++) {
        if (values[i]->same(v->values[i]) == aFALSE)
            return aFALSE;
    }
    return aTRUE;
}

Exporting unsigned int Atanortable::EditDistance(Atanor* e) {
    unsigned int s1len, s2len, x, y, lastdiag, olddiag;
    s1len = Size();
    s2len = e->Size();

    Atanor** v1 = new Atanor*[s1len + 1];
    Atanor** v2 = new Atanor*[s2len + 1];

    y = max(s1len, s2len);
    for (x = 0; x < y; x++) {
        if (x < s1len)
            v1[x] = getvalue(x);
        if (x < s2len)
            v2[x] = e->getvalue(x);
    }

    size_t* column = new size_t[s1len + 1];
    for (y = 1; y <= s1len; y++)
        column[y] = y;

    unsigned int ed;
    for (x = 1; x <= s2len; x++) {
        column[0] = x;
        for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
            olddiag = column[y];
            ed = v1[y - 1]->EditDistance(v2[x - 1]);
            column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + ed);
            lastdiag = olddiag;
        }
    }

    y = max(s1len, s2len);
    for (x = 0; x < y; x++) {
        if (x < s1len)
            v1[x]->Release();
        if (x < s2len)
            v2[x]->Release();
    }

    delete[] v1;
    delete[] v2;

    s2len = column[s1len];
    delete[] column;
    return s2len;

}

//---------------------------------------------------------------------------------------


Exporting Atanor* Atanortable::Loophaskell(Atanor* recipient, Atanor* context, Atanor* environment, AtanorFunctionLambda* bd, short idthread) {

	Atanor* a;
	char addvalue = 0;

	if (context != aNOELEMENT)
		addvalue = Selecttype(context);

	for (size_t i = 0; i < size; i++) {
		if (values[i] == aNOELEMENT)
			continue;

		recipient->Putvalue(values[i], idthread);

		a = bd->Get(environment, aNULL, idthread);

		if (globalAtanor->Error(idthread) || a == aBREAK) {
			if (a == aBREAK)
				break;

			recipient->Forcedclean();
			a->Release();
			context->Release();
			return globalAtanor->Errorobject(idthread);
		}

		if (a != aNULL) {
			context = Storealongtype(context, a, idthread, addvalue);
			a->Release();
		}
	}

	recipient->Forcedclean();
	return context;
}

Exporting Atanor* Atanortable::Filter(short idthread, Atanor* env, AtanorFunctionLambda* bd, Atanor* var, Atanor* kcont, Atanor* accu, Atanor* init, bool direct) {

	Atanor* returnval;

	bool first = false;
	Atanor* key;

	if (init != aNOELEMENT) {
		accu->Putvalue(init, idthread);
		if (kcont != NULL) {
			if (direct)
				kcont->Insert(0, init);
			else
				kcont->Push(init);
		}
	}
	else
		first = true; //we are dealing with a foldr1 or a foldl1

	for (size_t i = 0; i < size; i++) {
		key = values[i];
		if (key == aNOELEMENT)
			continue;

		if (first) {
			returnval = key->Atom();//We use the first value of the list to seed our accumulator variable
			first = false;
		}
		else {
			var->Putvalue(key, idthread);
			returnval = bd->Get(env, aNULL, idthread);

			if (returnval == aBREAK) {
				accu = returnval;
				break;
			}

			if (globalAtanor->Error(idthread)) {
				var->Forcedclean();
				if (kcont != NULL)
					kcont->Release();
				return globalAtanor->Errorobject(idthread);
			}
		}

		if (returnval != aNULL) {

			accu->Putvalue(returnval, idthread);
			if (kcont != NULL) {
				if (direct)
					kcont->Insert(0, returnval);
				else
					kcont->Push(returnval);
			}
			returnval->Release();
		}
	}

	var->Forcedclean();
	if (kcont != NULL)
		return kcont;
	return accu->Value();
}

//----------------------------------------------------------------------------------
Exporting void Atanortable::storevalue(string u) {
	Atanor* a = globalAtanor->Providestring(u);

	
	push(a);
}

Exporting void Atanortable::storevalue(wstring u) {
	Atanor* a = globalAtanor->Provideustring(u);

	
	push(a);
}

Exporting void Atanortable::storevalue(long u) {
	Atanor* a = globalAtanor->Provideint(u);
	
	push(a);
}

Exporting void Atanortable::storevalue(short u) {
	Atanor* a = new Atanorshort(u);

	
	push(a);
}

Exporting void Atanortable::storevalue(BLONG u) {
	Atanor* a = new Atanorlong(u);

	
	push(a);
}

Exporting void Atanortable::storevalue(float u) {
	Atanor* a = new Atanordecimal(u);

	
	push(a);
}

Exporting void Atanortable::storevalue(double u) {
	Atanor* a = globalAtanor->Providefloat(u);

	
	push(a);
}

Exporting void Atanortable::storevalue(unsigned char u) {
	Atanor* a = new Atanorbyte(u);
	
	push(a);
}

Exporting void Atanortable::storevalue(wchar_t u) {
	wstring w;
	w = u;
	Atanor* a = globalAtanor->Provideustring(w);
	
	push(a);
}





