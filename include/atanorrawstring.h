
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
filename   : atanorrawstring.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanorrawstring_h
#define atanorrawstring_h

#include "atanorint.h"
#include "atanorstring.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorrawstring;
//This typedef defines a type "rawstringMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorrawstring::*rawstringMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanorrawstring : public AtanorObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging hmap<unsigned short, rawstringMethod> methods;
	static Exchanging hmap<string, string> infomethods;
	static Exchanging bin_hash<unsigned long> exported;

	static Exchanging short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	unsigned char* value;
	long buffersize;
	long stringsize;


	//---------------------------------------------------------------------------------------------------------------------
	Atanorrawstring(AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
		//Do not forget your variable initialisation
		value = NULL;
		buffersize = 0;
		stringsize = 0;
	}

	Atanorrawstring() {
		//Do not forget your variable initialisation
		value = NULL;
		buffersize = 0;
		stringsize = 0;

	}

	Atanorrawstring(string v) {
		//Do not forget your variable initialisation
		value = NULL;
		buffersize = 0;
		stringsize = v.size();
		if (stringsize != 0) {
			buffersize = stringsize + 1;
			value = new uchar[buffersize];
			strcpy_s((char*)value, buffersize, STR(v));
		}
	}

	~Atanorrawstring() {
		if (value != NULL)
			delete[] value;
	}
	//----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* index, Atanor* v, short idthread);

	Atanor* Putvalue(Atanor* v, short idthread) {
		string s = v->String();

		if (value == NULL) {
			stringsize = s.size();
			buffersize = stringsize + 1;
			value = new uchar[buffersize];
			strcpy_s((char*)value, buffersize, STR(s));
			return this;
		}

		if (buffersize <= s.size())
			return globalAtanor->Returnerror("Stack overflow", idthread);

		strcpy((char*)value, STR(s));
		stringsize = s.size();
		return this;
	}

	Atanor* Get(Atanor* context, Atanor* value, short idthread);


	Atanor* Vector(short idthread) {		
		if (value==NULL)
			return globalAtanor->Returnerror("Unknown expression", idthread);
		string v((char*)value);
		return globalAtanor->EvaluateVector(v, idthread);
	}

	Atanor* Map(short idthread) {		
		if (value == NULL)
			return globalAtanor->Returnerror("Unknown expression", idthread);
		string v((char*)value);
		return globalAtanor->EvaluateMap(v, idthread);
	}

	Atanor* Push(Atanor* a) {
		if (value == NULL)
			return globalAtanor->Returnerror("String overflow", globalAtanor->GetThreadid());

		string s = a->String();
		if ((stringsize + s.size()) >= buffersize)
			return globalAtanor->Returnerror("String overflow", globalAtanor->GetThreadid());

		for (long i = 0; i < s.size(); i++)
			value[stringsize + i] = s[i];
		stringsize += s.size();
		return this;
	}

	void Pop(long i) {
		if (value == NULL || !stringsize)
			return;

		if (i == -1) {
			i = value[stringsize - 1] = 0;
			stringsize--;
			return;
		}

		if (i >= stringsize || i < 0)
			return;

		stringsize--;
		strcpy_s((char*)value + i, i, (char*)value + i + 1);
	}

	virtual short Type() {
		return Atanorrawstring::idtype;
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
		if (forced || !protect || reference) {
			Atanorrawstring* v = new Atanorrawstring;
			if (value != NULL) {
				v->value = new unsigned char[buffersize];
				v->buffersize = buffersize;
				v->stringsize = stringsize;
				strcpy((char*)v->value, (char*)value);
			}
			return v;
		}

		return this;
	}

	void storevalue(string s) {
		if (value == NULL) {
			globalAtanor->Returnerror("String overflow", globalAtanor->GetThreadid());
			return;
		}

		if ((stringsize + s.size()) >= buffersize) {
			globalAtanor->Returnerror("String overflow", globalAtanor->GetThreadid());
			return;
		}

		for (long i = 0; i < s.size(); i++)
			value[stringsize + i] = s[i];
		stringsize += s.size();
	}

	void storevalue(wstring s) {
		string v;
		s_unicode_to_utf8(v, s);
		storevalue(v);
	}

	void storevalue(wchar_t u) {
		wstring w;
		w = u;
		string v;
		s_unicode_to_utf8(v, w);
		storevalue(v);
	}

	void Forcedclean() {
		//we cannot reset or resize a rawstring in a thread, since they are not protected with locks.
		if (globalAtanor->globalLOCK) {
			globalAtanor->Returnerror("Cannot reset a 'rawstring' with threads on", globalAtanor->GetThreadid());
			return;
		}

		if (value != NULL) {
			delete[] value;
			value = NULL;
			stringsize = 0;
			buffersize = 0;
		}
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
		return new Atanorrawstring;
	}

	Atanor* Newvalue(Atanor* a, short idthread) {
		return new Atanorrawstring(a->String());
	}


	AtanorIteration* Newiteration(bool direction);

	static void AddMethod(AtanorGlobal* g, string name, rawstringMethod func, unsigned long arity, string infos);
	
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
	Atanor* MethodInitial(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		Atanor* av = callfunc->Evaluate(0, contextualpattern, idthread);

		if (av->isNumber()) {
			stringsize = av->Integer();
			buffersize = stringsize + 1;
			value = new uchar[buffersize];
			value[0] = 0;
			return aTRUE;
		}


		string v = av->String();
		stringsize = v.size();
		buffersize = stringsize + 1;
		value = new uchar[buffersize];
		strcpy_s((char*)value, buffersize, STR(v));
		return aTRUE;
	}

	Atanor* MethodSizeb(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		return globalAtanor->Provideint(buffersize);
	}

	Atanor* MethodResize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		if (globalAtanor->globalLOCK)
			return globalAtanor->Returnerror("Cannot reset a 'rawstring' with threads on", idthread);

		long bsz = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
		if (!bsz || bsz <= stringsize)
			return aFALSE;

		buffersize = bsz;
		uchar* v = new uchar[buffersize];
		strcpy_s((char*)v, buffersize, (char*)value);
		delete[] value;
		value = v;
		return aTRUE;
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
	Atanor* MethodRegexip(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodEvaluate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodTohtml(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodToxml(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodReplaceregexip(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
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
	unsigned int EditDistance(Atanor* e);
	//---------------------------------------------------------------------------------------------------------------------
	void Clear() {
		if (value != NULL) {
			delete[] value;
			value = NULL;
			stringsize = 0;
			buffersize = 0;
		}
	}


	string JSonString() {
		string res;
		string v;
		if (value != NULL)
			v = (char*)value;

		jstringing(res, v);
		return res;
	}

	string String() {
		if (value == NULL)
			return "";
		return (char*)value;
	}

	wstring UString() {
		wstring res;
		if (value == NULL)
			return res;

		s_utf8_to_unicode(res, (char*)value);
		return res;

	}

	long Integer() {
		if (value == NULL)
			return 0;
		return (long)conversionintegerhexa((char*)value);
	}

	virtual long Size() {
		return stringsize;
	}

	float Decimal() {
		if (value == NULL)
			return 0;
		return convertfloat((char*)value);
	}

	double Float() {
		if (value == NULL)
			return 0;
		return convertfloat((char*)value);
	}

	bool Boolean() {
		if (stringsize==0)
			return false;
		return true;
	}

	BLONG Long() {
		if (value == NULL)
			return 0;
		return conversionintegerhexa((char*)value);
	}

	Atanor* in(Atanor* context, Atanor* a, short idthread);

	//we add the current value with a
	Atanor* plus(Atanor* b, bool itself) {
		if (value == NULL)
			return aNULL;

		if (itself) {
			string add = b->String();
			long sz = add.size();
			if ((stringsize + sz) >= buffersize)
				return globalAtanor->Returnerror("String overflow", globalAtanor->GetThreadid());

			if (sz == 1)
				value[stringsize] = add[0];
			else {
				for (int i = 0; i < sz; i++)
					value[stringsize + i] = add[i];
			}
			stringsize += sz;
			return this;
		}

		string add = String() + b->String();
		return globalAtanor->Providestring(add);
	}


	//we remove a from the current value
	virtual Atanor* minus(Atanor* b, bool itself) {
		if (value == NULL)
			return aNULL;

		string add = b->String();
		int nb = add.size();

		if (itself) {
			if (nb > stringsize)
				return this;

			char* pt = strstr((char*)value, STR(add));
			if (pt == NULL)
				return this;
			strcpy(pt, pt + nb);
			stringsize -= nb;
			return this;
		}

		string v;
		if (value != NULL)
			v = (char*)value;

		if (nb > stringsize)
			return globalAtanor->Providestring(v);

		int pos = v.rfind(add);
		if (pos == string::npos)
			return globalAtanor->Providestring(v);
		add = v.substr(0, pos);
		pos += nb;
		add += v.substr(pos, v.size() - pos);

		return globalAtanor->Providestring(add);
	}


	Atanor* less(Atanor* a) {
		if (value == NULL)
			return aTRUE;

		string v = a->String();
		if (v < a->String())
			return aTRUE;
		return aFALSE;
	}

	Atanor* more(Atanor* a) {
		if (value == NULL)
			return aFALSE;
		string v = a->String();
		if (v > a->String())
			return aTRUE;
		return aFALSE;
	}

	Atanor* same(Atanor* a) {
		if (value == NULL) {
			if (a->Size() == 0)
				return aTRUE;
			return aFALSE;
		}

		string v = a->String();
		if (v == a->String())
			return aTRUE;
		return aFALSE;
	}

	Atanor* different(Atanor* a) {
		if (value == NULL) {
			if (a->Size() != 0)
				return aTRUE;
			return aFALSE;
		}
		string v = a->String();
		if (v != a->String())
			return aTRUE;
		return aFALSE;
	}

	Atanor* lessequal(Atanor* a) {
		if (value == NULL) {
			if (a->Size() == 0)
				return aTRUE;
			return aFALSE;
		}
		string v = a->String();
		if (v <= a->String())
			return aTRUE;
		return aFALSE;
	}

	Atanor* moreequal(Atanor* a) {
		if (value == NULL) {
			if (a->Size() == 0)
				return aTRUE;
			return aFALSE;
		}
		string v = a->String();
		if (v >= a->String())
			return aTRUE;
		return aFALSE;
	}
};

class AtanorIterationrawstring : public AtanorIteration {
public:

	agnostring ref;

	AtanorIterationrawstring(string v, bool d, AtanorGlobal* g = NULL) : ref(v), AtanorIteration(d, g) {
		if (d)
			ref = ref.revert();
	}

	Atanor* Key() {
		return globalAtanor->Provideint((long)ref.getcharpos());
	}

	Atanor* Value() {
		string s = ref.current();
		return new Atanorrawstring(s);
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

#endif









