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

	void Replacedeclaration(short idthread, short id, Atanor* a) {
		i = names.search(id);
		if (i != -1) {
			if (a != declarations.vecteur[i]) {
				if (declarations.vecteur[i]->isLetSelf())
					declarations.vecteur[i]->Putvalue(a, idthread);
				else {
					globalAtanor->Replacevariable(idthread, id, a);
					declarations.vecteur[i]->Resetreference();
					declarations.vecteur[i] = a;
					a->Setreference();
				}
			}
		}
		else {
			names.push_back(id);
			declarations.push_back(a);
		}
	}


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

	bool hasDeclaration() {
		return true;
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

	short Type() {
		return a_haskelldeclaration;
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

	short Typeinfered() {
		return a_callhaskell;
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

class AtanorFrameMethodParameter : public AtanorParameterFunction {
public:


	AtanorFrameMethodParameter(short n, AtanorGlobal* g, Atanor* parent) : AtanorParameterFunction(n, a_callmethod, g, parent) {}

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

class AtanorGetFunctionThrough : public AtanorArguments {
public:
	Atanor* call;

	AtanorGetFunctionThrough(Atanor* c, AtanorGlobal* g, Atanor* parent) : call(c), AtanorArguments(c->Name(), a_call, g, parent) {}


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

class Haskelldeclaration{
public:
	short atype;

	Haskelldeclaration(short w) : atype(w) {}

	short Type() {
		return atype;
	}

	virtual void Push(short) {}

	virtual short Size() {
		return 0;
	}

	virtual short Typevariable(short i) { return a_none; }

	virtual Haskelldeclaration* copy() {
		return new Haskelldeclaration(atype);
	}
};

class SubHaskelldeclaration : public Haskelldeclaration {
public:
	vector<short> types;

	SubHaskelldeclaration(short w) : Haskelldeclaration(w) {}

	void Push(short t) {
		types.push_back(t);
	}

	short Size() {
		return types.size();
	}

	short Typevariable(short i) {
		return types[i];
	}

	Haskelldeclaration* copy() {
		SubHaskelldeclaration* sub=new SubHaskelldeclaration(atype);
		sub->types = types;
		return sub;
	}

};

class AtanorFunctionLambda : public AtanorFunction {
public:
	AtanorLambdaDomain lambdadomain;	
	vector<Haskelldeclaration*> haskelldeclarations;
	bool hdeclared, store;

	AtanorFunctionLambda(int n, AtanorGlobal* g = NULL, Atanor* parent = NULL) : hdeclared(false), store(false), AtanorFunction(n, g, parent) {
		idtype = a_lambda;
	}

	~AtanorFunctionLambda() {
		for (int i = 0; i < haskelldeclarations.size(); i++)
			delete haskelldeclarations[i];
	}

	void sethaskelldeclarations(vector<Haskelldeclaration*>& m) {
		for (int i = 0; i < m.size(); i++)
			haskelldeclarations.push_back(m[i]->copy());
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

	bool Purehaskelldeclaration() {
		if (instructions.size() == 0 && hdeclared)
			return true;
		return false;
	}
};

class AtanorGetFunctionLambda : public AtanorTracked {
public:

	AtanorFunctionLambda* function;
	AtanorGetFunctionLambda(AtanorFunctionLambda* f, AtanorGlobal* g) : function(f), AtanorTracked(a_callhaskell, g) {}

	Atanor* Get(Atanor* c, Atanor* cc, short idthread) {
		return function;
	}
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

	AtanorFunction* Body(short idthread) {
		return body;
	}

	bool hasDeclaration() {
		return true;
	}

	virtual Atanor* Composition();

	bool Checkarity();

	short Typeinfered();

	short Returntype() {
		return body->Returntype();
	}

	AtanorCallFunctionHaskell(AtanorGlobal* global, Atanor* parent = NULL, AtanorFunctionLambda* b = NULL, short n = 1) : AtanorCall(a_callhaskell, global, parent) {
		body = b;
		name = n;
		haskellchoice = 1;
	}

	AtanorCallFunctionHaskell(AtanorFunctionLambda* func, AtanorGlobal* global, Atanor* parent) : AtanorCall(func->Name(), a_callhaskell, global, parent) {
		body = func;
		haskellchoice = 1;
	}

	AtanorCallFunctionHaskell(AtanorFunctionLambda* func) : AtanorCall(func->Name(), a_callhaskell) {
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
	bool hdeclared;

	AtanorCallFunctionArgsHaskell(AtanorGlobal* global) : hdeclared(false), AtanorCallFunctionHaskell(global, NULL) {}
	AtanorCallFunctionArgsHaskell(AtanorFunctionLambda* func, AtanorGlobal* global, Atanor* parent) : hdeclared(false), AtanorCallFunctionHaskell(func, global, parent) {
		if (func != NULL)
			hdeclared = func->hdeclared;
	}

	AtanorCallFunctionArgsHaskell(AtanorFunctionLambda* func) : hdeclared(false), AtanorCallFunctionHaskell(func) {
		hdeclared = func->hdeclared;
	}

	Atanor* Get(Atanor*context, Atanor* value, short idthread);

	Atanor* Composition() {
		return aNOELEMENT;
	}

	short Typeinfered();

};

class AtanorInstructionHaskellIF : public AtanorInstruction {
public:
	bool compiled;
	AtanorInstructionHaskellIF(AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_instructions, g, parent) {
		idtype = a_if;
		compiled = false;
	}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);
};

class AtanorInstructionHaskellMainCASE : public AtanorInstruction {
public:
	long other;

	AtanorInstructionHaskellMainCASE(AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_instructions, g, parent) {
		idtype = a_if;
		other = 0;
	}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);
};

class AtanorFrameParameter : public AtanorObject {
public:
	short framename;
	vector<short> equivalence;

	AtanorFrameParameter(short fn, AtanorGlobal* g, Atanor* parent) : AtanorObject(g, parent) { 
		framename = fn; 
	}

	Atanor* Compare(Atanor* env, Atanor* v, short idthread);

	void Declare(short id, Atanor* var) {
		equivalence.push_back(id);
		equivalence.push_back(var->Name());
	}

	short Type() {
		return framename;
	}
};

class AtanorCallFrameMethod : public AtanorCallClean {
public:
	bool Checkarity();

	AtanorCallFrameMethod(short n, AtanorGlobal* global = NULL, Atanor* parent = NULL) :
		AtanorCallClean(n, a_callfunction, global, parent) {}

	Atanor* Get(Atanor* context, Atanor* domain, short idthread);

	short Typeinfered() {
		if (function != NULL)
			return function->Typeinfered();
		return a_none;
	}

};


#endif
