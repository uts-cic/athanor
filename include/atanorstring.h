
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
filename   : atanorstring.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanorstring_h
#define atanorstring_h

#include "atanorint.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorstring;
//This typedef defines a type "stringMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorstring::*stringMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanorstring : public AtanorObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging hmap<unsigned short, stringMethod> methods;
	static Exchanging hmap<string, string> infomethods;
	static Exchanging bin_hash<unsigned long> exported;

	static Exchanging short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	string value;

	//---------------------------------------------------------------------------------------------------------------------
	Atanorstring(string v, AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
		//Do not forget your variable initialisation
		value = v;
	}

	Atanorstring(string v) {
		//Do not forget your variable initialisation
		value = v;
	}

	//----------------------------------------------------------------------------------------------------------------------
	Exporting Atanor* Put(Atanor* index, Atanor* v, short idthread);

	Atanor* Putvalue(Atanor* v, short idthread) {
		value = v->String();
		return this;
	}


	Exporting Atanor* Get(Atanor* context, Atanor* value, short idthread);

	Exporting Atanor* Loophaskell(Atanor* recipient, Atanor* context, Atanor* env, AtanorFunctionLambda* bd, short idthread);
	Exporting Atanor* Filter(short idthread, Atanor* env, AtanorFunctionLambda* bd, Atanor* var, Atanor* kcont, Atanor* accu, Atanor* init, bool direct);

	Atanor* Vector(short idthread) {
		Locking _lock(this);
		return globalAtanor->EvaluateVector(value, idthread);
	}

	Atanor* Map(short idthread) {
		Locking _lock(this);
		return globalAtanor->EvaluateMap(value, idthread);
	}

	Atanor* Push(Atanor* a) {
		value += a->String();
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
		c_char_index_remove(value, i);
	}

	virtual short Type() {
		return Atanorstring::idtype;
	}

	string Typename() {
		return "string";
	}

	bool isString() {
		return true;
	}

	bool isAtom() {
		return true;
	}

	bool isValueContainer() {
		return true;
	}

	Atanor* Atom(bool forced = false) {
		if (forced || !protect || reference)
			return globalAtanor->Providestring(value);
		return this;
	}

	void storevalue(float u) {
		stringstream s;
		s << u;
		value = s.str();
	}

	void storevalue(short u) {
		stringstream s;
		s << u;
		value = s.str();

	}

	void storevalue(long u) {
		stringstream s;
		s << u;
		value = s.str();

	}

	void storevalue(BLONG u) {
		stringstream s;
		s << u;
		value = s.str();

	}

	void storevalue(double u) {
		stringstream s;
		s << u;
		value = s.str();

	}


	void storevalue(string u) {
		value = u;
	}

	void storevalue(wstring u) {
		sc_unicode_to_utf8(value, u);
	}

	void storevalue(wchar_t u) {
		wstring w;
		w = u;
		sc_unicode_to_utf8(value, w);
	}

	void addstringto(string u) {
		value += u;
	}

	void addustringto(wstring u) {
		s_unicode_to_utf8(value, u);
	}

	void addstringto(wchar_t u) {
		wstring w;
		w = u;
		s_unicode_to_utf8(value, w);
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
		return globalAtanor->Providestring("");
	}

	Atanor* Newvalue(Atanor* a, short idthread) {
		return globalAtanor->Providestring(a->String());
	}


	Exporting AtanorIteration* Newiteration(bool direction);

	static void AddMethod(AtanorGlobal* g, string name, stringMethod func, unsigned long arity, string infos, short returntype);

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



	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	Atanor* MethodSizeb(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		return globalAtanor->Provideint((long)value.size());
	}

	Atanor* MethodHash(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodReverse(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodOrd(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodBytes(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
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
	Atanor* MethodGetstd(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodGeterr(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodSplit(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodSplite(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodTokenize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodStokenize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodCount(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodFind(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodRfind(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodRemovefirst(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodRemovelast(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodIsutf8(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodNgrams(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodUtf8(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodLatin(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodDos(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodDostoutf8(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodLeft(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodByteposition(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodCharposition(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
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
	Atanor* MethodIndent(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

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
		value = "";
	}

	Exporting unsigned int EditDistance(Atanor* e);

	void Clean() {
		//Your cleaning code
	}

	string JSonString() {
		Locking _lock(this);
		string res;
		jstringing(res, value);
		return res;
	}

	string Bytes() {
		Locking _lock(this);
		return value;
	}

	string String() {
		Locking _lock(this);
		return value;
	}

	wstring UString() {
		Locking _lock(this);
		wstring res;
		s_utf8_to_unicode(res, STR(value));
		return res;

	}

	long Integer() {
		Locking _lock(this);
		return (long)conversionintegerhexa(STR(value));
	}

	wstring Getustring(short idthread) {
		wstring res;
		Locking _lock(this);		
		s_utf8_to_unicode(res, STR(value));
		return res;
	}

	string Getstring(short idthread) {
		Locking _lock(this);
		return value;
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

	virtual long Size() {
		Locking _lock(this);
		return (long)value.size();
	}

	float Decimal() {
		Locking _lock(this);
		return convertfloat(STR(value));
	}

	double Float() {
		Locking _lock(this);
		return convertfloat(STR(value));
	}

	bool Boolean() {
		Locking _lock(this);
		if (value == "")
			return false;
		return true;
	}

	BLONG Long() {
		Locking _lock(this);
		return conversionintegerhexa(STR(value));
	}

	Exporting Atanor* in(Atanor* context, Atanor* a, short idthread);

	Atanor* Merging(Atanor* a) {
		Locking _lock(this);
		value += a->String();
		return this;
	}

	//we add the current value with a
	virtual Atanor* plus(Atanor* a, bool itself) {
		Locking _lock(this);
		if (itself) {
			value += a->String();
			return this;
		}

		return globalAtanor->Providestring(value + a->String());
	}

	//we remove a from the current value
	virtual Atanor* minus(Atanor* a, bool itself) {
		Locking _lock(this);
		string v = a->String();
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
		return globalAtanor->Providestring(v);
	}


	Atanor* less(Atanor* a) {
		Locking _lock(this);
		if (value < a->String())
			return aTRUE;
		return aFALSE;
	}

	Atanor* more(Atanor* a) {
		Locking _lock(this);
		if (value > a->String())
			return aTRUE;
		return aFALSE;
	}

	Atanor* same(Atanor* a) {
		Locking _lock(this);
		if (value == a->String())
			return aTRUE;
		return aFALSE;
	}

	Atanor* different(Atanor* a) {
		Locking _lock(this);
		if (value != a->String())
			return aTRUE;
		return aFALSE;
	}

	Atanor* lessequal(Atanor* a) {
		Locking _lock(this);
		if (value <= a->String())
			return aTRUE;
		return aFALSE;
	}

	Atanor* moreequal(Atanor* a) {
		Locking _lock(this);
		if (value >= a->String())
			return aTRUE;
		return aFALSE;
	}
};

class Atanorstringbuff : public Atanorstring {
public:
	long idx;
	bool used;

	Atanorstringbuff(long i) : Atanorstring("") {
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
					value = "";
					reference = 0;
					clr = true;
					protect = true;
				}
			}
		}

		if (clr) {
			Locking _lock(globalAtanor->stringlock);
			used = false;
			globalAtanor->sempties.push_back(idx);
		}
	}

};

class AtanorIterationstring : public AtanorIteration {
public:

	agnostring ref;

	AtanorIterationstring(string& v, bool d, AtanorGlobal* g = NULL) : ref(v), AtanorIteration(d, g) {
		if (d)
			ref = ref.revert();
	}

	Atanor* Key() {
		return globalAtanor->Provideint((long)ref.getcharpos());
	}

	Atanor* Value() {
		string s = ref.current();
		return globalAtanor->Providestring(s);
	}

	string Keystring() {
		stringstream s;
		s << ref.getcharpos();
		return s.str();
	}

	wstring Valueustring() {
		return ref.wcurrent();
	}

	string Valuestring() {
		string s = ref.current();
		return s;
	}

	long Keyinteger() {
		return (long)ref.getcharpos();
	}

	long Valueinteger() {
		string s = ref.current();
		return conversionintegerhexa(STR(s));
	}

	double Keyfloat() {
		return (double)ref.getcharpos();
	}

	double Valuefloat() {
		string s = ref.current();
		return convertfloat(STR(s));
	}

	void Next() {
		ref.following();
	}

	Atanor* End() {
		if (ref.end())
			return aTRUE;
		return aFALSE;
	}

	Atanor* Begin() {
		ref.begin();
		return aTRUE;
	}


};

class AtanorLoopString : public Atanorstring {
public:

	vector<string> interval;
	long position;
	Atanor* function;

	AtanorLoopString(AtanorGlobal* g) : Atanorstring("") {

		position = 0;
		function = NULL;
	}


	void Callfunction();

	void Addfunction(Atanor* f) {
		function = f;
	}

	short Type() {
		return a_sloop;
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
		AtanorLoopString* a = new AtanorLoopString(globalAtanor);
		a->function = f;
		return a;
	}

	Atanor* andset(Atanor* a, bool autoself) {
		string s = a->String();
		string u;
		long m = min(s.size(), value.size());
		for (long i = 0; i < m; i++) {
			if (s[i] == value[i])
				u += s[i];
		}
		if (autoself) {
			value = u;
			return this;
		}

		return globalAtanor->Providestring(u);
	}

	Atanor* xorset(Atanor* a, bool autoself) {
		string s = a->String();
		string u;
		long m = min(s.size(), value.size());
		for (long i = 0; i < m; i++) {
			if (s[i] != value[i])
				u += value[i];
		}
		if (autoself) {
			value = u;
			return this;
		}

		return globalAtanor->Providestring(u);
	}

	Atanor* plus(Atanor* b, bool autoself) {
		if (interval.size() == 0)
			return this;

		if (autoself) {
			position += b->Integer();

			position = abs(position) % interval.size();

			value = interval[position];
			return this;
		}

		return Atanorstring::plus(b, autoself);
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

		return Atanorstring::minus(b, autoself);
	}
};

#endif









