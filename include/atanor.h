
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

#ifndef atanor_h
#define atanor_h

#include <stdio.h>
#include <string.h>

#include <thread>
#include <mutex>
#include <condition_variable>

#ifdef UNIX
#define sprintf_s snprintf
#define strcat_s(a,c,b) strncat(a,b,c)
#define strcpy_s(a,c,b) strncpy(a,b,c)
#include <math.h>
#endif

#include "conversion.h"
#include "vecte.h"

class AtanorCode;
class AtanorGlobal;
class Atanor;
class AtanorCall;
class AtanorDeclaration;
class AtanorConst;
class AtanorFunction;
class AtanorFrame;
class AtanorIteration;
class AtanorInstruction;
class Atanorfraction;
class AtanorFunctionLambda;
class AtanorIndex;
class AtanorPredicateContainer;
class AtanorPredicateVariableInstance;
class AtanorInstructionEvaluate;
#include "atanorconstants.h"

#ifdef WIN32
#define localthread __declspec( thread )
#else
#define localthread thread_local
#endif
//-----------------------------------------------------------------------
//The main variable, which controls the world...
extern Exchanging AtanorGlobal* globalAtanor;
uchar Returnequ(short ty);
//-----------------------------------------------------------------------

#include "atanorglobal.h"

//Atanor is the class from which every element descends
class Atanor {
public:
	long idtracker;

#ifdef _DEBUG
	long iddebug;
	Exporting Atanor();
	Exporting virtual ~Atanor();
#else
	Atanor() {
		idtracker = -1;
	}

	virtual ~Atanor() {}
#endif

	virtual Atanor* Compile(Atanor* parent) {
		return this;
	}

	//These two methods are the most important, they store or get a value from an element
	virtual Atanor* Put(Atanor* index, Atanor* value, short idthread) {
		return this;
	}

	virtual Atanor* Get(Atanor* context, Atanor* value, short idthread) {
		return this;
	}

	virtual Atanor* Execute(Atanor* environment, Atanor* value, short idthread) {
		return Get(environment, value, idthread);
	}

    virtual Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        return this;
    }
    
    virtual Atanor* CallTopMethod(short idname, AtanorFrame* frame, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        return this;
    }
    
    virtual bool Setvalue(Atanor* index, Atanor* value, short idthread, bool strict = false) {
		return Put(index, value, idthread)->Boolean();
	}

	virtual Atanor* Putvalue(Atanor* value, short idthread) {
		return Put(aNULL, value, idthread);
	}

	virtual Atanor* Loopin(AtanorInstruction*, Atanor* context, short idthread) {
		return globalAtanor->Returnerror("Cannot loop on this element...", idthread);
	}

	virtual AtanorIndex* Evaluate(short idthread) { return NULL; }
	virtual void Rollback() {}

	virtual void Insert(int idx, Atanor* ke) {}

	//Some building operations...
	//This operation is called whenever an instruction is added to a structure...
	virtual void Subcontext(bool b) {}
	virtual Atanor* Parent() {
		return aNULL;
	}
	virtual void InstructionClear() {}
	virtual Atanor* Lastinstruction() { return aNULL; }
	virtual void AddInstruction(Atanor* a) {}
	virtual void Putinstruction(size_t i, Atanor* ke) {}
	virtual Atanor* InstructionRemoveLast() { return aNULL; }
	virtual void InstructionRemove(size_t i) {}
	virtual size_t InstructionSize() { return 0; }
	virtual Atanor* Instruction(size_t i) { return aNULL; }

	virtual void ScanVariables(vector<short>& vars) {}
	virtual void CheckHaskellComposition() {}

	virtual void Setinfo(Atanor* a) {}
	virtual void Getinfo(Atanor* a) {}
	virtual Atanor* Initialisation() { 
		return aNULL; 
	}

	virtual bool Checkvariable() {
		return false;
	}

	virtual void Setchoice(char v) {}

	virtual char Choice() {
		return -1;
	}
	//------------------------------------------------------------------
	//Reference operations
	virtual void Setreference(short r = 1) {}
	virtual void Resetreference(short r = 1) {}
	virtual void Resetreferencenoprotect(short r = 1) {}
	virtual void Release() {}
	virtual void Protect() {}
	virtual void Remove() {}
	virtual void SetConst() {}

	virtual string Info(string n) {
		return "Unknown method";
	}

	virtual void Methods(Atanor*) {}

	virtual void Prepare(Atanor* env, short idthread) {}
	virtual void Setaffectation(bool b){}
	virtual void Setevaluate(bool v) {}

	Exporting virtual Atanor* Succ();
	Exporting virtual Atanor* Pred();

	virtual Atanor* Nextfunction() {
		return NULL;
	}


	virtual Atanor* Atom(bool forced = false) {
		return this;
	}

	virtual Atanor* Returned(short idthread) {
		return this;
	}

	virtual Atanor* Value() {
		return this;
	}

	virtual Atanor* Value(wstring w) {
		string res;
		s_unicode_to_utf8(res, w);
		return Value(res);
	}


	virtual Atanor* Value(string) {
		return aNOELEMENT;
	}

	virtual Atanor* Value(long) {
		return aNOELEMENT;
	}

	virtual Atanor* Value(double) {
		return aNOELEMENT;
	}

	virtual Atanor* Value(Atanor*) {
		return aNOELEMENT;
	}

	//------------------------------------------------------------------

	Exporting virtual bool Unify(AtanorDeclaration* dom, Atanor* a);
	virtual bool isUnified(AtanorDeclaration* dom) {
		return true;
	}
	
	virtual bool isComputable(AtanorDeclaration* dom) {
		return isUnified(dom);
	}

	virtual void Cleans(Atanor* v, bool localvalue) {}

	virtual void Leaves(Atanor* v) {
		v->Push(this);
	}

	virtual Atanor* Returnobject() { return aNULL; }

	virtual Atanor* PredicateEvalue(AtanorInstructionEvaluate* context, VECTE<Atanor*>& stack, AtanorPredicate* currenthead, int depth) {
		return aFALSE;
	}

	Exporting virtual Atanor* Predicate(AtanorDeclaration* dom, short idthread);
	virtual bool Insertvalue(Atanor* dom, Atanor* v, basebin_hash<Atanor*>&) {
		return true;
	}

	virtual void Variables(vector<short>& vars) {}

	virtual Atanor* VariableValue(AtanorDeclaration* dom, short idthread) {
		return aNULL;
	}

	virtual Atanor* Variable(AtanorDeclaration* dom) {
		return NULL;
	}

	Exporting virtual Atanor* ExtractPredicateVariables(Atanor* contextualpattern, AtanorDeclaration* dom, Atanor* c, Atanor* e, short, bool root);
	Exporting virtual Atanor* EvaluePredicateVariables(Atanor* context, AtanorDeclaration* dom, short idthread);

	virtual void Setfail(bool) {}
	virtual Atanor* Last() {
		return aNULL;
	}
	//------------------------------------------------------------------
	virtual void SetVariablesWillBeCreated() {}
	virtual void Forcedclean() {}
	virtual void Setforced(bool v) {}
	virtual Atanor* Newinstance(short idthread, Atanor* f = NULL) { return this; }
	virtual Atanor* Newvalue(Atanor* a, short idthread) {
		Atanor* v = Newinstance(idthread);
		v->Put(aNULL, a, idthread);
		a = v->Get(aNULL, aNULL, idthread);
		v->Release();
		return a;
	}

	Exporting virtual AtanorIteration* Newiteration(bool direction);

	virtual void Postinstanciation(short idthread, bool setreference) {}

	virtual bool isObject() {
		return false;
	}

	virtual bool hasLock() {
		return false;
	}

	virtual long Lockid() {
		return -1;
	}
	
	virtual bool isEQU() {
		return false;
	}

	virtual bool isAffectation() {
		return false;
	}

	virtual bool isReturned() {
		return false;
	}

	virtual bool needInvestigate() {
		return false;
	}

	virtual bool needFullInvestigate() {
		return false;
	}

	virtual bool isSequence() {
		return false;
	}

	virtual bool hasDeclaration() {
		return false;
	}

	virtual bool isDeclaration() {
		return false;
	}

	virtual bool isDeclared(short id) {
		return false;
	}

	virtual void Declare(short id, Atanor* a) {}

	virtual Atanor* Declaration(short id) {
		return NULL;
	}

	virtual short Type() {
		return 0;
	}

	virtual short Typenumber() {
		if (isNumber())
			return Type();
		return 0;
	}

	virtual void Setname(short n) {}

	virtual string Typename() {
		return "";
	}

	void Setid(long i) {
		idtracker = i;
	}

	long Idtracker() {
		return idtracker;
	}

	virtual void Setcodereference(long l, short i) {}
	virtual long Currentline() {
		return -1;
	}

	virtual short Currentspace() {
		return -1;
	}

	virtual short Currentfile() {
		return -1;
	}

	virtual void Setaction(short a) {}
	virtual short Action() {
		return a_none;
	}

	virtual void Addparent(Atanor*) {}

	virtual Atanor* Frame() {
		return NULL;
	}

    virtual bool isParent(AtanorFrame*) {
        return false;
    }
    

	virtual void Setprotect(bool n) {}

	virtual void Protectcontainervalues() {}

	virtual void Popping() {}
	
	virtual long Setprotect() {
		return -1;
	}

	//-----------------------------------------------
	virtual void Reserve(long d) {}
	virtual void Clone(AtanorInstruction* ki) {}
	virtual Atanor* Merging(Atanor*) {
		return aNOELEMENT;
	}
	//-----------------------------------------------
	virtual void Setdisjunction(bool v) {}
	virtual basebin_hash<AtanorPredicateVariableInstance*>* Dico() {
		return NULL;
	}

	virtual void Setdico(short n, AtanorPredicateVariableInstance* a) {}
	virtual AtanorPredicateVariableInstance* Getdico(short n) {
		return NULL;
	}

	virtual bool Checkdico(short n) {
		return false;
	}

	virtual void setSucessfull(bool v) {}
	virtual bool hasbeenSuccesfull() {
		return false;
	}

	virtual char Indexes(short idthread, long sz, long& ileft, long& iright) {
		Atanor* val = Get(aNULL, aNULL, idthread);
		ileft = val->Integer();
		val->Release();
		if (ileft < 0 || ileft >= sz)
			return 0;
		return 1;
	}

	virtual bool Failed() {
		return false;
	}

	virtual Atanor* Evalue(AtanorDeclaration* dom, short idthread, bool duplicate) {
		return Getvalues(dom, idthread);
	}

	Exporting virtual Atanor* Getvalues(AtanorDeclaration* dom, bool duplicate);
	virtual void Setmerge() {}

	virtual bool Checkprecision(Atanor* r) {
		return true;
	}

	//-----------------------------------------------
	virtual bool isPredicateVariable() {
		return false;
	}

	virtual bool isPredicateFunction() {
		return false;
	}

	virtual bool isPredicate() {
		return false;
	}

	virtual bool isDisjunction() {
		return false;
	}

	// Status Functions
	virtual long Reference() {
		return 0;
	}

	virtual bool duplicateForCall() {
		return true;
	}

	virtual bool Subcontext() {
		return false;
	}

	virtual bool isProtected() {
		return false;
	}

	virtual bool isPrivate() {
		return false;
	}

	virtual bool isCall() {
		return false;
	}

	virtual bool isCallFunction() {
		return false;
	}

	virtual bool isString() {
		return false;
	}

	virtual bool isFile() {
		return false;
	}

	virtual bool isLong() {
		return false;
	}
	
	virtual bool isShort() {
		return false;
	}

	virtual bool isFloat() {
		return false;
	}

	virtual bool isInteger() {
		return false;
	}

	virtual bool isDecimal() {
		return false;
	}

	virtual bool isCallVariable() {
		return false;
	}

	virtual bool isPureCallVariable() {
		return false;
	}

	virtual bool isAssignable() {
		return false;
	}

	virtual bool isBoolean() {
		return false;
	}

	virtual bool isIncrement() {
		return false;
	}

	virtual bool isAtom() {
		return false;
	}

	virtual bool isNumber() {
		return false;
	}

	virtual bool isJoined() {
		return false;
	}

	virtual bool isContainer() {
		return false;
	}

	virtual bool isValueContainer() {
		return false;
	}

	virtual bool isVectorValueContainer() {
		return false;
	}

	virtual bool isVectorContainer() {
		return false;
	}

	virtual bool isMerge() {
		return false;
	}

	virtual bool isMapContainer() {
		return false;
	}

	virtual bool isConst() {
		return false;
	}

	virtual bool isEvaluate() {
		return false;
	}

	virtual bool isEmpty() {
		return false;
	}

	virtual bool isFrame() {
		return false;
	}

	virtual bool isInfinite() {
		return false;
	}

	virtual bool isChosen() {
		return false;
	}

	virtual bool isROOTOPERATION() {
		return false;
	}

	virtual bool isOperation() {
		return false;
	}

	virtual bool isInstruction() {
		return false;
	}

	virtual bool isIndex() {
		return false;
	}

	virtual bool isInterval() {
		return false;
	}

	virtual bool isStop() {
        return false;
    }
    
	virtual short Name() {
		return 0;
	}

	virtual bool isFunction() {
		return false;
	}

	virtual bool isFunctionParameter() {
		return false;
	}

	virtual bool isThread() {
		return false;
	}

	virtual bool isExclusive() {
		return false;
	}

	virtual bool isHaskellFunction() {
		return false;
	}

	virtual bool Predeclared() {
		return false;
	}

	virtual bool isUsed() {
		return false;
	}

	virtual short Typevariable() {
		return Type();
	}

	virtual short Typevariable(short i) {
		return a_none;
	}

	virtual short Typeinfered() {
		return Typevariable();
	}

	virtual short Yourowntype() {
		return 0;
	}

	virtual bool isError() {
		return false;
	}

	virtual bool baseValue() {
		return false;
	}

	virtual bool isConstant() {
		return false;
	}

	virtual bool Alreadydeclared() {
		return false;
	}

	virtual bool isVariable() {
		return false;
	}

	virtual bool isLetSelf() {
		return false;
	}

	virtual bool isConcept() {
		return false;
	}

	virtual bool isTracked() {
		return false;
	}

	virtual bool isNegation() {
		return false;
	}

	virtual void Setnegation(bool n) {}
    virtual bool Setstopindex() {
        return false;
    }
    
	//-----------------------------------------------
	Exporting virtual Atanor* Loophaskell(Atanor* recipient, Atanor* context, Atanor* env, AtanorFunctionLambda* bd, short idthread);
	Exporting virtual Atanor* Filter(short idthread, Atanor* env, AtanorFunctionLambda* bd, Atanor* var, Atanor* kcont, Atanor* accu, Atanor* init, bool direct);
	Exporting virtual Atanor* Filterboolean(short idthread, Atanor* env, AtanorFunctionLambda* bd, Atanor* var, Atanor* def);

	virtual void Addargmode() {}
	virtual void Addfunctionmode() {}

	virtual void Addfunction(Atanor* a) {}
	virtual void Sharedeclaration(AtanorFrame* k, bool final) {}
	virtual Atanor* Getindex() {
		return NULL;
	}

	virtual Atanor* Function() {
		return NULL;
	}

	virtual Atanor* Body(short) {
		return this;
	}

	virtual Atanor* Composition() {
		return aNOELEMENT;
	}

	//-----------------------------------------------
	virtual unsigned int EditDistance(Atanor* e) {
		if (e->Type() != Type() || !same(e)->Boolean())
			return 1;
		return 0;
	}

	//Vector and Map operations...
	virtual Atanor* Push(Atanor*) {
		return this;
	}

	virtual Atanor* push(Atanor*) {
		return this;
	}

	Exporting Atanor* Pushinmap(Atanor* a, short idhtread);

	virtual Atanor* Push(Atanor*, Atanor*) {
		return this;
	}

	Exporting virtual Atanor* Push(BLONG k, Atanor* a);
	Exporting virtual Atanor* Push(long k, Atanor* v);
	Exporting virtual Atanor* Push(ushort k, Atanor* v);
	Exporting virtual Atanor* Push(double k, Atanor* v);
	Exporting virtual Atanor* Push(string k, Atanor* v);

	virtual Atanor* push(string k, Atanor* v) {
		return this;
	}

	Exporting virtual Atanor* Push(wstring k, Atanor* v);

	virtual Atanor* Pop(Atanor*) {
		return this;
	}

	virtual Atanor* Vector(short idthread) {
		return aNULL;
	}

	virtual Atanor* Map(short idthread) {
		return aNULL;
	}

	//--------------------------------------------------

	virtual Atanor* Parameter(size_t i) {
		return aNULL;
	}

	virtual Atanor* Argument(size_t i) {
		return aNULL;
	}

	//--------------------------------------------------
	virtual bool Checkarity() {
		return true;
	}

	//--------------------------------------------------
	virtual Atanor* getvalue(BLONG i) {
		return aNOELEMENT;
	}

	Exporting virtual void addustringto(wstring ws);
	Exporting virtual void addstringto(string s);
	Exporting virtual void addstringto(wchar_t s);

	Exporting virtual void addustringto(wstring ws, int i);
	Exporting virtual void addstringto(string s, int i);
	Exporting virtual void addstringto(wchar_t s, int i);


	Exporting virtual void storevalue(string u);
	Exporting virtual void storevalue(float u);
	Exporting virtual void storevalue(short u);
	Exporting virtual void storevalue(wstring u);
	Exporting virtual void storevalue(long u);
	Exporting virtual void storevalue(BLONG u);
	Exporting virtual void storevalue(double u);
	Exporting virtual void storevalue(unsigned char u);
	Exporting virtual void storevalue(wchar_t u);

	virtual Atanor* Getvector(Atanor* value, short idthread, bool skip = false) {
		return Get(aNULL, aNULL, idthread);
	}

	virtual Atanor* Getmap(Atanor* value, short idthread, bool skip = false) {
		return Get(aNULL, aNULL, idthread);
	}

	virtual long Getinteger(short idthread) {
		Atanor* value = Get(aNULL, aNULL, idthread);
		long v = value->Integer();
		value->Release();
		return v;
	}

	virtual BLONG Getlong(short idthread) {
		Atanor* value = Get(aNULL, aNULL, idthread);
		BLONG v = value->Long();
		value->Release();
		return v;
	}

	virtual short Getshort(short idthread) {
		Atanor* value = Get(aNULL, aNULL, idthread);
		short v = value->Short();
		value->Release();
		return v;
	}

	virtual float Getdecimal(short idthread) {
		Atanor* value = Get(aNULL, aNULL, idthread);
		float v = value->Decimal();
		value->Release();
		return v;
	}

	virtual double Getfloat(short idthread) {
		Atanor* value = Get(aNULL, aNULL, idthread);
		double v = value->Float();
		value->Release();
		return v;
	}

	virtual string Getstring(short idthread) {
		Atanor* value = Get(aNULL, aNULL, idthread);
		string v = value->String();
		value->Release();
		return v;
	}

	virtual wstring Getustring(short idthread) {
		Atanor* value = Get(aNULL, aNULL, idthread);
		wstring v = value->UString();
		value->Release();
		return v;
	}

	//----------------------------------------------------------------------
	virtual long Getinteger(Atanor* context, Atanor* value, short idthread) {
		if (isAtom())
			return Integer();

		value = Get(context, aNULL, idthread);
		long v = value->Integer();
		if (value != this)
			value->Release();
		return v;
	}

	virtual BLONG Getlong(Atanor* context, Atanor* value, short idthread) {
		if (isAtom())
			return Long();

		value = Get(context, aNULL, idthread);
		BLONG v = value->Long();
		if (value != this)
			value->Release();
		return v;
	}

	virtual short Getshort(Atanor* context, Atanor* value, short idthread) {
		if (isAtom())
			return Short();

		value = Get(context, aNULL, idthread);
		short v = value->Short();
		if (value != this)
			value->Release();
		return v;
	}

	virtual float Getdecimal(Atanor* context, Atanor* value, short idthread) {
		if (isAtom())
			return Decimal();

		value = Get(context, aNULL, idthread);
		float v = value->Decimal();
		if (value != this)
			value->Release();
		return v;
	}

	virtual double Getfloat(Atanor* context, Atanor* value, short idthread) {
		if (isAtom())
			return Float();

		value = Get(context, aNULL, idthread);
		double v = value->Float();
		if (value != this)
			value->Release();
		return v;
	}

	virtual string Getstring(Atanor* context, Atanor* value, short idthread) {
		if (isAtom())
			return String();

		value = Get(context, aNULL, idthread);
		string v = value->String();
		if (value != this)
			value->Release();
		return v;
	}

	virtual wstring Getustring(Atanor* context, Atanor* value, short idthread) {
		if (isAtom())
			return UString();

		value = Get(context, aNULL, idthread);
		wstring v = value->UString();
		if (value != this)
			value->Release();
		return v;
	}
	//----------------------------------------------------------------------

	virtual long getinteger(int i) {
		Atanor* a = getvalue(i);
		long v = a->Integer();
		a->Release();
		return v;
	}

	virtual BLONG getlong(int i) {
		Atanor* a = getvalue(i);
		BLONG v = a->Long();
		a->Release();
		return v;
	}

	virtual short getshort(int i) {
		Atanor* a = getvalue(i);
		short v = a->Short();
		a->Release();
		return v;
	}

	virtual float getdecimal(int i) {
		Atanor* a = getvalue(i);
		float v = a->Decimal();
		a->Release();
		return v;
	}

	virtual double getfloat(int i) {
		Atanor* a = getvalue(i);
		double v = a->Float();
		a->Release();
		return v;
	}

	virtual string getstring(int i) {
		Atanor* a = getvalue(i);
		string v = a->String();
		a->Release();
		return v;
	}

	virtual wstring getustring(int i) {
		Atanor* a = getvalue(i);
		wstring v = a->UString();
		a->Release();
		return v;
	}

	//--------------------------------------------------

	//--- Some important conversion methods...
	virtual string String() { return ""; }

	virtual string Bytes() {
		return String();
	}

	virtual wstring UString() {
		string s = String();
		wstring res;
		s_utf8_to_unicode(res, STR(s));
		return res;
	}

	virtual string StringToDisplay() {
		return String();
	}

	virtual string JSonString() {
		return String();
	}

	virtual long Integer() { return 0; }
	virtual short Short() { return (short)Integer(); }
	virtual double Float() { return 0; }
	
	virtual float Decimal() {
		return (float)Float();
	}

	virtual bool Boolean() { return false; }
	virtual bool Protectedboolean() { 
		return Boolean();
	}

	virtual BLONG Long() { return 0; }
	virtual unsigned char Byte() {
		return (unsigned char)Integer();
	}

	virtual void Clear() {}
	virtual void Clean() {}
	virtual void Cleaning(short idthread) {}

	virtual long Size() { return 0; }

	//Basic operations: we add the current value with "a"
	virtual Atanor* andset(Atanor* a, bool itself) {
		return this;
	}

	virtual Atanor* orset(Atanor* a, bool itself) {
		return this;
	}

	virtual Atanor* xorset(Atanor* a, bool itself) {
		return this;
	}

	virtual Atanor* in(Atanor* context, Atanor* a, short idthread) {
		return aNULL;
	}

	Exporting virtual Atanorfraction* Fraction();
	Exporting virtual Atanor* Invert(bool autoself);

	virtual Atanor* Thesum(long i, long j) {
		return aZERO;
	}

	virtual Atanor* Theproduct(long i, long j) {
		return aZERO;
	}

	virtual double Sum() {
		return 0;
	}

	virtual double Product() {
		return 1;
	}

	virtual Atanor* plusplus() {
		return this;
	}

	virtual Atanor* minusminus() {
		return this;
	}

	virtual Atanor* plus(Atanor* a, bool itself) {
		return this;
	}
	virtual Atanor* minus(Atanor* a, bool itself) {
		return this;
	}
	virtual Atanor* multiply(Atanor* a, bool itself) {
		return this;
	}
	virtual Atanor* divide(Atanor* a, bool itself) {
		return this;
	}
	virtual Atanor* power(Atanor* a, bool itself) {
		return this;
	}
	virtual Atanor* shiftleft(Atanor* a, bool itself) {
		return this;
	}
	virtual Atanor* shiftright(Atanor* a, bool itself) {
		return this;
	}
	virtual Atanor* mod(Atanor* a, bool itself) {
		return this;
	}

	virtual Atanor* less(Atanor* a) {
		return aFALSE;
	}

	virtual Atanor* more(Atanor* a) {
		return aFALSE;
	}

	virtual Atanor* same(Atanor* a) {
		if (a == this)
			return aTRUE;
		return aFALSE;
	}

	virtual Atanor* different(Atanor* a) {
		if (same(a) == aFALSE)
			return aTRUE;
		return aFALSE;
	}

	virtual Atanor* lessequal(Atanor* a) {
		return aFALSE;
	}

	virtual Atanor* moreequal(Atanor* a) {
		return aFALSE;
	}

	virtual Atanor* Compare(Atanor* context, Atanor* value, short idthread) {
		return same(value);
	}

	virtual bool Purehaskelldeclaration() {
		return false;
	}

	virtual short Returntype() {
		return a_none;
	}
};

//-------------------------------------------------------------------------
class AtanorError : public Atanor {
public:
	string error;

	AtanorError(AtanorGlobal* g, string e) : error(e) {
		g->RecordInTracker(this);
	}

	AtanorError(string e) : error(e) {
		globalAtanor->RecordInTrackerProtected(this);
	}

	short Type() {
		return a_error;
	}

	string String() {
		return error;
	}

	bool needInvestigate() {
		return true;
	}

	bool needFullInvestigate() {
		return true;
	}

	bool isError() {
		return true;
	}

	void Resetreference(short inc) {}
	void Release() {}

};
//-------------------------------------------------------------------------
//We need to declare the following other classes

//This class is used to handle objects, which can be either cleaned or deleted...
class AtanorObject : public Atanor {
public:
	ThreadLock* _locker;
	short reference;

	bool affectation;
	bool protect;


	AtanorObject(AtanorGlobal* g, Atanor* parent = NULL)  {
		_locker = NULL;
		affectation = false;
		protect = true;
		reference = 0;
		if (g != NULL)
			g->RecordInTrackerProtected(this);
		if (parent != NULL)
			parent->AddInstruction(this);
	}

	AtanorObject()  {
		_locker = NULL;
		affectation = false;
		protect = true;
		reference = 0;
	}

	~AtanorObject() {
		if (_locker != NULL)
			delete _locker;
	}

	bool isObject() {
		return true;
	}

	bool hasLock() {
		return true;
	}

	long Reference() {
		return reference;
	}

	virtual void Setprotect(bool n) {
		protect = n;
	}

	virtual void Popping() {
		protect = false;
		if (reference <= 0)
			protect = true;
	}

	bool isProtected() {
		return protect;
	}

	Exporting virtual void Setreference(short r = 1);

	void Resetreferencenoprotect(short r = 1) {
		protect = 0;
		Resetreference(r);
	}


	Exporting virtual void Resetreference(short r = 1);

	virtual void Release() {
		if (reference == 0) {
			protect = false;
			Resetreference(0);
		}
	}

	virtual void Protect() {
		//We suppose there have been a Setreference before...
		if (reference >= 2) { //No problem, we simply remove one increment
			Resetreference();
			Setprotect(false); //Should not be any protect left in that case...
			return;
		}

		//Else, we decrease our reference, but we protect it with a protect
		Setprotect(true);
		Resetreference();
		Popping(); //and protection only if necessary...
	}

	void Setaffectation(bool b) {
		affectation = b;
	}

	bool isAffectation() {
		return affectation;
	}

	long Lockid() {
		if (_locker == NULL)
			return -1;
		return _locker->id;
	}

	void Initlock() {
		if (_locker == NULL)
			_locker = new ThreadLock(NULL, false);
	}

};

//--------------------------------------------------------------------
// These classes are used to declared code, which can be tracked in AtanorGlobal
class AtanorTracked : public Atanor {
public:
	long line;
	short idfile;
	short idcode;
	short idtype;

	Exporting AtanorTracked(short t, AtanorGlobal* g = NULL, Atanor* parent = NULL);
	Exporting AtanorTracked(string t, AtanorGlobal* g = NULL, Atanor* parent = NULL);

	void Setcodereference(long l, short i) {
		line = l;
		idcode = i;
	}

	void Remove() {		
		globalAtanor->RemoveFromTracker(idtracker);
		delete this;
	}

	bool isTracked() {
		return true;
	}

	short Type() {
		return idtype;
	}

	long Currentline() {
		return line;
	}

	short Currentspace() {
		return idcode;
	}

	short Currentfile() {
		return idfile;
	}

	virtual short Typeinfered() {
		return a_none;
	}

	void Setinfo(Atanor* ins) {
		if (globalAtanor->debugmode) {
			idfile = ins->Currentfile();
			idcode = ins->Currentspace();
			line = ins->Currentline();
		}
	}

	void Getinfo(Atanor* ins) {
		idfile = ins->Currentfile();
		idcode = ins->Currentspace();
		line = ins->Currentline();
	}

};
//------------------------------------------------------------------------
class AtanorIteration : public AtanorObject {
private:
	Atanor* prevkey;
	Atanor* prevvalue;

public:
	bool reverse;

	AtanorIteration(bool d, AtanorGlobal* g = NULL) : reverse(d) {
		prevkey = prevvalue = NULL;
		if (globalAtanor != NULL)
			prevkey = prevvalue = aNULL;
	}

	void Update(Atanor* n = aNULL) {
		prevkey = prevvalue = n;		
	}

	short Type() {
		return a_iteration;
	}

	~AtanorIteration() {
		prevkey->Resetreference();
		prevvalue->Resetreference();
	}

	Atanor* IteratorKey() {
		Atanor* v = Key();
		v->Setreference();
		prevkey->Resetreference();
		prevkey = v;
		return v;
	}

	Atanor* IteratorValue() {
		Atanor* v = Value();
		prevvalue->Resetreference();

		if (v->isProtected()) {
			v->Setreference();
			prevvalue = v;
		}
		else
			prevvalue = aNULL;

		return v;
	}

	virtual Atanor* Key() {
		return aNULL;
	}

	virtual Atanor* Value() {
		return aNULL;
	}

	virtual string Keystring() {
		return "";
	}

	virtual string Valuestring() {
		return "";
	}

	virtual wstring Keyustring() {
		string s = Keystring();
		wstring res;
		s_utf8_to_unicode(res, STR(s));
		return res;
	}

	virtual wstring Valueustring() {
		string s = Valuestring();
		wstring res;
		s_utf8_to_unicode(res, STR(s));
		return res;
	}

	virtual long Keyinteger() {
		return 0;
	}

	virtual short Keyshort() {
		return Key()->Short();
	}

	virtual BLONG Keylong() {
		return IteratorKey()->Long();
	}

	virtual long Valueinteger() {
		return 0;
	}

	virtual short Valueshort() {
		return IteratorValue()->Short();
	}
	
	virtual BLONG Valuelong() {
		return IteratorValue()->Long();
	}

	virtual double Keyfloat() {
		return 0;
	}

	virtual double Valuefloat() {
		return 0;
	}

	virtual float Valuedecimal() {
		return IteratorValue()->Decimal();
	}

	virtual void Next() {}

	virtual Atanor* End() {
		return aTRUE;
	}

	virtual Atanor* Begin() {
		return aFALSE;
	}

};

class AtanorConstiteration : public AtanorIteration {
public:

	AtanorConstiteration(bool d, AtanorGlobal* g) : AtanorIteration(d, g) {
		g->RecordInTracker(this);
	}

	void Setreference(short r = 1) {}
	void Resetreference(short r = 1) {}
	void Release() {}
	void Protect() {}
};
//------------------------------------------------------------------------
//To record objects such as functions or frames...
class AtanorDeclaration : public AtanorTracked {
public:
	basebin_hash<Atanor*> declarations;
	short name;

	AtanorDeclaration(short t = a_declarations, AtanorGlobal* g = NULL, Atanor* parent = NULL) : name(-1), AtanorTracked(t, g, parent) {}
	AtanorDeclaration(short n, short t, AtanorGlobal* g = NULL, Atanor* parent = NULL) : name(n), AtanorTracked(t, g, parent) {}


	bool hasDeclaration() {
		return true;
	}

	virtual bool isDeclaration() {
		return true;
	}

	bool isEmpty() {
		if (declarations.base == -1)
			return true;
		return false;
	}

	short Name() {
		return name;
	}

	virtual bool isDeclared(short id) {
		if (declarations.check(id))
			return true;
		return false;
	}

	virtual void Declare(short id, Atanor* a) {
		declarations[id] = a;
	}

	virtual Atanor* Declaration(short id) {
		if (!declarations.check(id))
			return NULL;
		return declarations[id];
	}

	void Variables(vector<short>& vars) {
		basebin_hash<Atanor*>::iterator it;
		for (it = declarations.begin(); it != declarations.end(); it++) {
			if (it->second->isFunction() || it->second->isFrame() || it->second->isVariable())
				continue;
			vars.push_back(it->first);
		}
	}

	void Cleaning(short idthread);
	virtual void Erase(short id) {
		declarations.erase(id);
	}
};

//--------------------------------------------------------------------

//To store a sequence of instructions
class AtanorInstruction : public AtanorTracked {
public:
	VECTE<Atanor*> instructions;
	Atanor* sibling;
	short action;
	bool negation;
	bool variablesWillBeCreated;

	AtanorInstruction(short t = a_instructions, AtanorGlobal* g = NULL, Atanor* parent = NULL) :
		variablesWillBeCreated(false), negation(false), action(a_none), sibling(NULL), AtanorTracked(t, g, parent) {}

	AtanorInstruction(AtanorGlobal* g, Atanor* parent = NULL) :
		variablesWillBeCreated(false), negation(false), action(a_none), sibling(NULL), AtanorTracked(a_instructions, g, parent) {}


	virtual void AddInstruction(Atanor* a) {
		instructions.push_back(a);
	}

    void ScanVariables(vector<short>& vars) {
        for (long i=0; i<instructions.size();i++)
            instructions[i]->ScanVariables(vars);
        
    }
    
	void SetVariablesWillBeCreated() {
		//This variable is set to true when a variable creation is present in the instruction vector...
		variablesWillBeCreated = true;
	}

	Atanor* Parent() {
		return sibling;
	}

	bool isNegation() {
		return negation;
	}

	void Setnegation(bool n) {
		negation = n;
	}

	void InstructionClear() {
		instructions.clear();
	}

	void Putinstruction(size_t i, Atanor* ke) {
		if (i < instructions.size())
			instructions.at(i, ke);
		else
			instructions.push_back(ke);
	}

	void Setaction(short a) {
		action = a;
	}

	Atanor* InstructionRemoveLast() {
		if (instructions.size() == 0)
			return aNULL;
		Atanor* ret = instructions[instructions.size() - 1];
		instructions.pop_back();
		return ret;
	}

	void InstructionRemove(size_t nb) {
		if (nb > instructions.size())
			nb = instructions.size();
		while (nb > 0) {
			instructions.pop_back();
			nb--;
		}
	}

	bool isInstruction() {
		return true;
	}

	Atanor* Lastinstruction() {
		if (instructions.size() == 0)
			return aNULL;
		return instructions[instructions.size() - 1];
	}

	size_t InstructionSize() { return instructions.size(); }

	Atanor* Instruction(size_t i) { return instructions[i]; }

	short Action() {
		return action;
	}

	void Clone(AtanorInstruction* ki) {
		ki->instructions = instructions;
		ki->negation = negation;
		ki->sibling = sibling;
	}

	void Addparent(Atanor* a) {
		sibling = a;
	}

	virtual Atanor* Get(Atanor* context, Atanor* callfunction, short idthread);
};


class AtanorBeforeLast : public AtanorInstruction {
public:

	AtanorBeforeLast(AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_bloc, g, parent) {
		action = a_bloc;
	}

	void AddInstruction(Atanor* a) {
		if (instructions.last == 0)
			instructions.push_back(a);
		else
			instructions.insert(instructions.last - 1, a);
	}

};

class AtanorSequence : public AtanorInstruction {
public:
	basebin_hash<Atanor*> declarations;

	AtanorSequence(AtanorGlobal* g, Atanor* parent = NULL) : AtanorInstruction(a_sequence, g, parent) {
		action = a_bloc;
	}


	bool hasDeclaration() {
		return true;
	}

	void Variables(vector<short>& vars) {
		basebin_hash<Atanor*>::iterator it;
		for (it = declarations.begin(); it != declarations.end(); it++) {
			if (it->second->isFunction() || it->second->isFrame() || it->second->isVariable())
				continue;
			vars.push_back(it->first);
		}
	}

	bool isDeclared(short id) {
		if (declarations.check(id))
			return true;
		return false;
	}

	virtual void Declare(short id, Atanor* a) {
		declarations[id] = a;
	}

	Atanor* Declaration(short id) {
		if (!declarations.check(id))
			return NULL;
		return declarations[id];
	}

	void Cleaning(short idthread) {
		bin_hash<Atanor*>::iterator it;
		for (it = declarations.begin(); it != declarations.end(); it++)
			it->second->Resetreference(1);
	}

	bool isSequence() {
		return true;
	}
};

class AtanorHBloc : public AtanorSequence {
public:

	AtanorHBloc(AtanorGlobal* g, Atanor* parent = NULL) : AtanorSequence(g, parent) {
		action = a_bloc;
	}

	Atanor* Get(Atanor* context, Atanor* callfunction, short idthread);
};
//--------------------------------------------------------------------
//A User Function declaration.
class AtanorFunction : public AtanorDeclaration {
public:
	VECTE<Atanor*> instructions;
	vector<Atanor*> parameters;
	AtanorFunction* next;

	short returntype;

	char choice;
	bool autorun;
	bool privatefunction;
	bool strict;
	bool used;
	bool nonlimited;
	bool adding;

	AtanorFunction(short n, AtanorGlobal* global, Atanor* parent = NULL) : returntype(a_null), AtanorDeclaration(n, a_function, global) {
		adding = true;
		idtype = a_function;
		autorun = false;
		privatefunction = false;
		strict = false;
		used = false;
		nonlimited = false;
		choice = 0;
		next = NULL;
	}

	size_t InstructionSize() { 
		return instructions.last; 
	}

	bool isDeclaration() {
		return false;
	}

	short Returntype() {
		if (returntype == a_null)
			return a_none;
		return returntype;
	}

	short Typeinfered() {
		if (returntype == a_null)
			return a_none;
		return returntype;
	}

	bool isFrame() {
		return false;
	}

	virtual void AddInstruction(Atanor* a) {
		if (choice == 0)
			parameters.push_back(a);
		else
		if (adding)
			instructions.push_back(a);
	}

    void ScanVariables(vector<short>& vars) {
        for (long i=0; i<instructions.size();i++)
            instructions[i]->ScanVariables(vars);
        
    }

	virtual Atanor* Get(Atanor* context, Atanor* callfunction, short idthread);

	bool isPrivate() {
		return privatefunction;
	}

	bool isFunction() {
		return true;
	}

	bool Predeclared() {
		if (choice == 2)
			return true;
		return false;
	}

	bool isUsed() {
		return used;
	}

	void Addfunction(Atanor* a) {
		AtanorFunction* l = this;
		while (l->next != NULL)
			l = l->next;
		l->next = (AtanorFunction*)a;
	}

	bool Alreadydeclared() {
		if (choice == 1)
			return true;
		return false;
	}

	long Size() {
		return (long)parameters.size();
	}

	Atanor* Parameter(size_t i) {
		return parameters[i];
	}

	Atanor* Nextfunction() {
		return next;
	}

	void Setchoice(char v) {
		choice = v;
	}

	char Choice() {
		return choice;
	}
};

class AtanorThread : public AtanorFunction {
public:
	ThreadLock* _locker;
	bool exclusive;
	bool joined;

	AtanorThread(short n, AtanorGlobal* global, bool j, char p, Atanor* parent = NULL) : joined(j), AtanorFunction(n, global, parent) {
		exclusive = false;
		_locker = NULL;
		if (p == 1)
			_locker = new ThreadLock(NULL, false);
		else
		if (p == 2)
			exclusive = true;
	}

	bool isJoined() {
		return joined;
	}

	bool isThread() {
		return true;
	}

	bool isExclusive() {
		return exclusive;
	}

	Atanor* Get(Atanor* context, Atanor* callfunction, short idthread);
};

class AtanorProtectedFunction : public AtanorThread {
public:

	AtanorProtectedFunction(short n, AtanorGlobal* global, char protect, Atanor* parent = NULL) : AtanorThread(n, global, false, protect, parent) {}
};


class AtanorFunctionCall : public AtanorObject {
public:
	AtanorFunction* body;

	AtanorFunctionCall(AtanorGlobal* global = NULL) : AtanorObject(global) {
		body = NULL;
	}

	short Action() {
		return a_call;
	}

	Atanor* Get(Atanor* context, Atanor* callfunction, short idthread) {
		if (body == NULL)
			return globalAtanor->Returnerror("No function assigned", idthread);

		return body->Get(context, callfunction, idthread);
	}

	Atanor* Put(Atanor* context, Atanor* callfunction, short idthread) {
		if (callfunction->isFunction()) {
			body = (AtanorFunction*)callfunction->Body(idthread);
			return aTRUE;
		}

		return globalAtanor->Returnerror("Expecting a function", idthread);
	}

	bool duplicateForCall() {
		return false;
	}

	Atanor* Body(short idthread) {
		return body;
	}

	bool isFunction() {
		return true;
	}

	long Size() {
		if (body != NULL)
			return body->Size();
		return -1;
	}

	Atanor* Parameter(size_t i) {
		if (body == NULL)
			return aNOELEMENT;
		return body->parameters[i];
	}

};

//------------------------------------------------------------------------
//To record frame declarations...
class AtanorFrame : public AtanorDeclaration {
public:
    bin_hash<unsigned long> exported;
	basebin_hash<Atanor*> numbers;
	VECTE<Atanor*> instructions;
	vector<Atanor*> variables;
    AtanorFrame* topframe;
	Atanor* function;	
	bool privee;
	bool post;

	AtanorFrame(short n, bool p, AtanorGlobal* g = NULL, Atanor* parent = NULL) : topframe(NULL),
		post(false), function(NULL), privee(p), AtanorDeclaration(n, a_frame, g) {}

	long Size() {
		return instructions.size();
	}

	void Addfunction(Atanor* a) {
		function = a;
	}

	bool isDeclaration() {
		return false;
	}

    void ScanVariables(vector<short>& vars) {
        for (long i=0; i<instructions.size();i++)
            instructions[i]->ScanVariables(vars);
        
    }

    void Sharedeclaration(AtanorFrame* k, bool final) {
        basebin_hash<Atanor*>::iterator it;
        //The first time, we copy the functions...
        if (final) {
            for (it = declarations.begin(); it != declarations.end(); it++) {
                if (it->second->isFunction()) {
                    if (!k->declarations.check(it->first))
                        k->declarations[it->first] = it->second;
                }
            }
            return;
        }
        
		//Then the variables...
        for (it = declarations.begin(); it != declarations.end(); it++) {
            if (it->second->isFunction())
                continue;
            k->declarations[it->first] = it->second;
			k->variables.push_back(it->second);
        }
        k->topframe=this;
    }

	void Declare(short id, Atanor* a) {
		if (id >= a_short && id <= a_float) {
			if (numbers.empty()) {
				numbers[a_short] = a;
				numbers[a_int] = a;
				numbers[a_long] = a;
				numbers[a_decimal] = a;
				numbers[a_float] = a;
			}
			else
				numbers[id] = a;
		}

		declarations[id] = a;
		if (a->isVariable()) {
			variables.push_back(a);
			if (a->isFrame())
				post = true;
		}
	}
	
    AtanorFrame* Topframe() {
        return topframe;
    }
    
    bool isParent(AtanorFrame* fr) {
        AtanorFrame* p=topframe;
        while (p!=NULL) {
            if (fr==p)
                return true;
            p=p->topframe;
        }
        return false;
    }
    
	void AddInstruction(Atanor* a) {
		instructions.push_back(a);
	}

	Atanor* Frame() {
		return this;
	}

	virtual bool isFrame() {
		return true;
	}

	virtual bool isDeclared(short id) {
		if (id == a_this)
			return true;

		if (declarations.check(id))
			return true;
		return false;
	}

};

class AtanorMainFrame : public AtanorFrame {
public:

	ThreadLock _locker;

	AtanorMainFrame(short n, bool p, AtanorGlobal* g = NULL, Atanor* parent = NULL) : _locker(NULL, false), AtanorFrame(n, p, g, parent) {}

	Atanor* Declaration(short id);
	void Declare(short id, Atanor* a);
	bool isDeclared(short id);
	void cleaning() {
		basebin_hash<Atanor*>::iterator it;
		for (it = declarations.begin(); it != declarations.end(); it++)
			it->second->Resetreference();
	}

	bool isFrame() {
		return false;
	}
};

//------------------------------------------------------------------------
class AtanorArguments : public AtanorTracked {
public:
	vector<Atanor*> arguments;
	short name;

	AtanorArguments(short t, AtanorGlobal* global = NULL, Atanor* parent = NULL) : AtanorTracked(t, global, parent) {}
	AtanorArguments(short n, short t, AtanorGlobal* global = NULL, Atanor* parent = NULL) : name(n), AtanorTracked(t, global, parent) {}

	short Name() {
		return name;
	}

	void AddInstruction(Atanor* a) {
		arguments.push_back(a);
	}

	bool isCall() {
		return true;
	}

	bool isCallFunction() {
		return true;
	}

	long Size() {
		return (long)arguments.size();
	}

	Atanor* Argument(size_t i) {
		if (i >= arguments.size())
			return aNOELEMENT;
		return arguments[i];
	}
};

//The basic Call, with its arguments...
class AtanorCall : public AtanorTracked {
public:
	vector<Atanor*> arguments;
	Atanor* function;
	short name;
	bool addarg;
	bool negation;

	AtanorCall(short t, AtanorGlobal* global = NULL, Atanor* parent = NULL) : name(-1), function(NULL), addarg(true), negation(false), AtanorTracked(t, global, parent) {}
	AtanorCall(short n, short t, AtanorGlobal* global = NULL, Atanor* parent = NULL) : function(NULL), addarg(true), name(n), negation(false), AtanorTracked(t, global, parent) {}

	short Name() {
		return name;
	}

	bool isNegation() {
		return negation;
	}

	void Setnegation(bool n) {
		negation = n;
	}

	void CheckHaskellComposition() {
		Atanor* a;
		for (short i = 0; i < arguments.size(); i++) {
			a = arguments[i]->Composition();
			if (a != aNOELEMENT) {
				arguments[i]->Remove();
				arguments[i] = a;
			}
		}
	}

	virtual void AddInstruction(Atanor* a) {
		if (addarg)
			arguments.push_back(a);
		else
			function = a;
	}

	bool isCall() {
		return true;
	}

	bool isCallFunction() {
		return true;
	}

    bool Setstopindex() {
        if (function !=NULL)
            function->Setstopindex();
        return false;
    }

	Atanor* Getindex() {
		if (function != NULL)
			return function->Getindex();
		return NULL;
	}

	void Addargmode() {
		addarg = true;
	}

	void Addfunctionmode() {
		addarg = false;
	}

	long Size() {
		return (long)arguments.size();
	}

	Atanor* Argument(size_t i) {
		if (i >= arguments.size())
			return aNOELEMENT;
		return arguments[i];
	}

	virtual Atanor* Get(Atanor* context, Atanor* callfunction, short idthread) {
		return this;
	}

	virtual Atanor* Evaluate(long i, Atanor* context, short idthread) {
		return arguments[i]->Get(context, aNULL, idthread);
	}
};

class AtanorCallClean : public AtanorCall {
public:
	vector<Atanor*> cleaning;

	AtanorCallClean(short t, AtanorGlobal* global = NULL, Atanor* parent = NULL) : AtanorCall(t, global, parent) {}
	AtanorCallClean(short n, short t, AtanorGlobal* global = NULL, Atanor* parent = NULL) : AtanorCall(n, t, global, parent) {}

	//We use Evaluate to keep a track of the arguments computed out of our list
	Atanor* Evaluate(long i, Atanor* context, short idthread) {
		Atanor* keval = arguments[i]->Get(context, aNULL, idthread);
		cleaning.push_back(keval);
		return keval;
	}

	//This function will automatically clean all these arguments...
	void Releasing(Atanor* returnobject, short idthread) {
		for (size_t i = 0; i < cleaning.size(); i++) {
			if (cleaning[i] != returnobject)
				cleaning[i]->Release();
		}
		cleaning.clear();
	}

	void Cleaning(short idthread) {
		for (size_t i = 0; i < cleaning.size(); i++)
			cleaning[i]->Release();
		cleaning.clear();
	}

	short Returntype() {
		return Typeinfered();
	}

};

//------------------------------------------------------------------------
class AtanorIndex : public Atanor {
public:
	Atanor* left;
	Atanor* right;
	Atanor* function;
	bool interval, signleft, signright, stop;

	AtanorIndex(bool itv, AtanorGlobal* global = NULL, Atanor* parent = NULL) :
		interval(itv), left(aNOELEMENT),
		signleft(false), signright(false), stop(false),
		function(NULL), right(NULL) {

		if (parent != NULL)
			parent->AddInstruction(this);

		if (global != NULL)
			global->RecordInTracker(this);
	}


	AtanorIndex* Evaluate(short idthread) {
		if (left->isConst() && (right == NULL || right->isConst()))
			return this;
		return globalAtanor->Provideindex(this, idthread);
	}

	short Type() {
		return a_callindex;
	}

	short Typeinfered() {
		return a_none;
	}

	char Indexes(short idthread, long sz, long& ileft, long& iright) {
		Atanor* keyleft = left->Get(aNULL, aNULL, idthread);
		Atanor* keyright = NULL;
		if (interval == true)
			keyright = right->Get(aNULL, aNULL, idthread);

		ileft = keyleft->Integer();
		keyleft->Release();
		if (keyright != NULL) {
			iright = keyright->Integer();
			keyright->Release();
		}

		if (ileft < 0)
			ileft += sz;

		if (ileft < 0 || ileft >= sz)
			return 0;

		if (!interval)
			return 1;

		if (iright <= 0 || keyright == aNULL) {
			iright += sz;
			if (iright<ileft)
				return 0;
		}
		else {
			if (iright>sz)
				return 0;
		}
		return 2;
	}

	void AddInstruction(Atanor* a) {
		if (left == aNOELEMENT)
			left = a;
		else if (interval && right == NULL)
			right = a;
		else
		if (function == NULL)
			function = a;
		else
			function->AddInstruction(a);
	}

	Atanor* Get(Atanor* context, Atanor* value, short idthread);
	Atanor* Put(Atanor* context, Atanor* value, short idthread);

	Atanor* Function() {
		return function;
	}

    bool isIndex() {
        return true;
    }
    
	bool isInterval() {
		return interval;
	}

    bool isStop() {
        return stop;
    }
    
    Atanor* Getindex() {
        if (stop)
            return this;
        return function->Getindex();
    }
    
    bool Setstopindex() {
        if (function==NULL) {
            stop=true;
           return true;
        }
        
        stop=function->Setstopindex();
        return stop;
    }
    
	bool isCall() {
		return true;
	}

	string String() {
		return left->String();
	}

	wstring UString() {
		return left->UString();
	}

	virtual string Bytes() {
		return left->Bytes();
	}

	string StringToDisplay() {
		return left->StringToDisplay();
	}

	string JSonString() {
		return left->JSonString();
	}

	virtual long Getinteger(Atanor* context, Atanor* value, short idthread) {
		return left->Getinteger(context, value, idthread);
	}

	virtual float Getdecimal(Atanor* context, Atanor* value, short idthread) {
		return left->Getdecimal(context, value, idthread);
	}

	virtual short Getshort(Atanor* context, Atanor* value, short idthread) {
		return left->Getshort(context, value, idthread);
	}

	virtual double Getfloat(Atanor* context, Atanor* value, short idthread) {
		return left->Getfloat(context, value, idthread);
	}

	virtual BLONG Getlong(Atanor* context, Atanor* value, short idthread) {
		return left->Getlong(context, value, idthread);
	}

	virtual string Getstring(Atanor* context, Atanor* value, short idthread) {
		return left->Getstring(context, value, idthread);
	}

	virtual wstring Getustring(Atanor* context, Atanor* value, short idthread) {
		return left->Getustring(context, value, idthread);
	}


	virtual long Getinteger(short idthread) {
		return left->Getinteger(idthread);
	}

	virtual float Getdecimal(short idthread) {
		return left->Getdecimal(idthread);
	}

	virtual short Getshort(short idthread) {
		return left->Getshort(idthread);
	}

	virtual double Getfloat(short idthread) {
		return left->Getfloat(idthread);
	}

	virtual BLONG Getlong(short idthread) {
		return left->Getlong(idthread);
	}

	virtual string Getstring(short idthread) {
		return left->Getstring(idthread);
	}

	virtual wstring Getustring(short idthread) {
		return left->Getustring(idthread);
	}

	long Integer() {
		return left->Integer();
	}

	short Short() {
		return left->Short();
	}

	double Float() {
		return left->Float();
	}

	bool Boolean() {
		return left->Boolean();
	}

	BLONG Long() {
		return left->Long();
	}

	unsigned char Byte() {
		return left->Byte();
	}

};

class AtanorIndexbuff : public AtanorIndex {
public:
	long idx;
	bool used;

	AtanorIndexbuff(long i) : AtanorIndex(false) {
		used = false;
		idx = i;
	}


	void Set(AtanorIndex* idx, short idthread)  {
		interval = idx->interval;
		signleft = idx->signleft;
		signright = idx->signright;

		left = idx->left->Get(aNULL, aNULL, idthread);
		if (idx->right != NULL)
			right = idx->right->Get(aNULL, aNULL, idthread);
		else
			right = NULL;
	}


	Exporting void Rollback();
	void Resetreference(short inc = 1) {}

	long Getinteger(Atanor* context, Atanor* value, short idthread) {
		return left->Integer();
	}

	float Getdecimal(Atanor* context, Atanor* value, short idthread) {
		return left->Decimal();
	}

	short Getshort(Atanor* context, Atanor* value, short idthread) {
		return left->Short();
	}

	double Getfloat(Atanor* context, Atanor* value, short idthread) {
		return left->Float();
	}

	BLONG Getlong(Atanor* context, Atanor* value, short idthread) {
		return left->Long();
	}

	string Getstring(Atanor* context, Atanor* value, short idthread) {
		return left->String();
	}

	wstring Getustring(Atanor* context, Atanor* value, short idthread) {
		return left->UString();
	}

	long Getinteger(short idthread) {
		return left->Integer();
	}

	float Getdecimal(short idthread) {
		return left->Decimal();
	}

	short Getshort(short idthread) {
		return left->Short();
	}

	double Getfloat(short idthread) {
		return left->Float();
	}

	BLONG Getlong(short idthread) {
		return left->Long();
	}

	string Getstring(short idthread) {
		return left->String();
	}

	wstring Getustring(short idthread) {
		return left->UString();
	}
};
//------------------------------------------------------------------------
class Locking {
private:
	std::lock_guard<std::recursive_mutex>* g;

public:
	Locking(AtanorGlobal* global) {
		g = NULL;
		if (global->globalLOCK)
			g = new std::lock_guard<std::recursive_mutex>(*globalAtanor->_locker.lock);
	}

	Locking(AtanorObject* d) {
		g = NULL;
		if (globalAtanor->globalLOCK) {
			d->Initlock();
			g = new std::lock_guard<std::recursive_mutex>(*d->_locker->lock);
		}
	}

	Locking(ThreadLock& d) {
		g = NULL;
		if (globalAtanor->globalLOCK)
			g = new std::lock_guard<std::recursive_mutex>(*d.lock);
	}

	~Locking() {
		if (g != NULL)
			delete g;
	}
};

//We impose a global order in which locks are created to reduce deadlock risks...
class Doublelocking {
public:
	std::lock_guard<std::recursive_mutex>* gfirst;
	std::lock_guard<std::recursive_mutex>* glast;

	Doublelocking(AtanorObject* d, Atanor* o) {
		gfirst = NULL;
		glast = NULL;
		if (globalAtanor->globalLOCK) {
			d->Initlock();
			if (o->hasLock()) {
				AtanorObject* dd = (AtanorObject*)o;
				dd->Initlock();
				if (d->Lockid() < o->Lockid()) {
					gfirst = new std::lock_guard<std::recursive_mutex>(*d->_locker->lock);
					glast = new std::lock_guard<std::recursive_mutex>(*dd->_locker->lock);
				}
				else {
					gfirst = new std::lock_guard<std::recursive_mutex>(*dd->_locker->lock);
					glast = new std::lock_guard<std::recursive_mutex>(*d->_locker->lock);
				}
			}
			else
				gfirst = new std::lock_guard<std::recursive_mutex>(*d->_locker->lock);
		}
	}

	~Doublelocking() {
		if (gfirst != NULL)
			delete gfirst;
		if (glast != NULL)
			delete glast;
	}

};
//------------------------------------------------------------------------
//The difference between let and self, is that in a let the first value defines once for all the type of the variable...
//let i=0; i is then an integer, unless you use the operator :=
// i := "tutu"; becomes a string...
//In a self, the type does not depend on the first assignment.

class AtanorLet : public AtanorObject {
public:
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	Atanor* value;

	//---------------------------------------------------------------------------------------------------------------------

	AtanorLet(AtanorGlobal* g) : AtanorObject(g) {
		value = aNOELEMENT;
	}

	AtanorLet() {
		//Do not forget your variable initialisation
		value = aNOELEMENT;
	}

	bool isLetSelf() {
		return true;
	}

	void Setreference(short r = 1) {
		Locking _lock(this);
		value->Setreference(r);
		reference += r;
		protect = false;
	}

	void Setprotect(bool n) {
		{
			Locking _lock(this);
			value->Setprotect(n);
		}
		protect = n;
	}

	void Popping() {
		{
			Locking _lock(this);
			value->Popping();
		}

		protect = false;
		if (reference <= 0)
			protect = true;
	}

	void Release() {
		if (protect == true && reference == 0) {
			{
				Locking _lock(this);
				value->Setprotect(false);
				protect = false;
			}
			Resetreference(1);
		}
	}

	void Resetreference(short r = 1) {
		{
			Locking _lock(this);
			value->Resetreference(r);
		}
		AtanorObject::Resetreference(r);
	}

	//----------------------------------------------------------------------------------------------------------------------
	Atanor* Newvalue(Atanor* a, short idthread) {
		if (value != aNOELEMENT)
			return value->Newvalue(a, idthread);
		return a->Atom();
	}
	//----------------------------------------------------------------------------------------------------------------------
	virtual void storevalue(string u);
	virtual void storevalue(float u);
	virtual void storevalue(short u);
	virtual void storevalue(wstring u);
	virtual void storevalue(long u);
	virtual void storevalue(BLONG u);
	virtual void storevalue(double u);
	virtual void storevalue(unsigned char u);
	virtual void storevalue(wchar_t u);
	//----------------------------------------------------------------------------------------------------------------------
	virtual Atanor* Put(Atanor* idx, Atanor* v, short idthread) {
		Locking _lock(this);
		if (value == v)
			return aTRUE;

		if (value != aNOELEMENT) {
			if (value == v)
				return aTRUE;

			value->Put(idx, v, idthread);
			return aTRUE;
		}

		if (idx->isIndex())
			return globalAtanor->Returnerror("LET(001): Undefined assignment", idthread);

		if (v != aNOELEMENT && v->isConst())
			v = v->Atom();

		value = v;
		value->Setreference(reference);
		return aTRUE;
	}

	bool Setvalue(Atanor* index, Atanor* v, short idthread, bool strict = false) {
		Locking _lock(this);
		if (value == v)
			return true;

		if (index->isIndex()) {
			globalAtanor->Returnerror("LET(001): Undefined assignment", idthread);
			return false;
		}

		if (value != aNOELEMENT)
			value->Resetreference(reference);

		if (v != aNOELEMENT && v->isConst())
			v = v->Atom();

		value = v;
		value->Setreference(reference);
		return true;
	}

	Atanor* Putvalue(Atanor* v, short idthread) {
		Locking _lock(this);
		if (value == v)
			return value;

		if (value != aNOELEMENT)
			value->Resetreference(reference);

		if (v->isConst())
			v = v->Atom();

		value = v;
		value->Setreference(reference);
		return value;
	}

	void Forcedclean() {
		Locking _lock(this);
		value->Resetreference(reference);
		value = aNOELEMENT;
	}

	Atanor* Get(Atanor* context, Atanor* v, short idthread) {
		Locking _lock(this);
		return value->Get(context, v, idthread);
	}

	virtual short Typevariable() {
		return a_let;
	}

	virtual short Type() {
		if (value == aNOELEMENT)
			return a_let;
		return value->Type();
	}

	virtual short Typeinfered() {
		return a_let;
	}

	virtual string Typename() {
		return "let";
	}

	Atanor* Atom(bool forced = false) {
		return value->Atom(forced);
	}

	bool isFrame() {
		return value->isFrame();
	}
	
	Atanor* Frame() {
		return value->Frame();
	}

	//If it is too complex to duplicate an element (for instance when passed to a function)
	//then you can use return false...  Usually in that case, Atom should always be just: return this;
	bool duplicateForCall() {
		return value->duplicateForCall();
	}

	Atanor* Value() {
		return value;
	}

	bool isFunction() {
		return value->isFunction();
	}

	Atanor* Body(short idthread) {
		return value->Body(idthread);
	}

	//---------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below

	virtual Atanor* Newinstance(short, Atanor* f = NULL) {
		return new AtanorLet;
	}

	void Clear() {
		//To set a variable back to empty
		value->Clear();
	}


	string String() {
		Locking _lock(this);
		return value->String();
	}

	wstring UString() {
		Locking _lock(this);
		return value->UString();
	}

	long Integer() {
		Locking _lock(this);
		return value->Integer();
	}
	double Float() {
		Locking _lock(this);
		return value->Float();
	}
	BLONG Long() {
		Locking _lock(this);
		return value->Long();
	}

	bool Boolean() {
		Locking _lock(this);
		return value->Boolean();
	}

	string StringToDisplay() {
		Locking _lock(this);
		return value->StringToDisplay();
	}

	string JSonString() {
		Locking _lock(this);
		return value->JSonString();
	}

	string Bytes() {
		Locking _lock(this);
		return value->Bytes();
	}


	//Basic operations
	long Size() {
		Locking _lock(this);
		return value->Size();
	}

	Atanor* Merging(Atanor* a) {
		Locking _lock(this);
		if (value == aNOELEMENT)
			return new AtanorError("Uninitialized 'self' variable");

		return value->Merging(a);
	}

	Atanor* andset(Atanor* a, bool itself) {
		Locking _lock(this);
		if (value == aNOELEMENT)
			return new AtanorError("Uninitialized 'self' variable");

		return value->andset(a, itself);
	}

	Atanor* orset(Atanor* a, bool itself) {
		Locking _lock(this);
		if (value == aNOELEMENT)
			return new AtanorError("Uninitialized 'self' variable");

		return value->orset(a, itself);
	}

	Atanor* xorset(Atanor* a, bool itself) {
		Locking _lock(this);
		if (value == aNOELEMENT)
			return new AtanorError("Uninitialized 'self' variable");
		return value->xorset(a, itself);
	}

	Atanor* plus(Atanor* a, bool itself) {
		Locking _lock(this);
		if (value == aNOELEMENT)
			return new AtanorError("Uninitialized 'self' variable");
		return value->plus(a, itself);
	}

	Atanor* minus(Atanor* a, bool itself) {
		Locking _lock(this);
		if (value == aNOELEMENT)
			return new AtanorError("Uninitialized 'self' variable");
		return value->minus(a, itself);
	}

	Atanor* multiply(Atanor* a, bool itself) {
		Locking _lock(this);
		if (value == aNOELEMENT)
			return new AtanorError("Uninitialized 'self' variable");
		return value->multiply(a, itself);
	}

	Atanor* divide(Atanor* a, bool itself) {
		Locking _lock(this);
		if (value == aNOELEMENT)
			return new AtanorError("Uninitialized 'self' variable");
		return value->divide(a, itself);
	}
	Atanor* power(Atanor* a, bool itself) {
		Locking _lock(this);
		if (value == aNOELEMENT)
			return new AtanorError("Uninitialized 'self' variable");
		return value->power(a, itself);
	}
	Atanor* shiftleft(Atanor* a, bool itself) {
		Locking _lock(this);
		if (value == aNOELEMENT)
			return new AtanorError("Uninitialized 'self' variable");
		return value->shiftleft(a, itself);
	}
	Atanor* shiftright(Atanor* a, bool itself) {
		Locking _lock(this);
		if (value == aNOELEMENT)
			return new AtanorError("Uninitialized 'self' variable");
		return value->shiftright(a, itself);
	}
	Atanor* mod(Atanor* a, bool itself) {
		Locking _lock(this);
		if (value == aNOELEMENT)
			return new AtanorError("Uninitialized 'self' variable");
		return value->mod(a, itself);
	}

	Atanor* less(Atanor* a) {
		Locking _lock(this);
		return value->less(a);
	}

	Atanor* more(Atanor* a) {
		Locking _lock(this);
		return value->more(a);
	}

	Atanor* same(Atanor* a) {
		Locking _lock(this);
		return value->same(a);
	}

	Atanor* different(Atanor* a) {
		Locking _lock(this);
		return value->different(a);
	}

	Atanor* lessequal(Atanor* a) {
		Locking _lock(this);
		return value->lessequal(a);
	}

	Atanor* moreequal(Atanor* a) {
		Locking _lock(this);
		return value->moreequal(a);
	}

};

//In a 'self' we clear the value before setting it back...
class AtanorSelf : public AtanorLet {
public:
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	//---------------------------------------------------------------------------------------------------------------------

	AtanorSelf(AtanorGlobal* g) : AtanorLet(g) {}

	AtanorSelf() {}

	//------------------------------------------------------------------------
	//--------------------------------------------------------------------
	void storevalue(string u);
	void storevalue(float u);
	void storevalue(short u);
	void storevalue(wstring u);
	void storevalue(long u);
	void storevalue(BLONG u);
	void storevalue(double u);
	void storevalue(unsigned char u);
	void storevalue(wchar_t u);
	//----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* idx, Atanor* v, short idthread) {
		Locking _lock(this);
	
		if (value == v)
			return aTRUE;

		if (value != aNOELEMENT) {
			if (idx->isIndex())
				return value->Put(idx, v, idthread);

			if (value->Type() == v->Type() && v->duplicateForCall()) {
				value->Put(idx, v, idthread);
				return aTRUE;
			}

			value->Resetreference(reference);
		}

		if (idx->isIndex())
			return globalAtanor->Returnerror("LET(001): Undefined assignment", idthread);

		if (v != aNOELEMENT && v->isConst())
			v = v->Atom();

		value = v;
		value->Setreference(reference);
		return aTRUE;
	}

	Atanor* Newinstance(short, Atanor* f = NULL) {
		return new AtanorSelf;
	}

	short Typevariable() {
		return a_self;
	}

	short Type() {
		if (value == aNOELEMENT)
			return a_self;
		return value->Type();
	}

	short Typeinfered() {
		return a_self;
	}

	string Typename() {
		return "self";
	}

};

#endif



