
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
filename   : codeexecute.cxx
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
#include <memory>
#include "atanoratanor.h"
#include "atanordecimal.h"
#include "predicate.h"
#include "atanorhvector.h"


//--------------------------------------------------------------------
static hmap<short, Atanor*> througvariables;
//--------------------------------------------------------------------
bool TestEndthreads();
//--------------------------------------------------------------------
#ifdef WIN32
static const char ATANOROS[] = "WINDOWS";
#else
#ifdef APPLE
static const char ATANOROS[] = "MACOS";
#else
static const char ATANOROS[] = "UNIX";
#endif
#endif

//--------------------------------------------------------------------
Exporting AtanorSystemVariable::AtanorSystemVariable(AtanorGlobal* g, Atanor* v, short n, short t) : value(v), AtanorVariableDeclaration(g, n, t) {
	g->systems[n] = this;
	if (v->idtracker == -1)
		g->RecordInTracker(v);
}

//--------------------------------------------------------------------

void AtanorGlobal::RecordSystemVariables() {
	Atanor* a;

	a = new AtanorSystemVariable(this, aNULL, a_null, a_const);
	a = new AtanorSystemVariable(this, aNOELEMENT, a_empty, a_const);
	a = new AtanorSystemVariable(this, aUNIVERSAL, a_universal, a_const);

	a = new AtanorSystemVariable(this, new AtanorConstString(""), Createid("_current"), a_string);
	a = new AtanorSystemVariable(this, new AtanorConstSVector, Createid("_paths"), a_svector);
	a = new AtanorSystemVariable(this, new AtanorConstSVector, Createid("_args"), a_svector);

	a = new AtanorSystemVariable(this, new AtanorConstString(ATANOROS), Createid("_OS"), a_string);

#ifdef WIN32
	a = new AtanorSystemVariable(this, new AtanorConstString("\\"), Createid("_sep"), a_string);
	a = new AtanorSystemVariable(this, new AtanorConstString("\r\n"), Createid("_endl"), a_string);
#else
	a = new AtanorSystemVariable(this, new AtanorConstString("/"), Createid("_sep"), a_string);
	a = new AtanorSystemVariable(this, new AtanorConstString("\n"), Createid("_endl"), a_string);
#endif

}

void AtanorGlobal::SystemInitialization(string spath) {

	char localpath[4096];
	localpath[0] = 0;

#ifdef WIN32
	_fullpath(localpath, ".", 4096);
#else
	realpath(".", localpath);
#endif

#ifdef WIN32
	long pos = spath.rfind("\\");
	if (pos == string::npos) {
		spath = localpath;
		if (localpath[spath.size() - 1] != '\\')
			spath += "\\";
	}
	else
		spath = spath.substr(0, pos + 1);
#else
	long pos = spath.rfind("/");
	if (pos == string::npos) {
		spath = localpath;
		if (localpath[spath.size() - 1] != '/')
			spath += "/";
	}
	else
		spath = spath.substr(0, pos + 1);
#endif
	AtanorConstString* s = (AtanorConstString*)systems[Getid("_current")]->value;
	s->value = spath;

	short i;
	AtanorConstSVector* v = (AtanorConstSVector*)systems[Getid("_paths")]->value;
	v->values.clear();


	v->values.push_back(localpath);

	for (i = 0; i < spaces.size(); i++)
		v->values.push_back(spaces[i]->filename);

	v = (AtanorConstSVector*)systems[Getid("_args")]->value;
	v->values.clear();

	for (i = 0; i < arguments.size(); i++)
		v->values.push_back(arguments[i]);
}
//--------------------------------------------------------------------

void AtanorDeclaration::Cleaning(short idthread) {
	short j;
	binuint64 filter;
	if (declarations.base == -1)
		return;

	for (short ii = 0; ii < declarations.tsize; ii++) {
		filter = declarations.indexes[ii];
		if (filter) {
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

				declarations.table[ii][j]->Resetreference();
				declarations.table[ii][j] = NULL;
				globalAtanor->Removevariable(idthread, ((ii + declarations.base) << binbits) + j);
				filter >>= 1;
				j++;
			}
			declarations.indexes[ii] = 0;
		}
	}
}

//--------------------------------------------------------------------
Atanor* AtanorGlobal::EvaluateMainVariable() {
	AtanorCode* acode = Getcurrentcode();
	AtanorFrame* frame = (AtanorFrame*)acode->Mainframe();
	Atanor* res;
	for (short i = 0; i < frame->instructions.size(); i++) {
		if (frame->instructions[i]->isVariable()) {
			res = frame->instructions[i]->Get(frame, aNULL, 0);
			if (res->isError())
				return res;
		}
	}
	return aTRUE;
}

#ifdef _DEBUG
static vector<string> lesnoms;
#endif

//--------------------------------------------------------------------
Exporting Atanor* AtanorExecute(AtanorCode* code, string name, vector<Atanor*>& params) {

	globalAtanor->globalLOCK = true;
	short idthread = globalAtanor->GetThreadid();
	short id = globalAtanor->Getid(name);
	Atanor* func = code->Mainframe()->Declaration(id);

	if (func == NULL || !func->isFunction()) {
		name = "Unknown function: " + name;
		return globalAtanor->Returnerror(name, idthread);
	}

	short i;
	AtanorCallFunction call(func);
	for (i = 0; i < params.size(); i++) {
		call.arguments.push_back(params[i]);
		params[i]->Setreference();
	}

	globalAtanor->Pushstack(code->Mainframe(), idthread);
	func = call.Get(aNULL, aNULL, idthread);
	globalAtanor->Popstack(idthread);

	for (i = 0; i < params.size(); i++)
		params[i]->Resetreference();
	return func;
}

Exporting Atanor* AtanorExecute(AtanorCode* code, string name, vector<Atanor*>& params, short idthread) {
	Atanor* main = code->Mainframe();

	globalAtanor->globalLOCK = true;
	short id = globalAtanor->Getid(name);
	Atanor* func = main->Declaration(id);

	if (func == NULL || !func->isFunction()) {
		name = "Unknown function: " + name;
		return globalAtanor->Returnerror(name, idthread);
	}

	short i;
	AtanorCallFunction call(func);
	for (i = 0; i < params.size(); i++) {
		call.arguments.push_back(params[i]);
		params[i]->Setreference();
	}
	
	globalAtanor->Pushstack(main, idthread);
	func = call.Get(aNULL, aNULL, idthread);
	globalAtanor->Popstack(idthread);

	for (i = 0; i < params.size(); i++)
		params[i]->Resetreference();
	return func;
}


Atanor* AtanorCode::Execute(long begininstruction, short idthread) {
	Atanor* a = aNULL;
	
	short sz = mainframe.instructions.size();
	_debuginitmin(idthread);
	short i;

	for (i = begininstruction; i < sz; i++) {

		_debugpush(mainframe.instructions[i]);
		a = mainframe.instructions[i]->Get(&mainframe, aNULL, idthread);
		_debugpop();

		if (global->Error(idthread)) {
			_cleandebug;
			a->Release();
			a = global->Errorobject(idthread);
			break;
		}

		a->Release();
		a = aNULL;
	}

	mainframe.instructions.last = begininstruction;

	_cleandebug;
	return a;
}

Atanor* AtanorCode::Loading() {
	globalAtanor->globalLOCK = false;


	short idthread = 0;
	global->running = true;
	global->executionbreak = false;
	global->InitThreadid(idthread);

	global->Pushstack(&mainframe);
	_debuginitmin(0);

	Atanor* a = aNULL;
	short sz = mainframe.instructions.size();

	for (short i = firstinstruction; i < sz; i++) {

		_debugpush(mainframe.instructions[i]);
		a = mainframe.instructions[i]->Get(&mainframe, aNULL, 0);
		_debugpop();

		if (global->Error(0)) {
			a->Release();
			a = global->Errorobject(0);
			break;
		}

		a->Release();
		a = aNULL;
	}


	_cleandebug;
	global->Popstack(0);

	return a;
}

Atanor* AtanorCode::Run() {
#ifdef _DEBUG
	lesnoms.clear();
	bin_hash<string>::iterator ot;
	for (ot = globalAtanor->idSymbols.begin(); ot != globalAtanor->idSymbols.end(); ot++)
		lesnoms.push_back(ot->second);
#endif

	globalAtanor->globalLOCK = false;


	short idthread = 0;
	global->running = true;
	global->executionbreak = false;
	global->InitThreadid(idthread);

	global->Pushstack(&mainframe);
	_debuginitmin(0);

	Atanor* a = aNULL;
	short sz = mainframe.instructions.size();

	for (short i = firstinstruction; i < sz; i++) {

		_debugpush(mainframe.instructions[i]);
		a = mainframe.instructions[i]->Get(&mainframe, aNULL, 0);
		_debugpop();

		if (global->Error(0)) {
			a->Release();
			a = global->Errorobject(0);
			break;
		}

		a->Release();
		a = aNULL;
	}


	_cleandebug;
	global->Popstack(0);


	{
		Locking _lock(global->_call);
		if (a != aNULL) {
			if (global->threads[0].currentinstruction != NULL) {
				currentline = global->threads[0].currentinstruction->Currentline();
				currentfileid = global->threads[0].currentinstruction->Currentfile();
				if (currentfileid != -1)
					filename = global->filenames[currentfileid];
			}
		}
		
		//globalAtanor->globalLOCK = false;
		if (global->terminationfunction != NULL)
			(*global->terminationfunction)(global);

		global->executionbreak = true;
		global->running = false;
		global->threads[0].used = false;
		global->Releasevariables();
	}

	while (!global->TestEndthreads()) {}

	return a;
}

//--------------------------------------------------------------------
Atanor* AtanorIndex::Get(Atanor* localidx, Atanor* obj, short idthread) {
	Atanor* klocal = obj->Get(localidx, this, idthread);

	if (function == NULL)
		return klocal;

	if (function->isIncrement()) {
		if (klocal == aNOELEMENT && obj->isValueContainer()) {
			if (obj->isString())
				klocal = globalAtanor->Providestring("");
			else
			if (obj->isFloat())
				klocal = globalAtanor->Providefloat(0);
			else
				klocal = globalAtanor->Provideint(0);
		}

		//we increment
		function->Get(aNULL, klocal, idthread);

		if (obj->isValueContainer()) {
			//we store the value back into place
			//we reevaluate our indexes...
			localidx = Evaluate(idthread);
			klocal = obj->Put(localidx, klocal, idthread);
			localidx->Rollback();
		}

		return klocal;
	}

	localidx = this;
	Atanor* kidx = function;
	Atanor* object = klocal;

	while (kidx != NULL && kidx->isIndex()) {
		if (object == aNOELEMENT) {
			if (globalAtanor->erroronkey)
				globalAtanor->Returnerror("Wrong key in a container or a string access", idthread);
			return aNOELEMENT;
		}

		obj = object;
		localidx = kidx;
		klocal = object->Get(aNULL, kidx, idthread);
		if (klocal != object) {
			object->Release();
			object = klocal;
		}

		kidx = kidx->Function();
	}

	if (kidx != NULL) {
		if (klocal == aNOELEMENT) {
			if (!obj->isValueContainer() && kidx->Name() == globalAtanor->Getid("push")) {
				//if it is not a value container, then it can contain a vector...
				klocal = globalAtanor->Providevector();
				localidx = Evaluate(idthread);
				obj->Put(localidx, klocal, idthread);
				localidx->Rollback();
			}
			else {
				if (globalAtanor->erroronkey)
					globalAtanor->Returnerror("Wrong key in a container or a string access", idthread);
				return aNOELEMENT;
			}
		}

		if (kidx->isIncrement()) {
			kidx->Get(aNULL, klocal, idthread);
			if (obj->isValueContainer()) {
				localidx = Evaluate(idthread);
				obj->Put(localidx, klocal, idthread);
				localidx->Rollback();
			}
			return klocal;
		}

		object = kidx->Get(aNULL, klocal, idthread);
		if (object != klocal)
			klocal->Release();
		return object;
	}

	return klocal;
}

Atanor* AtanorIndex::Put(Atanor* recipient, Atanor* value, short idthread) {
	AtanorIndex* idx;

	if (function == NULL) {
		idx = Evaluate(idthread);
		recipient = recipient->Put(idx, value, idthread);
		idx->Rollback();
		return aTRUE;
	}

	if (function->isIndex()) {
		idx = this;
		Atanor* intermediate = recipient;
		vector<Atanor*> stack;

		while (idx->function != NULL && idx->function->isIndex()) {
			stack.push_back(idx);
			stack.push_back(intermediate);
			intermediate = intermediate->Get(aNULL, idx, idthread);
			stack.push_back(intermediate);
			idx = (AtanorIndex*)idx->function;
		}

		idx = idx->Evaluate(idthread);
		intermediate->Put(idx, value, idthread);
		idx->Rollback();

		for (int i = stack.size() - 1; i >= 0; i -= 3) {
			intermediate = stack[i];
			if (!intermediate->isProtected() || intermediate->Reference())
				break;
			idx = stack[i - 2]->Evaluate(idthread);
			stack[i - 1]->Put(idx, intermediate, idthread);
			idx->Rollback();
		}
	}

	return aTRUE;
}

//--------------------------------------------------------------------
// Some necessary declarations to record code
Exporting AtanorTracked::AtanorTracked(short t, AtanorGlobal* g, Atanor* parent) {
	idtype = t;
	line = 0;
	idcode = -1;
	idfile = -1;

	if (parent != NULL)
		parent->AddInstruction(this);

	if (g != NULL) {
		g->RecordInTracker(this);
		line = g->Getcurrentline();
		idcode = (short)g->spaceid;
		if (idcode != -1)
			idfile = g->spaces[idcode]->currentfileid;
	}
}

Exporting AtanorTracked::AtanorTracked(string t, AtanorGlobal* g, Atanor* parent) {
	idtype = 0;
	line = 0;
	idcode = -1;
	idfile = -1;

	if (parent != NULL)
		parent->AddInstruction(this);

	if (g != NULL) {
		idtype = g->Getid(t);
		g->RecordInTracker(this);
		line = g->Getcurrentline();
		idcode = (short)g->spaceid;
		if (idcode != -1)
			idfile = g->spaces[idcode]->currentfileid;
	}
}

//----------------------------------------------------------------------------------
//This is actually a line of code, which when it is executed records a new variable into the declaration space...
AtanorVariableDeclaration::AtanorVariableDeclaration(AtanorGlobal* g, short n, short t, bool p, bool c, Atanor* parent) : AtanorTracked(a_declarations, g, parent) {
	if (parent != NULL)
		parent->SetVariablesWillBeCreated();
	
	name = n;
	typevariable = t;
	isprivate = p;
	initialization = NULL;
	function = NULL;
	choice = 0;
	isconstant = c;

	isframe = false;
	if (globalAtanor->newInstance.check(typevariable))
		isframe = globalAtanor->newInstance.get(typevariable)->isFrame();
}

AtanorVariableDeclaration::AtanorVariableDeclaration(AtanorGlobal* g, short n, short t, Atanor* parent) : AtanorTracked(a_declarations, g, parent) {
	if (parent != NULL)
		parent->SetVariablesWillBeCreated();

	name = n;
	typevariable = t;
	isprivate = false;
	initialization = NULL;
	function = NULL;
	choice = 0;
	isframe = false;
	isconstant = false;

	if (globalAtanor->newInstance.check(typevariable))
		isframe = globalAtanor->newInstance.get(typevariable)->isFrame();
}

//When we call this function, we actually will create an element of type value
Atanor* AtanorVariableDeclaration::Get(Atanor* domain, Atanor* value, short idthread) {
	//we create our instance...

	Atanor* a = globalAtanor->newInstance.get(typevariable)->Newinstance(idthread, function);
	a->Setname(name);
	domain->Declare(name, a);
	globalAtanor->Storevariable(idthread, name, a);
	a->Setreference(1);

	if (initialization != NULL) {
		if (initialization->Name() == a_initial) {
			value = initialization->Get(domain, a, idthread);
			if (value->isError())
				return value;
		}
		else {
			bool aff = a->isAffectation();
			if (!aff)
				a->Setaffectation(true);
			value = initialization->Get(a, aAFFECTATION, idthread);
			if (value->isError())
				return value;
			if (value != a)
				a->Put(aNULL, value, idthread);
			a->Setaffectation(aff);
		}

		value->Release();
	}

	//If it is a frame, then we instanciate local frame declaration here...
	//Hence, intialization of local frames can depend on local frame variables...
	a->Postinstanciation(idthread, true);

	return a;
}

Atanor* AtanorFrameVariableDeclaration::Get(Atanor* domain, Atanor* value, short idthread) {
	//we create our instance...
	Atanor* a;
	if (common) {
		a = domain->Frame()->Declaration(name);
		if (a == this)
			a = AtanorVariableDeclaration::Get(domain->Frame(), aNULL, idthread);
		domain->Declare(name, a);
		return a;
	}

	a = globalAtanor->newInstance.get(typevariable)->Newinstance(idthread, function);
	a->Setname(name);
	domain->Declare(name, a);

	//The only difference with the previous one is that we do not set the reference here...

	if (initialization != NULL) {
		if (initialization->Name() == a_initial) {
			//We protect our new variable against any potential destruction
			a->Setreference();
			value = initialization->Get(domain, a, idthread);
			//But we need to reset the internal references to 0, without deleting any of these elements.
			a->Protect();

			if (value->isError())
				return value;
		}
		else {
			bool aff = a->isAffectation();
			if (!aff)
				a->Setaffectation(true);
			value = initialization->Get(a, aAFFECTATION, idthread);
			if (value->isError())
				return value;
			if (value != a)
				a->Put(aNULL, value, idthread);
			a->Setaffectation(aff);
		}

		value->Release();
	}

	//If it is a frame, then we instanciate local frame declaration here...
	//Hence, intialization of local frames can depend on local frame variables...

	a->Postinstanciation(idthread, false);

	return a;
}

Atanor* AtanorVariableDeclaration::Put(Atanor* domain, Atanor* value, short idthread) {
	Atanor* a = globalAtanor->newInstance.get(typevariable)->Newinstance(idthread);
	domain->Declare(name, a);
	globalAtanor->Storevariable(idthread, name, a);
	a->Setreference();
	return a->Put(aNULL, value, idthread);
}

bool AtanorVariableDeclaration::Setvalue(Atanor* domain, Atanor* value, short idthread, bool strict) {
	//we create a new variable, whose type is the one from the function declaration...
	//We need to check some cases

	//This is a case when creating a duplicate variable is needless or too dangerous...
	//Exemple: vector, map, primemap etc...

	if (!value->duplicateForCall()) {
		if (value->Type() == typevariable || value->isLetSelf()) {
			value->Setreference();
			domain->Declare(name, value);
			globalAtanor->Storevariable(idthread, name, value);
			return true;
		}
	}

	//we accept "null" as a default value...
	if (value->Type() == typevariable || value == aNULL || globalAtanor->Testcompatibility(typevariable, value->Type(), strict)) {
		Atanor* a = globalAtanor->newInstance.get(typevariable)->Newinstance(idthread);
		a->Setname(name);
		a->Setreference();
		domain->Declare(name, a);
		globalAtanor->Storevariable(idthread, name, a);
		if (value != aNULL)
			a->Put(aNULL, value, idthread);
		return true;
	}

	return false;
}

Atanor* AtanorGlobalVariableDeclaration::Get(Atanor* domain, Atanor* value, short idthread) {
	//we create our instance...
	if (alreadydeclared)
		return globalAtanor->Getdeclaration(name, idthread);

	alreadydeclared = true;
	Atanor* a = globalAtanor->newInstance.get(typevariable)->Newinstance(idthread, function);
	a->Setname(name);
	domain->Declare(name, a);
	globalAtanor->Storevariable(idthread, name, a);

	a->Setreference(1);
	if (initialization != NULL) {
		if (initialization->Name() == a_initial)
			value = initialization->Get(aNULL, a, idthread);
		else {
			bool aff = a->isAffectation();
			if (!aff)
				a->Setaffectation(true);
			value = initialization->Get(a, aAFFECTATION, idthread);
			if (value->isError())
				return value;
			if (value != a)
				a->Putvalue(value, idthread);
			a->Setaffectation(aff);
		}
		value->Release();
	}

	//If it is a frame, then we instanciate local frame declaration here...
	//Hence, intialization of local frames can depend on local frame variables...
	a->Postinstanciation(idthread, true);

	return a;
}

Atanor* AtanorThroughVariableDeclaration::Get(Atanor* domain, Atanor* value, short idthread) {
	//we create our instance...
	if (througvariables.find(name) != througvariables.end())
		return througvariables[name];
	
	Atanor* a = globalAtanor->newInstance.get(globalAtanor->throughs[typevariable])->Newinstance(idthread, function);
	a->Setname(name);

	a->Setreference(1);
	if (initialization != NULL) {
		bool aff = a->isAffectation();
		if (!aff)
			a->Setaffectation(true);
		value = initialization->Get(a, aAFFECTATION, idthread);
		if (value->isError())
			return value;
		if (value != a)
			a->Putvalue(value, idthread);
		a->Setaffectation(aff);
		value->Release();
	}

	througvariables[name] = a;

	return a;
}


//----------------------------------------------------------------------------------
Exporting bool AtanorCallMethod::Checkarity() {
	//We execute the method associated to an object...
	return Arity(globalAtanor->allmethods[name], arguments.size());
}

bool AtanorCallCommonMethod::Checkarity() {
	return Arity(globalAtanor->arities[name], arguments.size());
}

bool AtanorCallProcedure::Checkarity() {
	return Arity(globalAtanor->arities[name], arguments.size());
}

Exporting bool AtanorCallFunction::Checkarity() {
	if (body == NULL || body->isCallVariable())
		return true;

	AtanorFunction* bd = NULL;
	AtanorFunction* b = (AtanorFunction*)body;
	int nbbody = 0;
	short btype, atype;
	while (b != NULL) {
		bool found = true;		
		if (b->parameters.size() == arguments.size()) {
			if (bd == NULL) {
				for (short i = 0; i < b->parameters.size(); i++) {
					btype = b->parameters[i]->Typevariable();
					atype = arguments[i]->Typeinfered();
					if (btype != atype && !globalAtanor->Compatible(btype, atype)) {
						found = false;
						break;
					}
				}

				if (found)
					bd = b;
			}			
			b = b->next;
			continue;
		}

		//We need to check if we have at least one section with pre-initialized values...
		//The size of arguments should then be inferior to the one of parameters
		if (arguments.size() < b->parameters.size()) {
			//We have one, we need to test if there is a default initialisation
			short nb = 0;			
			for (short i = 0; i < b->parameters.size(); i++) {
				if (i < arguments.size()) {
					btype = b->parameters[i]->Typevariable();
					atype = arguments[i]->Typeinfered();
					if (btype != atype && !globalAtanor->Compatible(btype, atype)) {
						found = false;
						break;
					}
				}

				if (b->parameters[i]->Initialisation() != NULL)
					nb++;
			}

			if (found) {
				while (nb) {
					//we have nb default initialization...
					if ((b->parameters.size() - nb) == arguments.size()) {
						//we know we have one in case of failure...
						if (bd == NULL)
							bd = b;
						nonlimited = true;
						break;
					}
					nb--;
				}
			}
		}
		b = b->next;
	}

	if (bd != NULL) {
		if (((AtanorFunction*)body)->next != NULL)
			bd->strict = true;

		body = bd;
		return true;
	}
	return false;
}

bool AtanorCallThread::Checkarity() {
	AtanorFunction* b = (AtanorFunction*)body;
	while (b != NULL) {
		if (b->parameters.size() == arguments.size())
			return true;

		short nb = 0;
		for (short i = 0; i < b->parameters.size(); i++) {
			if (b->parameters[i]->Initialisation() != NULL)
				nb++;
		}
		while (nb) {
			if ((b->parameters.size() - nb) == arguments.size()) {
				nonlimited = true;
				return true;
			}
			nb--;
		}

		b = b->next;
	}
	return false;
}

bool AtanorCallFrameFunction::Checkarity() {
	if (frame == NULL)
		return Arity(globalAtanor->framemethods[name], arguments.size());

	Atanor* b = frame->Declaration(name);
	if (b == NULL || !b->isFunction())
		return false;

	while (b != NULL) {
		if (b->Size() == arguments.size())
			return true;
		b = b->Nextfunction();
	}
	return false;
}

AtanorCallAtanorVariable::AtanorCallAtanorVariable(short n, Atanoratanor* f, AtanorGlobal* g, Atanor* parent) : 
	aa(f),AtanorCallVariable(n, f->Typevariable(), g, parent) {}

Atanor* AtanorCallAtanorVariable::Get(Atanor* context, Atanor* object, short idthread) {

	return aa->MethodMethod(context, idthread, (AtanorCall*)call);
}

Exporting Atanor* AtanorCallMethod::Put(Atanor* context, Atanor* object, short idthread) {
	//We execute the method associated to an object...
	short t = object->Type();
	if (!globalAtanor->methods.check(t) || Arity(globalAtanor->methods[t][name], arguments.size()) == false) {
		if (!globalAtanor->frames.check(t)) {
			string mess = "Unknown method or Wrong number of arguments for: ";
			mess += globalAtanor->Getsymbol(name);
			return globalAtanor->Returnerror(mess, idthread);
		}
	}

	AtanorCallMethod* call = this;
	if (idthread && call->arguments.size()) {
		//In the case of thread, we need to duplicate this structure
		//in order to get a proper cleaning variable...
		call = new AtanorCallMethod(name);
		call->arguments = arguments;
	}

	_debuginit(idthread, this);
	object = object->CallMethod(name, context, idthread, call);

	if (function != NULL) {
		if (object->isError()) {
			_cleandebug;
			call->Cleaning(idthread);
			if (call != this)
				delete call;
			return object;
		}

		//This is a function which is called from an assignment, in this case, we need to catch
		//the index before computing it...
		if (function->isStop())
			return object;

		context = function->Get(context, object, idthread);
		if (context != object) {
			if (object->isProtected()) {
				context->Setreference();
				object->Release();
				context->Protect();
			}
			object = context;			
		}
	}


	call->Releasing(object, idthread);
	if (call != this)
		delete call;

	_cleandebug;
	return object;
}

Exporting Atanor* AtanorCallMethod::Get(Atanor* context, Atanor* object, short idthread) {
	//We execute the method associated to an object...
	short t = object->Type();
	if (!globalAtanor->methods.check(t) || Arity(globalAtanor->methods[t][name], arguments.size()) == false) {
		if (!globalAtanor->frames.check(t)) {
			string mess = "Unknown method or Wrong number of arguments for: ";
			mess += globalAtanor->Getsymbol(name);
			return globalAtanor->Returnerror(mess, idthread);
		}
	}

	AtanorCallMethod* call = this;
	if (idthread && call->arguments.size()) {
		//In the case of thread, we need to duplicate this structure
		//in order to get a proper cleaning variable...
		call = new AtanorCallMethod(name);
		call->arguments = arguments;
	}

	_debuginit(idthread, this);
	object = object->CallMethod(name, context, idthread, call);

	if (function != NULL) {
		if (object->isError()) {
			call->Cleaning(idthread);
			if (call != this)
				delete call;
			_cleandebug;
			return object;
		}

		context = function->Get(context, object, idthread);
		if (context != object) {
			if (object->isProtected()) {
				context->Setreference();
				object->Release();
				context->Protect();
			}
			object = context;
		}
	}

	call->Releasing(object, idthread);
	if (call != this)
		delete call;

	_cleandebug;
	return object;
}

Atanor* AtanorCallCommonMethod::Get(Atanor* context, Atanor* object, short idthread) {
	AtanorCallCommonMethod* call = this;

	if (idthread && call->arguments.size()) {
		call = new AtanorCallCommonMethod(name);
		call->arguments = arguments;
	}

	_debuginit(idthread, this);
	object = globalAtanor->commons[name](object, idthread, call);

	if (function != NULL) {
		if (object->isError()) {
			call->Cleaning(idthread);
			if (call != this)
				delete call;
			_cleandebug;
			return object;
		}

		context = function->Get(context, object, idthread);
		if (context != object) {
			if (object->isProtected()) {
				context->Setreference();
				object->Release();
				context->Protect();
			}
			object = context;
		}
	}

	call->Releasing(object, idthread);
	if (call != this)
		delete call;

	_cleandebug;
	return object;
}

Atanor* AtanorCallFrameFunction::Put(Atanor* context, Atanor* object, short idthread) {
	_debuginit(idthread, this);
	object = object->CallMethod(name, context, idthread, this);

	if (function != NULL) {
		if (object->isError()) {
			_cleandebug;
			return object;
		}

		//We catch the index before assignment...
		if (function->isStop())
			return object;

		Atanor* a = function->Get(context, object, idthread);
		if (a != object) {
			if (object->isProtected()) {
				a->Setreference();
				object->Release();
				a->Protect();
			}
		}
		_cleandebug;
		return a;
	}
	_cleandebug;
	return object;
}


Atanor* AtanorCallFrameFunction::Get(Atanor* context, Atanor* object, short idthread) {
	_debuginit(idthread, this);
	object = object->CallMethod(name, context, idthread, this);

	if (function != NULL) {
		if (object->isError()) {
			_cleandebug;
			return object;
		}

		Atanor* a = function->Get(context, object, idthread);
		if (a != object) {
			if (object->isProtected()) {
				a->Setreference();
				object->Release();
				a->Protect();
			}
		}
		_cleandebug;
		return a;
	}
	_cleandebug;
	return object;
}

Atanor* AtanorCallTopFrameFunction::Put(Atanor* context, Atanor* object, short idthread) {
	_debuginit(idthread, this);
	object = object->CallTopMethod(name, frame, context, idthread, this);

	if (function != NULL) {
		if (object->isError()) {
			_cleandebug;
			return object;
		}

		if (function->isStop())
			return object;

		Atanor* a = function->Get(context, object, idthread);
		if (a != object) {
			if (object->isProtected()) {
				a->Setreference();
				object->Release();
				a->Protect();
			}
		}
		_cleandebug;
		return a;
	}
	_cleandebug;
	return object;
}


Atanor* AtanorCallTopFrameFunction::Get(Atanor* context, Atanor* object, short idthread) {
	_debuginit(idthread, this);
	object = object->CallTopMethod(name, frame, context, idthread, this);

	if (function != NULL) {
		if (object->isError()) {
			_cleandebug;
			return object;
		}

		Atanor* a = function->Get(context, object, idthread);
		if (a != object) {
			if (object->isProtected()) {
				a->Setreference();
				object->Release();
				a->Protect();
			}
		}
		_cleandebug;
		return a;
	}
	_cleandebug;
	return object;
}


Atanor* AtanorCallProcedure::Get(Atanor* context, Atanor* object, short idthread) {
	AtanorCallProcedure* call = this;

	if (idthread && call->arguments.size()) {
		call = new AtanorCallProcedure(name);
		call->arguments = arguments;
	}

	_debuginit(idthread, this);
	object = (*globalAtanor->procedures[name])(context, idthread, call);

	if (function != NULL) {
		if (object->isError()) {
			_cleandebug;
			call->Cleaning(idthread);
			if (call != this)
				delete call;
			return object;
		}

		Atanor* a = function->Get(context, object, idthread);
		if (a != object) {
			if (object->isProtected()) {
				a->Setreference();
				object->Release();
				a->Protect();
			}
			object = a;
		}
	}

	call->Releasing(object, idthread);
	if (call != this)
		delete call;

	_cleandebug;
	return object;
}

//------------------------------------------------------------------------------------

Exporting Atanor* AtanorCallFunction::Get(Atanor* domain, Atanor* a, short idthread) {
#ifdef _DEBUG
	string nn = globalAtanor->Getsymbol(Name());
	name = Name();
#endif


	AtanorDeclarationLocal environment;
	environment.Setinfo(this);

	Atanor* p;
	AtanorFunction* bd = (AtanorFunction*)body->Body(idthread);

	short i, sz = 0;
	bool strict = bd->strict;

	bool error = true;
	while (bd != NULL) {
		if (arguments.size() != bd->Size()) {
			if (!nonlimited) {
				bd = bd->next;
				continue;
			}
		}
		sz = bd->parameters.size();
		error = false;
		if (arguments.size() || nonlimited) {
			for (i = 0; i < sz; i++) {
				p = bd->parameters[i];
				if (!nonlimited || i < arguments.size())
					a = arguments[i]->Get(domain, aNULL, idthread);
				else {
					a = p->Initialisation();
					if (a == NULL) {
						error = true;
						break;
					}
				}

				if (!p->Setvalue(&environment, a, idthread, strict)) {
					a->Release();
					error = true;
					break;
				}

				a->Release();
			}

			if (!error)
				break;

			environment.Cleaning(idthread);

			bd = bd->next;
		}
		else
			break;
	}

	if (error) {
		string err = "Check the arguments of: ";
		err += globalAtanor->Getsymbol(Name());
		return globalAtanor->Returnerror(err, idthread);
	}

	globalAtanor->Pushstack(&environment, idthread);
	//We then apply our function within this environment
	a = bd->Get(&environment, aNULL, idthread);
	globalAtanor->Popstack(idthread);

	if (!a->Reference())
		environment.Cleaning(idthread);
	else {
		a->Setreference();
		//we clean our structure...
		environment.Cleaning(idthread);
		a->Protect();
	}

	return a;
}

//------------------------------------------------------------------------------------
//This is a trick which allows for a variable creation...
Atanor* AtanorCallVariable::Put(Atanor* domain, Atanor* value, short idthread) {
	if (domain->isDeclared(name))
		return aTRUE;

	value = globalAtanor->newInstance.get(typevariable)->Newinstance(idthread);
	domain->Declare(name, value);
	globalAtanor->Storevariable(idthread, name, value);
	value->Setreference(1);
	return aTRUE;
}

Atanor* AtanorCallFrameVariable::Put(Atanor* context, Atanor* value, short idthread) {
	value = globalAtanor->Getdefinition(name, idthread, context);

    if (call != NULL) {
        if (call->isStop())
            return value;
        return call->Put(context, value, idthread);
    }

	return value;
}


Atanor* AtanorCallFrameVariable::Get(Atanor* context, Atanor* value, short idthread) {
	value = globalAtanor->Getdefinition(name, idthread, context);

	if (call != NULL) {
		if (affectation && call->isStop())
			return value;
		return call->Get(context, value, idthread);
	}

	return value;
}

Atanor* AtanorCallThis::Get(Atanor* context, Atanor* value, short idthread) {
	value = globalAtanor->Getdefinition(a_this, idthread);

	if (call != NULL)
		return call->Get(context, value, idthread);

	return value;
}

Atanor* AtanorCallFromFrameVariable::Get(Atanor* context, Atanor* value, short idthread) {
    //In this case, the value is the calling variable...
    value = value->Declaration(name);
    
    if (call != NULL) {
        if (affectation && call->isStop())
            return value;
        return call->Get(context, value, idthread);
    }
    return value;
}

Atanor* AtanorCallFromFrameVariable::Put(Atanor* context, Atanor* value, short idthread) {
    //In this case, the value is the calling variable...
    value = value->Declaration(name);
    
    if (call != NULL) {
        if (call->isStop())
            return value;
        return call->Put(context, value, idthread);
    }
    
    return value;
}


Atanor* AtanorCallThroughVariable::Get(Atanor* context, Atanor* value, short idthread) {
	value = througvariables[name];
	
	if (call != NULL) {
		if (affectation) {
			if (call->isStop())
				return value;
			return call->Put(context, value, idthread);
		}
		return call->Get(context, value, idthread);
	}

	return value;
}


Atanor* AtanorCallVariable::Get(Atanor* context, Atanor* value, short idthread) {
	value = globalAtanor->Getdeclaration(name, idthread);

	if (call != NULL) {
		if (affectation) {
			if (call->isStop())
				return value;
			return call->Put(context, value, idthread);
		}
		return call->Get(context, value, idthread);
	}

	if (forced)
		value->Forcedclean();

	return value;
}

Atanor* AtanorCallConstantVariable::Get(Atanor* context, Atanor* value, short idthread) {
	value = globalAtanor->Getdeclaration(name, idthread);

	if (call != NULL)
		return call->Get(context, value, idthread);

	value->SetConst();
	return value;
}

Atanor* AtanorCallFunctionVariable::Get(Atanor* context, Atanor* value, short idthread) {
	value = globalAtanor->Getdeclaration(name, idthread);

	if (call != NULL) {
		if (affectation) {
			if (call->isStop())
				return value;
			return call->Put(context, value, idthread);
		}
		return call->Get(context, value, idthread);
	}

	return value;
}

//------------------------------------------------SELF SECTION-------------------------------------

Atanor* AtanorSelfDeclarationVariable::Get(Atanor* domain, Atanor* value, short idthread) {
	AtanorLet* a;
	if (typevariable == a_self)
		a = new AtanorSelf;
	else
		a = new AtanorLet;

	domain->Declare(name, a);
	globalAtanor->Storevariable(idthread, name, a);
	a->Setreference();
	if (initialization != NULL) {
		value = initialization->Get(aNULL, aNULL, idthread);
		if (value->isError())
			return value;
		a->value = value;
		value->Setreference();
	}

	return a;
}


Atanor* AtanorSelfDeclarationVariable::Put(Atanor* domain, Atanor* value, short idthread) {
	if (typevariable == a_self)
		value = new AtanorSelf;
	else
		value = new AtanorLet;
	
	domain->Declare(name, value);
	globalAtanor->Storevariable(idthread, name, value);
	value->Setreference();
	return value;
}

Atanor* AtanorCallSelfVariable::Get(Atanor* context, Atanor* value, short idthread) {
	value = globalAtanor->Getdeclaration(name, idthread);

	if (call != NULL) {
		value = value->Value();

		if (value == aNOELEMENT)
			return globalAtanor->Returnerror("Uninitialized 'self' variable", idthread);

		if (affectation && call->isStop())
			return value;
		return call->Get(context, value, idthread);
	}

	if (affectation) {
		if (forced)
			value->Forcedclean();
		return value;
	}

	return value->Value();
}

Atanor* AtanorCallSelfVariable::Put(Atanor* domain, Atanor* value, short idthread) {
	if (domain->isDeclared(name))
		return aTRUE;

	if (typevariable == a_self)
		value = new AtanorSelf;
	else
		value = new AtanorLet;

	domain->Declare(name, value);
	globalAtanor->Storevariable(idthread, name, value);
	value->Setreference();
	return aTRUE;
}

//------------------------------------------------------------------------------------
Atanor* AtanorInstruction::Get(Atanor* context, Atanor* a, short idthread) {
	short size = instructions.last;

	_debuginit(idthread, this);

	if (size == 1) {
		a = instructions.vecteur[0]->Get(context, aNULL, idthread);
		_cleandebug;
		return a;
	}

	Atanor* environment = context;
	if (variablesWillBeCreated) {
		environment = new AtanorDeclarationSelfPush(idthread);
		environment->Setinfo(this);
	}

	for (short i = 0; i < size; i++) {
		a = instructions[i];
		
		_debugpush(a);
		a = a->Get(environment, aNULL, idthread);
		_debugpop();

		if (globalAtanor->Error(idthread)) {
			if (!a->isError()) {
				a->Release();
				a = globalAtanor->Errorobject(idthread);
			}
		}

		if (a->needFullInvestigate()) {
			_cleandebug;
			if (!variablesWillBeCreated)
				return a;

			if (a->isReturned()) {
				if (environment->isEmpty()) {
					delete environment;
					return a;
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
				return a;
			}

			delete environment;
			return a;
		}

		a->Release();
	}

	_cleandebug;
	if (variablesWillBeCreated)
		delete environment;

	return aNULL;
}

Atanor* AtanorFunction::Get(Atanor* environment, Atanor* obj, short idthread) {
#ifdef _DEBUG
	string n = globalAtanor->Getsymbol(name);
#endif

	short size = instructions.size();

	_debuginit(idthread, this);
	Atanor* a;

	for (short i = 0; i < size; i++) {

		_debugpush(instructions[i]);
		a = instructions[i]->Get(environment, obj, idthread);
		_debugpop();

		if (globalAtanor->Error(idthread)) {			
			if (!a->isError()) {
				a->Release();
				a = globalAtanor->Errorobject(idthread);
			}
		}

		if (a->needFullInvestigate()) {
			_cleandebug;
			if (a->isError())
				return a;

			a = a->Returned(idthread);
			if (returntype) {
				if (a->Type() != returntype && !globalAtanor->Compatiblestrict(returntype, a->Type())) {
					a->Release();
					stringstream msg;
					msg << "Mismatch between return value and function declaration. Expecting: '"
						<< globalAtanor->Getsymbol(returntype)
						<< "' Got: '" << globalAtanor->Getsymbol(a->Type()) << "'";
					return globalAtanor->Returnerror(msg.str(), idthread);
				}
			}
			return a;
		}

		a->Release();
	}

	_cleandebug;
	if (returntype && name != a_initial)
		return globalAtanor->Returnerror("This function is expected to return a value", idthread);

	return aNULL;
}
//____________________________________________________________________________________
#ifdef WIN32
void __stdcall AThread(AtanorThreadCall* call) {
#else
void AThread(AtanorThreadCall* call) {
#endif
	short idthread = call->idthread;
	if (globalAtanor->Error(call->parentid)) {
		for (short i = 0; i < call->arguments.size(); i++)
			call->arguments[i]->Resetreference();
		if (call->cleandom) {
			if (globalAtanor->Checktracker(call->domain, call->idomain))
				call->domain->Resetreference();
		}
		globalAtanor->EraseThreadid(idthread);
		delete call;
		return;
	}
	
	globalAtanor->InitThreadid(idthread);
	{
		Locking _lock(globalAtanor->_call);
		call->tid = globalAtanor->threads[idthread].handle;
		if (call->body->isJoined()) {
			Locking _lock(globalAtanor->_join);
			call->joinpos = globalAtanor->Joinpos(call->parentid);
			if (call->joinpos >= globalAtanor->maxjoined) {
				for (short i = 0; i < call->arguments.size(); i++)
					call->arguments[i]->Resetreference();
				if (call->cleandom) {
					if (globalAtanor->Checktracker(call->domain, call->idomain))
						call->domain->Resetreference();
				}
				globalAtanor->EraseThreadid(idthread);
				globalAtanor->Returnerror("Maximum number of joined threads reached", call->parentid);
				delete call;
				return;
			}
			globalAtanor->threads[call->parentid].joined.push_back(call->thid);
		}
	}
	Locking* _exclusive = NULL;
	if (call->exclusive && call->cleandom)
		_exclusive = new Locking((AtanorObject*)call->domain);
	Atanor* a = call->Get(call->domain, aNULL, idthread);
	if (call->recipient != NULL) {//If we have a streamed variable
		call->recipient->Put(call->domain, a, idthread);
		call->recipient->Resetreference();
	}
	a->Release();
	if (_exclusive != NULL)
		delete _exclusive;
	

	for (auto& it : globalAtanor->threads[idthread].locks)
		it.second->Unlocking();
	globalAtanor->threads[idthread].locks.clear();
	globalAtanor->threads[idthread].Clearknowledgebase();
	if (call->joinpos != -1) {
		Locking _lock(globalAtanor->_join);
		globalAtanor->threads[call->parentid].joined.at(call->joinpos, NULL);
	}
	if (globalAtanor->Error(idthread)) {
		Locking _lock(globalAtanor->_call);
		if (!globalAtanor->threads[call->parentid].error) {
			globalAtanor->threads[call->parentid].errorraised = globalAtanor->threads[idthread].errorraised;
			globalAtanor->threads[call->parentid].currentinstruction = globalAtanor->threads[idthread].currentinstruction;
		}
	}
	for (short i = 0; i < call->arguments.size(); i++)
		call->arguments[i]->Resetreference();
	if (call->cleandom) {
		if (globalAtanor->Checktracker(call->domain, call->idomain))
			call->domain->Resetreference();
	}	
	globalAtanor->EraseThreadid(idthread);
	delete call;
}

Atanor* AtanorThreadCall::Get(Atanor* domain, Atanor* value, short idthread) {
	AtanorDeclarationLocal environment;

	environment.Setinfo(this);

	Atanor* p;
	AtanorFunction* bd = (AtanorFunction*)body->Body(idthread);
	bool strict = bd->strict;
	if (!strict && bd->next)
		strict = true;

	bool error = true;
	while (bd != NULL) {
		if (arguments.size() != bd->Size()) {
			if (!nonlimited) {
				bd = bd->next;
				continue;
			}
		}

		error = false;
		Atanor* a;
		for (short i = 0; i < bd->parameters.size(); i++) {
			p = bd->Parameter(i);

			if (!nonlimited || i < arguments.size())
				a = arguments[i];
			else {
				a = p->Initialisation();
				if (a == NULL) {
					error = true;
					break;
				}
			}

			if (!p->Setvalue(&environment, a, idthread, strict)) {
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
		err += globalAtanor->Getsymbol(Name());
		return globalAtanor->Returnerror(err, idthread);
	}

	globalAtanor->Pushstack(&environment, idthread);
	//We then apply our function within this environment
	Atanor* ret = bd->Get(&environment, aNULL, idthread);
	globalAtanor->Popstack(idthread);

	if (!ret->Reference())
		environment.Cleaning(idthread);
	else {
		ret->Setreference();
		//we clean our structure...
		environment.Cleaning(idthread);
		ret->Protect();
	}
	globalAtanor->threads[idthread].variables.clear();

	return ret;
}

Atanor* AtanorThread::Get(Atanor* environment, Atanor* a, short idthread) {
	Locking* _lock = NULL;
	if (_locker != NULL)
		_lock = new Locking(*_locker);

	_debuginit(idthread, this);
	short size = instructions.size();
	for (short i = 0; i < size; i++) {
		a = instructions[i];

		_debugpush(a);
		a = a->Get(environment, aNULL, idthread);
		_debugpop();

		if (a->isReturned()) {
			if (_lock != NULL)
				delete _lock;
			_cleandebug;
			if (returntype) {
				if (a->Type() != returntype && !globalAtanor->Compatible(returntype, a->Type())) {
					a->Release();
					stringstream msg;
					msg << "Mismatch between return value and thread declaration. Expecting: '"
						<< globalAtanor->Getsymbol(returntype)
						<< "' Got: '" << globalAtanor->Getsymbol(a->Type()) << "'";
					return globalAtanor->Returnerror(msg.str(), idthread);
				}
			}

			return a->Returned(idthread);
		}

		if (globalAtanor->Error(idthread)) {
			if (_lock != NULL)
				delete _lock;
			_cleandebug;
			return globalAtanor->Errorobject(idthread);
		}

		a->Release();
	}

	if (_lock != NULL)
		delete _lock;
	_cleandebug;
	if (returntype)
		return globalAtanor->Returnerror("This function is expected to return a value", idthread);

	return aNULL;
}

Atanor* AtanorCallThread::Get(Atanor* environment, Atanor* value, short idthread) {
	//We enter a thread... //Now Locks will be created...
	globalAtanor->Sethreading();

	short id = globalAtanor->GetaThreadId();
	if (id == -1)
		return globalAtanor->Returnerror("Too many threads", idthread);

	Atanor* main = globalAtanor->Getmainframe(idcode);

	globalAtanor->Pushstack(main, id);
	short i;
	vector<short> vars;
	main->Variables(vars);
	for (i = 0; i < vars.size(); i++)
		globalAtanor->threads[id].variables[vars[i]].push_back(main->Declaration(vars[i]));

	//Then we copy the current knowledge base into the new thread own knowledge base as they must share it...
	globalAtanor->threads[id].Setknowledgebase();

	bool cleandom = false;
	Atanor* dom = main;
	if (environment != main) {
		if (environment->isFrame()) {
			//We are in a frame...
			//We protect it
			environment->Setreference();
			globalAtanor->Pushstack(environment, id);
			cleandom = true;
			dom = environment;
			vars.clear();
			dom->Variables(vars);
			for (i = 0; i < vars.size(); i++)
				globalAtanor->threads[id].variables[vars[i]].push_back(dom->Declaration(vars[i]));
		}
	}

	AtanorThreadCall* threadcall = new AtanorThreadCall(body, recipient, dom, cleandom, body->isExclusive(), id, idthread);
	threadcall->Getinfo(this);

	//we need to evaluate our arguments beforehand
	Atanor* a;
	for (i = 0; i < arguments.size(); i++) {
		a = arguments[i]->Get(environment, aNULL, idthread)->Atom();
		a->Setreference();
		threadcall->arguments.push_back(a);
	}

	Locking _lock(globalAtanor->_call);
	threadcall->thid = new std::thread(AThread, threadcall);
	if (threadcall->thid == NULL) {
		globalAtanor->threads[id].Clear();
		delete threadcall;
		return globalAtanor->Returnerror("Could not create a thread", idthread);
	}
	return aNULL;
}

//____________________________________________________________________________________
Atanor* AtanorInstructionSIMPLEAFFECTATION::Get(Atanor* environment, Atanor* value, short idthread) {
	Atanor* variable = instructions.vecteur[0]->Get(environment, aNULL, idthread);
	if (variable->isError())
		return variable;

	bool aff = variable->isAffectation();
	if (!aff)
		variable->Setaffectation(true);

	value = instructions.vecteur[1]->Get(variable, aAFFECTATION, idthread);

	variable->Setaffectation(aff);

	if (value == variable) {
		if (globalAtanor->globalLOCK)
			globalAtanor->Triggeronfalse(variable);
		return aTRUE;
	}

	if (value->isError())
		return value;
	
	if (value == aNOELEMENT) {
		variable->Putvalue(aNOELEMENT, idthread);
		return value;
	}

	//Then we might have an issue, if value is a sub element of variable...
	//for instance: v=v[0];
	//we need to prevent a Clear() on "variable" to delete this element...
	value->Setprotect(true);

	environment = variable->Put(environment, value, idthread);

	value->Resetreferencenoprotect(0);

	if (environment->isError())
		return environment;

	if (globalAtanor->globalLOCK)
		globalAtanor->Triggeronfalse(variable);

	return aTRUE;
}

Atanor* AtanorInstructionAFFECTATION::Get(Atanor* environment, Atanor* value, short idthread) {
	Atanor* variable = instructions.vecteur[0]->Get(environment, aNULL, idthread);
	if (variable->isError())
		return variable;

	value = instructions.vecteur[0]->Function();
	bool relvar = false;

	if (value != NULL) {
		Atanor* idx = value->Getindex();
		
		if (idx != value) {//It could be a useless assignment
			if (idx != NULL && idx->isError()) {
				variable->Release();
				return idx;
			}
			relvar = true;
		}

		if (idx != NULL) {
			value = instructions.vecteur[1]->Get(environment, aNULL, idthread);

			//for instance: v[0]=v[0][1];
			//we need to prevent a Clear() on "variable" to delete this element...
			value->Setprotect(true);

			idx = idx->Put(variable, value, idthread);

			value->Resetreferencenoprotect(0);

			if (relvar)
				variable->Release();
			
			if (idx->isError())
				return idx;

			return aTRUE;
		}
	}

	bool aff = variable->isAffectation();
	if (!aff)
		variable->Setaffectation(true);

	value = instructions.vecteur[1]->Get(variable, aAFFECTATION, idthread);

	if (value == variable) {
		variable->Setaffectation(aff);
		if (globalAtanor->globalLOCK)
			globalAtanor->Triggeronfalse(variable);
		return aTRUE;
	}

	if (value->isError()) {
		if (relvar)
			variable->Release();
		return value;
	}

	//Then we might have an issue, if value is a sub element of variable...
	//for instance: v=v[0];
	//we need to prevent a Clear() on "variable" to delete this element...
	value->Setprotect(true);

	environment = variable->Put(environment, value, idthread);

	variable->Setaffectation(aff);
	
	value->Resetreferencenoprotect(0);

	if (relvar)
		variable->Release();

	if (environment->isError())
		return environment;

	if (globalAtanor->globalLOCK)
		globalAtanor->Triggeronfalse(variable);
	return aTRUE;
}

Atanor* AtanorInstructionSTREAM::Get(Atanor* environment, Atanor* value, short idthread) {
	Atanor* idx = instructions.vecteur[0]->Function();
	Atanor* variable = instructions.vecteur[0]->Get(environment, aNULL, idthread);

	if (instructions.vecteur[1]->Type() == a_callthread) {
		if (idx != NULL && idx->isIndex()) {
			value = variable->Get(environment, idx, idthread);
			if (value == aNOELEMENT) {
				if (variable->isValueContainer()) {
					variable->Put(idx, aNULL, idthread);
					value = variable->Get(environment, idx, idthread);
				}
				else {
					//We create a Self variable...
					value = new AtanorContent(idx->Evaluate(idthread), variable);
				}
			}
		}
		else
			value = variable;

		instructions.vecteur[1]->Push(value);
		instructions.vecteur[1]->Get(environment, value, idthread);
		return aTRUE;
	}

    return AtanorInstructionAFFECTATION::Get(environment,value,idthread);
}

Atanor* AtanorInstructionSWITCH::Get(Atanor* context, Atanor* ke, short idthread) {
	//First our variable
	Atanor* var = instructions.vecteur[0]->Get(context, aNULL, idthread);
	//First case, we have only discrete values...
	if (usekeys) {
		//Then first, we get the string out of our variable...
		string s = var->String();
		var->Release();
		if (keys.find(s) == keys.end()) {
			//we test if we have a default value...
			if (instructions[instructions.size() - 2] == aDEFAULT) {
				return instructions.back()->Get(context, aNULL, idthread);
			}
			return aNULL;
		}
		return instructions[keys[s] + 1]->Get(aNULL, aNULL, idthread);
	}

	//Then the instructions, the odd element is the value to compare with, the even element
	//the instruction to execute.
	int maxid = instructions.size();
	Atanor* result;
	for (int i = 1; i < maxid; i += 2) {
		result = instructions[i]->Get(context, aNULL, idthread);
		ke = instructions[i + 1];

		if (result == aDEFAULT)
			return ke->Get(context, aNULL, idthread);

		if (var->same(result)->Boolean() == true) {
			result->Release();
			return ke->Get(context, aNULL, idthread);
		}

		if (result->isError()) {
			var->Release();
			return result;
		}
		result->Release();
	}
	var->Release();
	return aNULL;
}

Atanor* AtanorInstructionIF::Get(Atanor* context, Atanor* value, short idthread) {
	_debuginitmin(idthread);

	_debugpush(instructions.vecteur[0]);
	Atanor* result = instructions.vecteur[0]->Get(aTRUE, aNULL, idthread);
	_debugpop();

	if (result->isError()) {
		_cleandebug;
		return result;
	}

	bool truevalue = result->Boolean();
	result->Release();

	if (truevalue != negation)
		result = instructions.vecteur[1];
	else
	if (instructions.size() == 3)
		result = instructions[2];
	else {
		_cleandebug;
		return aNULL;
	}

	_debugpush(result);
	value = result->Get(context, value, idthread);
	_debugpop();
	_cleandebug;
	return value;
}

Atanor* AtanorInstructionOR::Get(Atanor* context, Atanor* result, short idthread) {
	short maxid = instructions.size();
	for (short i = 0; i < maxid; i++) {
		context = instructions[i];
		result = context->Get(aTRUE, aNULL, idthread);
		if (result->Boolean() != context->isNegation()) {
			result->Release();
			return aTRUE;
		}
		result->Release();
	}
	return aFALSE;
}

Atanor* AtanorInstructionAND::Get(Atanor* context, Atanor* result, short idthread) {
	short maxid = instructions.size();
	for (short i = 0; i < maxid; i++) {
		context = instructions[i];
		result = context->Get(aTRUE, aNULL, idthread);
		if (result->Boolean() == context->isNegation()) {
			result->Release();
			return aFALSE;
		}
		result->Release();
	}
	return aTRUE;
}



Atanor* AtanorInstructionCOMPARESHORT::Get(Atanor* context, Atanor* res, short idthread) {

	short left = instructions.vecteur[0]->Getshort(idthread);
	short right = instructions.vecteur[1]->Getshort(idthread);

	res = aFALSE;

	switch (action) {
	case a_less:
		if (left < right)
			res = aTRUE;
		break;
	case a_more:
		if (left > right)
			res = aTRUE;
		break;
	case a_same:
		if (left == right)
			res = aTRUE;
		break;
	case a_different:
		if (left != right)
			res = aTRUE;
		break;
	case a_lessequal:
		if (left <= right)
			res = aTRUE;
		break;
	case a_moreequal:
		if (left >= right)
			res = aTRUE;
	}

	if (negation == true) {
		if (res == aTRUE)
			return aFALSE;
		return aTRUE;
	}

	return res;
}

Atanor* AtanorInstructionCOMPAREINTEGER::Get(Atanor* context, Atanor* res, short idthread) {

	long left = instructions.vecteur[0]->Getinteger(idthread);
	long right = instructions.vecteur[1]->Getinteger(idthread);

	res = aFALSE;

	switch (action) {
	case a_less:
		if (left < right)
			res = aTRUE;
		break;
	case a_more:
		if (left > right)
			res = aTRUE;
		break;
	case a_same:
		if (left == right)
			res = aTRUE;
		break;
	case a_different:
		if (left != right)
			res = aTRUE;
		break;
	case a_lessequal:
		if (left <= right)
			res = aTRUE;
		break;
	case a_moreequal:
		if (left >= right)
			res = aTRUE;
	}

	if (negation == true) {
		if (res == aTRUE)
			return aFALSE;
		return aTRUE;
	}

	return res;
}

Atanor* AtanorInstructionCOMPARELONG::Get(Atanor* context, Atanor* res, short idthread) {

	BLONG left = instructions.vecteur[0]->Getlong(idthread);
	BLONG right = instructions.vecteur[1]->Getlong(idthread);

	res = aFALSE;

	switch (action) {
	case a_less:
		if (left < right)
			res = aTRUE;
		break;
	case a_more:
		if (left > right)
			res = aTRUE;
		break;
	case a_same:
		if (left == right)
			res = aTRUE;
		break;
	case a_different:
		if (left != right)
			res = aTRUE;
		break;
	case a_lessequal:
		if (left <= right)
			res = aTRUE;
		break;
	case a_moreequal:
		if (left >= right)
			res = aTRUE;
	}

	if (negation == true) {
		if (res == aTRUE)
			return aFALSE;
		return aTRUE;
	}

	return res;
}

Atanor* AtanorInstructionCOMPAREDECIMAL::Get(Atanor* context, Atanor* res, short idthread) {

	float left = instructions.vecteur[0]->Getdecimal(idthread);
	float right = instructions.vecteur[1]->Getdecimal(idthread);

	res = aFALSE;

	switch (action) {
	case a_less:
		if (left < right)
			res = aTRUE;
		break;
	case a_more:
		if (left > right)
			res = aTRUE;
		break;
	case a_same:
		if (left == right)
			res = aTRUE;
		break;
	case a_different:
		if (left != right)
			res = aTRUE;
		break;
	case a_lessequal:
		if (left <= right)
			res = aTRUE;
		break;
	case a_moreequal:
		if (left >= right)
			res = aTRUE;
	}

	if (negation == true) {
		if (res == aTRUE)
			return aFALSE;
		return aTRUE;
	}

	return res;
}

Atanor* AtanorInstructionCOMPAREFLOAT::Get(Atanor* aright, Atanor* res, short idthread) {

	double left = instructions.vecteur[0]->Getfloat(idthread);
	double right = instructions.vecteur[1]->Getfloat(idthread);


	res = aFALSE;

	switch (action) {
	case a_less:
		if (left < right)
			res = aTRUE;
		break;
	case a_more:
		if (left > right)
			res = aTRUE;
		break;
	case a_same:
		if (left == right)
			res = aTRUE;
		break;
	case a_different:
		if (left != right)
			res = aTRUE;
		break;
	case a_lessequal:
		if (left <= right)
			res = aTRUE;
		break;
	case a_moreequal:
		if (left >= right)
			res = aTRUE;
	}

	if (negation == true) {
		if (res == aTRUE)
			return aFALSE;
		return aTRUE;
	}

	return res;
}



Atanor* AtanorInstructionCOMPARE::Get(Atanor* right, Atanor* res, short idthread) {

	Atanor* left = instructions.vecteur[0]->Get(right, res, idthread);
	right = instructions.vecteur[1]->Get(right, res, idthread);
	res = aFALSE;

	switch (action) {
	case a_less:
		res = left->less(right);
		break;
	case a_more:
		res = left->more(right);
		break;
	case a_same:
		res = left->same(right);
		break;
	case a_different:
		res = left->different(right);
		break;
	case a_lessequal:
		res = left->lessequal(right);
		break;
	case a_moreequal:
		res = left->moreequal(right);
	}

	left->Release();
	right->Release();

	if (negation == true) {
		bool vbool = res->Boolean();
		res->Release();
		if (vbool)
			return aFALSE;
		return aTRUE;
	}

	return res;
}

Atanor* AtanorInstructionWHILE::Get(Atanor* context, Atanor* result, short idthread) {

	result = instructions.vecteur[0]->Get(aTRUE, aNULL, idthread);

	if (result->Boolean() == result->isNegation()) {
		result->Release();
		return aNULL;
	}

	Atanor* ktest = instructions.vecteur[0];
	Atanor* bloc = instructions.vecteur[1];

	_debuginit(idthread, this);


	while (result->Boolean() != result->isNegation()) {

		result->Release();

		_debugpush(bloc);
		result = bloc->Get(context, aNULL, idthread);
		_debugpop();

		if (result->isReturned()) {
			_cleandebug;
			return result;
		}

		if (result->isError() || globalAtanor->Error(idthread)) {
			_cleandebug;

			if (result == aBREAK)
				return aTRUE;
			return globalAtanor->Errorobject(idthread);
		}

		result->Release();
		result = ktest->Get(aTRUE, aNULL, idthread);
	}

	result->Release();
	_cleandebug;
	return aTRUE;
}


Atanor* AtanorInstructionUNTIL::Get(Atanor* context, Atanor* result, short idthread) {

	result = instructions.vecteur[0]->Get(aTRUE, aNULL, idthread);

	if (result->Boolean() == result->isNegation()) {
		result->Release();
		return aNULL;
	}

	Atanor* ktest = instructions.vecteur[0];
	Atanor* bloc = instructions.vecteur[1];

	_debuginitmin(idthread);

	do {

		result->Release();

		_debugpush(bloc);
		result = bloc->Get(context, aNULL, idthread);
		_debugpop();

		if (result->isReturned()) {
			_cleandebug;
			return result;
		}

		if (result->isError() || globalAtanor->Error(idthread)) {
			_cleandebug;
			if (result == aBREAK)
				return aTRUE;
			return globalAtanor->Errorobject(idthread);
		}

		result->Release();
		result = ktest->Get(aTRUE, aNULL, idthread);
	} while (result->Boolean() != result->isNegation());

	result->Release();
	_cleandebug;
	return aTRUE;
}

Atanor* AtanorInstructionIN::Get(Atanor* context, Atanor* value, short idthread) {
	value = instructions.vecteur[0]->Get(context, aNULL, idthread);
	Atanor* object = instructions.vecteur[1]->Get(context, aNULL, idthread);

	Atanor* res = object->in(context, value, idthread);
	value->Release();
	if (action == a_notin) {
		if (res->Boolean())
			return aFALSE;
		return aTRUE;
	}

	return res;
}

Atanor* AtanorInstructionFORINVALUECONTAINER::Get(Atanor* context, Atanor* loop, short idthread) {
	loop = instructions.vecteur[0]->Instruction(1)->Get(context, aNULL, idthread);
	context = loop->Loopin(this, context, idthread);
	loop->Release();
	return context;
}

Atanor* AtanorInstructionFORINVECTOR::Get(Atanor* context, Atanor* loop, short idthread) {
	loop = instructions.vecteur[0];
	Atanor* var = loop->Instruction(0)->Get(context, aNULL, idthread);
	if (var->isFrame())
		return AtanorInstructionFORIN::Get(context, loop, idthread);

	loop = loop->Instruction(1)->Get(context, aNULL, idthread);
	
	Atanor* a;	
	Atanor* v;

	for (long i = 0; i < loop->Size(); i++) {
		v = loop->getvalue(i);
		var->Putvalue(v, idthread);

		a = instructions.vecteur[1]->Get(context, aNULL, idthread);

		//Continue does not trigger needInvestigate
		if (a->needInvestigate()) {
			if (a == aBREAK)
				break;		
			loop->Release();
			return a;
		}

		a->Release();
	}

	loop->Release();
	return this;
}

Atanor* AtanorInstructionFORIN::Get(Atanor* context, Atanor* loop, short idthread) {

	loop = instructions.vecteur[0]->Instruction(0);
	Atanor* var = loop->Get(context, aNULL, idthread);

	short idname = 0;
	short typevar = var->Type();
	//In some cases, we cannot duplicate the value...
	Atanor* dom = NULL;

	if (var->isFrame()) {
		idname = loop->Name();
		dom = globalAtanor->Declarator(idname, idthread);
	}

	loop = instructions.vecteur[0]->Instruction(1)->Get(context, aNULL, idthread);

	bool cleanup = true;
	if (loop->isValueContainer() || !var->isLetSelf())
		cleanup = false;

	Atanor* v;
	Atanor* a;

	if (loop->isVectorContainer()) {
		for (long i = 0; i < loop->Size(); i++) {
			v = loop->getvalue(i);

			if (dom != NULL) {
				if (!globalAtanor->Compatible(v->Type(), typevar)) {
					v->Release();
					dom->Declare(idname, var);
					loop->Release();
					return globalAtanor->Returnerror("Incompatible type in loop", idthread);
				}
				dom->Declare(idname, v);
			}
			else {
				if (cleanup)
					var->Forcedclean();
				var->Putvalue(v, idthread);
			}

			a = instructions.vecteur[1]->Get(context, aNULL, idthread);

			//Continue does not trigger needInvestigate
			if (a->needInvestigate()) {
				if (a == aBREAK)
					break;

				if (dom != NULL)
					dom->Declare(idname, var);
				v->Release();
				loop->Release();
				return a;
			}

			a->Release();
		}

		if (dom != NULL)
			dom->Declare(idname, var);

		loop->Release();
		return this;
	}

	bool getval = true;
	if (loop->isMapContainer())
		getval = false;


	AtanorIteration* it = loop->Newiteration(false);
	if (it == aITERNULL) {
		if (globalAtanor->erroronkey)
			return globalAtanor->Returnerror("Cannot loop on a this value", idthread);
		return this;
	}

	for (it->Begin(); it->End() != aTRUE; it->Next()) {
		if (getval)
			v = it->IteratorValue();
		else
			v = it->IteratorKey();

		if (dom != NULL) {
			if (!globalAtanor->Compatible(v->Type(), typevar)) {
				dom->Declare(idname, var);
				delete it;
				loop->Release();
				return globalAtanor->Returnerror("Incompatible type in loop", idthread);
			}
			dom->Declare(idname, v);
		}
		else {
			if (cleanup)
				var->Forcedclean();
			var->Putvalue(v, idthread);
		}

		a = instructions.vecteur[1]->Get(context, aNULL, idthread);

		//Continue does not trigger needInvestigate
		if (a->needInvestigate()) {
			if (a == aBREAK)
				break;

			if (dom != NULL)
				dom->Declare(idname, var);
			loop->Release();
			delete it;
			return a;
		}

		a->Release();
	}

	if (dom != NULL)
		dom->Declare(idname, var);

	delete it;
	loop->Release();
	return this;
}

Atanor* AtanorInstructionFORVECTORIN::Get(Atanor* context, Atanor* loop, short idthread) {

	Atanor* var = instructions.vecteur[0]->Instruction(0)->Get(context, aNULL, idthread);
	loop = instructions.vecteur[0]->Instruction(1)->Get(context, aNULL, idthread);
	if (!loop->isVectorContainer())
		return globalAtanor->Returnerror("Can only loop on vectors here", idthread);

	AtanorIteration* it = loop->Newiteration(false);

	Atanor* a;
	for (it->Begin(); it->End() != aTRUE; it->Next()) {
		var->Setvalue(aNULL, it->IteratorValue(), idthread);

		a = instructions.vecteur[1]->Get(context, aNULL, idthread);

		if (a->needInvestigate()) {
			if (a == aBREAK)
				break;
			loop->Release();
			it->Release();
			return a;
		}

		a->Release();
	}


	loop->Release();
	it->Release();

	return this;
}

Atanor* AtanorInstructionFORMAPIN::Get(Atanor* context, Atanor* loop, short idthread) {
	Atanor* var = instructions.vecteur[0]->Instruction(0)->Get(context, aNULL, idthread);
	loop = instructions.vecteur[0]->Instruction(1)->Get(context, aNULL, idthread);
	if (!loop->isMapContainer())
		return globalAtanor->Returnerror("Can only loop on a map here", idthread);

	AtanorIteration* it = loop->Newiteration(false);

	Atanor* a;
	for (it->Begin(); it->End() != aTRUE; it->Next()) {
		var->Setvalue(it, aNULL, idthread);

		a = instructions.vecteur[1]->Get(context, aNULL, idthread);

		if (a->needInvestigate()) {
			if (a == aBREAK)
				break;
			loop->Release();
			it->Release();
			return a;
		}

		a->Release();
	}

	loop->Release();
	it->Release();

	return aNULL;
}

Atanor* AtanorInstructionFILEIN::Get(Atanor* context, Atanor* loop, short idthread) {
	Atanor* var = instructions.vecteur[0]->Instruction(0)->Get(context, aNULL, idthread);
	loop = instructions.vecteur[0]->Instruction(1)->Get(context, aNULL, idthread);

	Atanor* a;

	while (loop->Get(var, aNULL, idthread)->Boolean()) {
		a = instructions.vecteur[1]->Get(context, aNULL, idthread);

		if (a->needInvestigate()) {
			if (a == aBREAK)
				break;

			return a;
		}

		a->Release();
	}

	return aNULL;
}

Atanor* AtanorInstructionFOR::Get(Atanor* context, Atanor* stop, short idthread) {
	Atanor* a = instructions.vecteur[0]->Get(context, aNULL, idthread);
	if (a->isError())
		return a;

	stop = instructions.vecteur[1];

	while (stop->Get(context, aNULL, idthread)->Boolean() != stop->isNegation()) {
		a = instructions[3]->Get(context, aNULL, idthread);


		if (a->needInvestigate()) {
			if (a == aBREAK)
				break;

			return a;
		}

		a->Release();

		instructions[2]->Get(context, aNULL, idthread);
	}
	return aNULL;
}

Atanor* AtanorInstructionFORINRANGE::Get(Atanor* context, Atanor* value, short idthread) {
	value = instructions.vecteur[0]->Instruction(0)->Get(context, aNULL, idthread);

	switch (value->Type()) {
	case a_int:
		return ExecuteInteger((Atanorint*)value, context, idthread);
	case a_float:
		return ExecuteFloat((Atanorfloat*)value, context, idthread);
	case a_decimal:
		return ExecuteDecimal((Atanordecimal*)value, context, idthread);
	case a_long:
		return ExecuteLong((Atanorlong*)value, context, idthread);
	case a_short:
		return ExecuteShort((Atanorshort*)value, context, idthread);
	}

	Atanorint v(0);
	return ExecuteInteger(&v, context, idthread);
}

Atanor* AtanorInstructionFORINRANGE::ExecuteInteger(Atanorint* value, Atanor* context, short idthread) {
	long v, t, i;

	value->Setreference();

	v = instructions.vecteur[0]->Instruction(1)->Getinteger(context, aNULL, idthread);
	t = instructions.vecteur[0]->Instruction(2)->Getinteger(context, aNULL, idthread);
	i = instructions.vecteur[0]->Instruction(3)->Getinteger(context, aNULL, idthread);

	Atanor* a;
	if (i < 0) {
		for (; v > t; v += i) {
			value->value = v;
			a = instructions.vecteur[1]->Get(context, aNULL, idthread);


			if (a->needInvestigate()) {
				value->Resetreference();
				if (a == aBREAK)
					return aNULL;

				return a;
			}

			a->Release();
		}

		value->Resetreference();
		return aNULL;
	}

	for (; v < t; v += i) {
		value->value = v;
		a = instructions.vecteur[1]->Get(context, aNULL, idthread);


		if (a->needInvestigate()) {
			value->Resetreference();
			if (a == aBREAK)
				return aNULL;

			return a;
		}

		a->Release();
	}

	value->Resetreference();
	return aNULL;
}

Atanor* AtanorInstructionFORINRANGE::ExecuteDecimal(Atanordecimal* value, Atanor* context, short idthread) {
	float v, t, i;
	value->Setreference();

	v = instructions.vecteur[0]->Instruction(1)->Getdecimal(context, aNULL, idthread);
	t = instructions.vecteur[0]->Instruction(2)->Getdecimal(context, aNULL, idthread);
	i = instructions.vecteur[0]->Instruction(3)->Getdecimal(context, aNULL, idthread);

	Atanor* a;
	if (i < 0) {
		for (; v > t; v += i) {
			value->value = v;
			a = instructions.vecteur[1]->Get(context, aNULL, idthread);

			if (a->needInvestigate()) {
				value->Resetreference();
				if (a == aBREAK)
					return aNULL;

				return a;
			}

			a->Release();
		}

		value->Resetreference();
		return aNULL;
	}

	for (; v < t; v += i) {
		value->value = v;
		a = instructions.vecteur[1]->Get(context, aNULL, idthread);


		if (a->needInvestigate()) {
			value->Resetreference();
			if (a == aBREAK)
				return aNULL;

			return a;
		}


		a->Release();
	}

	value->Resetreference();
	return aNULL;
}

Atanor* AtanorInstructionFORINRANGE::ExecuteFloat(Atanorfloat* value, Atanor* context, short idthread) {
	double v, t, i;

	value->Setreference();

	v = instructions.vecteur[0]->Instruction(1)->Getfloat(context, aNULL, idthread);
	t = instructions.vecteur[0]->Instruction(2)->Getfloat(context, aNULL, idthread);
	i = instructions.vecteur[0]->Instruction(3)->Getfloat(context, aNULL, idthread);

	Atanor* a;
	if (i < 0) {
		for (; v > t; v += i) {
			value->value = v;
			a = instructions.vecteur[1]->Get(context, aNULL, idthread);

			if (a->needInvestigate()) {
				value->Resetreference();
				if (a == aBREAK)
					return aNULL;

				return a;
			}

			a->Release();
		}

		value->Resetreference();
		return aNULL;
	}

	for (; v < t; v += i) {
		value->value = v;
		a = instructions.vecteur[1]->Get(context, aNULL, idthread);

		if (a->needInvestigate()) {
			value->Resetreference();
			if (a == aBREAK)
				return aNULL;

			return a;
		}

		a->Release();
	}

	value->Resetreference();
	return aNULL;
}

Atanor* AtanorInstructionFORINRANGE::ExecuteLong(Atanorlong* value, Atanor* context, short idthread) {
	BLONG v, t, i;

	value->Setreference();

	v = instructions.vecteur[0]->Instruction(1)->Getlong(context, aNULL, idthread);
	t = instructions.vecteur[0]->Instruction(2)->Getlong(context, aNULL, idthread);
	i = instructions.vecteur[0]->Instruction(3)->Getlong(context, aNULL, idthread);

	Atanor* a;
	if (i < 0) {
		for (; v > t; v += i) {
			value->value = v;
			a = instructions.vecteur[1]->Get(context, aNULL, idthread);


			if (a->needInvestigate()) {
				value->Resetreference();
				if (a == aBREAK)
					return aNULL;

				return a;
			}

			a->Release();
		}

		value->Resetreference();
		return aNULL;
	}

	for (; v < t; v += i) {
		value->value = v;
		a = instructions.vecteur[1]->Get(context, aNULL, idthread);


		if (a->needInvestigate()) {
			value->Resetreference();
			if (a == aBREAK)
				return aNULL;

			return a;
		}

		a->Release();
	}

	value->Resetreference();
	return aNULL;
}

Atanor* AtanorInstructionFORINRANGE::ExecuteShort(Atanorshort* value, Atanor* context, short idthread) {
	short v, t, i;

	value->Setreference();

	v = instructions.vecteur[0]->Instruction(1)->Getshort(context, aNULL, idthread);
	t = instructions.vecteur[0]->Instruction(2)->Getshort(context, aNULL, idthread);
	i = instructions.vecteur[0]->Instruction(3)->Getshort(context, aNULL, idthread);

	Atanor* a;
	if (i < 0) {
		for (; v > t; v += i) {
			value->value = v;
			a = instructions.vecteur[1]->Get(context, aNULL, idthread);


			if (a->needInvestigate()) {
				value->Resetreference();
				if (a == aBREAK)
					return aNULL;

				return a;
			}

			a->Release();
		}

		value->Resetreference();
		return aNULL;
	}

	for (; v < t; v += i) {
		value->value = v;
		a = instructions.vecteur[1]->Get(context, aNULL, idthread);

		if (a->needInvestigate()) {
			value->Resetreference();
			if (a == aBREAK)
				return aNULL;

			return a;
		}

		a->Release();
	}

	value->Resetreference();
	return aNULL;
}

Atanor* AtanorInstructionFORINRANGEINTEGER::Get(Atanor* context, Atanor* a, short idthread) {
	long v, t, i;

	v = init->Getinteger(idthread);
	t = test->Getinteger(idthread);
	i = increment->Getinteger(idthread);

	Atanorint* value = (Atanorint*)recipient->Get(context, aNULL, idthread);

	if (i < 0) {
		for (; v > t; v += i) {
			value->value = v;
			a = instruction->Get(context, aNULL, idthread);


			if (a->needInvestigate()) {
				if (a == aBREAK)
					break;

				return a;
			}

			a->Release();
		}

		return aNULL;
	}

	for (; v < t; v += i) {
		value->value = v;
		a = instruction->Get(context, aNULL, idthread);


		if (a->needInvestigate()) {
			if (a == aBREAK)
				break;

			return a;
		}

		a->Release();
	}

	return aNULL;
}

Atanor* AtanorInstructionFORINRANGEDECIMAL::Get(Atanor* context, Atanor* a, short idthread) {
	float v, t, i;

	v = init->Getdecimal(idthread);
	t = test->Getdecimal(idthread);
	i = increment->Getdecimal(idthread);

	Atanordecimal* value = (Atanordecimal*)recipient->Get(context, aNULL, idthread);

	if (i < 0) {
		for (; v > t; v += i) {
			value->value = v;
			a = instruction->Get(context, aNULL, idthread);

			if (a->needInvestigate()) {
				if (a == aBREAK)
					break;

				return a;
			}

			a->Release();
		}

		return aNULL;
	}

	for (; v < t; v += i) {
		value->value = v;
		a = instruction->Get(context, aNULL, idthread);


		if (a->needInvestigate()) {
			if (a == aBREAK)
				break;

			return a;
		}


		a->Release();
	}

	return aNULL;
}

Atanor* AtanorInstructionFORINRANGEFLOAT::Get(Atanor* context, Atanor* a, short idthread) {
	double v, t, i;
	
	v = init->Getfloat(idthread);
	t = test->Getfloat(idthread);
	i = increment->Getfloat(idthread);

	Atanorfloat* value = (Atanorfloat*)recipient->Get(context, aNULL, idthread);
	if (i < 0) {
		for (; v > t; v += i) {
			value->value = v;
			a = instruction->Get(context, aNULL, idthread);

			if (a->needInvestigate()) {
				if (a == aBREAK)
					break;

				return a;
			}

			a->Release();
		}

		return aNULL;
	}

	for (; v < t; v += i) {
		value->value = v;
		a = instruction->Get(context, aNULL, idthread);

		if (a->needInvestigate()) {
			if (a == aBREAK)
				break;

			return a;
		}

		a->Release();
	}

	return aNULL;
}

Atanor* AtanorInstructionFORINRANGELONG::Get(Atanor* context, Atanor* a, short idthread) {
	BLONG v, t, i;

	v = init->Getlong(idthread);
	t = test->Getlong(idthread);
	i = increment->Getlong(idthread);

	Atanorlong* value = (Atanorlong*)recipient->Get(context, aNULL, idthread);
	if (i < 0) {
		for (; v > t; v += i) {
			value->value = v;
			a = instruction->Get(context, aNULL, idthread);


			if (a->needInvestigate()) {
				if (a == aBREAK)
					break;

				return a;
			}

			a->Release();
		}

		return aNULL;
	}

	for (; v < t; v += i) {
		value->value = v;
		a = instruction->Get(context, aNULL, idthread);


		if (a->needInvestigate()) {
			if (a == aBREAK)
				break;

			return a;
		}

		a->Release();
	}

	return aNULL;
}

Atanor* AtanorInstructionFORINRANGESHORT::Get(Atanor* context, Atanor* a, short idthread) {
	short v, t, i;

	v = init->Getshort(idthread);
	t = test->Getshort(idthread);
	i = increment->Getshort(idthread);

	Atanorshort* value = (Atanorshort*)recipient->Get(context, aNULL, idthread);
	if (i < 0) {
		for (; v > t; v += i) {
			value->value = v;
			a = instruction->Get(context, aNULL, idthread);


			if (a->needInvestigate()) {
				if (a == aBREAK)
					break;

				return a;
			}

			a->Release();
		}

		return aNULL;
	}

	for (; v < t; v += i) {
		value->value = v;
		a = instruction->Get(context, aNULL, idthread);

		if (a->needInvestigate()) {
			if (a == aBREAK)
				break;

			return a;
		}

		a->Release();
	}

	return aNULL;
}

Atanor* AtanorInstructionTRY::Get(Atanor* res, Atanor* ins, short idthread) {

	short last = instructions.size() - 1;
	bool catchbloc = false;
	if (instructions[last]->Action() == a_catchbloc) {
		last--;
		catchbloc = true;
	}

	for (short i = 0; i < last; i++) {
		res = instructions[i]->Get(aNULL, aNULL, idthread);

		if (res->isError()) {
			if (res == aBREAK || res == aCONTINUE)
				return res;
			
			instructions[last]->Get(aNULL, aNULL, idthread);
			res = aFALSE;
			if (catchbloc)
				res = instructions[last + 1]->Get(aNULL, aNULL, idthread);
			return res;
		}

		if (res->isReturned())
			return res;

		if (globalAtanor->Stop()) {
			res->Release();
			return aNULL;
		}

		res->Release();
	}
	return aNULL;
}


Atanor* AtanorInstructionCATCH::Get(Atanor* context, Atanor* a, short idthread) {
	short size = instructions.size();

	_debuginit(idthread, this);

	if (size == 1) {
		a = instructions.vecteur[0]->Get(context, aNULL, idthread);
		_cleandebug;
		return a;
	}

	for (short i = 0; i < size; i++) {
		a = instructions[i];

		_debugpush(a);
		a = a->Get(aNULL, aNULL, idthread);
		_debugpop();

		if (a->needFullInvestigate()) {
			_cleandebug;
			return a;
		}

		a->Release();
	}

	_cleandebug;
	return aNULL;
}
