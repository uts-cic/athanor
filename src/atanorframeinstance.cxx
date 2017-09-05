
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
filename   : atanorframeinstance.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorframeinstance.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, frameinstanceMethod>  Atanorframeinstance::methods;
Exporting hmap<string, string> Atanorframeinstance::infomethods;
Exporting bin_hash<unsigned long> Atanorframeinstance::exported;

Exporting short Atanorframeinstance::idtype = 0;

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorframeinstance::AddMethod(AtanorGlobal* global, string name, frameinstanceMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorframeinstance::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorframeinstance::idtype = global->Getid("frameinstance");
	Atanorframeinstance::AddMethod(global, "_initial", &Atanorframeinstance::MethodInitial, P_NONE, "_initial(): Initialization of a frame.");
	Atanorframeinstance::AddMethod(global, "type", &Atanorframeinstance::MethodType, P_NONE, "type(): Return the frame's type.");


	global->newInstance[Atanorframeinstance::idtype] = new Atanorframeinstance(NULL, global);

	return true;
}


Atanorframeinstance* Atanorframeinstance::RecordFrame(short name, AtanorFrame* f, AtanorGlobal* global) {

	//Little hack so that a frame will always have a type() method...
	f->declarations[a_type] = new AtanorFunction(a_type, global);

	global->RecordMethods(name, f->exported);
	Atanorframeinstance* a = new Atanorframeinstance(f, global);
	globalAtanor->frames[name] = f;
	global->newInstance[name] = a;

	return a;
}

Atanor* Atanorframeinstance::Newinstance(short idthread, Atanor* a) {
	a = new Atanorframeinstance(frame);
	if (!frame->post) {
		for (long i = 0; i < frame->variables.size(); i++)
			frame->variables[i]->Get(a, aNULL, idthread);
		return a;
	}

	Atanor* o;
	//we create a frame object, with what was declared into the frame...	
	for (long i = 0; i < frame->variables.size(); i++) {
		o = frame->variables[i];
		if (o->isFrame()) {//these cases will be handled in the Postinstanciation method...
			//we create a temporate variable, with no actual type...
			a->Declare(o->Name(), new AtanorLet);
			continue;
		}
		o->Get(a, aNULL, idthread);
	}

	return a;
}

void Atanorframeinstance::Postinstanciation(short idthread, bool setreference) {
	if (!frame->post)
		return;

	Atanor* o;
	Atanor* v;
	short nm;
	globalAtanor->Pushstack(this, idthread);
	//we create a frame object, with what was declared into the frame...	
	//We have two cases:
	// Either: we have a creation within the _initial function of frame, where this local frame is declared
	// Or: we need to use the default initialization that was declared where the local frame itself is declared
	//frame toto {
	//		within w(10); //default initialization, see o->Get(this, aNULL, idthread);
	//		function _initial(int k) {
	//			w=within(k); // initialization within a _initial function that replaces the other default initialization

	for (long i = 0; i < frame->variables.size(); i++) {
		o = frame->variables[i];
		if (o->isFrame()) {//these cases will be handled in the Postinstanciation method...
			nm = o->Name();
			short ipos = declared.Idposition(nm);
			v = declared.declarations[ipos]->Value();
			//we delete the variable, a AtanorLet created in Newinstance...
			delete declared.declarations.vecteur[ipos];

			if (v != aNOELEMENT) { //we are dealing with an initialization within a _initial function
				if (v->Typevariable() != o->Typevariable()) {
					declared.erase(nm);
					v->Resetreference(reference);
					stringstream msg;
					msg << "Type mismatch for '" << globalAtanor->Getsymbol(nm) << "':";
					globalAtanor->Returnerror(msg.str(), idthread);
					return;
				}
				declared.Redeclare(ipos, nm, v);
			}
			else //we rollback to the default initialization...
				o->Get(this, aNULL, idthread);
			
			if (setreference)
				declared.declarations[ipos]->Setreference(reference);
		}
	}
	globalAtanor->Popstack(idthread);
}


Atanor* Atanorframeinstance::Get(Atanor* context, Atanor* idx, short idthread) {
	if (idx->isIndex()) {
		//In this case, we check if we have an index function
		AtanorIndex* aid = (AtanorIndex*)idx;
		Atanor* func;
		if (aid->interval)
			func = frame->Declaration(a_interval);
		else
			func = frame->Declaration(a_index);
		
		if (func == NULL)
			return globalAtanor->Returnerror("Cannot process indexes", idthread);

		setprotection();
		vector<Atanor*> arguments;
		Atanor* a = aid->left->Get(context, aNULL, idthread);
		a->Setreference();
		arguments.push_back(a);

		if (aid->interval) {
			a = aid->right->Get(context, aNULL, idthread);
			a->Setreference();
			arguments.push_back(a);
		}

		//No value...
		arguments.push_back(aNOELEMENT);

		globalAtanor->Pushstack(this, idthread);
		func = Execute(func, arguments, idthread);
		globalAtanor->Popstack(idthread);

		if (arguments[0] == aid->left)
			arguments[0]->Protect();
		else
			arguments[0]->Resetreference();

		if (aid->interval) {
			if (arguments[1] == aid->right)
				arguments[1]->Protect();
			else
				arguments[1]->Resetreference();
		}

		resetprotection();

		return func;
	}

	return this;
}

Atanor* Atanorframeinstance::Put(Atanor* idx, Atanor* value, short idthread) {
	if (idx->isIndex()) {
		//In this case, we check if we have an index function
		AtanorIndex* aid = (AtanorIndex*)idx;
		Atanor* func;
		
		if (aid->interval)
			func = frame->Declaration(a_interval);
		else
			func = frame->Declaration(a_index);

		if (func == NULL)
			return globalAtanor->Returnerror("Cannot process indexes", idthread);


		setprotection();
		vector<Atanor*> arguments;
		Atanor* a = aid->left->Get(aNULL, aNULL, idthread);
		a->Setreference();
		arguments.push_back(a);

		if (aid->interval) {
			a = aid->right->Get(aNULL, aNULL, idthread);
			a->Setreference();
			arguments.push_back(a);
		}

		arguments.push_back(value);
		value->Setreference();

		globalAtanor->Pushstack(this, idthread);
		func = Execute(func, arguments, idthread);
		globalAtanor->Popstack(idthread);

		if (arguments[0] == aid->left)
			arguments[0]->Protect();
		else
			arguments[0]->Resetreference();

		if (aid->interval) {
			if (arguments[1] == aid->right)
				arguments[1]->Protect();
			else
				arguments[1]->Resetreference();
		}
		value->Resetreference();

		resetprotection();

		return func;
	}

	if (value->Type() != frame->Name())
		return globalAtanor->Returnerror("Wong frame assignment", idthread);

	Locking _lock(this);
	Atanorframeinstance* instance = (Atanorframeinstance*)value;

	for (short ii = 0; ii < declared.declarations.last;ii)
		declared.declarations[ii]->Put(aNULL, instance->declared.declarations[ii], idthread);
	return aTRUE;
}

//We could have implemented the execution of a method as a simple AtanorCallFunction, however in the case of a multhreading application
//there can be some issues. If the value that is returned by our method belongs to the frame object, then in some cases, there could be 
//a desynchronisation between the reference setting in the function with the reference setting in other threads.
//Hence, this specific implementation in which returned variables are protected by the frame instance lock.
Atanor* Atanorframeinstance::Execute(Atanor* body, vector<Atanor*>& arguments, short idthread) {

	AtanorDeclarationLocal environment;
	environment.Setinfo(this);

	Atanor* p;
	AtanorFunction* bd = (AtanorFunction*)body->Body(idthread);

	bool error = true;
	while (bd != NULL) {
		if (arguments.size() != bd->Size()) {
			bd = bd->next;
			continue;
		}

		Locking _lock(this);
		error = false;
		for (size_t i = 0; i < arguments.size(); i++) {
			p = bd->Parameter(i);
			if (!p->Setvalue(&environment, arguments[i], idthread)) {
				error = true;
				break;
			}
		}

		if (!error)
			break;

		environment.Cleaning(idthread);

		bd = bd->next;
	}

	if (error) {
		string err = "Check the arguments of: ";
		err += globalAtanor->Getsymbol(body->Name());
		return globalAtanor->Returnerror(err, idthread);
	}

	globalAtanor->Pushstack(&environment, idthread);
	//We then apply our function within this environment
	Atanor* a = bd->Get(&environment, this, idthread);
	globalAtanor->Popstack(idthread);
	
	if (a->Reference()) {
		a->Setreference();
		//we clean our structure...
		environment.Cleaning(idthread);
		a->Protect();
	}
	else
		environment.Cleaning(idthread);

	return a;
}

Atanor* Atanorframeinstance::Execute(Atanor* body, short idthread) {

	AtanorDeclarationLocal environment;
	environment.Setinfo(this);

	AtanorFunction* bd = (AtanorFunction*)body->Body(idthread);

	while (bd != NULL) {
		if (bd->Size() == 0)
			break;
		bd = bd->next;
	}

	if (bd == NULL) {
		string err = "Check the arguments of: ";
		err += globalAtanor->Getsymbol(body->Name());
		return globalAtanor->Returnerror(err, idthread);
	}

	globalAtanor->Pushstack(&environment, idthread);
	//We then apply our function within this environment
	Atanor* a = bd->Get(&environment, this, idthread);
	globalAtanor->Popstack(idthread);

	if (a->Reference()) {
		a->Setreference();
		//we clean our structure...
		environment.Cleaning(idthread);
		a->Protect();
	}
	else
		environment.Cleaning(idthread);

	return a;
}



