
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
filename   : predicatefunctions.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef predicatefunctions_h
#define predicatefunctions_h


//----------------------------------------------------------------------------------------------------
class AtanorPredicateBetween : public AtanorPredicateFunction {
public:

	AtanorPredicateBetween(AtanorGlobal* g, short n) : AtanorPredicateFunction(g, NULL, n) {}

	Atanor* Newinstance(short idthread, Atanor* f = NULL) {
		return new AtanorPredicateBetween(NULL, name);
	}

	bool isPredicateFunction() {
		return true;
	}

	bool Checkarity() {
		if (parameters.size() != 3)
			return false;
		return true;
	}

	Exporting Atanor* PredicateEvalue(AtanorInstructionEvaluate* context, VECTE<Atanor*>& stack, AtanorPredicate* currenthead, int depth);
	Exporting AtanorPredicate* Duplicate(Atanor* context, AtanorDeclaration* dom);
	Exporting bool Checkparameters(AtanorDeclaration*);
};

//----------------------------------------------------------------------------------------------------
class AtanorPredicateTermSucc : public AtanorPredicateTerm {
public:

	AtanorPredicateTermSucc(AtanorGlobal* g, short n, Atanor* parent = NULL) : AtanorPredicateTerm(g, n, parent) {}
	Exporting Atanor* Getvalues(AtanorDeclaration* dom, bool duplicate);

	Atanor* Newinstance(short idthread, Atanor* parent = NULL) {
		return new AtanorPredicateTermSucc(NULL, name);
	}

};

class AtanorPredicateSucc : public AtanorPredicateFunction {
public:

	AtanorPredicateSucc(AtanorGlobal* g, short n) : AtanorPredicateFunction(g, NULL, n) {}

	Atanor* Newinstance(short idthread, Atanor* parent) {
		if (parent != NULL && parent->Type() == a_parameterpredicate)
			return new AtanorPredicateTermSucc(NULL, name);

		return new AtanorPredicateSucc(NULL, name);
	}


	bool isPredicateFunction() {
		return true;
	}

	bool Checkarity() {
		if (parameters.size() == 2)
			return true;
		return false;
	}

	Exporting bool Checkparameters(AtanorDeclaration*);

	Exporting Atanor* PredicateEvalue(AtanorInstructionEvaluate* context, VECTE<Atanor*>& stack, AtanorPredicate* currenthead, int depth);
	Exporting AtanorPredicate* Duplicate(Atanor* context, AtanorDeclaration* dom);

	Atanor* Get(Atanor* context, Atanor* obj, short idthread) {
		if (context->Type() == a_predicateevaluate)
			return AtanorPredicateFunction::Get(context, obj, idthread);
		return Succ();
	}
};

//----------------------------------------------------------------------------------------------------
class AtanorPredicateTermPred : public AtanorPredicateTerm {
public:

	AtanorPredicateTermPred(AtanorGlobal* g, short n, Atanor* parent = NULL) : AtanorPredicateTerm(g, n, parent) {}
	Exporting Atanor* Getvalues(AtanorDeclaration* dom, bool duplicate);

	Atanor* Newinstance(short idthread, Atanor* parent = NULL) {
		return new AtanorPredicateTermPred(NULL, name);
	}

};

class AtanorPredicatePred : public AtanorPredicateFunction {
public:

	AtanorPredicatePred(AtanorGlobal* g, short n) : AtanorPredicateFunction(g, NULL, n) {}

	Atanor* Newinstance(short idthread, Atanor* parent) {
		if (parent != NULL && parent->Type() == a_parameterpredicate)
			return new AtanorPredicateTermPred(NULL, name);

		return new AtanorPredicatePred(NULL, name);
	}


	bool isPredicateFunction() {
		return true;
	}

	bool Checkarity() {
		if (parameters.size() == 2)
			return true;
		return false;
	}

	Exporting bool Checkparameters(AtanorDeclaration*);

	Exporting Atanor* PredicateEvalue(AtanorInstructionEvaluate* context, VECTE<Atanor*>& stack, AtanorPredicate* currenthead, int depth);
	Exporting AtanorPredicate* Duplicate(Atanor* context, AtanorDeclaration* dom);

	Atanor* Get(Atanor* context, Atanor* obj, short idthread) {
		if (context->Type() == a_predicateevaluate)
			return AtanorPredicateFunction::Get(context, obj, idthread);
		return Pred();
	}

};

//----------------------------------------------------------------------------------------------------
class AtanorPredicateTermMethod : public AtanorPredicateTerm {
public:
	unsigned long size;

	AtanorPredicateTermMethod(AtanorGlobal* g, short n, unsigned long sz, Atanor* parent = NULL) : size(sz), AtanorPredicateTerm(g, n, parent) {}
	Exporting Atanor* Getvalues(AtanorDeclaration* dom, bool duplicate);

	Atanor* Newinstance(short idthread, Atanor* parent = NULL) {
		return new AtanorPredicateTermMethod(globalAtanor, name, size);
	}

	bool Checkarity() {
		return Arity(size, parameters.size() - 1);
	}

};

class AtanorPredicateMethod : public AtanorPredicateFunction {
public:
	unsigned long size;

	AtanorPredicateMethod(AtanorGlobal* g, short n, unsigned long sz) : AtanorPredicateFunction(g, NULL, n) { 
		size = sz; 
	}

	Atanor* Newinstance(short idthread, Atanor* parent = NULL) {
		if (parent != NULL && parent->Type() == a_parameterpredicate)
			return new AtanorPredicateTermMethod(globalAtanor, name, size);

		return new AtanorPredicateMethod(globalAtanor, name, size);
	}

	bool isPredicateFunction() {
		return true;
	}

	bool Checkarity() {
		return Arity(size, parameters.size() - 2);
	}

	Exporting Atanor* PredicateEvalue(AtanorInstructionEvaluate* context, VECTE<Atanor*>& stack, AtanorPredicate* currenthead, int depth);
	Exporting AtanorPredicate* Duplicate(Atanor* context, AtanorDeclaration* dom);
	Exporting bool Checkparameters(AtanorDeclaration*);


};

#endif



