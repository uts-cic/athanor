
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
filename   : atanor.cxx
Date       : 2017/04/01
Purpose    : The place of all initializations...
Programmer : Claude ROUX
Reviewer   :
*/


#include "atanor.h"
#include "compilecode.h"
#include "atanoratanor.h"
#include "constobjects.h"
#include "atanorbyte.h"
#include "atanorint.h"
#include "atanorfloat.h"
#include "atanorstring.h"
#include "atanorustring.h"
#include "atanorfraction.h"
#include "atanordecimal.h"
#include "instructions.h"
#include "atanormapss.h"
#include "atanorivector.h"
#include "atanorfvector.h"
#include "atanorsvector.h"
#include "atanoruvector.h"
#include "predicate.h"
#include <memory>
#include "vecte.h"

//----------------------------------------------------------------------------------
const char* atanor_version = "ATANOR 0.84 build 10";

extern "C" {
Exporting const char* AtanorVersion() {
	return atanor_version;
}
}

//----------------------------------------------------------------------------------
Exporting AtanorGlobal* globalAtanor = NULL;
//----------------------------------------------------------------------------------
vector<string> AtanorGlobal::arguments;
//----------------------------------------------------------------------------------

// Debug area, to detect potential memory leaks...
#ifdef _DEBUG

static VECTE<Atanor*> garbage;
ThreadLock _garbaging(NULL, false);

Exporting Atanor::Atanor() {
	idtracker = -1;
	//Locking _lock(_garbaging);
	//iddebug = garbage.size();
	//garbage.push_back(this);
}

Exporting Atanor::~Atanor() {
	Locking _lock(_garbaging);
	//garbage.vecteur[iddebug] = NULL;
}

void EvaluateGarbage() {
	vector<Atanor*> issues;
	vector<long> idissues;
	for (size_t i = 0; i < garbage.size(); i++) {
		if (garbage[i] != NULL) {
			issues.push_back(garbage[i]);
			idissues.push_back(i);
		}
	}
	cerr << issues.size() << endl;
}

#endif

//----------------------------------------------------------------------------------
Exporting long ThreadLock::ids = 0;

Exporting ThreadLock::ThreadLock(std::recursive_mutex* l, bool run, bool init) {
	id = ids++;
	idthread = 0;
	lock = l;
	recursive = init;
	locked = run;
	if (run)
		lock->lock();
	else
	if (lock == NULL)
		lock = new std::recursive_mutex();
}

void ThreadLock::Initialize() {
	lock = new std::recursive_mutex();
}

Exporting void ThreadLock::Locking() {
	lock->lock();
}

Exporting void ThreadLock::Unlocking() {
	lock->unlock();
}

Exporting ThreadLock::~ThreadLock() {
	if (locked)
		lock->unlock();
	else
	if (lock != NULL)
		delete lock;
}

//----------------------------------------------------------------------------------
// Locked variables...

Exporting void LockedThread::Blocked() {
	std::unique_lock<std::mutex> lck(mtx);
	released = 1;
	lock.wait(lck);
	released = 2;
}

Exporting void LockedThread::Released() {
	if (released == 1)
		lock.notify_all();
}

Exporting LockedThread::~LockedThread() {
	if (released == 1)
		lock.notify_all();
}

//----------------------------------------------------------------------------------

AtanorRaiseError::AtanorRaiseError(stringstream& mes, string file, size_t l, size_t r) {
	message = mes.str();

	filename = file;
	left = l;
	right = r;
}

//----------------------------------------------------------------------------------
ThreadStruct::ThreadStruct() : stack(1000), variables(false) {
	prologstack = 0;
	returnvalue = NULL;
    previousinstruction=NULL;
	currentinstruction = NULL;
	errorraised = NULL;
	error = false;
	used = false;
	parentthreads = -1;
	handle = _GETTHREADID();
}

void ThreadStruct::Update(short idthread) {	
	variables = globalAtanor->threads[idthread].variables;	
}

void ThreadStruct::Cleanerror() {
	errorraised = NULL;
	error = false;
}

void ThreadStruct::Clear() {
	used = false;
	error = false;
	prologstack = 0;
	errorraised = NULL;
	stack.clear();
	message.str("");
	joined.clear();
	variables.clear();
	returnvalue = aNULL;	
}

threadhandle ThreadStruct::Initialization() {
	prologstack = 0;
	returnvalue = aNULL;
	errorraised = NULL;
	used = true;

	locks.clear();

	handle = _GETTHREADID();
	error = false;
	message.str("");
	debugstack.clear();
    previousinstruction=NULL;
	currentinstruction = NULL;
	return handle;
}

Atanor* ThreadStruct::Raiserror(AtanorError* err) {
	if (error)
		return errorraised;

	errorraised = err;
	error = true;
	return errorraised;
}


void ThreadStruct::Setknowledgebase() {
	Locking _lock(globalAtanor->_knowledgelock);
	if (globalAtanor->threads[0].knowledgebase.size()) {
		long i;

		for (auto& it : globalAtanor->threads[0].knowledgebase) {
			for (i = 0; i < it.second.size(); i++) {
				knowledgebase[it.first].push_back(it.second[i]);
				it.second[i]->Setreference();
			}
		}
	}
}


void ThreadStruct::Clearknowledgebase() {
	for (auto& itk : knowledgebase) {
		for (size_t i = 0; i < itk.second.size(); i++) {
			itk.second[i]->Setprotect(false);
			itk.second[i]->Resetreference();
		}
	}

	knowledgebase.clear();
}

Exporting ThreadStruct::~ThreadStruct() {
	Clearknowledgebase();
}

//----------------------------------------------------------------------------------
AtanorGlobal::AtanorGlobal(long nb) : 
	idSymbols(false), methods(false), compatibilities(false), 
	operator_strings(false), terms(false), 
	_locker(NULL, false), _join(NULL, false), _call(NULL, false), _printlock(NULL, false), _knowledgelock(NULL, false) {

	maxrange = 100000;

	globalLOCK = false;
	Update();

	ThreadLock::ids = 0;

	maxstack = 1000;
	maxjoined = 1024;

	debugmode = false;
	trackercursor = 0;
	inittableutf8();
	currentbnf = NULL;
	maxthreads = nb;
	erroronkey = false;
	windowmode = false;
	spaceid = -1;
	running = false;
	threads = new ThreadStruct[maxthreads];

	predicateContainer = NULL;

	linereference = 0;
	lineerror = -1;

	kstd = NULL;
	kerr = NULL;

	RecordConstantNames();
	RecordSystemVariables();

	RecordCompileFunctions();
	RecordObjects();
	RecordContainers();
	RecordProcedures();
	RecordPredicates();
	RecordPredicateFunctions();

	RecordMathFunctions();
	RecordCompatibilities();
	//---------------------------------
	//We use it as a way to deflect printing to a string...
	stringbuffer = NULL;
	stringbuffererror = NULL;
	os = &cout;
	doubledisplay = false;
	displayfunction = NULL;
	displayobject = NULL;
	executionbreak = false;
	//---------------------------------
	debugfunction = NULL;
	debugdata = NULL;
	terminationfunction = NULL;

	//---------------------------------
	long mx = 1000;
	//---------------------------------
	for (long i = 0; i < mx; i++) {
		intreservoire.push_back(new Atanorintbuff(i));
		floatreservoire.push_back(new Atanorfloatbuff(i));
		stringreservoire.push_back(new Atanorstringbuff(i));
		ustringreservoire.push_back(new Atanorustringbuff(i));
	}
	intlock = (AtanorObject*)newInstance[a_int];
	floatlock = (AtanorObject*)newInstance[a_float];
	stringlock = (AtanorObject*)newInstance[a_string];
	ustringlock = (AtanorObject*)newInstance[a_ustring];
	intidx = 0;
	floatidx = 0;
	stringidx = 0;
	ustringidx = 0;
	//---------------------------------
	mx >>= 3;
	for (long i = 0; i < mx; i++) {
		indexreservoire.push_back(new AtanorIndexbuff(i));
		vectorreservoire.push_back(new Atanorvectorbuff(i));
		mapreservoire.push_back(new Atanormapbuff(i));
		mapssreservoire.push_back(new Atanormapssbuff(i));
		ivectorreservoire.push_back(new Atanorivectorbuff(i));
		fvectorreservoire.push_back(new Atanorfvectorbuff(i));
		svectorreservoire.push_back(new Atanorsvectorbuff(i));
		uvectorreservoire.push_back(new Atanoruvectorbuff(i));
	}

	//we need an object, let's pick a Boolean...
	indexlock = new Atanorbool(true, this);

	mapsslock = (AtanorObject*)newInstance[a_mapss];
	maplock = (AtanorObject*)newInstance[a_map];
	vectorlock = (AtanorObject*)newInstance[a_vector];
	ivectorlock = (AtanorObject*)newInstance[a_ivector];
	fvectorlock = (AtanorObject*)newInstance[a_fvector];
	svectorlock = (AtanorObject*)newInstance[a_svector];
	uvectorlock = (AtanorObject*)newInstance[a_uvector];

	indexidx = 0;
	mapidx = 0;
	mapssidx = 0;
	vectoridx = 0;
	ivectoridx = 0;
	fvectoridx = 0;
	svectoridx = 0;
	uvectoridx = 0;
}

AtanorGlobal::~AtanorGlobal() {

	delete[] threads;
	size_t i;

	for (auto & it : rules)
		delete it.second;

	for (i = 0; i < spaces.size(); i++) {
		if (spaces[i] != NULL)
			delete spaces[i];
	}
	spaces.clear();
	for (i = 0; i < tracked.size(); i++) {
		if (tracked[i] != NULL) {
			delete tracked[i];
			tracked[i] = NULL;
		}
	}

	for (i = 0; i < indexreservoire.size(); i++)
		delete indexreservoire[i];

	for (i = 0; i < mapreservoire.size(); i++)
		delete mapreservoire[i];

	for (i = 0; i < mapssreservoire.size(); i++)
		delete mapssreservoire[i];

	for (i = 0; i < vectorreservoire.size(); i++)
		delete vectorreservoire[i];

	for (i = 0; i < ivectorreservoire.size(); i++)
		delete ivectorreservoire[i];
	for (i = 0; i < fvectorreservoire.size(); i++)
		delete fvectorreservoire[i];
	for (i = 0; i < svectorreservoire.size(); i++)
		delete svectorreservoire[i];
	for (i = 0; i < uvectorreservoire.size(); i++)
		delete uvectorreservoire[i];

	for (i = 0; i < intreservoire.size(); i++)
		delete intreservoire[i];
	for (i = 0; i < floatreservoire.size(); i++)
		delete floatreservoire[i];
	for (i = 0; i < stringreservoire.size(); i++)
		delete stringreservoire[i];
	for (i = 0; i < ustringreservoire.size(); i++)
		delete ustringreservoire[i];


	for (auto & it : locktables)
		delete it.second;

	for (auto & it : waitstrings)
		delete it.second;

	for (auto & it : threadvariables)
		delete it.second;


	codes.clear();
	locktables.clear();
	waitstrings.clear();
	idSymbols.clear();
	symbolIds.clear();
	string_operators.clear();
	operator_strings.clear();
	atanOperatorMath.clear();
	parseFunctions.clear();

	newInstance.clear();
	methods.clear();
	allmethods.clear();
	framemethods.clear();
	functions.clear();
	procedures.clear();
	commons.clear();

	arities.clear();
	frames.clear();

	systems.clear();

	compatibilities.clear();

#ifdef _DEBUG
	vector<Atanor*> issues;
	vector<long> idissues;
	for (size_t i = 0; i < garbage.size(); i++) {
		if (garbage[i] != NULL) {
			issues.push_back(garbage[i]);
			idissues.push_back(i);
		}
	}
	garbage.clear();
#endif
}

void AtanorGlobal::Cleanfrom(long mx) {
	for (long i = tracked.size() - 1; i >= mx; i--) {
		delete tracked[i];
		tracked.pop_back();
	}
}


static void getname(string& f) {
#ifdef WIN32
	long pos = f.rfind("\\");
#else
	long pos = f.rfind("/");
#endif

	if (pos != -1)
		f = f.substr(pos + 1, f.size() - pos - 1);
}

Exporting void AtanorGlobal::Getdebuginfo(string& localvariables, string& allvariables, string& stackstring, bool shortname, short idthread) {
	vector<Atanor*>& stack = threads[idthread].debugstack;

	localvariables = "";
	allvariables = "";
	stackstring = "";

	short idcode = stack.back()->Currentspace();
	if (idcode == -1)
		return;

	short fileid = stack.back()->Currentfile();
	string filename = globalAtanor->Getfilename(fileid);
	long currentline = stack.back()->Currentline();
	getname(filename);

	VECTE<Atanor*>& threadstack = threads[idthread].stack;
	long i;
	stringstream sstack;
	sstack << currentline << " in " << filename << "\n";

	string funcname;
	Atanor* a =threads[idthread].currentinstruction;
    if (a!=NULL) {
        vector<short> vars;
        a->ScanVariables(vars);
        
        if (threads[idthread].previousinstruction!=NULL)
            threads[idthread].previousinstruction->ScanVariables(vars);
        
        threads[idthread].previousinstruction=a;
        
        hmap<short,bool> m;
        for (i=0;i<vars.size();i++)
            m[vars[i]]=true;
        vars.clear();
        for (auto& it : m)
            vars.push_back(it.first);
        
        stringstream var;
        
        for (size_t j = 0; j < vars.size(); j++) {
			a = Getdefinition(vars[j], idthread, aNULL);
            
            var << Getsymbol(vars[j]) << " = ";
            if (!shortname)
                var << a->String();
            else {
                string s = a->String();
                if (s.size() >= 30) {
                    s = s.substr(0, 30);
                    s += "...";
                    //we count both ' and "
                    int dq = 0;
                    int sq = 0;
                    for (long i = 0; i < s.size(); i++) {
                        if (s[i] == '\'')
                            sq++;
                        else
                            if (s[i] == '\"')
                                dq++;
                    }
                    if (dq && (dq % 2) != 0)
                        s += "\"";
                    else
                        if (sq && (sq % 2) != 0)
                            s += "'";
                }
                var << s;
            }
            
            var << "\n";
        }
        
        if (var.str().size())
            localvariables += var.str();
    }

    
	for (i = threadstack.size() - 1; i >= 0; i--) {
		stringstream var;
		vector<short> vars;
		
		funcname = "";
		short nm = threadstack[i]->Name();
		if (nm > 0)
			funcname = Getsymbol(nm);
		
		threadstack[i]->Variables(vars);
		for (size_t j = 0; j < vars.size(); j++) {
			a = threadstack[i]->Declaration(vars[j]);

			var << Getsymbol(vars[j]) << " = ";
			if (!shortname)
				var << a->String();
			else {
				string s = a->String();
                Trimright(s);
				if (s.size() >= 30) {
					s = s.substr(0, 30);
					s += "...";
					//we count both ' and "
					int dq = 0;
					int sq = 0;
					for (long i = 0; i < s.size(); i++) {
						if (s[i] == '\'')
							sq++;
						else
						if (s[i] == '\"')
							dq++;
					}
					if (dq && (dq % 2) != 0)
						s += "\"";
					else
					if (sq && (sq % 2) != 0)
						s += "'";
				}
				var << s;
			}

			if (nm > 0)
				var << " //" << funcname;

			var << "\n";
		}
        
		if (var.str().size())
            allvariables += var.str();
		
		if (nm > 0) {
			a = Getdefinition(nm, idthread);

			short idc = threadstack[i]->Currentspace();
			if (idc == -1)
				sstack << "\n";
			else {
				fileid = threadstack[i]->Currentfile();
				filename = Getfilename(fileid);
				getname(filename);
				long cline = threadstack[i]->Currentline();
				sstack << cline << " in " << filename << " (" << funcname << ")\n";
			}
		}
	}

	stackstring = sstack.str();
}

void AtanorGlobal::Releasevariables() {
	//This method is called when we need to stop an Atanor program
	//We then need to remove any wait on variables still active...
	for (auto& it : waitstrings)
		it.second->Released();

	for (auto& it : threadvariables)
		it.second->Released();
}

void AtanorGlobal::Triggeronfalse(Atanor* a) {
	Locking _lock(_call);
	if (a->idtracker != -1 && threadvariables.find(a->idtracker) != threadvariables.end()) {
		if (!a->Protectedboolean()) {
			threadvariables[a->idtracker]->Released();
			threadvariables.erase(a->idtracker);
		}
	}
}


void AtanorGlobal::Setarguments(string args) {
	arguments.clear();
	if (args.size() != 0)
		v_split(args, " ", arguments);
}

Exporting void AtanorGlobal::RecordMathFunctions() {
	mathFunctions[Getid("abs")] = fabs;
	mathFunctions[Getid("acos")] = acos;
	mathFunctions[Getid("acosh")] = acosh;
	mathFunctions[Getid("asin")] = asin;
	mathFunctions[Getid("asinh")] = asinh;
	mathFunctions[Getid("atan")] = atan;
	mathFunctions[Getid("atanh")] = atanh;
	mathFunctions[Getid("cbrt")] = cbrt;
	mathFunctions[Getid("∛")] = cbrt;	
	mathFunctions[Getid("cos")] = cos;
	mathFunctions[Getid("cosh")] = cosh;
	mathFunctions[Getid("erf")] = erf;
	mathFunctions[Getid("erfc")] = erfc;
	mathFunctions[Getid("exp")] = exp;
	mathFunctions[Getid("exp2")] = exp2;
	mathFunctions[Getid("expm1")] = expm1;
	mathFunctions[Getid("floor")] = floor;
	mathFunctions[Getid("lgamma")] = lgamma;
	mathFunctions[Getid("ln")] = log;
	mathFunctions[Getid("log")] = log10;
	mathFunctions[Getid("log1p")] = log1p;
	mathFunctions[Getid("log2")] = log2;
	mathFunctions[Getid("logb")] = logb;
	mathFunctions[Getid("nearbyint")] = nearbyint;
	mathFunctions[Getid("rint")] = rint;
	mathFunctions[Getid("round")] = round;
	mathFunctions[Getid("sin")] = sin;
	mathFunctions[Getid("sinh")] = sinh;
	mathFunctions[Getid("sqrt")] = sqrt;
	mathFunctions[Getid("tan")] = tan;
	mathFunctions[Getid("tanh")] = tanh;
	mathFunctions[Getid("tgamma")] = tgamma;
	mathFunctions[Getid("trunc")] = trunc;
	mathFunctions[Getid("√")] = sqrt;
}

void AtanorGlobal::Cleanerror(short idthread) {
	Atanor* err = threads[idthread].errorraised;
	if (err != NULL && err != aRAISEERROR && err != aEND) {
		RemoveFromTracker(err->idtracker);
		delete err;
	}
	threads[idthread].Cleanerror();
}


Exporting Atanor* AtanorGlobal::Errorobject(short idthread) {
	if (executionbreak)
		return aEND;

	if (threads[idthread].errorraised != NULL)
		return threads[idthread].errorraised;

	return aNULL;
}

string AtanorGlobal::Errorstring(short idthread) {
	if (executionbreak)
		return "Stop";

	if (threads[idthread].errorraised != NULL)
		return threads[idthread].errorraised->String();
	return "";
}

Exporting Atanor* AtanorGlobal::Returnerror(Atanor* err, short idthread) {
	return threads[idthread].Raiserror((AtanorError*)err);
}

Exporting Atanor* AtanorGlobal::Returnerror(string msgerr, short idthread) {
	AtanorError* err = new AtanorError(msgerr);
	return threads[idthread].Raiserror(err);
}

void AtanorGlobal::Pushstack(Atanor* a, short idthread) {
	if (a->isTracked() && !a->isDeclaration())
		threads[idthread].currentinstruction = a;

	threads[idthread].stack.push_back(a);
	if (threads[idthread].Size() >= maxstack)
		Returnerror("Stack overflow", idthread);

	if (debugmode)
		Pushdebug(idthread);
}


Atanor* ProcCreate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
Exporting void AtanorGlobal::RecordCompatibilities() {
	bin_hash<Atanor*>::iterator it;

	vector<short> numbers;
	vector<short> strings;
	vector<short> maps;
	vector<short> vectors;

	equto[a_plusequ] = a_plus;
	equto[a_minusequ] = a_minus;
	equto[a_multiplyequ] = a_multiply;
	equto[a_divideequ] = a_divide;
	equto[a_powerequ] = a_power;
	equto[a_shiftleftequ] = a_shiftleft;
	equto[a_shiftrightequ] = a_shiftright;
	equto[a_modequ] = a_mod;
	equto[a_mergeequ] = a_merge;
	equto[a_addequ] = a_add;
	equto[a_orequ] = a_or;
	equto[a_xorequ] = a_xor;
	equto[a_andequ] = a_and;
	equto[a_plus] = a_plus;
	equto[a_minus] = a_minus;
	equto[a_multiply] = a_multiply;
	equto[a_divide] = a_divide;
	equto[a_power] = a_power;
	equto[a_shiftleft] = a_shiftleft;
	equto[a_shiftright] = a_shiftright;
	equto[a_mod] = a_mod;
	equto[a_merge] = a_merge;
	equto[a_add] = a_add;
	equto[a_or] = a_or;
	equto[a_xor] = a_xor;
	equto[a_and] = a_and;

	compatibilities[a_self][a_const] = true;
	compatibilities[a_let][a_const] = true;

	compatibilities[a_boolean][a_const] = true;
	compatibilities[a_predicate][a_predicatevar] = true;
	compatibilities[a_predicatevar][a_predicate] = true;

	short ty;
	for (it = newInstance.begin(); it != newInstance.end(); it++) {
		if (it->second->isFrame()) //this type is only used to produce frames...
			continue;
		ty = it->second->Type();
		RecordOneProcedure(Getsymbol(ty), ProcCreate, P_NONE | P_ONE);

		compatibilities[ty][ty] = true;

		compatibilities[a_self][ty] = true;
		compatibilities[a_let][ty] = true;

		compatibilities[a_boolean][ty] = true;

		if (it->second->isVectorContainer()) {
			vectors.push_back(ty);
			continue;
		}
		if (it->second->isMapContainer()) {
			maps.push_back(ty);
			continue;
		}

		if (it->second->isNumber()) {
			numbers.push_back(ty);
			continue;
		}

		if (it->second->isString()) {
			strings.push_back(ty);
			continue;
		}
	}

	int i, j;
	for (i = 0; i < numbers.size(); i++) {
		for (j = 0; j < numbers.size(); j++)
			compatibilities[numbers[i]][numbers[j]] = true;
		for (j = 0; j < strings.size(); j++)
			compatibilities[numbers[i]][strings[j]] = true;
		compatibilities[numbers[i]][a_const] = true;
	}

	for (i = 0; i < strings.size(); i++) {
		for (j = 0; j < strings.size(); j++)
			compatibilities[strings[i]][strings[j]] = true;
		for (j = 0; j < numbers.size(); j++)
			compatibilities[strings[i]][numbers[j]] = true;
		compatibilities[strings[i]][a_const] = true;
	}

	for (i = 0; i < vectors.size(); i++) {
		for (j = 0; j < vectors.size(); j++)
			compatibilities[vectors[i]][vectors[j]] = true;
		compatibilities[vectors[i]][a_const] = true;
	}

	for (i = 0; i < maps.size(); i++) {
		for (j = 0; j < maps.size(); j++)
			compatibilities[maps[i]][maps[j]] = true;
		compatibilities[maps[i]][a_const] = true;
	}
}

long AtanorGlobal::Getinstructionline(short idthread) {
	if (threads[idthread].currentinstruction == NULL)
		return lineerror;
	return threads[idthread].currentinstruction->Currentline();
}

//This function is hack which is necessary to get these variables a value in a DLL
Exporting void AtanorGlobal::Update() {
	globalAtanor = this;	
}


Exporting void AtanorGlobal::RecordConstantNames() {
	string_operators["=="] = a_same;
	string_operators["<"] = a_less;
	string_operators[">"] = a_more;
	string_operators["$<"] = a_less; //in Haskell expressions to avoid confusion with opening and closing brackets...
	string_operators["$>"] = a_more;
	string_operators[">="] = a_moreequal;
	string_operators["<="] = a_lessequal;
	string_operators["="] = a_affectation;
	string_operators["is"] = a_affectation;
	string_operators[":="] = a_forcedaffectation;
	string_operators["isnot"] = a_different;
	string_operators["<>"] = a_different;
	string_operators["~="] = a_different;
	string_operators["!="] = a_different;
	string_operators["+"] = a_plus;
	string_operators["-"] = a_minus;
	string_operators["*"] = a_multiply;
	string_operators["×"] = a_multiply;
	string_operators["/"] = a_divide;
	string_operators["÷"] = a_divide;
	string_operators["%"] = a_mod;
	string_operators["^^"] = a_power;
	string_operators["**"] = a_power;
	string_operators["|"] = a_or;
	string_operators["^"] = a_xor;
	string_operators["&"] = a_and;
	string_operators["<<"] = a_shiftleft;
	string_operators["<<<"] = a_stream;
	string_operators[">>"] = a_shiftright;
	string_operators["+="] = a_plusequ;
	string_operators["++"] = a_plusplus;
	string_operators["-="] = a_minusequ;
	string_operators["--"] = a_minusminus;
	string_operators["&&&"] = a_merge;
	string_operators["&&&="] = a_mergeequ;
	string_operators["::"] = a_add;
	string_operators["::="] = a_addequ;
	string_operators["×="] = a_multiplyequ;
	string_operators["*="] = a_multiplyequ;
	string_operators["/="] = a_divideequ;
	string_operators["÷="] = a_divideequ; 
	string_operators["%="] = a_modequ;
	string_operators["^^="] = a_powerequ;
	string_operators["**="] = a_powerequ;
	string_operators["<<="] = a_shiftleftequ;
	string_operators[">>="] = a_shiftrightequ;
	string_operators["|="] = a_orequ;
	string_operators["^="] = a_xorequ;
	string_operators["&="] = a_andequ;
	string_operators["||"] = a_disjunction;
	string_operators["&&"] = a_conjunction;
	string_operators["or"] = a_disjunction;
	string_operators["and"] = a_conjunction;
	string_operators["in"] = a_in;
	string_operators["notin"] = a_notin;
	string_operators["²"] = a_square;
	string_operators["³"] = a_cube;

	operator_strings[a_same] = "==";
	operator_strings[a_less] = "<";
	operator_strings[a_more] = ">";
	operator_strings[a_moreequal] = ">=";
	operator_strings[a_lessequal] = "<=";
	operator_strings[a_affectation] = "=";
	operator_strings[a_forcedaffectation] = ":=";
	operator_strings[a_different] = "<>";
	operator_strings[a_different] = "~=";
	operator_strings[a_different] = "!=";
	operator_strings[a_plus] = "+";
	operator_strings[a_minus] = "-";
	operator_strings[a_multiply] = "*";
	operator_strings[a_divide] = "/";
	operator_strings[a_mod] = "%";
	operator_strings[a_power] = "^^";
	operator_strings[a_or] = "|";
	operator_strings[a_xor] = "^";
	operator_strings[a_and] = "&";
	operator_strings[a_stream] = "<<<";
	operator_strings[a_shiftleft] = "<<";
	operator_strings[a_shiftright] = ">>";
	operator_strings[a_plusequ] = "+=";
	operator_strings[a_plusplus] = "++";
	operator_strings[a_minusequ] = "-=";
	operator_strings[a_minusminus] = "--";
	operator_strings[a_multiplyequ] = "*=";
	operator_strings[a_divideequ] = "/=";
	operator_strings[a_modequ] = "%=";
	operator_strings[a_powerequ] = "^^=";
	operator_strings[a_shiftleftequ] = "<<=";
	operator_strings[a_shiftrightequ] = ">>=";
	operator_strings[a_orequ] = "|=";
	operator_strings[a_xorequ] = "^=";
	operator_strings[a_andequ] = "&=";
	operator_strings[a_disjunction] = "||";
	operator_strings[a_conjunction] = "&&";
	operator_strings[a_in] = "in";
	operator_strings[a_notin] = "notin";
	operator_strings[a_square] = "²";
	operator_strings[a_cube] = "³";

	atanOperatorMath[a_plus] = true;
	atanOperatorMath[a_minus] = true;
	atanOperatorMath[a_multiply] = true;
	atanOperatorMath[a_divide] = true;
	atanOperatorMath[a_mod] = true;
	atanOperatorMath[a_power] = true;
	atanOperatorMath[a_or] = true;
	atanOperatorMath[a_xor] = true;
	atanOperatorMath[a_and] = true;
	atanOperatorMath[a_shiftleft] = true;
	atanOperatorMath[a_shiftright] = true;

	//Constants and ids
	gITERNULL = new AtanorConstiteration(false, this); 
	gPIPE = new AtanorConstPipe(a_pipe, ":", this);
	
	gTRUE = new AtanorConstBool(true, this);
	gFALSE = new AtanorConstBool(false, this);

	gMINUSTEN = new AtanorConstShort(-10, this);
	gMINUSNINE = new AtanorConstShort(-9, this);
	gMINUSEIGHT = new AtanorConstShort(-8, this);
	gMINUSSEVEN = new AtanorConstShort(-7, this);
	gMINUSSIX = new AtanorConstShort(-6, this);
	gMINUSFIVE = new AtanorConstShort(-5, this);
	gMINUSFOUR = new AtanorConstShort(-4, this);
	gMINUSTHREE = new AtanorConstShort(-3, this);
	gMINUSTWO = new AtanorConstShort(-2, this);
	gMINUSONE = new AtanorConstShort(-1, this);
	gZERO = new AtanorConstShort(0, this);
	gONE = new AtanorConstShort(1, this);
	gTWO = new AtanorConstShort(2, this);
	gTHREE = new AtanorConstShort(3, this);
	gFOUR = new AtanorConstShort(4, this);
	gFIVE = new AtanorConstShort(5, this);
	gSIX = new AtanorConstShort(6, this);
	gSEVEN = new AtanorConstShort(7, this);
	gEIGHT = new AtanorConstShort(8, this);
	gNINE = new AtanorConstShort(9, this);
	gTEN = new AtanorConstShort(10, this);

	gEMPTYSTRING = new AtanorConstString("", this);
	
	gNULL = new AtanorConst(Createid("null"), "null", this); //0
	gITERNULL->Update(gNULL); //Horrible hack to have aITERNULL being deleted before aNULL, but benefiting from aNULL still

	Createid("true"); //1
	Createid("false"); //2
	Createid("one"); //3
	Createid("zero"); //4
	Createid("minusone"); //5
	
	gBREAK = new AtanorConstBreak(Createid("break"), "break", this); //6
	gCONTINUE = new AtanorConstContinue(Createid("continue"), "continue", this); //7
	gRETURN = new AtanorConst(Createid("return"), "return", this); //8
	Createid(":"); //9
	gNOELEMENT = new AtanorConst(Createid("empty"), "empty", this); //10
	

	Createid("mainframe"); //11
	Createid("call"); //12
	Createid("callfunction");//13
	Createid("callmethod"); //14
	Createid("callprocedure"); //15
	Createid("callindex"); //16
	Createid("variable"); //17
	Createid("declarations"); //18
	Createid("instructions"); //19
	Createid("function"); //20
	Createid("frame"); //21
	Createid("frameinstance"); //22
	Createid("extension"); //23

	Createid("_initial"); //24

	Createid("iterator"); //25

	gDEFAULT = new AtanorConst(Createid("&default"), "&default", this); //26

	Createid("forinrange"); //27
	Createid("sequence"); //28
	Createid("self"); //29

	Createid("vector");//30
	Createid("bvector");//31
	Createid("fvector");//32
	Createid("ivector");//33
	Createid("hvector");//34
	Createid("svector");//35
	Createid("uvector");//36
	Createid("dvector");//37
	Createid("lvector");//38

	Createid("short"); //39
	Createid("int"); //40
	Createid("long"); //41
	Createid("decimal");//42
	Createid("fraction");//43
	Createid("float");//44
	Createid("string");//45
	Createid("ustring");//46
	Createid("bool");//47

	Createid("map");//48
	Createid("treemap"); //49
	Createid("primemap"); //50
	Createid("byte"); //51
	Createid("&error");//52

	gEND = new AtanorError(this, "END");
	gRAISEERROR = new AtanorError(this, "ERROR");
	Createid("list"); //53

	Createid("constvector"); //54
	Createid("const"); //55
	Createid("constmap"); //56
	Createid("callhaskell");//57
	Createid("lambda");//58
	Createid("&return;");//59
	Createid("&breaktrue"); //60
	Createid("&breakfalse"); //61


	gBREAKTRUE = new AtanorCallBreak(gTRUE, this);
	gBREAKFALSE = new AtanorCallBreak(gFALSE, this);

	Createid("constvectormerge"); //62
	Createid("instructionequ"); //63
	//let is used in Haskell expression, it corresponds to a self variable... 	 
	Createid("let"); //64
	Createid("callthread");//65
	gAFFECTATION = new AtanorConst(Createid("&assign"), "&assign", this); //66

	Createid("atanor");//67

	Createid("sloop"); //68
	Createid("uloop");//69
	Createid("iloop");//70
	Createid("floop");//71
	Createid("bloop");//72
	Createid("dloop");//73
	Createid("lloop");//74

	Createid("this");//75
	Createid("[]");//76
	Createid("[:]");//77
	Createid("type");//78
	Createid("_final");//79

	Createid("&inifinitive"); //80
	Createid("&cycle"); //81
	Createid("&replicate"); //82

	Createid("fail");//83
	Createid("!"); //84
	Createid("stop"); //85

	Createid("&PREDICATEENTREE"); //86
	gUNIVERSAL = new AtanorConst(Createid("_"), "_", this); //87

	Createid("asserta"); //88
	Createid("assertz"); //89
	Createid("retract"); //90
	Createid("&remove&"); //91

	Createid("predicatevar"); //92
	Createid("predicate"); //93
	Createid("term"); //94
	Createid("p_instance"); //95
	Createid("p_predicateruleelement"); //96
	Createid("p_predicatecontainer"); //97
	Createid("p_predicaterule"); //98
	Createid("p_predicateinstruction"); //99
	Createid("p_knowledgebase"); //100
	Createid("p_dependencybase"); //101
	Createid("p_predicatedomain"); //102
	Createid("p_launch"); //103
	Createid("p_predicateelement"); //104
	Createid("p_parameterpredicate"); //105
	Createid("p_predicateevaluate"); //106
	Createid("dependency"); //107

	Createid("atan_none"); //108
	Createid("atan_stream"); //109
	Createid("atan_affectation"); //110
	Createid("atan_plusequ"); //111
	Createid("atan_minusequ"); //112
	Createid("atan_multiplyequ"); //113
	Createid("atan_divideequ"); //114
	Createid("atan_modequ"); //115
	Createid("atan_powerequ"); //116
	Createid("atan_shiftleftequ"); //117
	Createid("atan_shiftrightequ"); //118
	Createid("atan_orequ"); //119
	Createid("atan_xorequ"); //120
	Createid("atan_andequ"); //121

	Createid("atan_mergeequ"); //122
	Createid("atan_addequ"); //123

	//Operators 	 	 		 		
	Createid("+"); //124
	Createid("-"); //125
	Createid("*"); //126
	Createid("/"); //127
	Createid("^^"); //128
	Createid("<<"); //129
	Createid(">>"); //130
	Createid("%"); //131
	Createid("|"); //132
	Createid("^"); //133
	Createid("&"); //134

	Createid("&&&"); //135
	Createid("::"); //136

	Createid("<"); //137
	Createid(">"); //138
	Createid("=="); //139
	Createid("!="); //140
	Createid("<="); //141
	Createid(">="); //142
	Createid("++"); //143
	Createid("--"); //144

	Createid("in"); //145
	Createid("notin"); //146

	Createid("atan_match"); //147
	Createid("atan_bloc"); //148
	Createid("atan_blocloopin"); //149
	Createid("atan_filein"); //150
	Createid("atan_blocboolean"); //151
	Createid("atan_parameter"); //152
	Createid("atan_if"); //153
	Createid("atan_try"); //154
	Createid("atan_switch"); //155
	Createid("atan_while"); //156
	Createid("atan_for"); //157
	Createid("atan_catchbloc"); //158
	Createid("atan_conjunction"); //159
	Createid("atan_disjunction"); //160

	gHASKELL = new AtanorConst(Createid("&haskell"), "&haskell", this); //161

	Createid("atan_forcedaffectation"); //162

	gNULLDECLARATION = new AtanorDeclaration(a_declarations, this);

	Createid("²"); //163
	Createid("³"); //164
	Createid("&counter;"); //165
	Createid("synode"); //166

	Createid("ithrough"); //167
	Createid("fthrough"); //168
	Createid("sthrough"); //169
	Createid("uthrough"); //170
	Createid("dthrough"); //171
	Createid("vthrough"); //172
	Createid("mthrough"); //173
	Createid("lthrough"); //174
	Createid("mapss"); //175
	//when we want to modify a dependency...
	Createid("&modify_dependency"); //176
	Createid("&action_var"); //177

	dependenciesvariable[a_modifydependency] = a_modifydependency;

	
	throughs[a_intthrough] = a_int;
	throughs[a_floatthrough] = a_float;
	throughs[a_stringthrough] = a_string;
	throughs[a_ustringthrough] = a_ustring;
	throughs[a_decimalthrough] = a_decimal;
	throughs[a_vectorthrough] = a_svector;
	throughs[a_mapthrough] = a_mapss;
	throughs[a_longthrough] = a_long;

	actions[a_plus] = new AtanorAction(a_plus);
	actions[a_minus] = new AtanorAction(a_minus);
	actions[a_multiply] = new AtanorAction(a_multiply);
	actions[a_power] = new AtanorAction(a_power);
	actions[a_shiftleft] = new AtanorAction(a_shiftleft);
	actions[a_shiftright] = new AtanorAction(a_shiftright);
	actions[a_divide] = new AtanorAction(a_divide);
	actions[a_mod] = new AtanorAction(a_mod);
	actions[a_or] = new AtanorAction(a_or);
	actions[a_xor] = new AtanorAction(a_xor);
	actions[a_and] = new AtanorAction(a_and);
	actions[a_merge] = new AtanorAction(a_merge);
	actions[a_add] = new AtanorAction(a_add);

}

//------------------- Threads-------------------------------------
Atanor* ProcThreadhandle(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	threadhandle tid = _GETTHREADID();
	stringstream s;
	s << tid;
	return globalAtanor->Providestring(s.str());
}


bool AtanorGlobal::isFile(Atanor* a) {
	if (!newInstance.check(a->Typevariable()))
		return false;
	return newInstance[a->Typevariable()]->isFile();
}

bool AtanorGlobal::isString(Atanor* a) {
	if (!newInstance.check(a->Typevariable()))
		return false;
	return newInstance[a->Typevariable()]->isString();
}

bool AtanorGlobal::isNumber(Atanor* a) {
	if (!newInstance.check(a->Typevariable()))
		return false;
	return newInstance[a->Typevariable()]->isNumber();
}

bool AtanorGlobal::isContainer(Atanor* a) {
	if (!newInstance.check(a->Typevariable()))
		return false;
	return newInstance[a->Typevariable()]->isContainer();
}

bool AtanorGlobal::isVectorContainer(Atanor* a) {
	if (!newInstance.check(a->Typevariable()))
		return false;
	return newInstance[a->Typevariable()]->isVectorContainer();
}

bool AtanorGlobal::isMapContainer(Atanor* a) {
	if (!newInstance.check(a->Typevariable()))
		return false;
	return newInstance[a->Typevariable()]->isMapContainer();
}

short AtanorGlobal::Type(Atanor* a) {
	if (!newInstance.check(a->Typevariable()))
		return false;
	return newInstance[a->Typevariable()]->Type();
}

//------------------- Threads-------------------------------------

bool AtanorGlobal::TestEndthreads() {
	Locking _lock(_call);
	for (size_t i = 1; i < maxthreads; i++) {
		if (threads[i].used)
			return false;
	}
	return true;
}

Exporting short AtanorGlobal::GetThreadid() {
	if (!globalAtanor->globalLOCK)
		return 0;

	Locking _lock(_call);
	threadhandle tid = _GETTHREADID();
	return threadids[tid];
}

short AtanorGlobal::InitThreadid(short id) {
	Locking _lock(_call);
	threadids[threads[id].Initialization()] = id;
	return id;
}

Atanor* AtanorGlobal::Getmainframe(size_t idcode) {
	Locking _lock(_call);
	return spaces[idcode]->Mainframe();
}

short AtanorGlobal::GetaThreadId() {
	Locking _lock(_call);
	for (short i = 1; i < maxthreads; i++) {
		if (!threads[i].used) {
			threads[i].used = true;
			return i;
		}
	}
	return -1;
}

short AtanorGlobal::Joinpos(short id) {
	return threads[id].joined.size();
}

void AtanorGlobal::EraseThreadid(short id) {
	Locking _lock(_call);
	threadids.erase(threads[id].handle);
	threads[id].Clear();
}

//------------------- AtanorCode management-------------------------------------

AtanorCode* AtanorGlobal::GetNewCodeSpace(string filename) {
	filename = NormalizeFileName(filename);

	if (codes.find(filename) != codes.end())
		return codes[filename];

	AtanorCode* a = new AtanorCode((short)spaces.size(), filename, this);
	spaces.push_back(a);
	codes[filename] = a;
	return a;
}

//----------------------------------------------------------------------------------
Exporting void AtanorGlobal::RecordInTracker(Atanor* a) {
	trackercursor = tracked.size();
	a->Setid(trackercursor);
	tracked.push_back(a);
	trackercursor++;
}

Exporting long AtanorGlobal::RecordInTrackerProtected(Atanor* a) {
	Locking _lock(this);
	if (a->idtracker != -1)
		return a->idtracker;

	if (trackercursor == tracked.size()) {
		trackercursor++;
		a->Setid(tracked.size());
		tracked.push_back(a);
		return a->idtracker;
	}

	a->Setid(trackercursor);
	tracked[trackercursor] = a;

	while (trackercursor < tracked.size()) {
		if (tracked[trackercursor] == NULL)
			break;
		trackercursor++;
	}

	return a->idtracker;
}

Exporting void AtanorGlobal::RemoveFromTracker(long id) {
	if (id == -1)
		return;
	Locking _lock(this);
	tracked[id] = NULL;
	if (id < trackercursor)
		trackercursor = id;
}

Exporting void AtanorGlobal::ResetWithTracker(Atanor* a, long id, long inc) {
	Locking _lock(this);
	if (tracked[id] == NULL || tracked[id] != a)
		return;

	a->Resetreference(inc);
}

Exporting bool AtanorGlobal::Checktracker(Atanor* a, long id) {
	Locking _lock(this);
	if (id >= 0 && id < tracked.size() && tracked[id] == a)
		return true;
	return false;
}

Exporting Exporting Atanor* AtanorGlobal::GetFromTracker(long id) {
	Locking _lock(this);
	return tracked[id];
}

//----------------------------------------------------------------------------------
bool AtanorGlobal::Loadcontent(string content) {
	//We directly parse the content...
	x_readstring xr(content);
	xr.loadtoken();
	AtanorCode* a = GetNewCodeSpace("SCRIPT");
	return a->Load(xr);
}

AtanorCode* AtanorGlobal::Loadfile(string filename) {
	filename = NormalizeFileName(filename);


	x_readfile xr(filename);
	if (xr.pos == -1)
		return NULL;

	AtanorCode* a = GetNewCodeSpace(filename);

	xr.loadtoken();
	a->Load(xr);
	return a;
}

void AtanorGlobal::AtanorAllObjects(vector<string>& vs) {
	vs.clear();

	bin_hash<Atanor*>::iterator it;
	string n;
	for (it = newInstance.begin(); it != newInstance.end(); it++) {
		n = Getsymbol(it->first);
		vs.push_back(n);
	}

	bin_hash<AtanorProcedure>::iterator itp;
	for (itp = procedures.begin(); itp != procedures.end(); itp++) {
		n = Getsymbol(itp->first);
		vs.push_back(n);
	}

	bin_hash<unsigned long>::iterator itl;
	for (itl = allmethods.begin(); itl != allmethods.end(); itl++) {
		n = Getsymbol(itl->first);
		vs.push_back(n);
	}

	for (itl = framemethods.begin(); itl != framemethods.end(); itl++) {
		n = Getsymbol(itl->first);
		vs.push_back(n);
	}

	for (itp = commons.begin(); itp != commons.end(); itp++) {
		n = Getsymbol(itp->first);
		vs.push_back(n);
	}


	vs.push_back("absent");
	vs.push_back("and");
	vs.push_back("assert");
	vs.push_back("asserta");
	vs.push_back("assertz");
	vs.push_back("autorun");
	vs.push_back("break");
	vs.push_back("case");
	vs.push_back("catch");
	vs.push_back("common");
	vs.push_back("const");
	vs.push_back("continue");
	vs.push_back("cycle");
	vs.push_back("default");
	vs.push_back("do");
	vs.push_back("drop");
	vs.push_back("dropWhile");
	vs.push_back("elif");
	vs.push_back("else");
	vs.push_back("empty");
	vs.push_back("exclusive");
	vs.push_back("exit");
	vs.push_back("extension");
	vs.push_back("fail");
	vs.push_back("false");
	vs.push_back("filter");
	vs.push_back("flip");
	vs.push_back("foldl");
	vs.push_back("foldr");
	vs.push_back("for");
	vs.push_back("frame");
	vs.push_back("from");
	vs.push_back("function");
	vs.push_back("get");
	vs.push_back("if");
	vs.push_back("If");
	vs.push_back("IF");
	vs.push_back("in");
	vs.push_back("is");
	vs.push_back("iterator");
	vs.push_back("joined");
	vs.push_back("let");
	vs.push_back("map");
	vs.push_back("not");
	vs.push_back("notin");
	vs.push_back("null");
	vs.push_back("of");
	vs.push_back("or");
	vs.push_back("otherwise");
	vs.push_back("polynomial");
	vs.push_back("ponder");
	vs.push_back("private");
	vs.push_back("protected");
	vs.push_back("remove");
	vs.push_back("repeat");
	vs.push_back("replicate");
	vs.push_back("retract");
	vs.push_back("return");
	vs.push_back("scanl");
	vs.push_back("scanr");
	vs.push_back("stop");
	vs.push_back("store");
	vs.push_back("strict");
	vs.push_back("switch");
	vs.push_back("table");
	vs.push_back("take");
	vs.push_back("takeWhile");
	vs.push_back("this");
	vs.push_back("thread");
	vs.push_back("true");
	vs.push_back("try");
	vs.push_back("where");
	vs.push_back("while");
	vs.push_back("with");
	vs.push_back("zip");
	vs.push_back("zipWith");

}

//------------------------------------------------------------------------

bool AtanorCode::Load(x_reading& xr) {
	short currentspaceid = globalAtanor->spaceid;
	bnf_atanor* previous = globalAtanor->currentbnf;
	Atanoratanor* atan = new Atanoratanor(filename, this, globalAtanor);

	globalAtanor->spaceid = idcode;

	bnf_atanor bnf;
	bnf.baseline = globalAtanor->linereference;
	global->lineerror = -1;

	x_node* xn = bnf.x_parsing(&xr, FULL);
	if (xn == NULL) {
		cerr << " in " << filename << endl;
		stringstream message;
		global->lineerror = bnf.lineerror;
		currentline = global->lineerror;
		message << "Error while parsing program file: ";
		if (xr.error()) {
			message << "Unknown file: ";
			message << xr.name;
		}
		else
		if (bnf.errornumber != -1)
			message << bnf.x_errormsg(bnf.errornumber);
		else
			message << bnf.labelerror;
		throw new AtanorRaiseError(message, filename, global->lineerror, bnf.lineerror);
	}


	globalAtanor->currentbnf = &bnf;

	globalAtanor->Pushstack(&mainframe);
	Traverse(xn, &mainframe);
	globalAtanor->Popstack();

	globalAtanor->currentbnf = previous;
	globalAtanor->spaceid = currentspaceid;

	delete xn;
	return true;
}

//------------------------------------------------------------------------

bool AtanorCode::Compile(string& body) {
	//we store our AtanorCode also as an Atanoratanor...
	filename = NormalizeFileName(filename);
	Atanoratanor* atan = new Atanoratanor(filename, this, globalAtanor);

	globalAtanor->spaceid = idcode;

	bnf_atanor bnf;
	bnf.baseline = globalAtanor->linereference;

	x_readstring xr(body);
	xr.loadtoken();
	global->lineerror = -1;

	x_node* xn = bnf.x_parsing(&xr, FULL);
	if (xn == NULL) {
		cerr << " in " << filename << endl;
		stringstream& message = globalAtanor->threads[0].message;
		global->lineerror = bnf.lineerror;
		currentline = global->lineerror;
		message << "Error while parsing program file: ";
		if (xr.error()) {
			message << "Unknown file: ";
			message << xr.name;
		}
		else
		if (bnf.errornumber != -1)
			message << bnf.x_errormsg(bnf.errornumber);
		else
			message << bnf.labelerror;

		globalAtanor->Returnerror(message.str(), globalAtanor->GetThreadid());
		return false;
	}

	globalAtanor->currentbnf = &bnf;
	firstinstruction = mainframe.instructions.size();

	globalAtanor->Pushstack(&mainframe);
	try {
		Traverse(xn, &mainframe);
	}
	catch (AtanorRaiseError* a) {
		globalAtanor->threads[0].currentinstruction = NULL;
		globalAtanor->lineerror = a->left;
		globalAtanor->threads[0].message.str("");
		globalAtanor->threads[0].message << a->message;
		if (a->message.find(a->filename) == string::npos)
			globalAtanor->threads[0].message << " in " << a->filename;
		
		if (globalAtanor->threads[0].errorraised == NULL)
			globalAtanor->threads[0].errorraised = new AtanorError(globalAtanor->threads[0].message.str());
		else
			globalAtanor->threads[0].errorraised->error = globalAtanor->threads[0].message.str();

		globalAtanor->threads[0].error = true;
		AtanorCode* c = globalAtanor->Getcurrentcode();
		if (c->filename != a->filename)
			c->filename = a->filename;
		delete a;
		delete xn;
		globalAtanor->Popstack();
		return false;
	}

	globalAtanor->Popstack();

	delete xn;
	return true;
}


Atanor* AtanorCode::Compilefunction(string& body) {
	//we store our AtanorCode also as an Atanoratanor...

	bnf_atanor bnf;
	bnf.baseline = globalAtanor->linereference;

	x_readstring xr(body);
	xr.loadtoken();
	global->lineerror = -1;

	x_node* xn = bnf.x_parsing(&xr, FULL);
	if (xn == NULL) {
		cerr << " in " << filename << endl;
		stringstream& message = globalAtanor->threads[0].message;
		global->lineerror = bnf.lineerror;
		currentline = global->lineerror;
		message << "Error while parsing program file: ";
		if (xr.error()) {
			message << "Unknown file: ";
			message << xr.name;
		}
		else
		if (bnf.errornumber != -1)
			message << bnf.x_errormsg(bnf.errornumber);
		else
			message << bnf.labelerror;

		globalAtanor->Returnerror(message.str(), globalAtanor->GetThreadid());
		return aFALSE;
	}

	globalAtanor->currentbnf = &bnf;
	firstinstruction = mainframe.instructions.size();

	globalAtanor->Pushstack(&mainframe);
	Atanor* compiled = NULL;
	try {
		compiled = Traverse(xn, &mainframe);
	}
	catch (AtanorRaiseError* a) {
		globalAtanor->threads[0].message.str("");
		globalAtanor->threads[0].message << a->message;
		if (a->message.find(a->filename) == string::npos)
			globalAtanor->threads[0].message << " in " << a->filename;

		if (globalAtanor->threads[0].errorraised == NULL)
			globalAtanor->threads[0].errorraised = new AtanorError(globalAtanor->threads[0].message.str());
		else
			globalAtanor->threads[0].errorraised->error = globalAtanor->threads[0].message.str();

		globalAtanor->threads[0].error = true;
		AtanorCode* c = globalAtanor->Getcurrentcode();
		if (c->filename != a->filename)
			c->filename = a->filename;
		delete a;
		delete xn;
		globalAtanor->Popstack();
		return NULL;
	}

	globalAtanor->Popstack();

	delete xn;
	return compiled;
}

//----------------------------------------------------------------------------------------
Exporting void Atanor::storevalue(string u) {
	Atanor* a = globalAtanor->Providestring(u);
	Push(a);
	a->Release();
}

Exporting void Atanor::storevalue(wstring u) {
	Atanor* a = globalAtanor->Provideustring(u);
	Push(a);
	a->Release();
}

Exporting void Atanor::storevalue(long u) {
	Atanor* a = globalAtanor->Provideint(u);
	Push(a);
	a->Release();
}

Exporting void Atanor::storevalue(short u) {
	Atanor* a = new Atanorshort(u);
	Push(a);
	a->Release();
}

Exporting void Atanor::storevalue(BLONG u) {
	Atanor* a = new Atanorlong(u);
	Push(a);
	a->Release();
}

Exporting void Atanor::storevalue(float u) {
	Atanor* a = new Atanordecimal(u);
	Push(a);
	a->Release();
}

Exporting void Atanor::storevalue(double u) {
	Atanor* a = globalAtanor->Providefloat(u);
	Push(a);
	a->Release();
}

Exporting void Atanor::storevalue(unsigned char u) {
	Atanor* a = new Atanorbyte(u);
	Push(a);
	a->Release();
}

Exporting void Atanor::storevalue(wchar_t u) {
	wstring w;
	w = u;
	Atanor* a = globalAtanor->Provideustring(w);
	Push(a);
	a->Release();
}

//---------------------------------------------------------------------------------------------
void AtanorLet::storevalue(string u) {
	if (value != aNOELEMENT)
		value->storevalue(u);
	else {
		value = globalAtanor->Providestring(u);
		value->Setreference(reference);
	}
}

void AtanorLet::storevalue(float u) {
	if (value != aNOELEMENT)
		value->storevalue(u);
	else {
		value = new Atanordecimal(u);
		value->Setreference(reference);
	}
}

void AtanorLet::storevalue(short u) {
	if (value != aNOELEMENT)
		value->storevalue(u);
	else {
		value = new Atanorshort(u);
		value->Setreference(reference);
	}
}

void AtanorLet::storevalue(wstring u) {
	if (value != aNOELEMENT)
		value->storevalue(u);
	else {
		value = globalAtanor->Provideustring(u);
		value->Setreference(reference);
	}
}

void AtanorLet::storevalue(long u) {
	if (value != aNOELEMENT)
		value->storevalue(u);
	else {
		value = globalAtanor->Provideint(u);
		value->Setreference(reference);
	}
}

void AtanorLet::storevalue(BLONG u) {
	if (value != aNOELEMENT)
		value->storevalue(u);
	else {
		value = new Atanorlong(u);
		value->Setreference(reference);
	}
}

void AtanorLet::storevalue(double u) {
	if (value != aNOELEMENT)
		value->storevalue(u);
	else {
		value = globalAtanor->Providefloat(u);
		value->Setreference(reference);
	}
}

void AtanorLet::storevalue(unsigned char u) {
	if (value != aNOELEMENT)
		value->storevalue(u);
	else {
		value = new Atanorbyte(u);
		value->Setreference(reference);
	}
}

void AtanorLet::storevalue(wchar_t u) {
	if (value != aNOELEMENT)
		value->storevalue(u);
	else {
		wstring w;
		w = u;
		value = globalAtanor->Provideustring(w);
		value->Setreference(reference);
	}
}

//-------------------------------------------------------------------
void AtanorSelf::storevalue(string u) {
	if (value != aNOELEMENT && value->Type() == a_string)
		((Atanorstring*)value)->value = u;
	else {
		value->Resetreference(reference);
		value = globalAtanor->Providestring(u);
		value->Setreference(reference);
	}
}

void AtanorSelf::storevalue(float u) {
	if (value != aNOELEMENT && value->Type() == a_decimal)
		((Atanordecimal*)value)->value = u;
	else {
		value->Resetreference(reference);
		value = new Atanordecimal(u);
		value->Setreference(reference);
	}
}

void AtanorSelf::storevalue(short u) {
	if (value != aNOELEMENT && value->Type() == a_short)
		((Atanorshort*)value)->value = u;
	else {
		value->Resetreference(reference);
		value = new Atanorshort(u);
		value->Setreference(reference);
	}
}

void AtanorSelf::storevalue(wstring u) {
	if (value != aNOELEMENT && value->Type() == a_ustring)
		((Atanorustring*)value)->value = u;
	else {
		value->Resetreference(reference);
		value = globalAtanor->Provideustring(u);
		value->Setreference(reference);
	}
}

void AtanorSelf::storevalue(long u) {
	if (value != aNOELEMENT && value->Type() == a_int)
		((Atanorint*)value)->value = u;
	else {
		value->Resetreference(reference);
		value = globalAtanor->Provideint(u);
		value->Setreference(reference);
	}
}

void AtanorSelf::storevalue(BLONG u) {
	if (value != aNOELEMENT && value->Type() == a_long)
		((Atanorlong*)value)->value = u;
	else {
		value->Resetreference(reference);
		value = new Atanorlong(u);
		value->Setreference(reference);
	}
}

void AtanorSelf::storevalue(double u) {
	if (value != aNOELEMENT && value->Type() == a_float)
		((Atanorfloat*)value)->value = u;
	else {
		value->Resetreference(reference);
		value = globalAtanor->Providefloat(u);
		value->Setreference(reference);
	}
}

void AtanorSelf::storevalue(unsigned char u) {
	if (value != aNOELEMENT && value->Type() == a_byte)
		((Atanorbyte*)value)->value = u;
	else {
		value->Resetreference(reference);
		value = new Atanorbyte(u);
		value->Setreference(reference);
	}
}

void AtanorSelf::storevalue(wchar_t u) {
	if (value != aNOELEMENT && value->Type() == a_ustring)
		((Atanorustring*)value)->value = u;
	else {
		value->Resetreference(reference);
		wstring w;
		w = u;
		value = globalAtanor->Provideustring(w);
		value->Setreference(reference);
	}
}
//---------------------------------------------------------------------------------------------
Exporting Atanor* Atanor::Push(BLONG k, Atanor* v) {
	Atanor* a = new Atanorlong(k);
	v = Push(a, v);
	a->Release();
	return v;
}

Exporting Atanor* Atanor::Push(ushort k, Atanor* v) {
	Atanor* a = new Atanorshort(k);
	v = Push(a, v);
	a->Release();
	return v;
}

Exporting Atanor* Atanor::Push(long k, Atanor* v) {
	Atanor* a = globalAtanor->Provideint(k);
	v = Push(a, v);
	a->Release();
	return v;
}

Exporting Atanor* Atanor::Push(double k, Atanor* v) {
	Atanor* a = globalAtanor->Providefloat(k);
	v = Push(a, v);
	a->Release();
	return v;
}

Exporting Atanor* Atanor::Push(string k, Atanor* v) {
	Atanor* a = globalAtanor->Providestring(k);
	v = Push(a, v);
	a->Release();
	return v;
}

Exporting Atanor* Atanor::Push(wstring k, Atanor* v) {
	Atanor* a = globalAtanor->Provideustring(k);
	v = Push(a, v);
	a->Release();
	return v;
}

Exporting Atanor* Atanor::Pushinmap(Atanor* a, short idhtread) {
	if (a->isMapContainer()) {
		std::unique_ptr<AtanorIteration> itr(a->Newiteration(false));
		itr->Begin();
		while (!itr->End()->Boolean()) {
			Push(itr->IteratorKey(), itr->IteratorValue());
			itr->Next();
		}
		return aTRUE;
	}
	return Push(Size(), a);
}

Exporting AtanorIteration* Atanor::Newiteration(bool direction) {
	return aITERNULL;
}

Exporting Atanorfraction* Atanor::Fraction() {
	return new Atanorfraction(Float());
}

Exporting Atanor* Atanor::Invert(bool autoself) {
	Atanor* f = new Atanorfraction(Float());
	Atanor* res = f->Invert(true);
	if (res->isError()) {
		f->Release();
		return res;
	}
	return res;
}

//--------------------------------------------------------------

Exporting Atanor* AtanorConstString::CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanorstring v(value);
	return v.CallMethod(idname, contextualpattern, idthread, callfunc);
}

Exporting Atanor* AtanorConstUString::CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanorustring v(value);
	return v.CallMethod(idname, contextualpattern, idthread, callfunc);
}

Exporting Atanor* AtanorConstInt::CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc)  {
	Atanorint v(value);
	return v.CallMethod(idname, contextualpattern, idthread, callfunc);
}

Exporting Atanor* AtanorConstShort::CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc)  {
	Atanorshort v(value);
	return v.CallMethod(idname, contextualpattern, idthread, callfunc);
}

Exporting Atanor* AtanorConstDecimal::CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc)  {
	Atanordecimal v(value);
	return v.CallMethod(idname, contextualpattern, idthread, callfunc);
}

Exporting Atanor* AtanorConstFloat::CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc)  {
	Atanorfloat v(value);
	return v.CallMethod(idname, contextualpattern, idthread, callfunc);
}

Exporting Atanor* AtanorConstLong::CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanorlong v(value);
	return v.CallMethod(idname, contextualpattern, idthread, callfunc);
}

//------------------------------------------------------------------------------------------------------------------------
Exporting void AtanorObject::Setreference(short r) {
	Locking _lock(this);
	reference += r;
	protect = false;

}

Exporting void AtanorObject::Resetreference(short r) {
	bool deleted = false;
	{
		Locking _lock(this);
		reference -= r;
		if (reference <= 0) {
			if (!protect) {
				if (idtracker != -1)
					globalAtanor->RemoveFromTracker(idtracker);
				deleted = true;
			}
		}
	}
	if (deleted)
		delete this;
}

//---------------------------------------------------------------
Exporting Atanormap* AtanorGlobal::Providemap() {
	Locking _lock(maplock);
	if (mapempties.last > 0) {
		Atanormapbuff* ke = mapreservoire[mapempties.vecteur[--mapempties.last]];
		ke->used = true;
		return ke;
	}

	long mx = mapreservoire.size();

	while (mapidx < mx) {
		if (!mapreservoire[mapidx]->used) {
			mapreservoire[mapidx]->used = true;
			return mapreservoire[mapidx++];
		}
		mapidx++;
	}

	long sz = mx >> 2;
	mapreservoire.resize(mx + sz);
	mapidx = mx + sz;
	for (long i = mx; i < mapidx; i++)
		mapreservoire[i] = new Atanormapbuff(i);

	mapidx = mx;
	mapreservoire[mapidx]->used = true;
	return mapreservoire[mapidx++];
}


Exporting void AtanorIndexbuff::Rollback() {
	left->Release();
	if (right != NULL)
		right->Release();

	Locking _lock(globalAtanor->indexlock);
	used = false;
	globalAtanor->indexempties.push_back(idx);
}

Exporting AtanorIndex* AtanorGlobal::Provideindex(AtanorIndex* init, short idthread) {
	Locking _lock(indexlock);
	if (indexempties.last > 0) {
		AtanorIndexbuff* ke = indexreservoire[indexempties.vecteur[--indexempties.last]];
		ke->used = true;
		ke->Set(init, idthread);
		return ke;
	}

	long mx = indexreservoire.size();

	while (indexidx < mx) {
		if (!indexreservoire[indexidx]->used) {
			indexreservoire[indexidx]->used = true;
			indexreservoire[indexidx]->Set(init, idthread);
			return indexreservoire[indexidx++];
		}
		indexidx++;
	}

	long sz = mx >> 2;
	indexreservoire.resize(mx + sz);
	indexidx = mx + sz;
	for (long i = mx; i < indexidx; i++)
		indexreservoire[i] = new AtanorIndexbuff(i);

	indexidx = mx;
	indexreservoire[indexidx]->used = true;
	indexreservoire[indexidx]->Set(init, idthread);
	return indexreservoire[indexidx++];
}

Exporting Atanormapss* AtanorGlobal::Providemapss() {
	Locking _lock(mapsslock);
	if (mapssempties.last > 0) {
		Atanormapssbuff* ke = mapssreservoire[mapssempties.vecteur[--mapssempties.last]];
		ke->used = true;
		return ke;
	}

	long mx = mapssreservoire.size();

	while (mapssidx < mx) {
		if (!mapssreservoire[mapssidx]->used) {
			mapssreservoire[mapssidx]->used = true;
			return mapssreservoire[mapssidx++];
		}
		mapssidx++;
	}

	long sz = mx >> 2;
	mapssreservoire.resize(mx + sz);
	mapssidx = mx + sz;
	for (long i = mx; i < mapssidx; i++)
		mapssreservoire[i] = new Atanormapssbuff(i);

	mapssidx = mx;
	mapssreservoire[mapssidx]->used = true;
	return mapssreservoire[mapssidx++];
}

Exporting Atanorvector* AtanorGlobal::Providevector() {
	Locking _lock(vectorlock);
	if (vectorempties.last > 0) {
		Atanorvectorbuff* ke = vectorreservoire[vectorempties.vecteur[--vectorempties.last]];
		ke->used = true;
		return ke;
	}

	long mx = vectorreservoire.size();

	while (vectoridx < mx) {
		if (!vectorreservoire[vectoridx]->used) {
			vectorreservoire[vectoridx]->used = true;
			return vectorreservoire[vectoridx++];
		}
		vectoridx++;
	}

	long sz = mx >> 2;
	vectorreservoire.resize(mx + sz);
	vectoridx = mx + sz;
	for (long i = mx; i < vectoridx; i++)
		vectorreservoire[i] = new Atanorvectorbuff(i);

	vectoridx = mx;
	vectorreservoire[vectoridx]->used = true;
	return vectorreservoire[vectoridx++];
}


Exporting Atanorivector* AtanorGlobal::Provideivector() {
	Locking _lock(ivectorlock);
	if (ivectorempties.last > 0) {
		Atanorivectorbuff* ke = ivectorreservoire[ivectorempties.vecteur[--ivectorempties.last]];
		ke->used = true;
		return ke;
	}

	long mx = ivectorreservoire.size();

	while (ivectoridx < mx) {
		if (!ivectorreservoire[ivectoridx]->used) {
			ivectorreservoire[ivectoridx]->used = true;
			return ivectorreservoire[ivectoridx++];
		}
		ivectoridx++;
	}

	long sz = mx >> 2;
	ivectorreservoire.resize(mx + sz);
	ivectoridx = mx + sz;
	for (long i = mx; i < ivectoridx; i++)
		ivectorreservoire[i] = new Atanorivectorbuff(i);

	ivectoridx = mx;
	ivectorreservoire[ivectoridx]->used = true;
	return ivectorreservoire[ivectoridx++];
}

Exporting Atanorfvector* AtanorGlobal::Providefvector() {
	Locking _lock(fvectorlock);
	if (fvectorempties.last > 0) {
		Atanorfvectorbuff* ke = fvectorreservoire[fvectorempties.vecteur[--fvectorempties.last]];
		ke->used = true;
		return ke;
	}

	long mx = fvectorreservoire.size();

	while (fvectoridx < mx) {
		if (!fvectorreservoire[fvectoridx]->used) {
			fvectorreservoire[fvectoridx]->used = true;
			return fvectorreservoire[fvectoridx++];
		}
		fvectoridx++;
	}

	long sz = mx >> 2;
	fvectorreservoire.resize(mx + sz);
	fvectoridx = mx + sz;
	for (long i = mx; i < fvectoridx; i++)
		fvectorreservoire[i] = new Atanorfvectorbuff(i);

	fvectoridx = mx;
	fvectorreservoire[fvectoridx]->used = true;
	return fvectorreservoire[fvectoridx++];
}

Exporting Atanorsvector* AtanorGlobal::Providesvector() {
	Locking _lock(svectorlock);
	if (svectorempties.last > 0) {
		Atanorsvectorbuff* ke = svectorreservoire[svectorempties.vecteur[--svectorempties.last]];
		ke->used = true;
		return ke;
	}

	long mx = svectorreservoire.size();

	while (svectoridx < mx) {
		if (!svectorreservoire[svectoridx]->used) {
			svectorreservoire[svectoridx]->used = true;
			return svectorreservoire[svectoridx++];
		}
		svectoridx++;
	}

	long sz = mx >> 2;
	svectorreservoire.resize(mx + sz);
	svectoridx = mx + sz;
	for (long i = mx; i < svectoridx; i++)
		svectorreservoire[i] = new Atanorsvectorbuff(i);

	svectoridx = mx;
	svectorreservoire[svectoridx]->used = true;
	return svectorreservoire[svectoridx++];
}

Exporting Atanoruvector* AtanorGlobal::Provideuvector() {
	Locking _lock(uvectorlock);
	if (uvectorempties.last > 0) {
		Atanoruvectorbuff* ke = uvectorreservoire[uvectorempties.vecteur[--uvectorempties.last]];
		ke->used = true;
		return ke;
	}

	long mx = uvectorreservoire.size();

	while (uvectoridx < mx) {
		if (!uvectorreservoire[uvectoridx]->used) {
			uvectorreservoire[uvectoridx]->used = true;
			return uvectorreservoire[uvectoridx++];
		}
		uvectoridx++;
	}

	long sz = mx >> 2;
	uvectorreservoire.resize(mx + sz);
	uvectoridx = mx + sz;
	for (long i = mx; i < uvectoridx; i++)
		uvectorreservoire[i] = new Atanoruvectorbuff(i);

	uvectoridx = mx;
	uvectorreservoire[uvectoridx]->used = true;
	return uvectorreservoire[uvectoridx++];
}

Exporting Atanorint* AtanorGlobal::Provideint(long v) {
	Locking _lock(intlock);
	if (iempties.last > 0) {
		Atanorintbuff* ke = intreservoire[iempties.vecteur[--iempties.last]];
		ke->used = true;
		ke->value = v;
		return ke;
	}

	long mx = intreservoire.size();

	while (intidx < mx) {
		if (!intreservoire[intidx]->used) {
			intreservoire[intidx]->used = true;
			intreservoire[intidx]->value = v;
			return intreservoire[intidx++];
		}
		intidx++;
	}

	long sz = mx >> 2;
	intreservoire.resize(mx + sz);
	intidx = mx + sz;
	for (long i = mx; i < intidx; i++)
		intreservoire[i] = new Atanorintbuff(i);

	intidx = mx;
	intreservoire[intidx]->used = true;
	intreservoire[intidx]->value = v;
	return intreservoire[intidx++];
}

Exporting Atanorfloat* AtanorGlobal::Providefloat(double v) {
	Locking _lock(floatlock);
	if (fempties.last > 0) {
		Atanorfloatbuff* ke = floatreservoire[fempties.vecteur[--fempties.last]];
		ke->used = true;
		ke->value = v;
		return ke;
	}

	long mx = floatreservoire.size();

	while (floatidx < mx) {
		if (!floatreservoire[floatidx]->used) {
			floatreservoire[floatidx]->used = true;
			floatreservoire[floatidx]->value = v;
			return floatreservoire[floatidx++];
		}
		floatidx++;
	}

	long sz = mx >> 2;
	floatreservoire.resize(mx + sz);
	floatidx = mx + sz;
	for (long i = mx; i < floatidx; i++)
		floatreservoire[i] = new Atanorfloatbuff(i);

	floatidx = mx;
	floatreservoire[floatidx]->used = true;
	floatreservoire[floatidx]->value = v;
	return floatreservoire[floatidx++];
}

Exporting Atanorstring* AtanorGlobal::Providestring(string v) {
	Locking _lock(stringlock);
	if (sempties.last > 0) {
		Atanorstringbuff* ke = stringreservoire[sempties.vecteur[--sempties.last]];
		ke->used = true;
		ke->value = v;
		return ke;
	}

	long mx = stringreservoire.size();

	while (stringidx < mx) {
		if (!stringreservoire[stringidx]->used) {
			stringreservoire[stringidx]->used = true;
			stringreservoire[stringidx]->value = v;
			return stringreservoire[stringidx++];
		}
		stringidx++;
	}

	long sz = mx >> 2;
	stringreservoire.resize(mx + sz);
	stringidx = mx + sz;
	for (long i = mx; i < stringidx; i++)
		stringreservoire[i] = new Atanorstringbuff(i);

	stringidx = mx;
	stringreservoire[stringidx]->used = true;
	stringreservoire[stringidx]->value = v;
	return stringreservoire[stringidx++];
}

Exporting Atanorustring* AtanorGlobal::Provideustring(wstring v) {
	Locking _lock(ustringlock);
	if (uempties.last > 0) {
		Atanorustringbuff* ke = ustringreservoire[uempties.vecteur[--uempties.last]];
		ke->used = true;
		ke->value = v;
		return ke;
	}

	long mx = ustringreservoire.size();

	while (ustringidx < mx) {
		if (!ustringreservoire[ustringidx]->used) {
			ustringreservoire[ustringidx]->value = v;
			ustringreservoire[ustringidx]->used = true;
			return ustringreservoire[ustringidx++];
		}
		ustringidx++;
	}

	long sz = mx >> 2;
	ustringreservoire.resize(mx + sz);
	ustringidx = mx + sz;
	for (long i = mx; i < ustringidx; i++)
		ustringreservoire[i] = new Atanorustringbuff(i);

	ustringidx = mx;
	ustringreservoire[ustringidx]->used = true;
	ustringreservoire[ustringidx]->value = v;
	return ustringreservoire[ustringidx++];
}


