
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
filename   : instructions.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef instructions_h
#define instructions_h

#include "atanorshort.h"
#include "atanorint.h"
#include "atanordecimal.h"
#include "atanorfloat.h"
#include "atanorlong.h"
class Atanordecimal;

class Atanoratanor;

#define DIVIDEDBYZERO -101

//--------------------------------------------------------------------
class AtanorDeclarationLocal : public AtanorTracked {
public:

	VECTE<short> names;
	VECTE<Atanor*> declarations;
	short i;

	AtanorDeclarationLocal() : AtanorTracked(NULL) {}

	bool isDeclaration() {
		return true;
	}

	bool isEmpty() {
		if (!declarations.last)
			return true;
		return false;
	}

	bool isDeclared(short id) {
		for (i = 0; i < names.last; i++) {
			if (names.vecteur[i] == id)
				return true;
		}

		return false;
	}


	void Variables(vector<short>& vars) {
		for (i = 0; i < names.last; i++)
			vars.push_back(names[i]);
	}

	void Declare(short id, Atanor* a) {
		names.push_back(id);
		declarations.push_back(a);
	}

	Atanor* Declaration(short id) {
		for (i = 0; i < names.last; i++) {
			if (names.vecteur[i] == id)
				return declarations[i];
		}
		return NULL;
	}


	void Cleaning(short idthread) {
		for (i = 0; i < names.last; i++) {
			declarations.vecteur[i]->Resetreference();
			globalAtanor->Removevariable(idthread, names.vecteur[i]);
		}

		names.last = 0;
		declarations.last = 0;
	}

};

class AtanorDeclarationSelfPush : public AtanorDeclarationLocal {
public:

	short idthread;

	AtanorDeclarationSelfPush(short id) : idthread(id) {
		globalAtanor->Pushstack(this, idthread);
	}

	~AtanorDeclarationSelfPush() {
		Cleaning(idthread);
		globalAtanor->Popstack(idthread);
	}

};

//--------------------------------------------------------------------
//This function call is used to call methods associated to objects such as i.log()
class AtanorCallMethod : public AtanorCallClean {
public:

	Exporting bool Checkarity();
	AtanorCallMethod(short m, AtanorGlobal* global = NULL, Atanor* parent = NULL) : AtanorCallClean(m, a_callmethod, global, parent) {}

	Exporting virtual Atanor* Get(Atanor* context, Atanor* object, short idthread);
	Exporting Atanor* Put(Atanor* context, Atanor* object, short idthread);

	short Typeinfered() {
		if (function != NULL)
			return function->Typevariable();

		if (!globalAtanor->returntypes.check(name))
			return a_none;

		return globalAtanor->returntypes[name];
	}

};

class AtanorCallCommonMethod : public AtanorCallMethod {
public:

	bool Checkarity();

	AtanorCallCommonMethod(short m, AtanorGlobal* global = NULL, Atanor* parent = NULL) : AtanorCallMethod(m, global, parent) {}

	Atanor* Get(Atanor* context, Atanor* object, short idthread);

	Atanor* Execute(Atanor* environment, Atanor* value, short idthread) {
		return AtanorCallMethod::Get(environment, value, idthread);
	}

};

//This function call is used to call user declared functions
class AtanorCallFunction : public AtanorCall {
public:
	Atanor* body;
	bool nonlimited;

	Exporting bool Checkarity();

	AtanorCallFunction(Atanor* b, AtanorGlobal* global = NULL, Atanor* parent = NULL) : body(b), AtanorCall(a_callfunction, global, parent) {
		nonlimited = false;
		if (b != NULL)
			name = b->Name();
	}

	Exporting virtual Atanor* Get(Atanor* context, Atanor* domain, short idthread);

	short Name() {
		return name;
	}

	Atanor* Body(short idthread) {
		return body->Body(idthread);
	}

	short Typeinfered() {
		if (body != NULL)
			return body->Typeinfered();
		return a_none;
	}
};

class AtanorFunctionDeclarationCall : public AtanorCallFunction {
public:

	AtanorFunctionDeclarationCall(short n, AtanorGlobal* global, Atanor* parent) : AtanorCallFunction(NULL, global, parent) { name = n; }
	Atanor* Get(Atanor* context, Atanor* callfunction, short idthread) {
		body = globalAtanor->Getdeclaration(name, idthread);
		body = body->Body(idthread);
		if (body == NULL)
			return globalAtanor->Returnerror("Cannot execute this function", idthread);
		return AtanorCallFunction::Get(context, callfunction, idthread);
	}

};


class AtanorCallThread : public AtanorCall {
public:
	Atanor* body;
	Atanor* recipient;
	bool nonlimited;

	bool Checkarity();
	AtanorCallThread(Atanor* b, AtanorGlobal* global = NULL, Atanor* parent = NULL) : 
		nonlimited(false),recipient(NULL), body(b), AtanorCall(a_callthread, global, parent) {}

	virtual Atanor* Get(Atanor* context, Atanor* domain, short idthread);

	short Name() {
		return body->Name();
	}

	Atanor* Body(short idthread) {
		return body->Body(idthread);
	}

	Atanor* Push(Atanor* v) {
		recipient = v;
		recipient->Setreference();
		return aTRUE;
	}
};


//This function call is used to call user declared functions
class AtanorThreadCall : public AtanorCallFunction {
public:
	Atanor* domain;
	Atanor* recipient;
	long idomain;
	short joinpos;
	short parentid;
	short idthread;
	bool cleandom;
	bool exclusive;
	std::thread* thid;
	threadhandle tid;

	AtanorThreadCall(Atanor* b, Atanor* r, Atanor* d, bool c, bool e, short id, short pid) : AtanorCallFunction(b) {
		recipient = r;
		parentid = pid;
		joinpos = -1;
		idtype = a_callthread;
		domain = d;
		idthread = id;
		cleandom = c;
		exclusive = e;
		idomain = -1;
		if (cleandom)
			idomain = d->Setprotect();
	}

	Atanor* Get(Atanor* domain, Atanor* value, short idthread);

	short Name() {
		return body->Name();
	}

};

//This function call is used to call user declared functions
class AtanorCallFrameFunction : public AtanorCallClean {
public:
    AtanorFrame* frame;
    
    bool Checkarity();
    
    AtanorCallFrameFunction(AtanorFrame* f, short n, AtanorGlobal* global = NULL, Atanor* parent = NULL) :
    frame(f), AtanorCallClean(n, a_callfunction, global, parent) {}
    
    virtual Atanor* Get(Atanor* context, Atanor* domain, short idthread);
	virtual Atanor* Put(Atanor* context, Atanor* domain, short idthread);

    Atanor* Frame() {
        return frame;
    }

	short Typeinfered() {
		if (function != NULL)
			return function->Typeinfered();

		return frame->Declaration(name)->Typeinfered();
	}

};

//This function call is used to call user declared functions
class AtanorCallTopFrameFunction : public AtanorCallFrameFunction {
public:
    
    AtanorCallTopFrameFunction(AtanorFrame* f, short n, AtanorGlobal* global = NULL, Atanor* parent = NULL) :
    AtanorCallFrameFunction(f,n, global, parent) {}
    
    Atanor* Get(Atanor* context, Atanor* domain, short idthread);
	Atanor* Put(Atanor* context, Atanor* domain, short idthread);

    Atanor* Frame() {
        return frame;
    }
};

//This function call is used to call procedures such as print, println etc...
class AtanorCallProcedure : public AtanorCallClean {
public:

	bool Checkarity();

	AtanorCallProcedure(short m, AtanorGlobal* global = NULL, Atanor* parent = NULL) : AtanorCallClean(m, a_callprocedure, global, parent) {}

	Atanor* Get(Atanor* context, Atanor* value, short idthread);

	short Typeinfered() {
		if (globalAtanor->returntypes.check(name))
			return globalAtanor->returntypes[name];
		return a_none;
	}

};

class AtanorCallReturn : public AtanorTracked {
public:
	Atanor* argument;

	AtanorCallReturn(AtanorGlobal* global = NULL, Atanor* parent = NULL) : argument(NULL), AtanorTracked(a_return, global, parent) {}

	virtual Atanor* Get(Atanor* context, Atanor* v, short idthread) {
		if (argument != NULL)
			globalAtanor->threads[idthread].returnvalue = argument->Get(context, aNULL, idthread);
		else
			globalAtanor->threads[idthread].returnvalue = aNULL;
		return this;
	}

	virtual Atanor* Put(Atanor* index, Atanor* v, short idthread) {
		globalAtanor->threads[idthread].returnvalue = v;
		return aTRUE;
	}

	bool needFullInvestigate() {
		return true;
	}

	bool needInvestigate() {
		return true;
	}

	bool isReturned() {
		return true;
	}

	bool isCall() {
		return true;
	}

	bool isCallFunction() {
		return true;
	}

	long Size() {
		if (argument == NULL)
			return 0;
		return 1;
	}

	Atanor* Argument(size_t i) {
		if (argument == NULL)
			return aNULL;
		return argument;
	}

	Atanor* Returned(short idthread) {
		return globalAtanor->threads[idthread].returnvalue;
	}

	virtual void AddInstruction(Atanor* a) {
		if (argument != NULL)
			return;
		argument = a;
	}
};

//To handle aBREAKTRUE et aBREAKFALSE
class AtanorCallBreak : public AtanorCallReturn {
public:

	AtanorCallBreak(Atanor* a, AtanorGlobal* global) : AtanorCallReturn(global) {
		argument = a;
	}

	Atanor* Get(Atanor* context, Atanor* v, short idthread) {
		globalAtanor->threads[idthread].returnvalue = argument->Get(context, aNULL, idthread);
		return this;
	}

	Atanor* Put(Atanor* index, Atanor* v, short idthread) {
		return aTRUE;
	}

	void AddInstruction() {}

};
//----------------------------------------------------------------------
//Variables are declarations, which are executed through a Put
class AtanorVariableDeclaration : public AtanorTracked {
public:
	Atanor* initialization;
	Atanor* function;

	short name;
	short typevariable;

	bool isprivate;
	bool choice;
	bool isframe;
	bool isconstant;

	AtanorVariableDeclaration(AtanorGlobal* g, short n, short t, bool p, bool c, Atanor* parent);
	AtanorVariableDeclaration(AtanorGlobal* g, short n, short t, Atanor* parent = NULL);

	//When we call this function, we actually will create an element of type value
	virtual Atanor* Get(Atanor* context, Atanor* value, short idthread);
	virtual Atanor* Put(Atanor* index, Atanor* value, short idthread);
	bool Setvalue(Atanor* index, Atanor* value, short idthread, bool strict = false);

	bool isFrame() {
		return isframe;
	}

	bool isConstant() {
		return isconstant;
	}

    virtual void ScanVariables(vector<short>& vars) {
        vars.push_back(name);
        if (initialization!=NULL)
            initialization->ScanVariables(vars);
    }

	bool isFunction() {
		//We can check whether it is a AtanorFunctionCall object, then it can be a function
		if (typevariable == a_call)
			return true;
		return false;
	}

	Atanor* Initialisation() {
		return initialization;
	}

	void AddInstruction(Atanor* a) {
		if (!choice)
			initialization = a;
		else
			function = a;
	}

	bool Private() {
		return isprivate;
	}

	short Typevariable() {
		return typevariable;
	}

	short Typeinfered() {
		return typevariable;
	}

	Atanor* Frame() {
		if (!globalAtanor->newInstance.check(typevariable))
			return NULL;
		return globalAtanor->newInstance[typevariable]->Frame();
	}

	bool isVariable() {
		return true;
	}

	Atanor* Function() {
		return function;
	}

    bool Setstopindex() {
        if (function !=NULL)
            function->Setstopindex();
        return false;
    }

	short Name() {
		return name;
	}
};


class AtanorFrameVariableDeclaration : public AtanorVariableDeclaration {
public:
	bool common;

	AtanorFrameVariableDeclaration(AtanorGlobal* g, short n, short t, bool p, bool c, Atanor* parent) : common(c), AtanorVariableDeclaration(g, n, t, p, false, parent) {}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);

};

class AtanorGlobalVariableDeclaration : public AtanorVariableDeclaration {
public:
	bool alreadydeclared;

	AtanorGlobalVariableDeclaration(AtanorGlobal* g, short n, short t, bool p, bool c, Atanor* parent) : alreadydeclared(false), AtanorVariableDeclaration(g, n, t, p, c, parent) {}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);

};

class AtanorThroughVariableDeclaration : public AtanorVariableDeclaration {
public:

	AtanorThroughVariableDeclaration(short n, short t, Atanor* parent) : AtanorVariableDeclaration(NULL, n, t, false, false, parent) {}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);

	void Resetreference(short i = 1) {}

};

class AtanorSystemVariable : public AtanorVariableDeclaration {
public:

	Atanor* value;
	Exporting AtanorSystemVariable(AtanorGlobal* g, Atanor* v, short n, short t);

};

class AtanorSelfDeclarationVariable : public AtanorVariableDeclaration {
public:

	AtanorSelfDeclarationVariable(AtanorGlobal* g, short n, short tid = a_self, Atanor* parent = NULL) : AtanorVariableDeclaration(g, n, tid, false, false, parent) {}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);
	Atanor* Put(Atanor* index, Atanor* value, short idthread);

};

//------------------------------------------------
class AtanorCallVariable : public AtanorTracked {
public:
	Atanor* call;

	short name;
	short typevariable;

	bool affectation;
	bool forced;
	
	AtanorCallVariable(short n, short idt, AtanorGlobal* g = NULL, Atanor* parent = NULL) : forced(false), affectation(false), typevariable(idt), name(n), call(NULL), AtanorTracked(a_variable, g, parent) {}

	void Setforced(bool v) {
		forced = v;
	}

	virtual short Action() {
		if (call != NULL)
			return a_none;
		return a_variable;
	}
	
	bool isFloat() {
		if (typevariable == a_float || typevariable == a_decimal)
			return true;
		return false;
	}

	bool isString() {
		if (typevariable == a_string || typevariable == a_ustring)
			return true;
		return false;
	}

	bool isNumber() {
		switch (typevariable) {
		case a_int:
		case a_long:
		case a_short:
		case a_decimal:
		case a_float:
			return true;
		}
		return false;
	}

    virtual void ScanVariables(vector<short>& vars) {
        vars.push_back(name);
    }

	bool isFunction() {
		if (typevariable == a_call)
			return true;
		return false;
	}

	void AddInstruction(Atanor* a) {
		if (call == NULL)
			call = a;
		else
			call->AddInstruction(a);
	}

	void Setaffectation(bool b) {
		affectation = b;
	}

	bool isAffectation() {
		return affectation;
	}

	short Typevariable() {
		return typevariable;
	}

	short Typeinfered() {
		if (call != NULL && !call->isIndex() && !call->isIncrement())
			return call->Typeinfered();
		return typevariable;
	}

	bool isCallVariable() {
		return true;
	}

	bool isPureCallVariable() {
		if (call == NULL)
			return true;
		return false;
	}

	bool isAssignable() {
		return true;
	}

	bool Checkvariable() {
		return true;
	}

	bool isFrame() {
		if (!globalAtanor->newInstance.check(typevariable))
			return false;
		return globalAtanor->newInstance[typevariable]->isFrame();
	}

	Atanor* Frame() {
		if (!globalAtanor->newInstance.check(typevariable))
			return NULL;
		return globalAtanor->newInstance[typevariable]->Frame();
	}

	Atanor* Function() {
		return call;
	}
    
    bool Setstopindex() {
        if (call !=NULL)
            call->Setstopindex();
        return false;
    }

    Atanor* Getindex() {
        if (call != NULL)
            return call->Getindex();
        return NULL;
    }

	short Name() {
		return name;
	}

	virtual Atanor* Get(Atanor* context, Atanor* value, short idthread);
	virtual Atanor* Put(Atanor* context, Atanor* value, short idthread);

	Atanor* Body(short idthread) {
		return Get(aNULL, aNULL, idthread)->Body(idthread);
	}

	virtual long Getinteger(short idthread) {
		if (call == NULL)
			return globalAtanor->threads[idthread].variables.get(name).back()->Integer();

		Atanor* a = Get(aNULL, aNULL, idthread);
		long v = a->Integer();
		a->Release();
		return v;
	}

	virtual BLONG Getlong(short idthread) {
		if (call == NULL)
			return globalAtanor->threads[idthread].variables.get(name).back()->Long();

		Atanor* a = Get(aNULL, aNULL, idthread);
		BLONG v = a->Long();
		a->Release();
		return v;
	}

	virtual short Getshort(short idthread) {
		if (call == NULL)
			return globalAtanor->threads[idthread].variables.get(name).back()->Short();

		Atanor* a = Get(aNULL, aNULL, idthread);
		short v = a->Short();
		a->Release();
		return v;
	}

	virtual float Getdecimal(short idthread) {
		if (call == NULL)
			return globalAtanor->threads[idthread].variables.get(name).back()->Decimal();

		Atanor* a = Get(aNULL, aNULL, idthread);
		float v = a->Decimal();
		a->Release();
		return v;
	}

	virtual double Getfloat(short idthread) {
		if (call == NULL)
			return globalAtanor->threads[idthread].variables.get(name).back()->Float();

		Atanor* a = Get(aNULL, aNULL, idthread);
		double v = a->Float();
		a->Release();
		return v;
	}

	virtual string Getstring(short idthread) {
		if (call == NULL)
			return globalAtanor->threads[idthread].variables.get(name).back()->String();

		Atanor* a = Get(aNULL, aNULL, idthread);
		string v = a->String();
		a->Release();
		return v;
	}

	virtual wstring Getustring(short idthread) {
		if (call == NULL)
			return globalAtanor->threads[idthread].variables.get(name).back()->UString();

		Atanor* a = Get(aNULL, aNULL, idthread);
		wstring v = a->UString();
		a->Release();
		return v;
	}


};
//----------------------------------------------------------------------
class AtanorCallConstantVariable : public AtanorCallVariable {
public:

	AtanorCallConstantVariable(short n, short idt, AtanorGlobal* g, Atanor* parent) : AtanorCallVariable(n, idt, g, parent) {}

	Atanor* Get(Atanor* context, Atanor* value, short idthread);
	Atanor* Put(Atanor* context, Atanor* value, short idthread) {
		return aNULL;
	}

	long Getinteger(short idthread) {
		return Get(aNULL, aNULL, idthread)->Integer();
	}

	BLONG Getlong(short idthread) {
		return Get(aNULL, aNULL, idthread)->Long();
	}

	short Getshort(short idthread) {
		return Get(aNULL, aNULL, idthread)->Short();
	}

	float Getdecimal(short idthread) {
		return Get(aNULL, aNULL, idthread)->Decimal();
	}

	double Getfloat(short idthread) {
		return Get(aNULL, aNULL, idthread)->Float();
	}

	string Getstring(short idthread) {
		return Get(aNULL, aNULL, idthread)->String();
	}

	wstring Getustring(short idthread) {
		return Get(aNULL, aNULL, idthread)->UString();
	}

	short Action() {
		return a_none;
	}
};

class AtanorCallThroughVariable : public AtanorCallVariable {
public:

	AtanorCallThroughVariable(short n, short idt, Atanor* parent = NULL) : AtanorCallVariable(n, idt, NULL, parent) {}

	Atanor* Get(Atanor* context, Atanor* value, short idthread);

	long Getinteger(short idthread) {
		return Get(aNULL, aNULL, idthread)->Integer();
	}

	BLONG Getlong(short idthread) {
		return Get(aNULL, aNULL, idthread)->Long();
	}

	short Getshort(short idthread) {
		return Get(aNULL, aNULL, idthread)->Short();
	}

	float Getdecimal(short idthread) {
		return Get(aNULL, aNULL, idthread)->Decimal();
	}

	double Getfloat(short idthread) {
		return Get(aNULL, aNULL, idthread)->Float();
	}

	string Getstring(short idthread) {
		return Get(aNULL, aNULL, idthread)->String();
	}

	wstring Getustring(short idthread) {
		return Get(aNULL, aNULL, idthread)->UString();
	}

	short Action() {
		return a_none;
	}


};

//----------------------------------------------------------------------

class AtanorCallThis : public AtanorCallVariable {
public:

	AtanorCallThis(short idt, AtanorGlobal* g, Atanor* parent) : AtanorCallVariable(a_this, a_frame, g, parent) {}

	Atanor* Get(Atanor* context, Atanor* value, short idthread);

	long Getinteger(short idthread) {
		if (call == NULL)
			return globalAtanor->Getdefinition(a_this, idthread)->Integer();

		Atanor* a = globalAtanor->Getdefinition(a_this, idthread);
		a = call->Get(aNULL, a, idthread);
		long v = a->Integer();
		a->Release();
		return v;
	}

	BLONG Getlong(short idthread) {
		if (call == NULL)
			return globalAtanor->Getdefinition(a_this, idthread)->Long();

		Atanor* a = globalAtanor->Getdefinition(a_this, idthread);
		a = call->Get(aNULL, a, idthread);
		BLONG v = a->Long();
		a->Release();
		return v;
	}

	short Getshort(short idthread) {
		if (call == NULL)
			return globalAtanor->Getdefinition(a_this, idthread)->Short();
		
		Atanor* a = globalAtanor->Getdefinition(a_this, idthread);
		a = call->Get(aNULL, a, idthread);
		short v = a->Short();
		a->Release();
		return v;
	}

	float Getdecimal(short idthread) {
		if (call == NULL)
			return globalAtanor->Getdefinition(a_this, idthread)->Decimal();
		
		Atanor* a = globalAtanor->Getdefinition(a_this, idthread);
		a = call->Get(aNULL, a, idthread);
		float v = a->Decimal();
		a->Release();
		return v;
	}

	double Getfloat(short idthread) {
		if (call == NULL)
			return globalAtanor->Getdefinition(a_this, idthread)->Float();
		
		Atanor* a = globalAtanor->Getdefinition(a_this, idthread);
		a = call->Get(aNULL, a, idthread);
		double v = a->Float();
		a->Release();
		return v;
	}

	string Getstring(short idthread) {
		if (call == NULL)
			return globalAtanor->Getdefinition(a_this, idthread)->String();
		
		Atanor* a = globalAtanor->Getdefinition(a_this, idthread);
		a = call->Get(aNULL, a, idthread);
		string v = a->String();
		a->Release();
		return v;
	}

	wstring Getustring(short idthread) {
		if (call == NULL)
			return globalAtanor->Getdefinition(a_this, idthread)->UString();
		
		Atanor* a = globalAtanor->Getdefinition(a_this, idthread);
		a = call->Get(aNULL, a, idthread);
		wstring v = a->UString();
		a->Release();
		return v;
	}

	short Action() {
		return a_none;
	}

};

class AtanorCallFrameVariable : public AtanorCallVariable {
public:
	AtanorFrame* frame;

	AtanorCallFrameVariable(short n, AtanorFrame* f, short idt, AtanorGlobal* g = NULL, Atanor* parent = NULL) :
		frame(f), AtanorCallVariable(n, idt, g, parent) {}

	Atanor* Get(Atanor* context, Atanor* value, short idthread);
	Atanor* Put(Atanor* context, Atanor* value, short idthread);

	short Action() {
		return a_none;
	}
};

class AtanorCallFromFrameVariable : public AtanorCallVariable {
public:
	AtanorCallFromFrameVariable(short n, short idt, AtanorGlobal* g = NULL, Atanor* parent = NULL) :
		AtanorCallVariable(n, idt, g, parent) {}

    Atanor* Get(Atanor* context, Atanor* value, short idthread);
    Atanor* Put(Atanor* context, Atanor* value, short idthread);
    
	long Getinteger(short idthread) {
		Atanor* a = Get(aNULL, aNULL, idthread);
		long v = a->Integer();
		a->Release();
		return v;
	}

	BLONG Getlong(short idthread) {
		Atanor* a = Get(aNULL, aNULL, idthread);
		BLONG v = a->Long();
		a->Release();
		return v;
	}

	short Getshort(short idthread) {
		Atanor* a = Get(aNULL, aNULL, idthread);
		short v = a->Short();
		a->Release();
		return v;
	}

	float Getdecimal(short idthread) {
		Atanor* a = Get(aNULL, aNULL, idthread);
		float v = a->Decimal();
		a->Release();
		return v;
	}

	double Getfloat(short idthread) {
		Atanor* a = Get(aNULL, aNULL, idthread);
		double v = a->Float();
		a->Release();
		return v;
	}

	string Getstring(short idthread) {
		Atanor* a = Get(aNULL, aNULL, idthread);
		string v = a->String();
		a->Release();
		return v;
	}

	wstring Getustring(short idthread) {
		Atanor* a = Get(aNULL, aNULL, idthread);
		wstring v = a->UString();
		a->Release();
		return v;
	}
	short Action() {
		return a_none;
	}

};

class AtanorCallAtanorVariable : public AtanorCallVariable {
public:
	Atanoratanor* aa;

	AtanorCallAtanorVariable(short n, Atanoratanor* f, AtanorGlobal* g = NULL, Atanor* parent = NULL);
	Atanor* Get(Atanor* context, Atanor* value, short idthread);

	short Action() {
		return a_none;
	}

};

class AtanorCallFunctionVariable : public AtanorCallVariable {
public:

	AtanorCallFunctionVariable(short n, short idt, AtanorGlobal* g = NULL, Atanor* parent = NULL) : AtanorCallVariable(n, idt, g, parent) {}

	Atanor* Get(Atanor* context, Atanor* value, short idthread);

};

class AtanorCallSelfVariable : public AtanorCallVariable {
public:

	AtanorCallSelfVariable(short n, short tid, AtanorGlobal* g = NULL, Atanor* parent = NULL) : AtanorCallVariable(n, tid, g, parent) {}


	Atanor* Get(Atanor* context, Atanor* value, short idthread);
	Atanor* Put(Atanor* context, Atanor* value, short idthread);

	long Getinteger(short idthread) {
		Atanor* a = Get(aNULL, aNULL, idthread);
		long v = a->Integer();
		a->Release();
		return v;
	}

	BLONG Getlong(short idthread) {
		Atanor* a = Get(aNULL, aNULL, idthread);
		BLONG v = a->Long();
		a->Release();
		return v;
	}

	short Getshort(short idthread) {
		Atanor* a = Get(aNULL, aNULL, idthread);
		short v = a->Short();
		a->Release();
		return v;
	}

	float Getdecimal(short idthread) {
		Atanor* a = Get(aNULL, aNULL, idthread);
		float v = a->Decimal();
		a->Release();
		return v;
	}

	double Getfloat(short idthread) {
		Atanor* a = Get(aNULL, aNULL, idthread);
		double v = a->Float();
		a->Release();
		return v;
	}

	string Getstring(short idthread) {
		Atanor* a = Get(aNULL, aNULL, idthread);
		string v = a->String();
		a->Release();
		return v;
	}

	wstring Getustring(short idthread) {
		Atanor* a = Get(aNULL, aNULL, idthread);
		wstring v = a->UString();
		a->Release();
		return v;
	}

};

class AtanorCallSystemVariable : public AtanorCallVariable {
public:

	AtanorCallSystemVariable(short n, short t, AtanorGlobal* g, Atanor* parent) : AtanorCallVariable(n, t, g, parent) {}

	Atanor* Get(Atanor* context, Atanor* value, short idthread) {
		value = globalAtanor->systems[name]->value;

		if (call != NULL)
			return call->Get(context, value, idthread);

		return value;
	}

	long Getinteger(short idthread) {
		Atanor* value = globalAtanor->systems[name]->value;

		if (call != NULL)
			return call->Getinteger(idthread);

		return value->Integer();
	}

	BLONG Getlong(short idthread) {
		Atanor* value = globalAtanor->systems[name]->value;

		if (call != NULL)
			return call->Getlong(idthread);

		return value->Long();
	}

	short Getshort(short idthread) {
		Atanor* value = globalAtanor->systems[name]->value;

		if (call != NULL)
			return call->Getshort(idthread);

		return value->Short();
	}

	float Getdecimal(short idthread) {
		Atanor* value = globalAtanor->systems[name]->value;

		if (call != NULL)
			return call->Getdecimal(idthread);

		return value->Decimal();
	}

	double Getfloat(short idthread) {
		Atanor* value = globalAtanor->systems[name]->value;

		if (call != NULL)
			return call->Getfloat(idthread);

		return value->Float();
	}

	string Getstring(short idthread) {
		Atanor* value = globalAtanor->systems[name]->value;

		if (call != NULL)
			return call->Getstring(idthread);

		return value->String();
	}

	wstring Getustring(short idthread) {
		Atanor* value = globalAtanor->systems[name]->value;

		if (call != NULL)
			return call->Getustring(idthread);

		return value->UString();
	}

	short Action() {
		return a_none;
	}

};

//----------------------------------------------------------------------
class AtanorBreak : public AtanorTracked {
public:

	AtanorBreak(AtanorGlobal* g = NULL, Atanor* parent = NULL) : AtanorTracked(a_instructions, g, parent) {}
	Atanor* Get(Atanor* context, Atanor* value, short idthread) {
		return aBREAK;
	}

	bool needFullInvestigate() {
		return true;
	}

	bool needInvestigate() {
		return true;
	}
};

class AtanorContinue : public AtanorTracked {
public:

	AtanorContinue(AtanorGlobal* g = NULL, Atanor* parent = NULL) : AtanorTracked(a_instructions, g, parent) {}

	Atanor* Get(Atanor* context, Atanor* value, short idthread) {
		return aCONTINUE;
	}

	bool needFullInvestigate() {
		return true;
	}

};

//----------------------------------------------------------------------
class AtanorInstructionAFFECTATION : public AtanorInstruction {
public:

	AtanorInstructionAFFECTATION(AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_instructions, g, parent) {}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);

	string String() {
		string v = instructions[0]->String();
		v += "=";
		v += instructions[1]->String();
		return v;
	}
};

class AtanorInstructionSTREAM : public AtanorInstructionAFFECTATION {
public:
    
    AtanorInstructionSTREAM(AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstructionAFFECTATION(g, parent) {
        idtype=a_stream;
    }
    
    Atanor* Get(Atanor* context, Atanor* value, short idthread);
};

class AtanorInstructionSIMPLEAFFECTATION : public AtanorInstruction {
public:

	AtanorInstructionSIMPLEAFFECTATION(AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_affectation, g, parent) {}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);

	string String() {
		string v = instructions[0]->String();
		v += "=";
		v += instructions[1]->String();
		return v;
	}
};


//----------------------------------------------------------------------

class AtanorAction : public Atanor {
public:

	short action;

	AtanorAction(short op) : action(op) {
		globalAtanor->RecordInTracker(this);
	}

	short Action() {
		return action;
	}

	string String() {
		return globalAtanor->Getsymbol(action);
	}

};

class AtanorActionVariable : public Atanor {
public:

	short name;
	short typevar;

	AtanorActionVariable(short n, short t) : name(n), typevar(t) {
		globalAtanor->RecordInTracker(this);
	}

	short Name() {
		return name;
	}

	virtual Atanor* Get(Atanor* context, Atanor* value, short idthread) {
		return globalAtanor->Getdeclaration(name, idthread);
	}

	short Action() {
		return a_variable;
	}

	short Typeinfered() {
		return typevar;
	}

	short Type() {
		return a_actionvariable;
	}


	virtual long Getinteger(short idthread) {
		return globalAtanor->threads[idthread].variables.get(name).back()->Integer();
	}

	virtual BLONG Getlong(short idthread) {
		return globalAtanor->threads[idthread].variables.get(name).back()->Long();
	}

	virtual short Getshort(short idthread) {
		return globalAtanor->threads[idthread].variables.get(name).back()->Short();
	}

	virtual float Getdecimal(short idthread) {
		return globalAtanor->threads[idthread].variables.get(name).back()->Decimal();
	}

	virtual double Getfloat(short idthread) {
		return globalAtanor->threads[idthread].variables.get(name).back()->Float();
	}

	virtual string Getstring(short idthread) {
		return globalAtanor->threads[idthread].variables.get(name).back()->String();
	}

	virtual wstring Getustring(short idthread) {
		return globalAtanor->threads[idthread].variables.get(name).back()->UString();
	}

};

class AtanorActionLetVariable : public AtanorActionVariable {
public:

	AtanorActionLetVariable(short n, short t) : AtanorActionVariable(n, t) {}

	Atanor* Get(Atanor* context, Atanor* value, short idthread) {
		return globalAtanor->threads[idthread].variables.get(name).back()->Value();
	}

	long Getinteger(short idthread) {
		return globalAtanor->threads[idthread].variables.get(name).back()->Value()->Integer();
	}

	BLONG Getlong(short idthread) {
		return globalAtanor->threads[idthread].variables.get(name).back()->Value()->Long();
	}

	short Getshort(short idthread) {
		return globalAtanor->threads[idthread].variables.get(name).back()->Value()->Short();
	}

	float Getdecimal(short idthread) {
		return globalAtanor->threads[idthread].variables.get(name).back()->Value()->Decimal();
	}

	double Getfloat(short idthread) {
		return globalAtanor->threads[idthread].variables.get(name).back()->Value()->Float();
	}

	string Getstring(short idthread) {
		return globalAtanor->threads[idthread].variables.get(name).back()->Value()->String();
	}

	wstring Getustring(short idthread) {
		return globalAtanor->threads[idthread].variables.get(name).back()->Value()->UString();
	}

};


class AtanorInstructionAPPLYOPERATIONROOT : public AtanorInstruction {
public:
	
	basebin_hash<AtanorActionVariable*> variables;

	long size;
	uchar thetype;
	uchar head;
	uchar alls;
	bool sub;

	AtanorInstructionAPPLYOPERATIONROOT(AtanorGlobal* g, Atanor* parent = NULL, short a_i = a_instructions) : thetype(0), head(0), alls(0), AtanorInstruction(a_i, g, parent) {
		size = 0; 
		sub = false;
	}

	bool isROOTOPERATION() {
		return true;
	}

	Atanor* Newinstance(short idthread, Atanor* f = NULL) {
		AtanorInstructionAPPLYOPERATIONROOT* root = new AtanorInstructionAPPLYOPERATIONROOT(NULL);
		root->instructions = instructions;
		root->size = size;
		return root;
	}

	void Setsize() {
		size = instructions.size();		
	}

	long Size() {
		return size;
	}
	
	bool Stacking(Atanor* ins, char top);

	uchar Evaluate(Atanor* ins);
	AtanorInstruction* Returnlocal(AtanorGlobal* g, Atanor* parent = NULL);

	string String() {
		string v;
		for (long i = size - 1; i >= 0; i--) {
			if (i < size - 1)
				v += " ";
			if (instructions[i] == this)
				v += globalAtanor->Getsymbol(action);
			else
				v += instructions[i]->String();
		}
		return v;
	}

	long Getinteger(short idthread) {
		short d = size - 1;
		Atanor* r = ccompute(idthread, true, d);

		if (d == DIVIDEDBYZERO) {
			globalAtanor->Errorobject(idthread);
			return 0;
		}

		long w = r->Integer();
		r->Release();
		return w;
	}

	BLONG Getlong(short idthread) {
		short d = size - 1;
		Atanor* r = ccompute(idthread, true, d);

		if (d == DIVIDEDBYZERO) {
			globalAtanor->Errorobject(idthread);
			return 0;
		}

		BLONG w = r->Long();
		r->Release();
		return w;
	}

	short Getshort(short idthread) {
		short d = size - 1;
		Atanor* r = ccompute(idthread, true, d);

		if (d == DIVIDEDBYZERO) {
			globalAtanor->Errorobject(idthread);
			return 0;
		}

		short w = r->Short();
		r->Release();
		return w;
	}

	float Getdecimal(short idthread) {
		short d = size - 1;
		Atanor* r = ccompute(idthread, true, d);

		if (d == DIVIDEDBYZERO) {
			globalAtanor->Errorobject(idthread);
			return 0;
		}

		float w = r->Decimal();
		r->Release();
		return w;
	}

	double Getfloat(short idthread) {
		short d = size - 1;
		Atanor* r = ccompute(idthread, true, d);

		if (d == DIVIDEDBYZERO) {
			globalAtanor->Errorobject(idthread);
			return 0;
		}

		double w = r->Float();
		r->Release();
		return w;
	}

	string Getstring(short idthread) {
		short d = size - 1;
		Atanor* r = ccompute(idthread, true, d);

		if (d == DIVIDEDBYZERO) {
			globalAtanor->Errorobject(idthread);
			return "";
		}

		string w = r->String();
		r->Release();
		return w;
	}

	wstring Getustring(short idthread) {
		short d = size - 1;
		Atanor* r = ccompute(idthread, true, d);

		if (d == DIVIDEDBYZERO) {
			globalAtanor->Errorobject(idthread);
			return L"";
		}

		wstring w = r->UString();
		r->Release();
		return w;
	}

	Atanor* Get(Atanor* res, Atanor* r, short idthread) {
		bool store = true;
		if (r != aAFFECTATION)
			store = false;

		short d = size - 1;

		r = ccompute(idthread, true, d);

		if (d == DIVIDEDBYZERO) 
			return globalAtanor->Errorobject(idthread);

		if (store) {			
			res->Put(aNULL, r, idthread);
			r->Release();
			return res;
		}

		return r;
	}

	Atanor* ccompute(short idthread, uchar top, short& d);

	bool isOperation() {
		return false;
	}

};

class AtanorInstructionAPPLYOPERATIONEQU : public AtanorInstructionAPPLYOPERATIONROOT {
public:

	Atanor* recipient;
	AtanorInstruction* instruction;

	AtanorInstructionAPPLYOPERATIONEQU(AtanorGlobal* g, Atanor* parent = NULL) : recipient(aNULL), AtanorInstructionAPPLYOPERATIONROOT(g, parent, a_instructionequ) {}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);

	bool isEQU() {
		return true;
	}
};


class AtanorInstructionSTRING : public AtanorInstruction {
public:

	long size;

	AtanorInstructionSTRING(AtanorInstruction* r, AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_instructions, g, parent) {
		instructions = r->instructions;
		size = r->Size();
	}

	long Size() {
		return size;
	}

	Atanor* Newinstance(short idthread, Atanor* f = NULL) {
		return new AtanorInstructionSTRING(this, NULL);
	}

	string Getstring(short idthread) {
		short d = size - 1;
		string r;
		if (!cstring(idthread, d, r))
			globalAtanor->Returnerror("Unknown operation on strings", idthread);
		return r;
	}

	Atanor* Get(Atanor* res, Atanor* inter, short idthread) {
		short d = size - 1;
		string r;
		if (!cstring(idthread, d, r))
			return globalAtanor->Returnerror("Unknown operation on strings", idthread);

		if (inter == aAFFECTATION) {
			res->storevalue(r);
			return res;
		}

		return globalAtanor->Providestring(r);
	}

	bool cstring(short idthread, short& d, string& r);

	bool isOperation() {
		return false;
	}

	string String() {
		string v;
		for (long i = size - 1; i >= 0; i--) {
			if (i < size - 1)
				v += " ";
			if (instructions[i] == this)
				v += globalAtanor->Getsymbol(action);
			else
				v += instructions[i]->String();
		}
		return v;
	}


};



class AtanorInstructionSHORT : public AtanorInstruction {
public:

	long size;

	AtanorInstructionSHORT(AtanorInstruction* r, AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_instructions, g, parent) {
		instructions = r->instructions;
		size = r->Size();
	}

	long Size() {
		return size;
	}

	Atanor* Newinstance(short idthread, Atanor* f = NULL) {
		return new AtanorInstructionSHORT(this, NULL);		
	}

	short Getshort(short idthread) {
		short d = size - 1;
		short r = cshort(idthread, d);
		if (d == DIVIDEDBYZERO)
			globalAtanor->Errorobject(idthread);
		return r;
	}

	Atanor* Get(Atanor* res, Atanor* inter, short idthread) {
		short d = size - 1;
		short r = cshort(idthread, d);
		if (d == DIVIDEDBYZERO)
			return globalAtanor->Errorobject(idthread);

		if (inter == aAFFECTATION) {
			res->storevalue(r);
			return res;
		}

		return new Atanorshort(r);
	}

	short cshort(short idthread, short& d);
	bool isOperation() {
		return false;
	}

	string String() {
		string v;
		for (long i = size - 1; i >= 0; i--) {
			if (i < size - 1)
				v += " ";
			if (instructions[i] == this)
				v += globalAtanor->Getsymbol(action);
			else
				v += instructions[i]->String();
		}
		return v;
	}


};



class AtanorInstructionLONG : public AtanorInstruction {
public:

	long size;

	AtanorInstructionLONG(AtanorInstruction* r, AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_instructions, g, parent) {
		instructions = r->instructions;
		size = r->Size();
	}

	long Size() {
		return size;
	}

	Atanor* Newinstance(short idthread, Atanor* f = NULL) {
		return new AtanorInstructionLONG(this, NULL);
	}

	BLONG Getlong(short idthread) {
		short d = size - 1;
		BLONG r = clong(idthread, d);
		if (d == DIVIDEDBYZERO)
			globalAtanor->Errorobject(idthread);
		return r;
	}

	Atanor* Get(Atanor* res, Atanor* inter, short idthread) {
		short d = size - 1;
		BLONG r = clong(idthread, d);
		if (d == DIVIDEDBYZERO)
			return globalAtanor->Errorobject(idthread);

		if (inter == aAFFECTATION) {
			res->storevalue(r);
			return res;
		}

		return new Atanorlong(r);
	}

	BLONG clong(short idthread, short& d);
	bool isOperation() {
		return false;
	}

	string String() {
		string v;
		for (long i = size - 1; i >= 0; i--) {
			if (i < size - 1)
				v += " ";
			if (instructions[i] == this)
				v += globalAtanor->Getsymbol(action);
			else
				v += instructions[i]->String();
		}
		return v;
	}


};



class AtanorInstructionUSTRING : public AtanorInstruction {
public:

	long size;

	AtanorInstructionUSTRING(AtanorInstruction* r, AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_instructions, g, parent) {
		instructions = r->instructions;
		size = r->Size();
	}

	long Size() {
		return size;
	}

	Atanor* Newinstance(short idthread, Atanor* f = NULL) {
		return new AtanorInstructionUSTRING(this, NULL);
	}

	wstring Getustring(short idthread) {
		short d = size - 1;
		wstring r;
		if (!custring(idthread, d, r))
			globalAtanor->Returnerror("Unknown operation on strings", idthread);
		return r;
	}

	Atanor* Get(Atanor* res, Atanor* inter, short idthread) {
		short d = size - 1;
		wstring r;
		if (!custring(idthread, d, r))
			return globalAtanor->Returnerror("Unknown operation on strings", idthread);

		if (inter == aAFFECTATION) {
			res->storevalue(r);
			return res;
		}

		return globalAtanor->Provideustring(r);
	}

	bool custring(short idthread, short& d, wstring& r);
	bool isOperation() {
		return false;
	}

	string String() {
		string v;
		for (long i = size - 1; i >= 0; i--) {
			if (i < size - 1)
				v += " ";
			if (instructions[i] == this)
				v += globalAtanor->Getsymbol(action);
			else
				v += instructions[i]->String();
		}
		return v;
	}


};


class AtanorInstructionINTEGER : public AtanorInstruction {
public:

	long size;

	AtanorInstructionINTEGER(AtanorInstruction* r, AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_instructions, g, parent) {
		instructions = r->instructions;
		size = r->Size();
	}

	long Size() {
		return size;
	}

	Atanor* Newinstance(short idthread, Atanor* f = NULL) {
		return new AtanorInstructionINTEGER(this, NULL);
	}

	long Getinteger(short idthread) {
		short d = size - 1;
		long r = cinteger(idthread, d);
		if (d == DIVIDEDBYZERO)
			globalAtanor->Errorobject(idthread);
		return r;
	}

	Atanor* Get(Atanor* res, Atanor* inter, short idthread) {
		short d = size - 1;
		long r = cinteger(idthread, d);
		if (d == DIVIDEDBYZERO)
			return globalAtanor->Errorobject(idthread);

		if (inter == aAFFECTATION) {
			res->storevalue(r);
			return res;
		}

		return globalAtanor->Provideint(r);
	}

	long cinteger(short idthread, short& d);
	bool isOperation() {
		return false;
	}

	string String() {
		string v;
		for (long i = size - 1; i >= 0; i--) {
			if (i < size - 1)
				v += " ";
			if (instructions[i] == this)
				v += globalAtanor->Getsymbol(action);
			else
				v += instructions[i]->String();
		}
		return v;
	}


};



class AtanorInstructionDECIMAL : public AtanorInstruction {
public:

	long size;

	AtanorInstructionDECIMAL(AtanorInstruction* r, AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_instructions, g, parent) {
		instructions = r->instructions;
		size = r->Size();
	}


	long Size() {
		return size;
	}

	Atanor* Newinstance(short idthread, Atanor* f = NULL) {
		return new AtanorInstructionDECIMAL(this, NULL);
	}

	float Getdecimal(short idthread) {
		short d = size - 1;
		float r = cdecimal(idthread, d);
		if (d == DIVIDEDBYZERO)
			globalAtanor->Errorobject(idthread);
		return r;
	}

	Atanor* Get(Atanor* res, Atanor* inter, short idthread) {
		short d = size - 1;
		float r = cdecimal(idthread, d);
		if (d == DIVIDEDBYZERO)
			return globalAtanor->Errorobject(idthread);

		if (inter == aAFFECTATION) {
			res->storevalue(r);
			return res;
		}

		return new Atanordecimal(r);
	}

	float cdecimal(short idthread, short& d);
	bool isOperation() {
		return false;
	}

	string String() {
		string v;
		for (long i = size - 1; i >= 0; i--) {
			if (i < size - 1)
				v += " ";
			if (instructions[i] == this)
				v += globalAtanor->Getsymbol(action);
			else
				v += instructions[i]->String();
		}
		return v;
	}


};



class AtanorInstructionFLOAT : public AtanorInstruction {
public:

	long size;

	AtanorInstructionFLOAT(AtanorInstruction* r, AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_instructions, g, parent) {
		instructions = r->instructions;
		size = r->Size();
	}

	long Size() {
		return size;
	}

	Atanor* Newinstance(short idthread, Atanor* f = NULL) {
		return new AtanorInstructionFLOAT(this, NULL);
	}

	double Getfloat(short idthread) {
		short d = size - 1;
		double r = cfloat(idthread, d);
		if (d == DIVIDEDBYZERO)
			globalAtanor->Errorobject(idthread);
		return r;
	}

	Atanor* Get(Atanor* res, Atanor* inter, short idthread) {
		short d = size - 1;
		double r = cfloat(idthread, d);
		if (d == DIVIDEDBYZERO)
			return globalAtanor->Errorobject(idthread);

		if (inter == aAFFECTATION) {
			res->storevalue(r);
			return res;
		}

		return globalAtanor->Providefloat(r);
	}

	double cfloat(short idthread, short& d);
	bool isOperation() {
		return false;
	}

	string String() {
		string v;
		for (long i = size - 1; i >= 0; i--) {
			if (i < size - 1)
				v += " ";
			if (instructions[i] == this)
				v += globalAtanor->Getsymbol(action);
			else
				v += instructions[i]->String();
		}
		return v;
	}

};



class AtanorInstructionAPPLYOPERATION : public AtanorInstruction {
public:
	AtanorInstructionAPPLYOPERATIONROOT* root;
	bool subcontext;

	AtanorInstructionAPPLYOPERATION(AtanorGlobal* g, Atanor* parent = NULL, short ins = a_instructions) :
		AtanorInstruction(ins, g, parent), subcontext(false), root(NULL) {
	}

	void Subcontext(bool b) { subcontext = b; }

	bool Subcontext() {
		return subcontext;
	}

	bool isOperation() {
		return true;
	}

	Atanor* Get(Atanor* context, Atanor* value, short idthread);
};


class AtanorInstructionCOMPARE : public AtanorInstruction {
public:

	AtanorInstructionCOMPARE(AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_instructions, g, parent) {}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);
};

class AtanorPLUSPLUS : public AtanorTracked {
public:

	AtanorPLUSPLUS(AtanorGlobal* g, Atanor* parent = NULL) : AtanorTracked(a_plusplus, g, parent) {}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);
	bool isIncrement() {
		return true;
	}

	string String() {
		return "++";
	}
};

class AtanorMINUSMINUS : public AtanorTracked {
public:

	AtanorMINUSMINUS(AtanorGlobal* g, Atanor* parent = NULL) : AtanorTracked(a_minusminus, g, parent) {}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);
	bool isIncrement() {
		return true;
	}

	string String() {
		return "--";
	}

};

class AtanorSQUARE : public AtanorTracked {
public:

	AtanorSQUARE(AtanorGlobal* g, Atanor* parent = NULL) : AtanorTracked(a_minusminus, g, parent) {}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);
	bool isIncrement() {
		return true;
	}

	string String() {
		return "²";
	}
};

class AtanorCUBE : public AtanorTracked {
public:

	AtanorCUBE(AtanorGlobal* g, Atanor* parent = NULL) : AtanorTracked(a_minusminus, g, parent) {}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);
	bool isIncrement() {
		return true;
	}

	string String() {
		return "³";
	}
};

class AtanorInstructionOR : public AtanorInstruction {
public:

	AtanorInstructionOR(AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_instructions, g, parent) {}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);
};

class AtanorInstructionAND : public AtanorInstruction {
public:

	AtanorInstructionAND(AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_instructions, g, parent) {}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);
};

class AtanorInstructionIN : public AtanorInstruction {
public:

	AtanorInstructionIN(AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_instructions, g, parent) {}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);
};


class AtanorInstructionIF : public AtanorInstruction {
public:

	AtanorInstructionIF(AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_instructions, g, parent) {
		idtype = a_if;
	}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);
};

class AtanorInstructionFOR : public AtanorSequence {
public:

	AtanorInstructionFOR(AtanorGlobal* g, Atanor* parent = NULL) : AtanorSequence(g, parent) {}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);
};

class AtanorInstructionSWITCH : public AtanorInstruction {
public:
	hmap<string, int> keys;
	bool usekeys;


	AtanorInstructionSWITCH(AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_instructions, g, parent) {
		usekeys = false;
	}


	Atanor* Get(Atanor* context, Atanor* value, short idthread);
};

class AtanorInstructionWHILE : public AtanorSequence {
public:

	AtanorInstructionWHILE(AtanorGlobal* g, Atanor* parent = NULL) : AtanorSequence(g, parent) {}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);
};

class AtanorInstructionUNTIL : public AtanorSequence {
public:

	AtanorInstructionUNTIL(AtanorGlobal* g, Atanor* parent = NULL) : AtanorSequence(g, parent) {}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);
};

class AtanorInstructionFILEIN : public AtanorInstruction {
public:

	AtanorInstructionFILEIN(AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_instructions, g, parent) {}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);
};

//-----------------------------------------------------------------------------------------------------------------------------
class AtanorInstructionFORINRANGE : public AtanorInstruction {
public:

	AtanorInstructionFORINRANGE(AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_forinrange, g, parent) {}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);

	Atanor* ExecuteInteger(Atanorint* value, Atanor* context, short idthread);
	Atanor* ExecuteDecimal(Atanordecimal* value, Atanor* context, short idthread);
	Atanor* ExecuteFloat(Atanorfloat* value, Atanor* context, short idthread);
	Atanor* ExecuteLong(Atanorlong* value, Atanor* context, short idthread);
	Atanor* ExecuteShort(Atanorshort* value, Atanor* context, short idthread);

};

class AtanorInstructionFORINRANGESHORT : public AtanorTracked {
public:
	Atanor* recipient;
	Atanor* init;
	Atanor* test;
	Atanor* increment;
	Atanor* instruction;

	AtanorInstructionFORINRANGESHORT(Atanor* r, AtanorInstruction* i, AtanorGlobal* g, Atanor* parent = NULL) : recipient(r), AtanorTracked(a_forinrange, g, parent) {
		init = i->instructions.vecteur[0]->Instruction(1);
		test = i->instructions.vecteur[0]->Instruction(2);
		increment = i->instructions.vecteur[0]->Instruction(3);
		instruction = i->instructions.vecteur[1];
	}

	Atanor* Get(Atanor* context, Atanor* value, short idthread);
};

class AtanorInstructionFORINRANGEINTEGER : public AtanorTracked {
public:
	Atanor* recipient;
	Atanor* init;
	Atanor* test;
	Atanor* increment;
	Atanor* instruction;

	AtanorInstructionFORINRANGEINTEGER(Atanor* r, AtanorInstruction* i, AtanorGlobal* g, Atanor* parent = NULL) : recipient(r), AtanorTracked(a_forinrange, g, parent)  {
		init = i->instructions.vecteur[0]->Instruction(1);
		test = i->instructions.vecteur[0]->Instruction(2);
		increment = i->instructions.vecteur[0]->Instruction(3);
		instruction = i->instructions.vecteur[1];
	}

	Atanor* Get(Atanor* context, Atanor* value, short idthread);
};

class AtanorInstructionFORINRANGELONG : public AtanorTracked {
public:
	Atanor* recipient;
	Atanor* init;
	Atanor* test;
	Atanor* increment;
	Atanor* instruction;

	AtanorInstructionFORINRANGELONG(Atanor* r, AtanorInstruction* i, AtanorGlobal* g, Atanor* parent = NULL) : recipient(r), AtanorTracked(a_forinrange, g, parent)  {
		init = i->instructions.vecteur[0]->Instruction(1);
		test = i->instructions.vecteur[0]->Instruction(2);
		increment = i->instructions.vecteur[0]->Instruction(3);
		instruction = i->instructions.vecteur[1];
	}

	Atanor* Get(Atanor* context, Atanor* value, short idthread);
};

class AtanorInstructionFORINRANGEDECIMAL : public AtanorTracked {
public:
	Atanor* recipient;
	Atanor* init;
	Atanor* test;
	Atanor* increment;
	Atanor* instruction;

	AtanorInstructionFORINRANGEDECIMAL(Atanor* r, AtanorInstruction* i, AtanorGlobal* g, Atanor* parent = NULL) : recipient(r), AtanorTracked(a_forinrange, g, parent)  {
		init = i->instructions.vecteur[0]->Instruction(1);
		test = i->instructions.vecteur[0]->Instruction(2);
		increment = i->instructions.vecteur[0]->Instruction(3);
		instruction = i->instructions.vecteur[1];
	}

	Atanor* Get(Atanor* context, Atanor* value, short idthread);
};

class AtanorInstructionFORINRANGEFLOAT : public AtanorTracked {
public:
	Atanor* recipient;
	Atanor* init;
	Atanor* test;
	Atanor* increment;
	Atanor* instruction;

	AtanorInstructionFORINRANGEFLOAT(Atanor* r, AtanorInstruction* i, AtanorGlobal* g, Atanor* parent = NULL) : recipient(r), AtanorTracked(a_forinrange, g, parent)  {
		init = i->instructions.vecteur[0]->Instruction(1);
		test = i->instructions.vecteur[0]->Instruction(2);
		increment = i->instructions.vecteur[0]->Instruction(3);
		instruction = i->instructions.vecteur[1];
	}

	Atanor* Get(Atanor* context, Atanor* value, short idthread);
};

//-----------------------------------------------------------------------------------------------------------------------------
class AtanorInstructionFORVECTORIN : public AtanorInstruction {
public:

	AtanorInstructionFORVECTORIN(AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_instructions, g, parent) {}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);
};


class AtanorInstructionFORMAPIN : public AtanorInstruction {
public:

	AtanorInstructionFORMAPIN(AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_instructions, g, parent) {}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);
};


class AtanorInstructionFORIN : public AtanorInstruction {
public:

	AtanorInstructionFORIN(AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_instructions, g, parent) {}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);
};


class AtanorInstructionFORINVALUECONTAINER : public AtanorInstruction {
public:

	AtanorInstructionFORINVALUECONTAINER(AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_instructions, g, parent) {}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);
};



class AtanorInstructionTRY : public AtanorInstruction {
public:

	AtanorInstructionTRY(AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_instructions, g, parent) {}
	Atanor* Get(Atanor* context, Atanor* value, short idthread);
};

class AtanorInstructionCATCH : public AtanorInstruction {
public:

	AtanorInstructionCATCH(AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_instructions, g, parent) {
		action = a_catchbloc;
	}

	Atanor* Get(Atanor* context, Atanor* value, short idthread);
};


class AtanorContent : public AtanorObject {
public:
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	AtanorIndex* idx;
	Atanor* container;

	//---------------------------------------------------------------------------------------------------------------------

	AtanorContent(AtanorIndex* i, Atanor* c) {
		//Do not forget your variable initialisation
		idx = i;
		container = c;
	}

	~AtanorContent() {
		idx->Rollback();
	}

	Atanor* Put(Atanor* e, Atanor* v, short idthread) {
		container->Put(idx, v, idthread);
		return aTRUE;
	}

	Atanor* Atom(bool forced = false) {
		return this;
	}

	void Setreference(short r = 1) {
		container->Setreference(r);
		AtanorObject::Setreference(r);
	}

	void Resetreference(short r = 1) {
		container->Resetreference(r);
		AtanorObject::Resetreference(r);
	}

	//If it is too complex to duplicate an element (for instance when passed to a function)
	//then you can use return false...  Usually in that case, Atom should always be just: return this;
	bool duplicateForCall() {
		return false;
	}

};

#endif







