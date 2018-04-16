
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
filename   : predicatefunctions.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "predicate.h"
#include "predicatefunctions.h"
#include "atanorsynode.h"

//----------------------------------------------------------------------------------------------------

Exporting bool AtanorPredicateBetween::Checkparameters(AtanorDeclaration* dom) {
	if (!parameters[0]->isUnified(dom) || !parameters[1]->isUnified(dom))
		return false;
	return true;
}

Exporting Atanor* AtanorPredicateBetween::PredicateEvalue(AtanorInstructionEvaluate* context, VECTE<Atanor*>& stack, AtanorPredicate* currenthead, int depth) {
	//if we are here, it means that one of the variables has not yet been assigned to...
	//We accept only two cases, either the third variable is unified or is not...
	
	long left = parameters[0]->Getinteger(context->dom, aNULL, context->threadowner);
	long right = parameters[1]->Getinteger(context->dom, aNULL, context->threadowner);

	Atanor* v = parameters[2];
	if (v->isUnified(context->dom)) {
		long val = v->Get(context, context->dom, context->threadowner)->Integer();
		if (val >= left && val <= right)
			return context->PredicateEvalue(stack, currenthead, depth);
		return aFALSE;
	}


	Atanorint* val = globalAtanor->Provideint(0);
	v->Newvalue(val, context->threadowner);
	Atanor* res = aFALSE;
	Atanor* e;
	for (long i = left; i <= right; i++) {
		val->value = i;
		e = context->PredicateEvalue(stack, currenthead, depth);
		if (e == aTRUE) {
			res = e;
			continue;
		}

		if (e == aFALSE)
			continue;

		if (e == aCUT) {
			res = aTRUE;
			break;
		}

		if (e == aBREAK)
			break;
	}

	v->Setvalue(aNULL, aNOELEMENT, context->threadowner, false);
	return res;
}

Exporting AtanorPredicate* AtanorPredicateBetween::Duplicate(Atanor* context, AtanorDeclaration* dom) {
	AtanorPredicateBetween* p = new AtanorPredicateBetween(globalAtanor, name);
	short idthread = ((AtanorInstructionEvaluate*)context)->threadowner;
	Atanor* e;

	for (short i = 0; i < 3; i++) {
		e = parameters[i]->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, true);
		if (e == NULL) {
			p->Release();
			return NULL;
		}
		p->parameters.push_back(e);
	}

	p->Setreference();
	return p;
}

//----------------------------------------------------------------------------------------------------
Exporting Atanor* AtanorPredicateTermSucc::Getvalues(AtanorDeclaration* dom, bool duplicate) {
	Atanor* e = parameters[0]->Getvalues(dom, duplicate);
	if (e == aNOELEMENT)
		return aNOELEMENT;
	return e->Succ();
}

Exporting bool AtanorPredicateSucc::Checkparameters(AtanorDeclaration* dom) {
	if (!parameters[0]->isUnified(dom) && !parameters[1]->isUnified(dom))
		return false;
	return true;
}

Exporting Atanor* AtanorPredicateSucc::PredicateEvalue(AtanorInstructionEvaluate* context, VECTE<Atanor*>& stack, AtanorPredicate* currenthead, int depth) {
	Atanor* v;
	if (parameters[0]->isUnified(context->dom)) {
		Atanor* v1 = parameters[0]->Get(context, aNULL, context->threadowner);

		if (parameters[1]->isUnified(context->dom)) {
			v = parameters[1]->Get(context, context->dom, context->threadowner);
			if (v == aUNIVERSAL)
				return aFALSE;

			Atanor* v2 = v->Succ();
			v->Release();
			if (v1->same(v2)) {
				v2->Release();
				return context->PredicateEvalue(stack, currenthead, depth);
			}
			v2->Release();
			return aFALSE;
		}

		v = v1->Succ();
		parameters[1]->Newvalue(v, context->threadowner);
		v = context->PredicateEvalue(stack, currenthead, depth);
		parameters[1]->Setvalue(aNULL, aNOELEMENT, context->threadowner, false);
		return v;
	}

	if (parameters[1]->isUnified(context->dom)) {
		v = parameters[1]->Get(context, context->dom, context->threadowner);
		if (v == aUNIVERSAL)
			return aFALSE;

		Atanor* v2 = v->Pred();
		v->Release();
		parameters[0]->Newvalue(v2, context->threadowner);
		v = context->PredicateEvalue(stack, currenthead, depth);
		parameters[0]->Setvalue(aNULL, aNOELEMENT, context->threadowner, false);
		return v2;
	}

	return aFALSE;
}


Exporting AtanorPredicate* AtanorPredicateSucc::Duplicate(Atanor* context, AtanorDeclaration* dom) {
	if (parameters.size() != 2)
		return NULL;

	AtanorPredicateSucc* p = new AtanorPredicateSucc(globalAtanor, name);
	short idthread = ((AtanorInstructionEvaluate*)context)->threadowner;
	Atanor* e;

	for (short i = 0; i < 2; i++) {
		e = parameters[i]->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, true);
		if (e == NULL) {
			p->Release();
			return NULL;
		}
		p->parameters.push_back(e);
	}

	p->Setreference();
	return p;
}

//----------------------------------------------------------------------------------------------------
Exporting Atanor* AtanorPredicateTermPred::Getvalues(AtanorDeclaration* dom, bool duplicate) {
	Atanor* e = parameters[0]->Getvalues(dom, duplicate);
	if (e == aNOELEMENT)
		return aNOELEMENT;
	return e->Pred();
}

Exporting bool AtanorPredicatePred::Checkparameters(AtanorDeclaration* dom) {
	if (!parameters[0]->isUnified(dom) && !parameters[1]->isUnified(dom))
		return false;
	return true;
}

Exporting Atanor* AtanorPredicatePred::PredicateEvalue(AtanorInstructionEvaluate* context, VECTE<Atanor*>& stack, AtanorPredicate* currenthead, int depth) {
	Atanor* v;
	if (parameters[0]->isUnified(context->dom)) {
		Atanor* v1 = parameters[0]->Get(context, aNULL, context->threadowner);

		if (parameters[1]->isUnified(context->dom)) {
			v = parameters[1]->Get(context, context->dom, context->threadowner);
			if (v == aUNIVERSAL)
				return aFALSE;

			Atanor* v2 = v->Pred();
			v->Release();
			if (v1->same(v2)) {
				v2->Release();
				return context->PredicateEvalue(stack, currenthead, depth);
			}
			v2->Release();
			return aFALSE;
		}

		v = v1->Pred();
		parameters[1]->Newvalue(v, context->threadowner);
		v = context->PredicateEvalue(stack, currenthead, depth);
		parameters[1]->Setvalue(aNULL, aNOELEMENT, context->threadowner, false);
		return v;
	}

	if (parameters[1]->isUnified(context->dom)) {
		v = parameters[1]->Get(context, context->dom, context->threadowner);
		if (v == aUNIVERSAL)
			return aFALSE;

		Atanor* v2 = v->Succ();
		v->Release();
		parameters[0]->Newvalue(v2, context->threadowner);
		v = context->PredicateEvalue(stack, currenthead, depth);
		parameters[0]->Setvalue(aNULL, aNOELEMENT, context->threadowner, false);
		return v2;
	}

	return aFALSE;
}


Exporting AtanorPredicate* AtanorPredicatePred::Duplicate(Atanor* context, AtanorDeclaration* dom) {
	if (parameters.size() != 2)
		return NULL;

	AtanorPredicatePred* p = new AtanorPredicatePred(globalAtanor, name);
	short idthread = ((AtanorInstructionEvaluate*)context)->threadowner;
	Atanor* e;

	for (short i = 0; i < 2; i++) {
		e = parameters[i]->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, true);
		if (e == NULL) {
			p->Release();
			return NULL;
		}
		p->parameters.push_back(e);
	}

	p->Setreference();
	return p;
}

//----------------------------------------------------------------------------------------------------
Exporting Atanor* AtanorPredicateTermMethod::Getvalues(AtanorDeclaration* dom, bool duplicate) {
	Atanor* head = parameters[0]->Getvalues(dom, duplicate);
	if (head == aNOELEMENT)
		return aNOELEMENT;

	Atanor* e;
	AtanorCallMethod call(name);
	short i;
	for (i = 1; i < parameters.size(); i++) {
		e = parameters[i]->Getvalues(dom, duplicate);
		if (e == aNOELEMENT) {
			head->Release();
			return aNOELEMENT;
		}
		call.arguments.push_back(e);		
	}

	short idthread = globalAtanor->GetThreadid();
	Atanor* res = call.Get(dom, head, idthread);

	for (i = 0; i < call.arguments.size(); i++)
		call.arguments[i]->Release();
	head->Release();
	if (res->isError()) {
		globalAtanor->Cleanerror(idthread);
		return aNOELEMENT;
	}
	return res;
}
//----------------------------------------------------------------------------------------------------

Exporting bool AtanorPredicateMethod::Checkparameters(AtanorDeclaration* dom) {
	short last = parameters.size() - 1;
	for (short i = 0; i < last; i++) {
		if (!parameters[i]->isUnified(dom))
			return false;
	}

	return true;
}

Exporting Atanor* AtanorPredicateMethod::PredicateEvalue(AtanorInstructionEvaluate* context, VECTE<Atanor*>& stack, AtanorPredicate* currenthead, int depth) {
	vector<Atanor*> args;
	short last = parameters.size() - 1;
	short i;
	for (i = 0; i < last; i++) {
		if (parameters[i]->isUnified(context->dom))
			args.push_back(parameters[i]->Get(context, context->dom, context->threadowner));
	}

	Atanor* res = aFALSE;
	Atanor* v;
	Atanor* r;
	if (parameters[last]->isUnified(context->dom)) {
		v = parameters[1]->Get(context, context->dom, context->threadowner);
		if (v == aUNIVERSAL) {
			for (i = 0; i < last; i++)
				args[i]->Release();
			
			if (negation)
				return aTRUE;
			return aFALSE;
		}

		AtanorCallMethod call(name);
		for (i = 1; i < last; i++)
			call.arguments.push_back(args[i]);

		res = call.Get(context->dom, args[0], context->threadowner);
		if (res->isVectorContainer()) {
			bool found = false;
			for (i = 0; i < res->Size(); i++) {
				r = res->getvalue(i);
				if (r->same(v) == aTRUE) {
					found = true;
					res->Release();
					if (negation)
						res = aFALSE;
					else
						res = context->PredicateEvalue(stack, currenthead, depth);
					break;
				}
			}

			if (!found) {
				res->Release();
				if (negation)
					res = aTRUE;
				else
					res = aFALSE;
			}
		}
		else {
			if (res->same(v) == aTRUE) {
				res->Release();
				if (negation)
					res = aFALSE;
				else
					res = context->PredicateEvalue(stack, currenthead, depth);
			}
			else {
				if (res->isError())
					globalAtanor->Cleanerror(context->threadowner);
				res->Release();
				res = aFALSE;
			}
		}

		v->Release();
		for (i = 0; i < last; i++)
			args[i]->Release();
		return res;
	}

	AtanorCallMethod call(name);
	for (i = 1; i < last; i++)
		call.arguments.push_back(args[i]);

	Atanor* prev = parameters[last]->Value();

	//if the value that is stored in the parameter is not empty
	if (prev != aNOELEMENT) {
		//then we push it into our new function
		call.arguments.push_back(prev);
		//but we remove it from the parameter value, to avoid its accidental destruction...
		parameters[last]->Setvalue(aNULL, aNOELEMENT, 0, true);
	}

	r = call.Get(context->dom, args[0], context->threadowner);

	if (r != aNOELEMENT && r != aNULL) {
		if (!negation) {
			long iter = 0;
			long mx = 1;

			if (r->isVectorContainer()) {
				v = aNOELEMENT;
				mx = r->Size();
			}
			else
				v = r;

			while (iter < mx) {
				if (r->isVectorContainer())
					v = r->getvalue(iter);

				parameters[last]->Newvalue(v, context->threadowner);

				v = context->PredicateEvalue(stack, currenthead, depth);

				if (v == aCUT) {
					res = aTRUE;
					break;
				}

				if (v->isError()) {
					globalAtanor->Cleanerror(context->threadowner);
					res = aFALSE;
					break;
				}

				if (v == aTRUE)
					res = aTRUE;

				iter++;
			}
		}
		r->Release();
	}
	else {
		if (negation)
			res = aTRUE;
	}

    for (i = 0; i < last; i++)
        args[i]->Release();
    
	//we put back our initial value...
	parameters[last]->Setvalue(aNULL, prev, 0, false);
	
    return res;
}



Exporting AtanorPredicate* AtanorPredicateMethod::Duplicate(Atanor* context, AtanorDeclaration* dom) {
	AtanorPredicateMethod* p = new AtanorPredicateMethod(globalAtanor, name, size);
	short idthread = ((AtanorInstructionEvaluate*)context)->threadowner;
	Atanor* e;

	for (short i = 0; i < parameters.size(); i++) {
		e = parameters[i]->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, true);
		if (e == NULL) {
			p->Release();
			return NULL;
		}
		p->parameters.push_back(e);
	}

	p->Setreference();
	return p;
}

//--------------------------------------------------------------------------------------------
void AtanorGlobal::RecordPredicateFunctions() {

	gpredicatename = 0;
	gpredicatedico = 0;
	gpredicatezone = 0;
	gpredicatedependency = 0;
	gpredicatefeature = 0;

	short n = Getid("between");
	predicates[n] = new AtanorPredicateBetween(this, n);
	n = Getid("succ");
	predicates[n] = new AtanorPredicateSucc(this, n);
	n = Getid("pred");
	predicates[n] = new AtanorPredicatePred(this, n);

	bin_hash<unsigned long>::iterator it;
	string name;
	for (it = allmethods.begin(); it != allmethods.end(); it++) {
		name = "p_" + Getsymbol(it->first);
		n = Getid(name);		
		predicates[n] = new AtanorPredicateMethod(this, it->first, it->second);
	}
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
// Procedures that deal with predicates... 
//----------------------------------------------------------------------------------------------------

Atanor* ProcPredicateDump(Atanor* context, short idthread, AtanorCall* callfunc) {
	//We display all predicates or one...
	string label;
	hmap<unsigned short, vector<AtanorPredicate*> >& knowledge = globalAtanor->threads[idthread].knowledgebase;
	Atanorvector* kvect = globalAtanor->Providevector();
	if (callfunc->Size() == 0) {

		for (auto& it : knowledge) {
			vector<AtanorPredicate*>& vect = it.second;
			for (size_t i = 0; i < vect.size(); i++)
				kvect->Push(vect[i]);
		}
		return kvect;
	}
	label = callfunc->Evaluate(0, context, idthread)->String();
	short name = globalAtanor->Getid(label);
	if (knowledge.find(name) == knowledge.end())
		return kvect;
	vector<AtanorPredicate*>& vect = knowledge[name];
	for (size_t i = 0; i < vect.size(); i++)
		kvect->Push(vect[i]);
	return kvect;
}
Atanor* ProcRetractAll(Atanor* context, short idthread, AtanorCall* callfunc) {
	//We display all predicates or one...	
	hmap<unsigned short, vector<AtanorPredicate*> >& knowledge = globalAtanor->threads[idthread].knowledgebase;
	if (callfunc->Size() == 0) {

		for (auto& it : knowledge) {
			vector<AtanorPredicate*>& vect = it.second;
			for (size_t i = 0; i < vect.size(); i++)
				vect[i]->Resetreference();
			vect.clear();
		}
		globalAtanor->threads[idthread].knowledgebase.clear();
		return aTRUE;
	}
	string label = callfunc->Evaluate(0, context, idthread)->String();
	short name = globalAtanor->Getid(label);
	if (knowledge.find(name) == knowledge.end())
		return aFALSE;
	vector<AtanorPredicate*>& vect = knowledge[name];
	for (size_t i = 0; i < vect.size(); i++)
		vect[i]->Resetreference();
	vect.clear();
	return aTRUE;
}

Atanor* ProcPredicateRetract(Atanor* context, short idthread, AtanorCall* callfunc) {
	Atanor* a = callfunc->arguments[0];
	if (!a->isPredicate()) {
		a = callfunc->Evaluate(0, context, idthread);
		if (!a->isPredicate())
			return globalAtanor->Returnerror("Expecting a predicate as argument", idthread);
	}
	AtanorPredicate* head = (AtanorPredicate*)a;
	long i;
	for (i = 0; i < head->parameters.size(); i++) {
		head->parameters[i] = head->parameters[i]->Get(context, aNULL, idthread);
		head->parameters[i]->Setreference();
	}

	if (globalAtanor->RemovePredicates(aNULLDECLARATION, head, idthread))
		a = aTRUE;
	else
		a = aFALSE;

	for (i = 0; i < head->parameters.size(); i++)
		head->parameters[i]->Resetreference();

	return a;
}

Atanor* ProcPredicateAsserta(Atanor* context, short idthread, AtanorCall* callfunc) {
	Atanor* a = callfunc->arguments[0];
	if (!a->isPredicate()) {
		a = callfunc->Evaluate(0, context, idthread);
		if (!a->isPredicate())
			return globalAtanor->Returnerror("Expecting a predicate as argument", idthread);
	}
	AtanorPredicate* head = (AtanorPredicate*)a;
	long i;
	for (i = 0; i < head->parameters.size(); i++) {
		head->parameters[i] = head->parameters[i]->Get(context, aNULL, idthread);
		head->parameters[i]->Setreference();
	}

	globalAtanor->StorePredicate(aNULLDECLARATION, head, false, idthread);

	for (i = 0; i < head->parameters.size(); i++)
		head->parameters[i]->Resetreference();
	return aTRUE;
}

Atanor* ProcPredicateAssertz(Atanor* context, short idthread, AtanorCall* callfunc) {
	Atanor* a = callfunc->arguments[0]; 
	if (!a->isPredicate()) {
		a = callfunc->Evaluate(0, context, idthread);
		if (!a->isPredicate())
			return globalAtanor->Returnerror("Expecting a predicate as argument", idthread);
	}
	AtanorPredicate* head = (AtanorPredicate*)a;
	long i;
	for (i = 0; i < head->parameters.size(); i++) {
		head->parameters[i] = head->parameters[i]->Get(context, aNULL, idthread);
		head->parameters[i]->Setreference();
	}

	globalAtanor->StorePredicate(aNULLDECLARATION, head, true, idthread);

	for (i = 0; i < head->parameters.size(); i++)
		head->parameters[i]->Resetreference();
	return aTRUE;
}

Atanor* ProcPredicateSucc(Atanor* context, short idthread, AtanorCall* callfunc) {
	return callfunc->Evaluate(0, context, idthread)->Succ();
}

Atanor* ProcPredicatePred(Atanor* context, short idthread, AtanorCall* callfunc) {
	return callfunc->Evaluate(0, context, idthread)->Pred();
}


Atanor* ProcDependencies(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	int i;

	bool trace = false;
	if (callfunc->Size())
		trace = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();

	if (globalAtanor->predicates.check(a_dependency) == false)
		return aNULL;

	AtanorPredicateFunction pv(globalAtanor, globalAtanor->predicates[a_dependency]->Function(), a_dependency);

	AtanorInstructionEvaluate kl(globalAtanor, &pv);
	kl.threadowner = idthread;

	kl.trace = trace;

	//Important variables...
	globalAtanor->SetPredicateVariableFlags();

	AtanorDeclarationPredicate domain;

	//if (globalAtanor->dependenciesvariable.size() != 0) {
	//	basebin_hash<short>::iterator itdep;
	//	for (itdep = globalAtanor->dependenciesvariable.begin(); itdep != globalAtanor->dependenciesvariable.end(); itdep++) {
	//		domain.Declare(itdep->first, aUNIVERSAL);
	//	}
	//}

	basebin_hash<AtanorPredicateVariableInstance*> localdico;
	kl.dico = &localdico;

	AtanorPredicateContainer* kpcont = globalAtanor->GetPredicatecontainer(idthread);

	if (kpcont != NULL)
		kl.rules = kpcont->rules;

	Atanor* e;
	AtanorPredicateVariable v1(NULL, globalAtanor->Getid("#1"));
	AtanorPredicateVariable v2(NULL, globalAtanor->Getid("#2"));

	e = v1.ExtractPredicateVariables(&kl, &domain, NULL, NULL, idthread, true);
	e->Setreference();
	pv.parameters.push_back(e);

	e = v2.ExtractPredicateVariables(&kl, &domain, NULL, NULL, idthread, true);
	e->Setreference();
	pv.parameters.push_back(e);

	//We force in this case the full traversal of all rules...
	kl.fulltraversal = FULLSEARCH;

	e = kl.Get(contextualpattern, &domain, idthread);

	for (i = 0; i < pv.Size(); i++)
		pv.parameters[i]->Resetreference();

	basebin_hash<Atanor*>::iterator it;

	for (it = domain.declarations.begin(); it != domain.declarations.end(); it++) {
		it->second->Setprotect(0);
		it->second->Resetreference();
	}

	return e;
}


//----------------------------------------------------------------------------------------------------
void AtanorGlobal::RecordPredicates() {
	gFAIL = new AtanorConstPredicate(this, a_fail);
	gCUT = new AtanorConstPredicate(this, a_cut);
	gSTOP = new AtanorConstPredicate(this, a_stop);

	RecordOneProcedure("asserta", &ProcPredicateAsserta, P_ONE | P_ONE);
	RecordOneProcedure("assertz", &ProcPredicateAssertz, P_ONE | P_ONE);
	RecordOneProcedure("predicatedump", &ProcPredicateDump, P_NONE | P_ONE);
	RecordOneProcedure("findall", &ProcPredicateDump, P_NONE | P_ONE);
	RecordOneProcedure("retract", &ProcPredicateRetract, P_ONE);
	RecordOneProcedure("retractall", &ProcRetractAll, P_NONE | P_ONE);
	RecordOneProcedure("succ", &ProcPredicateSucc, P_ONE);
	RecordOneProcedure("pred", &ProcPredicatePred, P_ONE);
	RecordOneProcedure("_dependencies", &ProcDependencies, P_NONE | P_ONE);
}



