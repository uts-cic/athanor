
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
filename   : tools.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifdef WIN32
#include "Windows.h"
#endif

#include "codeparse.h"
#include "atanor.h"
#include "compilecode.h"
#include "atanorstring.h"
#include "atanorvector.h"
#include "atanorivector.h"
#include "atanorfvector.h"
#include "atanorsvector.h"
#include "atanoruvector.h"
#include "atanorbvector.h"
#include "atanordvector.h"
#include "atanormap.h"
#include "atanorprimemap.h"
#include "atanorprimemapss.h"
#include "atanorversion.h"
#include "atanorlvector.h"
#include "atanorhvector.h"


#ifdef UNIX
#include <unistd.h>
#include <dlfcn.h>
#include <signal.h>
#include <termios.h>
#include <sys/time.h>
#endif

#ifdef WIN32
#include <io.h>
#define DUP _dup
#define DUP2 _dup2
#define FILENO _fileno
#else
#define FILENO fileno
#define DUP dup
#define DUP2 dup2
#endif

//---------------------------------------------------------
#ifdef WIN32
bool errorsprintf = false;
void wrongSprintf(const wchar_t* expression,
	const wchar_t* function,
	const wchar_t* file,
	unsigned int line,
	uintptr_t pReserved) {
	errorsprintf = true;
}
#endif

const char ANTISEP = '\\';

#ifdef WIN32
const char SEP = '\\';
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

long gettimeofday(struct timeval *tv, struct timezone *tz) {

	FILETIME ft;
	unsigned __int64 tmpres = 0;

	if (NULL != tv) {
		GetSystemTimeAsFileTime(&ft);

		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;

		/*converting file time to unix epoch*/
		tmpres -= DELTA_EPOCH_IN_MICROSECS;
		tmpres /= 10;  /*convert into microseconds*/
		tv->tv_sec = (long)(tmpres / 1000000UL);
		tv->tv_usec = (long)(tmpres % 1000000UL);
	}

	return 0;
}
#endif

#ifdef UNIX
const char SEP = '/';
#endif

//-----------------------------------------------------------------------------------
char* convertir(char* res) {
    string conversion = conversion_latin_to_utf8(res);
    char* r = (char*)malloc(conversion.size() + 1);
    strcpy(r, STR(conversion));
    return r;
}
//-----------------------------------------------------------------------------------

double plustime(timeval& tempsinitial, timeval& tempsfinal) {
	double init = (((unsigned long)tempsinitial.tv_sec) * 1000 + ((unsigned long)tempsinitial.tv_usec) / 1000.0) + 0.5;
	double final = (((unsigned long)tempsfinal.tv_sec) * 1000 + ((unsigned long)tempsfinal.tv_usec) / 1000.0) + 0.5;
	return(final + init);
}

double minustime(timeval& tempsfinal, timeval& tempsinitial) {
	double init = (((unsigned long)tempsinitial.tv_sec) * 1000 + ((unsigned long)tempsinitial.tv_usec) / 1000.0);
	double final = (((unsigned long)tempsfinal.tv_sec) * 1000 + ((unsigned long)tempsfinal.tv_usec) / 1000.0);
	return(final - init);
}


long timeminus(double init, timeval& tempsfinal) {
	double final = (((unsigned long)tempsfinal.tv_sec) * 1000 + ((unsigned long)tempsfinal.tv_usec) / 1000.0);
	return(final - init);
}

//---------------------------------------------------------
static bool kifrestrandom = false;
static ThreadLock randomlock(NULL, false);
double localrandom(long mx) {
	if (!mx)
		mx = 1;
	static unsigned long x = 123456789;
	static unsigned long y = 362436069;
	static unsigned long z = 521288629;
	static long w = time(0);
	unsigned long t;
	if (kifrestrandom) {
		w = time(0);
		kifrestrandom = false;
	}

	Locking _lock(randomlock);
	t = x ^ (x << 11);
	x = y; y = z; z = w;
	w = w ^ (w >> 19) ^ (t ^ (t >> 8));
	return abs(w%mx);
}

//---------------------------------------------------------
bool IsLong(BLONG v) {
	if (v < -2147483647 || v > 2147483647)
		return true;
	return false;
}

bool IsShort(BLONG v) {
	if (v < -32768 || v > 32767)
		return false;

	return true;
}

//---------------------------------------------------------
bool v_comma_split_string(string& thestr, vector<string>& v) {
	size_t sz = thestr.size() - 1;
	if (thestr[0] != '[' || thestr[sz] != ']')
		return false;
	if (sz == 1)
		return true;

	size_t pos;
	bool comma = true;
	string value;
	uchar c, nxt;

	for (pos = 1; pos < sz; pos++) {
		c = thestr[pos];
		switch (c) {
		case 9:
		case 10:
		case 13:
		case 32:
			continue;
		case '"':
		case '\'':
			nxt = c;
			break;
		case '@':
			nxt = '"';
			if (thestr[++pos] != '"') {
				v.clear();
				return false;
			}
			break;
		case ',':
			if (comma) {
				v.clear();
				return false;
			}
			comma = true;
			continue;
		default:
			v.clear();
			return false;

		}
		if (!comma) {
			v.clear();
			return false;
		}
		comma = false;
		value = "";
		pos++;
		while (pos < sz && thestr[pos] != nxt)
			value += thestr[pos++];
		if (pos == sz) {
			v.clear();
			return false;
		}
		if (c == '@') {
			if (thestr[++pos] != '@') {
				v.clear();
				return false;
			}
		}
		v.push_back(value);
	}

	if (comma) {
		v.clear();
		return false;
	}
	return true;
}

bool v_comma_split_string(wstring& thestr, vector<wstring>& v) {
	size_t sz = thestr.size() - 1;
	if (thestr[0] != '[' || thestr[sz] != ']')
		return false;
	if (sz == 1)
		return true;

	size_t pos;
	bool comma = true;
	wstring value;
	wchar_t c, nxt;

	for (pos = 1; pos < sz; pos++) {
		c = thestr[pos];
		switch (c) {
		case 9:
		case 10:
		case 13:
		case 32:
			continue;
		case '"':
		case '\'':
			nxt = c;
			break;
		case '@':
			nxt = '"';
			if (thestr[++pos] != '"') {
				v.clear();
				return false;
			}
			break;
		case ',':
			if (comma) {
				v.clear();
				return false;
			}
			comma = true;
			continue;
		default:
			v.clear();
			return false;

		}
		if (!comma) {
			v.clear();
			return false;
		}
		comma = false;
		value = L"";
		pos++;
		while (pos < sz && thestr[pos] != nxt)
			value += thestr[pos++];
		if (pos == sz) {
			v.clear();
			return false;
		}
		if (c == '@') {
			if (thestr[++pos] != '@') {
				v.clear();
				return false;
			}
		}
		v.push_back(value);
	}

	if (comma) {
		v.clear();
		return false;
	}
	return true;
}

bool v_comma_split_decimal(string& thestr, vector<float>& v) {
	size_t sz = thestr.size() - 1;
	if (thestr[0] != '[' || thestr[sz] != ']')
		return false;
	if (sz == 1)
		return true;

	size_t pos;
	bool comma = true;
	string value;
	uchar c;

	for (pos = 1; pos < sz; pos++) {
		c = thestr[pos];
		switch (c) {
		case 9:
		case 10:
		case 13:
		case 32:
			continue;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '.':
			comma = false;
			value += c;
			break;
		case ',':
			if (comma) {
				v.clear();
				return false;
			}
			v.push_back(convertfloat(STR(value)));
			value = "";
			comma = true;
			break;
		default:
			v.clear();
			return false;
		}
	}
	if (comma) {
		v.clear();
		return false;
	}
	v.push_back(convertfloat(STR(value)));
	return true;
}

bool v_comma_split_float(string& thestr, vector<double>& v) {
	size_t sz = thestr.size() - 1;
	if (thestr[0] != '[' || thestr[sz] != ']')
		return false;
	if (sz == 1)
		return true;

	size_t pos;
	bool comma = true;
	string value;
	uchar c;

	for (pos = 1; pos < sz; pos++) {
		c = thestr[pos];
		switch (c) {
		case 9:
		case 10:
		case 13:
		case 32:
			continue;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '.':
			comma = false;
			value += c;
			break;
		case ',':
			if (comma) {
				v.clear();
				return false;
			}
			v.push_back(convertfloat(STR(value)));
			value = "";
			comma = true;
			break;
		default:
			v.clear();
			return false;
		}
	}
	if (comma) {
		v.clear();
		return false;
	}
	v.push_back(convertfloat(STR(value)));
	return true;
}

bool v_comma_split_int(string& thestr, vector<long>& v) {
	size_t sz = thestr.size() - 1;
	if (thestr[0] != '[' || thestr[sz] != ']')
		return false;
	if (sz == 1)
		return true;

	size_t pos;
	bool comma = true;
	bool point = false;
	long value = 0;
	uchar c;

	for (pos = 1; pos < sz; pos++) {
		c = thestr[pos];
		switch (c) {
		case 9:
		case 10:
		case 13:
		case 32:
			continue;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if (point)
				continue;
			value = value * 10 + c - 48;
			comma = false;
			break;
		case '.':
			comma = false;
			point = true;
			break;
		case ',':
			if (comma) {
				v.clear();
				return false;
			}
			v.push_back(value);
			value = 0;
			comma = true;
			point = false;
			break;
		default:
			v.clear();
			return false;
		}
	}
	if (comma) {
		v.clear();
		return false;
	}
	v.push_back(value);
	return true;
}

bool v_comma_split_long(string& thestr, vector<BLONG>& v) {
	size_t sz = thestr.size() - 1;
	if (thestr[0] != '[' || thestr[sz] != ']')
		return false;
	if (sz == 1)
		return true;

	size_t pos;
	bool comma = true;
	bool point = false;
	BLONG value = 0;
	uchar c;

	for (pos = 1; pos < sz; pos++) {
		c = thestr[pos];
		switch (c) {
		case 9:
		case 10:
		case 13:
		case 32:
			continue;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if (point)
				continue;
			value = value * 10 + c - 48;
			comma = false;
			break;
		case '.':
			comma = false;
			point = true;
			break;
		case ',':
			if (comma) {
				v.clear();
				return false;
			}
			v.push_back(value);
			value = 0;
			comma = true;
			point = false;
			break;
		default:
			v.clear();
			return false;
		}
	}
	if (comma) {
		v.clear();
		return false;
	}
	v.push_back(value);
	return true;
}

bool v_comma_split_byte(string& thestr, vector<uchar>& v) {
	size_t sz = thestr.size() - 1;
	if (thestr[0] != '[' || thestr[sz] != ']')
		return false;
	if (sz == 1)
		return true;

	size_t pos;
	bool comma = true;
	bool point = false;
	long value = 0;
	uchar c;

	for (pos = 1; pos < sz; pos++) {
		c = thestr[pos];
		switch (c) {
		case 9:
		case 10:
		case 13:
		case 32:
			continue;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if (point)
				continue;
			value = value * 10 + c - 48;
			comma = false;
			break;
		case '.':
			comma = false;
			point = true;
			break;
		case ',':
			if (comma) {
				v.clear();
				return false;
			}
			v.push_back(value);
			value = 0;
			comma = true;
			point = false;
			break;
		default:
			v.clear();
			return false;
		}
	}
	if (comma) {
		v.clear();
		return false;
	}
	v.push_back(value);
	return true;
}

bool v_comma_split_short(string& thestr, vector<short>& v) {
	size_t sz = thestr.size() - 1;
	if (thestr[0] != '[' || thestr[sz] != ']')
		return false;
	if (sz == 1)
		return true;

	size_t pos;
	bool comma = true;
	bool point = false;
	short value = 0;
	uchar c;

	for (pos = 1; pos < sz; pos++) {
		c = thestr[pos];
		switch (c) {
		case 9:
		case 10:
		case 13:
		case 32:
			continue;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if (point)
				continue;
			value = value * 10 + c - 48;
			comma = false;
			break;
		case '.':
			comma = false;
			point = true;
			break;
		case ',':
			if (comma) {
				v.clear();
				return false;
			}
			v.push_back(value);
			value = 0;
			comma = true;
			point = false;
			break;
		default:
			v.clear();
			return false;
		}
	}
	if (comma) {
		v.clear();
		return false;
	}
	v.push_back(value);
	return true;
}
//---------------------------------------------------------------------------------------------

Atanor* Mapcompare(Atanor*a, Atanor*b, AtanorGlobal* global) {
	if (!b->isMapContainer() || a->Size() != b->Size())
		return aFALSE;

	if (!a->Size())
		return aTRUE;

	Atanor* key;
	Atanor* value;
	Atanor* vb;
	Atanor* res;
	Locking _lock((AtanorObject*)a);

	AtanorIteration* itr = a->Newiteration(false);
	for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
		value = itr->IteratorValue();
		key = itr->IteratorKey();
		res = aFALSE;
		switch (key->Type()) {
		case a_long:
		case a_int:
			vb = b->Value(key->Integer());
			if (vb != aNOELEMENT)
				res = vb->same(value);
			break;
		case a_float:
			vb = b->Value(key->Float());
			if (vb != aNOELEMENT)
				res = vb->same(value);
			break;
		default:
			vb = b->Value(key->String());
			if (vb != aNOELEMENT)
				res = vb->same(value);
		}

		vb->Release();
		if (res == aFALSE) {
			itr->Release();
			return aFALSE;
		}
	}
	itr->Release();
	return aTRUE;
}


//---------------------------------------------------------------------------
Exporting char StringIndexes(string& svalue, Atanor* index, int& ileft, int& iright, short idthread) {

	Atanor* left = index;
	Atanor* right = NULL;
	bool sleft = false;
	bool sright = false;
	bool releft = false;
	bool reright = false;
	if (index->isIndex()) {
		AtanorIndex* kind = (AtanorIndex*)index;
		sleft = kind->signleft;
		sright = kind->signright;
		left = kind->left->Get(aNULL, aNULL, idthread);
		if (left != kind->left)
			releft = true;

		if (kind->interval == true) {
			right = kind->right->Get(aNULL, aNULL, idthread);
			if (right != kind->right)
				reright = true;
		}
	}

	long sz = svalue.size();
	long szc = size_c(svalue);
	long offset = -1;
	long subsize = -1;

	if (left->Type() == a_string) {
		string sub = left->String();
		//then we are looking for a substring
		if (sleft)
			ileft = s_rfindbyte(svalue, sub, sz);
		else
			ileft = s_findbyte(svalue, sub, 0);
		if (ileft == -1)
			return 0;
		offset = size_c(sub);
		subsize = sub.size();
	}
	else {
		ileft = left->Integer();
		if (ileft < 0)
			ileft += szc;
		if (ileft >= szc)
			return 0;
		ileft = c_chartobyteposition(USTR(svalue), ileft);
		if (ileft < 0 || ileft >= sz)
			return 0;
	}

	if (releft)
		left->Release();

	//We return as a non interval
	if (right == NULL) {
		if (subsize != -1) {//unless the left index was a string...
			iright = ileft + subsize;
			return 2;
		}
		return 1;
	}

	if (right->Type() == a_string) {
		string sub = right->String();
		if (sright)
			iright = s_rfindbyte(svalue, sub, sz);
		else {
			if (subsize != -1)
				iright = s_findbyte(svalue, sub, ileft + subsize);
			else
				iright = s_findbyte(svalue, sub, ileft);
		}
		if (iright == -1)
			return 0;
		iright += sub.size();
	}
	else {
		iright = right->Integer();

		if (iright < 0 || right == aNULL)
			iright += size_c(svalue);
		else {
			if (offset != -1) {
				int szoffset = ileft;
				szoffset = c_bytetocharposition(USTR(svalue), szoffset);
				iright += szoffset + offset;
			}
		}
		if (iright >= szc)
			iright = sz;
		else {
			iright = c_chartobyteposition(USTR(svalue), iright);
			if (iright > sz)
				iright = sz;
		}
	}
	
	if (reright)
		right->Release();

	if ((iright - ileft) <= 0)
		return 0;
	return 2;
}
//---------------------------------------------------------
Exporting char StringIndexes(wstring& svalue, Atanor* index, int& ileft, int& iright, short idthread) {

	Atanor* left = index;
	Atanor* right = NULL;
	bool sleft = false;
	bool sright = false;
	bool releft = false;
	bool reright = false;

	if (index->isIndex()) {
		AtanorIndex* kind = (AtanorIndex*)index;
		sleft = kind->signleft;
		sright = kind->signright;
		left = kind->left->Get(aNULL, aNULL, idthread);
		if (left != kind->left)
			releft = true;

		if (kind->interval == true) {
			right = kind->right->Get(aNULL, aNULL, idthread);
			if (right != kind->right)
				reright = true;
		}
	}

	int sz = svalue.size();
	int offset = -1;

	if (left->isString()) {
		wstring sub = left->UString();
		//then we are looking for a substring
		if (sleft)
			ileft = s_rfind(svalue, sub, sz);
		else
			ileft = s_find(svalue, sub, 0);

		if (ileft == -1)
			return 0;
		offset = sub.size();
	}
	else {
		ileft = left->Integer();
		if (ileft < 0)
			ileft += sz;
		if (ileft >= sz)
			return 0;
	}
	
	if (releft)
		left->Release();

	//We return as a non interval
	if (right == NULL) {
		if (offset != -1) {//unless the left index was a string...
			iright = ileft + offset;
			return 2;
		}
		return 1;
	}

	if (right->isString()) {
		wstring wr = right->UString();
		if (sright)
			iright = s_rfind(svalue, wr, sz);
		else {
			if (offset != -1)
				iright = s_find(svalue, wr, ileft + offset);
			else
				iright = s_find(svalue, wr, ileft);
		}
		if (iright == -1)
			return 0;
		iright += wr.size();
	}
	else {
		iright = right->Integer();

		if (iright < 0 || right == aNULL)
			iright += svalue.size();
		else {
			if (offset != -1)
				iright += ileft + offset;
		}
		if (iright >= sz)
			iright = sz;
		else {
			if (iright>sz)
				iright = sz;
		}
	}

	if (reright)
		right->Release();

	if ((iright - ileft) <= 0)
		return 0;
	return 2;
}
//---------------------------------------------------------
unsigned long StringEditDistance(wstring& value, wstring& s2) {
	unsigned long s1len, s2len, x, y, lastdiag, olddiag;
	s1len = value.size();
	s2len = s2.size();
	size_t* column = new size_t[s1len + 1];
	for (y = 1; y <= s1len; y++)
		column[y] = y;
	for (x = 1; x <= s2len; x++) {
		column[0] = x;
		for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
			olddiag = column[y];
			column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + (value[y - 1] == s2[x - 1] ? 0 : 1));
			lastdiag = olddiag;
		}
	}
	s2len = column[s1len];
	delete[] column;
	return s2len;
}


unsigned long StringEditDistance(string& value, string& s2) {
	unsigned long s1len, s2len, x, y, lastdiag, olddiag;
	s1len = value.size();
	s2len = s2.size();
	size_t* column = new size_t[s1len + 1];
	for (y = 1; y <= s1len; y++)
		column[y] = y;
	for (x = 1; x <= s2len; x++) {
		column[0] = x;
		for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
			olddiag = column[y];
			column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + (value[y - 1] == s2[x - 1] ? 0 : 1));
			lastdiag = olddiag;
		}
	}
	s2len = column[s1len];
	delete[] column;
	return s2len;
}

string StringMinus(string v, string sub) {
	size_t pos = v.find(sub);
	if (pos == string::npos)
		return v;

	size_t nb = sub.size();
	//we keep our string up to p
	sub = v.substr(0, pos);
	//then we skip the nb characters matching the size of v
	pos += nb;
	//then we concatenate with the rest of the string
	sub += v.substr(pos, v.size() - pos);
	return sub;
}

wstring StringMinus(wstring v, wstring sub) {
	size_t pos = v.find(sub);
	if (pos == wstring::npos)
		return v;

	size_t nb = sub.size();
	//we keep our wstring up to p
	sub = v.substr(0, pos);
	//then we skip the nb characters matching the size of v
	pos += nb;
	//then we concatenate with the rest of the wstring
	sub += v.substr(pos, v.size() - pos);
	return sub;
}
//---------------------------------------------------------
void convertbytepositiontochar(vector<long>& v, agnostring& s) {
	vector<long> vres;
	int i = 0;
	int r = 0;
	for (int j = 0; j < v.size(); j++) {
		c_bytetocharpositionidx(USTR(s), v[j], r, i);
		vres.push_back(r);
	}
	v = vres;
}

void convertbytepositiontochar(vector<double>& v, agnostring& s) {
	vector<double> vres;
	int i = 0;
	int r = 0;
	for (int j = 0; j < v.size(); j++) {
		c_bytetocharpositionidx(USTR(s), v[j], r, i);
		vres.push_back(r);
	}
	v = vres;
}

//---------------------------------------------------------
void XNBrowse(x_node* xn, Atanor* kf) {
	if (xn == NULL)
		return;
	Atanorprimemap* kmap = new Atanorprimemap;
	kf->Push(kmap);

	if (xn->nodes.size()) {
		string key = xn->token;
		if (xn->value != "")
			key += "_" + xn->value;
		Atanorvector* kvect = globalAtanor->Providevector();
		kmap->Push(key, kvect);
		for (size_t i = 0; i < xn->nodes.size(); i++) {
			XNBrowse(xn->nodes[i], kvect);
		}
	}
	else
		kmap->Push(xn->token, globalAtanor->Providestring(xn->value));
}

//---------------------------------------------------------
Exporting Atanor* Selectaprimemapss(Atanor* context) {
	if (context != NULL && context->Type()==Atanorprimemapss::idtype && context->isAffectation()) {
		context->Clear();
		return context;
	}

	return new Atanorprimemapss;
}

Exporting Atanor* SelectContainer(Atanor* context, short idthread) {
    if (context != NULL && context->isContainer()) {
        if(context->isAffectation()) {
            context->Clear();
            return context;
        }
        
        return context->Newinstance(idthread);
    }
    
    return NULL;
}


Exporting Atanor* Selectacontainer(Atanor* context, short idthread) {
	if (context != NULL && context->isContainer()) {
		if (context->isAffectation()) {
			context->Clear();
			return context;
		}
		return context->Newinstance(idthread);
	}

	return globalAtanor->Providevector();
}

Exporting Atanor* Selectamap(Atanor* context) {
	if (context != NULL && context->isMapContainer() && context->isAffectation()) {
		context->Clear();
		return context;
	}

	return globalAtanor->Providemap();
}


Exporting Atanor* Selectavector(Atanor* context) {
	if (context != NULL && context->isVectorContainer() && context->isAffectation()) {
		context->Clear();
		return context;
	}

	return globalAtanor->Providevector();
}

Exporting Atanor* Selectasvector(Atanor* context) {
	if (context != NULL && context->isVectorContainer() && context->isAffectation() && context->Type() == a_svector) {
		context->Clear();
		return context;
	}

	return globalAtanor->Providesvector();
}

Exporting Atanor* Selectauvector(Atanor* context) {
	if (context != NULL && context->isVectorContainer() && context->isAffectation() && context->Type() == a_uvector) {
		context->Clear();
		return context;
	}

	return globalAtanor->Provideuvector();
}

Exporting Atanor* Selectaivector(Atanor* context) {
	if (context != NULL && context->isVectorContainer() && context->isAffectation() && context->Type() == a_ivector) {
		context->Clear();
		return context;
	}

	return globalAtanor->Provideivector();
}

Exporting Atanor* Selectahvector(Atanor* context) {
	if (context != NULL && context->isVectorContainer() && context->isAffectation() && context->Type() == a_hvector) {
		context->Clear();
		return context;
	}

	return new Atanorhvector;
}

Exporting Atanor* Selectalvector(Atanor* context) {
	if (context != NULL && context->isVectorContainer() && context->isAffectation() && context->Type() == a_lvector) {
		context->Clear();
		return context;
	}

	return new Atanorlvector;
}

Exporting Atanor* Selectadvector(Atanor* context) {
	if (context != NULL && context->isVectorContainer() && context->isAffectation() && context->Type() == a_dvector) {
		context->Clear();
		return context;
	}

	return new Atanordvector;
}

Exporting Atanor* Selectafvector(Atanor* context) {
	if (context != NULL && context->isVectorContainer() && context->isAffectation() && context->Type() == a_fvector) {
		context->Clear();
		return context;
	}

	return globalAtanor->Providefvector();
}

Exporting Atanor* Selectabvector(Atanor* context) {
	if (context != NULL && context->isVectorContainer() && context->isAffectation() && context->Type() == a_bvector) {
		context->Clear();
		return context;
	}

	return new Atanorbvector;
}
//--------------------------------------------------------------------
Exporting Atanor* AtanorGlobal::EvaluateLisp(string& s, string& o, string& c, short idthread) {

	x_readstring xr(s);
	xr.concatenatedots = false;
	xr.readstrings = false;
	xr.loadtoken();
	bnf_atanor bnf;
	bnf.baseline = linereference;

	bnf.initialize(&xr);

	bnf_atanor* previous = currentbnf;
	currentbnf = &bnf;

	string lret;
	x_node* xn = new x_node;

	bnf.Y_var_0 = o[0];
	bnf.Y_var_1 = c[0];

	if (bnf.m_lisp(lret, &xn) != 1)
		return Returnerror("Unknown expression", idthread);

	Atanor* kret = globalAtanor->Providevector();
	AtanorCode* code = spaces[0];
	code->compilemode = false;

	try {
		code->Traverse(xn, kret);
	}
	catch (AtanorRaiseError* m) {
		code->global = this;
		kret->Release();
		kret = Returnerror(m->message, idthread);
		delete m;
	}

	currentbnf = previous;
	delete xn;
	return kret;
}

//we check the presence of specific non alpha characters
static string normalizeopen(agnostring& s, string& ch) {
	if (s_is_alpha(ch))
		return ch;

	agnostring o(ch);
	string res;
	string a;
	o.begin();
	int i = 0;
	while (!o.end()) {
		a = o.next();
		i = 0;
		if (c_is_alpha(USTR(a), i) || (a[0] >= 48 && a[0] <= 57))
			res += a;
		else
			res += "_";
	}

	while (s.find(res, 0) != -1)
		res += "_";
	s = s.replace(o, res);
	return res;
}

Exporting Atanor* AtanorGlobal::EvaluateTags(agnostring& s, string& o, string& c, short idthread) {

	o = normalizeopen(s, o);
	c = normalizeopen(s, c);

	x_readstring xr(s);
	xr.readstrings = false;
	xr.concatenatedots = false;

	xr.loadtoken();
	bnf_atanor bnf;
	bnf.baseline = linereference;
	bnf.initialize(&xr);

	bnf_atanor* previous = currentbnf;
	currentbnf = &bnf;

	string lret;
	x_node* xn = new x_node;


	bnf.VS_var_2 = o;
	bnf.VS_var_3 = c;

	if (bnf.m_tag(lret, &xn) != 1)
		return Returnerror("Unknown expression", idthread);

	Atanor* kret = globalAtanor->Providevector();

	AtanorCode* code = spaces[0];
	code->compilemode = false;

	try {
		code->Traverse(xn, kret);
	}
	catch (AtanorRaiseError* m) {
		code->global = this;
		kret->Release();
		kret = Returnerror(m->message, idthread);
		delete m;
	}

	currentbnf = previous;
	delete xn;
	return kret;
}

Exporting Atanor* AtanorGlobal::EvaluateVector(string& s, short idthread) {
	x_readstring xr(s);
	xr.loadtoken();
	bnf_atanor bnf;
	bnf.baseline = linereference;
	bnf.initialize(&xr);

	bnf_atanor* previous = currentbnf;
	currentbnf = &bnf;
	string lret;
	x_node* xn = new x_node;


	if (bnf.m_jvector(lret, &xn) != 1)
		return Returnerror("Unknown expression", idthread);

	Atanor* kret = aNULL;
	AtanorCode* code = spaces[0];
	code->compilemode = false;

	try {
		kret = code->Traverse(xn, NULL);
	}
	catch (AtanorRaiseError* m) {
		code->global = this;
		kret->Release();
		kret = Returnerror(m->message, idthread);
		delete m;
	}

	currentbnf = previous;
	delete xn;
	return kret;
}

Exporting Atanor* AtanorGlobal::EvaluateMap(string& s, short idthread) {
	x_readstring xr(s);
	xr.loadtoken();
	bnf_atanor bnf;
	bnf.baseline = linereference;
	bnf.initialize(&xr);

	bnf_atanor* previous = currentbnf;
	currentbnf = &bnf;
	string lret;
	x_node* xn = new x_node;


	if (bnf.m_jmap(lret, &xn) != 1)
		return Returnerror("Unknown expression", idthread);

	Atanor* kret = aNULL;
	AtanorCode* code = spaces[0];
	code->compilemode = false;

	try {
		kret = code->Traverse(xn, NULL);
	}
	catch (AtanorRaiseError* m) {
		code->global = this;
		kret->Release();
		kret = Returnerror(m->message, idthread);
		delete m;
	}

	delete xn;
	currentbnf = previous;
	return kret;
}

Exporting Atanor* AtanorGlobal::EvaluateJSON(string& s, short idthread) {
	x_readstring xr(s);
	xr.loadtoken();
	bnf_atanor bnf;
	bnf.baseline = linereference;
	bnf.initialize(&xr);

	bnf_atanor* previous = currentbnf;
	currentbnf = &bnf;
	string lret;
	x_node* xn = new x_node;

	if (bnf.m_jexpression(lret, &xn) != 1)
		return Returnerror("Unknown expression", idthread);

	Atanor* kret = aNULL;
	AtanorCode* code = spaces[0];
	code->compilemode = false;

	try {
		kret = code->Traverse(xn, NULL);
	}
	catch (AtanorRaiseError* m) {
		code->global = this;
		kret->Release();
		kret = Returnerror(m->message, idthread);
		delete m;
	}

	currentbnf = previous;
	delete xn;
	return kret;
}

//--------------------------------------------------------------------
bool WaitingFor(double tm, PauseBack pb, void* data) {
	double init;
	double res;
	timeval tempsfinal;

	gettimeofday(&tempsfinal, NULL);
	init = (((unsigned long)tempsfinal.tv_sec) * 1000 + ((unsigned long)tempsfinal.tv_usec) / 1000.0);
	res = tm * 1000;

	double diff = 0;
	while (diff < res) {
		if (pb(data) == true)
			return true;
		gettimeofday(&tempsfinal, NULL);
		diff = timeminus(init, tempsfinal);
	}
	return false;
}

void Wait(double tm) {
	double init;
	double res;
	timeval tempsfinal;

	gettimeofday(&tempsfinal, NULL);
	init = (((unsigned long)tempsfinal.tv_sec) * 1000 + ((unsigned long)tempsfinal.tv_usec) / 1000.0);
	res = tm * 1000;

	double diff = 0;
	while (diff < res) {
		gettimeofday(&tempsfinal, NULL);
		diff = timeminus(init, tempsfinal);
	}
}

#ifdef WIN32
bool DirectoryCreation(char* noms, long pos) {
	char* pt = strchr(noms + pos, SEP);
	while (pt != NULL) {
		*pt = 0;
		CreateDirectoryA(noms, NULL);
		*pt = SEP;
		pt = strchr(pt + 1, SEP);
	}

	bool created = true;
	if (CreateDirectoryA(noms, NULL) == FALSE)
		created = false;

	return created;
}
#else
bool DirectoryCreation(char* noms, long pos) {
	char* pt = strchr(noms + pos, SEP);
	while (pt != NULL) {
		*pt = 0;
		mkdir(noms, S_IRWXU | S_IRWXG | S_IRWXO);
		*pt = SEP;
		pt = strchr(pt + 1, SEP);
	}

	bool created = true;
	if (mkdir(noms, S_IRWXU | S_IRWXG | S_IRWXO) != 0)
		created = false;
	return created;
}
#endif

#ifdef APPLE
extern "C" {
	int    gethostuuid(uuid_t id, const struct timespec *wait) {
		return -1;
	}
}
#endif
//-----------------------------------------------------------------------
static FILE* localstreamerr = NULL;
static FILE* localstream = NULL;

void clear_output() {
	if (localstream != NULL) {
		fclose(localstream);
		localstream = NULL;
	}

	if (localstreamerr != NULL) {
		fclose(localstreamerr);
		localstreamerr = NULL;
	}
}

char redirect_output(string& filename, long output) {
	if (output == 1) {
		if (localstream != NULL)
			return -2;

		int o = DUP(output);

		localstream = fopen(STR(filename), "w");
		if (localstream == NULL)
			return -1;
		DUP2(FILENO(localstream), output);
		return o;
	}

	if (localstreamerr != NULL)
		return -2;

	int o = DUP(output);

	localstreamerr = fopen(STR(filename), "w");
	if (localstreamerr == NULL)
		return -1;
	DUP2(FILENO(localstreamerr), output);
	return o;

}

char restate_output(int o, long output) {
	if (output == 1) {
		if (localstream == NULL)
			return -1;

		fflush(stdout);
		fclose(localstream);
		localstream = NULL;
		DUP2(o, output);
		return 0;
	}

	if (localstreamerr == NULL)
		return -1;

	fflush(stdout);
	fclose(localstreamerr);
	localstreamerr = NULL;
	DUP2(o, output);
	return 0;
}
//-----------------------------------------------------------------------------
void Flattening(Atanor* ret, Atanor* res) {
	AtanorIteration* itr = res->Newiteration(false);
	Atanor* kval;
	for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
		kval = itr->IteratorValue();
		if (kval->isContainer())
			Flattening(ret, kval);
		else
			ret->Push(kval);
	}
	itr->Release();
}
//-----------------------------------------------------------------------------
#ifdef WIN32
char* Getenv(char* name) {
	static char rep[4096];
	size_t size = 0;
	getenv_s(&size, rep, 4096, name);
	return rep;
}
#else
#define Getenv getenv
#endif

void FileNameNomalization(char* fileName, char* buffer, long buffersz) {
	//Tout d'abord on ramene tous les chemins a de l'UNIX
	long l = strlen(buffer);
	//On normalise les chemins
	for (int i = 0; i < l; i++) {
		if (buffer[i] == ANTISEP)
			buffer[i] = SEP;
	}

	//s'il y a une variable d'environnement
	char* vari = strchr(buffer, '$');
	fileName[0] = 0;
	while (vari) {
		char* reper = Getenv(vari + 1);
		char* pt = strchr(vari + 1, SEP);

		if (pt != NULL)
			*pt = 0;

		//On recopie la partie qui precede la variable
		int lvar = vari - buffer;
		int lnom = strlen(fileName);
		memcpy(fileName + lnom, buffer, lvar);
		fileName[lvar + lnom] = 0;

		if (reper != NULL)
			strcat_s(fileName, buffersz, reper);

		if (pt != NULL) {
			*pt = SEP;
			static char inter[1000];
			strcpy_s(inter,1000, pt);
			strcpy_s(buffer, buffersz, inter);
		}
		else
			buffer[0] = 0;
		vari = strchr(buffer, '$');
	}

	strcat_s(fileName, buffersz, buffer);
	char localpath[4096];
#ifdef WIN32
	_fullpath(localpath, fileName, 4096);
#else
	realpath(fileName, localpath);
#endif

	strcpy_s(fileName, buffersz, localpath);
}

string NormalizeFileName(string n) {
	if (n == "")
		return "";

	char buff[4096];
	char name[4096];
	strcpy_s(name, 4096, STR(n));

	FileNameNomalization(buff, name, 4096);
	return buff;
}

Exporting bool AtanorGlobal::RecordExternalLibrary(string name, AtanorExternalModule module) {
	if (externalLibraries.find(name) != externalLibraries.end())
		return false;

	if (module == NULL)
		return true;

	externalLibraries[name] = module;
	return true;
}

//------------------------------------------------------------
char Selecttype(Atanor* context) {
	if (context->isMapContainer())
		return a_map;

	switch (context->Typevariable()) {
	case a_ivector:
		return a_ivector;
	case a_fvector:
		return a_fvector;
	case a_svector:
		return a_svector;
	case a_uvector:
		return a_uvector;
	case a_vector:
		return a_vector;
	case a_string:
		return a_string;
	case a_ustring:
		return a_ustring;
	case a_dvector:
		return a_dvector;
	case a_lvector:
		return a_lvector;
	default:
		if (context->isVectorContainer())
			return a_vector;
	}
	return 0;
}

Atanor* Storealongtype(Atanor* context, Atanor* a, short idthread, char& addvalue) {
	switch (addvalue) {
	case a_vector:
		context->Push(a);
		return context;
	case a_hvector:
		((Atanorhvector*)context)->values.push_back(a->Short());
		return context;
	case a_ivector:
		((Atanorivector*)context)->values.push_back(a->Integer());
		return context;
	case a_fvector:
		((Atanorfvector*)context)->values.push_back(a->Float());
		return context;
	case a_svector:
		((Atanorsvector*)context)->values.push_back(a->String());
		return context;
	case a_uvector:
		((Atanoruvector*)context)->values.push_back(a->UString());
		return context;
	case a_dvector:
		((Atanordvector*)context)->values.push_back(a->Decimal());
		return context;
	case a_lvector:
		((Atanorlvector*)context)->values.push_back(a->Long());
		return context;
	case a_string:
		((Atanorstring*)context)->value += a->String();
		return context;
	case a_ustring:
		((Atanorustring*)context)->value += a->UString();
		return context;
	case a_map:
		context->Pushinmap(a, idthread);
		return context;
	}

	switch (a->Typevariable()) {
	case a_short:
		context = new Atanorhvector;
		((Atanorhvector*)context)->values.push_back(a->Short());
		addvalue = a_hvector;
		return context;
	case a_int:
		context = globalAtanor->Provideivector();
		((Atanorivector*)context)->values.push_back(a->Integer());
		addvalue = a_ivector;
		return context;
	case a_long:
		context = new Atanorlvector;
		((Atanorlvector*)context)->values.push_back(a->Long());
		addvalue = a_lvector;
		return context;
	case a_float:
		context = globalAtanor->Providefvector();
		((Atanorfvector*)context)->values.push_back(a->Float());
		addvalue = a_fvector;
		return context;
	case a_string:
		context = globalAtanor->Providesvector();
		((Atanorsvector*)context)->values.push_back(a->String());
		addvalue = a_svector;
		return context;
	case a_ustring:
		context = globalAtanor->Provideuvector();
		((Atanoruvector*)context)->values.push_back(a->UString());
		addvalue = a_uvector;
		return context;
	case a_decimal:
		context = new Atanordvector;
		((Atanordvector*)context)->values.push_back(a->Decimal());
		addvalue = a_dvector;
		return context;
	default:
		if (a->isMapContainer()) {
			context = globalAtanor->Providemap();
			context->Pushinmap(a, idthread);
			addvalue = a_map;
		}
		else {
			context = globalAtanor->Providevector();
			context->Push(a);
			addvalue = a_vector;
		}
	}
	return context;
}
//------------------------------------------------------------

#ifdef WIN32
bool AtanorCode::Loadlibrary(string n, string& library_name) {
	if (n == "")
		return false;

	string name = NormalizeFileName(n);
	HINSTANCE LoadMe;
	string lname = name;
	int pos = lname.find_last_of(SEP);
	string subname = lname;
	if (pos != -1) {
		subname = lname.substr(pos + 1, lname.size() - pos - 1);
		pos = subname.find(".");
		if (pos != -1)
			subname = subname.substr(0, pos);
	}

	string moduleinitname = subname;
	moduleinitname += "_InitialisationModule";

	//If it has already been loaded, we return...
	library_name = lname;
	AtanorExternalModule LibEntryPoint;

	if (globalAtanor->RecordExternalLibrary(moduleinitname, NULL) == false) {
		LibEntryPoint = globalAtanor->externalLibraries[moduleinitname];
		(*LibEntryPoint)(globalAtanor, AtanorVersion());
		return true;
	}

	LoadMe = LoadLibraryA(STR(lname));
	if (LoadMe == 0) {
		string atanlib;
		if (Getenv("ATANORLIBS") != NULL)
			atanlib = Getenv("ATANORLIBS");
		else {
			stringstream message;
			message << "Please set ATANORLIBS: " << name;
			throw new AtanorRaiseError(message, filename, current_start, current_end);
		}

		if (Getenv("PATH") != NULL) {
			string path = "Path=";
			path += atanlib;
			path += ";";
			path += Getenv("PATH");
			_putenv(STR(path));
		}

		atanlib += "\\";
		atanlib += subname;
		atanlib += ".dll";
		atanlib = NormalizeFileName(atanlib);
		library_name = atanlib;
		LoadMe = LoadLibraryA(STR(atanlib));
	}
	// Check to see if the library was loaded successfully 

	if (LoadMe == 0) {
		DWORD err = GetLastError();
		stringstream message;
		message << "Cannot load library: " << name;
		throw new AtanorRaiseError(message, filename, current_start, current_end);
	}

	LibEntryPoint = (AtanorExternalModule)GetProcAddress(LoadMe, STR(moduleinitname));

	if (LibEntryPoint == NULL) {
		stringstream message;
		message << "No entry point in this library: " << name;
		throw new AtanorRaiseError(message, filename, current_start, current_end);
	}

	globalAtanor->RecordExternalLibrary(moduleinitname, LibEntryPoint);

	if ((*LibEntryPoint)(globalAtanor, AtanorVersion()) == false) {
		stringstream message;
		message << "Wrong version: " << name;
		throw new AtanorRaiseError(message, filename, current_start, current_end);
	}

	return true;
}
#else
#ifdef KIFNEEDCLOSELIBS
vector<void*> ATANORLIBSTOCLOSE;

void AtanorCloseLibraries() {
	for (int x = 0; x<ATANORLIBSTOCLOSE.size(); x++)
		dlclose(ATANORLIBSTOCLOSE[x]);
}
#endif

bool AtanorCode::Loadlibrary(string n, string& library_name) {
	string name = NormalizeFileName(n);
	void* LoadMe;

	char lname[4096];
	strcpy(lname, STR(name));
	char* error;
	char* pt = strrchr(lname, '/');
	char buff[4096];
	bool addso = true;
	string basename;
	if (strstr(lname + strlen(lname) - 3, ".so"))
		addso = false;
	if (pt != NULL) {
		if (memcmp(pt + 1, "lib", 3)) {
			if (addso)
				sprintf(buff, "lib%s.so", pt + 1);
			else
				sprintf(buff, "lib%s", pt + 1);
		}
		else {
			if (addso)
				sprintf(buff, "%s.so", pt + 1);
			else
				strcpy(buff, pt + 1);
		}
		basename = buff;
		strcpy(pt + 1, buff);
	}
	else {
		if (memcmp(lname, "lib", 3)) {
			if (addso)
				sprintf(buff, "lib%s.so", lname);
			else
				sprintf(buff, "lib%s", lname);
		}
		else {
			if (addso)
				sprintf(buff, "%s.so", lname);
			else
				strcpy(buff, lname);
		}
		basename = buff;
		strcpy(lname, buff);
	}

	FileNameNomalization(buff, lname, 4096);
	strcpy(lname, buff);
	string subname = basename.substr(3, basename.size() - 6);

	string moduleinitname = subname;
	moduleinitname += "_InitialisationModule";

	AtanorExternalModule LibEntryPoint;

	library_name = lname;
	//If it has already been loaded, we return...
	if (globalAtanor->RecordExternalLibrary(moduleinitname, NULL) == false) {
		LibEntryPoint = globalAtanor->externalLibraries[moduleinitname];
		(*LibEntryPoint)(globalAtanor,AtanorVersion());
		return true;
	}

	LoadMe = dlopen(lname, RTLD_LAZY);
	if (LoadMe == NULL) {
		string atanlib;
		if (Getenv("ATANORLIBS") != NULL)
			atanlib = Getenv("ATANORLIBS");
		else {
			stringstream message;
			message << "Please set ATANORLIBS: " << name;
			throw new AtanorRaiseError(message, filename, current_start, current_end);
		}

		string ldlibpath;
		if (Getenv("LD_LIBRARY_PATH") != NULL) {
			ldlibpath = Getenv("LD_LIBRARY_PATH");
			ldlibpath = ":" + ldlibpath;
		}

		ldlibpath = atanlib + ldlibpath;
		setenv("LD_LIBRARY_PATH", ldlibpath.c_str(), 1);

		ldlibpath = "";
		if (Getenv("PATH") != NULL) {
			ldlibpath = Getenv("PATH");
			ldlibpath = ":" + ldlibpath;
		}
		ldlibpath = atanlib + ldlibpath;
		setenv("PATH", ldlibpath.c_str(), 1);


		atanlib += "/";
		atanlib += basename;
		atanlib = NormalizeFileName(atanlib);
		library_name = atanlib;
		LoadMe = dlopen(STR(atanlib), RTLD_LAZY);
	}

	// Check to see if the library was loaded successfully 

	if (LoadMe == NULL) {
		error = dlerror();
		stringstream message;
		message << error << ": " << lname;
		throw new AtanorRaiseError(message, filename, current_start, current_end);
	}

#ifdef KIFNEEDCLOSELIBS
	ATANORLIBSTOCLOSE.push_back(LoadMe);
#endif


	LibEntryPoint = (AtanorExternalModule)dlsym(LoadMe, STR(moduleinitname));

	if ((error = dlerror()) != NULL) {
		stringstream message;
		message << error << ": " << name;
		throw new AtanorRaiseError(message, filename, current_start, current_end);
	}

	globalAtanor->RecordExternalLibrary(moduleinitname, LibEntryPoint);

	if ((*LibEntryPoint)(globalAtanor,AtanorVersion()) == false) {
		stringstream message;
		message << "Wrong version: " << name;
		throw new AtanorRaiseError(message, filename, current_start, current_end);
	}

	return true;
}
#endif



