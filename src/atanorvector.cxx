
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
filename   : atanorvector.cxx
Date       : 2017/04/01
Purpose    : vector implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorhaskell.h"
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
Exporting hmap<unsigned short, vectorMethod>  Atanorvector::methods;
Exporting hmap<string, string> Atanorvector::infomethods;
Exporting bin_hash<unsigned long> Atanorvector::exported;

Exporting short Atanorvector::idtype = 0;

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorvector::AddMethod(AtanorGlobal* global, string name, vectorMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorvector::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();


	Atanorvector::idtype = global->Getid("vector");

	Atanorvector::AddMethod(global, "clear", &Atanorvector::MethodClear, P_NONE, "clear(): clear the container.");
	Atanorvector::AddMethod(global, "flatten", &Atanorvector::MethodFlatten, P_NONE, "flatten(): flatten a vector structure.");

	Atanorvector::AddMethod(global, "remove", &Atanorvector::MethodRemove, P_ONE, "remove(e): remove 'e' from the vector.");

	Atanorvector::AddMethod(global, "reverse", &Atanorvector::MethodReverse, P_NONE, "reverse(): reverse a vector.");
	Atanorvector::AddMethod(global, "unique", &Atanorvector::MethodUnique, P_NONE, "unique(): remove duplicate elements.");
	Atanorvector::AddMethod(global, "reserve", &Atanorvector::MethodReserve, P_ONE, "reserve(int sz): Reserve a size of 'sz' potential element in the vector.");
	Atanorvector::AddMethod(global, "join", &Atanorvector::MethodJoin, P_ONE, "join(string sep): Produce a string representation for the container.");
	Atanorvector::AddMethod(global, "shuffle", &Atanorvector::MethodShuffle, P_NONE, "shuffle(): shuffle the values in the vector.");
	Atanorvector::AddMethod(global, "last", &Atanorvector::MethodLast, P_NONE, "last(): return the last element.");
	Atanorvector::AddMethod(global, "sort", &Atanorvector::MethodSort, P_ONE, "sort(bool reverse | function): sort the values in the vector.");
	Atanorvector::AddMethod(global, "sortint", &Atanorvector::MethodSortint, P_ONE, "sortint(bool reverse | function): sort the values in the vector as int.");
	Atanorvector::AddMethod(global, "sortustring", &Atanorvector::MethodSortustring, P_ONE, "sortustring(bool reverse | function): sort the values in the vector as ustring.");
	Atanorvector::AddMethod(global, "sortstring", &Atanorvector::MethodSortstring, P_ONE, "sortstring(bool reverse): sort the values in the vector as string.");
	Atanorvector::AddMethod(global, "sortfloat", &Atanorvector::MethodSortfloat, P_ONE, "sortfloat(bool reverse): sort the values in the vector as float.");
	Atanorvector::AddMethod(global, "sum", &Atanorvector::MethodSum, P_NONE, "sum(): return the sum of elements.");
	Atanorvector::AddMethod(global, "product", &Atanorvector::MethodProduct, P_NONE, "product(): return the product of elements.");
	Atanorvector::AddMethod(global, "push", &Atanorvector::MethodPush, P_ATLEASTONE, "push(v): Push a value into the vector.");
	Atanorvector::AddMethod(global, "pop", &Atanorvector::MethodPop, P_NONE | P_ONE, "pop(i): Erase an element from the vector");
	Atanorvector::AddMethod(global, "merge", &Atanorvector::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");
	Atanorvector::AddMethod(global, "editdistance", &Atanorvector::MethodEditDistance, P_ONE, "editdistance(v): Compute the edit distance with vector 'v'.");
	Atanorvector::AddMethod(global, "insert", &Atanorvector::MethodInsert, P_TWO, "insert(i,v): Insert v at position i.");

	global->newInstance[Atanorvector::idtype] = new Atanorvector(global);
	global->RecordMethods(Atanorvector::idtype, Atanorvector::exported);
	global->newInstance[a_constvector] = new AtanorConstvector(global);
	global->RecordMethods(a_constvector, Atanorvector::exported);

	return true;
}

Exporting AtanorIteration* Atanorvector::Newiteration(bool direction) {
	return new AtanorIterationvector(this, direction);
}

Exporting bool Atanorvector::Permute() {
	return next_permutation(values.begin(), values.end());
}

Exporting void Atanorvector::Insert(int idx, Atanor* ke) {
	ke = ke->Atom();
	values.insert(values.begin() + idx, ke);
	ke->Setreference(reference + 1);
}

Exporting void Atanorvector::Setreference(short r) {
	if (idtracker == CIRCULARDETECTION) {
		globalAtanor->Returnerror("Error: Circularity in container", globalAtanor->GetThreadid());
		return;
	}

	int id = idtracker;
	idtracker = CIRCULARDETECTION;

	Locking _lock(this);
	protect = false;
	reference += r;
	for (size_t i = 0; i < values.size(); i++)
		values[i]->Setreference(r);

	idtracker = id;
}

static void resetVector(Atanorvector* kvect, long inc) {
	Locking _lock(kvect);
	kvect->reference -= inc;

	vector<Atanor*>& values = kvect->values;
	if (values.size() == 0)
		return;

	for (int itx = 0; itx < values.size(); itx++)
		values[itx]->Resetreference(inc);
}

Exporting void Atanorvector::Resetreference(short inc) {
	if ((reference - inc) > 0)
		resetVector(this, inc);
	else {
		resetVector(this, inc + 1 - protect);
		if (!protect) {
			if (idtracker != -1)
				globalAtanor->RemoveFromTracker(idtracker);
			delete this;
		}
	}
}


Exporting void Atanorvectorbuff::Resetreference(short inc) {
	bool clr = false;
	if ((reference - inc) > 0)
		resetVector(this, inc);
	else {
		resetVector(this, inc + 1 - protect);
		if (!protect) {
			clr = true;
			reference = 0;
			protect = true;
		}
	}

	if (clr) {
		Locking _lock(globalAtanor->vectorlock);
		values.clear();
		used = false;
		globalAtanor->vectorempties.push_back(idx);
	}
}

Exporting Atanor* Atanorvector::in(Atanor* context, Atanor* a, short idthread) {
	//Three cases along the container type...
	//It is a Boolean, we expect false or true
	//It is an integer, we expect a position in v
	//It is a container, we are looking for all positions...


	if (context->isBoolean()) {
		Locking _lock(this);
		for (size_t i = 0; i < values.size(); i++) {
			if (a->same(values[i]) == aTRUE)
				return aTRUE;
		}
		return aFALSE;
	}

	if (context->isVectorContainer()) {
		Atanorivector* v = (Atanorivector*)Selectaivector(context);
		Doublelocking _lock(this, v);
		for (size_t i = 0; i < values.size(); i++) {
			if (a->same(values[i]) == aTRUE)
				v->values.push_back(i);
		}
		return v;
	}

	Locking _lock(this);
	for (size_t i = 0; i < values.size(); i++) {
		if (a->same(values[i]) == aTRUE)
			return globalAtanor->Provideint(i);
	}

	return aNULL;
}

Exporting Atanor* Atanorvector::Push(Atanor* a) {
	Locking _lock(this);
	a = a->Atom();
	a->Setreference(reference + 1);
	values.push_back(a);
	return aTRUE;
}

Exporting Atanor* Atanorvector::Push(AtanorGlobal* g, Atanor* a, short idhtread) {
	Locking _lock(this);
	a = a->Atom();
	a->Setreference(reference + 1);
	values.push_back(a);
	return aTRUE;
}

Exporting Atanor* Atanorvector::Pop(Atanor* idx) {
	Locking _lock(this);
	if (!values.size())
		return aFALSE;

	BLONG v = idx->Integer();

	if (v == -1) {
		idx = values.back();
		values.pop_back();
	}
	else {
		if (v < 0 || v >= (BLONG)values.size())
			return aFALSE;
		idx = values[v];
		values.erase(values.begin() + v);
	}

	idx->Resetreference(reference + 1);
	return aTRUE;
}

Exporting void Atanorvector::Clean() {
	//Your cleaning code
}

Exporting string Atanorvector::String() {
	Locking _lock(this);
	string res;
	int it;
	res = "[";
	bool beg = true;
	string sx;
	Atanor* element;
	for (it = 0; it < values.size(); it++) {
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

Exporting Atanor* Atanorvector::Map(short idthread) {
	Locking _lock(this);
	Atanormap* kmap = globalAtanor->Providemap();
	char ch[20];
	for (int it = 0; it < values.size(); it++) {
		sprintf_s(ch, 20, "%d", it);
		kmap->Push(ch, values[it]);
	}
	return kmap;
}

Exporting string Atanorvector::JSonString() {
	Locking _lock(this);
	string res;
	int it;
	res = "[";
	bool beg = true;
	string sx;
	Atanor* element;
	for (it = 0; it < values.size(); it++) {
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

Exporting long Atanorvector::Integer() {
	Locking _lock(this);
	return values.size();
}

Exporting double Atanorvector::Float() {
	Locking _lock(this);
	return values.size();
}

Exporting BLONG Atanorvector::Long() {
	Locking _lock(this);
	return values.size();
}

Exporting bool Atanorvector::Boolean() {
	Locking _lock(this);
	if (values.size())
		return true;
	return false;
}

//Basic operations
Exporting long Atanorvector::Size() {
	Locking _lock(this);
	return values.size();
}

Exporting Atanor* Atanorvector::andset(Atanor *b, bool itself) {
	Atanorvector* ref;

	long size = Size();
	size_t it;
	Atanor* ke;
	if (!b->isContainer()) {
		if (itself)
			ref = this;
		else
			ref = (Atanorvector*)Atom(true);

		Locking _lock(ref);

		for (it = 0; it < size; it++) {
			ke = ref->values[it]->andset(b, true);
			if (ke->isError()) {
				ref->Release();
				return ke;
			}
		}
		return ref;
	}

	ref = globalAtanor->Providevector();
	Doublelocking _lock(this, b);

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

Exporting Atanor* Atanorvector::orset(Atanor *b, bool itself) {
	Atanorvector* ref;

	long size = Size();
	size_t it;
	Atanor* ke;
	if (itself)
		ref = this;
	else
		ref = (Atanorvector*)Atom(true);

	if (!b->isContainer()) {
		Locking _lock(ref);

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

Exporting Atanor* Atanorvector::xorset(Atanor *b, bool itself) {
	Atanorvector* ref;

	long size = Size();
	size_t it;
	Atanor* ke;
	if (!b->isContainer()) {
		if (itself)
			ref = this;
		else
			ref = (Atanorvector*)Atom(true);

		Locking _lock(ref);

		for (it = 0; it < size; it++) {
			ke = ref->values[it]->xorset(b, true);
			if (ke->isError()) {
				ref->Release();
				return ke;
			}
		}
		return ref;
	}

	ref = globalAtanor->Providevector();
	Doublelocking _lock(this, b);

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


Exporting Atanor* Atanorvector::plus(Atanor* b, bool itself) {
	Atanorvector* ref;
	if (itself)
		ref = this;
	else
		ref = (Atanorvector*)Atom(true);

	int it;
	Atanor* ke;
	Atanor* kv;
	Doublelocking _lock(ref, b);
	long size = values.size();

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

Exporting Atanor* Atanorvector::minus(Atanor *b, bool itself) {
	Atanorvector* ref;
	if (itself)
		ref = this;
	else
		ref = (Atanorvector*)Atom(true);

	int it;
	Atanor* ke;
	Atanor* kv;

	Locking _lock(ref);
	long size = values.size();

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

Exporting Atanor* Atanorvector::multiply(Atanor *b, bool itself) {
	Atanorvector* ref;
	if (itself)
		ref = this;
	else
		ref = (Atanorvector*)Atom(true);

	int it;
	Atanor* ke;
	Atanor* kv;

	Locking _lock(ref);
	long size = values.size();

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

Exporting Atanor* Atanorvector::divide(Atanor *b, bool itself) {
	Atanorvector* ref;
	if (itself)
		ref = this;
	else
		ref = (Atanorvector*)Atom(true);

	int it;
	Atanor* ke;
	Atanor* kv;

	Locking _lock(ref);
	long size = values.size();

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

Exporting Atanor* Atanorvector::power(Atanor *b, bool itself) {
	Atanorvector* ref;
	if (itself)
		ref = this;
	else
		ref = (Atanorvector*)Atom(true);

	int it;
	Atanor* ke;
	Atanor* kv;

	Locking _lock(ref);
	long size = values.size();

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

Exporting Atanor* Atanorvector::shiftleft(Atanor *b, bool itself) {
	Atanorvector* ref;
	if (itself)
		ref = this;
	else
		ref = (Atanorvector*)Atom(true);

	int it;
	Atanor* ke;
	Atanor* kv;

	Locking _lock(ref);
	long size = values.size();

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

Exporting Atanor* Atanorvector::shiftright(Atanor *b, bool itself) {
	Atanorvector* ref;
	if (itself)
		ref = this;
	else
		ref = (Atanorvector*)Atom(true);

	int it;
	Atanor* kv;

	Atanor* ke;
	Locking _lock(ref);
	long size = values.size();

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

Exporting Atanor* Atanorvector::mod(Atanor *b, bool itself) {
	Atanorvector* ref;
	if (itself)
		ref = this;
	else
		ref = (Atanorvector*)Atom(true);

	int it;
	Atanor* ke;
	Atanor* kv;

	Locking _lock(ref);
	long size = values.size();

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


Exporting AtanorIteration* AtanorInfinitevector::Newiteration(bool rev) {
	Atanor* s = seed;
	Atanor* st = step;

	short idthread = -1;
	if (!seed->isNumber())
		s = seed->Put(aNULL, aNULL, globalAtanor->GetThreadid());

	if (!step->isNumber())
		st = step->Put(aNULL, aNULL, globalAtanor->GetThreadid());

	if (compute) {//In this case, the step is a difference between the two values step-seed
		if (s->Type() == a_float || st->Type() == a_float) {
			double d = st->Float() - s->Float();
			AtanorIterationInfinitefloatvector* iter = new AtanorIterationInfinitefloatvector(false);
			iter->itx = s->Float();
			iter->inc = d;
			if (s != seed)
				s->Release();
			if (st != step)
				st->Release();
			return iter;
		}

		long d = st->Integer() - s->Integer();
		AtanorIterationInfinitevector* iter = new AtanorIterationInfinitevector(false);
		iter->itx = s->Integer();
		iter->inc = d;
		if (s != seed)
			s->Release();
		if (st != step)
			st->Release();
		return iter;
	}

	if (s->Type() == a_float || st->Type() == a_float) {
		AtanorIterationInfinitefloatvector* iter = new AtanorIterationInfinitefloatvector(false);
		iter->itx = s->Float();
		iter->inc = st->Float();
		if (s != seed)
			s->Release();
		if (st != step)
			st->Release();
		return iter;
	}

	AtanorIterationInfinitevector* iter = new AtanorIterationInfinitevector(false);
	iter->itx = s->Integer();
	iter->inc = st->Integer();
	if (s != seed)
		s->Release();
	if (st != step)
		st->Release();
	return iter;
}

Exporting string AtanorInfinitevector::String() {
	stringstream res;
	if (direction)
		res << "[.." << seed->String() << "]";
	else
		res << "[" << seed->String() << "..]";
	return res.str();
}



Exporting Atanor*  Atanorvector::Put(Atanor* idx, Atanor* value, short idthread) {
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
			Reserve(value->Size());
			{

				for (auto& it : kvect->values)
					Push(it);
			}
			return aTRUE;
		}
		if (value->isVectorContainer()) {
			//We copy all values from value to this
			Clear();
			long sz = value->Size();
			Reserve(sz);
			for (int it = 0; it < sz; it++)
				Push(value->getvalue(it));
			return aTRUE;
		}
		//We gather all the keys from the MAP
		if (value->isMapContainer()) {
			Clear();
			Reserve(value->Size());
			AtanorIteration* itr = value->Newiteration(false);
			for (itr->Begin(); itr->End() == aFALSE; itr->Next())
				Push(itr->Key());
			itr->Release();
			return aTRUE;
		}

		value = value->Vector(idthread);
		if (!value->isVectorContainer())
			return globalAtanor->Returnerror("Cannot set this value", idthread);

		if (value != this) {
			Clear();
			Reserve(value->Size());
			//We copy all values from ke to this
			for (long it = 0; it < value->Size(); ++it)
				Push(value->getvalue(it));
			value->Release();
		}
		return aTRUE;
	}
	//In this specific case, we try to replace a bloc of values with a new bloc
	if (idx->isInterval()) {
		//First we remove the values...
		long lkey = idx->Integer();
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
		Atanor* krkey;
		while (rkey >= lkey) {
			krkey = values[rkey];
			values.erase(values.begin() + rkey);
			krkey->Resetreference(reference + 1);
			rkey--;
		}
		if (value->isVectorContainer()) {
			for (int i = value->Size() - 1; i >= 0; i--) {
				krkey = value->getvalue(i);
				krkey = krkey->Atom();
				values.insert(values.begin() + lkey, krkey);
				krkey->Setreference(reference + 1);
			}
			return aTRUE;
		}
		if (value->isContainer()) {
			AtanorIteration* itr = value->Newiteration(false);
			for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
				krkey = itr->Value();
				if (krkey->Reference())
					krkey = krkey->Atom();
				values.insert(values.begin() + lkey, krkey);
				krkey->Setreference(reference + 1);
			}
			itr->Release();
			return aTRUE;
		}
		krkey = value->Atom();
		values.insert(values.begin() + lkey, krkey);
		krkey->Setreference(reference + 1);
		return aTRUE;
	}
	register long ikey = idx->Integer();
	int nb, mx;
	mx = values.size();
	if (ikey >= mx) {
		for (nb = values.size(); nb < ikey; nb++)
			values.push_back(aNULL);
		Push(value);
	}
	else {
		if (ikey < 0) {
			ikey = mx + ikey;
			if (ikey < 0)
				return globalAtanor->Returnerror("Cannot set this value", idthread);
		}
		value = value->Atom();
		if (values[ikey] != NULL)
			values[ikey]->Resetreference(reference + 1);
		values[ikey] = value;
		value->Setreference(reference + 1);
	}
	return aTRUE;
}


Exporting Atanor* Atanorvector::Get(Atanor* contextualpattern, Atanor* idx, short idthread) {
	Locking _lock(this);

	if (!idx->isIndex()) {

		Atanor* ke;
		//In this case, we copy the elements from the vector to the map, using the position as index
		if (contextualpattern->isMapContainer()) {
			Atanor* map = Selectamap(contextualpattern);
			size_t i = 0;
			char ch[20];
			for (int it = 0; it < values.size(); it++) {
				sprintf_s(ch, 20, "%d", i);
				ke = values[it];
				map->Push(ch, ke);
				i++;
			}
			return map;
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

	register long ikey;
	bool stringkey = false;
	if (key->Type() == a_string) {
		string sf = key->String();
		stringkey = true;
		bool found = false;
		if (kind->signleft) {
			for (ikey = values.size() - 1; ikey >= 0; ikey--) {
				if (sf == values[ikey]->String()) {
					found = true;
					break;
				}
			}

		}
		else {
			for (ikey = 0; ikey < values.size(); ikey++) {
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
		ikey = values.size() + ikey;

	if (ikey < 0 || ikey >= values.size()) {
		if (ikey != values.size() || keyright == NULL) {
			if (globalAtanor->erroronkey)
				return globalAtanor->Returnerror("Wrong index", idthread);
			return aNOELEMENT;
		}
	}

	if (keyright == NULL)
		return values[ikey];

	Atanorvector* kvect;
	long iright;
	if (keyright->Type() == a_string) {
		string sf = keyright->String();
		bool found = false;
		if (kind->signright) {
			for (iright = values.size() - 1; iright >= 0; iright--) {
				if (sf == values[iright]->String()) {
					found = true;
					iright++;
					break;
				}
			}
		}
		else {
			for (iright = 0; iright < values.size(); iright++) {
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

	//In this case, we must create a new vector
	kvect = globalAtanor->Providevector();
	for (int i = ikey; i < iright; i++)
		kvect->Push(values[i]);

	return kvect;
}

Exporting void Atanorvector::Shuffle() {
	size_t sz = values.size();
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


Exporting Atanor* Atanorvector::Unique() {
	Locking _lock(this);
	Atanorvector* kvect = globalAtanor->Providevector();
	map<string, Atanor*> inter;
	string k;
	for (int i = 0; i < values.size(); i++) {
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


Exporting void Atanorvector::Clear() {
	Locking _lock(this);
	for (int itx = 0; itx < values.size(); itx++)
		values[itx]->Resetreference(reference + 1);
	values.clear();
}

Exporting Atanor* Atanorvector::Merging(Atanor* ke) {
	if (!ke->isContainer()) {
		Push(ke);
		return this;
	}

	if (ke->Size() == 0)
		return this;

	Doublelocking _lock(this, ke);
	//Three cases:
	if (ke->Type() == idtype) {
		Atanorvector* kvect = (Atanorvector*)ke;
		for (long i = 0; i < kvect->values.size(); i++) {
			ke = kvect->values[i]->Atom();
			ke->Setreference(reference + 1);
			values.push_back(ke);
		}
		return this;
	}

	if (ke->isVectorValueContainer()) {
		Atanor* k;
		for (long i = 0; i < ke->Size(); i++) {
			k = ke->getvalue(i);
			k->Setreference(reference + 1);
			values.push_back(k);
		}
		return this;
	}

	AtanorIteration* itr = ke->Newiteration(false);
	for (itr->Begin(); itr->End() == aFALSE; itr->Next())
		Push(itr->Value());
	itr->Release();
	return this;
}


Exporting Atanor* Atanorvector::same(Atanor* a) {
	Doublelocking _lock(this, a);

	if (a->Type() != idtype) {
		if (a->isVectorContainer()) {
			if (a->Size() != values.size())
				return aFALSE;
			Atanor* v;
			for (int i = 0; i < values.size(); i++) {
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

	Atanorvector* v = (Atanorvector*)a;

	if (values.size() != v->values.size())
		return aFALSE;

	for (int i = 0; i < values.size(); i++) {
		if (values[i]->same(v->values[i]) == aFALSE)
			return aFALSE;
	}
	return aTRUE;
}

Exporting unsigned int Atanorvector::EditDistance(Atanor* e) {
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

Exporting void AtanorConstvector::Prepare(Atanor* env, short idthread) {
	Atanor* v;
	for (size_t i = 0; i < values.size(); i++) {
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

Exporting bool AtanorConstvector::Checkvariable() {
	if (values.size() == 0)
		return false;

	for (size_t i = 0; i < values.size(); i++) {
		if (!values[i]->isCallVariable())
			return false;
	}

	return true;
}

Exporting bool AtanorConstvector::Setvalue(Atanor* index, Atanor* value, short idthread, bool strict) {
	Locking _lock(this);
	Atanor* a;
	Atanor* v;
	for (size_t i = 0; i < values.size(); i++) {
		if (i >= value->Size()) {
			globalAtanor->Returnerror("Out of range affectation.", idthread);
			return false;
		}

		v = value->getvalue(i);
		a = values[i];
		a->Setaffectation(true);
		a = a->Get(aNULL, aNULL, idthread);
		a->Putvalue(v, idthread);
		v->Release();
	}

	return true;
}

Exporting Atanor* AtanorConstvector::same(Atanor* value) {
	if (values.size() == 0) {
		if (value->Size() == 0)
			return aTRUE;
		return aFALSE;
	}

	size_t sz = value->Size();
	if (!isMerge()) {
		if (!values.back()->isMerge()) {
			if (values.size() != sz)
				return aFALSE;
		}
		else {
			if (!sz)
				return aFALSE;
		}
	}

	short idthread = globalAtanor->GetThreadid();
	Atanor* a;
	Atanor* v;
	bool clean = true;
	for (size_t i = 0; i < values.size(); i++) {
		a = values[i];

		if (a->isMerge()) {
			AtanorIndex idx(true);
			idx.left = globalAtanor->Provideint(i);
			idx.right = aNULL;

			v = value->Get(aNULL, &idx, idthread);
			a = a->same(v);
			v->Release();
			return a;
		}

		if (isMerge()) {
			v = value;
			clean = false;
		}
		else {
			clean = true;
			if (i >= sz)
				return aFALSE;
			v = value->getvalue(i);
		}

		if (a->isCallVariable()) {
			a->Setaffectation(true);
			a = a->Get(aNULL, aNULL, idthread);
			a->Putvalue(v, idthread);
			if (clean)
				v->Release();
			continue;
		}

		//a comes from the const vector, while v comes from the value to match the const vector with...
		if (a->isVectorContainer()) {
			if (!v->isVectorContainer()) {
				if (clean)
					v->Release();
				return globalAtanor->Returnerror("Mismatch assignment, expecting two vector containers.", idthread);
			}

			a->Putvalue(v, idthread);
			if (clean)
				v->Release();
			if (a->isError())
				return a;

			continue;
		}

		if (a->isMapContainer()) {
			if (!v->isMapContainer()) {
				if (clean)
					v->Release();
				return globalAtanor->Returnerror("Mismatch assignment, expecting two map containers.", idthread);
			}

			a->Putvalue(v, idthread);
			if (clean)
				v->Release();
			if (a->isError())
				return a;
			continue;
		}

		a = a->Get(aNULL, aNULL, idthread);
		if (a != aNOELEMENT && a != aUNIVERSAL && a->same(v) == aFALSE) {
			if (clean)
				v->Release();
			return aFALSE;
		}

		if (clean)
			v->Release();
	}
	return aTRUE;
}


Exporting Atanor* AtanorConstvector::Put(Atanor* index, Atanor* value, short idthread) {
	if (!evaluate)
		return this;

	Locking _lock(this);
	if (values.size() == 0) {
		if (value->Size() == 0)
			return aTRUE;
		if (index == aRAISEERROR)
			return index;
		return globalAtanor->Returnerror("Size mismatch.", idthread);
	}

	size_t sz = value->Size();
	if (!isMerge()) {
		if (!values.back()->isMerge()) {
			if (values.size() != sz) {
				if (index == aRAISEERROR)
					return index;
				return globalAtanor->Returnerror("Size mismatch.", idthread);
			}
		}
		else {
			if (!sz) {
				if (index == aRAISEERROR)
					return index;
				return globalAtanor->Returnerror("Size mismatch.", idthread);
			}
		}
	}

	Atanor* a;
	Atanor* v;
	bool clean = true;
	for (size_t i = 0; i < values.size(); i++) {
		a = values[i];
		if (a->isMerge()) {
			a->Setevaluate(true);
			AtanorIndex idx(true);
			idx.left = globalAtanor->Provideint(i);
			idx.right = aNULL;

			v = value->Get(aNULL, &idx, idthread);
			return a->Put(index, v, idthread);
		}


		if (isMerge()) {
			v = value;
			clean = false;
		}
		else {
			clean = true;
			if (i >= sz) {
				if (index == aRAISEERROR)
					return index;
				return globalAtanor->Returnerror("Out of range affectation.", idthread);
			}
			v = value->getvalue(i);
		}

		if (a->isCallVariable()) {
			a->Setaffectation(true);
			a = a->Get(aNULL, aNULL, idthread);
			a->Putvalue(v, idthread);
			if (clean)
				v->Release();
			continue;
		}

		if (a->isVariable()) {
			//Then it should have created before hand...
			a = globalAtanor->Getdeclaration(a->Name(), idthread);
			a->Putvalue(v, idthread);
			if (clean)
				v->Release();
			continue;
		}

		//a comes from the const vector, while v comes from the value to match the const vector with...
		if (a->isVectorContainer()) {
			if (!v->isVectorContainer()) {
				if (clean)
					v->Release();
				if (index == aRAISEERROR)
					return index;
				return globalAtanor->Returnerror("Mismatch assignment, expecting two vector containers.", idthread);
			}
			a = a->Put(index, v, idthread);
			if (clean)
				v->Release();
			if (a->isError())
				return a;

			continue;
		}

		if (a->isMapContainer()) {
			if (!v->isMapContainer()) {
				if (clean)
					v->Release();
				if (index == aRAISEERROR)
					return index;
				return globalAtanor->Returnerror("Mismatch assignment, expecting two map containers.", idthread);
			}
			a = a->Put(index, v, idthread);
			if (clean)
				v->Release();

			if (a->isError())
				return a;
			continue;
		}

		a = a->Get(aNULL, aNULL, idthread);
		if (a != aUNIVERSAL && a->same(v) == aFALSE) {
			if (clean)
				v->Release();
			if (index == aRAISEERROR)
				return index;
			return globalAtanor->Returnerror("Mismatch assignment.", idthread);
		}

		if (clean)
			v->Release();
	}
	return this;
}

Exporting Atanor* AtanorConstvector::Get(Atanor* index, Atanor* value, short idthread) {


	if (affectation && evaluate)
		return this;

	Atanorvector* kvect = globalAtanor->Providevector();

	Locking _lock(this);

	BLONG it;
	BLONG sz = values.size();
	if (isEvaluate()) {
		Atanor* v;
		for (it = 0; it < sz; it++) {
			v = values[it]->Get(aNULL, aNULL, idthread);
			kvect->Push(v);
			v->Release();
		}
	}
	else {
		for (it = 0; it < sz; it++)
			kvect->Push(values[it]);
	}
	return kvect;
}

Exporting string AtanorConstvectormerge::String() {
	Locking _lock(this);
	string res("|");
	int it;
	bool beg = true;
	string sx;
	Atanor* element;
	for (it = 0; it < values.size(); it++) {
		element = values[it];
		if (beg == false)
			res += ",";
		beg = false;
		sx = element->StringToDisplay();
		if (!element->isString() || element->isContainer())
			res += sx;
		else
			stringing(res, sx);
	}
	return res;
}

class Comp {
public:
	AtanorCallFunction compare;
	short idthread;

	Comp(AtanorFunction* comp, short idt) : compare(comp), idthread(idt) {
		compare.arguments.push_back(aNULL);
		compare.arguments.push_back(aNULL);
	}

	bool get(Atanor* i, Atanor* j) {
		compare.arguments[0] = i;
		compare.arguments[1] = j;
		return compare.Get(aNULL, aNULL, idthread)->Boolean();
	}
};

class Comparison {
public:
	Comp* compare;

	Comparison(Comp* c)  {
		compare = c;
	}

	bool operator() (Atanor* i, Atanor* j) {
		return compare->get(i, j);
	}
};

static bool CompareInteger(Atanor* e1, Atanor* e2) {
	if (e1->Integer() < e2->Integer())
		return true;
	return false;
}

static bool CompareShort(Atanor* e1, Atanor* e2) {
	if (e1->Short() < e2->Short())
		return true;
	return false;
}

static bool CompareLong(Atanor* e1, Atanor* e2) {
	if (e1->Long() < e2->Long())
		return true;
	return false;
}

static bool CompareFloat(Atanor* e1, Atanor* e2) {
	if (e1->Float() < e2->Float())
		return true;
	return false;
}

static bool CompareDecimal(Atanor* e1, Atanor* e2) {
	if (e1->Decimal() < e2->Decimal())
		return true;
	return false;
}

static bool CompareString(Atanor* e1, Atanor* e2) {
	if (e1->String() < e2->String())
		return true;
	return false;
}


static bool CompareUString(Atanor* e1, Atanor* e2) {
	if (e1->UString() < e2->UString())
		return true;
	return false;
}


static bool InvCompareInteger(Atanor* e1, Atanor* e2) {
	if (e1->Integer()>e2->Integer())
		return true;
	return false;
}

static bool InvCompareShort(Atanor* e1, Atanor* e2) {
	if (e1->Short() > e2->Short())
		return true;
	return false;
}

static bool InvCompareLong(Atanor* e1, Atanor* e2) {
	if (e1->Long() > e2->Long())
		return true;
	return false;
}

static bool InvCompareDecimal(Atanor* e1, Atanor* e2) {
	if (e1->Decimal() > e2->Decimal())
		return true;
	return false;
}

static bool InvCompareFloat(Atanor* e1, Atanor* e2) {
	if (e1->Float() > e2->Float())
		return true;
	return false;
}

static bool InvCompareString(Atanor* e1, Atanor* e2) {
	if (e1->String() > e2->String())
		return true;
	return false;
}

static bool InvCompareUString(Atanor* e1, Atanor* e2) {
	if (e1->UString() > e2->UString())
		return true;
	return false;
}

Exporting Atanor* Atanorvector::Sort(AtanorFunction* comp, short idthread, bool d) {
	//First parameter should be a vector
	if (Size() <= 1)
		return this;

	Locking _lock(this);

	if (comp != NULL) {
		Comp kcomp(comp, idthread);
		Comparison kfcomp(&kcomp);
		sort(values.begin(), values.end(), kfcomp);
		return this;
	}

	//we use the first element as a trigger
	Atanor* kfirst = values[0];
	if (!d) {
		switch (kfirst->Type()) {
		case a_short:
			sort(values.begin(), values.end(), CompareShort);
			break;
		case a_int:
			sort(values.begin(), values.end(), CompareInteger);
			break;
		case a_long:
			sort(values.begin(), values.end(), CompareLong);
			break;
		case a_decimal:
			sort(values.begin(), values.end(), CompareDecimal);
			break;
		case a_float:
			sort(values.begin(), values.end(), CompareFloat);
			break;
		case a_string: //else we only use strings for other types
			sort(values.begin(), values.end(), CompareString);
			break;
		case a_ustring: //else we only use strings for other types
			sort(values.begin(), values.end(), CompareUString);
		}
		return this;
	}

	switch (kfirst->Type()) {
	case a_short:
		sort(values.begin(), values.end(), InvCompareShort);
		break;
	case a_int:
		sort(values.begin(), values.end(), InvCompareInteger);
		break;
	case a_long:
		sort(values.begin(), values.end(), InvCompareLong);
		break;
	case a_decimal:
		sort(values.begin(), values.end(), InvCompareDecimal);
		break;
	case a_float:
		sort(values.begin(), values.end(), InvCompareFloat);
		break;
	case a_string: //else we only use strings for other types
		sort(values.begin(), values.end(), InvCompareString);
		break;
	case a_ustring: //else we only use strings for other types
		sort(values.begin(), values.end(), InvCompareUString);
	}

	return this;
}

Exporting Atanor* Atanorvector::Sortint(short idthread, bool d) {
	//First parameter should be a vector
	if (Size() <= 1)
		return this;

	Locking _lock(this);

	if (d)
		sort(values.begin(), values.end(), InvCompareInteger);
	else
		sort(values.begin(), values.end(), CompareInteger);
	return this;
}

Exporting Atanor* Atanorvector::Sortstring(short idthread, bool d) {
	//First parameter should be a vector
	if (Size() <= 1)
		return this;

	Locking _lock(this);

	if (d)
		sort(values.begin(), values.end(), InvCompareString);
	else
		sort(values.begin(), values.end(), CompareString);
	return this;
}

Exporting Atanor* Atanorvector::Sortustring(short idthread, bool d) {
	//First parameter should be a vector
	if (Size() <= 1)
		return this;

	Locking _lock(this);

	if (d)
		sort(values.begin(), values.end(), InvCompareUString);
	else
		sort(values.begin(), values.end(), CompareUString);
	return this;
}

Exporting Atanor* Atanorvector::Sortfloat(short idthread, bool d) {
	//First parameter should be a vector
	if (Size() <= 1)
		return this;

	Locking _lock(this);

	if (d)
		sort(values.begin(), values.end(), InvCompareFloat);
	else
		sort(values.begin(), values.end(), CompareFloat);
	return this;
}


Exporting Atanor* AtanorCycleVector::Get(Atanor* context, Atanor* v, short idthread) {
	value = base->Get(context, aNULL, idthread);
	value->Setreference();
	return this;
}

Exporting Atanor* AtanorCycleVector::Newinstance(short idthread, Atanor* f) {
	if (!repeat)
		return value->Newinstance(idthread);

	//Depends on the internal type of value...
	switch (value->Type()) {
	case a_int:
		return globalAtanor->Provideivector();
	case a_float:
		return globalAtanor->Providefvector();
	case a_string:
		return globalAtanor->Providesvector();
	case a_ustring:
		return globalAtanor->Provideuvector();
	}
	return globalAtanor->Providevector();
}

Exporting AtanorIteration* AtanorCycleVector::Newiteration(bool rev) {
	if (repeat)
		return new AtanorIteratorCycleElement(this);

	return new AtanorIteratorCycleVector(this);
}

Exporting Atanor* AtanorReplicateVector::Get(Atanor* context, Atanor* v, short idthread) {
	value = base->Get(context, aNULL, idthread);
	nb = nbbase->Get(context, aNULL, idthread);
	value->Setreference();
	nb->Setreference();
	return this;
}

Exporting Atanor* AtanorReplicateVector::Newinstance(short idthread, Atanor* f) {
	//Depends on the internal type of value...
	switch (value->Type()) {
	case a_int:
		return globalAtanor->Provideivector();
	case a_float:
		return globalAtanor->Providefvector();
	case a_string:
		return globalAtanor->Providesvector();
	case a_ustring:
		return globalAtanor->Provideuvector();
	}
	return globalAtanor->Providevector();
}

Exporting AtanorIteration* AtanorReplicateVector::Newiteration(bool rev) {
	AtanorIteratorReplicate* iter = new AtanorIteratorReplicate(this);
	return iter;
}


Exporting Atanor* Atanorvector::Loophaskell(Atanor* recipient, Atanor* context, Atanor* environment, AtanorFunctionLambda* bd, short idthread) {

	Atanor* a;
	char addvalue = 0;

	if (context != aNOELEMENT)
		addvalue = Selecttype(context);

	for (size_t i = 0; i < values.size(); i++) {
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

		if (a != aNULL && a != aNOELEMENT) {
			context = Storealongtype(context, a, idthread, addvalue);
			a->Release();
		}
	}

	recipient->Forcedclean();
	return context;
}

Exporting Atanor* Atanorvector::Filter(short idthread, Atanor* env, AtanorFunctionLambda* bd, Atanor* var, Atanor* kcont, Atanor* accu, Atanor* init, bool direct) {

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

	for (size_t i = 0; i < values.size(); i++) {
		key = values[i];
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
Exporting void Atanorvector::storevalue(string u) {
	Atanor* a = globalAtanor->Providestring(u);
	a->Setreference(reference + 1);
	Locking _lock(this);
	values.push_back(a);
}

Exporting void Atanorvector::storevalue(wstring u) {
	Atanor* a = globalAtanor->Provideustring(u);
	a->Setreference(reference + 1);
	Locking _lock(this);
	values.push_back(a);
}

Exporting void Atanorvector::storevalue(long u) {
	Atanor* a = globalAtanor->Provideint(u);
	a->Setreference(reference + 1);
	Locking _lock(this);
	values.push_back(a);
}

Exporting void Atanorvector::storevalue(short u) {
	Atanor* a = new Atanorshort(u);
	a->Setreference(reference + 1);
	Locking _lock(this);
	values.push_back(a);
}

Exporting void Atanorvector::storevalue(BLONG u) {
	Atanor* a = new Atanorlong(u);
	a->Setreference(reference + 1);
	Locking _lock(this);
	values.push_back(a);
}

Exporting void Atanorvector::storevalue(float u) {
	Atanor* a = new Atanordecimal(u);
	a->Setreference(reference + 1);
	Locking _lock(this);
	values.push_back(a);
}

Exporting void Atanorvector::storevalue(double u) {
	Atanor* a = globalAtanor->Providefloat(u);
	a->Setreference(reference + 1);
	Locking _lock(this);
	values.push_back(a);
}

Exporting void Atanorvector::storevalue(unsigned char u) {
	Atanor* a = new Atanorbyte(u);
	a->Setreference(reference + 1);
	Locking _lock(this);
	values.push_back(a);
}

Exporting void Atanorvector::storevalue(wchar_t u) {
	wstring w;
	w = u;
	Atanor* a = globalAtanor->Provideustring(w);
	a->Setreference(reference + 1);
	Locking _lock(this);
	values.push_back(a);
}


Exporting Atanor* Atanorvector::Thesum(long i, long j) {
	long sz = values.size();
	if (!sz)
		return aNULL;

	if (i < 0)
		i += sz;
	if (j < 0)
		j += sz;

	if (i < 0)
		i = 0;
	else
	if (i >= sz)
		i = sz - 1;

	if (j <= i)
		j = i + 1;

	if (j >= sz)
		j = sz - 1;

	if (values[0]->isContainer()) {
		Atanorfvector* sub = globalAtanor->Providefvector();
		Atanor* res;
		for (; i <= j; i++) {
			res = values[i]->Thesum(0, (long)values[i]->Size());
			sub->values.push_back(res->Float());
			res->Release();
		}
		return sub;
	}

	if (values[0]->isNumber()) {
		double v = 0;
		for (; i <= j; i++)
			v += values[i]->Float();

		return globalAtanor->Providefloat(v);
	}

	string v;
	for (; i < j; i++)
		v += values[i]->String();
	return globalAtanor->Providestring(v);
}

Exporting Atanor* Atanorvector::Theproduct(long i, long j) {
	long sz = values.size();
	if (!sz)
		return aNULL;

	if (i < 0)
		i += sz;
	if (j < 0)
		j += sz;

	if (i < 0)
		i = 0;
	else
	if (i >= sz)
		i = sz - 1;

	if (j <= i)
		j = i + 1;

	if (j >= sz)
		j = sz - 1;

	if (values[0]->isContainer()) {
		Atanorfvector* sub = globalAtanor->Providefvector();
		Atanor* res;
		for (; i <= j; i++) {
			res = values[i]->Theproduct(0, (long)values[i]->Size());
			sub->values.push_back(res->Float());
			res->Release();
		}
		return sub;
	}

	double v = 1;
	for (; i <= j; i++)
		v *= values[i]->Float();

	return globalAtanor->Providefloat(v);
}



