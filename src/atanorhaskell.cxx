
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
filename   : atanorhaskell.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "instructions.h"
#include "compilecode.h"
#include "atanorstring.h"
#include "atanorsvector.h"
#include "constobjects.h"
#include "atanormap.h"
#include "atanorivector.h"
#include "atanorfvector.h"
#include "atanorsvector.h"
#include "atanorhaskell.h"


//------------------------------HASKELL----------------------------------------

bool AtanorCallFunctionHaskell::Checkarity() {
	AtanorFunctionLambda* bd = body;
	while (bd != NULL) {
		if (arguments.size() == bd->parameters.size())
			return true;
		bd = bd->Following();
	}
	return false;
}


Atanor* AtanorFunctionHaskellParameter::Execute(Atanor* context, Atanor* hcall, short idthread) {
	hcall = globalAtanor->Getdefinition(name, idthread, context);

	AtanorCallFunctionHaskell hfunc((AtanorFunctionLambda*)hcall);
	return hfunc.Get(context, aNULL, idthread);
}


Atanor* AtanorFunctionLambda::Get(Atanor* environment, Atanor* a, short idthread) {
	for (short i = 0; i < instructions.last; i++) {

		a = instructions.vecteur[i]->Get(environment, aNULL, idthread);

		if (a->needFullInvestigate())
			return a->Returned(idthread);

		a->Release();
	}

	return aNULL;
}
//------------------------------HASKELL----------------------------------------
Atanor* AtanorInstructionHaskellIF::Get(Atanor* context, Atanor* value, short idthread) {
	value = instructions.vecteur[0]->Get(aTRUE, context, idthread);

	if (value->isError())
		return value;

	bool truevalue = value->Boolean();
	value->Release();

	if (truevalue != negation)
		return instructions.vecteur[1]->Get(context, aNULL, idthread);

	if (instructions.last == 3)
		return instructions.vecteur[2]->Get(context, aNULL, idthread);

	return aNULL;
}

//------------------------------HASKELL----------------------------------------
Atanor* AtanorInstructionHASKELLCASE::Get(Atanor* var, Atanor* context, short idthread) {
	var = instructions[0]->Get(context, aNULL, idthread);
	Atanor* ins = instructions[1];
	if (ins->Type() == a_constvector || ins->Type() == a_constmap) {		
		ins->Prepare(context, idthread);
		context = ins->same(var);
		var->Release();
		return context;
	}

	ins = ins->Get(context, aNULL, idthread);
	context = ins->same(var);
	var->Release();
	ins->Release();
	return context;
}

Atanor* AtanorCallFunctionHaskell::Get(Atanor* context, Atanor* res, short idthread) {
	AtanorDeclarationAutoClean environment(idthread);

	switch (body->lambdadomain.instructions.last) {
	case 0:
		res = body->Get(&environment, aNULL, idthread)->Returned(idthread);
		break;
	case 2:
		res = GetHaskell2(context, &environment, body, idthread);
		break;
	case 3:
		res = GetHaskell3(context, &environment, body, idthread);
		break;
	case 5:
		res = GetHaskell5(context, &environment, body, idthread);
		break;
	default:
		res = GetHaskellN(context, &environment, body, idthread);
	}


	if (res->isError()) {
		environment.Cleaning(idthread);
		return res;
	}

	if (!environment.isEmpty()) {
		if (!res->Reference())
			environment.Cleaning(idthread);
		else {
			res->Setreference();
			environment.Cleaning(idthread);
			res->Protect();
		}
	}

	return res;
}

Atanor* AtanorCallFunctionArgsHaskell::Get(Atanor* context, Atanor* res, short idthread) {
	AtanorFunctionLambda* bd = body;

	short sz = arguments.size();

	short i;
	Atanor** args = new Atanor*[sz];
	for (i = 0; i < sz; i++) {
		args[i] = arguments[i]->Get(context, aHASKELL, idthread);
		args[i]->Setreference();
	}

	AtanorDeclarationAutoClean environment(idthread);

	short param_name;

	while (bd != NULL) {
		if (sz != bd->parameters.size()) {
			bd = bd->Following();
			continue;
		}

		for (i = 0; i < sz; i++) {
			res = bd->parameters[i];

			if (res->Type() == a_constvector || res->Type() == a_constmap) {
				//They could be some variables, that need to be created...
				res->Setevaluate(true);
				res->Prepare(&environment, idthread);

				if (res->Put(aRAISEERROR, args[i], idthread)->isError()) {
					res = aRAISEERROR;
					break;
				}
				continue;
			}

			param_name = res->Name();
			if (!param_name) {//it is a value comparison
				if (args[i]->same(res) == aFALSE) {
					res = aRAISEERROR;
					break;
				}
			}
			else {
				environment.Declare(param_name, args[i]);
				globalAtanor->Storevariable(idthread, param_name, args[i]);
				args[i]->Setreference();
			}
		}
		
		if (res != aRAISEERROR) {
			switch (bd->lambdadomain.instructions.last) {
			case 0: 	
				res = bd->Get(&environment, aNULL, idthread)->Returned(idthread);
				break;
			case 2:
				res = GetHaskell2(context, &environment, bd, idthread);
				break;
			case 3:
				res = GetHaskell3(context, &environment, bd, idthread);
				break;
			case 5:
				res = GetHaskell5(context, &environment, bd, idthread);
				break;
			default:
				res = GetHaskellN(context, &environment, bd, idthread);
			}

			if (!res->isError())
				break;
		}

		if (globalAtanor->Error(idthread)) {
			if (!res->isError())
				res = globalAtanor->Errorobject(idthread);
			break;
		}

		environment.Cleaning(idthread);

		res->Release();
		res = aNULL;
		bd = bd->Following();
	}

	for (i = 0; i < sz; i++) {
		if (args[i] != res)
			args[i]->Resetreference();
	}
	delete[] args;

	if (!environment.isEmpty()) {
		if (!res->Reference())
			environment.Cleaning(idthread);
		else {
			res->Setreference();
			environment.Cleaning(idthread);
			res->Protect();
		}
	}

	return res;
}

//This is the function that is called to process <and ...> and <or...>
//The main difference between the two calls is that when bd return FALSE for a AND it is a fail, while it suffices of one  TRUE for a OR to succeed.
//When a NULL is returned for a AND, it is OK, while it is a FAIL for a OR... Hence, the default value for AND is TRUE while it is FALSE for OR
Exporting Atanor* Atanor::Filterboolean(short idthread, Atanor* env, AtanorFunctionLambda* bd, Atanor* var, Atanor* def) {

	Atanor* returnval;
	AtanorIteration* iter = Newiteration(false);


	for (iter->Begin(); iter->End() == aFALSE; iter->Next()) {
		var->Putvalue(iter->IteratorValue(), idthread);
		returnval = bd->Get(env, aNULL, idthread);

		if (returnval->isError()) {
			iter->Release();
			var->Forcedclean();
			return returnval;
		}

		if (returnval == aFALSE || returnval == aTRUE) {
			def = returnval;
			break;
		}
	}
	
	var->Forcedclean();
	iter->Release();
	return def;
}

//The <and ...> and the <or ...> operators...
Atanor* AtanorCallFunctionHaskell::GetHaskell2(Atanor* context, Atanor* environment, AtanorFunctionLambda* bd, short idthread) {

	Atanor* var = bd->lambdadomain.instructions[0]->Get(environment, aNULL, idthread);
	Atanor* alist = bd->lambdadomain.instructions[1]->Get(environment, aNULL, idthread);

	if (bd->lambdadomain.name == 1) //name==1 is a AND
		context = alist->Filterboolean(idthread, environment, bd, var, aTRUE);
	else
		context = alist->Filterboolean(idthread, environment, bd, var, aFALSE); //It is a OR

	alist->Release();
	var->Release();
	return context;
}

Exporting Atanor* Atanor::Loophaskell(Atanor* recipient, Atanor* context, Atanor* environment, AtanorFunctionLambda* bd, short idthread) {
	AtanorIteration* loop = Newiteration(false);
	if (loop == aITERNULL)
		return context;

	Atanor* a;
	char addvalue = 0;

	if (context != aNOELEMENT)
		addvalue = Selecttype(context);

	while (!loop->End()->Boolean()) {
		a = loop->IteratorValue();
		recipient->Putvalue(a, idthread);

		a = bd->Get(environment, aNULL, idthread);

		if (a->isError()) {
			if (a == aBREAK)
				break;

			delete loop;
			recipient->Forcedclean();
			context->Release();
			return a;
		}

		
		if (a != aNULL) {
			context = Storealongtype(context, a, idthread, addvalue);
			a->Release();
		}
		loop->Next();
	}

	delete loop;
	recipient->Forcedclean();
	return context;

}

Atanor* AtanorCallFunctionHaskell::GetHaskell3(Atanor* context, Atanor* environment, AtanorFunctionLambda* bd, short idthread) {
	//First we create our variables...
	//The first variable is the one that receives the iteration in the vector...
	Atanor* recipient = bd->lambdadomain.Declaration(a_counter);
	if (recipient != NULL)
		recipient->Get(environment, aNULL, idthread);

	bd->lambdadomain.instructions[0]->Setaffectation(true);
	recipient = bd->lambdadomain.instructions[0]->Get(environment, aNULL, idthread);
	Atanor* vect = bd->lambdadomain.instructions[1]->Get(environment, aNULL, idthread);

	//if it is constvector or a constmap, we need to declare the variables in them...
	if (recipient->Type() == a_constvector || recipient->Type() == a_constmap)
		recipient->Prepare(environment, idthread);

	//We then need to take into account our context...
	if (context->isContainer()) {
		if (vect != context)
			context = Selectacontainer(context, idthread);
		else
			context = context->Newinstance(idthread);
	}
	else {
		if (context->isString())
			context = context->Newinstance(idthread);
		else
			context = aNOELEMENT; //by default it will be a simple vector...
	}

	//we need to loop...
	recipient = vect->Loophaskell(recipient, context, environment, bd, idthread);
	vect->Release();
	return recipient;
}


Exporting Atanor* Atanor::Filter(short idthread, Atanor* env, AtanorFunctionLambda* bd, Atanor* var, Atanor* kcont, Atanor* accu, Atanor* init, bool direct) {

	Atanor* returnval;
	AtanorIteration* iter = Newiteration(false);

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

	for (iter->Begin(); iter->End() == aFALSE; iter->Next()) {
		key = iter->IteratorValue();
		if (first) {
			returnval = key->Atom();//We use the first value of the list to seed our accumulator variable
			first = false;
		}
		else {
			var->Putvalue(key, idthread);
			returnval = bd->Get(env, aNULL, idthread);

			if (returnval->isError()) {
				if (returnval == aBREAK) {
					accu = returnval;
					break;
				}

				var->Forcedclean();
				accu->Forcedclean();
				iter->Release();
				if (kcont != NULL)
					kcont->Release();
				return returnval;
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
	iter->Release();
	if (kcont != NULL)
		return kcont;
	return accu->Value();
}


Atanor* AtanorCallFunctionHaskell::GetHaskell5(Atanor* context, Atanor* environment, AtanorFunctionLambda* bd, short idthread) {

	Atanor* kcont = NULL;

	Atanor* key;
	bool testcontainer = false;


	Atanor* init = bd->lambdadomain.instructions[3]->Get(aNULL, aNULL, idthread);

	Atanor* klist = bd->lambdadomain.instructions[1]->Get(aNULL, aNULL, idthread);
	if (globalAtanor->Stop() || globalAtanor->Error(idthread))
		return aNULL;

	if (klist->Size() == 0 && !klist->isInfinite()) {
		kcont = init;
		init = NULL;
		return kcont;
	}

	Atanor* var = bd->lambdadomain.instructions[0]->Get(environment, aNULL, idthread);
	Atanor* accu = bd->lambdadomain.instructions[2]->Get(environment, aNULL, idthread);

	if (globalAtanor->Error(idthread))
		return globalAtanor->Errorobject(idthread);

	key = bd->lambdadomain.instructions[4];
	bool direct = key->Boolean();

	if (key == aZERO || key == aONE) // this is a scan
		kcont = klist->Newinstance(idthread);


	if (kcont != NULL) {
		if (!kcont->isContainer() && !kcont->isString()) {
			kcont->Release();
			kcont = globalAtanor->Providevector();
		}
		klist->Filter(idthread, environment, bd, var, kcont, accu, init, direct);
		klist->Release();
		return kcont;
	}

	key = klist->Filter(idthread, environment, bd, var, kcont, accu, init, direct);
	klist->Release();
	return key;
}


Atanor* AtanorCallFunctionHaskell::Process(HaskellLoop* loop, Atanor* context, Atanor* env, AtanorFunctionLambda* bd, short idthread) {
	Atanor* a;

	loop->reset();
	while (!loop->end()) {
		loop->execute(idthread);
		if (loop->next != NULL)
			Process(loop->next, context, env, bd, idthread);
		else {
			a = bd->Get(env, aNULL, idthread);

			if (a->isError()) {
				if (a == aBREAK)
					break;				
				return a;
			}

			
			if (a == aNULL)
				continue;

			if (context->isVectorContainer())
				context->Push(a);
			else
				context->Pushinmap(a, idthread);
			a->Release();
		}
	}

	return context;
}

Atanor* AtanorCallFunctionHaskell::GetHaskellN(Atanor* context, Atanor* environment, AtanorFunctionLambda* bd, short idthread) {


	HaskellLoop loops(idthread, environment);

	//First we store the recipients and the variables.
	Atanor* recipient;
	Atanor* vect;
	AtanorIteration* loop;
	HaskellLoop* hloop = &loops;
	bool first = true;
	bool forcerenew = false;
	short sz = bd->lambdadomain.instructions.last;

	short i;
	for (i = 0; i < sz; i += 3) {
		if (bd->lambdadomain.instructions[i + 2] == aFALSE)  {
			if (first)
				first = false;
			else {
				hloop = new HaskellLoop(idthread, environment);
				loops.add(hloop);
			}
		}

		bd->lambdadomain.instructions[i]->Setaffectation(true);
		recipient = bd->lambdadomain.instructions[i]->Get(environment, aNULL, idthread);
		vect = bd->lambdadomain.instructions[i + 1]->Get(environment, aNULL, idthread);
		if (recipient->Type() == a_constvector || recipient->Type() == a_constmap)
			recipient->Prepare(environment, idthread);
		
		if (vect == aNOELEMENT) {
			//Missing variables to finalize the loop...
			//we need to postpone the evaluation...
			loop = aITERNULL;
			vect = bd->lambdadomain.instructions[i + 1];
		}
		else
			loop = vect->Newiteration(false);

		if (vect == context)
			forcerenew = true;

		hloop->push(recipient, vect, loop);
	}

	vect = loops.vects[0];
	//We then need to take into account our context...
	if (context->isContainer()) {
		if (!forcerenew)
			context = Selectacontainer(context, idthread);
		else
			context = context->Newinstance(idthread);
	}
	else {
		if (context->isString())
			context = context->Newinstance(idthread);
		else
			context = aNOELEMENT; //by default it will be a simple vector...
	}

	recipient = Process(&loops, context, environment, bd, idthread);

	if (recipient->isError()) {
		context->Release();
		return recipient;
	}

	return context;
}


//-------------------------------------------------------------
// The following classes are used to execute a function through a variable in a Haskell expression: <Apply(func,a) = <func a>>
// For all the Atanor...Parameter, Get needs to return itself (it is a function or a method), hence the implementation of an Execute to do the job

string AtanorParameterFunction::String() {
	string nm = globalAtanor->Getsymbol(idtype);
	if (nm.find("call") != string::npos)
		nm = nm.substr(4, nm.size() - 4);
	nm += ":";
	nm += globalAtanor->Getsymbol(name);
	return nm;
}


Atanor* AtanorFunctionParameter::Get(Atanor* context, Atanor* val, short idthread) {
	if (val == aHASKELL)
		return this;

	return globalAtanor->Getdefinition(name, idthread, context);
}

Atanor* AtanorFunctionParameter::Execute(Atanor* context, Atanor* callfunction, short idthread) {
	Atanor* body = globalAtanor->Getdefinition(name, idthread, context);

	if (!body->isFunction())
		return globalAtanor->Returnerror("This object is not a function", idthread);

	AtanorCallFunction fcall(body);
	fcall.arguments = ((AtanorCall*)callfunction)->arguments;

	return fcall.Get(context, aNULL, idthread);
}

Atanor* AtanorMethodParameter::Execute(Atanor* context, Atanor* callfunction, short idthread) {
	//In this case, the first argument is our object...
	if (callfunction->Size() == 0)
		return globalAtanor->Returnerror("Missing object", idthread);

	AtanorCall* acall = (AtanorCall*)callfunction;

	AtanorCallMethod fcall(name);
	for (short i = 1; i < acall->arguments.size(); i++)
		fcall.arguments.push_back(acall->arguments[i]);

	Atanor* object = acall->arguments[0]->Get(context, aNULL, idthread);
	Atanor* a = fcall.Get(context, object, idthread);
	object->Release();
	return a;
}

Atanor* AtanorCommonParameter::Execute(Atanor* context, Atanor* callfunction, short idthread) {
	//In this case, the first argument is our object...
	if (callfunction->Size() == 0)
		return globalAtanor->Returnerror("Missing object", idthread);

	AtanorCall* acall = (AtanorCall*)callfunction;

	AtanorCallCommonMethod fcall(name);
	for (short i = 1; i < acall->arguments.size(); i++)
		fcall.arguments.push_back(acall->arguments[i]);

	Atanor* object = acall->arguments[0]->Get(context, aNULL, idthread);
	Atanor* a = fcall.Get(context, object, idthread);
	object->Release();
	return a;
}


Atanor* AtanorProcedureParameter::Execute(Atanor* context, Atanor* callfunction, short idthread) {

	AtanorCallProcedure pcall(name);
	pcall.arguments = ((AtanorCall*)callfunction)->arguments;

	return pcall.Get(context, aNULL, idthread);
}

//This is the entry point of such an expression...
Atanor* AtanorGetFunction::Get(Atanor* context, Atanor* callfunction, short idthread) {
	Atanor* c = globalAtanor->Getdeclaration(name, idthread);
	if (!c->isFunctionParameter()) {
		string message = "Cannot evaluate this variable as a function call: ";
		message += globalAtanor->Getsymbol(name);
		return globalAtanor->Returnerror(message, idthread);
	}

	return c->Execute(context, this, idthread);
}

//----------------------------------------------------------------------------------

//In these two cases, these methods are used to apply a method using the first argument as calling object
//This is used in Haskell such as: <trim x>
Atanor* AtanorGetMethod::Get(Atanor* context, Atanor* callfunction, short idthread) {
	AtanorCallMethod fcall(name);
	for (short i = 1; i < arguments.size(); i++)
		fcall.arguments.push_back(arguments[i]);

	Atanor* object = arguments[0]->Get(context, aNULL, idthread);
	Atanor* a = fcall.Get(context, object, idthread);
	object->Release();
	return a;
}

Atanor* AtanorGetCommon::Get(Atanor* context, Atanor* callfunction, short idthread) {

	AtanorCallCommonMethod fcall(name);
	for (short i = 1; i < arguments.size(); i++)
		fcall.arguments.push_back(arguments[i]);

	Atanor* object = arguments[0]->Get(context, aNULL, idthread);
	Atanor* a = fcall.Get(context, object, idthread);
	object->Release();
	return a;
}


