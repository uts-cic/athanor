
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
filename   : atanormap.cxx
Date       : 2017/04/01
Purpose    : map implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanormap.h"
#include "atanorlist.h"
#include "atanorivector.h"
#include <memory>

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, mapMethod>  Atanormap::methods;
Exporting hmap<string, string> Atanormap::infomethods;
Exporting bin_hash<unsigned long> Atanormap::exported;

Exporting short Atanormap::idtype = 0;

void AtanorConstmap::Prepare(Atanor* env, short idthread) {
    Atanor* v;
    for (size_t i = 0; i < values.size(); i++) {
        v = keys[i];
        if (v != aPIPE) {
            if (v->isCallVariable())
                v->Put(env, aNULL, idthread);
            else
                if (v->isVariable())
                    v->Get(env, aNULL, idthread);
                else
                    v->Prepare(env, idthread);
        }

        v = values[i];
        if (v->isCallVariable())
            v->Put(env, aNULL, idthread);
        else
            if (v->isVariable())
                v->Get(env, aNULL, idthread);
            else
                v->Prepare(env, idthread);
    }
}

bool AtanorConstmap::Checkvariable() {
    if (values.size() != 1)
        return false;

    if (values[0]->isCallVariable() && keys[0]->isCallVariable())
        return true;

    return false;
}

bool AtanorConstmap::Setvalue(Atanor* iter, Atanor* value, short idthread, bool strict) {


    Atanor* k = ((AtanorIteration*)iter)->IteratorKey();
	keys[0]->Setaffectation(true);
    value = keys[0]->Get(aNULL, aNULL, idthread);
    value->Put(aNULL, k, idthread);

    Atanor* v = ((AtanorIteration*)iter)->IteratorValue();
	values[0]->Setaffectation(true);
    value = values[0]->Get(aNULL, aNULL, idthread);
    value->Put(aNULL, v, idthread);

    return true;
}

Atanor* AtanorConstmap::same(Atanor* value) {
    short idthread = globalAtanor->GetThreadid();

    if (!value->isMapContainer())
        return aFALSE;

    Atanor* a;
    Atanor* v;

	std::unique_ptr<AtanorIteration> it(value->Newiteration(false));

    Locking _lock(this);

    bool clean = false;
    it->Begin();
    for (size_t i = 0; i < values.size(); i++) {
        a = keys[i];
        if (a == aPIPE) {
            //Then we are in a split...
            //the value is a map...
            a = values[i];            
            if (it->End() != aTRUE) {
                AtanorIndex idx(true);
                idx.left = it->IteratorKey();
                idx.right = aNULL;
                v = value->Get(aNULL, &idx, idthread);
            }
            else
                v = value->Newinstance(idthread);
        }
        else {
            if (it->End() == aTRUE)                
                return aFALSE;

            v = it->IteratorKey();

            if (a->isCallVariable()) {
                a->Setaffectation(true);
                a = a->Get(aNULL, aNULL, idthread);
                a->Putvalue(v, idthread);
            }
            else {
                a = a->Get(aNULL, aNULL, idthread);
                if (a != aNOELEMENT && a != aUNIVERSAL && a->same(v) == aFALSE)
                    return aFALSE;
            }
            a = values[i];
            v = it->IteratorValue();
        }

        if (a->isCallVariable()) {
            a->Setaffectation(true);
            a = a->Get(aNULL, aNULL, idthread);
            a->Putvalue(v, idthread);
			continue;
        }
        
		if (a->isVectorContainer()) {			
			if (!v->isVectorContainer())				
				return aFALSE;

			a = a->Putvalue(v, idthread);
			if (a->isError())
				return a;
			continue;
		}

		if (a->isMapContainer()) {			
			if (!v->isMapContainer())				
				return aFALSE;
			a = a->Putvalue(v, idthread);
			if (a->isError())				
				return a;
			continue;
		}

		a = a->Get(aNULL, aNULL, idthread);
		if (a != aNOELEMENT && a != aUNIVERSAL && a->same(v) == aFALSE)			
			return aFALSE;

		it->Next();
	}

	return aTRUE;
}


Atanor* AtanorConstmap::Put(Atanor* index, Atanor* value, short idthread) {
    if (!evaluate)
        return this;

    if (!value->isMapContainer()) {
        if (index == aRAISEERROR)
            return aRAISEERROR;
        return globalAtanor->Returnerror("Wrong affectation", idthread);
    }

    Atanor* a;
    Atanor* v;

	std::unique_ptr<AtanorIteration> it(value->Newiteration(false));

    Locking _lock(this);

    bool clean = false;
    it->Begin();
	for (size_t i = 0; i < values.size(); i++) {
		a = keys[i];
		if (a == aPIPE) {
			//Then we are in a split...
			//the value is a map...
			a = values[i];			
			if (it->End() != aTRUE) {
				AtanorIndex idx(true);
				idx.left = it->IteratorKey();
				idx.right = aNULL;
				v = value->Get(aNULL, &idx, idthread);
			}
			else
				v = value->Newinstance(idthread);
		}
		else {
			if (it->End() == aTRUE) {			
				if (index == aRAISEERROR)
					return aRAISEERROR;
				return globalAtanor->Returnerror("Out of range affectation.", idthread);
			}


			v = it->IteratorKey();

			if (a->isCallVariable()) {
				a->Setaffectation(true);
				a = a->Get(aNULL, aNULL, idthread);
				a->Putvalue(v, idthread);
			}
			else {
				if (a->isVariable()) {
					//Then it should have created before hand...
					a = globalAtanor->Getdeclaration(a->Name(), idthread);
					a->Putvalue(v, idthread);
				}
				else {
					a = a->Get(aNULL, aNULL, idthread);
					if (a != aUNIVERSAL && a->same(v) == aFALSE) {						
						if (index == aRAISEERROR)
							return aRAISEERROR;
						return globalAtanor->Returnerror("No match affectation.", idthread);
					}
				}
			}
			a = values[i];
			v = it->IteratorValue();
		}

		if (a->isCallVariable()) {
			a->Setaffectation(true);
			a = a->Get(aNULL, aNULL, idthread);
			a->Putvalue(v, idthread);
			continue;
		}
		if (a->isVariable()) {
			//Then it should have created before hand...
			a = globalAtanor->Getdeclaration(a->Name(), idthread);
			a->Putvalue(v, idthread);
			continue;
		}

		if (a->isVectorContainer()) {			
			if (!v->isVectorContainer()) {				
				if (index == aRAISEERROR)
					return index;
				return globalAtanor->Returnerror("Mismatch assignment, expecting two vector containers.", idthread);
			}
			a = a->Put(index, v, idthread);
			if (a->isError())				
				return a;		
			continue;
		}

		if (a->isMapContainer()) {			
			if (!v->isMapContainer()) {				
				if (index == aRAISEERROR)
					return index;
				return globalAtanor->Returnerror("Mismatch assignment, expecting two map containers.", idthread);
			}
			a = a->Put(index, v, idthread);
			if (a->isError())				
				return a;
			continue;
		}

		a = a->Get(aNULL, aNULL, idthread);
		if (a != aUNIVERSAL && a->same(v) == aFALSE) {			
			if (index == aRAISEERROR)
				return aRAISEERROR;
			return globalAtanor->Returnerror("Mismatch assignment", idthread);
		}
	
        it->Next();
    }
	
    return this;
}

Atanor* AtanorConstmap::Get(Atanor* index, Atanor* value, short idthread) {
    if (affectation && evaluate)
        return this;

    Locking _lock(this);

    Atanormap* kmap = globalAtanor->Providemap();
    BLONG it;
    BLONG sz = values.size();

    if (isEvaluate()) {
        Atanor* k;
        Atanor* v;
        for (it = 0; it < sz; it++) {
            k = keys[it]->Get(aNULL, aNULL, idthread);
            v = values[it]->Get(aNULL, aNULL, idthread);
            kmap->Push(k, v);
            k->Release();
            v->Release();
        }
    }
    else {
        for (it = 0; it < sz; it++)
            kmap->Push(keys[it], values[it]);
    }
    return kmap;
}

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanormap::AddMethod(AtanorGlobal* global, string name, mapMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanormap::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();


    Atanormap::idtype = global->Getid("map");

    Atanormap::AddMethod(global, "clear", &Atanormap::MethodClear, P_NONE, "clear(): clear the container.");

    Atanormap::AddMethod(global, "items", &Atanormap::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");
    
    Atanormap::AddMethod(global, "invert", &Atanormap::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Atanormap::AddMethod(global, "find", &Atanormap::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");
    
    Atanormap::AddMethod(global, "join", &Atanormap::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    global->returntypes[global->Getid("join")] = a_string;

    Atanormap::AddMethod(global, "test", &Atanormap::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Atanormap::AddMethod(global, "keys", &Atanormap::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    global->returntypes[global->Getid("keys")] = a_vector;

    Atanormap::AddMethod(global, "values", &Atanormap::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    global->returntypes[global->Getid("values")] = a_vector;

    Atanormap::AddMethod(global, "sum", &Atanormap::MethodSum, P_NONE, "sum(): return the sum of elements.");
    global->returntypes[global->Getid("sum")] = a_float;

    Atanormap::AddMethod(global, "product", &Atanormap::MethodProduct, P_NONE, "product(): return the product of elements.");
    global->returntypes[global->Getid("product")] = a_float;

    Atanormap::AddMethod(global, "pop", &Atanormap::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Atanormap::AddMethod(global, "merge", &Atanormap::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Atanormap::idtype] = new Atanormap(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("smap")] = new Atanormap(global);
    global->newInstance[global->Getid("maps")] = new Atanormap(global);

    global->RecordMethods(global->Getid("maps"), Atanormap::exported);
    global->RecordMethods(global->Getid("smap"), Atanormap::exported);
    #endif
    global->RecordMethods(Atanormap::idtype, Atanormap::exported);

	global->newInstance[a_constmap] = new AtanorConstmap(global);
	global->RecordMethods(a_constmap, Atanormap::exported);
	
    return true;
}


Exporting AtanorIteration* Atanormap::Newiteration(bool direction) {
    return new AtanorIterationmap(this, direction);
}



Exporting Atanor* Atanormap::in(Atanor* context, Atanor* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    string val = a->String();

    if (context->isBoolean()) {
        Locking _lock(this);
        if (values.find(val) != values.end())
            return aTRUE;
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Atanorsvector* v = (Atanorsvector*)Selectasvector(context);
        Doublelocking _lock(this, v);
        if (values.find(val) != values.end())
            v->values.push_back(val);

        return v;
    }

    Locking _lock(this);
    if (values.find(val) != values.end())
        return globalAtanor->Providestring(val);

    return aNULL;

}

Exporting Atanor* Atanormap::MethodFind(Atanor* context, short idthread, AtanorCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    Atanor* a = callfunc->Evaluate(0, context, idthread);

    if (context->isBoolean()) {
        Locking _lock(this);
        for (auto& it : values) {
            if (it.second->same(a) == aTRUE)
                return aTRUE;
        }
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Atanorsvector* v = (Atanorsvector*)Selectasvector(context);
        Doublelocking _lock(this, v);

        for (auto& it : values) {
            if (it.second->same(a) == aTRUE)
                v->values.push_back(it.first);
        }
        return v;
    }

    Locking _lock(this);
    for (auto& it : values) {
        if (it.second->same(a) == aTRUE)
            return globalAtanor->Providestring(it.first);
    }

    return aNULL;

}
Exporting void Atanormap::Setreference(short inc) {
    if (idtracker == CIRCULARDETECTION) {
        globalAtanor->Returnerror("Error: Circularity in container", globalAtanor->GetThreadid());
        return;
    }

    int id = idtracker;
    idtracker = CIRCULARDETECTION;
    reference += inc;
    Setprotect(0);
    vector<Atanor*> doubles;
    {
        Locking _lock(this);
        for (auto& itx : values) {
            if (itx.second != NULL)
                doubles.push_back(itx.second);
        }
    }
    for (int itx = 0; itx < doubles.size(); itx++)
        doubles[itx]->Setreference(inc);
    idtracker = id;
}

static void resetMap(Atanormap* kmap, long inc) {
    Locking _lock(kmap);
    kmap->reference -= inc;

    hmap<string, Atanor*>& values = kmap->values;
    if (values.size() == 0)
        return;
        
    for (auto& itx : values)
        itx.second->Resetreference(inc);
}

Exporting void Atanormap::Resetreference(short inc) {
    if ((reference - inc) > 0)
        resetMap(this, inc);
    else {
        resetMap(this, inc + 1 - protect);
        if (!protect) {
            if (idtracker != -1)
                globalAtanor->RemoveFromTracker(idtracker);
            delete this;
        }
    }
}

Exporting void Atanormapbuff::Resetreference(short inc) {
    bool clr = false;
    if ((reference - inc) > 0)
        resetMap(this, inc);
    else {
        resetMap(this, inc + 1 - protect);
        if (!protect) {
            clr = true;
            reference = 0;
            protect = true;
        }
    }

    if (clr) {
        Locking _lock(globalAtanor->maplock);
        values.clear();
        used = false;
        globalAtanor->mapempties.push_back(idx);
    }
}


Exporting Atanor* Atanormap::Push(Atanor* k, Atanor* v) {
    Locking _lock(this);
    string s = k->String();
    if (values.find(s) != values.end()) {
        Atanor* kval = values[s];
        values.erase(s);
        kval->Resetreference(reference + 1);
    }

    v = v->Atom();
    values[s] = v;
    v->Setreference(reference + 1);
    return aTRUE;
}

Exporting Atanor* Atanormap::Pop(Atanor* kkey) {
    Locking _lock(this);
    string k = kkey->String();
    if (values.find(k) != values.end()) {
        kkey = values[k];
        values.erase(k);
        kkey->Resetreference(reference + 1);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Atanormap::Clear() {
    hmap<string, Atanor*>::iterator itx;
    for (auto& itx : values)
        itx.second->Resetreference(reference + 1);
    values.clear();
}

Exporting void Atanormap::Clean() {
    //Your cleaning code
}

Exporting string Atanormap::String() {
    Locking _lock(this);
    stringstream res;
    
    res << "{";
    bool beg = true;
    string sx;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        sx = it.first;
        stringing(res, sx);
        res << ":";
        sx = it.second->StringToDisplay();
        if (!it.second->isString() || it.second->isContainer())
            res << sx;
        else
            stringing(res, sx);
    }
    res << "}";
    return res.str();
}

Exporting string Atanormap::JSonString() {
    Locking _lock(this);
    stringstream res;
    
    res << "{";
    bool beg = true;
    string sx;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        sx = it.first;
        jstringing(res, sx);
        res << ":" << it.second->JSonString();
    }
    res << "}";
    return res.str();
}


Exporting long Atanormap::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Atanormap::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Atanormap::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Atanormap::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Atanormap::Size() {
    Locking _lock(this);
    return values.size();
}


Atanor*  Atanormap::Put(Atanor* idx, Atanor* ke, short idthread) {
    if (ke == this)
        return aTRUE;
    Doublelocking _lock(this, ke);
    int nb;
    if (!idx->isIndex()) {
        if (ke == aNULL) {
            Clear();
            return aTRUE;
        }
        
        if (ke->isMapContainer()) {
            Clear();
            AtanorIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                Push(itr->Keystring(), itr->Value());
            itr->Release();
            return aTRUE;
        }

        char ch[20];
        if (ke->isVectorContainer()) {
            Clear();
            nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                sprintf_s(ch, 20, "%d", nb);
                Push(ch, ke->getvalue(it));
                nb++;
            }
            return aTRUE;
        }
        if (ke->Type() == a_list) {
            Atanorlist* kvect = (Atanorlist*)ke;
            Clear();
            nb = 0;

            for (auto& it : kvect->values) {
                sprintf_s(ch, 20, "%d", nb);
                Push(ch, it);
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalAtanor->Returnerror("Wrong map initialization", idthread);
        if (ke != this) {
            Clear();
            if (ke->Type() == Atanormap::idtype) {
                Atanormap* kmap = (Atanormap*)ke;
                //We copy all values from ke to this
                
                for (auto& it : kmap->values)
                    Push(it.first, it.second);
                kmap->Release();
            }
            else {
                AtanorIteration* itr = ke->Newiteration(false);
                Atanor* a;
                for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
                    a=itr->IteratorValue();
                    a=a->Atom();
                    values[itr->Keystring()] = a;
                    a->Setreference(reference + 1);
                }
                itr->Release();
            }
            ke->Release();
        }
        return aTRUE;
    }
    Push(STR(idx->String()), ke);
    return aTRUE;
}

Atanor* Atanormap::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Atanor* vect = contextualpattern->Newinstance(idthread);
            
            for (auto& it : values)
                vect->Push(globalAtanor->Providestring(it.first));
            return vect;
        }

        if (contextualpattern->Type() == a_int || contextualpattern->Type() == a_float)
            return globalAtanor->Provideint(values.size());

        return this;
    }

    Atanor* key;
    if (idx->isInterval()) {
        Atanormap* kmap = globalAtanor->Providemap();
        key = ((AtanorIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Atanor* keyright = ((AtanorIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        string vleft = key->String();
        string vright = keyright->String();
        hmap<string, Atanor*>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        hmap<string, Atanor*>::iterator itr = values.end();
        if (keyright != aNULL) {
            itr = values.find(vright);
            if (itr == values.end()) {
                keyright->Release();
                return kmap;
            }
        }
        keyright->Release();
        for (; it != values.end(); it++) {
            kmap->Push(it->first, it->second);
            if (it == itr)
                return kmap;
        }

        if (itr != values.end())
            kmap->Clear();
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

Atanor* Atanormap::same(Atanor* a) {

    if (a->Type() != a_map)
        return Mapcompare(this, a, NULL);

    Atanormap* m = (Atanormap*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    hmap<string, Atanor*>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second->same(values[it->first]) == aFALSE)
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Atanor* Atanormap::xorset(Atanor* b, bool itself) {

    Doublelocking _lock(this, b);


    Atanormap* res;
    
    
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = globalAtanor->Providemap();
        hmap<string, Atanor*> keys;

        for (auto& it : values)
            keys[it.first] = it.second;
            
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keystring();
            if (keys.find(v) == keys.end())
                keys[v]=itr->IteratorValue();
            else {
                if (values[v]->same(itr->IteratorValue()) == aTRUE)
                    keys.erase(v);
            }
        }
        itr->Release();

        for (auto& a : keys)
            res->Push(a.first,a.second);

        return res;


    }


    if (itself)
        res = this;
    else
        res = (Atanormap*)Atom(true);

    
    for (auto& it : values)
        it.second->xorset(b, true);
    return res;
}

Exporting Atanor* Atanormap::orset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    
    Atanormap* res;
    if (itself)
        res = this;
    else
        res = (Atanormap*)Atom(true);

    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }

    for (auto& it : res->values)
        it.second->orset(b, true);
    return res;

}

Exporting Atanor* Atanormap::andset(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    
    Atanormap* res;
    if (b->isMapContainer()) {
        AtanorIteration* itr = b->Newiteration(false);

        res = globalAtanor->Providemap();
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            if (values.find(itr->Keystring()) != values.end() && values[itr->Keystring()]->same(v) == aTRUE)
                res->Push(itr->Keystring(), v);
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanormap*)Atom(true);

    for (auto& it : res->values)
        it.second->andset(b, true);
    return res;
}

Exporting Atanor* Atanormap::plus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanormap* res;
    if (b->isMapContainer()) {
        hmap<string, Atanor*>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = globalAtanor->Providemap();
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keystring());
            if (it != values.end()) {
                res->Push(it->first, it->second);
                res->values[it->first]->plus(v, true);
            }
        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanormap*)Atom(true);

    for (auto& it : res->values)
        it.second->plus(b, true);
    return res;
}

Exporting Atanor* Atanormap::minus(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanormap* res;
    if (b->isMapContainer()) {
        hmap<string, Atanor*>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = globalAtanor->Providemap();
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keystring());
            if (it != values.end()) {
                res->Push(it->first, it->second);
                res->values[it->first]->minus(v, true);
            }
        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanormap*)Atom(true);

    for (auto& it : res->values)
        it.second->minus(b, true);
    return res;
}

Exporting Atanor* Atanormap::multiply(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanormap* res;
    if (b->isMapContainer()) {
        hmap<string, Atanor*>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = globalAtanor->Providemap();
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keystring());
            if (it != values.end()) {
                res->Push(it->first, it->second);
                res->values[it->first]->multiply(v, true);
            }
        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanormap*)Atom(true);

    for (auto& it : res->values)
        it.second->multiply(b, true);
    return res;
}

Exporting Atanor* Atanormap::divide(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanormap* res;
    Atanor* v;
    Atanor* r;

    if (b->isMapContainer()) {
        hmap<string, Atanor*>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = globalAtanor->Providemap();
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keystring());
            if (it != values.end()) {
                r = res->values[it->first]->divide(v, true);
                if (r->isError()) {
                    res->Release();
                    itr->Release();
                    return r;
                }
            }
        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanormap*)Atom(true);

    for (auto& it : res->values) {
        r = it.second->divide(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Atanor* Atanormap::mod(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanormap* res;
    Atanor* v;
    Atanor* r;

    if (b->isMapContainer()) {
        hmap<string, Atanor*>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = globalAtanor->Providemap();
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keystring());
            if (it != values.end()) {
                r = res->values[it->first]->mod(v, true);
                if (r->isError()) {
                    res->Release();
                    itr->Release();
                    return r;
                }
            }
        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanormap*)Atom(true);

    for (auto& it : res->values) {
        r = it.second->mod(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Atanor* Atanormap::shiftright(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanormap* res;
    if (b->isMapContainer()) {
        hmap<string, Atanor*>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = globalAtanor->Providemap();
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keystring());
            if (it != values.end()) {
                res->Push(it->first, it->second);
                res->values[it->first]->shiftright(v, true);
            }
        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanormap*)Atom(true);

    for (auto& it : res->values)
        it.second->shiftright(b, true);
    return res;
}

Exporting Atanor* Atanormap::shiftleft(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanormap* res;
    if (b->isMapContainer()) {
        hmap<string, Atanor*>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = globalAtanor->Providemap();
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keystring());
            if (it != values.end()) {
                res->Push(it->first, it->second);
                res->values[it->first]->shiftleft(v, true);
            }
        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanormap*)Atom(true);

    for (auto& it : res->values)
        it.second->shiftleft(b, true);
    return res;
}

Exporting Atanor* Atanormap::power(Atanor* b, bool itself) {
    Doublelocking _lock(this, b);

    Atanormap* res;
    if (b->isMapContainer()) {
        hmap<string, Atanor*>::iterator it;

        AtanorIteration* itr = b->Newiteration(false);

        res = globalAtanor->Providemap();
        Atanor* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keystring());
            if (it != values.end()) {
                res->Push(it->first, it->second);
                res->values[it->first]->power(v, true);
            }
        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Atanormap*)Atom(true);

    for (auto& it : res->values)
        it.second->power(b, true);
    return res;
}


Exporting Atanor* Atanormap::Loopin(AtanorInstruction* ins, Atanor* context, short idthread) {
    Locking _lock(this);
    Atanor* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    hmap<string, Atanor*>::iterator it;
    
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
