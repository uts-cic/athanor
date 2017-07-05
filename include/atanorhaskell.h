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
filename   : atanor.h
Date       : 2017/04/01
Purpose    :
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanorhaskell_h
#define atanorhaskell_h


//----------------------------------------------------------------------------------
class AtanorDeclarationAutoClean : public Atanor {
public:

	VECTE<short> names;
	VECTE<Atanor*> declarations;
	short i;
	short idthread;

	AtanorDeclarationAutoClean(short id) : idthread(id) {}

	void Declare(short id, Atanor* a) {
		for (i = 0; i < names.last; i++) {
			if (names.vecteur[i] == id) {
				globalAtanor->Removevariable(idthread, id);
				declarations.vecteur[i]->Resetreference();
				declarations.vecteur[i] = a;
				return;
			}
		}

		names.push_back(id);
		declarations.push_back(a);
	}


	void Cleaning(short idthread) {
		for (i = 0; i < names.last; i++) {
			declarations.vecteur[i]->Resetreference();
			globalAtanor->Removevariable(idthread, names.vecteur[i]);
		}

		names.last = 0;
		declarations.last = 0;
	}

	bool isEmpty() {
		if (!names.last)
			return true;
		return false;
	}
};

//------------------------------------------------------------------------
class AtanorInstructionHASKELLCASE : public AtanorInstruction {
public:

	AtanorInstructionHASKELLCASE(AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_match, g, parent) {
		idtype = a_match;
	}

	Atanor* Get(Atanor* context, Atanor* value, short idthread);
};


//------------------------------------------------------------------------
/// When a function, a method or a procedure are passed as parameter...
class AtanorParameterFunction : public AtanorTracked {
public:

	short name;

	AtanorParameterFunction(short n, short ty, AtanorGlobal* g, Atanor* parent) : name(n), AtanorTracked(ty, g, parent) {}

	short Name() {
		return name;
	}

	string String();

	bool isFunctionParameter() {
		return true;
	}

	bool duplicateForCall() {
		return false;
	}

};


class AtanorFunctionHaskellParameter : public AtanorParameterFunction {
public:


	AtanorFunctionHaskellParameter(short n, AtanorGlobal* g, Atanor* parent) : AtanorParameterFunction(n, a_callhaskell, g, parent) {}

	Atanor* Execute(Atanor* environment, Atanor* value, short idthread);
};


class AtanorFunctionParameter : public AtanorParameterFunction {
public:

	AtanorFunctionParameter(short n, AtanorGlobal* g, Atanor* parent) : AtanorParameterFunction(n, a_callfunction, g, parent) {}

	Atanor* Execute(Atanor* environment, Atanor* value, short idthread);

	Atanor* Get(Atanor* context, Atanor* val, short idthread);

};

class AtanorMethodParameter : public AtanorParameterFunction {
public:


	AtanorMethodParameter(short n, AtanorGlobal* g, Atanor* parent) : AtanorParameterFunction(n, a_callmethod, g, parent) {}

	Atanor* Execute(Atanor* environment, Atanor* value, short idthread);

};

class AtanorCommonParameter : public AtanorParameterFunction {
public:

	AtanorCommonParameter(short n, AtanorGlobal* g, Atanor* parent) : AtanorParameterFunction(n, a_callmethod, g, parent) {}

	Atanor* Execute(Atanor* environment, Atanor* value, short idthread);

};

class AtanorProcedureParameter : public AtanorParameterFunction {
public:


	AtanorProcedureParameter(short n, AtanorGlobal* g, Atanor* parent) : AtanorParameterFunction(n, a_callprocedure, g, parent) {}

	Atanor* Execute(Atanor* environment, Atanor* value, short idthread);

};

//-------------------------------------------------------------------------------------------
class AtanorGetMethod : public AtanorArguments {
public:


	AtanorGetMethod(short n, AtanorGlobal* g, Atanor* parent) : AtanorArguments(n, a_self, g, parent) {}


	Atanor* Get(Atanor* context, Atanor* callfunction, short idthread);

};

class AtanorGetCommon : public AtanorArguments {
public:


	AtanorGetCommon(short n, AtanorGlobal* g, Atanor* parent) : AtanorArguments(n, a_self, g, parent) {}


	Atanor* Get(Atanor* context, Atanor* callfunction, short idthread);

};

class AtanorGetFunction : public AtanorArguments {
public:


	AtanorGetFunction(short n, AtanorGlobal* g, Atanor* parent) : AtanorArguments(n, a_self, g, parent) {}


	Atanor* Get(Atanor* context, Atanor* callfunction, short idthread);

};
//-------------------------------------------------------------------------------------------
class AtanorLambdaDomain : public AtanorSequence {
public:
	short name;
	bool local;
	bool adding;

	AtanorLambdaDomain(AtanorGlobal* g = NULL, Atanor* parent = NULL) : local(false), name(1), adding(true), AtanorSequence(g, parent)  {
		name = 1;
	}

	void AddInstruction(Atanor* a) {
		if (adding)
			instructions.push_back(a);
	}

};

class AtanorFunctionLambda : public AtanorFunction {
public:
	AtanorLambdaDomain lambdadomain;	

	AtanorFunctionLambda(int n, AtanorGlobal* g = NULL, Atanor* parent = NULL) : AtanorFunction(n, g, parent) {
		idtype = a_lambda;
	}

	AtanorLambdaDomain* Thedomain() {
		return &lambdadomain;
	}

	AtanorFunctionLambda* Following() {
		return (AtanorFunctionLambda*)next;
	}

	bool isHaskellFunction() {
		return true;
	}

	Atanor* Get(Atanor* context, Atanor* callfunction, short idthread);	
};

//-----------------------------------------------------------------------
class HaskellLoop {
public:
	vector<Atanor*> recipients;
	vector<Atanor*> vects;
	vector<AtanorIteration*> loops;
	vector<Atanor*> recreate;
	Atanor* environment;
	HaskellLoop* next;
	short idthread;

	HaskellLoop(short id, Atanor* e) : next(NULL), idthread(id), environment(e) {}

	~HaskellLoop() {
		for (int i = 0; i < loops.size(); i++) {
			loops[i]->Release();
			vects[i]->Release();
		}
		if (next != NULL)
			delete next;
	}

	void push(Atanor* r, Atanor* v, AtanorIteration* l) {
		recipients.push_back(r);
		loops.push_back(l);
		if (l == aITERNULL) {
			recreate.push_back(v);
			vects.push_back(aNULL);
		}
		else {
			recreate.push_back(NULL);
			vects.push_back(v);
		}
	}

	void add(HaskellLoop* h) {
		HaskellLoop* a = this;
		while (a->next != NULL)
			a = a->next;
		a->next = h;
	}

	bool end() {
		for (size_t l = 0; l < loops.size(); l++) {
			if (loops[l]->End()->Boolean())
				return true;
		}
		return false;
	}

	void execute(short idthread) {
		for (size_t l = 0; l < loops.size(); l++) {
			Atanor* a = loops[l]->IteratorValue();
			recipients[l]->Setvalue(aNULL, a, idthread);
			loops[l]->Next();
		}
	}

	void reset() {
		for (size_t l = 0; l < loops.size(); l++) {
			if (recreate[l] != NULL) {
				Atanor* a = recreate[l]->Get(environment, aNULL, idthread);
				if (a != aNOELEMENT) {
					vects[l]->Release();
					loops[l]->Release();
					vects[l] = a;
					loops[l] = a->Newiteration(false);
					loops[l]->Begin();
				}
			}
			else
				loops[l]->Begin();
		}
	}
};
//-----------------------------------------------------------------------
class AtanorCallFunctionHaskell : public AtanorCall {
public:
	AtanorFunctionLambda* body;	
	char haskellchoice;

	AtanorLambdaDomain*  Thedomain() {
		return &body->lambdadomain;
	}

	AtanorFunction* Body() {
		return body;
	}

	virtual Atanor* Composition();

	bool Checkarity();

	AtanorCallFunctionHaskell(AtanorGlobal* global, Atanor* parent = NULL, AtanorFunctionLambda* b = NULL, short n = 1) : AtanorCall(a_callhaskell, global, parent) {
		body = b;
		name = n;
		haskellchoice = 1;
	}

	AtanorCallFunctionHaskell(AtanorFunctionLambda* func, AtanorGlobal* global = NULL, Atanor* parent = NULL) : AtanorCall(func->Name(), a_callhaskell, global, parent) {
		body = func;
		haskellchoice = 1;
	}


	virtual Atanor* Get(Atanor*context, Atanor* value, short idthread);
	Atanor* GetHaskell2(Atanor* context, Atanor* env, AtanorFunctionLambda*, short idthread);
	Atanor* GetHaskell3(Atanor* context, Atanor* env, AtanorFunctionLambda*, short idthread);
	Atanor* GetHaskell5(Atanor* context, Atanor* env, AtanorFunctionLambda*, short idthread);
	Atanor* GetHaskellN(Atanor* context, Atanor* env, AtanorFunctionLambda*, short idthread);

	Atanor* Process(HaskellLoop* loop, Atanor* context, Atanor* env, AtanorFunctionLambda* af, short idthread);

	void Init(Atanor* f) {
		if (body != NULL)
			return;

		if (f == NULL)
			body = new AtanorFunctionLambda(1, globalAtanor);
		else
			body = (AtanorFunctionLambda*)f;
	}

	void AddInstruction(Atanor* ke) {
		switch (haskellchoice) {
		case 1:
			arguments.push_back(ke);
			break;
		case 2:
			function = (AtanorCall*)ke;
			break;
		case 3:
			body->lambdadomain.AddInstruction(ke);
			break;
		case 4:
			body->AddInstruction(ke);
			break;
		}
	}

	bool isDeclared(short id) {
		if (body == NULL)
			return false;
		return body->lambdadomain.isDeclared(id);
	}

	void Declare(short id, Atanor* a) {
		if (body == NULL)
			return;
		body->lambdadomain.Declare(id, a);
	}

	Atanor* Declaration(short id) {
		if (body == NULL)
			return NULL;
		return body->lambdadomain.Declaration(id);
	}

};

class AtanorCallFunctionArgsHaskell : public AtanorCallFunctionHaskell {
public:

	AtanorCallFunctionArgsHaskell(AtanorGlobal* global) : AtanorCallFunctionHaskell(global, NULL) {}
	AtanorCallFunctionArgsHaskell(AtanorFunctionLambda* func, AtanorGlobal* global, Atanor* parent) : AtanorCallFunctionHaskell(func, global, parent) {}

	Atanor* Get(Atanor*context, Atanor* value, short idthread);

	Atanor* Composition() {
		return aNOELEMENT;
	}
};

class AtanorInstructionHaskellIF : public AtanorInstruction {
public:

	AtanorInstructionHaskellIF(AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_instructions, g, parent) {
		idtype = a_if;
	}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);
};

#endif
