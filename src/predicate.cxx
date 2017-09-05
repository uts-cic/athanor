
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
filename   : predicate.cxx
Date       : 2017/04/01
Purpose    : Predicate Implementation
Programmer : Claude ROUX
Reviewer   :
*/

#include "predicate.h"
#include <memory>
#include "compilecode.h"
#include "constobjects.h"
#include "atanortreemap.h"
#include "atanorfvector.h"
#include "atanorsynode.h"
#include "atanormapss.h"

//-------------------------------------------------------------------------------------------------
static unsigned short predicatename = 0;
static unsigned short predicatedico = 0;
static unsigned short predicatezone = 0;
static unsigned short predicatedependency = 0;
static unsigned short predicatefeature = 0;

void InitPredicateVariableFlags() {
	predicatename = 0;
	predicatedico = 0;
	predicatezone = 0;
	predicatedependency = 0;
	predicatefeature = 0;
}

void SetPredicateVariableFlags() {
	predicatezone = globalAtanor->idSymbols.size();
	predicatedico = predicatezone + 1;
	predicatedependency = predicatedico + 1;
	predicatefeature = predicatedependency + 1;
	predicatename = predicatefeature + 1;
}


//A little bit of explanation.
//To avoid creating useless elements in the binmap, we initialize the actual name of a "^dependency" (code is a_modifydependency)
//with predicatedependency, which allows for a much more compact storage in the declaration map.
//We intercept this a_modifydependency to replace it on the fly with predicatedependency...
bool AtanorDependencyKnowledgeBaseFunction::Setinvariable(Atanor* v, AtanorDeclaration* dom, short idthread) {
	if (idvar) {
		if (idvar == a_modifydependency) {
			dom->Declare(predicatedependency, v);
			globalAtanor->Storevariable(idthread, predicatedependency, v);
			return true;
		}

		dom->Declare(idvar, v);
		globalAtanor->Storevariable(idthread, idvar, v);
	}
	return false;
}

void AtanorDependencyKnowledgeBaseFunction::Resetintvariable(AtanorDeclaration* dom, short idthread) {
	if (idvar) {
		if (idvar == a_modifydependency) {
			dom->declarations.erase(predicatedependency);
			globalAtanor->Removevariable(idthread, predicatedependency);
			return;
		}

		dom->declarations.erase(idvar);
		globalAtanor->Removevariable(idthread, idvar);
	}
}


short AtanorDependencyKnowledgeBaseFunction::Idvar() {
	if (idvar == a_modifydependency)
		return predicatedependency;
	return idvar;
}

bool AtanorDependency::Setinvariable(Atanor* v, AtanorDeclaration* dom, short idthread) {
	if (idvar) {
		if (idvar == a_modifydependency) {
			dom->Declare(predicatedependency, v);
			globalAtanor->Storevariable(idthread, predicatedependency, v);
			return true;
		}

		dom->Declare(idvar, v);
		globalAtanor->Storevariable(idthread, idvar, v);
	}
	return false;
}

void AtanorDependency::Resetintvariable(AtanorDeclaration* dom, short idthread) {
	if (idvar) {
		if (idvar == a_modifydependency) {
			dom->declarations.erase(predicatedependency);
			globalAtanor->Removevariable(idthread, predicatedependency);
			return;
		}

		dom->declarations.erase(idvar);
		globalAtanor->Removevariable(idthread, idvar);
	}
}

short AtanorDependency::Idvar() {
	if (idvar == a_modifydependency)
		return predicatedependency;
	return idvar;
}

//-------------------------------------------------------------------------------------------------
// Predicate variables can have methods...
//-------------------------------------------------------------------------------------------------
Exporting hmap<unsigned short, predicateMethod> AtanorPredicate::methods;
Exporting hmap<string, string> AtanorPredicate::infomethods;
Exporting bin_hash<unsigned long> AtanorPredicate::exported;

Exporting short AtanorPredicate::idtype = 0;

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void AtanorPredicate::AddMethod(AtanorGlobal* global, string name, predicateMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool AtanorPredicate::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();
	
	AtanorPredicate::idtype = global->Getid("predicate");

	AtanorPredicate::AddMethod(global, "_initial", &AtanorPredicate::MethodInitial, P_ONE, "_initial(): Get the associated object.");
	AtanorPredicate::AddMethod(global, "leaves", &AtanorPredicate::MethodLeaves, P_NONE, "leaves(): Return the leaf values.");
	AtanorPredicate::AddMethod(global, "features", &AtanorPredicate::MethodFeatures, P_NONE, "features(): Return the dependency features.");
	AtanorPredicate::AddMethod(global, "name", &AtanorPredicate::MethodName, P_NONE, "name(): Return the predicate name.");
	AtanorPredicate::AddMethod(global, "trace", &AtanorPredicate::MethodTrace, P_ONE, "trace(bool v): Set trace mode.");
	AtanorPredicate::AddMethod(global, "rule", &AtanorPredicate::MethodRuleid, P_NONE, "rule(): Return the rule id.");

	global->newInstance[AtanorPredicate::idtype] = new AtanorPredicate(a_predicate, global);
	global->RecordMethods(AtanorPredicate::idtype, AtanorPredicate::exported);

	global->newInstance[a_predicatevar] = new AtanorPredicateVar(global, a_predicatevar);
	global->RecordMethods(a_predicatevar, AtanorPredicate::exported);

	global->newInstance[a_dependency] = new AtanorDependency(global, aNULL, a_dependency, 0);
	short id = global->Getid("name");
	global->methods[a_dependency][id] = global->methods[a_predicate][id];
	id = global->Getid("_initial");
	global->methods[a_dependency][id] = global->methods[a_predicate][id];
	id = global->Getid("features");
	global->methods[a_dependency][id] = global->methods[a_predicate][id];
	id = global->Getid("rule");
	global->methods[a_dependency][id] = global->methods[a_predicate][id];

	return true;
}

Atanor* AtanorPredicate::MethodRuleid(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	AtanorPredicateContainer* apc = globalAtanor->Predicatecontainer();
	if (apc->rules.find(a_dependency) == apc->rules.end())
		return aNULL;

	Atanor* kmap = Selectamap(contextualpattern);
	long id = Idrule();
	if (id != -1) {
		Atanor* object = apc->rules[a_dependency][id];
		if (object != NULL) {
			long ln = object->Currentline();
			string filename = globalAtanor->Getfilename(object->Currentfile());
			kmap->Push("id", globalAtanor->Provideint(id));
			kmap->Push("line", globalAtanor->Provideint(ln));
			kmap->Push("filename", globalAtanor->Providestring(filename));
		}
	}
	return kmap;
}

Atanor* AtanorPredicate::MethodFeatures(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (isDependency()) {
		AtanorDependency* dep = (AtanorDependency*)this;

		if (dep->features == aNULL)
			return globalAtanor->Providemapss();
		return dep->features;
	}
	return globalAtanor->Providemapss();
}

Atanor* AtanorPredicate::MethodInitial(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (isDependency()) {
		Atanor* vect = callfunc->Evaluate(0, contextualpattern, idthread);
		if (!vect->isVectorContainer())
			return globalAtanor->Returnerror("DEP(010): We expect a vector as variable", idthread);

		long sz = vect->Size();
		if (sz < 2)
			return globalAtanor->Returnerror("DEP(011): This vector does not contain enough information. It should contain: [name,features,synode1,synode2...]", idthread);
		AtanorDependency* dep = (AtanorDependency*)this;
		dep->name = globalAtanor->Getid(vect->getstring(0));
		Atanor* val = vect->getvalue(1);
		if (val != aNULL && !val->isMapContainer() && val->Type() != a_synode)
			return globalAtanor->Returnerror("DEP(012): Wrong feature definition. It should be either 'null' or a 'map'", idthread);

		if (val->isMapContainer()) {
			if (sz < 3)
				return globalAtanor->Returnerror("DEP(011): This vector does not contain enough information. It should contain: [name,features,synode1,synode2...]", idthread);
			Atanormapss* feat = globalAtanor->Providemapss();
			dep->features = feat;
			feat->Setreference(reference);
			AtanorIteration* itr = val->Newiteration(false);
			for (itr->Begin(); itr->End() != aTRUE; itr->Next())
				feat->values[itr->Keystring()] = itr->Valuestring();
			itr->Release();
		}

		long beg = 2;
		if (val->Type() == a_synode)
			beg = 1;

		for (long i = beg; i < sz; i++) {
			val = vect->getvalue(i);
			if (val->Type() != a_synode)
				return globalAtanor->Returnerror("DEP(013): Only 'synode' can be used here to create a dependency", idthread);
			dep->parameters.push_back(val);
			val->Setreference(reference);
		}
		return aTRUE;
	}

	if (Function() != NULL) {
		AtanorPredicateFunction* apf = (AtanorPredicateFunction*)this;
		apf->object = callfunc->Evaluate(0, contextualpattern, idthread);
		return aTRUE;
	}
	return aFALSE;
}

Atanor* AtanorPredicate::MethodTrace(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	bool thetrace = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
	if (isPredicate()) {
		if (globalAtanor->predicates.check(name))
			globalAtanor->predicates[name]->trace = thetrace;
		return aTRUE;
	}
	return aFALSE;
}

Atanor* AtanorPredicate::Newinstance(short idthread, Atanor* f) {
	if (f != NULL)
		return new AtanorPredicateFunction(NULL, f, name);

	return new AtanorPredicate(name, globalAtanor);
}


void AtanorPredicateFunction::Setname(short n) {
	name = n;
	globalAtanor->predicates[name] = this;
}


//-------------------------------------------------------------------------------------------------
// Cleaning the variable instances created during the predicate evaluation...

inline void ClearInstances(AtanorDeclaration* dom, basebin_hash<Atanor*>& basedomain) {
	Atanor** tb;
	binuint64 filter;
	short idx;
	long j;

	for (long ii = 0; ii < dom->declarations.tsize; ii++) {
		filter = dom->declarations.indexes[ii];
		if (filter) {
			tb = dom->declarations.table[ii];
			j = 0;
			while (filter) {
				if (!(filter & 1)) {
					while (!(filter & 65535)) {
						filter >>= 16;
						j = j + 16;
					}
					while (!(filter & 255)) {
						filter >>= 8;
						j = j + 8;
					}
					while (!(filter & 15)) {
						filter >>= 4;
						j = j + 4;
					}
					while (!(filter & 1)) {
						filter >>= 1;
						j++;
					}
				}

				//we do not delete the instances that were created by the underlying calls to PredicateEvalue
				//More exactly, when we push a new analysis of a rule into action, we want to clean the variables that were created
				//within this call, not the current ones.
				idx = ((ii + dom->declarations.base) << binbits) + j;
				if (!basedomain.check(idx) || basedomain[idx] != tb[j])
					tb[j]->Resetreferencenoprotect();
				filter >>= 1;
				j++;
			}
		}
	}

	dom->declarations = basedomain;
}


//-------------------------------------------------------------------------------------------------
AtanorDependency::AtanorDependency(AtanorGlobal* g, Atanor* f, short n, short id) : idrule(0), AtanorPredicate(n, g, a_dependency) {
	if (f->Type() == a_mapss) {
		features = globalAtanor->Providemapss();
		string key;
		for (auto& it : ((Atanormapss*)f)->values) {
			key = it.first;
			if (key[0] == '~' || it.second == "~")
				continue;
			if (key[0] == '=')
				key = &key.c_str()[1];
			((Atanormapss*)features)->values[key] = it.second;
		}
	}
	else
		features = aNULL;

	chosen = false;
	idvar = id;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void PreLocalPrint(string s) {
	Locking _lock(globalAtanor->_printlock);

	Atanorstring* kval = (Atanorstring*)globalAtanor->kstd;

	if (kval == NULL) {
		if (globalAtanor->displayfunction == NULL)
			*globalAtanor->os << s;
		else
			globalAtanor->displayfunction(s, globalAtanor->displayobject);
		if (globalAtanor->doubledisplay) {
#ifdef DOSOUTPUT
			string sdos = conversion2dos(s);
			cout << sdos;
#else
			cout << s;
#endif
		}
	}
	else
		kval->value += s;
}

//-----------------------------------------------
//We implement a local reshuffling...

bool AtanorGlobal::predicateTrace(short n) {
	return predicates[n]->trace;
}

//----------------------------------------------------------------------------------------------------


void AtanorPredicateVariableInstance::Resetreference(short inc) {
	if (value != aNOELEMENT)
		value->Resetreferencenoprotect(inc);

	AtanorBasisPredicate::Resetreference(inc);
}

void AtanorPredicateVariableInstance::Resetreferencenoprotect(short inc) {
	if (value != aNOELEMENT)
		value->Resetreferencenoprotect(inc);

	protect = 0;
	AtanorBasisPredicate::Resetreference(inc);
}

void AtanorPredicateVariableInstance::affiche() {
	if (value != aNOELEMENT)
		cerr << "=" << value->String();
	cerr << "/" << reference;
}

bool compshort(short n, short nn) {
	if (n < nn)
		return true;
	return false;
}

void AffichageDom(AtanorDeclaration* dom, int depth) {
	for (int i = 0; i < depth; i++)
		cerr << " ";
	basebin_hash<Atanor*>::iterator it;
	vector<short> names;
	for (it = dom->declarations.begin(); it != dom->declarations.end(); it++)
		names.push_back(it->first);

	sort(names.begin(), names.end(), compshort);
	bool first = true;
	for (int i = 0; i < names.size(); i++) {
		if (!first)
			cerr << ",";
		first = false;
		Atanor* v = dom->declarations[names[i]];
		if (names[i] != v->Name())
			cerr << names[i] << ":" << v->Name();
		else {
			cerr << names[i];
			((AtanorPredicateVariableInstance*)v)->affiche();
		}
	}
	cerr << endl;

}

//------------------------------------------------------------------------------------------
bool ThreadStruct::TestPredicate(AtanorDeclaration* dom, AtanorPredicate* p) {
	if (p->name != a_universal && knowledgebase.find(p->name) == knowledgebase.end())
		return false;
	if (p->name == a_universal) {

		for (auto& it : knowledgebase) {
			vector<AtanorPredicate*>& v = it.second;
			for (int i = 0; i < v.size(); i++) {
				if (p->Unify(dom, v[i]))
					return true;
			}
		}
		return false;
	}
	vector<AtanorPredicate*>& v = knowledgebase[p->name];
	for (int i = 0; i < v.size(); i++) {
		if (p->Unify(dom, v[i]))
			return true;
	}
	return false;
}

char ThreadStruct::isaValidPredicate(AtanorDeclaration* dom, AtanorPredicate* p, hmap<unsigned short, vector<AtanorPredicateRule*> >& rulebase) {
	if (p->name != a_universal && knowledgebase.find(p->name) == knowledgebase.end())
		return 1;
	
	int exist = -1;
	int i;
	if (p->name == a_universal) {
		for (auto& it : knowledgebase) {
			vector<AtanorPredicate*>& v = it.second;
			for (i = 0; i < v.size(); i++) {
				if (p->Unify(dom, v[i]))
					return 2;
			}
		}
		return 0;
	}

	vector<AtanorPredicate*>& v = knowledgebase[p->name];
	for (i = 0; i < v.size(); i++) {
		if (p->Unify(dom, v[i]))
			return 2;
	}
	//we keep a chance of finding a rule to match with
	if (rulebase.find(p->name) != rulebase.end())
		return 1;
	return 0;
}

bool ThreadStruct::GetPredicates(AtanorDeclaration* dom, AtanorPredicate* p, vector<AtanorPredicate*>& res, bool cut) {
	
	bool ret = false;	
	int exist = -1;
	int i;

	if (p->name == a_universal) {
		for (auto& it : knowledgebase) {
			vector<AtanorPredicate*>& v = it.second;
			for (i = 0; i < v.size(); i++) {
				if (p->Unify(dom, v[i])) {
					res.push_back(v[i]);
					if (cut)
						return true;
					ret = true;
				}
			}
		}
		return ret;
	}
	vector<AtanorPredicate*>& v = knowledgebase[p->name];
	for (i = 0; i < v.size(); i++) {
		if (p->Unify(dom, v[i])) {
			res.push_back(v[i]);
			if (cut)
				return true;
			ret = true;
		}
	}
	return ret;
}


bool ThreadStruct::StorePredicate(AtanorDeclaration* dom, AtanorPredicate* pv, bool last) {
	if (TestPredicate(dom, pv))
		return false;
	
	if (pv->Name() == a_universal) {
		if (pv->Idvar() != 0) {
			//we share the same name as with the dependency that was associated with a variable...
			Atanor* a = dom->Declaration(pv->Idvar());
			if (a != NULL && a->Type() == a_dependency)
				pv->name = a->Name();
			else
				return false;
		}
		else
			return false;
	}

	if (last)
		knowledgebase[pv->name].push_back(pv);
	else
		knowledgebase[pv->name].insert(knowledgebase[pv->name].begin(), pv);

	pv->Setreference();
	return true;
}

bool ThreadStruct::RemovePredicates(AtanorDeclaration* dom, AtanorPredicate* p) {

	if (knowledgebase.find(p->name) == knowledgebase.end())
		return false;

	vector<AtanorPredicate*>& v = knowledgebase[p->name];
	bool res = false;
	for (BLONG i = v.size() - 1; i >= 0; i--) {
		if (p->same(v[i])->Boolean()) {
			v[i]->Resetreference();
			v.erase(v.begin() + i);
			res = true;
		}
	}
	return res;
}

bool ThreadStruct::RemoveThePredicate(AtanorDeclaration* dom, AtanorPredicate* p) {
	
	vector<AtanorPredicate*>& v = knowledgebase[p->name];
	bool res = false;
	for (BLONG i = 0; i < v.size(); i++) {
		if (p == v[i]) {
			p->Resetreference();
			v.erase(v.begin() + i);
			return true;
		}
	}
	return false;
}


//----------------------------------------------------------------------------------------

Atanor* AtanorPredicateVariableInstance::same(Atanor* a) {
	Atanor* dom = globalAtanor->Declarator(predicatezone, globalAtanor->GetThreadid());
	if (dom == aNULL)
		return aFALSE;

	Atanor* v = Value((AtanorDeclaration*)dom);

	if (v == aNOELEMENT)
		return aFALSE;
	return v->same(a);
}

Atanor* AtanorPredicate::same(Atanor* a) {
	if (a == this)
		return aTRUE;
	return aFALSE;
}

Atanor* AtanorDependencyKnowledgeBaseFunction::same(Atanor* a) {
	if (!a->isPredicate() || parameters.size() != a->Size())
		return aFALSE;

	for (long i = 0; i < parameters.size(); i++) {
		if (parameters[i]->same(a->Parameter(i)) == aFALSE)
			return aFALSE;
	}

	if (features == aNULL)
		return aTRUE;

	if (((AtanorPredicate*)a)->isDependency()) {
		Atanor* feats = ((AtanorDependency*)a)->features;
		if (feats == aNULL)
			return aTRUE;

		string key;
		string val;

		bool neg;

		hmap<string, string >& values = ((Atanormapss*)feats)->values;
		hmap<string, string >& avalues = ((Atanormapss*)features)->values;

		for (auto& it : values) {
			neg = false;
			key = it.first;
			if (key[0] == '~') {
				neg = true;
				key = &key.c_str()[1];
			}

			val = it.second;
			if (val == "~") {
				if (avalues.find(key) != avalues.end()) {
					if (!neg)
						return aFALSE;
				}
				continue;
			}

			if (avalues.find(key) != avalues.end()) {
				if (val != avalues[key]) {
					if (!neg)
						return aFALSE;
				}
				else {
					if (neg)
						return aFALSE;
				}
			}
			else {
				if (!neg)
					return aFALSE;
			}
		}
	}

	return aTRUE;
}

Atanor* AtanorPredicateTerm::same(Atanor* a) {
	if (a->Type() != a_term || parameters.size() != a->Size() || a->Name() != name)
		return aFALSE;
	for (long i = 0; i < parameters.size(); i++) {
		if (parameters[i]->same(a->Parameter(i)) == aFALSE)
			return aFALSE;
	}
	return aTRUE;
}
//----------------------------------------------------------------------------------------
// This method is used to detect if a variable or a set of variable (as in a vector) in a predicate has been unified.
// This is especially usefull to detect if a variable can be used as an index.
// By design, every object that is not one of the following (which inherits the Atanor version) returns true for this method
bool AtanorBasePredicateVariable::isUnified(AtanorDeclaration* dom) {
	return false;
}

bool Atanorvector::isUnified(AtanorDeclaration* dom) {
	for (BLONG i = 0; i < values.size(); i++) {
		if (values[i]->isUnified(dom) == false)
			return false;
	}
	return true;
}

bool AtanorConstmap::isUnified(AtanorDeclaration* dom) {
	for (BLONG i = 0; i < values.size(); i++) {
		if (keys[i]->isUnified(dom) == false)
			return false;
		if (values[i]->isUnified(dom) == false)
			return false;
	}
	return true;
}

bool AtanorPredicateTerm::isUnified(AtanorDeclaration* dom) {
	for (long i = 0; i < parameters.size(); i++) {
		if (parameters[i]->isUnified(dom) == false)
			return false;
	}
	return true;
}

bool AtanorPredicateVariableInstance::isUnified(AtanorDeclaration* dom) {
	Atanor* v;
	if (dom != NULL)
		v = Value(dom);
	else
		v = value;
	if (v == aNOELEMENT)
		return false;

	if (v == aUNIVERSAL)
		return true;

	return v->isUnified(dom);
}

bool AtanorPredicate::isUnified(AtanorDeclaration* dom) {
	for (long i = 0; i < parameters.size(); i++) {
		if (parameters[i]->isUnified(dom) == false)
			return false;
	}
	return true;
}


//--------------------------------------------------------------
bool Atanor::Unify(AtanorDeclaration* dom, Atanor* a) {
	if (this == aUNIVERSAL || a == aUNIVERSAL)
		return true;

	if (same(a)->Boolean() == true)
		return true;
	return false;
}

bool AtanorPredicateVariableInstance::Unify(AtanorDeclaration* dom, Atanor* a) {
	Atanor* v = Value(dom);

	if (v == aNOELEMENT || v == aUNIVERSAL)
		return true;

	return v->Unify(dom, a);
}

bool AtanorPredicateTerm::Unify(AtanorDeclaration* dom, Atanor* a) {
	if (a->Type() != a_term || a->Name() != name || a->Size() != parameters.size())
		return false;
	AtanorPredicateTerm* term = (AtanorPredicateTerm*)a;
	for (long i = 0; i < parameters.size(); i++) {
		if (parameters[i]->Unify(dom, term->parameters[i]) == false)
			return false;
	}
	return true;
}

bool Atanorvector::Unify(AtanorDeclaration* dom, Atanor* a) {
	if (a->Type() != a_vector || a->Size() != values.size())
		return false;

	Atanorvector* vect = (Atanorvector*)a;
	for (BLONG i = 0; i < values.size(); i++) {
		if (values[i]->Unify(dom, vect->values[i]) == false)
			return false;
	}
	return true;
}

bool AtanorConstmap::Unify(AtanorDeclaration* dom, Atanor* a) {
	if (a->Type() != a_constmap || a->Size() != values.size())
		return false;

	AtanorConstmap* kmap = (AtanorConstmap*)a;
	for (BLONG i = 0; i < values.size(); i++) {
		if (keys[i]->Unify(dom, kmap->keys[i]) == false)
			return false;
		if (values[i]->Unify(dom, kmap->values[i]) == false)
			return false;
	}
	return true;
}

bool AtanorPredicate::Unify(AtanorDeclaration* dom, Atanor* a) {
	if (!a->isPredicate() || a->Name() != name || a->Size() != parameters.size())
		return false;

	AtanorPredicate* pred = (AtanorPredicate*)a;
	for (long i = 0; i < parameters.size(); i++) {
		if (parameters[i]->Unify(dom, pred->parameters[i]) == false)
			return false;
	}
	return true;
}

void AtanorDependency::Setfeatures(Atanor* feats) {
	if (feats == aNULL)
		return;
	if (features == aNULL)
		features = globalAtanor->Providemapss();

	Atanormapss* f = (Atanormapss*)features;
	string key;
	string val;

	for (auto& it : ((Atanormapss*)feats)->values) {
		key = it.first;
		val = it.second;
		if (key[0] == '=')
			key = &key.c_str()[1];

		if (val == "~") {
			if (f->values.find(key) != f->values.end())
				f->values.erase(key);
			continue;
		}

		f->values[key] = val;
	}
}

bool AtanorDependency::Unify(AtanorDeclaration* dom, Atanor* a) {
	if (!a->isPredicate() || a->Size() != parameters.size() || (name != a_universal && a->Name() != name))
		return false;

	AtanorPredicate* pred = (AtanorPredicate*)a;
	//a is from the database
	if (features != aNULL) {
		if (!pred->isDependency()) {
			if (features->Size() == 0)
				return true;

			//In this case, all features should be negative...
			hmap<string, string >& values = ((Atanormapss*)features)->values;
			for (auto& it : values) {
				if (it.second != "~")
					return false;
			}

			return true;
		}

		Atanor* feat = ((AtanorDependency*)pred)->features;
		hmap<string, string >& values = ((Atanormapss*)features)->values;

		if (feat == aNULL) {
			if (features->Size() == 0)
				return true;

			//In this case, all features should be negative...
			hmap<string, string >& values = ((Atanormapss*)features)->values;
			for (auto& it : values) {
				if (it.second != "~")
					return false;
			}

			return true;
		}

		string key;
		string val;
		bool aff = false;
		bool neg;

		hmap<string, string >& avalues = ((Atanormapss*)feat)->values;
		hmap<string, string > assignation;
		for (auto& it : values) {
			aff = false;
			neg = false;
			key = it.first;
			if (key[0] == '=') {
				aff = true;
				key = &key.c_str()[1];
			}
			else {
				if (key[0] == '~') {
					neg = true;
					key = &key.c_str()[1];
				}
			}
			val = it.second;
			if (val == "~") {
				if (avalues.find(key) != avalues.end()) {
					if (!aff) {
						if (!neg)
							return false;
					}
					else
						assignation[key] = val;
				}
				continue;
			}
			if (avalues.find(key) != avalues.end()) {
				if (val != avalues[key]) {
					if (!neg)
						return false;
				}
				else {
					if (neg)
						return false;
				}
			}
			else {
				if (aff)
					assignation[key] = val;
				else {
					if (!neg)
						return false;
				}
			}
		}
		for (auto& it : assignation) {
			if (it.second == "~")
				avalues.erase(it.first);
			else
				avalues[it.first] = it.second;
		}
	}

	for (long i = 0; i < parameters.size(); i++) {
		if (parameters[i]->Unify(dom, pred->parameters[i]) == false)
			return false;
	}


	return true;
}

//--------------------------------------------------------------------
Atanor* AtanorBasePredicateVariable::Put(Atanor* ke, Atanor* dom, short idthread) {
	if (!dom->isDeclared(predicatedico)) {
		dom = globalAtanor->Declarator(predicatedico, idthread);
		if (dom == aNULL)
			return globalAtanor->Returnerror("Unknown predicate variable", idthread);
	}

	if (dom->Failed())
		return aTRUE;


	Atanor* d = dom->Declaration(predicatedico);

	if (d == NULL && !d->Checkdico(name)) {
		dom->Setfail(true);
		return aTRUE;
	}

	return d->Getdico(name)->Put(ke, dom, idthread);
}


Atanor* AtanorPredicateVariableInstance::Put(Atanor* dom, Atanor* ke, short idthread) {
	if (!dom->isDeclared(predicatezone)) {
		dom = globalAtanor->Declarator(predicatezone, idthread);
		if (dom == aNULL)
			return globalAtanor->Returnerror("Unknown predicate variable", idthread);
	}

	AtanorPredicateVariableInstance* v = (AtanorPredicateVariableInstance*)Variable((AtanorDeclaration*)dom);
	if (v->value == ke)
		return aTRUE;

	Atanor* val = v->value;
	v->value = ke->Atom(true);
	val->Resetreference(reference);
	v->value->Setreference(reference);
	return v->value;
}

Atanor* AtanorPredicateTerm::Put(Atanor* dom, Atanor* ke, short idthread) {
	if (!dom->isDeclared(predicatezone)) {
		dom = globalAtanor->Declarator(predicatezone, idthread);
		if (dom == aNULL)
			return aFALSE;
	}

	if (ke->Type() != a_term || ke->Size() != parameters.size() || ke->Name() != name) {
		dom->Setfail(true);
		return aTRUE;
	}
	AtanorPredicateTerm* kpf = (AtanorPredicateTerm*)ke;

	for (long i = 0; i < parameters.size(); i++)
		parameters[i]->Put(dom, kpf->parameters[i], idthread);
	return aTRUE;
}


Atanor* AtanorPredicate::Put(Atanor* dom, Atanor* val, short idthread) {
	return globalAtanor->Returnerror("PRE(009): You cannot instanciate a predicate this way", idthread);
}

Atanor* AtanorPredicateVar::Put(Atanor* dom, Atanor* val, short idthread) {
	if (val->Type() != ptype) {
		globalAtanor->Returnerror("PRE(001): Cannot instantiate a predicate with this value", idthread);
		return aTRUE;
	}

	//val cannot contain any PredicateVariable
	int i;
	Atanor* e;
	vector<Atanor*> v;

	for (i = 0; i < val->Size(); i++) {
		e = val->Parameter(i)->Get(dom, aNULL, idthread);
		if (!e->isUnified((AtanorDeclaration*)dom)) {
			string message("PRE(001): Cannot instantiate a predicate with this value");
			globalAtanor->Returnerror(message, idthread);
			return aTRUE;
		}
		v.push_back(e->Atom(true));
	}

	for (i = 0; i < parameters.size(); i++)
		parameters[i]->Resetreference(reference);

	parameters.clear();

	name = val->Name();
	for (i = 0; i < v.size(); i++) {
		v[i]->Setreference(reference);
		parameters.push_back(v[i]);
	}

	return aTRUE;
}

void AtanorPredicateTerm::Setreference(short inc) {
	reference += inc;
	protect = 0;
	for (long i = 0; i < parameters.size(); i++)
		parameters[i]->Setreference(inc);
}

void AtanorPredicateTerm::Setprotect(bool n) {
	protect = n;
	for (long i = 0; i < parameters.size(); i++)
		parameters[i]->Setprotect(n);
}

void AtanorPredicateTerm::Popping() {
	protect = false;
	if (reference <= 0)
		protect = true;
	for (long i = 0; i < parameters.size(); i++)
		parameters[i]->Popping();
}

void AtanorPredicateTerm::Resetreference(short inc) {
	for (long i = 0; i < parameters.size(); i++)
		parameters[i]->Resetreference(inc);
	AtanorBasisPredicate::Resetreference(inc);
}


void AtanorPredicate::Setreference(short inc) {
	reference += inc;
	protect = 0;
	for (long i = 0; i < parameters.size(); i++)
	if (!parameters[i]->isInstruction())
		parameters[i]->Setreference(inc);
}

void AtanorPredicate::Setprotect(bool v) {
	protect = v;
	for (long i = 0; i < parameters.size(); i++)
	if (!parameters[i]->isInstruction())
		parameters[i]->Setprotect(v);
}

void AtanorPredicate::Popping() {
	protect = false;
	if (reference <= 0)
		protect = true;
	for (long i = 0; i < parameters.size(); i++)
	if (!parameters[i]->isInstruction())
		parameters[i]->Popping();
}

void AtanorPredicate::Resetreference(short inc) {
	for (long i = 0; i < parameters.size(); i++) {
		if (!parameters[i]->isInstruction()) {
			if (protect)
				parameters[i]->Setprotect(1);
			parameters[i]->Resetreference(inc);
		}
	}
	AtanorBasisPredicate::Resetreference(inc);
}

void AtanorPredicateFunction::Resetreference(short inc) {
	AtanorPredicate::Resetreference(inc);
}


string AtanorPredicateKnowledgeBaseFunction::String() {
	string s;
	switch (name) {
	case a_asserta:
		s = "asserta";
		break;
	case a_assertz:
		s = "assertz";
		break;
	case a_retract:
		s = "retract";
		break;
	default:
		return("");
	}

	s += "(";
	if (parameters.size()) {
		for (long i = 0; i < parameters.size(); i++) {
			if (i)
				s += ",";
			if (parameters[i]->isString())
				s += parameters[i]->String();
			else
				s += parameters[i]->String();
		}
	}
	s += ")";
	return(s);
}

string AtanorBasePredicateVariable::String() {
	Atanor* dom = globalAtanor->Declarator(predicatedico, globalAtanor->GetThreadid());
	if (dom == aNULL)
		return globalAtanor->Getsymbol(name);

	Atanor* d = dom->Declaration(predicatedico);

	if (d == NULL || !d->Checkdico(name))
		return globalAtanor->Getsymbol(name);

	stringstream v;
	AtanorPredicateVariableInstance* vx = (AtanorPredicateVariableInstance*)d->Getdico(name)->Variable((AtanorDeclaration*)dom);
	v << globalAtanor->Getsymbol(name) << vx->name;
	if (vx->value != aUNIVERSAL && vx->value != aNOELEMENT)
		v << ":" << vx->value->String();
	return v.str();
}

string AtanorPredicateVariableInstance::String() {
	Atanor* dom = globalAtanor->Declarator(predicatezone, globalAtanor->GetThreadid());
	if (dom == aNULL)
		return globalAtanor->Getsymbol(name);

	AtanorPredicateVariableInstance* vx = (AtanorPredicateVariableInstance*)Variable((AtanorDeclaration*)dom);

	stringstream s;
	if (merge) {
		if (vx->value->Type() == a_vector && vx->value->Size() == 1) {
			Atanorvector* kvect = (Atanorvector*)vx->value;
			string val = kvect->values[0]->String();
			s << "|";
			if (val[0] == '[' || val[0] == '{') {
				if (val[1] != ']' && val[1] != '}') {
					val = val.substr(1, val.size() - 2);
					s << val;
				}
			}
		}
		else
			s << "|" << vx->value->String();
	}
	else {
		if (vx->value == aUNIVERSAL)
			s << "_";
		else {
			if (vx->value != aNOELEMENT) {
				s << vx->value->String();
			}
			else
			if (globalAtanor->Getsymbol(vx->name) != "")
				s << globalAtanor->Getsymbol(vx->name) << vx->name;
			else
				s << globalAtanor->Getsymbol(vx->labelname) << vx->name;
		}
	}
	return s.str().c_str();
}

string AtanorPredicateTerm::String() {
	string v;
	if (name != a_universal)
		v = globalAtanor->Getsymbol(name);
	v += "(";
	for (long i = 0; i < parameters.size(); i++) {
		if (i)
			v += ",";
		v += parameters[i]->JSonString();
	}
	v += ")";
	return v;
}


string AtanorPredicate::String() {
	string v = globalAtanor->Getsymbol(name);
	long sz = parameters.size();

	if (sz) {
		v += "(";
		for (long i = 0; i < sz; i++) {
			if (i)
				v += ",";
			v += parameters[i]->JSonString();
		}
		v += ")";
	}
	return v;
}

string AtanorPredicate::JSonString() {
	stringstream res;

	res << "[\"" << globalAtanor->Getsymbol(name) << "\",";
	for (long i = 0; i < parameters.size(); i++)
		res << "," << parameters[i]->JSonString();
	res << "]";
	return res.str();
}



string AtanorDependency::String() {
	string v = globalAtanor->Getsymbol(name);
	if (features != aNULL) {
		string x = features->String();
		x[0] = '[';
		x[x.size() - 1] = ']';
		v += x;
	}

	long sz = parameters.size();
	if (sz) {
		v += "(";
		for (long i = 0; i < sz; i++) {
			if (i)
				v += ",";
			v += parameters[i]->JSonString();
		}
		v += ")";
	}
	return v;
}

string AtanorDependency::JSonString() {
	stringstream res;

	res << "[\"" << globalAtanor->Getsymbol(name) << "\",";
	if (features == aNULL)
		res << "{}";
	else
		res << features->JSonString();
	for (long i = 0; i < parameters.size(); i++)
		res << "," << parameters[i]->JSonString();
	res << "]";
	return res.str();
}

//----------------------------------------------------------------------------------------------
Atanor* AtanorPredicate::Vector(short idthread) {
	Atanorvector* kvect = globalAtanor->Providevector();

	AtanorConstString local(globalAtanor->Getsymbol(name));
	kvect->Push(&local);
	Atanor* e;
	for (long i = 0; i < parameters.size(); i++) {
		e = parameters[i];
		if (e->Type() == a_term) {
			Atanor* vloc = e->Vector(idthread);
			kvect->Push(vloc);
		}
		else
		if (e->Type() == a_predicatevar) {
			e = globalAtanor->Getdefinition(e->Name(), idthread);
			if (e == NULL || e == aNOELEMENT) {
				local.value = globalAtanor->Getsymbol(parameters[i]->Name());
				kvect->Push(&local);
			}
			else
				kvect->Push(e);
		}
		else
			kvect->Push(e);
	}
	return kvect;
}

Atanor* AtanorPredicate::Map(short idthread) {
	Atanormap* kmap = globalAtanor->Providemap();

	AtanorConstString local(globalAtanor->Getsymbol(name));

	kmap->Push("name", &local);
	char buff[] = { '0', 0, 0 };
	int pos = 0;
	Atanor* e;
	for (long i = 0; i < parameters.size(); i++) {
		e = parameters[i];
		if (i && (i % 10) == 0) {
			buff[1] = '0';
			if (!pos) {
				buff[0] = '1';
				pos = 1;
			}
			else
				buff[0]++;
		}
		if (e->Type() == a_term) {
			Atanor* locmap = e->Map(idthread);
			kmap->Push(buff, locmap);
		}
		else
		if (e->Type() == a_predicatevar) {
			e = globalAtanor->Getdefinition(e->Name(), idthread);
			if (e == NULL || e == aNOELEMENT) {
				local.value = globalAtanor->Getsymbol(parameters[i]->Name());
				kmap->Push(buff, &local);
			}
			else
				kmap->Push(buff, e);
		}
		else
			kmap->Push(buff, e);
		buff[pos]++;
	}
	return kmap;
}


Atanor* AtanorPredicateTerm::Vector(short idthread) {
	Atanorvector* kvect = globalAtanor->Providevector();

	AtanorConstString local(globalAtanor->Getsymbol(name));

	kvect->Push(&local);
	Atanor* e;
	for (long i = 0; i < parameters.size(); i++) {
		e = parameters[i];
		for (long i = 0; i < parameters.size(); i++) {
			e = parameters[i];
			if (e->Type() == a_term) {
				Atanor* vloc = e->Vector(idthread);
				kvect->Push(vloc);
			}
			else
			if (e->Type() == a_predicatevar) {
				e = globalAtanor->Getdefinition(e->Name(), idthread);
				if (e == NULL || e == aNOELEMENT) {
					local.value = globalAtanor->Getsymbol(parameters[i]->Name());
					kvect->Push(&local);
				}
				else
					kvect->Push(e);
			}
			else
				kvect->Push(e);
		}
	}
	return kvect;
}

Atanor* AtanorPredicateTerm::Map(short idthread) {
	Atanormap* kmap = globalAtanor->Providemap();

	AtanorConstString local(globalAtanor->Getsymbol(name));

	kmap->Push("name", &local);
	char buff[] = { '0', 0, 0 };
	int pos = 0;
	Atanor* e;
	for (long i = 0; i < parameters.size(); i++) {
		e = parameters[i];
		if (i && (i % 10) == 0) {
			buff[1] = '0';
			if (!pos) {
				buff[0] = '1';
				pos = 1;
			}
			else
				buff[0]++;
		}
		if (e->Type() == a_term) {
			Atanor* locmap = e->Map(idthread);
			kmap->Push(buff, locmap);
		}
		else
		if (e->Type() == a_predicatevar) {
			e = globalAtanor->Getdefinition(e->Name(), idthread);
			if (e == NULL || e == aNOELEMENT) {
				local.value = globalAtanor->Getsymbol(parameters[i]->Name());
				kmap->Push(buff, &local);
			}
			else
				kmap->Push(buff, e);
		}
		else
			kmap->Push(buff, e);
		buff[pos]++;
	}
	return kmap;
}

bool AtanorPredicate::Boolean() {
	return true;
}

Atanor* Atanor::Predicate(AtanorDeclaration* dom, short idthread) {
	string name;
	AtanorPredicate* pred = NULL;
	Atanor* e;
	int idname = -1;
	if (isVectorContainer()) {
		//It is a vector container
		AtanorIteration* itr = Newiteration(false);
		bool first = true;
		//the first element should be the head name
		for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
			if (first == true) {
				name = itr->Valuestring();
				idname = globalAtanor->Getid(name);
				if (!globalAtanor->predicates.check(idname))
					globalAtanor->predicates[idname] = new AtanorPredicateFunction(globalAtanor, NULL, idname);
				pred = new AtanorPredicate(globalAtanor, idname);
				first = false;
				continue;
			}
			//Then we proceed with the parameters...
			e = itr->IteratorValue();
			pred->parameters.push_back(e->Atom(true));
			e->Release();
		}
		itr->Release();
		pred->Setprotect(1);
		return pred;
	}

	if (isMapContainer()) {
		e = Value("name");
		if (e == aNOELEMENT)
			return globalAtanor->Returnerror("PRE(003): Unknown predicate name", idthread);
		name = e->String();
		if (globalAtanor->symbolIds.find(name) == globalAtanor->symbolIds.end())
			return globalAtanor->Returnerror("PRE(003): Unknown predicate name", idthread);
		idname = globalAtanor->Getid(name);
		if (globalAtanor->predicates.find(idname) == globalAtanor->predicates.end())
			return globalAtanor->Returnerror("PRE(003): Unknown predicate name", idthread);

		pred = new AtanorPredicate(globalAtanor, idname);

		long sz = Size() - 1;
		char buff[] = { '0', 0, 0 };
		int pos = 0;
		for (long i = 0; i < sz; i++) {
			if (i && (i % 10) == 0) {
				buff[1] = '0';
				if (!pos) {
					buff[0] = '1';
					pos = 1;
				}
				else
					buff[0]++;
			}
			e = Value(buff);
			if (e == aNOELEMENT)
				return globalAtanor->Returnerror("PRE(004): Missing index parameter in the map", idthread);
			pred->parameters.push_back(e->Atom(true));
			buff[pos]++;
		}

		pred->Setprotect(1);
		return pred;
	}

	return globalAtanor->Returnerror("PRE(005): Cannot build a predicate out of that object", idthread);
}


AtanorPredicate::AtanorPredicate(short n, AtanorGlobal* g, short t, Atanor* parent) : name(n), AtanorBasisPredicate(g, parent) {
	ptype = t;
	disjunction = false;
	negation = false;

#ifdef _DEBUG
	nom = globalAtanor->Getsymbol(name);
#endif
}

//This is a hack (quite ugly) which is used to avoid recording into the tracker temporary elements...
AtanorPredicate::AtanorPredicate(AtanorGlobal* g, short n) : name(n) {
	ptype = a_predicate;
	disjunction = false;
	negation = false;

#ifdef _DEBUG
	nom = globalAtanor->Getsymbol(name);
#endif
}

void AtanorPredicateRuleElement::Addtail(AtanorPredicateRule* pred, int& disj) {
	if (instructions.size() == 0)
		return;
	if (instructions[0]->Type() != a_predicateruleelement) {
		if (disjunction && disj == -1)
			disj = pred->instructions.size();
		AtanorPredicateRuleItem* i = new AtanorPredicateRuleItem(globalAtanor, instructions[0], negation, disjunction, pred->instructions.size());
		pred->instructions.push_back(i);
	}

	if (instructions.back()->Type() == a_predicateruleelement)
		((AtanorPredicateRuleElement*)instructions.back())->Addtail(pred, disj);
	Release();
}

void AtanorPredicateRule::addfinal(AtanorPredicateContainer* kpc) {
	kpc->rules[name].push_back(this);
}


//-------------------------------------------------------------------------------------

bool AtanorPredicateVariableInstance::Insertvalue(Atanor* dom, Atanor* val, basebin_hash<Atanor*>& kept) {
	AtanorPredicateVariableInstance* v = (AtanorPredicateVariableInstance*)Variable((AtanorDeclaration*)dom);

	if (v->value != aNOELEMENT && v->value != aUNIVERSAL) {
		if (v->value->Insertvalue(dom, val, kept))
			return true;
	}

	kept[name] = v->value;
	v->value = val;
	val->Setreference(reference);
	return true;
}

bool AtanorPredicateTerm::Insertvalue(Atanor* dom, Atanor* val, basebin_hash<Atanor*>& kept) {
	AtanorPredicateTerm* kpt = (AtanorPredicateTerm*)val;
	for (long i = 0; i < parameters.size(); i++)
		parameters[i]->Insertvalue(dom, kpt->parameters[i], kept);
	return true;
}

bool Atanorvector::Insertvalue(Atanor* dom, Atanor* val, basebin_hash<Atanor*>& kept) {
	Atanorvector* kpt = (Atanorvector*)val;
	for (BLONG i = 0; i < values.size(); i++)
		values[i]->Insertvalue(dom, kpt->values[i], kept);
	return true;
}

bool AtanorConstmap::Insertvalue(Atanor* dom, Atanor* val, basebin_hash<Atanor*>& kept) {
	AtanorConstmap* kpt = (AtanorConstmap*)val;
	for (BLONG i = 0; i < values.size(); i++) {
		keys[i]->Insertvalue(dom, kpt->keys[i], kept);
		values[i]->Insertvalue(dom, kpt->values[i], kept);
	}
	return true;
}

//---------------------------------------------------------------------
//Unification of the rules with the predicate variables...
Atanor* AtanorPredicateVariableInstance::Value(AtanorDeclaration* dom) {
	Atanor* v = this;
	short i;
	ushort r;
	while (dom->declarations.get(v->Name(), i, r) && v != dom->declarations.table[i][r])
		v = dom->declarations.table[i][r];
	return ((AtanorPredicateVariableInstance*)v)->value;
}

Atanor* AtanorPredicateVariableInstance::Variable(AtanorDeclaration* dom) {
	Atanor* v = this;
	short i;
	ushort r;
	while (dom->declarations.get(v->Name(), i, r) && v != dom->declarations.table[i][r])
		v = dom->declarations.table[i][r];
	return v;
}

//---------------------------------------------------------------------

Atanor* Atanorsynode::ExtractPredicateVariables(Atanor* context, AtanorDeclaration* dom, Atanor* previousinstance, Atanor* e, short idthread, bool root) {
	//In this case, we are dealing with an element from the new rule to apply...
	//The previousinstance is the element which has been extracted from the goal stack...
	AtanorPredicateVariableInstance* kpvi;
	if (name == a_universal) {
		kpvi = new AtanorPredicateVariableInstance(predicatename++, a_universal);
		kpvi->Setreference();
		dom->declarations[kpvi->Name()] = kpvi;
	}
	else {
		if (context->Checkdico(name))
			kpvi = context->Getdico(name);
		else {
			kpvi = new AtanorPredicateVariableInstance(predicatename++, name);
			context->Setdico(name, kpvi);
			kpvi->Setreference();
			dom->declarations[kpvi->Name()] = kpvi;
		}
	}

	if (previousinstance == NULL) {
		kpvi->value = this;
		Setreference(kpvi->reference);
		return kpvi;
	}

	return kpvi->ExtractPredicateVariables(context, dom, previousinstance, NULL, idthread, root);
}


Atanor* AtanorBasePredicateVariable::ExtractPredicateVariables(Atanor* context, AtanorDeclaration* dom, Atanor* previousinstance, Atanor* e, short idthread, bool root) {
	//In this case, we are dealing with an element from the new rule to apply...
	//The previousinstance is the element which has been extracted from the goal stack...
	AtanorPredicateVariableInstance* kpvi;
	if (context->Checkdico(name))
		kpvi = context->Getdico(name);
	else {
		kpvi = new AtanorPredicateVariableInstance(predicatename++, name);
		context->Setdico(name, kpvi);		
		kpvi->Setreference();
		dom->declarations[kpvi->Name()] = kpvi;
	}

	if (previousinstance == NULL)
		return kpvi;

	return kpvi->ExtractPredicateVariables(context, dom, previousinstance, NULL, idthread, root);
}

Atanor* AtanorPredicateVariableInstance::ExtractPredicateVariables(Atanor* context, AtanorDeclaration* dom, Atanor* previousinstance, Atanor* E, short idthread, bool root) {
	if (previousinstance->Type() == a_instance) {
		previousinstance = previousinstance->Variable(dom);
		Atanor* val = previousinstance->VariableValue();
		if (val != aNOELEMENT) {
			if (value == aNOELEMENT) {
				value = val->Atom(true);
				value->Setreference(reference);
			}
			else {
				if (!value->Unify(dom, val))
					return NULL;
			}
		}

        //previousinstance is the variable, which should be matched with the new one ("this") through a new rule...
        //Example:
        //    acall(?X) : ...
        //    p = acall(?A);
        //In this example, previousinstance is ?A and "this" is ?X, the system tries to match ?A with ?X
        //We then simply record in our domain a link between the old name and the new name...
        //since the "dico" records a link between each variable and their pointer, we can simply traverse the domain
        //to find the top value... see AtanorPredicateVariableInstance::Value and AtanorPredicateVariableInstance::Variable
        //We have here a simple chained list of instance variables...
		if (dom->Declaration(previousinstance->Name()) != this) {
			dom->declarations[previousinstance->Name()] = this;
			Setreference();
		}
		return this;
	}

	if (previousinstance != aNOELEMENT) {
		if (value == aNOELEMENT) {
			value = previousinstance->Atom(true);
			value->Setreference(reference);
		}
		else {
			if (!value->Unify(dom, previousinstance))
				return NULL;
		}
	}

	return this;
}

Atanor* Atanor::ExtractPredicateVariables(Atanor* contextualpattern, AtanorDeclaration* dom, Atanor* param, Atanor* e, short idthread, bool root) {
	if (e == NULL) {
		dom->Setfail(false);
		e = Get(contextualpattern, dom, idthread);
		if (dom->Failed())
			return this;
	}
	if (param != NULL) {
		if (param->Type() == a_instance) {			
			AtanorPredicateVariableInstance* C = (AtanorPredicateVariableInstance*)param->Variable(dom);
			if (C->value == aUNIVERSAL)
				return param;

			if (C->value == aNOELEMENT) {
				short ln = ((AtanorPredicateVariableInstance*)param)->labelname;
				//In this case, we need to duplicate this variable, in order to clean it later on...
				//Otherwise, it will keep its value for the rest of the evaluation
				//we create a new predicate instance, with the same label but a new name...
				C = new AtanorPredicateVariableInstance(predicatename++, ln);
				if (e->isConst()) {
					//then we store our value...
					C->value = e->Atom(true);
				}
				else
					C->value = e;

				contextualpattern->Setdico(ln, C);

				dom->declarations[C->Name()] = C;
				dom->declarations[param->Name()] = C;
				C->Setreference(2);
				return e;
			}

			//else in this case, we use the value of that variable to compare it with the one expected stored in 'e'
			param = C->value;
		}

		if (e != aUNIVERSAL && param != NULL && param != aNOELEMENT && param != aNULL)
		if (e->same(param)->Boolean() == false)
			return NULL;
	}
	return e;
}


Atanor* AtanorConstvectormerge::ExtractPredicateVariables(Atanor* context, AtanorDeclaration* dom, Atanor* csend, Atanor* c, short idthread, bool root) {
	c = csend;
	if (csend != NULL) {
		if (csend->Type() != a_vector) {
			if (csend->Type() != a_instance)
				return NULL;
			c = csend->Variable(dom);
			if (c->VariableValue() != aNOELEMENT)
				c = c->VariableValue();
		}
	}

	int szc = -1;
	Atanor* e;
	csend = c;
	int i;
	Atanorvector* localvect = NULL;
	if (c != NULL && c->Type() == a_vector) {
		szc = c->Size();
		if (szc) {
			Atanor* lst = c->Last();
			if (lst->isMerge()) {
				localvect = globalAtanor->Providevector();
				for (i = 0; i < szc - 1; i++)
					localvect->values.push_back(c->getvalue(i));
				lst = lst->VariableValue()->getvalue(0)->VariableValue();
				for (i = 0; i < lst->Size(); i++)
					localvect->values.push_back(lst->getvalue(i));
				c = localvect;
				szc = c->Size();
			}
			else
				localvect = NULL;
		}
	}

	Atanorvector* vect = globalAtanor->Providevector();
	for (i = 0; i < values.size(); i++) {
		e = values[i];
		if (szc != -1) {
			//then, if it is the last element, then we associate our element with the rest of the vector
			if (i == values.size() - 1) {
				if (values.size() == 1)
					e = e->ExtractPredicateVariables(context, dom, c, NULL, idthread, false);
				else {
					//Else we need to extract a subset of that vector
					Atanorvector subvect;
					for (long j = i; j < c->Size(); j++)
						subvect.values.push_back(((Atanorvector*)c)->values[j]);
					e = e->ExtractPredicateVariables(context, dom, &subvect, NULL, idthread, false);
				}
			}
			else {
				if (szc <= i) {
					if (localvect != NULL) {
						localvect->values.clear();
						localvect->Release();
					}
					vect->Release();
					return NULL;
				}
				e = e->ExtractPredicateVariables(context, dom, ((Atanorvector*)c)->values[i], NULL, idthread, false);
			}
		}
		else
			e = e->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, false);

		if (e == NULL) {
			if (localvect != NULL) {
				localvect->values.clear();
				localvect->Release();
			}
			vect->Release();
			return NULL;
		}
		vect->Push(e);
	}

	if (localvect != NULL) {
		localvect->values.clear();
		localvect->Release();
	}
	
	AtanorPredicateVariableInstance* kpvi = new AtanorPredicateVariableInstance(predicatename++, a_map);

	if (isMerge())
		kpvi->merge = true;

	kpvi->value = vect;
	kpvi->Setreference();
	dom->declarations[kpvi->name] = kpvi;

	if (csend != NULL && csend->Type() == a_instance) {
		context->Setdico(csend->Name(), kpvi);
		dom->declarations[csend->Name()] = kpvi;
		kpvi->Setreference();
	}
	return kpvi;
}

Atanor* Atanorvector::ExtractPredicateVariables(Atanor* context, AtanorDeclaration* dom, Atanor* csend, Atanor* c, short idthread, bool root) {
	c = csend;
	if (csend != NULL) {
		if (!csend->isVectorContainer()) {
			if (csend->Type() != a_instance)
				return NULL;
			c = csend->Variable(dom);
			if (c->VariableValue() != aNOELEMENT)
				c = c->VariableValue();
		}
	}


	csend = c;
	int i, j;
	int sz = values.size();
	int szc = -1;
	Atanor* kval;
	AtanorPredicateVariableInstance* kpvi;
	Atanorvector* localvect = NULL;
	if (c != NULL && c->isVectorContainer()) {
		szc = c->Size();
		if (szc) {
			if (c->Last()->isMerge()) {
				if (!sz)
					return NULL;
				kpvi = (AtanorPredicateVariableInstance*)c->Last();
				localvect = globalAtanor->Providevector();
				for (i = 0; i < szc - 1; i++)
					localvect->values.push_back(c->getvalue(i));
				kval = kpvi->value->getvalue(0)->VariableValue();
				szc = kval->Size();
				for (i = 0; i < szc; i++)
					localvect->values.push_back(kval->getvalue(i));
				c = localvect;
				szc = localvect->values.size();
			}
			else
				localvect = NULL;
		}
		if (!sz || !values.back()->isMerge()) {
			if (szc != sz) {
				if (localvect != NULL) {
					localvect->values.clear();
					localvect->Release();
				}
				return NULL;
			}
		}
	}

	Atanor* e;
	Atanorvector* vect = globalAtanor->Providevector();
	bool merge = false;

	for (i = 0; i < sz; i++) {
		e = values[i];
		if (szc != -1) {
			//In the case of merge, we need to extract from c the rest of the vector...
			if (e->isMerge()) {
				if (c == localvect) {
					merge = true;
					localvect->values.erase(localvect->values.begin(), localvect->values.begin() + i);
					e = e->ExtractPredicateVariables(context, dom, localvect, NULL, idthread, false);
				}
				else {
					localvect = globalAtanor->Providevector();

					merge = false;
					for (j = i; j < c->Size(); j++) {
						kval = c->getvalue(j);
						if (kval->isObject()) {
							merge = true;
							kval->Setreference();
						}
						localvect->values.push_back(kval);
					}
					e = e->ExtractPredicateVariables(context, dom, localvect, NULL, idthread, false);
					if (merge) {
						for (j = 0; j < localvect->values.size(); j++)
							localvect->values[j]->Resetreference();
					}
					merge = true;
					localvect->values.clear();
					localvect->Release();
					localvect = NULL;
				}
			}
			else {
				if (szc <= i) {
					vect->Release();
					if (localvect != NULL) {
						localvect->values.clear();
						localvect->Release();
					}
					return NULL;
				}
				kval = c->getvalue(i);
				e = e->ExtractPredicateVariables(context, dom, kval, NULL, idthread, false);
				kval->Release();
			}
		}
		else
			e = e->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, false);

		if (e == NULL) {
			vect->Release();
			if (localvect != NULL) {
				localvect->values.clear();
				localvect->Release();
			}
			return NULL;
		}
		vect->Push(e);
	}

	if (localvect != NULL) {
		localvect->values.clear();
		localvect->Release();
	}

	if (root || merge) {
		kpvi = new AtanorPredicateVariableInstance(predicatename++, a_vector);

		kpvi->value = vect;
		dom->declarations[kpvi->name] = kpvi;
		kpvi->Setreference();
		if (csend != NULL && csend->Type() == a_instance) {
			context->Setdico(csend->Name(), kpvi);
			dom->declarations[csend->Name()] = kpvi;
			kpvi->Setreference();
		}
		return kpvi;
	}
	return vect;
}

Atanor* AtanorConstmap::ExtractPredicateVariables(Atanor* context, AtanorDeclaration* dom, Atanor* C, Atanor* E, short idthread, bool root) {
	Atanor* c = C;
	if (C != NULL) {
		if (!C->isMapContainer()) {
			if (C->Type() != a_instance)
				return NULL;
			c = C->Variable(dom);
			if (c->VariableValue() != aNOELEMENT)
				c = c->VariableValue();
		}
	}


	long sz = values.size();
	long szc = -1;
	if (c != NULL && c->isMapContainer()) {
		szc = c->Size();
		if ((!szc && sz) || (szc && !sz))
			return NULL;
		//No merge, then the size should be equal
		if (!sz || keys.back()->Name() != a_pipe) {
			if (sz != szc)
				return NULL;
		}
	}

	AtanorIteration* iter;
	if (c == NULL)
		iter = new AtanorIteration(false);
	else
		iter = c->Newiteration(false);
	iter->Begin();
	AtanorConstmap* kmap = new AtanorConstmap;
	Atanor* ek;
	Atanor* ev;
	Atanor* ck;
	Atanor* cv;
	int i;
	for (i = 0; i < sz; i++) {
		ev = values[i];
		ek = keys[i];
		if (szc != -1) {
			//In the case of merge, we need to extract from c the rest of the vector...
			if (ek->Name() == a_pipe) {
				//A merge...
				//We need to access all the remaining elements...
				Atanor* remaining = c->Newinstance(idthread);
				while (iter->End() == aFALSE) {
					ck = iter->IteratorKey();
					cv = iter->IteratorValue();
					remaining->Push(ck, cv);
					ck->Release();
					cv->Release();
					iter->Next();
				}
				ev = ev->ExtractPredicateVariables(context, dom, remaining, NULL, idthread, false);
				remaining->Release();
			}
			else {
				if (szc <= i) {
					iter->Release();
					kmap->Release();
					return NULL;
				}
				ck = iter->IteratorKey();
				cv = iter->IteratorValue();
				ek = ek->ExtractPredicateVariables(context, dom, ck, NULL, idthread, false);
				ev = ev->ExtractPredicateVariables(context, dom, cv, NULL, idthread, false);
				ck->Release();
				cv->Release();
				iter->Next();
			}
		}
		else {
			ek = ek->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, false);
			ev = ev->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, false);
		}

		if (ek == NULL || ev == NULL) {
			iter->Release();
			kmap->Release();
			return NULL;
		}

		kmap->keys.push_back(ek);
		kmap->values.push_back(ev);
	}
	iter->Release();

	if (root || merge) {
		AtanorPredicateVariableInstance* kpvi = new AtanorPredicateVariableInstance(predicatename++, a_map);

		kpvi->value = kmap;
		dom->declarations[kpvi->name] = kpvi;
		kpvi->Setreference();
		if (c != NULL && c->Type() == a_instance) {
			context->Setdico(c->Name(), kpvi);
			dom->declarations[c->Name()] = kpvi;
			kpvi->Setreference();
		}
		return kpvi;
	}
	return kmap;
}


Atanor* AtanorPredicateTerm::ExtractPredicateVariables(Atanor* context, AtanorDeclaration* dom, Atanor* C, Atanor* E, short idthread, bool root) {
	Atanor* c = C;
	int i;
	bool param = false;
	AtanorPredicateTerm* term;

	if (C != NULL) {
		if (C->Type() == a_term) {
			if (C->Name() != name || C->Size() != parameters.size()) {
				return NULL;
			}
			param = true;
		}
		else {
			if (C->Type() != a_instance)
				return NULL;
			c = C->Variable(dom);
			if (c->VariableValue() != aNOELEMENT) {
				c = c->VariableValue();
				if (c->Type() != a_term || c->Name() != name || c->Size() != parameters.size())
					return NULL;
			}

			if (c->Type() == a_term)
				param = true;
		}
	}

	Atanor* e;
	//term = new AtanorPredicateTerm(globalAtanor, name, label);
	term = (AtanorPredicateTerm*)Newinstance(idthread);
	term->Set(name);

	for (i = 0; i < parameters.size(); i++) {
		e = parameters[i];
		if (param)
			e = e->ExtractPredicateVariables(context, dom, c->Parameter(i), NULL, idthread, false);
		else
			e = e->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, false);

		if (e == NULL) {
			term->Release();
			return NULL;
		}
		term->parameters.push_back(e);
	}

	if (root) {
		AtanorPredicateVariableInstance* kpvi = new AtanorPredicateVariableInstance(predicatename++, a_term);

		kpvi->value = term;
		kpvi->Setreference();
		dom->declarations[kpvi->name] = kpvi;

		if (c != NULL && c->Type() == a_instance) {
			context->Setdico(c->Name(), kpvi);
			dom->declarations[c->Name()] = kpvi;
			kpvi->Setreference();
		}
		return kpvi;
	}

	return term;
}
//---------------------------------------------------------------------

AtanorPredicate* AtanorPredicateKnowledgeBaseFunction::Duplicate(Atanor* context, AtanorDeclaration* dom) {
	Atanor * e;

	AtanorPredicateKnowledgeBaseFunction* p;

	if (context == NULL && dom == NULL) {
		p = (AtanorPredicateKnowledgeBaseFunction*)Newinstance(0);

		for (long i = 0; i < parameters.size(); i++) {
			e = parameters[i]->Atom(true);
			p->parameters.push_back(e);
		}
		p->Setreference();
		return p;
	}

	p = (AtanorPredicateKnowledgeBaseFunction*)Newinstance(0);

	int i, j;
	for (i = 0; i < parameters.size(); i++) {
		e = parameters[i]->ExtractPredicateVariables(context, dom, NULL, NULL, globalAtanor->GetThreadid(), true);
		if (e == NULL) {
			for (j = 0; j < i; j++) {
				//These elements will be cleared in AtanorPredicateRule::Evalue
				if (p->parameters[j]->Type() == a_instance)
					p->parameters[j] = aNULL;
			}
			p->Release();
			return NULL;
		}

		p->parameters.push_back(e);
		e->Setprotect(0);
	}

	p->Setreference();
	return p;
}

AtanorPredicate* AtanorPredicate::Duplicate(Atanor* context, AtanorDeclaration* dom) {
	if (name == a_cut || name == a_fail || name == a_stop)
		return this;

	Atanor * e;

	AtanorPredicate* p;

	if (context == NULL && dom == NULL) {
		p = new AtanorPredicate(globalAtanor, name);
		for (long i = 0; i < parameters.size(); i++) {
			e = parameters[i]->Atom(true);
			p->parameters.push_back(e);
		}
		p->Setreference();
		return p;
	}

	p = new AtanorPredicate(globalAtanor, name);

	p->negation = negation;

	int i, j;
	short idthread = globalAtanor->GetThreadid();
	for (i = 0; i < parameters.size(); i++) {
		e = parameters[i]->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, true);
		if (e == NULL) {
			for (j = 0; j < i; j++) {
				//These elements will be cleared in AtanorPredicateRule::Evalue
				if (p->parameters[j]->Type() == a_instance)
					p->parameters[j] = aNULL;
			}
			p->Release();
			return NULL;
		}

		p->parameters.push_back(e);
		e->Setprotect(0);
	}

	p->Setreference();
	return p;
}

AtanorPredicate* AtanorDependency::Duplicate(Atanor* context, AtanorDeclaration* dom) {
	if (name == a_cut || name == a_fail || name == a_stop)
		return this;

	Atanor * e;

	AtanorDependency* p;

	if (context == NULL && dom == NULL) {
		p = new AtanorDependency(globalAtanor, aNULL, name, idvar);
		if (features->Type() == a_mapss) {
			p->features = globalAtanor->Providemapss();
			((Atanormapss*)p->features)->values = ((Atanormapss*)features)->values;
		}

		for (long i = 0; i < parameters.size(); i++) {
			e = parameters[i]->Atom(true);
			p->parameters.push_back(e);
		}
		p->Setreference();
		return p;
	}

	p = new AtanorDependency(globalAtanor, aNULL, name, idvar);
	if (features->Type() == a_mapss) {
		p->features = globalAtanor->Providemapss();
		((Atanormapss*)p->features)->values = ((Atanormapss*)features)->values;
	}

	p->negation = negation;

	int i, j;
	short idthread = globalAtanor->GetThreadid();
	for (i = 0; i < parameters.size(); i++) {
		e = parameters[i]->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, true);
		if (e == NULL) {
			for (j = 0; j < i; j++) {
				//These elements will be cleared in AtanorPredicateRule::Evalue
				if (p->parameters[j]->Type() == a_instance)
					p->parameters[j] = aNULL;
			}
			p->Release();
			return NULL;
		}

		p->parameters.push_back(e);
		e->Setprotect(0);
	}

	p->Setreference();
	return p;
}

AtanorPredicate* AtanorPredicate::Copyfrom(Atanor* context, AtanorDeclaration* dom, AtanorPredicate* C, short idthread) {
	Atanor * e;

	AtanorPredicate* p = new AtanorPredicate(globalAtanor, name);

	for (long i = 0; i < parameters.size(); i++) {
		e = parameters[i]->ExtractPredicateVariables(context, dom, C->parameters[i], NULL, idthread, true);
		if (e == NULL) {
			p->Release();
			return NULL;
		}
		p->parameters.push_back(e);
		e->Setreference();
	}

	p->reference++;
	return p;
}

//---------------------------------------------------------------------

Exporting Atanor* Atanor::EvaluePredicateVariables(Atanor* context, AtanorDeclaration* dom) {
	Atanor* e = Get(context, dom, globalAtanor->GetThreadid());
	return e->Atom(true);
}

Atanor* AtanorBasePredicateVariable::EvaluePredicateVariables(Atanor* context, AtanorDeclaration* dom) {
	AtanorPredicateVariableInstance* kpi = new AtanorPredicateVariableInstance(predicatename++, name);
	dom->declarations[kpi->name] = kpi;
	return kpi;
}

Atanor* AtanorPredicateVariableInstance::EvaluePredicateVariables(Atanor* context, AtanorDeclaration* dom) {
	Atanor* v = Variable(dom);

	if (v->VariableValue() != aNOELEMENT)
		return v->VariableValue();
	return v;
}

Atanor* Atanorvector::EvaluePredicateVariables(Atanor* context, AtanorDeclaration* dom) {
	Atanor* e;

	Atanorvector* vect = globalAtanor->Providevector();
	for (BLONG i = 0; i < values.size(); i++) {
		e = values[i]->EvaluePredicateVariables(context, dom);
		if (e == NULL)
			vect->Push(values[i]);
		else
			vect->Push(e);
	}
	return vect;
}

Atanor* AtanorConstmap::EvaluePredicateVariables(Atanor* context, AtanorDeclaration* dom) {
	Atanor* e;

	AtanorConstmap* kmap = new AtanorConstmap;
	for (BLONG i = 0; i < values.size(); i++) {
		e = keys[i]->EvaluePredicateVariables(context, dom);
		if (e == NULL)
			kmap->keys.push_back(values[i]);
		else
			kmap->keys.push_back(e);

		e = values[i]->EvaluePredicateVariables(context, dom);
		if (e == NULL)
			kmap->values.push_back(values[i]);
		else
			kmap->values.push_back(e);
	}
	return kmap;
}

Atanor* AtanorPredicateTerm::EvaluePredicateVariables(Atanor* context, AtanorDeclaration* dom) {
	Atanor * e;
	AtanorPredicateTerm* model = new AtanorPredicateTerm(globalAtanor, name);

	for (long i = 0; i < parameters.size(); i++) {
		e = parameters[i]->EvaluePredicateVariables(context, dom);
		if (e == NULL)
			model->parameters.push_back(parameters[i]);
		else
			model->parameters.push_back(e);
	}
	return model;
}
//---------------------------------------------------------------------

AtanorPredicate* AtanorInstructionEvaluate::PredicateUnification(VECTE<Atanor*>& goals, int& posreplace, int& from) {
	posreplace = -1;
	from = 0;
	AtanorPredicate* headpredicate = NULL;
	Atanor* e;
	Atanor* res;
	bool keep = false;
	bool test;

	for (long i = 0; i < goals.size(); i++) {
		e = goals[i];
		if (e->isPredicate()) {
			if (e == aFAIL || e == aSTOP)
				return (AtanorPredicate*)e;

			if (e == aCUT)
				test = false;
			else //isUnified tests whether the current predicate is fully "unifiable": all variables have been unified...
				test = ((AtanorPredicate*)e)->isUnified(dom);

			short eval = ((AtanorPredicate*)e)->kbaction();

			if (test) {
				//If it is an action, such as assert then we return the current headpredicate...				
				if (eval >= a_asserta && eval <= a_remove) {
					if (!keep) {
						headpredicate = (AtanorPredicate*)e;
						from = i + 1;
					}
					return headpredicate;
				}

				//We then evaluate it... This function returns 3 values:
				// 0: fail
				// 1: does not match any fact in the knowledgebase fact
				// 2: found a matching fact in the knowledge base
				eval = globalAtanor->isaValidPredicate(dom, ((AtanorPredicate*)e), rules, threadowner);
				if (e->isNegation()) {
					//if we have a match in the knowledge base, then it is an error
					if (eval == 2)
						return aFAIL;
					//A case a little bit complex
					//If we have a negation then we will need to evaluate more thoroughly this element
					//It might not belong to the knowledgebase
					if (eval == 1) {
						if (!keep) {
							headpredicate = (AtanorPredicate*)e;
							keep = true;
							from = i + 1;
						}
					}
				}
				else {
					if (eval == 0)
						return aFAIL;
					else
					if (eval == 1)
						test = false;
				}
			}

			if (!test) { //still some variables to go

				if (headpredicate == NULL && ((AtanorPredicate*)e)->Checkparameters(dom)) {
					if (eval == a_asserta || eval == a_assertz)
						return aFAIL;
					headpredicate = (AtanorPredicate*)e;
					posreplace = i;
					keep = true;
				}
			}
			else {
				if (!keep) //If we have not found anything yet as a head, we will start the evaluation from the following element...
					from = i + 1;
			}
		}
		else
		if (headpredicate == NULL) {
			res = e->Get(this, dom, threadowner);
			if (res->Boolean() == false) {
				//if res==aNOELEMENT, then it means that there are no unified variables...
				if (!e->isNegation() && !dom->Failed() && res != aNOELEMENT)
					return aFAIL;
				keep = true;
			}
			else {
				if (e->isNegation())
					return aFAIL;
				if (!keep)
					from = i + 1;
			}
		}
		else
			e->setSucessfull(false);
	}
	return headpredicate;
}

//-------------------------------------------------------------------
Atanor* Atanor::Getvalues(AtanorDeclaration* dom, bool duplicate) {
	if (duplicate)
		return Atom(true);
	return this;
}

Atanor* Atanorsynode::Getvalues(AtanorDeclaration* dom, bool duplicate) {
	return this;
}

Atanor* AtanorPredicateVariableInstance::Getvalues(AtanorDeclaration* dom, bool duplicate) {
	Atanor* v = Value(dom);
	v = v->Getvalues(dom, duplicate);
	if (merge && v->Type() == a_vector)
		v->Setmerge();
	return v;
}

Atanor* AtanorPredicateTerm::Getvalues(AtanorDeclaration* dom, bool duplicate) {
	AtanorPredicateTerm* term = new AtanorPredicateTerm(globalAtanor, name);
	Atanor* e;
	for (long i = 0; i < parameters.size(); i++) {
		e = parameters[i]->Getvalues(dom, duplicate);
		if (e == aNOELEMENT) {
			term->Release();
			return aNOELEMENT;
		}
		term->parameters.push_back(e);
	}
	return term;
}

Atanor* Atanorvector::Getvalues(AtanorDeclaration* dom, bool duplicate) {
	Atanorvector* kvect = globalAtanor->Providevector();
	Atanor* e;
	for (BLONG i = 0; i < values.size(); i++) {
		e = values[i]->Getvalues(dom, duplicate);
		if (e == aNOELEMENT) {
			kvect->Release();
			return aNOELEMENT;
		}
		if (e == NULL || e == aUNIVERSAL)
			continue;
		if (e->isMerge()) {
			Atanor* kv = e->getvalue(0);
			if (kv->Type() == a_vector) {
				Atanorvector* vect = (Atanorvector*)kv;
				for (BLONG v = 0; v < vect->values.size(); v++)
					kvect->Push(vect->values[v]);
				if (!e->isObject())
					kv->Resetreference();
				else
					e->Release();
			}
		}
		else
			kvect->Push(e);
	}
	return kvect;
}

Atanor* AtanorConstmap::Getvalues(AtanorDeclaration* dom, bool duplicate) {
	Atanortreemap* kmap = new Atanortreemap;
	Atanor* ev;
	Atanor* ek;
	for (BLONG i = 0; i < values.size(); i++) {
		ek = keys[i]->Getvalues(dom, duplicate);
		if (ek == aNOELEMENT) {
			ev->Release();
			kmap->Release();
			return aNOELEMENT;
		}
		if (ek == NULL || ek == aUNIVERSAL)
			continue;

		ev = values[i]->Getvalues(dom, duplicate);
		if (ev == aNOELEMENT) {
			kmap->Release();
			return aNOELEMENT;
		}
		if (ev == NULL || ev == aUNIVERSAL)
			continue;

		if (ek->Name() == a_pipe) {
			//a merging...
			//in that case, ev must be a map...
			if (ev->isMapContainer()) {
				Atanor* tobecleaned = ev;
				//We take all the values from ev into the current map...
				AtanorIteration* it = ev->Newiteration(false);
				for (it->Begin(); it->End() == aFALSE; it->Next()) {
					ek = it->IteratorKey();
					ev = it->IteratorValue();
					kmap->Push(STR(ek->String()), ev);
				}
				tobecleaned->Release();
				it->Release();
				continue;
			}
		}

		kmap->Push(STR(ek->String()), ev);
		ek->Release();
		ev->Release();
	}
	return kmap;
}

Atanor* AtanorInstructionEvaluate::PredicateCreate(AtanorPredicate* headrule, int depth) {
	//This a specific case, when the system is looking for a subgoal (as in a negation)
	if (headrule == NULL)
		return aTRUE;

	AtanorPredicate* pv = new AtanorPredicate(globalAtanor, headrule->name);
	Atanor* e;
	int i;
	for (i = 0; i < headrule->parameters.size(); i++) {
		e = headrule->parameters[i]->Getvalues(dom, true);
		if (e == aNOELEMENT) {
			pv->Release();
			return aFALSE;
		}
		pv->parameters.push_back(e);
		e->Setprotect(0);
	}

	if (trace) {
		stringstream s;
		for (i = 0; i < depth; i++)
			s << " ";
		s << "success:" << depth << "=" << pv->String() << endl;
		PreLocalPrint(s.str());
	}

	pv->Setreference();

	Atanor* func = headrule->Function();
	if (func != NULL) {
		AtanorCallFunction kfunc(func);
		func = ((AtanorPredicateFunction*)headrule)->object;
		func->Setreference();
		kfunc.arguments.push_back(pv);
		kfunc.arguments.push_back(func);
		e = kfunc.Get(this, dom, threadowner);
		func->Resetreference();
		if (!e->Boolean()) {
			pv->Resetreference();
			if (e->isError())
				return e;
			return aBREAK;
		}
	}

	results.push_back(pv);
	return aTRUE;
}

//-----------------------------------------------------
//-----------------------------------------------------
void Displaypredicatestack(char ty, AtanorDeclaration* dom, AtanorPredicate* kpr, VECTE<Atanor*>& goals, int depth, AtanorInstructionEvaluate* ctx) {
	//AffichageDom(dom,depth);
	stringstream s;
	bool displayed = false;
	bool first = true;
	Atanor* se;
	int i, j, sz = goals.size();
	char sep = ',';
	for (i = -1; i < sz; i++) {
		if (i == -1)
			se = kpr;
		else
			se = goals[i];
		if (se == NULL)
			continue;
		if (se->isPredicate()) {
			if (!displayed) {
				for (j = 0; j < depth; j++)
					s << " ";
				s << ty << ":" << depth << "=";
			}

			if (first == false)
				s << sep;
			if (i != -1)
				first = false;
			displayed = true;
			AtanorPredicate* kpi = (AtanorPredicate*)se;
			string n = globalAtanor->Getsymbol(kpi->name);
			if (kpi->isNegation())
				s << "~";
			s << n;
			if (kpi->isConst())
				continue;
			s << "(";
			for (j = 0; j < kpi->parameters.size(); j++) {
				n = kpi->parameters[j]->String();
				if (j)
					s << ",";
				s << n;
			}
			s << ")";
		}
		else {
			if (!displayed) {
				for (j = 0; j < depth; j++)
					s << " ";
				s << ty << ":" << depth << "=";
			}
			if (first == false)
				s << sep;
			if (i != -1)
				first = false;
			displayed = true;
			if (se->Name() != -1 || (se->Action() >= a_affectation && se->Action() <= a_notin))
				s << se->String();
			else
				s << "[i]";
		}
		if (i == -1) {
			if (sz)
				s << " --> ";
			else
				s << ".";
		}
		sep = ',';
		if (se->isDisjunction())
			sep = ';';
	}

	if (displayed)
		PreLocalPrint(s.str());
}
//---------------------------------------------------------------------

class localpredict {
public:
	VECTE<Atanor*> localgoals;
	short idthread;

	localpredict(short id) {
		idthread = id;
		globalAtanor->Pushpredicate(id);

	}

	~localpredict() {
		globalAtanor->Poppredicate(idthread);
	}

};

class predictvalue {
public:

	VECTE<Atanor*> localgoals;
	VECTE<Atanor*> currentgoals;
	
	short idthread;

	predictvalue(int id) {
		idthread = id;
		globalAtanor->Pushpredicate(id);

	}

	~predictvalue() {
		globalAtanor->Poppredicate(idthread);
	}
};

class kbpredict {
public:

	vector<AtanorPredicate*> kbase;
	basebin_hash<Atanor*> kept;

};

Atanor* AtanorInstructionEvaluate::PredicateEvalue(VECTE<Atanor*>& goals, AtanorPredicate* currenthead, int depth) {
	if (fulltraversal == STOPSEARCH)
		return aCUT;

	if (globalAtanor->Error(threadowner))
		return aFALSE;

	Atanor* res = aFALSE;
	size_t sz = goals.size();
	int i;	

	//--------------------------------------------------------------------------------
	// Disjunction
	//If the top element in the goal stack is a disjunction, we then evaluate the next one...
	//The current one will be evaluated after... The disjunction is evaluated backward...
	if (sz && goals[0]->isDisjunction()) {
		std::unique_ptr<localpredict> Oo(new localpredict(threadowner));

		Oo->localgoals.taillor(sz);
		for (i = 1; i < sz; i++)
			Oo->localgoals.vecteur[i - 1] = goals.vecteur[i];
		Oo->localgoals.last = sz - 1;
		//---------------------------------------------------------------
		res = PredicateEvalue(Oo->localgoals, currenthead, depth);
		//---------------------------------------------------------------
		if (res == aCUT)
			return aTRUE;
		if (res == aBREAK || res == aTRUE)
			return res;
	}

	int posreplace, from;
	//--------------------------------------------------------------------------------
	//The evaluation of the goals, in which we detect which are the goals that full unified...
	// It returns NULL if everything has been unified, or the next predicate to be unified...
	AtanorPredicate* headpredicate = PredicateUnification(goals, posreplace, from);
	
	//--------------------------------------------------------------------------------
	//We want to stop any further analysis...
	if (headpredicate == aSTOP) {
		fulltraversal = STOPSEARCH;
		return aCUT;
	}
	//--------------------------------------------------------------------------------
	//If the unification went wrong...
	if (headpredicate == aFAIL) {
		if (trace) {
			stringstream scerr;
			scerr << " --> Fail" << endl;
			PreLocalPrint(scerr.str());
		}
		return res;
	}

	//--------------------------------------------------------------------------------
	int j;
	//if we have a CUT...
	if (headpredicate == aCUT) {
		std::unique_ptr<localpredict> Oo(new localpredict(threadowner));

		for (j = from; j < sz; j++) {
			if (j == posreplace || goals[j]->hasbeenSuccesfull())
				continue;
			Oo->localgoals.push_back(goals[j]);
		}
		//---------------------------------------------------------------
		res = PredicateEvalue(Oo->localgoals, currenthead, depth);
		//---------------------------------------------------------------
		if (res == aTRUE)
			return aCUT;
		return res;
	}

	//--------------------------------------------------------------------------------
	//It is already all unified... The goal list is now empty, we can create our resulting predicates.	
	if (headpredicate == NULL) {
		if (trace)
			PreLocalPrint(Endl);
		for (j = from; j < sz; j++) {
			if (!goals[j]->hasbeenSuccesfull())
				return res;
		}
		res = PredicateCreate(headrule, depth);
		if (res == aTRUE && fulltraversal == SEARCHONE) {
			fulltraversal = STOPSEARCH;
			return aCUT;
		}

		return res;
	}

	//--------------------------------------------------------------------------------
	// Functions that have constructed on the basis of existing methods...
	if (headpredicate->isPredicateFunction()) {
		std::unique_ptr<localpredict> Oo(new localpredict(threadowner));

		if (headpredicate->negation) {	

			Oo->localgoals.push_back(headpredicate);
			//We do not need to explore the whole world, one TRUE is enough to decide
			Oo->localgoals.push_back(aCUT);

			headpredicate->Setnegation(false);
			AtanorPredicate* rec = headrule;
			//We do not need to build anything, the headrule is set to NULL to tell the system
			//That we are only interested in a local evaluation...
			headrule = NULL;
			//---------------------------------------------------------------
			res = PredicateEvalue(Oo->localgoals, currenthead, depth + 1);
			//---------------------------------------------------------------
			headrule = rec;
			headpredicate->Setnegation(true);

			//---------------------------------------------------------------
			if (res == aFALSE) {//Ok we can continue with our remaining goals...
				Oo->localgoals.clear();
				for (j = from; j < sz; j++) {
					if (j == posreplace || goals[j]->hasbeenSuccesfull())
						continue;
					Oo->localgoals.push_back(goals[j]);
				}

				return PredicateEvalue(Oo->localgoals, currenthead, depth + 1);
			}

			//---------------------------------------------------------------
			//It was TRUE then it is FALSE...
			return aFALSE;
		}
	

		for (j = from; j < sz; j++) {
			if (j == posreplace || goals[j]->hasbeenSuccesfull())
				continue;
			Oo->localgoals.push_back(goals[j]);
		}

		//---------------------------------------------------------------
		return headpredicate->PredicateEvalue(this, Oo->localgoals, currenthead, depth);
		//---------------------------------------------------------------
	}

	//--------------------------------------------------------------------------------
	// Assertion and dependency removal are treated here...
	short kbaction = headpredicate->kbaction();
	AtanorPredicate* kvpred;
	if (kbaction == a_asserta || kbaction == a_assertz || kbaction == a_remove) {
		Atanor* a = NULL;
		if (kbaction == a_remove) {
			a = dom->Declaration(predicatedependency);
			if (a == NULL || !a->isPredicate())
				return aFALSE;

			if (headpredicate->Idvar()) {
				//If it is a dependency modification
				//we remove it from the database...
				dom->Declare(predicatefeature, a);
				a->Setreference(); //we do not want this object to be fully deleted here
			}			

			if (!globalAtanor->RemoveThePredicate(dom, (AtanorPredicate*)a, threadowner))
				return aFALSE;
		}
		else {
			if (headpredicate->isDependency()) {
				a = dom->Declaration(predicatefeature);
				if (a != NULL) {//we clear it...
					dom->declarations.erase(predicatefeature);
					kvpred = new AtanorDependency(globalAtanor,
						((AtanorPredicate*)a)->Features(),						
						headpredicate->name,
						headpredicate->Idvar());

					kvpred->Setfeatures(headpredicate->Features());
				}
				else {
					kvpred = new AtanorDependency(globalAtanor,
						headpredicate->Features(),
						headpredicate->name,
						headpredicate->Idvar());
				}					
				kvpred->Setidrule(headpredicate->Idrule());
			}
			else
				kvpred = new AtanorPredicate(globalAtanor, headpredicate->name);

			for (i = 0; i < headpredicate->parameters.size(); i++) {
				res = headpredicate->parameters[i]->Get(this, dom, threadowner);
				kvpred->parameters.push_back(res->Atom(true));
			}

			if (kbaction == a_asserta)
				globalAtanor->StorePredicate(dom, kvpred, false, threadowner);
			else
				globalAtanor->StorePredicate(dom, kvpred, true, threadowner);

			if (a != NULL) {
				a->Resetreference();
				//we then reinject our novel dependency into our dom
				//it might be usefull for other loops...
				dom->Declare(predicatedependency, kvpred);
			}
		}

		std::unique_ptr<localpredict> Oo(new localpredict(threadowner));
		for (j = from; j < sz; j++) {
			if (j == posreplace || goals[j]->hasbeenSuccesfull())
				continue;
			Oo->localgoals.push_back(goals[j]);
		}
		//---------------------------------------------------------------
		return PredicateEvalue(Oo->localgoals, currenthead, depth);
		//---------------------------------------------------------------
	}

	if (trace)
		PreLocalPrint(Endl);

	//--------------------------------------------------------------------------------
	std::unique_ptr<predictvalue> Oo(new predictvalue(threadowner));
	//Some variables still need to be evaluated

	//We will try to solve headpredicate...
	//first it could be part of the knowledgebase

	//We only keep the elements that are still to be evaluated...
	bool check = false;
	for (j = from; j < sz; j++) {
		if (goals[j]->hasbeenSuccesfull())
			continue;
		if (check) {
			if (!goals[j]->isDisjunction())
				check = false;
			continue;
		}
		if (j == posreplace)
			posreplace = Oo->currentgoals.size();

		if (goals[j]->isDisjunction())
			check = true;
		Oo->currentgoals.push_back(goals[j]);
	}

	check = false;
	Atanor* localres;
	
	//--------------------------------------------------------------------------------
	if (globalAtanor->Checkpredicate(headpredicate->name, threadowner)) {
		std::unique_ptr<kbpredict> Ko(new kbpredict());
		// This section is used to test our current predicate against the knowledgebase...
		if (globalAtanor->GetPredicates(dom, headpredicate, Ko->kbase, false, threadowner)) {			
			//we set the res value to aTRUE, which will be considered as an error
			if (headpredicate->isNegation())
				res = aTRUE;
			else {
				sz = Oo->currentgoals.size();
				Oo->localgoals.reserve(sz - 1);
				Oo->localgoals.last = sz - 1;
				i = 0;
				for (j = 0; j < sz; j++) {
					if (j == posreplace)
						continue;
					Oo->localgoals.vecteur[i++] = Oo->currentgoals.vecteur[j];
				}

				sz = Ko->kbase.size();
				bool ref;

				basebin_hash<Atanor*>::iterator it;
				//we remove our element at the position posreplace in goal stack
				for (i = 0; i < sz; i++) {

					//we then unify our values in dom with respect to the element in the goal stack (stored in headpredicate) and the extracted elements
					for (j = 0; j < headpredicate->parameters.size(); j++)
						headpredicate->parameters[j]->Insertvalue(dom, Ko->kbase[i]->parameters[j], Ko->kept);

					if (trace)
						Displaypredicatestack('k', dom, headpredicate, Oo->localgoals, depth, this);

					//---------------------------------------------------------------
					//In the case of dependency goals, we might need to store in a variable the current goal...
					ref = headpredicate->Setinvariable(Ko->kbase[i], dom, threadowner);
					//If it is a potential dependency modification, then we protect it ahead of processing...
					//Otherwise, the dependency could linger in kbase, being deleted...
					//The explanation is that, we could have many occurrences of the same dependency name in the rule.
					//If we store the current one as being a potential modification, and we go into recursion, then this current dependency might be 
					//deleted, however, since there are more than one loop in the knwoeledge base with the same query on the these dependencies, 
					//the current dependency might find itself in this second survey...
					if (ref) 
						Ko->kbase[i]->Setreference();
					//---------------------------------------------------------------
					//We then continue on the rest of the goals...
					localres = PredicateEvalue(Oo->localgoals, currenthead, depth + 1);
					//---------------------------------------------------------------
					//we then remove it, if it was inserted
					headpredicate->Resetintvariable(dom, threadowner);
					if (ref) //we can safely return back to the actual reference of that dependency...
						Ko->kbase[i]->Resetreference();
					//---------------------------------------------------------------


					if (localres == aTRUE || localres == aCUT) {
						if (kbaction == a_retract)
							globalAtanor->RemoveThePredicate(dom, Ko->kbase[i], threadowner);

						if (check) {
							for (it = Ko->kept.begin(); it.table != NULL; it++)
								dom->declarations[it->first]->Cleans(it->second, false);
							if (localres == aCUT)
								return aTRUE;
							return localres;
						}
						else
							res = localres;
					}

					//We clean them, then...
					for (it = Ko->kept.begin(); it.table != NULL; it++)
						dom->declarations[it->first]->Cleans(it->second, true);

					Ko->kept.clear();

					if (localres == aCUT)
						return aTRUE;

					if (localres == aBREAK)
						return localres;

					if (globalAtanor->Error(threadowner))
						return aFALSE;
				}
			}
		}
	}

	//--------------------------------------------------------------------------------
	//Else there is no rules matching this predicate...
	if (rules.find(headpredicate->name) == rules.end()) {		
		if (headpredicate->negation) {
			//---------------------------------------------------------------
			if (res == aFALSE)
				//we then continue to analyse the rest of the goal stack
				return PredicateEvalue(Oo->currentgoals, currenthead, depth + 1);
			//---------------------------------------------------------------
			return aFALSE;
		}
		return res; //no rule available
	}


	//--------------------------------------------------------------------------------
	//We have a negation...
	if (headpredicate->negation) {
		//We need to build a local call, which should return FALSE... We need only one TRUE to detect if the negation should fail		
		Oo->localgoals.clear();		

		Oo->localgoals.push_back(headpredicate);
		//We do not need to explore the whole world, one TRUE is enough to decide
		Oo->localgoals.push_back(aCUT);

		headpredicate->Setnegation(false);
		AtanorPredicate* rec = headrule;
		//We do not need to build anything, the headrule is set to NULL to tell the system
		//That we are only interested in a local evaluation...
		headrule = NULL;
		//---------------------------------------------------------------
		res = PredicateEvalue(Oo->localgoals, currenthead, depth + 1);
		//---------------------------------------------------------------
		headrule = rec;
		headpredicate->Setnegation(true);
		
		//---------------------------------------------------------------
		if (res == aFALSE) //Ok we can continue with our remaining goals...
			return PredicateEvalue(Oo->currentgoals, currenthead, depth + 1);
		//---------------------------------------------------------------
		//It was TRUE then it is FALSE...
		return aFALSE;
	}

	//--------------------------------------------------------------------------------
	//we extract the rules matching our current target predicate...
	vector<AtanorPredicateRule*>& rulebase = rules[headpredicate->name];

	sz = rulebase.size();
	AtanorPredicateRule* rb;


	//--------------------------------------------------------------------------------
	//Now in the following section we evaluate which rules to enrich our goals with...
	//we sort the values according to some random algorithm
	if (sz)
		currenthead = (AtanorPredicate*)Oo->currentgoals.remove(posreplace);

	VECTE<Atanor*> rulegoals;
	basebin_hash<Atanor*> basedomain = dom->declarations;
	std::unique_ptr<basebin_hash<AtanorPredicateVariableInstance*> > refdico(new basebin_hash<AtanorPredicateVariableInstance*>);

	for (i = 0; i < sz; i++) {
		rb = rulebase[i];
		if (rb->head->parameters.size() != currenthead->parameters.size())
			continue;

		Oo->localgoals = Oo->currentgoals;
		//Variable management
		dico = refdico.get();

		//localres = rulebase[i]->Evalue(this, currenthead, Oo->localgoals, posreplace, depth, cut);

		if (trace)
			Displaypredicatestack('r', dom, rb->head, Oo->localgoals, depth, this);

		//headpredicate is not stored in the garbage collector, no need for that. Hence, we only need to destroy it to get rid of it...
		headpredicate = rb->head->Copyfrom(this, dom, currenthead, threadowner);
		if (headpredicate == NULL) {
			ClearInstances(dom, basedomain);

			if (trace) {
				stringstream scerr;
				scerr << " --> Fail" << endl;
				PreLocalPrint(scerr.str());
			}
			localres = aFALSE;
		}
		else {

			if (trace)
				PreLocalPrint(Endl);

			localres = aTRUE;

			if (rb->instructions.size() != 0) {
				AtanorPredicateRuleItem* ee;
				for (j = 0; j < rb->instructions.size(); j++) {
					ee = (AtanorPredicateRuleItem*)rb->instructions[j];
					if (ee->item->isPredicate()) {
						//In that case, it is worth noting that the object IS NOT STORED in the garbage collector
						//The same applies to kpredi below... We will then be able to destroy them in one single pass
						kvpred = ((AtanorPredicate*)ee->item)->Duplicate(this, dom);
						if (kvpred == NULL)  {
							localres = aFALSE;
							break;
						}
						switch (kvpred->name) {
						case a_cut:
							rulegoals.push_back(aCUT);
							break;
						case a_fail:
							rulegoals.push_back(aFAIL);
							break;
						case a_stop:
							rulegoals.push_back(aSTOP);
							break;
						default:
							kvpred->set(ee->isNegation(), ee->disjunction);
							rulegoals.push_back(kvpred);
						}
					}
					else {
						if (ee->item->isConst())
							rulegoals.push_back(ee->item);
						else
							rulegoals.push_back(new AtanorPredicateLocalInstruction(globalAtanor, dico, ee->item, ee->isNegation(), ee->disjunction));							
					}
				}

				if (localres == aTRUE) {
					for (j = rulegoals.size() - 1; j >= 0; j--)
						Oo->localgoals.insert(posreplace, rulegoals[j]);
				}
			}

			if (localres == aTRUE) {
				if (trace)
					Displaypredicatestack('e', dom, headpredicate, Oo->localgoals, depth, this);

				//---------------------------------------------------------------
				//THEN We use these new goals and apply PredicateEvaluate to this new list...
				localres = PredicateEvalue(Oo->localgoals, headpredicate, depth + 1);
				//---------------------------------------------------------------

			}

			//We clean the parameters and delete the head...
			headpredicate->Clear();
			delete headpredicate;

			for (j = 0; j < rulegoals.size(); j++) {
				if (rulegoals[j]->isConst())
					continue;
				rulegoals[j]->Clear();
				delete rulegoals[j];
			}

			rulegoals.clear();
			ClearInstances(dom, basedomain);
		}

		refdico->clear();

		///-----------------------OLD PREVIOUS LINES-------------------------------
		if (localres == aCUT)
			return aTRUE;

		if (localres == aBREAK)
			return localres;

		if (localres == aTRUE)
			res = localres;
		
		if (globalAtanor->Error(threadowner))
			return aFALSE;
	}

	return res;
}


//---------------------------------------------------------------------------------------------------------------------

AtanorPredicateRuleItem::AtanorPredicateRuleItem(AtanorGlobal* g, Atanor* parent, bool n, bool d, long i) : AtanorTracked(a_predicateruleelement, g) {
	negation = n;
	disjunction = d;
	position = i;
	item = parent;
}

Atanor* Atanorsynode::Get(Atanor* contextualpattern, Atanor* dom, short idthread) {
	if (contextualpattern->Type() == a_predicateevaluate) {
		if (!contextualpattern->Checkdico(name)) {
			dom->Setfail(true);
			return aNOELEMENT;
		}

		if (affectation)
			return contextualpattern->Getdico(name);

		dom = globalAtanor->Declarator(predicatedico, idthread);

		return contextualpattern->Getdico(name)->Get(contextualpattern, dom, idthread);
	}

	if (dom->isIndex()) {
		if (features == aNULL)
			return aNOELEMENT;

		return features->Get(contextualpattern, dom, idthread);
	}

	return this;
}

Atanor* AtanorBasePredicateVariable::Get(Atanor* contextualpattern, Atanor* dom, short idthread) {
	if (contextualpattern->Type() != a_predicateevaluate) {
		if (!dom->isDeclared(predicatedico)) {
			dom = globalAtanor->Declarator(predicatedico, idthread);
			if (dom == aNULL)
				return aNOELEMENT;
		}

		contextualpattern = dom->Declaration(predicatedico);
	}
	else
		dom = globalAtanor->Declarator(predicatedico, idthread);

	if (contextualpattern == NULL || !contextualpattern->Checkdico(name)) {
		dom->Setfail(true);
		return aNOELEMENT;
	}

	return contextualpattern->Getdico(name)->Get(contextualpattern, dom, idthread);
}

Atanor* AtanorPredicateVariable::Get(Atanor* contextualpattern, Atanor* dom, short idthread) {
	if (contextualpattern->Type() != a_predicateevaluate) {
		if (!dom->isDeclared(predicatedico)) {
			dom = globalAtanor->Declarator(predicatedico, idthread);
			if (dom == aNULL)
				return aNOELEMENT;
		}

		contextualpattern = dom->Declaration(predicatedico);
	}
	else
		dom = globalAtanor->Declarator(predicatedico, idthread);

	if (contextualpattern == NULL || !contextualpattern->Checkdico(name)) {
		dom->Setfail(true);
		return aNOELEMENT;
	}

	Atanor* val = contextualpattern->Getdico(name);

	if (call != NULL) {
		val = val->Get(contextualpattern, dom, idthread);
		return call->Get(contextualpattern, val, idthread);
	}

	if (affectation)
		return val;

	return val->Get(contextualpattern, dom, idthread);
}

Atanor* AtanorPredicateVariableInstance::Get(Atanor* contextualpattern, Atanor* dom, short idthread) {
	Atanor* v = Value((AtanorDeclaration*)dom);
	if (v == aNOELEMENT) {
		dom->Setfail(true);
		return v;
	}
	return v->Getvalues((AtanorDeclaration*)dom, false);
}

Atanor* AtanorPredicateTerm::Get(Atanor* contextualpattern, Atanor* ke, short idthread) {
	if (ke == NULL || ke->isConst())
		return this;
	char ty = 1;
	long left = -1, right = -1;
	ty = ke->Indexes(idthread, parameters.size(), left, right);
	if (ty == 0) {
		if (globalAtanor->erroronkey)
			return globalAtanor->Returnerror("Wrong index", idthread);
		return aNOELEMENT;
	}

	if (ty == 1) {
		if (parameters[left]->Type() == a_predicatevar)
			return globalAtanor->Getdefinition(parameters[left]->Name(), idthread);

		return parameters[left];
	}

	//In this case, we must create a new vector
	Atanorvector* kvect = globalAtanor->Providevector();
	for (long i = left; i < right; i++) {
		if (parameters[i]->Type() == a_predicatevar)
			kvect->Push(globalAtanor->Getdefinition(parameters[i]->Name(), idthread));
		else
			kvect->values.push_back(parameters[i]);
	}
	return kvect;
}


Atanor* AtanorPredicateKnowledgeBase::Get(Atanor* contextualpattern, Atanor* dom, short idthread) {
	//we add it...
	if (add) {
		//In this case, we need to evaluate the parameters
		AtanorPredicate* pv;
		
		if (ptype == a_dependencybase)
			pv = new AtanorDependency(globalAtanor, ((AtanorDependencyKnowledgeBase*)this)->features, name, 0);
		else
			pv = new AtanorPredicate(globalAtanor, name);

		Atanor* e;
		for (long i = 0; i < parameters.size(); i++) {
			e = parameters[i]->EvaluePredicateVariables(contextualpattern, (AtanorDeclaration*)dom);
			pv->parameters.push_back(e);
			e->Setprotect(0);
		}

		if (!globalAtanor->StorePredicate((AtanorDeclaration*)dom, pv, true, idthread))
			pv->Release();

		return aTRUE;
	}

	//or we remove it...
	AtanorPredicate pv(globalAtanor, name);
	Atanor* e;
	for (long i = 0; i < parameters.size(); i++) {
		e = parameters[i]->Get(contextualpattern, dom, idthread);
		e->Setprotect(1);
		pv.parameters.push_back(e);
	}

	if (!globalAtanor->RemovePredicates((AtanorDeclaration*)dom, &pv, idthread)) {
		pv.Clear();
		return aFALSE;
	}
	pv.Clear();
	return aTRUE;
}


Atanor* AtanorInstructionPredicate::Get(Atanor* contextualpattern, Atanor* dom, short idthread) {
	return instructions[0]->Get(contextualpattern, dom, idthread);
}


//We test our value here...
Atanor* AtanorPredicate::Get(Atanor* contextualpattern, Atanor* ke, short idthread) {
	//In this case, we have our variables in contextualpattern

	if (ke == NULL || ke->isConst())
		return this;
	char ty = 1;
	long left = -1, right = -1;

	ty = ke->Indexes(idthread, parameters.size(), left, right);
	if (ty == 0) {
		if (globalAtanor->erroronkey)
			return globalAtanor->Returnerror("Wrong index", idthread);
		return aNOELEMENT;
	}

	if (ty == 1) {
		if (parameters[left]->Type() == a_predicatevar)
			return globalAtanor->Getdefinition(parameters[left]->Name(), idthread);
		return parameters[left];
	}

	//In this case, we must create a new vector
	Atanorvector* kvect = globalAtanor->Providevector();
	for (long i = left; i < right; i++) {
		if (parameters[i]->Type() == a_predicatevar)
			kvect->Push(globalAtanor->Getdefinition(parameters[i]->Name(), idthread));
		else
			kvect->values.push_back(parameters[i]);
	}
	return kvect;
}

Atanor* AtanorPredicateLocalInstruction::Get(Atanor* contextualpattern, Atanor* domain, short idthread) {
	//we need to get the right evaluation for the possible variables hidden in the instruction
	//we have been through a variable renaming, which is stored in dico[localname]=declaration name

	AtanorDeclaration* dom = (AtanorDeclaration*)domain;

	success = false;
	dom->Setfail(false);

	dom->Declare(predicatedico, this);
	Atanor* res = instruction->Get(this, dom, idthread);
	dom->declarations.erase(predicatedico);

	if (dom->Failed() || globalAtanor->Error(idthread))
		return aFALSE;

	if (negation) {
		if (!res->Boolean()) {
			success = true;
			return aTRUE;
		}
		return aFALSE;
	}


	if (res->Boolean())
		success = true;

	return res;
}

string AtanorPredicateLocalInstruction::String() {
	AtanorDeclaration* dom = (AtanorDeclaration*)globalAtanor->Declarator(predicatezone, globalAtanor->GetThreadid());
	if (dom == aNULL)
		return "";

	dom->declarations[predicatedico] = this;
	string v = instruction->String();

	if (v == "") {
		Atanor* ins = instruction;
		v = "[";
		short act = ins->Action();
		if (act == a_affectation) {
			v += ins->Instruction(0)->String();
			v += " is ";
			ins = ins->Instruction(1);
		}

		if (ins->Name()) {
			v += globalAtanor->Getsymbol(ins->Name());
			if (ins->isCall()) {
				v += "(";
				for (long i = 0; i < ins->Size(); i++) {
					if (i)
						v += ",";
					v += ins->Argument(i)->String();
				}
				v += ")";
			}
		}
		else {
			if (act != a_none && act != a_affectation)
				v += globalAtanor->operator_strings[act];
			else
				v += "ins";
		}
		v += "]";
	}
	dom->declarations.erase(predicatedico);
	if (instruction->Function() != NULL)
		v += instruction->Function()->String();
	return v;
}

//-------------------------------------------------------------------------------------------------------
Atanor* AtanorPredicateRule::Get(Atanor*, Atanor*, short) {
	return aTRUE;
}

void AtanorPredicateRule::Reorderdisjunction(int disjposition) {
	vector<Atanor*> ins;
	bool disjfound = false;
	long first = -1;
	for (int i = disjposition; i < instructions.size(); i++) {
		if (instructions[i]->isDisjunction()) {
			ins.push_back(instructions[i]);
			first = i;
			disjfound = true;
		}
		else {
			if (disjfound) {
				ins.push_back(instructions[i]);
				//we reverse it...in order to simplify the disjunction application
				//The last element will become the first and the first the last....
				for (long p = ins.size() - 1; p >= 0; p--) {
					ins[p]->Setdisjunction(true);
					instructions[first++] = ins[p];
				}
				ins[0]->Setdisjunction(false); //of course the new last element should not be disjuncted..
				first = -1;
				ins.clear();
			}
			disjfound = false;
		}
	}
}

//This is the entry point of a predicate analysis...
Atanor* AtanorInstructionEvaluate::Get(Atanor* contextualpattern, Atanor* domcall, short idthread) {
	//head is our definition
	//First we create an actionable AtanorPredicate 
	//we gather all the rules and elements, we might be interested in
	//First the elements from the knowledge base
	//Second the rules from rulebase

	//if we have a vector as a recipient, then we want to explore the whole tree, otherwise, one solution is enough
	bool cut = true;
	if (contextualpattern->isVectorContainer() || head->Function() != NULL)
		cut = false;

	//we use our current element as a way to transfer our stuff...
	AtanorDeclarationPredicate domain;

	domain.declarations = ((AtanorDeclaration*)domcall)->declarations;
	VECTE<Atanor*> goals;
	goals.push_back(head);
	headrule = head;

	domain.Declare(predicatezone, this);
	globalAtanor->Pushstack(&domain, threadowner);

	if (contextualpattern->isContainer())
		fulltraversal = FULLSEARCH;

	dom = &domain; 
	PredicateEvalue(goals, NULL, 0);

	globalAtanor->Popstack(threadowner);
	domain.declarations.erase(predicatezone);

	basebin_hash<Atanor*>::iterator it;

	for (it = domain.declarations.begin(); it != domain.declarations.end(); it++) {
		if (!domcall->isDeclared(it->first)) {
			it->second->Setprotect(0);
			it->second->Resetreference();
		}
	}

	int i;

	if (contextualpattern->isVectorContainer()) {
		Atanor* kvect = Selectavector(contextualpattern);
		for (i = 0; i < results.size(); i++) {
			kvect->Push(results[i]);
			results[i]->Resetreference();
		}
		return kvect;
	}

	if (results.size() == 0)
		return aFALSE;

	if (contextualpattern->isBoolean()) {
		for (i = 0; i < results.size(); i++)
			results[i]->Resetreference();
		return aTRUE;
	}

	if (contextualpattern->isString() && !contextualpattern->isConst()) {
		string v = results[0]->String();
		results[0]->Resetreference();
		return globalAtanor->Providestring(v);
	}

	if (contextualpattern->isNumber()) {
		for (i = 0; i < results.size(); i++)
			results[i]->Resetreference();
		return new Atanorint(results.size());
	}

	if (contextualpattern->isPredicate()) {
		results[0]->Setprotect(1);
		results[0]->Resetreference();
		return results[0];
	}

	for (i = 0; i < results.size(); i++)
		results[i]->Resetreference();

	return aTRUE;
}

Atanor* AtanorInstructionLaunch::Get(Atanor* context, Atanor* val, short idthread) {
	//head is our definition
	//First we create an actionable AtanorPredicate 
	int i;

	Atanor* dom;
	if (val == aAFFECTATION)
		dom = globalAtanor->Topstack(idthread);
	else
		dom = context;

	AtanorPredicateFunction pv(globalAtanor, globalAtanor->predicates[head->name]->Function(), head->name);
	if (pv.function != NULL)
		pv.object = globalAtanor->predicates[head->name]->Returnobject();

	AtanorInstructionEvaluate kl(globalAtanor, &pv);
	kl.threadowner = idthread;

	//Important variables...
	SetPredicateVariableFlags();

	AtanorDeclarationPredicate domain;

	basebin_hash<AtanorPredicateVariableInstance*> localdico;
	kl.dico = &localdico;
	AtanorPredicateContainer* kpcont = globalAtanor->GetPredicatecontainer(idthread);

	if (kpcont != NULL)
		kl.rules = kpcont->rules;

	if (globalAtanor->predicateTrace(head->name))
		kl.trace = true;

	Atanor* e;
	for (i = 0; i < head->parameters.size(); i++) {
		e = head->parameters[i]->ExtractPredicateVariables(&kl, &domain, NULL, NULL, idthread, true);
		e->Setreference();
		pv.parameters.push_back(e);
	}

	e = kl.Get(context, &domain, idthread);

	for (i = 0; i < pv.Size(); i++)
		pv.parameters[i]->Resetreference();


	basebin_hash<Atanor*>::iterator it;
	for (it = domain.declarations.begin(); it != domain.declarations.end(); it++) {
		it->second->Setprotect(0);
		it->second->Resetreference();
	}

	return e;
}





