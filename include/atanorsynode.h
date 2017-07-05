
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
filename   : atanorsynode.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanorsynode_h
#define atanorsynode_h

#include "atanormapss.h"
#include "atanorsvector.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorsynode;
//This typedef defines a type "synodeMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorsynode::*synodeMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------

class Atanorsynode : public AtanorObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging hmap<unsigned short, synodeMethod> methods;
	static Exchanging hmap<string, string> infomethods;
	static Exchanging bin_hash<unsigned long> exported;

	static Exchanging Atanormapss* validfeatures;
	static Exchanging bool testvalid;

	static short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	Atanor* features;
	Atanorsynode* parent;
	Atanorsynode* child;
	Atanorsynode* next;
	Atanorsynode* previous;
	Atanorsynode* last;
	char buff[5];

	short id;
	short name;

	//---------------------------------------------------------------------------------------------------------------------
	Atanorsynode(short i, AtanorGlobal* g, Atanor* parnt = NULL) : AtanorObject(g, parnt) {
		//Do not forget your variable initialisation
		if (i != a_universal) {
			sprintf_s(buff, 5, "#%d", i);
			name = globalAtanor->Getid(buff);
		}
		else {
			strcpy_s(buff, 5, "_");
			name = a_universal;
		}

		features = aNULL;
		parent = NULL;
		child = NULL;
		next = NULL;
		previous = NULL;
		last = NULL;
		id = i;
	}

	Atanorsynode(short i = 0) {
		//Do not forget your variable initialisation
		sprintf_s(buff, 5, "#%d", i);
		name = globalAtanor->Getid(buff);
		id = i;

		features = aNULL;
		parent = NULL;
		child = NULL;
		next = NULL;
		previous = NULL;
		last = NULL;
	}

	Atanorsynode(Atanorsynode* syn, AtanorGlobal* g) : AtanorObject(g) {
		strcpy_s(buff, 5, syn->buff);
		id = syn->id;
		name = syn->name;

		features = aNULL;
		parent = NULL;
		child = NULL;
		next = NULL;
		previous = NULL;
		last = NULL;
	}

	void AddInstruction(Atanor* a) {
		features = a;
		features->Setreference(reference);
	}

	Exporting Atanor* ExtractPredicateVariables(Atanor* contextualpattern, AtanorDeclaration* dom, Atanor* c, Atanor* e, short idthread, bool root);
	Exporting bool Unify(AtanorDeclaration* dom, Atanor* a);
	Exporting bool isUnified(AtanorDeclaration* dom);
	bool Insertvalue(Atanor* dom, Atanor* v, basebin_hash<Atanor*>&) {
		if (v == this)
			return true;
		return false;
	}

	//----------------------------------------------------------------------------------------------------------------------
	Exporting Atanor* Put(Atanor* v, Atanor* i, short idthread);

	Exporting Atanor* Get(Atanor* context, Atanor* v, short idthread);


	short Type() {
		return Atanorsynode::idtype;
	}

	string Typename() {
		return "synode";
	}

	short Name() {
		return name;
	}

	//The next two functions work in conjunction...
	//If your object is very atomic (such as a number or a string)
	//Then when it is passed to a function or stored into a container,
	//it might be worth duplicating it.
	// In that case: duplicateForCall should return true...

	//However if your object is complex and probably unique through out the code
	//The duplicateForCall should return false, and Atom should always reduce to a "return this;" only...
	Atanor* Atom(bool forced = false) {
		return this;
	}

	bool duplicateForCall() {
		return false;
	}

	Atanor* Map(short idthread) {
		return features;
	}

	Exporting Atanor* Merge(Atanor* a);

	Atanor* Merging(Atanor* a) {
		Merge(a);
		return this;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	bool isDeclared(short n) {
		if (exported.find(n) != exported.end())
			return true;
		return false;
	}

	bool isNew() {
		if (parent != NULL)
			return false;
		return true;
	}

	Atanor* Newinstance(short, Atanor* f = NULL) {
		return new Atanorsynode;
	}

	AtanorIteration* Newiteration(bool direction) {
		if (features == aNULL)
			return aITERNULL;

		return features->Newiteration(direction);
	}

	Atanor* in(Atanor* context, Atanor* a, short idthread) {
		if (features == aNULL) {
			if (context->isBoolean())
				return aFALSE;

			if (context->isVectorContainer())
				return Selectasvector(context);

			return aNULL;
		}

		return features->in(context, a, idthread);
	}

	Exporting static void AddMethod(AtanorGlobal* g, string name, synodeMethod func, unsigned long arity, string infos);
	Exporting static bool InitialisationModule(AtanorGlobal* g, string version);
	Exporting static bool Checkattribute(string key);
	Exporting static bool Checkfeature(string key, string value);

	Atanor* Getvalues(AtanorDeclaration* dom, bool duplicate);

	bool Checkfeatures(Atanor* m, short idthread) {
		if (!testvalid)
			return true;

		AtanorIteration* itr = m->Newiteration(false);
		for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
			if (!Checkfeature(itr->Keystring(), itr->Valuestring())) {
				stringstream msg;
				msg << "Unknown feature: '" << itr->Keystring() << ":" << itr->Valuestring() << "'";
				globalAtanor->Returnerror(msg.str(), idthread);
				itr->Release();
				return false;
			}
		}

		itr->Release();
		return true;
	}

	void Methods(Atanor* v) {

		for (auto& it : infomethods)
			v->storevalue(it.first);
	}
	string Info(string n) {
		if (infomethods.find(n) != infomethods.end())
			return infomethods[n];
		return "Unknown method";
	}
	void Setreference(short r = 1) {
		features->Setreference(r);
		AtanorObject::Setreference(r);
		Atanorsynode* syn = child;
		while (syn != NULL) {
			syn->Setreference(r);
			syn = syn->next;
		}
	}

	void Resetreference(short r = 1) {
		Atanorsynode* syn = child;
		while (syn != NULL) {
			Atanorsynode* nx = syn->next;
			syn->Resetreference(r);
			syn = nx;
		}
		features->Resetreference(r);
		AtanorObject::Resetreference(r);
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is a sample  of a function that could be implemented for your needs.

	Atanor* MethodInitial(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		Atanor* res = callfunc->Evaluate(0, contextualpattern, idthread);
		if (!res->isMapContainer())
			return globalAtanor->Returnerror("Expecting a map as value", idthread);

		if (res->Type() == Atanormapss::idtype)
			features = res;
		else {
			features = globalAtanor->Providemapss();
			features->Put(aNULL, res, idthread);
		}

		features->Setreference(reference);
		return this;
	}

	bool compare(Atanorsynode* as) {
		if (as == NULL || as->features == aNULL)
			return true;

		string key;
		string val;
		bool aff = false;


		hmap<string, string >& values = ((Atanormapss*)features)->values;
		hmap<string, string >& avalues = ((Atanormapss*)as->features)->values;
		for (auto& it : avalues) {
			aff = false;
			key = it.first;
			if (key[0] == '=') {
				aff = true;
				key = &key.c_str()[1];
			}
			val = it.second;
			if (val == "~") {
				if (values.find(key) != values.end()) {
					if (!aff)
						return false;
					values.erase(key);
				}
				continue;
			}
			if (values.find(key) != values.end()) {
				if (val != values[key])
					return false;
			}
			else {
				if (!aff)
					return false;
			}
		}

		return true;
	}

	Atanorsynode* Root() {
		if (parent == NULL)
			return this;
		return parent->Root();
	}

	void Leftdepth(Atanorsynode* syn, Atanorvector& vect, long& pos) {
		if (syn == this)
			pos = vect.values.size();

		vect.values.push_back(this);

		if (child != NULL)
			child->Leftdepth(syn, vect, pos);
		if (next != NULL)
			next->Leftdepth(syn, vect, pos);
	}

	void Lefttraverse(Atanorvector& vect, long& pos) {
		Atanorsynode* root = Root();
		root->Leftdepth(this, vect, pos);
	}

	void Traverse(Atanorsynode* syn, Atanorsynode* cmp, Atanor* vect) {
		if (syn->compare(cmp) && syn != this)
			vect->Push(syn);
		if (syn->child != NULL)
			Traverse(syn->child, cmp, vect);
		if (syn->next != NULL)
			Traverse(syn->next, cmp, vect);
	}

	Atanorsynode* traverse(Atanorsynode* syn) {
		if (syn == this)
			return this;

		Atanorsynode* a;
		if (child != NULL) {
			a = child->traverse(syn);
			if (a != NULL)
				return a;
		}
		if (next != NULL) {
			a = next->traverse(syn);
			if (a != NULL)
				return a;
		}
		return NULL;
	}



	//----------------------------------------------------------------
	Atanor* MethodTest(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		if (features->Type() != Atanormapss::idtype)
			return aFALSE;

		string s = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	
		if (((Atanormapss*)features)->values.find(s) == ((Atanormapss*)features)->values.end())
			return aFALSE;
		return aTRUE;
	}

	//----------------------------------------------------------------
	Atanor* MethodParent(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		if (callfunc->Size() == 0) {
			if (parent == NULL)
				return aNULL;
			return parent;
		}

		Atanor* a = callfunc->Evaluate(0, contextualpattern, idthread);
		if (a->Type() != a_synode || parent == NULL)
			return aNULL;

		Atanorsynode* syn = (Atanorsynode*)a;
		if (parent == syn)
			return parent;

		if (parent->compare(syn))
			return parent;

		return aNULL;
	}

	Atanor* MethodLast(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		if (callfunc->Size() == 0) {
			if (last == NULL)
				return aNULL;
			return last;
		}

		Atanor* a = callfunc->Evaluate(0, contextualpattern, idthread);
		if (a->Type() != a_synode || last == NULL)
			return aNULL;

		Atanorsynode* syn = (Atanorsynode*)a;
		if (last == syn)
			return last;

		if (last->compare(syn))
			return last;

		return aNULL;
	}


	Atanor* MethodChild(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		if (callfunc->Size() == 0) {
			if (child == NULL)
				return aNULL;
			return child;
		}

		Atanor* a = callfunc->Evaluate(0, contextualpattern, idthread);
		if (a->Type() != a_synode || child == NULL)
			return aNULL;

		Atanorsynode* syn = (Atanorsynode*)a;
		if (child == syn)
			return child;

		if (child->compare(syn))
			return child;

		return aNULL;
	}

	Atanor* MethodPrevious(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		if (callfunc->Size() == 0) {
			if (previous == NULL)
				return aNULL;
			return previous;
		}

		Atanor* a = callfunc->Evaluate(0, contextualpattern, idthread);
		if (a->Type() != a_synode || previous == NULL)
			return aNULL;

		Atanorsynode* syn = (Atanorsynode*)a;
		if (previous == syn)
			return previous;

		if (previous->compare(syn))
			return previous;

		return aNULL;
	}

	Atanor* MethodNext(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {		
		if (callfunc->Size() == 0) {
			if (next == NULL)
				return aNULL;
			return next;
		}

		Atanor* a = callfunc->Evaluate(0, contextualpattern, idthread);
		if (a->Type() != a_synode || next == NULL)
			return aNULL;

		Atanorsynode* syn = (Atanorsynode*)a;
		if (next == syn)
			return next;

		if (next->compare(syn))
			return next;

		return aNULL;

	}

	Atanor* MethodAfter(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		Atanorsynode* s;
		if (callfunc->Size() == 0) {
			Atanor* vect = Selectavector(contextualpattern);
			s = next;
			while (s != NULL) {
				vect->Push(s);
				s = s->next;
			}
			return vect;
		}

		Atanor* a = callfunc->Evaluate(0, contextualpattern, idthread);
		if (a->Type() != a_synode || parent == NULL)
			return aNULL;

		Atanorsynode* syn = (Atanorsynode*)a;
		if (syn == this)
			return syn;

		if (!syn->id) {
			s = next;
			while (s != NULL) {
				if (s == syn)
					return s;
				s = s->next;
			}

			return aNULL;
		}

		Atanor* vect = Selectavector(contextualpattern);
		s = next;
		while (s != NULL) {
			if (s->compare(syn))
				vect->Push(s);
			s = s->next;
		}

		return vect;
	}

	Atanor* MethodBefore(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		Atanorsynode* s;
		if (callfunc->Size() == 0) {
			Atanor* vect = Selectavector(contextualpattern);
			s = previous;
			while (s != NULL) {
				vect->Push(s);
				s = s->previous;
			}
			return vect;
		}

		Atanor* a = callfunc->Evaluate(0, contextualpattern, idthread);
		if (a->Type() != a_synode || parent == NULL)
			return aNULL;

		Atanorsynode* syn = (Atanorsynode*)a;
		if (syn == this)
			return syn;

		if (!syn->id) {
			s = previous;
			while (s != NULL) {
				if (s == syn)
					return s;
				s = s->previous;
			}

			return aNULL;
		}

		Atanor* vect = Selectavector(contextualpattern);
		s = previous;
		while (s != NULL) {
			if (s->compare(syn))
				vect->Push(s);
			s = s->previous;
		}

		return vect;
	}


	Atanor* MethodPrecede(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {


		long pos = -1;
		long i;
		if (callfunc->Size() == 0) {
			Atanorvector nodes;
			Lefttraverse(nodes, pos);
			Atanor* vect = Selectavector(contextualpattern);
			for (i = pos + 1; i < nodes.values.size(); i++)
				vect->Push(nodes.values[i]);

			return vect;
		}

		Atanor* a = callfunc->Evaluate(0, contextualpattern, idthread);
		if (a->Type() != a_synode || parent == NULL)
			return aNULL;

		Atanorsynode* syn = (Atanorsynode*)a;
		if (syn == this)
			return syn;

		Atanorvector nodes;
		Lefttraverse(nodes, pos);

		if (!syn->id) {
			for (i = pos + 1; i < nodes.values.size(); i++) {
				if (nodes.values[i] == syn)
					return syn;
			}
			return aNULL;
		}

		Atanorsynode* s;
		Atanor* vect = Selectavector(contextualpattern);
		for (i = pos + 1; i < nodes.values.size(); i++) {
			s = (Atanorsynode*)nodes.values[i];
			if (s->compare(syn))
				vect->Push(s);
		}

		return vect;		return vect;
	}

	Atanor* MethodSucceed(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

		long pos = -1;
		long i;
		if (callfunc->Size() == 0) {
			Atanorvector nodes;
			Lefttraverse(nodes, pos);
			Atanor* vect = Selectavector(contextualpattern);
			for (i = 0; i < pos; i++)
				vect->Push(nodes.values[i]);

			return vect;
		}

		Atanor* a = callfunc->Evaluate(0, contextualpattern, idthread);
		if (a->Type() != a_synode || parent == NULL)
			return aNULL;

		Atanorsynode* syn = (Atanorsynode*)a;
		if (syn == this)
			return syn;
		
		Atanorvector nodes;
		Lefttraverse(nodes, pos);

		if (!syn->id) {
			for (i = 0; i < pos; i++) {
				if (nodes.values[i] == syn)
					return syn;
			}
			return aNULL;
		}

		Atanorsynode* s;
		Atanor* vect = Selectavector(contextualpattern);
		for (i = 0; i < pos; i++) {
			s = (Atanorsynode*)nodes.values[i];
			if (s->compare(syn))
				vect->Push(s);
		}

		return vect;
	}


	Atanor* MethodSisters(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		Atanorsynode* s;
		if (callfunc->Size() == 0) {
			if (parent == NULL)
				return aNULL;

			Atanor* vect = Selectavector(contextualpattern);
			s = parent->child;
			while (s != NULL) {
				if (s != this)
					vect->Push(s);
				s = s->next;
			}
			return vect;
		}

		Atanor* a = callfunc->Evaluate(0, contextualpattern, idthread);
		if (a->Type() != a_synode || parent == NULL)
			return aNULL;

		Atanorsynode* syn = (Atanorsynode*)a;
		if (syn == this)
			return syn;

		if (!syn->id) {
			s = parent->child;
			while (s != NULL) {
				if (s == syn)
					return s;
				s = s->next;
			}

			return aNULL;
		}

		Atanor* vect = Selectavector(contextualpattern);
		s = parent->child;
		while (s != NULL) {
			if (s == this) {
				s = s->next;
				continue;
			}
			if (s->compare(syn))
				vect->Push(s);
			s = s->next;
		}

		return vect;
	}

	Atanor* MethodNbChildren(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		if (child == NULL)
			return aZERO;
		long nb = 0;
		Atanorsynode* s = child;

		while (s != NULL) {
			nb++;
			s = s->next;
		}

		return globalAtanor->Provideint(nb);
	}

	Atanor* MethodChildren(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		Atanor* vect = aNULL;
		if (callfunc->Size() == 0) {
			vect = Selectavector(contextualpattern);
			if (child == NULL)
				return aNULL;

			vect = Selectavector(contextualpattern);
			Atanorsynode* s = child;
			while (s != NULL) {
				vect->Push(s);
				s = s->next;
			}
			return vect;
		}

		Atanor* a = callfunc->Evaluate(0, contextualpattern, idthread);
		
		if (a->Type() != a_synode || child == NULL)
			return aNULL;

		Atanorsynode* syn = (Atanorsynode*)a;
		
		if (syn->id != 0)
			vect = Selectavector(contextualpattern);

		Atanorsynode* s = child;
		while (s != NULL) {
			if (!syn->id) {
				if (s == syn)
					return s;
			}
			else {
				if (s->compare(syn))
					vect->Push(s);
			}
			s = s->next;
		}

		return vect;
	}

	Atanor* MethodSibling(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		if (callfunc->Size() == 0) {			
			if (child == NULL)
				return aNULL;

			Atanor* vect = Selectavector(contextualpattern);
			Traverse(child, NULL, vect);
			return vect;
		}

		Atanor* a = callfunc->Evaluate(0, contextualpattern, idthread);
		if (a->Type() != a_synode)
			return aNULL;

		Atanorsynode* syn = (Atanorsynode*)a;

		if (!syn->id) {
			if (parent != NULL)
				syn = parent->child->traverse(syn);
			else
				syn = traverse(syn);

			if (syn != NULL)
				return syn;
			return aNULL;
		}

		Atanor* vect = Selectavector(contextualpattern);
		Traverse(child, syn, vect);
		return vect;
	}

	Atanor* MethodAddChild(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		Atanor* a;
		Atanorsynode* syn;
		for (int i = 0; i < callfunc->Size(); i++) {
			a = callfunc->Evaluate(i, contextualpattern, idthread);
			if (a->Type() != a_synode)
				return globalAtanor->Returnerror("Expecting a 'synode' here", idthread);

			syn = (Atanorsynode*)a;
			if (!syn->isNew())
				return globalAtanor->Returnerror("This 'synode' is already part of a tree", idthread);

			syn->parent = this;

			syn->Setreference(reference);

			if (child == NULL)
				child = syn;
			else {
				syn->previous = last;
				last->next = syn;
			}

			last = syn;
		}
		return this;
	}
	
	Atanor* MethodAddNext(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		Atanor* a = callfunc->Evaluate(0, contextualpattern, idthread);
        if (a->Type() != a_synode)
            return globalAtanor->Returnerror("Expecting a 'synode' here", idthread);

		Atanorsynode* syn = (Atanorsynode*)a;
		if (!syn->isNew())
			return globalAtanor->Returnerror("This 'synode' is already part of a tree", idthread);

		syn->previous = this;
		syn->next = next;
		if (next != NULL)
			next->previous = syn;
		next = syn;

		if (parent != NULL) {
			parent->last = syn;
			syn->parent = parent;
			syn->Setreference(parent->reference);
		}

		return syn;
	}

	Atanor* MethodAddPrevious(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		Atanor* a = callfunc->Evaluate(0, contextualpattern, idthread);
        if (a->Type() != a_synode)
            return globalAtanor->Returnerror("Expecting a 'synode' here", idthread);

		Atanorsynode* syn = (Atanorsynode*)a;
		if (!syn->isNew())
			return globalAtanor->Returnerror("This 'synode' is already part of a tree", idthread);

		syn->next = this;
		syn->previous = previous;
		if (previous != NULL)
			previous->next = syn;
		previous = syn;

		if (parent != NULL) {
			if (parent->child == this)
				parent->child = syn;
			syn->parent = parent;
			syn->Setreference(parent->reference);
		}

		return syn;
	}

	Atanor* MethodDefinitions(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		Atanor* fmap = callfunc->Evaluate(0, contextualpattern, idthread);
		if (!fmap->isMapContainer())
			return globalAtanor->Returnerror("Expecting a 'map' as argument", idthread);

		if (validfeatures == NULL)
			validfeatures = globalAtanor->Providemapss();

		AtanorIteration* itr = fmap->Newiteration(false);
		for (itr->Begin(); itr->End() != aTRUE; itr->Next())
			validfeatures->values[itr->Keystring()] = itr->Valuestring();
		itr->Release();

		if (validfeatures->values.size() != 0)
			testvalid = true;

		return aTRUE;
	}

	Atanor* MethodAttributes(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		Atanorsvector* vstr = (Atanorsvector*)Selectasvector(contextualpattern);
		if (features == aNULL)
			return vstr;

		Atanormapss* f = (Atanormapss*)features;
		Locking _lock(f);
		hmap<string, string>::iterator it;
		for (it = f->values.begin(); it != f->values.end(); it++)
			vstr->values.push_back(it->first);
		return vstr;
	}

	Atanor* MethodValues(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		Atanorsvector* vstr = (Atanorsvector*)Selectasvector(contextualpattern);
		if (features == aNULL)
			return vstr;

		Atanormapss* f = (Atanormapss*)features;
		Locking _lock(f);

		for (auto& it : f->values)
			vstr->values.push_back(it.first);
		return vstr;
	}
	//---------------------------------------------------------------------------------------------------------------------
	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
	//is returned by the Declaration method.
	Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		return (this->*methods[idname])(contextualpattern, idthread, callfunc);
	}
	void Clear(AtanorGlobal* g) {
		//To set a variable back to empty
	}
	void Clean() {
		//Your cleaning code
	}
	bool isAssignable() {
		return true;
	}
	string JSonString() {
		if (features == aNULL)
			return "{}";

		return features->JSonString();
	}

	string String() {
		string s;
		if (features->isMapContainer()) {
			s = features->String();
			s[0] = '[';
			s[s.size() - 1] = ']';
		}

		stringstream r;
		r << "#" << id << s;
		return r.str();
	}

	long Integer() { return id; }
	short Short() { return id; }
	double Float() { return id; }
	bool Boolean() { return false; }
	BLONG Long() { return id; }
	unsigned char Byte() {
		return id;
	}

	long Size() { return features->Size(); }

	Exporting Atanor* same(Atanor* a);
	Exporting Atanor* different(Atanor* a);
	Exporting Atanor* plus(Atanor* a, bool itself);
};


class AtanorCallsynode : public AtanorObject {
public:
	Atanorsynode* syn;
	Atanor* features;

	AtanorCallsynode(Atanorsynode* s, AtanorGlobal* g, Atanor* parnt = NULL) : AtanorObject(g, parnt) {
		syn = s;
		features = aNULL;
	}

	void AddInstruction(Atanor* f) {
		features = f;
		f->Setreference();
	}

	bool isAssignable() {
		return true;
	}

	Exporting Atanor* Get(Atanor* v, Atanor* i, short idthread);
	Exporting Atanor* Put(Atanor* v, Atanor* i, short idthread);
};


#endif




