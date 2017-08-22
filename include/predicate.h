/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2003 - 2010 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
 * This file can only be used with the ATHANOR library,
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : ATHANOR
   Version    : ATANOR 0.26
   filename   : kif.h
   Date       : 2017/04/01
   Purpose    : Predicate Implementation
   Programmer : Claude ROUX (claude.roux@xrce.xerox.com)
   Reviewer   :
   */


#ifndef i_predicate
#define i_predicate

#include "atanor.h"
#include "atanormap.h"
#include "atanormapss.h"
#include "atanorvector.h"
#include "atanorsvector.h"
#include "instructions.h"

class AtanorPredicateRule;

typedef enum { SEARCHONE, FULLSEARCH, STOPSEARCH } predicatesearch;

class AtanorDeclarationPredicate : public AtanorDeclaration {
public:
	bool fail;

	AtanorDeclarationPredicate() : fail(false) {}

	void Setfail(bool v) {
		fail = v;
	}

	bool Failed() {
		return fail;
	}

};

class AtanorBasisPredicate : public Atanor {
public:
	short reference;
	bool protect;


	AtanorBasisPredicate(AtanorGlobal* g, Atanor* parent = NULL)  {
		protect = true;
		reference = 0;
		if (g != NULL)
			g->RecordInTrackerProtected(this);
		if (parent != NULL)
			parent->AddInstruction(this);
	}

	AtanorBasisPredicate()  {
		protect = true;
		reference = 0;
	}

	long Reference() {
		return reference;
	}

	virtual void Setprotect(bool n) {
		protect = n;
	}

	virtual void Popping() {
		protect = false;
		if (reference <= 0)
			protect = true;
	}

	bool isProtected() {
		return protect;
	}

	virtual void Setreference(short r = 1) {
		reference += r;
		protect = false;
	}

	virtual void Resetreferencenoprotect(short r = 1) {
		protect = 0;
		Resetreference(r);
	}


	virtual void Resetreference(short r = 1) {
		reference -= r;
		if (reference <= 0) {
			if (!protect) {
				if (idtracker != -1)
					globalAtanor->RemoveFromTracker(idtracker);
				delete this;
			}
		}
	}

	virtual void Release() {
		if (reference == 0) {
			protect = false;
			Resetreference(0);
		}
	}

	virtual void Protect() {
		//We suppose there have been a Setreference before...
		if (reference >= 2) { //No problem, we simply remove one increment
			Resetreference();
			Setprotect(false); //Should not be any protect left in that case...
			return;
		}

		//Else, we decrease our reference, but we protect it with a protect
		Setprotect(true);
		Resetreference();
		Popping(); //and protection only if necessary...
	}

	bool isObject() {
		return true;
	}

};


//A PredicateVariable such as: ?val
class AtanorBasePredicateVariable : public AtanorBasisPredicate {
public:

	short name;

	AtanorBasePredicateVariable(AtanorGlobal* g, short n, Atanor* parent = NULL) : AtanorBasisPredicate(g, parent) {
		name = n;
	}
	
	AtanorBasePredicateVariable(short n) {
		name = n;
	}

	bool isPredicateVariable() {
		return true;
	}

	bool isAssignable() {
		return true;
	}

	bool duplicateForCall() {
		return false;
	}

	virtual short Type() {
		return a_predicatevar;
	}

	void Setname(short n) {
		name = n;
	}

	short Name() { return name; }

	virtual string String();

	virtual bool Unify(AtanorDeclaration* dom, Atanor* a) {
		return false;
	}

	virtual bool isUnified(AtanorDeclaration* dom);


	virtual Atanor* Newinstance(short idthread, Atanor* f = NULL) {
		return new AtanorBasePredicateVariable(globalAtanor, name);
	}

	virtual Atanor* same(Atanor* a) {
		return aTRUE;
	}

	virtual Atanor* Get(Atanor* context, Atanor* value, short idthread);

	virtual Atanor* Put(Atanor* idx, Atanor* value, short idthread);
	virtual Atanor* ExtractPredicateVariables(Atanor* context, AtanorDeclaration* dom, Atanor* val, Atanor* e, short, bool root);
	virtual Atanor* EvaluePredicateVariables(Atanor* context, AtanorDeclaration* dom);
};

class AtanorPredicateVariable : public AtanorBasePredicateVariable {
public:
	Atanor* call;
	bool affectation;

	AtanorPredicateVariable(AtanorGlobal* g, short n, Atanor* parent = NULL) : AtanorBasePredicateVariable(g, n, parent) {
		affectation = false;
		call = NULL;
	}

	void AddInstruction(Atanor* a) {
		call = a;
	}

	void Setaffectation(bool v) {
		affectation = v;
	}

	bool isAffectation() {
		return affectation;
	}

	Atanor* Get(Atanor* context, Atanor* value, short idthread);

	Atanor* Function() {
		return call;
	}

};

class AtanorPredicateVariableInstance : public AtanorBasePredicateVariable {
public:

	Atanor* value;
	short labelname;
	bool merge;

	AtanorPredicateVariableInstance(short n, short ln) : labelname(ln), value(aNOELEMENT), merge(false), AtanorBasePredicateVariable(n) {}

	bool isToMerge() {
		return merge;
	}

	short Type() {
		return a_instance;
	}

	string String();

	bool Unify(AtanorDeclaration* dom, Atanor* a);
	bool isUnified(AtanorDeclaration* dom);

	Atanor* VariableValue() {
		return value;
	}

	Atanor* Variable(AtanorDeclaration* dom);


	Atanor* Newinstance(short idthread, Atanor* f = NULL) {
		return new AtanorPredicateVariableInstance(name, labelname);
	}

	bool Insertvalue(Atanor* dom, Atanor* v, basebin_hash<Atanor*>&);

	void Setreference(short inc = 1) {
		if (value != aNOELEMENT)
			value->Setreference(inc);
		reference += inc;
		protect = false;
	}
	void Resetreference(short inc = 1);
	void Resetreferencenoprotect(short r = 1);

	void affiche();

	void Cleans(Atanor* v, bool localvalue) {
		if (localvalue) {
			value->Resetreference(reference);
			value = v;
		}
		else
			v->Resetreference(reference);
	}

	Atanor* Value(AtanorDeclaration* dom);

	Atanor* Value() {
		return value;
	}

	Atanor* same(Atanor* a);

	Atanor* Get(Atanor* context, Atanor* value, short idthread);

	Atanor* Put(Atanor* idx, Atanor* value, short idthread);

	bool Setvalue(Atanor* index, Atanor* val, short idthread, bool initial) {
		if (!initial)
			value->Resetreference(reference);
		value = val;
		return true;
	}

	Atanor* ExtractPredicateVariables(Atanor* context, AtanorDeclaration* dom, Atanor* val, Atanor* e, short, bool root);
	Atanor* EvaluePredicateVariables(Atanor* context, AtanorDeclaration* dom);
	Atanor* Getvalues(AtanorDeclaration* dom, bool duplicate);

	Atanor* Newvalue(Atanor* a, short idthread) {
		value->Resetreference(reference);
		value = a->Atom();
		value->Setreference(reference);
		return value;
	}

	Atanor* plusplus() {
		return value->plusplus();
	}

	Atanor* minusminus() {
		return value->plusplus();
	}

	Atanor* plus(Atanor* a, bool itself) {
		return value->plus(a, itself);
	}

	Atanor* minus(Atanor* a, bool itself) {
		return value->minus(a, itself);
	}

	Atanor* multiply(Atanor* a, bool itself) {
		return value->multiply(a, itself);
	}

	Atanor* divide(Atanor* a, bool itself) {
		return value->divide(a, itself);
	}

	Atanor* power(Atanor* a, bool itself) {
		return value->power(a, itself);
	}

	Atanor* shiftleft(Atanor* a, bool itself) {
		return value->shiftleft(a, itself);
	}

	Atanor* shiftright(Atanor* a, bool itself) {
		return value->shiftright(a, itself);
	}

	Atanor* mod(Atanor* a, bool itself) {
		return value->mod(a, itself);
	}

	Atanor* less(Atanor* a) {
		return value->less(a);
	}

	Atanor* more(Atanor* a) {
		return value->more(a);
	}

	Atanor* lessequal(Atanor* a) {
		return value->lessequal(a);
	}

	Atanor* moreequal(Atanor* a) {
		return value->moreequal(a);
	}

};

class AtanorPredicateVariableInstanceForCleaning : public AtanorBasisPredicate {
public:
	AtanorPredicateVariableInstance* variable;

	AtanorPredicateVariableInstanceForCleaning(AtanorPredicateVariableInstance* v) {
		variable = v;
	}

	void Resetreference(short inc = 1) {
		variable->value->Resetreference(variable->reference);
		variable->value = aNOELEMENT;
		AtanorBasisPredicate::Resetreference(inc);
	}
};

class AtanorPredicateTerm : public AtanorBasePredicateVariable {
public:

	vector<Atanor*> parameters;

	AtanorPredicateTerm(AtanorGlobal* g, short n, Atanor* parent = NULL) : AtanorBasePredicateVariable(g, n, parent) {}

	bool Insertvalue(Atanor* dom, Atanor* v, basebin_hash<Atanor*>&);

	virtual Atanor* Newinstance(short idthread, Atanor* f = NULL) {
		return new AtanorPredicateTerm(globalAtanor, name);
	}

	short Type() {
		return a_term;
	}

	void Set(short n) {
		name = n;
		globalAtanor->RecordInTrackerProtected(this);
	}

	Atanor* Value() {
		return this;
	}

	void Leaves(Atanor* v) {
		for (size_t i = 0; i < parameters.size(); i++)
			parameters[i]->Leaves(v);
	}

	Atanor* ExtractPredicateVariables(Atanor* context, AtanorDeclaration* dom, Atanor* val, Atanor* e, short, bool root);
	Atanor* Get(Atanor* context, Atanor* value, short idthread);
	Atanor* EvaluePredicateVariables(Atanor* context, AtanorDeclaration* dom);

	Atanor* Put(Atanor* idx, Atanor* value, short idthread);

	bool Unify(AtanorDeclaration* dom, Atanor* a);
	bool isUnified(AtanorDeclaration* dom);

	Atanor* Vector(short idthread);
	Atanor* Map(short idthread);

	Atanor* Parameter(size_t i) {
		return parameters[i];
	}

	virtual void Release() {
		if (reference == 0) {
			protect = false;
			for (size_t i = 0; i < parameters.size(); i++)
				parameters[i]->Release();
			AtanorBasisPredicate::Resetreference();
		}
	}

	void Setreference(short inc = 1);
	void Resetreference(short inc = 1);
	void Setprotect(bool);
	void Popping();


	virtual Atanor* Getvalues(AtanorDeclaration* dom, bool duplicate);

	virtual void AddInstruction(Atanor* e) {
		parameters.push_back(e);
	}

	string String();

	BLONG Long() {
		return parameters.size();
	}

	long Integer() {
		return (long)parameters.size();
	}
	double Float() {
		return (double)parameters.size();
	}

	Atanor* same(Atanor* a);

	long Size() {
		return parameters.size();
	}

};

class AtanorPredicate;
//This typedef defines a type "predicateMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (AtanorPredicate::*predicateMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//The instance. It can contain predicate variables in its parameters: ?val
class AtanorPredicate : public AtanorBasisPredicate {
public:


	short ptype;
	vector<Atanor*> parameters;

	short name;

	bool negation;
	bool disjunction;

#ifdef _DEBUG
	string nom;
#endif

	static Exchanging hmap<unsigned short, predicateMethod> methods;
	static Exchanging hmap<string, string> infomethods;
	static Exchanging bin_hash<unsigned long> exported;
	static Exchanging short idtype;

	AtanorPredicate(short n, AtanorGlobal* g = NULL, short t = a_predicate, Atanor* parent = NULL);
	AtanorPredicate(AtanorGlobal* g, short n);

	void Leaves(Atanor* v) {
		for (size_t i = 0; i < parameters.size(); i++)
			parameters[i]->Leaves(v);
	}

	virtual bool Setinvariable(Atanor* v, AtanorDeclaration* dom, short idthread) {
		return false;
	}

	virtual void Resetintvariable(AtanorDeclaration* dom, short idthread) {}

	bool duplicateForCall() {
		return false;
	}

	void Setnegation(bool v) {
		negation = v;
	}

	short Name() { return name; }

	~AtanorPredicate() {
		if (idtracker != -1 && globalAtanor->Gettracked(idtracker) == this)
			globalAtanor->RemoveFromTracker(idtracker);
	}

	virtual bool Checkparameters(AtanorDeclaration*) {
		return true;
	}

	virtual long Idrule() {
		return -1;
	}

	virtual void Setidrule(long i) {}

	virtual Atanor* Features() {
		return aNULL;
	}

	virtual void Setfeatures(Atanor* feats) {}


	virtual short Idvar() {
		return 0;
	}

	virtual void Setname(short n) {
		name = n;
	}

	virtual short kbaction() {
		return 0;
	}

	virtual bool isDependency() {
		return false;
	}

	virtual short Type() {
		return ptype;
	}

	void Setdisjunction(bool v) {
		disjunction = v;
	}

	void set(bool n, bool d) {
		negation = n;
		disjunction = d;
	}

	bool isNegation() {
		return negation;
	}

	bool isDisjunction() {
		return disjunction;
	}

	bool isPredicate() {
		return true;
	}

	bool Unify(AtanorDeclaration* dom, Atanor* a);
	bool isUnified(AtanorDeclaration* dom);

	Atanor* Vector(short idthread);
	Atanor* Map(short idthread);


	virtual AtanorPredicate* Duplicate(Atanor* context, AtanorDeclaration* d);
	AtanorPredicate* Copyfrom(Atanor* context, AtanorDeclaration* d, AtanorPredicate* h, short);

	Atanor* Parameter(size_t i) {
		return parameters[i];
	}

	Atanor* Vector(Atanor*, Atanorvector* itself = NULL);
	Atanor* Map(Atanor*, Atanormap* itself = NULL);


	virtual void Setreference(short inc = 1);
	virtual void Resetreference(short inc = 1);
	virtual void Setprotect(bool);
	void Popping();

	void Clear() {
		for (long i = 0; i < parameters.size(); i++) {
			if (!parameters[i]->isInstruction()) {
				parameters[i]->Setprotect(0);
				parameters[i]->Resetreference();
			}
		}
		parameters.clear();
	}

	virtual void AddInstruction(Atanor* e) {
		parameters.push_back(e);
	}

	bool Basic() {
		return true;
	}

	bool Simple() {
		return true;
	}

	bool Statique() {
		return true;
	}

	Atanor* Get(Atanor* context, Atanor* callfunction, short idthread);
	virtual Atanor* Put(Atanor* context, Atanor* callfunction, short idthread);

	virtual Atanor* Newinstance(short idthread, Atanor* f = NULL);

	virtual string String();
	virtual string JSonString();

	bool Boolean();

	Atanor* same(Atanor* a);

	long Size() {
		return parameters.size();
	}

	//--------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	bool isDeclared(short n) {
		if (exported.find(n) != exported.end())
			return true;
		return false;
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
	//---------------------------------------------------------------------------------------------------------------------
	Atanor* MethodRuleid(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodFeatures(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodLeaves(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		Atanor* v = Selectavector(contextualpattern);
		Leaves(v);
		return v;
	}
	Atanor* MethodName(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Providestring(globalAtanor->Getsymbol(name));
	}
	Atanor* MethodInitial(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodTrace(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	static void AddMethod(AtanorGlobal* g, string name, predicateMethod func, unsigned long arity, string infos);
	static bool InitialisationModule(AtanorGlobal* g, string version);
	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
	//is returned by the Declaration method.
	Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		return (this->*methods[idname])(contextualpattern, idthread, callfunc);
	}
};
class AtanorPredicateFunction : public AtanorPredicate {
public:
	Atanor* function;
	Atanor* object;
	bool trace;
	AtanorPredicateFunction(AtanorGlobal* g, Atanor* f, short n) : AtanorPredicate(n, g, a_predicate) {
		function = f;
		object = aNULL;
		trace = false;
	}
	Atanor* Function() {
		return function;
	}
	string String() {
		return globalAtanor->Getsymbol(name);
	}
	Atanor* Newinstance(short idthread, Atanor* f = NULL) {
		return new AtanorPredicateFunction(globalAtanor, function, name);
	}
	void Resetreference(short inc = 1);

	Atanor* Returnobject() {
		return object;
	}
	void Setname(short n);
};

class AtanorConstPredicate : public AtanorPredicate {
public:
	AtanorConstPredicate(AtanorGlobal* g, short n) : AtanorPredicate(n, g, a_predicate) {}
	string String() {
		return globalAtanor->Getsymbol(name);
	}
	Atanor* Newinstance(short idthread, Atanor* f = NULL) {
		return this;
	}
	void Resetreference(short inc = 1) {}
	bool isConst() {
		return true;
	}
};

class AtanorDependency : public AtanorPredicate {
public:
	Atanor* features;
	long idrule;
	short idvar;	
	char chosen;


	AtanorDependency(AtanorGlobal* g, Atanor* f, short n, short id);

	short Idvar();

	long Idrule() {
		return idrule;
	}

	void Setidrule(long i) {
		idrule = i;
	}

	bool Setinvariable(Atanor* v, AtanorDeclaration* dom, short idthread);

	void Resetintvariable(AtanorDeclaration* dom, short idthread);

	AtanorPredicate* Duplicate(Atanor* context, AtanorDeclaration* d);
	bool Unify(AtanorDeclaration* dom, Atanor* a);

	Atanor* Newinstance(short idthread, Atanor* f = NULL) {
		return new AtanorDependency(NULL, aNULL, name, idvar);
	}

	void AddInstruction(Atanor* e) {
		if (chosen == 2)
			features = e;
		else
			parameters.push_back(e);
	}

	bool isChosen() {
		return chosen;
	}

	bool isDependency() {
		return true;
	}

	void Setreference(short inc = 1) {
		features->Setreference(inc);
		AtanorPredicate::Setreference(inc);
	}

	void Resetreference(short inc = 1) {
		features->Resetreference(inc);
		AtanorPredicate::Resetreference(inc);
	}

	void Setprotect(bool v) {
		features->Setprotect(v);
		AtanorPredicate::Setprotect(v);
	}

	string String();
	string JSonString();

	Atanor* Vector(short idthread) {
		Atanorvector* v = new Atanorvector;
		v->Push(new Atanorstring(globalAtanor->Getsymbol(name)));
		v->Push(features);
		for (long i = 0; i < parameters.size(); i++) {
			v->Push(parameters[i]->Map(idthread));
		}
		return v;
	}

	Atanor* Features() {
		return features;
	}

	void Setfeatures(Atanor* feats);
};

class AtanorPredicateVar : public AtanorPredicate {
public:


	AtanorPredicateVar(AtanorGlobal* g, short n) : AtanorPredicate(n, g, a_predicate) {}

	Atanor* Put(Atanor* dom, Atanor* val, short idthread);
	Atanor* Newinstance(short idthread, Atanor* f = NULL) {
		return new AtanorPredicateVar(globalAtanor, name);
	}

};

class AtanorPredicateKnowledgeBase : public AtanorPredicate {
public:
	bool add;

	AtanorPredicateKnowledgeBase(AtanorGlobal* g, short n, Atanor* parent = NULL, short t = a_knowledgebase) : AtanorPredicate(n, g, t, parent) {
		add = false;
	}

	Atanor* Get(Atanor* context, Atanor* callfunction, short idthread);

	Atanor* Newinstance(short idthread, Atanor* f = NULL) {
		return new AtanorPredicateKnowledgeBase(globalAtanor, name);
	}
};

class AtanorDependencyKnowledgeBase : public AtanorPredicateKnowledgeBase {
public:
	Atanor* features;

	AtanorDependencyKnowledgeBase(AtanorGlobal* g, short n, Atanor* parent = NULL) : AtanorPredicateKnowledgeBase(g, n, parent, a_dependencybase) {
		features = aNULL;
	}

	Atanor* Newinstance(short idthread, Atanor* f = NULL) {
		return new AtanorDependencyKnowledgeBase(globalAtanor, name);
	}

	void AddInstruction(Atanor* e) {
		if (add)
			features = e;
		else
			parameters.push_back(e);
	}
};

class AtanorPredicateKnowledgeBaseFunction : public AtanorPredicate {
public:
	short action;

	AtanorPredicateKnowledgeBaseFunction(AtanorGlobal* g, short n, Atanor* parent = NULL) : AtanorPredicate(n, g, a_predicate, parent) {
		action = n;
	}

	short kbaction() {
		return action;
	}

	string String();

	void AddInstruction(Atanor* e) {
		AtanorPredicate* p = (AtanorPredicate*)e;
		name = p->name;
		parameters = p->parameters;
		negation = p->negation;
		p->parameters.clear();
	}

	AtanorPredicate* Duplicate(Atanor* context, AtanorDeclaration* d);

	virtual Atanor* Newinstance(short idthread, Atanor* f = NULL) {
		AtanorPredicateKnowledgeBaseFunction* p = new AtanorPredicateKnowledgeBaseFunction(globalAtanor, action);
		p->name = name;
		p->negation = negation;
		return p;
	}

};

class AtanorDependencyKnowledgeBaseFunction : public AtanorPredicateKnowledgeBaseFunction {
public:
	Atanor* features;
	short idvar;
	long idrule;

	AtanorDependencyKnowledgeBaseFunction(AtanorGlobal* g, short n, long idr, Atanor* parent = NULL) : idrule(idr), AtanorPredicateKnowledgeBaseFunction(g, n, parent) {
		features = aNULL;
		idvar = 0;
	}

	short Idvar();

	long Idrule() {
		return idrule;
	}

	bool Setinvariable(Atanor* v, AtanorDeclaration* dom, short idthread);

	void Resetintvariable(AtanorDeclaration* dom, short idthread);

	bool isDependency() {
		return true;
	}

	Atanor* Features() {
		return features;
	}

	void AddInstruction(Atanor* e) {
		AtanorDependency* p = (AtanorDependency*)e;
		name = p->name;
		parameters = p->parameters;
		negation = p->negation;
		features = p->features;
		idvar = p->idvar;
	}

	Atanor* Newinstance(short idthread, Atanor* f = NULL) {
		AtanorDependencyKnowledgeBaseFunction* p = new AtanorDependencyKnowledgeBaseFunction(globalAtanor, action, idrule);
		p->name = name;
		p->features = features;
		p->negation = negation;
		p->idvar = idvar;
		return p;
	}

	void Setreference(short inc = 1) {
		features->Setreference(inc);
		AtanorPredicate::Setreference(inc);
	}

	Atanor* same(Atanor* a);
};

//this class is used to record an entry within the knowledge base such as: toto(x,y) :- true; 
//with toto(x,y):- false; the element is removed.
class AtanorInstructionPredicate : public AtanorSequence {
public:
	short name;
	AtanorInstructionPredicate(short n, AtanorGlobal* g, Atanor* parent = NULL) : AtanorSequence(g, parent) {
		name = n;
	}

	Atanor* Get(Atanor*, Atanor*, short);
	short Type() {
		return a_predicateinstruction;
	}
};

//This is a most important class, as it contains the element which is used as a trigger to force the analyse of rules...
class AtanorInstructionEvaluate : public AtanorBasisPredicate {
public:
	hmap<unsigned short, vector<AtanorPredicateRule*> > rules;
	basebin_hash<AtanorPredicateVariableInstance*>* dico;	
	VECTE<AtanorPredicate*> results;

	AtanorPredicate* head;
	AtanorPredicate* headrule;
	AtanorDeclarationPredicate* dom;
	Atanor* value;
	AtanorCallFunction* callfunc;

	short threadowner;

	bool count;
	bool trace;
	predicatesearch fulltraversal;


	AtanorInstructionEvaluate(AtanorGlobal* g, AtanorPredicate* h = NULL) : results(10), AtanorBasisPredicate(g) {
		fulltraversal = SEARCHONE;
		callfunc = NULL;
		value = aNULL;
		trace = false;
		count = false;
		headrule = NULL;
		head = h;
		dom = NULL;
	}

	~AtanorInstructionEvaluate() {
		if (idtracker != -1 && globalAtanor->Gettracked(idtracker) == this)
			globalAtanor->RemoveFromTracker(idtracker);
	}

	short Type() {
		return a_predicateevaluate;
	}

	basebin_hash<AtanorPredicateVariableInstance*>* Dico() {
		return dico;
	}

	void Setdico(short n, AtanorPredicateVariableInstance* a) {
		(*dico)[n] = a;
	}

	AtanorPredicateVariableInstance* Getdico(short n) {
		return (*dico)[n];
	}

	bool Checkdico(short n) {
		return dico->check(n);
	}

	Atanor* Get(Atanor* context, Atanor* value, short idthread);
	Atanor* PredicateEvalue(VECTE<Atanor*>& goals, AtanorPredicate* currenthead, int depth);
	AtanorPredicate* PredicateUnification(VECTE<Atanor*>& goals, int& posreplace, int& from);
	Atanor* PredicateCreate(AtanorPredicate* headrule, int depth);

};


class AtanorInstructionLaunch : public AtanorTracked{
public:
	AtanorPredicate* head;

	AtanorInstructionLaunch(AtanorGlobal* g, Atanor* parent = NULL) : AtanorTracked(a_predicatelaunch, g, parent) {}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);

	short Type() {
		return a_predicatelaunch;
	}

	void AddInstruction(Atanor* e) {
		head = (AtanorPredicate*)e;
	}
};

//definition of a rule element, which can be anything from another predicate to a function call...
//contextualpattern is ALWAYS a AtanorInstructionEvaluate
class AtanorPredicateRuleElement : public AtanorTracked {
public:
	vector<Atanor*> instructions;
	bool negation;
	bool disjunction;

	AtanorPredicateRuleElement(AtanorGlobal* g, Atanor* parent = NULL) : disjunction(false), negation(false), AtanorTracked(a_predicateruleelement, g, parent) {}


	short Type() {
		return a_predicateruleelement;
	}

	short Action() {
		return a_predicateelement;
	}

	void AddInstruction(Atanor* ke) {
		instructions.push_back(ke);
	}

	void Setnegation(bool n) {
		negation = n;
	}

	void Setdisjunction(bool v) {
		disjunction = v;
	}

	void Addtail(AtanorPredicateRule*, int& disj);
};


class AtanorPredicateContainer : public AtanorBasisPredicate {
public:
	hmap<unsigned short, vector<AtanorPredicateRule*> > rules;


	AtanorPredicateContainer(AtanorGlobal* g) : AtanorBasisPredicate(g) {}

	bool duplicateForCall() {
		return false;
	}

	short Type() {
		return a_predicatecontainer;
	}

};


//Definition of a predicate rule
class AtanorPredicateRule : public AtanorTracked {
public:
	vector<Atanor*> instructions;
	AtanorPredicate* head;
	short name;
	char disjunction;

	short Name() {
		return name;
	}

	AtanorPredicateRule(short n, AtanorGlobal* g = NULL, Atanor* parent = NULL) : AtanorTracked(a_predicaterule, g, parent) {
		name = n;
		disjunction = 0;
		head = NULL;
	}

	~AtanorPredicateRule() {
		head = NULL;
	}

	short Type() {
		return a_predicaterule;
	}

	void Reorderdisjunction(int);
	void Addtail(AtanorPredicateContainer* kcont, AtanorPredicateRuleElement* elements) {
		if (elements != NULL) {
			int disj = -1;
			kcont->rules[name].push_back(this);
			elements->Addtail(this, disj);
			if (disj != -1)
				Reorderdisjunction(disj);
		}
	}
	Atanor* Get(Atanor* c, Atanor* v, short idthread);
	void addfinal(AtanorPredicateContainer* kpc);
};

class AtanorPredicateRuleItem : public AtanorTracked {
public:
	Atanor* item;
	long position;
	bool negation;
	bool disjunction;

	AtanorPredicateRuleItem(AtanorGlobal* g, Atanor* parent, bool n, bool d, long i);

	short Type() {
		return a_predicateruleelement;
	}

	void Setdisjunction(bool v) {
		item->Setdisjunction(v);
		disjunction = v;
	}

	bool isNegation() {
		return negation;
	}

	bool isDisjunction() {
		return disjunction;
	}

};

class AtanorPredicateLocalInstruction : public AtanorBasisPredicate {
public:


	basebin_hash<AtanorPredicateVariableInstance*>* dico;
	Atanor* instruction;
	bool negation;
	bool success;
	bool disjunction;


	AtanorPredicateLocalInstruction(AtanorGlobal* g, basebin_hash<AtanorPredicateVariableInstance*>* context, Atanor* e, bool n, bool d)  {

		dico = context;
		negation = n;
		disjunction = d;
		instruction = e;
		success = false;
	}


	short Type() {
		return a_predicateevaluate;
	}

	basebin_hash<AtanorPredicateVariableInstance*>* Dico() {
		return dico;
	}

	void Setdico(short n, AtanorPredicateVariableInstance* a) {
		(*dico)[n] = a;
	}

	AtanorPredicateVariableInstance* Getdico(short n) {
		return (*dico)[n];
	}

	bool Checkdico(short n) {
		return dico->check(n);
	}

	void Clean() {}

	void Setdisjunction(bool v) {
		disjunction = v;
	}


	bool hasbeenSuccesfull() {
		return success;
	}

	void setSucessfull(bool v) {
		success = v;
	}

	virtual Atanor* Get(Atanor* context, Atanor* value, short idthread);

	short Action() {
		return instruction->Action();
	}

	short Name() {
		return instruction->Name();
	}

	string String();
};


#endif





