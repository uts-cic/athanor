
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
filename   : atanorustring.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanorustring_h
#define atanorustring_h


#include "atanorint.h"
//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorustring;
//This typedef defines a type "ustringMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorustring::*ustringMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanorustring : public AtanorObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging hmap<unsigned short, ustringMethod> methods;
	static Exchanging hmap<string, string> infomethods;
	static Exchanging bin_hash<unsigned long> exported;

	static Exchanging short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	wstring value;

	//---------------------------------------------------------------------------------------------------------------------
	Atanorustring(wstring v, AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
		//Do not forget your variable initialisation
		value = v;
	}

	Atanorustring(wstring v) {
		//Do not forget your variable initialisation
		value = v;
	}

	Atanorustring(string v) {
		//Do not forget your variable initialisation
		s_utf8_to_unicode(value, STR(v));
	}

	Atanorustring(wchar_t v) {
		//Do not forget your variable initialisation
		value = v;
	}


	//----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* index, Atanor* v, short idthread);

	Atanor* Putvalue(Atanor* v, short idthread) {
		value = v->UString();
		return this;
	}

	Atanor* Get(Atanor* context, Atanor* value, short idthread);

	Atanor* Loophaskell(Atanor* recipient, Atanor* context, Atanor* env, AtanorFunctionLambda* bd, short idthread);
	Atanor* Filter(short idthread, Atanor* env, AtanorFunctionLambda* bd, Atanor* var, Atanor* kcont, Atanor* accu, Atanor* init, bool direct);

	Atanor* Vector(short idthread) {		
		string s = String();
		return globalAtanor->EvaluateVector(s, idthread);
	}

	Atanor* Map(short idthread) {		
		string s = String();
		return globalAtanor->EvaluateMap(s, idthread);
	}

	short Type() {
		return Atanorustring::idtype;
	}

	string Typename() {
		return "ustring";
	}

	bool isString() {
		return true;
	}

	bool isValueContainer() {
		return true;
	}

	bool isAtom() {
		return true;
	}

	bool isNumber() {
		return false;
	}

	bool isBoolean() {
		return false;
	}

	bool isFrame() {
		return false;
	}

	Atanor* Atom(bool forced = false) {
		if (forced || !protect || reference)
			return globalAtanor->Provideustring(value);
		return this;
	}

	string Bytes() {
		Locking _lock(this);
		string buffer;
		s_unicode_to_utf8(buffer, value);
		return buffer;
	}

	void storevalue(float u) {
		std::wstringstream s;
		s << u;
		value = s.str();
	}

	void storevalue(short u) {
		std::wstringstream s;
		s << u;
		value = s.str();

	}

	void storevalue(long u) {
		std::wstringstream s;
		s << u;
		value = s.str();

	}

	void storevalue(BLONG u) {
		std::wstringstream s;
		s << u;
		value = s.str();

	}

	void storevalue(double u) {
		std::wstringstream s;
		s << u;
		value = s.str();

	}

	void storevalue(string u) {
		sc_utf8_to_unicode(value, STR(u));
	}

	void storevalue(wstring u) {
		value = u;
	}

	void storevalue(wchar_t u) {
		value = u;
	}

	void addustringto(wstring ws) {
		value += ws;
	}

	void addstringto(string s) {
		s_utf8_to_unicode(value, STR(s));
	}

	void addstringto(wchar_t s) {
		value += s;
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
		return globalAtanor->Provideustring(L"");
	}

	Atanor* Newvalue(Atanor* a, short idthread) {
		return globalAtanor->Provideustring(a->UString());
	}

	AtanorIteration* Newiteration(bool direction);

	static void AddMethod(AtanorGlobal* g, string name, ustringMethod func, unsigned long arity, string infos);
	
	static bool InitialisationModule(AtanorGlobal* g, string version);

	
	void Methods(Atanor* v) {

		for (auto& it : infomethods)
			v->storevalue(it.first);
	}
	string Info(string n) {
		if (infomethods.find(n) != infomethods.end())
			return infomethods[n];
		return "Unknown method";
	}

	Atanor* Succ() {
		if (value == L"")
			return globalAtanor->Provideustring(L"");

		wstring v = value;
		v[v.size() - 1] = v[v.size() - 1] + 1;
		return globalAtanor->Provideustring(v);
	}

	Atanor* Pred() {
		if (value == L"")
			return globalAtanor->Provideustring(L"");

		wstring v = value;
		wchar_t c = v[v.size() - 1];
		if (c <= 1)
			return globalAtanor->Provideustring(value);

		v[v.size() - 1] = c - 1;
		return globalAtanor->Provideustring(v);
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	Atanor* MethodSucc(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return Succ();
	}

	Atanor* MethodPred(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return Pred();
	}

	Atanor* MethodOrd(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodSizeb(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		return globalAtanor->Provideint((long)value.size());
	}
	Atanor* MethodHash(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodReverse(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodFormat(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodBase(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodFill(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodPadding(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodParse(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodPop(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodLisp(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodTags(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodGrammar(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodEvaluate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodTohtml(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodToxml(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodReplaceGrammar(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodEditdistance(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodReplace(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodSplit(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodSplite(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodTokenize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodStokenize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodCount(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodFind(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodRfind(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodRemovefirst(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodRemovelast(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodIsutf8(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return aTRUE;
	}
	Atanor* MethodNgrams(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodLatin(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodDos(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodDostoutf8(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodLeft(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodRight(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodMid(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodIsalpha(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodIsconsonant(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodIsvowel(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodIspunctuation(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodIsdigit(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodExtract(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodEmoji(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodIsemoji(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodIsupper(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodIslower(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodUpper(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodDeaccentuate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodLower(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodTrim(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodTrimleft(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodTrimright(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodLast(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodInsert(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodClear(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodJamo(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodIsJamo(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodIsHangul(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodNormalizeHangul(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodTransliteration(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

#ifdef ATANOR_REGEX
	Atanor* MethodReplaceRgx(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodRegex(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodSplitRegex(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
#endif
	//---------------------------------------------------------------------------------------------------------------------
	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
	//is returned by the Declaration method.
	Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		return (this->*methods[idname])(contextualpattern, idthread, callfunc);
	}
	//---------------------------------------------------------------------------------------------------------------------
	void Clear() {
		Locking _lock(this);
		value = L"";
	}
	unsigned int EditDistance(Atanor* e);
	
	void Clean() {
		//Your cleaning code
	}
	string JSonString() {
		Locking _lock(this);
		string res;
		string v = String();
		jstringing(res, v);
		return res;
	}
	string String() {
		string s;
		Locking _lock(this);
		s_unicode_to_utf8(s, value);
		return s;
	}
	wstring UString() {
		Locking _lock(this);
		return value;
	}

	wstring Getustring(short idthread) {
		Locking _lock(this);
		return value;
	}

	string Getstring(short idthread) {
		string s;
		Locking _lock(this);
		s_unicode_to_utf8(s, value);
		return s;
	}

	long Getinteger(short idthread) {
		return Integer();
	}

	double Getfloat(short idthread) {
		return Float();
	}

	float Getdecimal(short idthread) {
		return Decimal();
	}

	BLONG Getlong(short idthread) {
		return Long();
	}

	long Integer() {
		Locking _lock(this);
		if (value == L"")
			return 0;
		string v;
		sc_unicode_to_utf8(v, value);
		return conversionintegerhexa(STR(v));
	}
	float Decimal() {
		Locking _lock(this);
		if (value == L"")
			return 0;
		string v;
		sc_unicode_to_utf8(v, value);
		return convertfloat(STR(v));
	}
	double Float() {
		Locking _lock(this);
		if (value == L"")
			return 0;
		string v;
		sc_unicode_to_utf8(v, value);
		return convertfloat(STR(v));
	}
	BLONG Long() {
		Locking _lock(this);
		return Integer();
	}
	bool Boolean() {
		Locking _lock(this);
		if (value == L"")
			return false;
		return true;
	}
	unsigned char Byte() {		
		string buffer;
		s_unicode_to_utf8(buffer, value);
		return buffer[0];
	}
	Atanor* Push(Atanor* a) {
		value += a->UString();
		return this;
	}
	
	void Pop(long i) {
		Locking _lock(this);
		if (value.size() == 0)
			return;
		if (i == -1)
			i = value.size() - 1;
		else
		if (i >= value.size() || i < 0)
			return;
		value.erase(i);
	}
	//Basic operations
	virtual long Size() {
		Locking _lock(this);
		return (long)value.size();
	}
	Atanor* in(Atanor* context, Atanor* a, short idthread);
	
	Atanor* andset(Atanor* a, bool autoself);
	Atanor* xorset(Atanor* a, bool autoself);

	Atanor* Merging(Atanor* a) {
		Locking _lock(this);
		value += a->UString();
		return this;
	}

	virtual Atanor* plus(Atanor* a, bool autoself) {
		if (autoself) {
			Locking _lock(this);
			value += a->UString();
			return this;
		}
		Locking _lock(this);
		return globalAtanor->Provideustring(value + a->UString());
	}

	virtual Atanor* minus(Atanor* a, bool itself) {
		Locking _lock(this);
		wstring v = a->UString();
		size_t nb = v.size();
		size_t pos = value.find(v);
		if (pos == string::npos)
			return this;

		//we keep our string up to p
		v = value.substr(0, pos);
		//then we skip the nb characters matching the size of v
		pos += nb;
		//then we concatenate with the rest of the string
		v += value.substr(pos, value.size() - pos);
		if (itself) {
			value = v;
			return this;
		}
		return globalAtanor->Provideustring(v);
	}

	Atanor* less(Atanor* a) {
		wstring s = a->UString();
		Locking _lock(this);
		if (value<s)
			return aTRUE;
		return aFALSE;
	}

	Atanor* more(Atanor* a) {
		wstring s = a->UString();
		Locking _lock(this);
		if (value>s)
			return aTRUE;
		return aFALSE;
	}

	Atanor* same(Atanor* a) {
		wstring s = a->UString();
		Locking _lock(this);
		if (value == s)
			return aTRUE;
		return aFALSE;
	}

	Atanor* different(Atanor* a) {
		wstring s = a->UString();
		Locking _lock(this);
		if (value != s)
			return aTRUE;
		return aFALSE;
	}

	Atanor* lessequal(Atanor* a) {
		wstring s = a->UString();
		Locking _lock(this);
		if (value <= s)
			return aTRUE;
		return aFALSE;
	}

	Atanor* moreequal(Atanor* a) {
		wstring s = a->UString();
		Locking _lock(this);
		if (value >= s)
			return aTRUE;
		return aFALSE;
	}
};

class Atanorustringbuff : public Atanorustring {
public:
	long idx;
	bool used;

	Atanorustringbuff(long i) : Atanorustring(L"") {
		//Do not forget your variable initialisation
		idx = i;
		used = false;
	}

	void Setreference(short r) {
		Locking _lock(this);
		reference += r;
		protect = false;		
	}

	void Resetreference(short r) {
		bool clr = false;
		{
			Locking _lock(this);
			reference -= r;
			if (reference <= 0) {
				if (!protect) {
					value = L"";
					reference = 0;
					clr = true;
					protect = true;
				}
			}
		}

		if (clr) {
			Locking _lock(globalAtanor->ustringlock);
			used = false;
			globalAtanor->uempties.push_back(idx);
		}
	}

};

class AtanorIterationustring : public AtanorIteration {
public:

	BLONG itx;
	wstring ref;

	AtanorIterationustring(wstring& v, bool d, AtanorGlobal* g = NULL) : AtanorIteration(d, g) {
		ref = v;
		itx = 0;
	}

	Atanor* Key() {
		return globalAtanor->Provideint(itx);
	}

	Atanor* Value() {
		wstring w;
		w = ref[itx];
		return globalAtanor->Provideustring(w);
	}

	string Keystring() {
		stringstream s;
		s << itx;
		return s.str();
	}

	wstring Valueustring() {
		wstring w;
		w = ref[itx];
		return w;
	}

	string Valuestring() {
		string s;
		wstring w;
		w = ref[itx];
		s_unicode_to_utf8(s, w);
		return s;
	}

	long Keyinteger() {
		return itx;
	}

	long Valueinteger() {
		string v;
		wstring w;
		w = ref[itx];
		s_unicode_to_utf8(v, w);
		return conversionintegerhexa(STR(v));
	}

	double Keyfloat() {
		return itx;
	}

	double Valuefloat() {
		string v;
		wstring w;
		w = ref[itx];
		s_unicode_to_utf8(v, w);
		return convertfloat(STR(v));
	}

	void Next() {
		if (reverse)
			itx--;
		else
			itx++;
	}

	Atanor* End() {
		if (reverse) {
			if (itx < 0)
				return aTRUE;
		}
		else {
			if (itx >= ref.size())
				return aTRUE;
		}
		return aFALSE;
	}

	Atanor* Begin() {
		if (reverse) {
			itx = ref.size() - 1;
		}
		else
			itx = 0;
		return aTRUE;
	}


};

class AtanorLoopUString : public Atanorustring {
public:
	
	vector<wstring> interval;
	long position;
	Atanor* function;

	AtanorLoopUString(AtanorGlobal* g) : Atanorustring(L"") {
		
		position = 0;
		function = NULL;
	}


	void Callfunction();

	void Addfunction(Atanor* f) {
		function = f;
	}

	short Type() {
		return a_uloop;
	}

	Atanor* Put(Atanor*, Atanor*, short);
	Atanor* Vector(short idthread);

	long Size() {
		return interval.size();
	}

	void Next() {
		if (interval.size() == 0)
			return;

		position++;
		if (position >= interval.size()) {
			if (function != NULL)
				Callfunction();
			position = 0;
		}
		value = interval[position];
	}

	Atanor* plusplus() {
		if (interval.size() == 0)
			return this;
		position++;
		if (position >= interval.size()) {
			if (function != NULL)
				Callfunction();
			position = 0;
		}
		value = interval[position];
		return this;
	}

	Atanor* minusminus() {
		if (interval.size() == 0)
			return this;
		position--;
		if (position < 0) {
			if (function != NULL)
				Callfunction();
			position = interval.size() - 1;
		}
		value = interval[position];
		return this;
	}

	Atanor* Atom(bool forced) {
		return this;
	}

	bool duplicateForCall() {
		return false;
	}

	Atanor* Newinstance(short, Atanor* f = NULL) {
		AtanorLoopUString* a = new AtanorLoopUString(globalAtanor);
		a->function = f;
		return a;
	}

	Atanor* andset(Atanor* a, bool autoself);
	Atanor* xorset(Atanor* a, bool autoself);

	Atanor* plus(Atanor* b, bool autoself) {
		if (interval.size() == 0)
			return this;

		if (autoself) {
			position += b->Integer();

			position = abs(position) % interval.size();

			value = interval[position];
			return this;
		}

		return Atanorustring::plus(b, autoself);
	}

	Atanor* minus(Atanor* b, bool autoself) {
		if (interval.size() == 0)
			return this;

		if (autoself) {
			position -= b->Integer();

			position = (interval.size() - abs(position)) % interval.size();

			value = interval[position];
			return this;
		}

		return Atanorustring::minus(b, autoself);
	}
};

#endif









