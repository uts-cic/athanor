#ifndef atanorwapiti_h
#define atanorwapiti_h

extern "C" {
#include "options.h"

	char* CRFError();
	void DoTrain(opt_t* opt);
	void DoLabel(opt_t* opt);
	void WAPITIInit();
	char* WAPITILastError();

	void* CreatePureTagger(opt_t* o);
	void CleanPureTagger(void* pt);
	bool LabelPureTagger(void* tp, char** words, char** results, int sz);
	bool InitPureTagger(void* tp);

	static void initopt(opt_t& opt) {
		opt.mode = -1;
		opt.input = NULL;
		opt.output = NULL;
		opt.type = "crf";
		opt.maxent = false;
		opt.algo = "l-bfgs";
		opt.pattern = NULL;
		opt.model = NULL;
		opt.devel = NULL;
		opt.rstate = NULL;
		opt.sstate = NULL;
		opt.compact = false;
		opt.sparse = false;
		opt.nthread = 1;
		opt.jobsize = 64;
		opt.maxiter = 0;
		opt.rho1 = 0.5;
		opt.rho2 = 0.0001;
		opt.objwin = 5;
		opt.stopwin = 5;
		opt.stopeps = 0.02;
		opt.lbfgs.clip = false;
		opt.lbfgs.histsz = 5;
		opt.lbfgs.maxls = 40;
		opt.sgdl1.eta0 = 0.8;
		opt.sgdl1.alpha = 0.85;
		opt.bcd.kappa = 1.5;
		opt.rprop.stpmin = 1e-8;
		opt.rprop.stpmax = 50.0;
		opt.rprop.stpinc = 1.2;
		opt.rprop.stpdec = 0.5;
		opt.rprop.cutoff = false;
		opt.label = false;
		opt.check = false;
		opt.outsc = false;
		opt.lblpost = false;
		opt.nbest = 1;
		opt.force = false;
		opt.prec = 5;
		opt.all = false;
	}

}

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorwapiti;
//This typedef defines a type "wapitiMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorwapiti::*wapitiMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------

class Atanorwapiti : public AtanorObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static hmap<unsigned short, wapitiMethod> methods;
	static hmap<string, string> infomethods;
	static bin_hash<unsigned long> exported;

	static short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	string crfmodel;
	void* crftagger;
	char loadingmode;
	opt_t opt;
	char* args[100];

	//---------------------------------------------------------------------------------------------------------------------
	Atanorwapiti(AtanorGlobal* g = NULL, Atanor* parent = NULL) : AtanorObject(g, parent) {
		//Do not forget your variable initialisation
		crftagger = NULL;
		loadingmode = 0;
		initopt(opt);
		for (int i = 0; i < 100; i++)
			args[i] = (char*)malloc(1024);
		WAPITIInit();
	}

	~Atanorwapiti() {
		for (int i = 0; i < 100; i++)
			free(args[i]);
		if (loadingmode == 2)
			CleanPureTagger(crftagger);
	}

	//----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* v, Atanor* i, short idthread) {
		return aTRUE;
	}

	Atanor* Get(Atanor* context, Atanor* v, short idthread) {
		return this;
	}


	short Type() {
		return Atanorwapiti::idtype;
	}

	string Typename() {
		return "wapiti";
	}

	//The next two functions work in conjunction...
	//If your object is very atomic (such as a number or a string)
	//Then when it is passed to a function or stored into a container,
	//it might be worth duplicating it.
	// In that case: duplicateForCall should return true...

	//However if your object is complex and probably unique through out the code
	//Then duplicateForCall should return false, and Atom should always reduce to a "return this;" only...
	Atanor* Atom(bool forced = false) {
		if (forced || !protect)
			return new  Atanorwapiti;
		return this;
	}

	bool duplicateForCall() {
		return true;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	bool isDeclared(short n) {
		if (exported.find(n) != exported.end())
			return true;
		return false;
	}

	Atanor* Newinstance(short, Atanor* f = NULL) {
		return new Atanorwapiti;
	}

	static void AddMethod(AtanorGlobal* g, string name, wapitiMethod func, unsigned long arity, string infos);
	static bool InitialisationModule(AtanorGlobal* g, string version);

	void Methods(Atanor* v) {
		hmap<string, string>::iterator it;
		for (it = infomethods.begin(); it != infomethods.end(); it++)
			v->storevalue(it->first);
	}

	string Info(string n) {

		if (infomethods.find(n) != infomethods.end())
			return infomethods[n];
		return "Unknown method";
	}


	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is a sample  of a function that could be implemented for your needs.

	Atanor* MethodLoadModel(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodOptions(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodTraining(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodLabelling(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodLasterror(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);


	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
	//is returned by the Declaration method.
	Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		return (this->*methods[idname])(contextualpattern, idthread, callfunc);
	}

	void Clear(AtanorGlobal* g) {
		//To set a variable back to empty
	}

	void Clean() {
		//Your cleaning code
	}

	string String() {
		return "";
	}

	wstring UString() {
		return L"";
	}

	long Integer() { return 0; }
	short Short() { return 0; }
	double Float() { return 0; }
	bool Boolean() { return false; }
	BLONG Long() { return 0; }
	unsigned char Byte() {
		return 0;
	}

	long Size() { return 0; }

};
#endif
