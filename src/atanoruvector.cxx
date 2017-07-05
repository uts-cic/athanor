
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
filename   : atanoruvector.cxx
Date       : 2017/04/01
Purpose    : vector implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorhaskell.h"
#include "atanoruvector.h"
#include "atanorustring.h"
#include "atanorstring.h"
#include "atanormap.h"
#include "atanorvector.h"
#include "atanorivector.h"
#include "atanorfvector.h"
#include "constobjects.h"
#include "instructions.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, uvectorMethod>  Atanoruvector::methods;
Exporting hmap<string, string> Atanoruvector::infomethods;
Exporting bin_hash<unsigned long> Atanoruvector::exported;

Exporting short Atanoruvector::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanoruvector::AddMethod(AtanorGlobal* global, string name, uvectorMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanoruvector::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();



    Atanoruvector::idtype = global->Getid("uvector");

    Atanoruvector::AddMethod(global, "remove", &Atanoruvector::MethodRemove, P_ONE, "remove(e): remove 'e' from the vector.");

    Atanoruvector::AddMethod(global, "sum", &Atanoruvector::MethodSum, P_NONE, "sum(): concatenate the strings in the vector.");
    Atanoruvector::AddMethod(global, "reverse", &Atanoruvector::MethodReverse, P_NONE, "reverse(): reverse a vector.");
    Atanoruvector::AddMethod(global, "unique", &Atanoruvector::MethodUnique, P_NONE, "unique(): remove duplicate elements.");
    Atanoruvector::AddMethod(global, "reserve", &Atanoruvector::MethodReserve, P_ONE, "reserve(int sz): Reserve a size of 'sz' potential element in the vector.");
    Atanoruvector::AddMethod(global, "last", &Atanoruvector::MethodLast, P_NONE, "last(): return the last element.");
    Atanoruvector::AddMethod(global, "join", &Atanoruvector::MethodJoin, P_ONE, "join(string sep): Produce a string representation for the container.");
    Atanoruvector::AddMethod(global, "shuffle", &Atanoruvector::MethodShuffle, P_NONE, "shuffle(): shuffle the values in the vector.");
    Atanoruvector::AddMethod(global, "sort", &Atanoruvector::MethodSort, P_ONE, "sort(bool reverse): sort the elements within.");
    Atanoruvector::AddMethod(global, "push", &Atanoruvector::MethodPush, P_ATLEASTONE, "push(v): Push a value into the vector.");
    Atanoruvector::AddMethod(global, "pop", &Atanoruvector::MethodPop, P_NONE | P_ONE, "pop(i): Erase an element from the vector");
    Atanoruvector::AddMethod(global, "merge", &Atanoruvector::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");
    Atanoruvector::AddMethod(global, "editdistance", &Atanoruvector::MethodEditDistance, P_ONE, "editdistance(v): Compute the edit distance with vector 'v'.");
    Atanoruvector::AddMethod(global, "insert", &Atanoruvector::MethodInsert, P_TWO, "insert(i,v): Insert v at position i.");



    global->newInstance[Atanoruvector::idtype] = new Atanoruvector(global);
    global->RecordMethods(Atanoruvector::idtype, Atanoruvector::exported);

    return true;
}

Exporting AtanorIteration* Atanoruvector::Newiteration(bool direction) {
    return new AtanorIterationuvector(this, direction);
}

Exporting Atanor* Atanoruvector::in(Atanor* context, Atanor* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    wstring val = a->UString();
    if (context->isBoolean()) {
        Locking _lock(this);
        for (size_t i = 0; i < values.size(); i++) {
            if (values[i] == val)
                return aTRUE;
        }
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Atanorivector* v = (Atanorivector*)Selectaivector(context);
        Doublelocking _lock(this, v);
        for (size_t i = 0; i < values.size(); i++) {
            if (values[i] == val)
                v->values.push_back(i);
        }
        return v;
    }

    Locking _lock(this);
    for (size_t i = 0; i < values.size(); i++) {
        if (values[i] == val)
            return globalAtanor->Provideint(i);
    }

    return aNULL;
}



Exporting Atanor* Atanoruvector::getvalue(BLONG i) {
    Locking _lock(this);
    if (i < 0 || i >= values.size())
        return aNOELEMENT;
    return globalAtanor->Provideustring(values[i]);
}

Exporting Atanor* Atanoruvector::Push(Atanor* a) {
    Locking _lock(this);
    values.push_back(a->UString());
    return aTRUE;
}

Exporting Atanor* Atanoruvector::Pop(Atanor* idx) {
    Locking _lock(this);
    if (!values.size())
        return aFALSE;

    BLONG v = idx->Integer();
    if (v == -1) {
        v = values.size() - 1;
        values.pop_back();
    }
    else {
        if (v < 0 || v >= (BLONG)values.size())
            return aFALSE;
        values.erase(values.begin() + v);
    }
    return aTRUE;
}

Exporting void Atanoruvector::Clear() {
    //To set a variable back to empty
    Locking _lock(this);
    values.clear();
}

Exporting void Atanoruvector::Clean() {
    //Your cleaning code
}

Exporting string Atanoruvector::JSonString() {
    string res;
    res = "[";
    bool beg = true;
    string sx;
    for (int i = 0; i < values.size(); i++) {
        if (beg == false)
            res += ",";
        beg = false;
        sc_unicode_to_utf8(sx, values[i]);
        jstringing(res, sx);
    }
    res += "]";
    return res;
}

Exporting string Atanoruvector::String() {
    string res;
    res = "[";
    bool beg = true;
    string sx;
    for (int i = 0; i < values.size(); i++) {
        if (beg == false)
            res += ",";
        beg = false;
        sc_unicode_to_utf8(sx, values[i]);
        stringing(res, sx);
    }
    res += "]";
    return res;
}

Exporting wstring Atanoruvector::UString() {
    Locking _lock(this);
    wstring res;
    res = L"[";
	bool beg = true;
	wstring sx;
	for (int i = 0; i < values.size(); i++) {
		if (beg == false)
			res += L",";
		beg = false;
		sx += values[i];
		ustringing(res, sx);
	}
	res += L"]";
	return res;
}


Exporting long Atanoruvector::Integer() {
	Locking _lock(this);
	return values.size();
}

Exporting double Atanoruvector::Float() {
	Locking _lock(this);
	return values.size();
}

Exporting BLONG Atanoruvector::Long() {
	Locking _lock(this);
	return values.size();
}

Exporting bool Atanoruvector::Boolean() {
	Locking _lock(this);
	if (values.size() == 0)
		return false;
	return true;
}


//Basic operations
Exporting long Atanoruvector::Size() {
	Locking _lock(this);
	return values.size();
}

Exporting void Atanoruvector::addustringto(wstring ws, int i) {
	Locking _lock(this);
	if (values.size() == 0)
		return;

	string s;
	if (i < 0)
		values.back() += ws;
	else {
		if (i >= values.size())
			return;
		values[i] += ws;
	}
}

Exporting void Atanoruvector::addstringto(wchar_t wc, int i) {
	Locking _lock(this);
	if (values.size() == 0)
		return;

	if (i < 0)
		values.back() += wc;
	else {
		if (i >= values.size())
			return;
		values[i] += wc;
	}
}

Exporting void Atanoruvector::addstringto(string ms, int i) {
	Locking _lock(this);
	if (values.size() == 0)
		return;

	wstring s;
	s_utf8_to_unicode(s, STR(ms));
	if (i < 0)
		values.back() += s;
	else {
		if (i >= values.size())
			return;
		values[i] += s;
	}
}

Exporting Atanor* Atanoruvector::Map(short idthread) {
	Locking _lock(this);
	Atanormap* kmap = globalAtanor->Providemap();

	char buff[100];
	for (int it = 0; it < values.size(); it++) {
		sprintf_s(buff, 100, "%d", it);
    kmap->Push(buff, globalAtanor->Provideustring(values[it]));
}
return kmap;
}

Exporting Atanor* Atanoruvector::Vector(short idthread) {
    Locking _lock(this);
    Atanorvector* kvect = globalAtanor->Providevector();
    kvect->values.reserve(values.size());
    for (int i = 0; i < values.size(); i++)
        kvect->Push(globalAtanor->Provideustring(values[i]));
    return kvect;
}

Exporting Atanor*  Atanoruvector::Put(Atanor* idx, Atanor* ke, short idthread) {
    if (ke == this)
        return aTRUE;

    Doublelocking _lock(this, ke);

    if (!idx->isIndex()) {
        if (ke == aNULL) {
            values.clear();
            return aTRUE;
        }

        if (ke->Type() == a_uvector) {
            values = ((Atanoruvector*)ke)->values;
            return aTRUE;
        }
        if (ke->isMapContainer()) {
            values.clear();
            values.reserve(ke->Size());
            AtanorIteration* itr = (AtanorIteration*)ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values.push_back(itr->Keyustring());
            itr->Release();
            return aTRUE;
        }
        if (ke->isContainer()) {
            values.clear();
            values.reserve(ke->Size());
            AtanorIteration* itr = (AtanorIteration*)ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values.push_back(itr->Valueustring());
            itr->Release();
            return aTRUE;
        }
        if (ke->isString()) {
            values.clear();
            wstring sv = ke->UString();
            if (!v_comma_split_string(sv, values))
                return globalAtanor->Returnerror("Cannot set this value", idthread);
            return aTRUE;
        }
        ke = ke->Vector(idthread);
        if (!ke->isVectorContainer())
            return globalAtanor->Returnerror("Cannot set this value", idthread);

        values.clear();
        values.reserve(ke->Size());
        //We copy all values from ke to this
        for (long it = 0; it < ke->Size(); ++it)
            values.push_back(ke->getustring(it));
        ke->Release();
        return aTRUE;
    }
    else {
        //In this specific case, we try to replace a bloc of values with a new bloc
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

            if (rkey != lkey)
                rkey--;

            while (rkey >= lkey) {
                values.erase(values.begin() + rkey);
                rkey--;
            }

            if (ke->isVectorContainer()) {
                for (int i = ke->Size() - 1; i >= 0; i--) {
                    values.insert(values.begin() + lkey, ke->getustring(i));
                }
                return aTRUE;
            }

            if (ke->isContainer()) {
                AtanorIteration* itr = (AtanorIteration*)ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    values.insert(values.begin() + lkey, itr->Valueustring());
                itr->Release();
                return aTRUE;
            }

            values.insert(values.begin() + lkey, ke->UString());
        }
        else {
            register int ikey = idx->Integer();
            int sz = values.size();
            int nb;
            wstring n = ke->UString();
            if (ikey < 0) {
                ikey = sz + ikey;
                if (ikey < 0)
                    return globalAtanor->Returnerror("Cannot set this value", idthread);
            }

            if (ikey >= sz) {
                for (nb = values.size(); nb < ikey; nb++)
                    values.push_back(L"");
				values.push_back(n);
			}
			else
				values[ikey] = n;
		}
	}
	return aTRUE;
}

Exporting Atanor* Atanoruvector::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {
	Locking _lock(this);

	if (!idx->isIndex()) {
		Atanor* kv = NULL;
		//particular case, the contextualpattern is a vector, which means that we expect a set of keys
		//as a result
		if (contextualpattern->isVectorContainer()) {
			kv = Selectauvector(contextualpattern);
			if (kv->Type() == a_uvector)
				((Atanoruvector*)kv)->values = values;
			else
			for (int i = 0; i < values.size(); i++)
				kv->Push(globalAtanor->Provideustring(values[i]));
			return kv;
		}

		if (contextualpattern->Type() == a_int || contextualpattern->Type() == a_float)
			return globalAtanor->Provideint(values.size());
		return this;
	}

	Atanor* key;
	Atanor* keyright = NULL;

	AtanorIndex* kind = (AtanorIndex*)idx;
	key = kind->left->Get(aNULL, aNULL, idthread);
	if (kind->interval == true)
		keyright = kind->right->Get(aNULL, aNULL, idthread);

	register int ikey;
	bool stringkey = false;
	if (key->Type() == a_string) {
		stringkey = true;
		wstring sf = key->UString();
		bool found = false;
		if (kind->signleft) {
			for (ikey = values.size() - 1; ikey >= 0; ikey--) {
				if (sf == values[ikey]) {
					found = true;
					break;
				}
			}
		}
		else {
			for (ikey = 0; ikey < values.size(); ikey++) {
				if (sf == values[ikey]) {
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
            ikey = values.size() + ikey;

        if (ikey < 0 || ikey >= values.size()) {
            if (ikey != values.size() || keyright == NULL) {
                if (globalAtanor->erroronkey)
                    return globalAtanor->Returnerror("Wrong index", idthread);
                return aNOELEMENT;
            }
        }

        if (keyright == NULL)
            return globalAtanor->Provideustring(values[ikey]);

        long iright;
        if (keyright->Type() == a_string) {
            wstring sf = keyright->UString();
            bool found = false;
            if (kind->signright) {
                for (iright = values.size() - 1; iright >= 0; iright--) {
                    if (sf == values[iright]) {
                        found = true;
                        iright++;
                        break;
                    }
                }
            }
            else {
                for (iright = 0; iright < values.size(); iright++) {
                    if (sf == values[iright]) {
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

        Atanoruvector* kvect;

        keyright->Release();
        if (iright < 0 || keyright == aNULL) {
            iright = values.size() + iright;
            if (iright<ikey) {
                if (globalAtanor->erroronkey)
                    return globalAtanor->Returnerror("Wrong index", idthread);
                return aNOELEMENT;
            }
        }
        else  {
            if (iright>values.size()) {
                if (globalAtanor->erroronkey)
                    return globalAtanor->Returnerror("Wrong index", idthread);
                return aNOELEMENT;
            }
        }

        //In this case, we must create a new vector
        kvect = globalAtanor->Provideuvector();
        for (int i = ikey; i < iright; i++)
            kvect->values.push_back(values[i]);
        return kvect;
    }

    Exporting Atanor* Atanoruvector::same(Atanor* a) {
        Doublelocking _lock(this, a);

        if (a->Type() != idtype) {
            if (a->isVectorContainer()) {
                if (a->Size() != values.size())
                    return aFALSE;
                AtanorIteration* itr = a->Newiteration(false);
                for (int i = 0; i < values.size(); i++) {
                    if (values[i] != itr->Valueustring()) {
                        itr->Release();
                        return aFALSE;
                    }
                }
                itr->Release();
                return aTRUE;
            }
            return aFALSE;
        }

        Atanoruvector* m = (Atanoruvector*)a;
        if (m->values.size() != values.size())
            return aFALSE;

        for (int i = 0; i < values.size(); i++) {
            if (m->values[i] != values[i])
                return aFALSE;
        }
        return aTRUE;
    }

    Exporting bool Atanoruvector::Permute() {
        Locking _lock(this);

        return next_permutation(values.begin(), values.end());
    }

    Exporting void Atanoruvector::Shuffle() {
        Locking _lock(this);

        size_t sz = values.size();
        size_t i, f;
        int mx = sz;
        wstring v;
        for (i = 0; i < sz; i++) {
            f = localrandom(mx--);
            if (mx != f) {
                v = values[mx];
                values[mx] = values[f];
                values[f] = v;
            }
        }
    }

    Exporting Atanor* Atanoruvector::Unique() {
        Locking _lock(this);
        Atanoruvector* kvect = globalAtanor->Provideuvector();
        map<wstring, bool> inter;
        for (int i = 0; i < values.size(); i++) {
            if (inter.find(values[i]) == inter.end()) {
                inter[values[i]] = true;
                kvect->values.push_back(values[i]);
            }
        }
        return kvect;
    }

    Exporting unsigned int Atanoruvector::EditDistance(Atanor* e) {
        unsigned int s1len, s2len, x, y, lastdiag, olddiag;
        s1len = Size();
        s2len = e->Size();

        wstring* v1 = new wstring[s1len + 1];
        wstring* v2 = new wstring[s2len + 1];

        y = max(s1len, s2len);
        for (x = 0; x < y; x++) {
            if (x < s1len)
                v1[x] = values[x];
            if (x < s2len)
                v2[x] = e->getustring(x);
        }

        size_t* column = new size_t[s1len + 1];
        for (y = 1; y <= s1len; y++)
            column[y] = y;

        unsigned int ed;
        for (x = 1; x <= s2len; x++) {
            column[0] = x;
            for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
                olddiag = column[y];
                ed = StringEditDistance(v1[y - 1], v2[x - 1]);
                column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + ed);
                lastdiag = olddiag;
            }
        }

        delete[] v1;
        delete[] v2;

        s2len = column[s1len];
        delete[] column;
        return s2len;

    }

    Exporting Atanor* Atanoruvector::andset(Atanor *b, bool itself) {
        if (!b->isContainer())
            return aNULL;

        Atanoruvector* ref;

        long size = Size();
        size_t it;

        ref = globalAtanor->Provideuvector();
        Locking _lock(this);
        AtanorIteration* itr = b->Newiteration(false);
        for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
            for (it = 0; it < size; it++) {
                if (values[it] == itr->Valueustring()) {
                    ref->values.push_back(values[it]);
                    break;
                }
            }
        }
        itr->Release();
        return ref;

    }

    Exporting Atanor* Atanoruvector::orset(Atanor *b, bool itself) {
        Atanoruvector* ref;
        if (itself)
            ref = this;
        else
            ref = (Atanoruvector*)Atom(true);

        ref->Merging(b);
        return ref;
    }

    Exporting Atanor* Atanoruvector::xorset(Atanor *b, bool itself) {
        if (!b->isVectorContainer())
            return aNULL;


        long size = Size();
        size_t it;

        Atanoruvector* ref = globalAtanor->Provideuvector();
        Locking _lock(this);
        wstring s;
        hmap<wstring, bool> store;
        for (it = 0; it < b->Size(); it++)
            store[b->getustring(it)] = true;

        for (it = 0; it < size; it++) {
            if (store.find(values[it]) == store.end())
                store[values[it]] = true;
        }

        for (auto& i : store)
            ref->values.push_back(i.first);

        return ref;
    }

    Exporting Atanor* Atanoruvector::plus(Atanor* b, bool itself) {
        Atanoruvector* ref;
        if (itself)
            ref = this;
        else
            ref = (Atanoruvector*)Atom(true);

        int it;
        Doublelocking _lock(ref, b);
        long size = values.size();

        if (b->isContainer()) {
            AtanorIteration* itr = b->Newiteration(false);
            itr->Begin();
            for (it = 0; it < size; it++) {
                if (itr->End() == aTRUE)
                    break;
                ref->values[it] += itr->Valueustring();
                itr->Next();
            }
            itr->Release();
            return ref;
        }

        wstring v = b->UString();
        for (it = 0; it < size; it++) {
            ref->values[it] += v;
        }

        return ref;
    }

    Exporting Atanor* Atanoruvector::minus(Atanor *b, bool itself) {
        Atanoruvector* ref;
        if (itself)
            ref = this;
        else
            ref = (Atanoruvector*)Atom(true);

        int it;
        Locking _lock(ref);
        long size = values.size();

        if (b->isContainer()) {
            AtanorIteration* itr = b->Newiteration(false);
            itr->Begin();
            for (it = 0; it < size; it++) {
                if (itr->End() == aTRUE)
                    break;
                ref->values[it] = StringMinus(ref->values[it], itr->Valueustring());
                itr->Next();
            }
            itr->Release();
            return ref;
        }

        wstring v = b->UString();
        for (it = 0; it < size; it++) {
            ref->values[it] = StringMinus(ref->values[it], v);
        }

        return ref;
    }


    static bool comp(wstring s1, wstring s2) {
        if (s1<s2)
            return true;
        return false;
    }

    static bool icomp(wstring s1, wstring s2) {
        if (s1>s2)
            return true;
        return false;
    }

    Exporting void Atanoruvector::Sort(bool direction) {
        Locking _lock(this);
        if (direction == false)
            sort(values.begin(), values.end(), comp);
        else
            sort(values.begin(), values.end(), icomp);
    }


    Exporting Atanor* Atanoruvector::Merging(Atanor* ke) {
        if (!ke->isContainer()) {
            values.push_back(ke->UString());
            return this;
        }

        if (ke->Size() == 0)
            return this;

        Doublelocking _lock(this, ke);	//Three cases:

        if (ke->Type() == idtype) {
            Atanoruvector* kvect = (Atanoruvector*)ke;
            values.insert(values.end(), kvect->values.begin(), kvect->values.end());
            return this;
        }

        AtanorIteration* itr = ke->Newiteration(false);
        for (itr->Begin(); itr->End() == aFALSE; itr->Next())
            values.push_back(itr->Valueustring());
        itr->Release();

        return this;
    }

    Exporting Atanor* Atanoruvector::Loophaskell(Atanor* recipient, Atanor* context, Atanor* environment, AtanorFunctionLambda* bd, short idthread) {
        Atanor* a;
        Atanorustring* it = globalAtanor->Provideustring();
        it->Setreference();

        char addvalue = 0;

        if (context != aNOELEMENT)
            addvalue = Selecttype(context);

        recipient->Putvalue( it, idthread);

        for (size_t i = 0; i < values.size(); i++) {
            it->value = values[i];

            a = bd->Get(environment, aNULL, idthread);

            if (globalAtanor->Error(idthread) || a == aBREAK) {
                if (a == aBREAK)
                    break;

                it->Resetreference();
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

        it->Resetreference();
        recipient->Forcedclean();

        return context;
    }

    Exporting Atanor* Atanoruvector::Filter(short idthread, Atanor* env, AtanorFunctionLambda* bd, Atanor* var, Atanor* kcont, Atanor* accu, Atanor* init, bool direct) {

        Atanor* returnval;

        bool first = false;
        Atanorustring* key = globalAtanor->Provideustring();
        key->Setreference();

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

        for (size_t i = 0; i < values.size(); i++) {
            key->value = values[i];
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
                    key->Resetreference();
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

        key->Resetreference();
        var->Forcedclean();
        if (kcont != NULL)
            return kcont;
        return accu->Value();
    }

    class UComp {
        public:
        AtanorCallFunction compare;
        short idthread;
        AtanorConstUString p;
        AtanorConstUString s;

        UComp(AtanorFunction* comp, short idt) : compare(comp), p(L""), s(L""), idthread(idt) {
		compare.arguments.push_back(&p);
		compare.arguments.push_back(&s);
	}

	bool get() {
		return compare.Get(aNULL, aNULL, idthread)->Boolean();
	}
};

class UComparison {
public:
	UComp* compare;

	UComparison(UComp* c)  {
		compare = c;
	}

	bool operator() (const wstring& i, const wstring& j) {
		compare->p.value = i;
		compare->s.value = j;
		return compare->get();
	}
};

Atanor* Atanoruvector::MethodSort(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* comp = callfunc->Evaluate(0, contextualpattern, idthread);
	if (comp->isFunction()) {
		if (comp->Size() != 2)
			return globalAtanor->Returnerror("Expecting a comparison function with two parameters", idthread);
        UComp kcomp((AtanorFunction*)comp->Body(idthread), idthread);
        UComparison kfcomp(&kcomp);
        sort(values.begin(), values.end(), kfcomp);
        return this;
    }
    Sort(comp->Boolean());
    return this;
}


Exporting Atanor* Atanoruvector::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
    Locking _lock(this);
    Atanor* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    
    Atanor* a;
    for (long i = 0; i < values.size(); i++) {
        var->storevalue(values[i]);

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
