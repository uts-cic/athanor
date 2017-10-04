
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
filename   : atanorglobal.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanorglobal_h
#define atanorglobal_h

//--------------------------------------------------------------------
//AtanorGlobal is one unique object in which the threads and other ids are stored...
//--------------------------------------------------------------------
#include <thread>
#include <mutex>
#include <condition_variable>
#include "vecte.h"

#include "x_node.h"

#define threadhandle std::thread::id
#define THREADPTR std::thread*
#define _GETTHREADID() std::this_thread::get_id()

//--------------------------------------------------------------------
class automate;
class AtanorError;
class Atanorstring;
class AtanorPredicateVariable;
class AtanorPredicateFunction;
class AtanorPredicate;
class AtanorPredicateRule;
class AtanorPredicate;
class AtanorPredicateContainer;
class AtanorCallBreak;
class AtanorObject;
class Atanorintbuff;
class Atanorint;
class Atanorfloatbuff;
class Atanorfloat;
class Atanorstringbuff;
class Atanorstring;
class Atanorustringbuff;
class Atanorustring;
class Atanorvector;
class Atanorvectorbuff;
class Atanorivector;
class Atanorivectorbuff;
class Atanorfvector;
class Atanorfvectorbuff;
class Atanorsvector;
class Atanorsvectorbuff;
class Atanoruvector;
class Atanoruvectorbuff;
class Atanormap;
class Atanormapbuff;
class Atanormapss;
class Atanormapssbuff;
class AtanorIndex;
class AtanorIndexbuff;
class bnf_atanor;

//--------------------------------------------------------------------
typedef bool(*AtanorExternalModule)(AtanorGlobal*, string);
//--------------------------------------------------------------------
typedef double(*mathFunc)(double);
//--------------------------------------------------------------------

class ThreadLock {
public:
	static Exchanging long ids;

	long id;
	short idthread;
	std::recursive_mutex* lock;
	bool locked;
	bool recursive;

	Exporting ThreadLock(std::recursive_mutex* l, bool run = true, bool init = true);
	void Initialize();
	Exporting void Locking();
	Exporting void Unlocking();
	Exporting ~ThreadLock();
};

class LockedThread {
public:
	std::condition_variable lock;
	std::mutex mtx;
	char released;

	LockedThread() {
		released = 0; 
	}

	Exporting ~LockedThread();
	Exporting void Blocked();
	Exporting void Released();
};


//The definition of parsing function...
typedef Atanor* (AtanorCode::*ParseElement)(x_node*, Atanor*);

class ThreadStruct {
public:
	VECTE<Atanor*> stack;
	vector<Atanor*> debugstack;

	VECTE<THREADPTR> joined;

	hmap<unsigned short, vector<AtanorPredicate*> > knowledgebase;
	basebin_hash<VECTE<Atanor*> > variables;

	string nonblockingerror;

	stringstream message;
	threadhandle handle;

	Atanor* returnvalue;
    Atanor* previousinstruction;
    Atanor* currentinstruction;
	AtanorError* errorraised;
	hmap<string, ThreadLock*> locks;

	long parentthreads;
	short prologstack;

	bool error;
	bool used;

	ThreadStruct();
	Exporting ~ThreadStruct();

	long Size() {
		return (prologstack + stack.size());
	}

	void Update(short idthread);

	void Removevariable(short n) {
		if (variables.check(n)) {
			VECTE<Atanor*>& v = variables.get(n);
			if (v.last)
				v.pop_back();
		}
	}

	void Clear();
	threadhandle Initialization();
	Atanor* Raiserror(AtanorError* err);
	void Cleanerror();
	void Clearknowledgebase();
	void Setknowledgebase();

	Atanor* Getdefinition(short id);
	Atanor* Declarator(short id);
	bool isDeclared(short id);
	Atanor* GetTopFrame();

	bool TestPredicate(AtanorDeclaration* dom, AtanorPredicate* p);
	char isaValidPredicate(AtanorDeclaration* dom, AtanorPredicate* p, hmap<unsigned short, vector<AtanorPredicateRule*> >& rulebase);
	bool GetPredicates(AtanorDeclaration* dom, AtanorPredicate* p, vector<AtanorPredicate*>& res, bool cut);
	bool StorePredicate(AtanorDeclaration* dom, AtanorPredicate* pv, bool last);
	bool RemovePredicates(AtanorDeclaration* dom, AtanorPredicate* p);
	bool RemoveThePredicate(AtanorDeclaration* dom, AtanorPredicate* p);

	bool Checkpredicate(short name) {
		if (name == a_universal){
			if (knowledgebase.size())
				return true;
			return false;
		}

		if (knowledgebase.find(name) != knowledgebase.end())
			return true;
		return false;
	}

};

//-----------------------------------------------------------------------------------
//------------- MAIN GLOBAL OBJECT --------------------------------------------------
//-----------------------------------------------------------------------------------
class AtanorGlobal;
class AtanorSystemVariable;
typedef void(*DisplayFunctionCall)(string s, void* object);
typedef Atanor* (*AtanorProcedure)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
typedef Atanor* (*AtanorDebug)(vector<Atanor*>& stack, short idthread, void* data);
typedef void (*AtanorTermination)(AtanorGlobal* global);

class AtanorGlobal {
private:

	//The specific objects which we want to track (not exactly a garbage)
	vector<Atanor*> tracked;
	
	long maxthreads;
		
public:

	long maxjoined;
	long maxrange;

	//All programmes stored in memory
	vector<AtanorCode*> spaces;
	hmap<string, AtanorCode*> codes;

	ThreadStruct* threads;

#if defined(WIN32) || defined(APPLE)
	hmap<threadhandle, int> threadids;
#else
	map<threadhandle, int> threadids;
#endif
	

	//This variable is set to TRUE in thread mode... It allows then for the actual creation of locks...
	bool globalLOCK;

	hmap<string, ThreadLock*> locktables;
	hmap<string, LockedThread*> waitstrings;
	hmap<long, LockedThread*> threadvariables;

	void Releasevariables();
	void Triggeronfalse(Atanor* a);

	bool erroronkey;

	//-----------------------------------
	vector<string> filenames;
	//-----------------------------------
	hmap<short, mathFunc> mathFunctions;
	//-----------------------------------
	hmap<string, bool> systemfunctions;
	//-----------------------------------
	basebin_hash<short> throughs;
	//-----------------------------------
	basebin_hash<Atanor*> actions;
	//-----------------------------------
	basebin_hash<Atanor*> concepts;
	basebin_hash<basebin_hash<bool> > hierarchy;
	//-----------------------------------

	//Displaying stuff on screen or into a variable...


	Atanorstring* stringbuffer;
	Atanorstring* stringbuffererror;
	ostream* os;
	bool doubledisplay;
	DisplayFunctionCall displayfunction;
	void* displayobject;
	Atanor* kstd;
	Atanor* kerr;

	//-------------------------------
	long linereference;
	long lineerror;
	bnf_atanor* currentbnf;

	//-------------------------------
	AtanorDebug debugfunction;
	void* debugdata;

	AtanorTermination terminationfunction;
	//-------------------------------
	long spaceid;
	bool running;
	bool executionbreak;
	bool debugmode;
	bool windowmode;
	
	//We convert all our strings into short id...
	bin_hash<string> idSymbols;
#ifdef _DEBUG
	map<string, short> symbolIds;
#else
	hmap<string, short> symbolIds;
#endif
	hmap<string, short> string_operators;
	bin_hash<string> operator_strings;
	bin_hash<bool> atanOperatorMath;
	bin_hash<short> equto;

	bin_hash<short> returntypes;

	//--------------------------------
	//Buffers...
	vector<AtanorIndexbuff*> indexreservoire;
	VECTE<long> indexempties;
	long indexidx;
	AtanorObject* indexlock;
	Exporting AtanorIndex* Provideindex(AtanorIndex*, short);


	vector<Atanormapssbuff*> mapssreservoire;
	VECTE<long> mapssempties;
	long mapssidx;
	AtanorObject* mapsslock;
	Exporting Atanormapss* Providemapss();

	vector<Atanormapbuff*> mapreservoire;
	VECTE<long> mapempties;
	long mapidx;
	AtanorObject* maplock;
	Exporting Atanormap* Providemap();

	vector<Atanorvectorbuff*> vectorreservoire;
	VECTE<long> vectorempties;
	long vectoridx;
	AtanorObject* vectorlock;
	Exporting Atanorvector* Providevector();

	vector<Atanorivectorbuff*> ivectorreservoire;
	VECTE<long> ivectorempties;
	long ivectoridx;
	AtanorObject* ivectorlock;
	Exporting Atanorivector* Provideivector();

	vector<Atanorfvectorbuff*> fvectorreservoire;
	VECTE<long> fvectorempties;
	long fvectoridx;
	AtanorObject* fvectorlock;
	Exporting Atanorfvector* Providefvector();

	vector<Atanorsvectorbuff*> svectorreservoire;
	VECTE<long> svectorempties;
	long svectoridx;
	AtanorObject* svectorlock;
	Exporting Atanorsvector* Providesvector();

	vector<Atanoruvectorbuff*> uvectorreservoire;
	VECTE<long> uvectorempties;
	long uvectoridx;
	AtanorObject* uvectorlock;
	Exporting Atanoruvector* Provideuvector();

	vector<Atanorintbuff*> intreservoire;
	VECTE<long> iempties;
	long intidx;
	AtanorObject* intlock;
	Exporting Atanorint* Provideint(long v = 0);

	vector<Atanorfloatbuff*> floatreservoire;
	VECTE<long> fempties;
	long floatidx;
	AtanorObject* floatlock;
	Exporting Atanorfloat* Providefloat(double v = 0);

	vector<Atanorstringbuff*> stringreservoire;
	VECTE<long> sempties;
	long stringidx;
	AtanorObject* stringlock;
	Exporting Atanorstring* Providestring(string v = "");

	vector<Atanorustringbuff*> ustringreservoire;
	VECTE<long> uempties;
	long ustringidx;
	AtanorObject* ustringlock;
	Exporting Atanorustring* Provideustring(wstring v = L"");

	//--------------------------------
	Atanor* gNULL;
	Atanor* gUNIVERSAL;
	Atanor* gTRUE;
	Atanor* gFALSE;

	Atanor* gMINUSTEN;
	Atanor* gMINUSNINE;
	Atanor* gMINUSEIGHT;
	Atanor* gMINUSSEVEN;
	Atanor* gMINUSSIX;
	Atanor* gMINUSFIVE;
	Atanor* gMINUSFOUR;
	Atanor* gMINUSTHREE;
	Atanor* gMINUSTWO;
	Atanor* gMINUSONE;
	Atanor* gZERO;
	Atanor* gONE;
	Atanor* gTWO;
	Atanor* gTHREE;
	Atanor* gFOUR;
	Atanor* gFIVE;
	Atanor* gSIX;
	Atanor* gSEVEN;
	Atanor* gEIGHT;
	Atanor* gNINE;
	Atanor* gTEN;
	hmap<BLONG, Atanor*> numbers; //we record the values we've seen so far in order to avoid some overgeneration of constant values...

	Atanor* gEMPTYSTRING;
	Atanor* gBREAK;
	Atanor* gCONTINUE;
	Atanor* gRETURN;
	Atanor* gPIPE;
	Atanor* gNOELEMENT;
	Atanor* gDEFAULT;
	Atanor* gEND;
	Atanor* gRAISEERROR;
	AtanorCallBreak* gBREAKFALSE;
	AtanorCallBreak* gBREAKTRUE;
	Atanor* gAFFECTATION;
	AtanorConstiteration* gITERNULL;
	AtanorPredicate* gFAIL;
	AtanorPredicate* gCUT;
	AtanorPredicate* gSTOP;
	Atanor* gHASKELL;
	AtanorDeclaration* gNULLDECLARATION;

	Exporting void Update();
	//--------------------------------

	//This is a list of functions to actually parse code
	hmap<string, ParseElement> parseFunctions;

	//This is the association between an class id and its instance creation...
	bin_hash<Atanor*> newInstance;

	hmap<short, hmap<short, Atanor*> > mapnewInstances;

	bin_hash<bin_hash<unsigned long> > methods;
	bin_hash<unsigned long> allmethods;
	bin_hash<unsigned long> framemethods;
	bin_hash<bool> functions;
	bin_hash<AtanorProcedure> procedures;
	bin_hash<AtanorProcedure> commons;

	bin_hash<unsigned long> arities;
	bin_hash<AtanorFrame*> frames;

	bin_hash<AtanorSystemVariable*> systems;
	
	bin_hash<basebin_hash<bool> > compatibilities;
	bin_hash<basebin_hash<bool> > strictcompatibilities;
	
	bin_hash<AtanorFrame*> extensions;
	bin_hash<bool> extensionmethods;

	AtanorPredicateContainer* predicateContainer;

	hmap<string, automate*> rules;
	void AtanorAllObjects(vector<string>& vs);
	//--------------------------------
	long maxstack;
	//--------------------------------

	hmap<string, AtanorPredicateVariable*> predicatevariables;
	map<string, Atanor*> dependencyvariables;
	basebin_hash<short> dependenciesvariable;
	Atanor* modifieddependency;
	bin_hash<AtanorPredicateFunction*> predicates;
	bin_hash<string> terms;

	void Setnonblockingerror(string s, short idthread) {
		threads[idthread].nonblockingerror = s;
	}

	string Getnonblockingerror(short idthread) {
		return threads[idthread].nonblockingerror;
	}

	bool TestPredicate(AtanorDeclaration* dom, AtanorPredicate* p, short idthread) {
		return threads[idthread].TestPredicate(dom, p);
	}
	
	char isaValidPredicate(AtanorDeclaration* dom, AtanorPredicate* p, hmap<unsigned short, vector<AtanorPredicateRule*> >& rulebase, short idthread) {
		return threads[idthread].isaValidPredicate(dom, p, rulebase);
	}
	
	bool Checkpredicate(short name, short idthread) {
		return threads[idthread].Checkpredicate(name);
	}

	bool GetPredicates(AtanorDeclaration* dom, AtanorPredicate* p, vector<AtanorPredicate*>& res, bool cut, short idthread) {
		return threads[idthread].GetPredicates(dom, p, res, cut);
	}
	
	bool StorePredicate(AtanorDeclaration* dom, AtanorPredicate* pv, bool last, short idthread) {
		return threads[idthread].StorePredicate(dom, pv, last);
	}
	
	bool RemovePredicates(AtanorDeclaration* dom, AtanorPredicate* p, short idthread) {
		return threads[idthread].RemovePredicates(dom, p);
	}

	bool RemoveThePredicate(AtanorDeclaration* dom, AtanorPredicate* p, short idthread) {
		return threads[idthread].RemoveThePredicate(dom, p);
	}
	
	AtanorPredicateContainer* Predicatecontainer();

	AtanorPredicateContainer* GetPredicatecontainer(short idthread) {
		return predicateContainer;
	}

	bool predicateTrace(short n);


	//--------------------------------
	
	AtanorGlobal(long nb);

	~AtanorGlobal();

	//--------------------------------
	//Threading...
	ThreadLock _locker;
	ThreadLock _join;
	ThreadLock _call;
	ThreadLock _printlock;
	ThreadLock _knowledgelock;

	bool isRunning() {
		return running;
	}

	bool Stop() {
		return executionbreak;
	}

	Exporting short GetThreadid();
	void EraseThreadid(short id);
	short InitThreadid(short id);
	short GetaThreadId();
	short Joinpos(short id);

	void Sethreading() {
		globalLOCK = true;
	}

	bool isThreading() {
		return globalLOCK;
	}

	void Reallocatemaxthreads(short nb) {
		if (nb > maxthreads) {
			delete[] threads;
			maxthreads = nb;
			threads = new ThreadStruct[maxthreads];
		}
	}


	void Reallocatemaxjoined(short nb) {
		if (nb > maxjoined)
			maxjoined = nb;
	}

	bool TestEndthreads();

	Exporting void Getdebuginfo(string& localvariables, string& allvariables, string& stack, bool, short idthread);

	void Pushpredicate(short idthread) {
		threads[idthread].prologstack++;
		if (threads[idthread].Size() >= maxstack)
			Returnerror("Stack overflow", idthread);
	}

	void Poppredicate(short idthread) {
		threads[idthread].prologstack--;
	}

    void Cleardebug(short idthread) {
        threads[idthread].previousinstruction=NULL;
    }
    
	//Push on stack a function or a domain
	void Pushstack(Atanor* a, short idthread = 0);
	void Popstack(short idthread = 0) {
		threads[idthread].stack.pop_back();
		if (debugmode)
			Popdebug(idthread);
	}

	Atanor* Topstack(short idthread = 0) {
		return threads[idthread].stack.back();
	}

	size_t Stacksize(short idthread = 0) {
		return threads[idthread].stack.size();
	}

	Atanor* Stack(size_t i, short idthread = 0) {
		if (i >= threads[idthread].stack.size())
			return NULL;
		return threads[idthread].stack[i];
	}

	Atanor* GetTopFrame(short idthread);
	Atanor* Getdeclaration(short id, short idthread);
	Atanor* Getdefinition(short id, short idthread, Atanor* current = aNULL);
	bool isDeclared(short id, short idthread);
	Atanor* Declarator(short id, short idthread);

	void Update(short idthread, short fromidthread) {
		threads[idthread].Update(fromidthread);
	}

	Atanor* Getvariable(short idthread, short name) {
		return threads[idthread].variables[name].back();
	}

	void Storevariable(short idthread, short name, Atanor* var) {
		threads[idthread].variables[name].push_back(var);
	}

	void Removevariable(short idthread, short name) {
		threads[idthread].Removevariable(name);
	}

	void Current(Atanor* g, short idthread) {
		threads[idthread].currentinstruction = g;
	}

	void Pushdebug(short idthread) {		
		if (debugmode) {
			threads[idthread].debugstack.push_back(threads[idthread].currentinstruction);
			if (debugfunction != NULL)
				(*debugfunction)(threads[idthread].debugstack, idthread, debugdata);
		}
	}

	void Popdebug(short idthread) {
		if (debugmode)
			threads[idthread].debugstack.pop_back();
	}

	void Setdebugmode(bool d) {
		debugmode = d;
	}

	void Setdebugfunction(AtanorDebug f, void* data = NULL) {
		debugfunction = f;
		debugdata = data;
	}

	long Getinstructionline(short idthread);
	//--------------------------------
	Exporting void RecordCompileFunctions();
	Exporting void RecordCompatibilities();
	Exporting void SetCompatibilities(short ty);
	Exporting void RecordObjects();
	Exporting void RecordContainers();
	Exporting void RecordConstantNames();
	Exporting void RecordProcedures();
	Exporting void RecordOneProcedure(string, AtanorProcedure, unsigned long arity, short rettype = a_null);
	Exporting void RecordCommon(string, AtanorProcedure, unsigned long arity, short rettype = a_null);
	Exporting void RecordSystemVariables();
	Exporting void RecordPredicates();
	Exporting void RecordPredicateFunctions();
	Exporting void RecordMathFunctions();

	void SystemInitialization(string spath);


	Exporting void RecordMethods(short type, bin_hash<unsigned long>& exported);

	Exporting void RecordMethods(short type, short name, unsigned long arity);

//------------------------------ External Libraries ---------------------------------------
	hmap<string, AtanorExternalModule> externalLibraries;
	Exporting bool RecordExternalLibrary(string name, AtanorExternalModule);
	//--------------------------------Evaluate Function------------------------------------

	Exporting Atanor* EvaluateLisp(string& s, string& o, string& c, short idthread);
	Exporting Atanor* EvaluateTags(agnostring& s, string& o, string& c, short idthread);
	Exporting Atanor* EvaluateVector(string& s, short idthread);
	Exporting Atanor* EvaluateMap(string& s, short idthread);
	Exporting Atanor* EvaluateJSON(string& s, short idthread);

	bool Checkhierarchy(short c1, short c2) {
		if (c1 == c2)
			return true;
		if (hierarchy.check(c1) && hierarchy[c1].check(c2))
			return true;
		return false;
	}

	bool Testcompatibility(short r, short v, bool strict) {
		if (strict) {
			if (strictcompatibilities.check(r) && strictcompatibilities[r].check(v))
				return true;
			return false;
		}

		if (compatibilities.check(r) && compatibilities[r].check(v))
			return true;
		return false;
	}

	bool Compatible(short r, short v) {
		if (compatibilities.check(r) && compatibilities[r].check(v))
			return true;
		return false;
	}

	bool Compatiblestrict(short r, short v) {
		if (strictcompatibilities.check(r) && strictcompatibilities[r].check(v))
			return true;
		return false;
	}

	//--------------------------------
	static vector<string> arguments;
	void Setarguments(string args);
	//--------------------------------

	void Clear() {
		for (long i = 0; i < maxthreads; i++)
			threads[i].Clear();
	}

	//--------------------------------
	string Getsymbol(short s) {
		if (idSymbols.check(s))
			return idSymbols[s];
		return "";
	}

	short Getid(string s) {
		if (symbolIds.find(s) == symbolIds.end()) {
			short id = (short)symbolIds.size();
			symbolIds[s] = id;
			idSymbols[id] = s;
			return id;
		}

		return symbolIds[s];
	}

	short Createid(string s) {
		short id = (short)symbolIds.size();
		symbolIds[s] = id;
		idSymbols[id] = s;
		return id;
	}

	//--------------------------------
	AtanorCode* GetNewCodeSpace(string filename);
	AtanorCode* Getcode(size_t idcode) {
		if (idcode >= spaces.size())
			return NULL;
		return spaces[idcode];
	}

	AtanorCode* Getcurrentcode() {
		if (spaceid != -1)
			return spaces[spaceid];
		return NULL;
	}

	Atanor* Getmainframe(size_t idcode);

	long Getcurrentline();
	string Getcurrentfile();
	short Getfileid(short& idc, string f);
	Exporting string Getfilename(short fileid);

	Atanor* EvaluateMainVariable();

	bool Loadcontent(string content);
	AtanorCode* Loadfile(string filename);

	//--------------------------------
	long trackercursor;
	
	Exporting void RecordInTracker(Atanor* a);
	Exporting long RecordInTrackerProtected(Atanor* a);
	Exporting void RemoveFromTracker(long id);
	Exporting bool Checktracker(Atanor* a, long id);
	Exporting void ResetWithTracker(Atanor*, long id, long inc = 1);
	Exporting Atanor* GetFromTracker(long id);

	Atanor* Gettracked(long id) {
		if (id >= tracked.size())
			return NULL;
		return tracked[id];
	}

	size_t Trackedsize() {
		return tracked.size();
	}

	void Cleanfrom(long mx);

	//--------------------------------
	bool Error(short idthread) {
		if (executionbreak || threads[idthread].error)
			return true;
		return false;
	}

	bool GenuineError(short idthread) {
		if (threads[idthread].error)
			return true;
		return false;
	}


	Exporting Atanor* Errorobject(short idthread);
	string Errorstring(short idthread);
	Exporting Atanor* Returnerror(Atanor* err, short idthread);
	Exporting Atanor* Returnerror(string err, short idthread);
	Exporting Atanor* Returnerror(string err);

	void Cleanerror(short idthread);

	bool isFile(Atanor* a);
	bool isString(Atanor* a);
	bool isNumber(Atanor* a);
	bool isContainer(Atanor* a);
	bool isVectorContainer(Atanor* a);
	bool isMapContainer(Atanor* a);
	short Type(Atanor* a);
};

//-----------------------------------------------------------------------

class AtanorRaiseError {
public:

	string message;
	string filename;
	size_t left;
	size_t right;

	AtanorRaiseError(stringstream& mes, string file = "", size_t l = 0, size_t r = 0);
};

class Atanordebug {
public:

	short idthread;
	bool protect;
	bool protectfirst;

	Atanordebug(short id, Atanor* i = NULL) {
		idthread = id;
		protect = true;
		if (i != NULL) {
			globalAtanor->Pushdebug(idthread);
			protectfirst = false;
		}
		else
			protectfirst = true;
	}

	void push() {
		protect = false;
		globalAtanor->Pushdebug(idthread);
	}

	void pop() {
		globalAtanor->Popdebug(idthread);
		protect = true;
	}

	~Atanordebug() {
		if (!protect)
			globalAtanor->Popdebug(idthread);
		if (!protectfirst)
			globalAtanor->Popdebug(idthread);
	}

};

#endif





