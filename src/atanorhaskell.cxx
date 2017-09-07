
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
#include "atanorframeinstance.h"

//------------------------------HASKELL----------------------------------------
short AtanorCallFunctionArgsHaskell::Typeinfered() {
	if (body != NULL) {
		short ref = body->Typeinfered();
		AtanorFunctionLambda* bd = body->Following();
		short ty;
		while (bd != NULL) {
			ty = bd->Typeinfered();
			if (ref == ty) {
				bd = bd->Following();
				continue;
			}

			if (Isnumber(ref)) {
				if (Isnumber(ty)) {
					if (ref < ty) {
						if (ref == a_long)
							ref = a_float;
					}
				}
				else {
					ref = a_none;
					break;
				}
			}
			else
			if (Istring(ref)) {
				if (!Istring(ty)) {
					ref = a_none;
					break;
				}
			}
			bd = bd->Following();
		}
		return ref;
	}
	return a_none;
}

short AtanorCallFunctionHaskell::Typeinfered() {
	if (body->returntype != a_null)
		return body->returntype;
	return a_none;
}

bool AtanorCallFunctionHaskell::Checkarity() {
	AtanorFunctionLambda* bd = body;
	while (bd != NULL) {
		if (bd->Purehaskelldeclaration()) {//A hdeclared description
			bd = bd->Following();
			continue;
		}
		if (arguments.size() == bd->parameters.size()) {
			if (bd->hdeclared) {
				bool found = true;
				for (short i = 0; i < bd->haskelldeclarations.size(); i++) {
					if (!globalAtanor->Compatiblestrict(bd->haskelldeclarations[i]->Type(), arguments[i]->Typeinfered())) {
						found = false;
						break;
					}
				}

				if (found) {
					//we can start at this specific function point...
					body = bd;
					return true;
				}
			}
			else
				return true;
		}
		bd = bd->Following();
	}
	return false;
}


Atanor* AtanorFunctionHaskellParameter::Execute(Atanor* context, Atanor* hcall, short idthread) {
	Atanor* defcall = globalAtanor->Getdefinition(name, idthread, context);

	long sz = defcall->Size();
	if (sz != hcall->Size()) {
		stringstream err;
		err << "Error: mismatch argument in a haskell function call to: " << globalAtanor->Getsymbol(name);
		return globalAtanor->Returnerror(err.str(), idthread);
	}

	if (!sz) {
		AtanorCallFunctionHaskell hfunc((AtanorFunctionLambda*)defcall);
		return hfunc.Get(context, aNULL, idthread);
	}


	AtanorCallFunctionArgsHaskell hfunc((AtanorFunctionLambda*)defcall);
	for (int i = 0; i < sz; i++)
		hfunc.arguments.push_back(hcall->Argument(i));
	return hfunc.Get(context, aNULL, idthread);

}

Atanor* AtanorCallSQUARE::Get(Atanor* context, Atanor* value, short idthread) {
	value = globalAtanor->Getdeclaration(name, idthread);
	return value->multiply(value, false);
}

Atanor* AtanorCallCUBE::Get(Atanor* context, Atanor* value, short idthread) {
	value = globalAtanor->Getdeclaration(name, idthread);
	value = value->multiply(value, false);
	return value->multiply(value, true);
}

Atanor* AtanorFunctionLambda::Get(Atanor* environment, Atanor* a, short idthread) {
	for (short i = 0; i < instructions.last; i++) {

		a = instructions.vecteur[i]->Get(environment, aNULL, idthread);

		if (a->needFullInvestigate()) {
			if (!hdeclared || store)
				return a->Returned(idthread);

			a = a->Returned(idthread);
			if (a->isError())
				return a;
			short atype = a->Type();
			if (returntype != a_null && returntype != atype) {
				if (globalAtanor->Compatiblestrict(returntype, atype)) {
					//The test atype < returntype is a simple way to tackle number definition order: short < int < long < decimal < float
					if (returntype == a_call || (Isnumber(returntype) && atype < returntype) || Istring(returntype) || a->isFrame())
						return a;

					//In the case when the types are compatible but different, we force the result to be of returntype
					environment = globalAtanor->newInstance.get(returntype)->Newinstance(idthread);
					environment->Putvalue(a, idthread);
					a->Release();
					return environment;
				}

				if (globalAtanor->debugmode) {
					string s = "Return value does not match declaration... Expected: '";
					s += globalAtanor->Getsymbol(returntype);
					s += "' ... Returned: '";
					s += globalAtanor->Getsymbol(atype) + "'";
					globalAtanor->Setnonblockingerror(s, idthread);
				}

				a->Release();
				return aRAISEERROR;
			}
			return a;
		}

		a->Release();
	}

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


Atanor* AtanorInstructionHaskellMainCASE::Get(Atanor* context, Atanor* value, short idthread) {
	for (long i = 0; i < instructions.last - other; i += 2) {
		value = instructions.vecteur[i]->Get(aTRUE, context, idthread);
		if (value->isError())
			return value;

		if (value->Boolean())
			return instructions.vecteur[i + 1]->Get(context, aNULL, idthread);
	}

	if (other)
		return instructions.vecteur[instructions.last - 1]->Get(context, aNULL, idthread);

	return aNULL;
}

Atanor* AtanorInstructionHaskellIF::Get(Atanor* context, Atanor* value, short idthread) {
	//if (!compiled) {
	//	instructions.vecteur[0] = instructions.vecteur[0]->Compile(aTRUE);
	//	compiled = true;
	//}

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

Atanor* AtanorHBloc::Get(Atanor* context, Atanor* a, short idthread) {
	short size = instructions.last;

	Atanor* environment = context;
	if (variablesWillBeCreated) {
		environment = new AtanorDeclarationSelfPush(idthread);
		environment->Setinfo(this);
	}

	for (short i = 0; i < size; i++) {
		a = instructions[i];

		a = a->Get(environment, aNULL, idthread);

		if (a->needFullInvestigate()) {
			if (!variablesWillBeCreated)
				return a->Returned(idthread);

			if (environment->isEmpty()) {
				delete environment;
				return a->Returned(idthread);
			}

			context = a->Returned(idthread);
			if (!context->Reference())
				environment->Cleaning(idthread);
			else {
				context->Setreference();
				environment->Cleaning(idthread);
				context->Protect();
			}
			delete environment;
			return context;
		}

		a->Release();
	}

	if (variablesWillBeCreated)
		delete environment;

	return aNULL;
}

//------------------------------HASKELL----------------------------------------
Atanor* AtanorCallFunctionHaskell::Get(Atanor* context, Atanor* res, short idthread) {
	AtanorDeclarationAutoClean environment(idthread);
	globalAtanor->Setnonblockingerror("", idthread);

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
		globalAtanor->Current(this, idthread);
		environment.Cleaning(idthread);
		if (res == aRAISEERROR && context->Type() != a_haskelldeclaration)
			return globalAtanor->Returnerror("Lambda expression failed...", idthread);
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
	AtanorDeclarationAutoClean environment(idthread);
	globalAtanor->Setnonblockingerror("", idthread);

	AtanorFunctionLambda* bd = body;
	Atanor** args;
	
	short sz = arguments.size();
	short i, param_name;
	bool execute = false;
	
	args = new Atanor*[sz];
	for (i = 0; i < sz; i++) {
		args[i] = arguments[i]->Get(context, aHASKELL, idthread);
		args[i]->Setreference();
	}

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
					if (globalAtanor->debugmode)
						globalAtanor->Setnonblockingerror("Cannot project this container onto the current argument...", idthread);
					res = aRAISEERROR;
					break;
				}
				continue;
			}

			param_name = res->Name();
			if (!param_name) {//it is a value comparison
				if (args[i]->isFrame()) {
					if (hdeclared == true && bd->haskelldeclarations[i]->Size()) {
						if (args[i]->Size() != bd->haskelldeclarations[i]->Size()) {
							if (globalAtanor->debugmode) {
								string s = "Argument number does not match data structure declaration size... ";
								s += "Argument: '" + globalAtanor->Getsymbol(args[i]->Type());
								s += "'... Declaration: '" + globalAtanor->Getsymbol(bd->haskelldeclarations[i]->Type()) + "'";
								globalAtanor->Setnonblockingerror(s, idthread);
							}
							res = aRAISEERROR;
							break;
						}
						for (long j = 0; j < args[i]->Size(); j++) {
							if (args[i]->Typevariable(j) != bd->haskelldeclarations[i]->Typevariable(j)) {
								if (globalAtanor->debugmode) {
									string s = "Mismatch between argument and declaration...";
									s += "Argument: '" + globalAtanor->Getsymbol(args[i]->Typevariable(j));
									s += "'... Declaration: '" + globalAtanor->Getsymbol(bd->haskelldeclarations[i]->Typevariable(j)) + "'";
									globalAtanor->Setnonblockingerror(s, idthread);
								}
								res = aRAISEERROR;
								break;
							}
						}
						if (res == aRAISEERROR)
							break;
					}
					if (res->Compare(&environment, args[i], idthread) == aFALSE) {
						if (globalAtanor->debugmode) {
							string s = "Mismatch between argument and data structure declaration... ";
							s += "Argument: '" + globalAtanor->Getsymbol(args[i]->Type());
							s += "' ...Declaration: '" + globalAtanor->Getsymbol(res->Type()) + "'";
							globalAtanor->Setnonblockingerror(s, idthread);
						}
						res = aRAISEERROR;
						break;
					}
				}
				else {
					if (args[i]->same(res) == aFALSE) {
						if (globalAtanor->debugmode) {
							string s = "Mismatch between argument and declaration...";
							s += "Argument: '" + args[i]->String();
							s += "' ...Declaration: '" + res->String() + "'";
							globalAtanor->Setnonblockingerror(s, idthread);
						}
						res = aRAISEERROR;
						break;
					}
				}
			}
			else {
				if (hdeclared == true && res->Typevariable() != args[i]->Type() && !globalAtanor->Compatiblestrict(res->Typevariable(), args[i]->Type())) {
					if (globalAtanor->debugmode) {
						string s = "Mismatch between argument and declaration...";
						s += "Argument: '" + globalAtanor->Getsymbol(args[i]->Type());
						s += "' ...Declaration: '" + globalAtanor->Getsymbol(res->Type()) + "'";
						globalAtanor->Setnonblockingerror(s, idthread);
					}
					res = aRAISEERROR;
					break;
				}

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

			if (!res->isError()) {
				execute = true;
				break;
			}
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

	if (!execute && !globalAtanor->Error(idthread)) {
		globalAtanor->Current(this, idthread);
		string err = "Error: no suitable declaration was found for function: ";
		err += globalAtanor->Getsymbol(name);
		err += " --> ";
		err += globalAtanor->Getnonblockingerror(idthread);
		res->Release();
		return globalAtanor->Returnerror(err, idthread);
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

	char getkey = 0;
	if (isMapContainer()) {
		if (recipient->isMapContainer()) {
			recipient->Prepare(environment, idthread);
			getkey = 2;
		}
		else
			getkey = 1;
	}

	loop->Begin();
	while (!loop->End()->Boolean()) {
		switch (getkey) {
		case 0:
			recipient->Putvalue(loop->IteratorValue(), idthread);
			break;
		case 1:
			recipient->Putvalue(loop->IteratorKey(), idthread);
			break;
		case 2:
			recipient->Setvalue(loop, aNULL, idthread);
		}

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
	//If we have a returntype defined for this function, and we have a context (the variable that is being assigned to) then
	//we need to check the compatibility of the returntype and this context variable.
	if (bd->returntype != a_null && (context->isContainer() || context->isString()) && !globalAtanor->Compatiblestrict(bd->returntype, context->Type()))
		return aRAISEERROR;

	//First we create our variables...
	//The first variable is the one that receives the iteration in the vector...

	Atanor* recipient = bd->lambdadomain.Declaration(a_counter);
	if (recipient != NULL)
		recipient->Get(environment, aNULL, idthread);

	bd->lambdadomain.instructions[0]->Setaffectation(true);

	//We put our system in store mode, which means that the returntype SHOULD not be tested in AtanorFunctionLambda
	//The reason is that the context variable is the variable that is going to store the data and its type has already been tested
	//against returntype above, or below when context is missing... In that case, we create a context whose type will match returntype.
	bd->store = true;

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
		else {
			if (bd->returntype == a_null)
				context = aNOELEMENT; //by default it will be a simple vector...
			else
				context = globalAtanor->newInstance.get(bd->returntype)->Newinstance(idthread);
		}
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

	if (bd->returntype != a_null && (context->isContainer() || context->isString()) && !globalAtanor->Compatiblestrict(bd->returntype, context->Type()))
		return aRAISEERROR;

	HaskellLoop loops(idthread, environment);

	//First we store the recipients and the variables.
	Atanor* recipient;
	Atanor* vect;
	AtanorIteration* loop;
	HaskellLoop* hloop = &loops;
	bool first = true;
	bool forcerenew = false;
	short sz = bd->lambdadomain.instructions.last;

	//we put our system in store mode, which means that the returntype SHOULD not be tested in AtanorFunctionLambda
	bd->store = true;

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
		else {
			if (bd->returntype == a_null)
				context = aNOELEMENT; //by default it will be a simple vector...
			else
				context = globalAtanor->newInstance.get(bd->returntype)->Newinstance(idthread);
		}
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

Atanor* AtanorFrameMethodParameter::Execute(Atanor* context, Atanor* callfunction, short idthread) {
	//In this case, the first argument is our object...
	if (callfunction->Size() == 0)
		return globalAtanor->Returnerror("Missing object", idthread);

	AtanorCall* acall = (AtanorCall*)callfunction;

	AtanorCallFrameFunction fcall(name);
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
Atanor* AtanorGetFunction::Get(Atanor* context, Atanor* c, short idthread) {
	c = globalAtanor->Getdeclaration(name, idthread);
	if (c->isFunctionParameter())
		return c->Execute(context, this, idthread);

	c = c->Body(idthread);
	if (c->isFunctionParameter())
		return c->Execute(context, this, idthread);

	if (c->isFunction()) {

		if (c->isHaskellFunction()) {
			if (arguments.size()) {
				AtanorCallFunctionArgsHaskell hfunc((AtanorFunctionLambda*)c);
				hfunc.arguments = arguments;
				return hfunc.Get(context, aNULL, idthread);
			}

			AtanorCallFunctionHaskell hfunc((AtanorFunctionLambda*)c);
			return hfunc.Get(context, aNULL, idthread);
		}


		AtanorCallFunction acall(c);
		acall.arguments = arguments;
		return acall.Get(context, aNULL, idthread);
	}

	string message = "Cannot evaluate this variable as a function call: ";
	message += globalAtanor->Getsymbol(name);
	return globalAtanor->Returnerror(message, idthread);
}

Atanor* AtanorGetFunctionThrough::Get(Atanor* context, Atanor* callfunction, short idthread) {
	callfunction = call->Get(context, aNULL, idthread);

	if (callfunction->isFunctionParameter())
		return callfunction->Execute(context, this, idthread);

	if (callfunction->isHaskellFunction()) {

		if (arguments.size()) {
			AtanorCallFunctionArgsHaskell hfunc((AtanorFunctionLambda*)callfunction);
			hfunc.arguments = arguments;
			return hfunc.Get(context, aNULL, idthread);
		}

		AtanorCallFunctionHaskell hfunc((AtanorFunctionLambda*)callfunction);
		return hfunc.Get(context, aNULL, idthread);
	}

	if (callfunction->isFunction()) {
		AtanorCallFunction acall(callfunction);
		acall.arguments = arguments;
		return acall.Get(context, aNULL, idthread);
	}

	string message = "Cannot evaluate this as a function call: ";
	message += globalAtanor->Getsymbol(name);
	return globalAtanor->Returnerror(message, idthread);
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

Atanor* AtanorFrameParameter::Compare(Atanor* env, Atanor* a, short idthread) {
	if (!a->isFrame() || a->Frame()->Name() != framename)
		return aFALSE;

	Atanorframeinstance* af = (Atanorframeinstance*)a->Value();
	
	for (int i = 0; i < equivalence.size(); i += 2) {
		a = af->Declaration(equivalence[i]);
		env->Declare(equivalence[i + 1], a);
		a->Setreference();
		globalAtanor->Storevariable(idthread, equivalence[i + 1], a);
	}

	return aTRUE;
}

Atanor* AtanorCallFrameMethod::Get(Atanor* context, Atanor* value, short idthread) {
	//the first argument must be a frame instance...
	if (arguments.size() == 0)
		return globalAtanor->Returnerror("Missing object", idthread);

	value = arguments[0]->Get(context, aNULL, idthread);
	if (!value->isFrame())
		return globalAtanor->Returnerror("Expecting object", idthread);

	AtanorCallFrameFunction callfunc((AtanorFrame*)value->Frame(), name);
	Atanor* a;
	int i;
	for (i = 1; i < arguments.size(); i++) {
		a = arguments[i]->Get(context, aNULL, idthread);
		callfunc.arguments.push_back(a);
		a->Setreference();
	}

	a = callfunc.Get(context, value, idthread);

	for (i = 0; i < callfunc.arguments.size(); i++)
		callfunc.arguments[i]->Resetreference();

	return a;
}

bool AtanorCallFrameMethod::Checkarity() {
	return Arity(globalAtanor->framemethods[name], arguments.size());
}
