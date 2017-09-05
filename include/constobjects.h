
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
filename   : constobjects.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef constobjects_h
#define constobjects_h

#include "atanorstring.h"
#include "atanorshort.h"
#include "atanorustring.h"
#include "atanorint.h"
#include "atanorfloat.h"
#include "atanordecimal.h"
#include "atanorlong.h"
#include "atanorbool.h"

//---------------------------------------------------------------------------------
class AtanorBaseConst : public Atanor {
public:
	short idtype;

	AtanorBaseConst(short ty, AtanorGlobal* g, Atanor* parent = NULL) : idtype(ty) {
		if (parent != NULL)
			parent->AddInstruction(this);

		if (g != NULL)
			g->RecordInTracker(this);
	}

	short Type() {
		return idtype;
	}

	short Typeinfered() {
		return idtype;
	}

	short Typevariable() {
		return idtype;
	}

	virtual short Action() {
		return a_const;
	}

	bool isAtom() {
		return true;
	}

	bool isConst() {
		return true;
	}

	virtual bool baseValue() {
		return true;
	}

};

//---------------------------------------------------------------------------------
class AtanorConst : public AtanorBaseConst {
public:
	short id;
	string name;
	int value;

	AtanorConst(short i, string n, AtanorGlobal* g) : id(i), name(n), value(0), AtanorBaseConst(a_const, g)  {}


	string String() {
		return "";
	}

	wstring UString() {
		return L"";
	}

	string JSonString() {
		string v;
		v = '"';
		v += name;
		v += '"';
		return(v);
	}

	string StringToDisplay() {
		return name;
	}

	bool baseValue() {
		return false;
	}

	long Getinteger(short idthread) {
		return value;
	}

	BLONG Getlong(short idthread) {
		return value;
	}

	short Getshort(short idthread) {
		return value;
	}

	float Getdecimal(short idthread) {
		return value;
	}

	double Getfloat(short idthread) {
		return value;
	}

	string Getstring(short idthread) {
		return "";
	}

	wstring Getustring(short idthread) {
		return L"";
	}

	long Integer() { return value; }
	double Float() { return value; }
	float Decimal() { return value; }
	BLONG Long() { return value; }
	short Short() { return value; }

	bool Boolean() {
		if (value == 0)
			return false;
		return true;
	}


	Atanor* Atom(bool forced = false) {
		if (this == aNOELEMENT)
			return aNULL;
		return this;
	}
};

class AtanorConstBreak : public AtanorConst {
public:

	AtanorConstBreak(short i, string n, AtanorGlobal* g) : AtanorConst(i, n, g) {}

	bool needInvestigate() {
		return true;
	}

	bool needFullInvestigate() {
		return true;
	}

	bool isError() {
		return true;
	}

};

class AtanorConstContinue : public AtanorConst {
public:

	AtanorConstContinue(short i, string n, AtanorGlobal* g) : AtanorConst(i, n, g) {}

	bool needFullInvestigate() {
		return true;
	}

	bool isError() {
		return true;
	}

};

class AtanorConstPipe : public AtanorConst {
public:

	AtanorConstPipe(short i, string n, AtanorGlobal* g) : AtanorConst(i, n, g) {}

	bool isError() {
		return true;
	}

	short Action() {
		return a_pipe;
	}

};
//---------------------------------------------------------------------------------

class AtanorConstString : public AtanorBaseConst {
public:
	string value;

	AtanorConstString(string v, AtanorGlobal* g = NULL, Atanor* parent = NULL) : value(v), AtanorBaseConst(a_string, g, parent) {}

	Exporting Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

	bool isString() {
		return true;
	}
	
	short Typeinfered() {
		return a_string;
	}

	Atanor* Newinstance(short, Atanor* f = NULL) {
		return globalAtanor->Providestring("");
	}

	Atanor* Newvalue(Atanor* a, short idthread) {
		return globalAtanor->Providestring(a->String());
	}

	wstring UString() {
		wstring res;
		s_utf8_to_unicode(res, STR(value));
		return res;
	}

	string String() { return value; }

	string Getstring(short idthread) {
		return value;
	}

	wstring Getustring(short idthread) {
		wstring res;
		s_utf8_to_unicode(res, STR(value));
		return res;
	}

	long Integer() {
		return (long)conversionintegerhexa(STR(value));
	}

	long Size() {
		return (long)value.size();
	}

	float Decimal() {
		return convertfloat(STR(value));
	}

	double Float() {
		return convertfloat(STR(value));
	}

	bool Boolean() {
		if (value == "")
			return false;
		return true;
	}

	BLONG Long() {
		return conversionintegerhexa(STR(value));
	}

	short Short() {
		return (short)conversionintegerhexa(STR(value));
	}

	Atanor* Atom(bool forced = false) {
		return globalAtanor->Providestring(value);
	}

	Atanor* Vector(short idthread) {
		return globalAtanor->EvaluateVector(value, idthread);
	}

	Atanor* Map(short idthread) {
		return globalAtanor->EvaluateMap(value, idthread);
	}

	//we add the current value with a
	Atanor* plus(Atanor* a, bool itself) {
		string s = value + a->String();
		return globalAtanor->Providestring(s);
	}

	//we remove a from the current value
	Atanor* minus(Atanor* a, bool itself) {
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
		return globalAtanor->Providestring(v);
	}


	Atanor* less(Atanor* a) {
		if (value < a->String())
			return aTRUE;
		return aFALSE;
	}

	Atanor* more(Atanor* a) {
		if (value > a->String())
			return aTRUE;
		return aFALSE;
	}

	Atanor* same(Atanor* a) {
		if (value == a->String())
			return aTRUE;
		return aFALSE;
	}

	Atanor* different(Atanor* a) {
		if (value != a->String())
			return aTRUE;
		return aFALSE;
	}

	Atanor* lessequal(Atanor* a) {
		if (value <= a->String())
			return aTRUE;
		return aFALSE;
	}

	Atanor* moreequal(Atanor* a) {
		if (value >= a->String())
			return aTRUE;
		return aFALSE;
	}

	Atanor* Succ() {
		Atanorstring v(value);
		return v.Succ();
	}

	Atanor* Pred() {
		Atanorstring v(value);
		return v.Pred();
	}

};

//--------------------------------------------------------------------
class AtanorConstUString : public AtanorBaseConst {
public:
	wstring value;

	AtanorConstUString(wstring v, AtanorGlobal* g = NULL, Atanor* parent = NULL) : value(v), AtanorBaseConst(a_ustring, g, parent) {}
	Exporting Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

	bool isString() {
		return true;
	}

	short Typeinfered() {
		return a_ustring;
	}


	Atanor* Newinstance(short, Atanor* f = NULL) {
		return globalAtanor->Provideustring(L"");
	}

	Atanor* Newvalue(Atanor* a, short idthread) {
		return globalAtanor->Provideustring(a->UString());
	}

	wstring UString() { 
		return value; 
	}

	string String() {
		string res;
		s_unicode_to_utf8(res, value);
		return res;
	}

	string Getstring(short idthread) {
		string res;
		s_unicode_to_utf8(res, value);
		return res;
	}

	wstring Getustring(short idthread) {
		return value;
	}

	long Integer() {
		return (long)conversionintegerhexa(STR(value));
	}

	long Size() {
		return (long)value.size();
	}

	float Decimal() {
		return convertfloat(STR(value));
	}

	double Float() {
		return convertfloat(STR(value));
	}

	bool Boolean() {
		if (value == L"")
			return false;
		return true;
	}

	BLONG Long() {
		return conversionintegerhexa(STR(value));
	}

	short Short() {
		return (short)conversionintegerhexa(STR(value));
	}

	Atanor* Atom(bool forced = false) {
		return globalAtanor->Provideustring(value);
	}

	Atanor* Vector(short idthread) {
		string res;
		s_unicode_to_utf8(res, value);
		return globalAtanor->EvaluateVector(res, idthread);
	}

	Atanor* Map(short idthread) {
		string res;
		s_unicode_to_utf8(res, value);
		return globalAtanor->EvaluateMap(res, idthread);
	}

	//we add the current value with a
	Atanor* plus(Atanor* a, bool itself) {
		wstring s = value + a->UString();
		return globalAtanor->Provideustring(s);
	}

	//we remove a from the current value
	Atanor* minus(Atanor* a, bool itself) {
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
		return globalAtanor->Provideustring(v);
	}


	Atanor* less(Atanor* a) {
		if (value < a->UString())
			return aTRUE;
		return aFALSE;
	}

	Atanor* more(Atanor* a) {
		if (value > a->UString())
			return aTRUE;
		return aFALSE;
	}

	Atanor* same(Atanor* a) {
		if (value == a->UString())
			return aTRUE;
		return aFALSE;
	}

	Atanor* different(Atanor* a) {
		if (value != a->UString())
			return aTRUE;
		return aFALSE;
	}

	Atanor* lessequal(Atanor* a) {
		if (value <= a->UString())
			return aTRUE;
		return aFALSE;
	}

	Atanor* moreequal(Atanor* a) {
		if (value >= a->UString())
			return aTRUE;
		return aFALSE;
	}

	Atanor* Succ() {
		Atanorustring v(value);
		return v.Succ();
	}

	Atanor* Pred() {
		Atanorustring v(value);
		return v.Pred();
	}

};
//--------------------------------------------------------------------

class AtanorConstInt : public AtanorBaseConst {
public:
	long value;

	AtanorConstInt(long v, AtanorGlobal* g = NULL, Atanor* parent = NULL) : value(v), AtanorBaseConst(a_int, g, parent) {}
	Exporting Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

	bool Checkprecision(Atanor* r) {
		if (idtype < r->Typenumber())
			return false;

		return true;
	}

	short Typeinfered() {
		return a_int;
	}

	Atanor* Newinstance(short, Atanor* f = NULL) {
		return globalAtanor->Provideint(0);
	}

	Atanor* Newvalue(Atanor* a, short idthread) {
		return globalAtanor->Provideint(a->Integer());
	}

	short Typenumber() {
		return a_int;
	}

	bool isNumber() {
		return true;
	}

	bool isInteger() {
		return true;
	}

	long Size() {
		return value;
	}

	wstring UString() {
		std::wstringstream v;
		v << value;
		return v.str();
	}

	string String() {
		stringstream v;
		v << value;
		return v.str();
	}

	long Getinteger(short idthread) {
		return value;
	}

	BLONG Getlong(short idthread) {
		return (BLONG)value;
	}

	short Getshort(short idthread) {
		return (short)value;
	}

	float Getdecimal(short idthread) {
		return (float)value;
	}

	double Getfloat(short idthread) {
		return (double)value;
	}

	string Getstring(short idthread) {
		stringstream v;
		v << value;
		return v.str();
	}

	wstring Getustring(short idthread) {
		std::wstringstream v;
		v << value;
		return v.str();
	}

	long Integer() {
		return value;
	}

	float Decimal() {
		return (float)value;
	}

	double Float() {
		return value;
	}

	bool Boolean() {
		if (value == 0)
			return false;
		return true;
	}

	BLONG Long() {
		return value;
	}

	short Short() {
		return value;
	}

	Atanor* Atom(bool forced = false) {
		return globalAtanor->Provideint(value);
	}

	//we add the current value with a
	Atanor* andset(Atanor* a, bool itself) {
		return globalAtanor->Provideint(value & a->Integer());
	}

	Atanor* orset(Atanor* a, bool itself) {
		return globalAtanor->Provideint(value | a->Integer());
	}

	Atanor* xorset(Atanor* a, bool itself) {
		return globalAtanor->Provideint(value ^ a->Integer());
	}


	Atanor* plus(Atanor* a, bool itself) {
		return globalAtanor->Provideint(value + a->Integer());
	}

	Atanor* minus(Atanor* a, bool itself) {
		return globalAtanor->Provideint(value - a->Integer());
	}

	Atanor* multiply(Atanor* a, bool itself) {
		return globalAtanor->Provideint(value * a->Integer());
	}

	Atanor* divide(Atanor* a, bool itself) {
		double v = a->Float();
		if (v == 0)
			return new AtanorError("Error: Divided by 0");
		v = (double)value / v;
		return globalAtanor->Providefloat(v);
	}

	Atanor* power(Atanor* a, bool itself) {
		double v = value;
		v = pow(v, a->Float());
		return globalAtanor->Providefloat(v);
	}

	Atanor* shiftleft(Atanor* a, bool itself) {
		return globalAtanor->Provideint(value << a->Integer());
	}

	Atanor* shiftright(Atanor* a, bool itself) {
		return globalAtanor->Provideint(value >> a->Integer());
	}

	Atanor* mod(Atanor* a, bool itself) {
		long v = a->Integer();
		if (v == 0)
			return new AtanorError("Error: Divided by 0");

		v = value % v;
		return globalAtanor->Provideint(v);
	}

	Atanor* less(Atanor* a) {
		if (value < a->Integer())
			return aTRUE;
		return aFALSE;
	}

	Atanor* more(Atanor* a) {
		if (value > a->Integer())
			return aTRUE;
		return aFALSE;
	}

	Atanor* same(Atanor* a) {
		if (value == a->Integer())
			return aTRUE;
		return aFALSE;
	}

	Atanor* different(Atanor* a) {
		if (value != a->Integer())
			return aTRUE;
		return aFALSE;
	}

	Atanor* lessequal(Atanor* a) {
		if (value <= a->Integer())
			return aTRUE;
		return aFALSE;
	}

	Atanor* moreequal(Atanor* a) {
		if (value >= a->Integer())
			return aTRUE;
		return aFALSE;
	}

	Atanor* Succ() {
		return globalAtanor->Provideint(value + 1);
	}

	Atanor* Pred() {
		return globalAtanor->Provideint(value - 1);
	}

};
//--------------------------------------------------------------------

class AtanorConstShort : public AtanorBaseConst {
public:
	short value;

	AtanorConstShort(short v, AtanorGlobal* g = NULL, Atanor* parent = NULL) : value(v), AtanorBaseConst(a_short, g, parent) {}
	Exporting Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

	Atanor* Newinstance(short, Atanor* f = NULL) {
		return new Atanorshort(0);
	}

	short Typeinfered() {
		return a_short;
	}

	Atanor* Newvalue(Atanor* a, short idthread) {
		return new Atanorshort(a->Short());
	}

	bool Checkprecision(Atanor* r) {
		if (r->Type() <= a_short)
			return true;

		return false;
	}


	bool isShort() {
		return true;
	}

	short Typenumber() {
		return a_short;
	}

	bool isNumber() {
		return true;
	}

	bool isInteger() {
		return true;
	}

	long Size() {
		return value;
	}

	string String() {
		stringstream v;
		v << value;
		return v.str();
	}

	wstring UString() {
		std::wstringstream v;
		v << value;
		return v.str();
	}

	long Getinteger(short idthread) {
		return (long)value;
	}

	BLONG Getlong(short idthread) {
		return (BLONG)value;
	}

	short Getshort(short idthread) {
		return value;
	}

	float Getdecimal(short idthread) {
		return (float)value;
	}

	double Getfloat(short idthread) {
		return (double)value;
	}

	string Getstring(short idthread) {
		stringstream v;
		v << value;
		return v.str();
	}

	wstring Getustring(short idthread) {
		std::wstringstream v;
		v << value;
		return v.str();
	}

	long Integer() {
		return value;
	}

	float Decimal() {
		return (float)value;
	}

	double Float() {
		return value;
	}

	bool Boolean() {
		if (value == 0)
			return false;
		return true;
	}

	BLONG Long() {
		return value;
	}

	short Short() {
		return value;
	}

	Atanor* Atom(bool forced = false) {
		return new Atanorshort(value);
	}

	//we add the current value with a
	Atanor* andset(Atanor* a, bool itself) {
		return new Atanorshort(value & a->Short());
	}

	Atanor* orset(Atanor* a, bool itself) {
		return new Atanorshort(value | a->Short());
	}

	Atanor* xorset(Atanor* a, bool itself) {
		return new Atanorshort(value ^ a->Short());
	}


	Atanor* plus(Atanor* a, bool itself) {
		return new Atanorshort(value + a->Short());
	}

	Atanor* minus(Atanor* a, bool itself) {
		return new Atanorshort(value - a->Short());
	}

	Atanor* multiply(Atanor* a, bool itself) {
		return new Atanorshort(value * a->Short());
	}

	Atanor* divide(Atanor* a, bool itself) {
		double v = a->Float();
		if (v == 0)
			return new AtanorError("Error: Divided by 0");
		v = (double)value / v;
		return globalAtanor->Providefloat(v);
	}

	Atanor* power(Atanor* a, bool itself) {
		double v = value;
		v = pow(v, a->Float());
		return globalAtanor->Providefloat(v);
	}

	Atanor* shiftleft(Atanor* a, bool itself) {
		return new Atanorshort(value << a->Short());
	}

	Atanor* shiftright(Atanor* a, bool itself) {
		return new Atanorshort(value >> a->Short());
	}

	Atanor* mod(Atanor* a, bool itself) {
		short v = a->Short();
		if (v == 0)
			return new AtanorError("Error: Divided by 0");

		v = value % v;
		return new Atanorshort(v);
	}

	Atanor* less(Atanor* a) {
		if (value < a->Short())
			return aTRUE;
		return aFALSE;
	}

	Atanor* more(Atanor* a) {
		if (value > a->Short())
			return aTRUE;
		return aFALSE;
	}

	Atanor* same(Atanor* a) {
		if (value == a->Short())
			return aTRUE;
		return aFALSE;
	}

	Atanor* different(Atanor* a) {
		if (value != a->Short())
			return aTRUE;
		return aFALSE;
	}

	Atanor* lessequal(Atanor* a) {
		if (value <= a->Short())
			return aTRUE;
		return aFALSE;
	}

	Atanor* moreequal(Atanor* a) {
		if (value >= a->Short())
			return aTRUE;
		return aFALSE;
	}

	Atanor* Succ() {
		return new Atanorshort(value + 1);
	}

	Atanor* Pred() {
		return new Atanorshort(value - 1);
	}

};
//--------------------------------------------------------------------
class AtanorConstDecimal : public AtanorBaseConst {
public:
	float value;

	AtanorConstDecimal(double v, AtanorGlobal* g = NULL, Atanor* parent = NULL) : value(v), AtanorBaseConst(a_decimal, g, parent) {}
	Exporting Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

	bool Checkprecision(Atanor* r) {
		if (r->Type() >= a_float)
			return false;
		return true;
	}
	Atanor* Newinstance(short, Atanor* f = NULL) {
		return new Atanordecimal(0);
	}

	short Typeinfered() {
		return a_decimal;
	}

	Atanor* Newvalue(Atanor* a, short idthread) {
		return new Atanordecimal(a->Decimal());
	}

	short Typenumber() {
		return a_decimal;
	}

	bool isNumber() {
		return true;
	}

	bool isFloat() {
		return true;
	}

	string String() {
		stringstream v;
		v << value;
		return v.str();
	}

	wstring UString() {
		std::wstringstream v;
		v << value;
		return v.str();
	}

	long Getinteger(short idthread) {
		return (long)value;
	}

	BLONG Getlong(short idthread) {
		return (BLONG)value;
	}

	short Getshort(short idthread) {
		return (short)value;
	}

	float Getdecimal(short idthread) {
		return value;
	}

	double Getfloat(short idthread) {
		return (double)value;
	}

	string Getstring(short idthread) {
		stringstream v;
		v << value;
		return v.str();
	}

	wstring Getustring(short idthread) {
		std::wstringstream v;
		v << value;
		return v.str();
	}

	long Integer() {
		return (long)value;
	}

	long Size() {
		return (long)value;
	}

	float Decimal() {
		return value;
	}

	double Float() {
		return value;
	}

	bool Boolean() {
		if (value == 0)
			return false;
		return true;
	}

	BLONG Long() {
		return (BLONG)value;
	}

	short Short() {
		return value;
	}


	Atanor* Atom(bool forced = false) {
		return new Atanordecimal(value);
	}


	//we add the current value with a
	Atanor* andset(Atanor* a, bool itself) {
		return new Atanordecimal((long)value & a->Integer());
	}

	Atanor* orset(Atanor* a, bool itself) {
		return new Atanordecimal((long)value | a->Integer());
	}

	Atanor* xorset(Atanor* a, bool itself) {
		return new Atanordecimal((long)value ^ a->Integer());
	}


	Atanor* plus(Atanor* a, bool itself) {
		return new Atanordecimal(value + a->Decimal());
	}

	Atanor* minus(Atanor* a, bool itself) {
		return new Atanordecimal(value - a->Decimal());
	}

	Atanor* multiply(Atanor* a, bool itself) {
		return new Atanordecimal(value * a->Decimal());
	}

	Atanor* divide(Atanor* a, bool itself) {
		float v = a->Decimal();
		if (v == 0)
			return new AtanorError("Error: Divided by 0");
		v = value / v;
		return new Atanordecimal(v);
	}

	Atanor* power(Atanor* a, bool itself) {
		double v = value;
		v = pow(v, a->Decimal());
		return new Atanordecimal(v);
	}

	Atanor* shiftleft(Atanor* a, bool itself) {
		return new Atanordecimal((long)value << a->Integer());
	}

	Atanor* shiftright(Atanor* a, bool itself) {
		return new Atanordecimal((long)value >> a->Integer());
	}

	Atanor* mod(Atanor* a, bool itself) {
		long v = a->Integer();
		if (v == 0)
			return new AtanorError("Error: Divided by 0");

		v = (long)value % v;
		return new Atanordecimal(v);
	}

	Atanor* less(Atanor* a) {
		if (value < a->Decimal())
			return aTRUE;
		return aFALSE;
	}

	Atanor* more(Atanor* a) {
		if (value > a->Decimal())
			return aTRUE;
		return aFALSE;
	}

	Atanor* same(Atanor* a) {
		if (value == a->Decimal())
			return aTRUE;
		return aFALSE;
	}

	Atanor* different(Atanor* a) {
		if (value != a->Decimal())
			return aTRUE;
		return aFALSE;
	}

	Atanor* lessequal(Atanor* a) {
		if (value <= a->Decimal())
			return aTRUE;
		return aFALSE;
	}

	Atanor* moreequal(Atanor* a) {
		if (value >= a->Decimal())
			return aTRUE;
		return aFALSE;
	}

	Atanor* Succ() {
		return new Atanordecimal(value + 1);
	}

	Atanor* Pred() {
		return new Atanordecimal(value - 1);
	}

};

//--------------------------------------------------------------------
class AtanorConstFloat : public AtanorBaseConst {
public:
	double value;

	AtanorConstFloat(double v, AtanorGlobal* g = NULL, Atanor* parent = NULL) : value(v), AtanorBaseConst(a_float, g, parent) {}
	Exporting Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

	Atanor* Newinstance(short, Atanor* f = NULL) {
		return globalAtanor->Providefloat(0);
	}

	Atanor* Newvalue(Atanor* a, short idthread) {
		return globalAtanor->Providefloat(a->Float());
	}

	short Typeinfered() {
		return a_float;
	}

	short Typenumber() {
		return a_float;
	}

	bool isNumber() {
		return true;
	}

	bool isFloat() {
		return true;
	}

	string String() {
		stringstream v;
		v << value;
		return v.str();
	}

	wstring UString() {
		std::wstringstream v;
		v << value;
		return v.str();
	}

	long Getinteger(short idthread) {
		return (long)value;
	}

	BLONG Getlong(short idthread) {
		return (BLONG)value;
	}

	short Getshort(short idthread) {
		return (short)value;
	}

	float Getdecimal(short idthread) {
		return (float)value;
	}

	double Getfloat(short idthread) {
		return value;
	}

	string Getstring(short idthread) {
		stringstream v;
		v << value;
		return v.str();
	}

	wstring Getustring(short idthread) {
		std::wstringstream v;
		v << value;
		return v.str();
	}


	long Integer() {
		return (long)value;
	}

	long Size() {
		return (long)value;
	}

	float Decimal() {
		return (float)value;
	}

	double Float() {
		return value;
	}

	bool Boolean() {
		if (value == 0)
			return false;
		return true;
	}

	BLONG Long() {
		return (BLONG)value;
	}

	short Short() {
		return value;
	}

	Atanor* Atom(bool forced = false) {
		return globalAtanor->Providefloat(value);
	}

	//we add the current value with a
	Atanor* andset(Atanor* a, bool itself) {
		return globalAtanor->Providefloat((long)value & a->Integer());
	}

	Atanor* orset(Atanor* a, bool itself) {
		return globalAtanor->Providefloat((long)value | a->Integer());
	}

	Atanor* xorset(Atanor* a, bool itself) {
		return globalAtanor->Providefloat((long)value ^ a->Integer());
	}


	Atanor* plus(Atanor* a, bool itself) {
		return globalAtanor->Providefloat(value + a->Float());
	}

	Atanor* minus(Atanor* a, bool itself) {
		return globalAtanor->Providefloat(value - a->Float());
	}

	Atanor* multiply(Atanor* a, bool itself) {
		return globalAtanor->Providefloat(value * a->Float());
	}

	Atanor* divide(Atanor* a, bool itself) {
		double v = a->Float();
		if (v == 0)
			return new AtanorError("Error: Divided by 0");
		v = value / v;
		return globalAtanor->Providefloat(v);
	}

	Atanor* power(Atanor* a, bool itself) {
		double v = value;
		v = pow(v, a->Float());
		return globalAtanor->Providefloat(v);
	}

	Atanor* shiftleft(Atanor* a, bool itself) {
		return globalAtanor->Providefloat((long)value << a->Integer());
	}

	Atanor* shiftright(Atanor* a, bool itself) {
		return globalAtanor->Providefloat((long)value >> a->Integer());
	}

	Atanor* mod(Atanor* a, bool itself) {
		long v = a->Integer();
		if (v == 0)
			return new AtanorError("Error: Divided by 0");

		v = (long)value % v;
		return globalAtanor->Providefloat(v);
	}

	Atanor* less(Atanor* a) {
		if (value < a->Float())
			return aTRUE;
		return aFALSE;
	}

	Atanor* more(Atanor* a) {
		if (value > a->Float())
			return aTRUE;
		return aFALSE;
	}

	Atanor* same(Atanor* a) {
		if (value == a->Float())
			return aTRUE;
		return aFALSE;
	}

	Atanor* different(Atanor* a) {
		if (value != a->Float())
			return aTRUE;
		return aFALSE;
	}

	Atanor* lessequal(Atanor* a) {
		if (value <= a->Float())
			return aTRUE;
		return aFALSE;
	}

	Atanor* moreequal(Atanor* a) {
		if (value >= a->Float())
			return aTRUE;
		return aFALSE;
	}

	Atanor* Succ() {
		return globalAtanor->Providefloat(value + 1);
	}

	Atanor* Pred() {
		return globalAtanor->Providefloat(value - 1);
	}
};

//--------------------------------------------------------------------

class AtanorConstLong : public AtanorBaseConst {
public:
	BLONG value;

	AtanorConstLong(BLONG v, AtanorGlobal* g = NULL, Atanor* parent = NULL) : value(v), AtanorBaseConst(a_long, g, parent) {}
	Exporting Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

	Atanor* Newinstance(short, Atanor* f = NULL) {
		return new Atanorlong(0);
	}

	Atanor* Newvalue(Atanor* a, short idthread) {
		return new Atanorlong(a->Long());
	}

	short Typeinfered() {
		return a_long;
	}

	bool Checkprecision(Atanor* r) {
		if (r->Type() >= a_float)
			return false;
		return true;
	}

	short Typenumber() {
		return a_long;
	}

	bool isNumber() {
		return true;
	}

	bool isInteger() {
		return true;
	}

	bool isLong() {
		return true;
	}

	string String() {
		stringstream v;
		v << value;
		return v.str();
	}

	wstring UString() {
		std::wstringstream v;
		v << value;
		return v.str();
	}

	long Getinteger(short idthread) {
		return (long)value;
	}

	BLONG Getlong(short idthread) {
		return value;
	}

	short Getshort(short idthread) {
		return (short)value;
	}

	float Getdecimal(short idthread) {
		return (float)value;
	}

	double Getfloat(short idthread) {
		return (double)value;
	}

	string Getstring(short idthread) {
		stringstream v;
		v << value;
		return v.str();
	}

	wstring Getustring(short idthread) {
		std::wstringstream v;
		v << value;
		return v.str();
	}

	long Integer() {
		return (long)value;
	}

	long Size() {
		return (long)value;
	}

	float Decimal() {
		return (float)value;
	}

	double Float() {
		return (double)value;
	}

	bool Boolean() {
		if (value == 0)
			return false;
		return true;
	}

	BLONG Long() {
		return value;
	}

	short Short() {
		return value;
	}

	Atanor* Atom(bool forced = false) {
		return new Atanorlong(value);
	}


	//we add the current value with a
	Atanor* andset(Atanor* a, bool itself) {
		return new Atanorlong(value & a->Long());
	}

	Atanor* orset(Atanor* a, bool itself) {
		return new Atanorlong(value | a->Long());
	}

	Atanor* xorset(Atanor* a, bool itself) {
		return new Atanorlong(value ^ a->Long());
	}


	Atanor* plus(Atanor* a, bool itself) {
		return new Atanorlong(value + a->Long());
	}

	Atanor* minus(Atanor* a, bool itself) {
		return new Atanorlong(value - a->Long());
	}

	Atanor* multiply(Atanor* a, bool itself) {
		return new Atanorlong(value * a->Long());
	}

	Atanor* divide(Atanor* a, bool itself) {
		double v = a->Float();
		if (v == 0)
			return new AtanorError("Error: Divided by 0");
		v = (double)value / v;
		return globalAtanor->Providefloat(v);
	}

	Atanor* power(Atanor* a, bool itself) {
		double v = (double)value;
		v = pow(v, a->Float());
		return globalAtanor->Providefloat(v);
	}

	Atanor* shiftleft(Atanor* a, bool itself) {
		return new Atanorlong(value << a->Long());
	}

	Atanor* shiftright(Atanor* a, bool itself) {
		return new Atanorlong(value >> a->Long());
	}

	Atanor* mod(Atanor* a, bool itself) {
		BLONG v = a->Long();
		if (v == 0)
			return new AtanorError("Error: Divided by 0");

		v = value % v;
		return new Atanorlong(v);
	}

	Atanor* less(Atanor* a) {
		if (value < a->Long())
			return aTRUE;
		return aFALSE;
	}

	Atanor* more(Atanor* a) {
		if (value > a->Long())
			return aTRUE;
		return aFALSE;
	}

	Atanor* same(Atanor* a) {
		if (value == a->Long())
			return aTRUE;
		return aFALSE;
	}

	Atanor* different(Atanor* a) {
		if (value != a->Long())
			return aTRUE;
		return aFALSE;
	}

	Atanor* lessequal(Atanor* a) {
		if (value <= a->Long())
			return aTRUE;
		return aFALSE;
	}

	Atanor* moreequal(Atanor* a) {
		if (value >= a->Long())
			return aTRUE;
		return aFALSE;
	}

	Atanor* Succ() {
		return new Atanorlong(value + 1);
	}

	Atanor* Pred() {
		return new Atanorlong(value - 1);
	}

};

//--------------------------------------------------------------------

class AtanorConstBool : public AtanorBaseConst {
public:
	bool value;

	AtanorConstBool(bool v, AtanorGlobal* g = NULL, Atanor* parent = NULL) : value(v), AtanorBaseConst(a_boolean, g, parent) {}

	bool isBoolean() {
		return true;
	}

	long Getinteger(short idthread) {
		return value;
	}

	short Typeinfered() {
		return a_boolean;
	}

	BLONG Getlong(short idthread) {
		return value;
	}

	short Getshort(short idthread) {
		return value;
	}

	float Getdecimal(short idthread) {
		return value;
	}

	double Getfloat(short idthread) {
		return value;
	}

	string Getstring(short idthread) {
		if (value)
			return "true";
		return "false";
	}

	wstring Getustring(short idthread) {
		if (value)
			return L"true";
		return L"false";
	}

	string String() {
		if (value)
			return "true";
		return "false";
	}

	wstring UString() {
		if (value)
			return L"true";
		return L"false";
	}

	long Integer() {
		return (long)value;
	}

	long Size() {
		return (long)value;
	}

	bool Boolean() {
		return value;
	}

	float Decimal() {
		return (float)value;
	}

	double Float() {
		return (double)value;
	}

	BLONG Long() {
		return (BLONG)value;
	}

	short Short() {
		return value;
	}

	Atanor* same(Atanor* a) {
		if (value == a->Boolean())
			return aTRUE;
		return aFALSE;
	}

	Atanor* different(Atanor* a) {
		if (value != a->Boolean())
			return aTRUE;
		return aFALSE;
	}

	Atanor* Newvalue(Atanor* a, short idthread) {
		if (a->Boolean() == true)
			return aTRUE;
		return aFALSE;
	}

	Atanor* plus(Atanor* a, bool itself) {
		if (a->Boolean() || value)
			return aTRUE;

		return aFALSE;
	}

	Atanor* minus(Atanor* a, bool itself) {
		if (value) {
			if (a->Boolean())
				return aFALSE;
			return aTRUE;
		}

		if (a->Boolean())
			return aTRUE;

		return aFALSE;
	}

	Atanor* Succ() {
		if (value)
			return aFALSE;
		return aTRUE;
	}

	Atanor* Pred() {
		if (value)
			return aFALSE;
		return aTRUE;
	}
};

#endif






