
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
filename   : atanorrawstring.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "codeparse.h"
#include "atanor.h"
#include "atanorrawstring.h"
#include "constobjects.h"
#include "atanorivector.h"
#include "atanorfvector.h"
#include "atanorbvector.h"
#include "atanorsvector.h"
#include "automate.h"
#include "atanorvector.h"
#include "atanorconstants.h"
#include "atanorbyte.h"
#include "atanorustring.h"
#include "fractalhmap.h"
#include "instructions.h"
#include "atanormap.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, rawstringMethod>  Atanorrawstring::methods;
Exporting hmap<string, string> Atanorrawstring::infomethods;
Exporting bin_hash<unsigned long> Atanorrawstring::exported;

Exporting short Atanorrawstring::idtype = 0;

#ifdef DOSOUTPUT
static bool dosoutput = true;
static void setdosoutput(bool d) { dosoutput = d; }
#define conversion2Dos(x) dosoutput?s_utf8_to_dos(USTR(x)):x
#endif


char RawStringIndexes(char* svalue, long sz, Atanor* index, int& ileft, int& iright, int idthread) {
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

	long subsize = -1;

	char* strleft;
	int offset = -1;

	if (left->isString()) {
		wstring sub = left->UString();
		//then we are looking for a substring
		if (sleft)
			strleft = strrstr(svalue, STR(sub), sz, sub.size());
		else
			strleft = strstr(svalue, STR(sub));

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


	left->Release();
	//We return as a non interval
	if (right == NULL) {
		if (subsize != -1) {//unless the left index was a string...
			iright = ileft + subsize;
			return 2;
		}
		return 1;
	}

	//We return as a non interval
	if (right == NULL) {
		if (offset != -1) {//unless the left index was a string...
			iright = ileft + offset;
			return 2;
		}
		return 1;
	}

	if (right->isString()) {
		char* strright;
		string sub(right->String());
		if (sright)
			strright = strrstr(svalue, STR(sub), sz, sub.size());
		else {
			if (subsize != -1)
				strright = strstr(svalue + ileft + subsize, STR(sub));
			else
				strright = strstr(svalue + ileft, STR(sub));
		}
		if (strright == NULL)
			return 0;
		iright = (strright - svalue) + sub.size();
	}
	else {
		iright = right->Integer();

		if (iright < 0 || right == aNULL)
			iright += sz;
		else {
			if (ileft != -1)
				iright += ileft;
		}
		if (iright > sz)
			iright = sz;
	}

	if (reright)
		right->Release();

	if ((iright - ileft) <= 0)
		return 0;
	return 2;
}


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorrawstring::AddMethod(AtanorGlobal* global, string name, rawstringMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorrawstring::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorrawstring::idtype = global->Getid("rawstring");

	Atanorrawstring::AddMethod(global, "_initial", &Atanorrawstring::MethodInitial, P_ONE, "_initial(string|int): Set the initial value of the raw string.");

	Atanorrawstring::AddMethod(global, "hash", &Atanorrawstring::MethodHash, P_NONE, "hash(): Return the hash value of a string.");
	Atanorrawstring::AddMethod(global, "reverse", &Atanorrawstring::MethodReverse, P_NONE, "reverse(): reverse the string");
	Atanorrawstring::AddMethod(global, "ord", &Atanorrawstring::MethodOrd, P_NONE, "ord(): return the ASCII code of the first character, or a list of all ASCII code if the recipient is a vector");
	Atanorrawstring::AddMethod(global, "bytes", &Atanorrawstring::MethodBytes, P_NONE, "bytes(): Return the string as a vector of bytes");
	Atanorrawstring::AddMethod(global, "format", &Atanorrawstring::MethodFormat, P_ATLEASTONE, "format(p1,p2,p3): Create a new string from the current string in which each '%x' is associated to one of the parameters, 'x' being the position of that parameter in the argument list. 'x' starts at 1.");
	Atanorrawstring::AddMethod(global, "base", &Atanorrawstring::MethodBase, P_ONE, "base(int b): Return the value corresponding to the string in base b");
	Atanorrawstring::AddMethod(global, "fill", &Atanorrawstring::MethodFill, P_TWO, "fill(int nb,string c): create a string of nb characters c");
	Atanorrawstring::AddMethod(global, "padding", &Atanorrawstring::MethodPadding, P_TWO | P_THREE, "padding(int nb,string c,bool paddattheend): add nb characters c to the current string. Last parameter is optional");
	Atanorrawstring::AddMethod(global, "parse", &Atanorrawstring::MethodParse, P_NONE | P_TWO, "parse(): Parse a string as a piece of code and returns the evaluation as a vector.");
	Atanorrawstring::AddMethod(global, "pop", &Atanorrawstring::MethodPop, P_NONE | P_ONE | P_TWO, "pop(): remove last character");
	Atanorrawstring::AddMethod(global, "buffersize", &Atanorrawstring::MethodSizeb, P_NONE, "buffersize(): Return the size of the internal buffer");
	Atanorrawstring::AddMethod(global, "resize", &Atanorrawstring::MethodResize, P_NONE, "resize(): Redefine the internal buffer size. Cannot be inferior to string size.");
	Atanorrawstring::AddMethod(global, "lisp", &Atanorrawstring::MethodLisp, P_NONE | P_TWO, "lisp(): Parse a string as a parenthetic expressions, which is returned as a vector.|lisp(string o,string c): Parse a string as a parenthetic expressions, where the opening and closing characters are provided.");
	Atanorrawstring::AddMethod(global, "tags", &Atanorrawstring::MethodTags, P_TWO, "tags(string o,string c): Parse a string as a parenthetic expression, where the opening and closing strings are provided.");
	Atanorrawstring::AddMethod(global, "regexip", &Atanorrawstring::MethodRegexip, P_ONE, "regexip(sub): Find the substrings matching sub");
	Atanorrawstring::AddMethod(global, "evaluate", &Atanorrawstring::MethodEvaluate, P_NONE, "evaluate(): evaluate the meta-characters within a string and return the evaluated string.");
	Atanorrawstring::AddMethod(global, "html", &Atanorrawstring::MethodTohtml, P_NONE, "html(): Return the string into an HTML compatible string or as a vector of strings");
	Atanorrawstring::AddMethod(global, "tohtml", &Atanorrawstring::MethodTohtml, P_NONE, "tohtml(): Return the string into an HTML compatible string or as a vector of strings");
	Atanorrawstring::AddMethod(global, "toxml", &Atanorrawstring::MethodToxml, P_NONE, "toxml(): Return the string into an XML compatible string or as a vector of strings");
	Atanorrawstring::AddMethod(global, "replaceregexip", &Atanorrawstring::MethodReplaceregexip, P_TWO, "replaceregexip(sub,str): Replace the substrings matching sub with str");
	Atanorrawstring::AddMethod(global, "levenshtein", &Atanorrawstring::MethodEditdistance, P_ONE | P_TWO, "levenshtein(string s,bool byte): Return the edit distance with 's' according to Levenshtein algorithm. If byte is true, force a byte level comparison. byte is optionnal.");
	Atanorrawstring::AddMethod(global, "editdistance", &Atanorrawstring::MethodEditdistance, P_ONE | P_TWO | P_THREE, "editdistance(string s,bool byte): Return the edit distance with 's'. If byte is true, force a byte level comparison. byte is optionnal.");
	Atanorrawstring::AddMethod(global, "replace", &Atanorrawstring::MethodReplace, P_TWO, "replace(sub,str): Replace the substrings matching sub with str");
	Atanorrawstring::AddMethod(global, "getstd", &Atanorrawstring::MethodGetstd, P_ONE, "getstd(bool): catch or release the standard output");
	Atanorrawstring::AddMethod(global, "geterr", &Atanorrawstring::MethodGeterr, P_ONE, "geterr(bool): catch or release the error output");
	Atanorrawstring::AddMethod(global, "split", &Atanorrawstring::MethodSplit, P_TWO | P_ONE | P_NONE, "split(string splitter,vector vect): split a string along splitter and store the results  in a vector. If splitter=='', then the string is split into a vector of characters");
	Atanorrawstring::AddMethod(global, "splite", &Atanorrawstring::MethodSplite, P_TWO | P_ONE | P_NONE, "splite(string splitter,vector vect): split a string along splitter and store the results  in a vector. If splitter=='', then the string is split into a vector of characters. Empty strings are kept in the result.");
	Atanorrawstring::AddMethod(global, "tokenize", &Atanorrawstring::MethodTokenize, P_NONE | P_ONE | P_TWO | P_THREE, "tokenize(bool comma,bool separator,bool concatenate): Segment a string into words and punctuations. If 'comma' is true, then the decimal character is ',' otherwise it is '.'. If 'separator' is true then '1,000' is accepted as a number. If 'concatenate' is true then '3a' is a valid token.");
	Atanorrawstring::AddMethod(global, "stokenize", &Atanorrawstring::MethodStokenize, P_NONE | P_ONE, "stokenize(map keeps): Segment a string into words and punctuations, with a keep.");
	Atanorrawstring::AddMethod(global, "count", &Atanorrawstring::MethodCount, P_THREE | P_TWO | P_ONE | P_NONE, "count(string sub,int pos,int mx): Count the number of substrings starting at position pos, ending at mx");
	Atanorrawstring::AddMethod(global, "find", &Atanorrawstring::MethodFind, P_TWO | P_ONE, "find(string sub,int pos): Return the position of substring sub starting at position pos");
	Atanorrawstring::AddMethod(global, "rfind", &Atanorrawstring::MethodRfind, P_TWO | P_ONE, "rfind(string sub,int pos): Return the position of substring sub backward starting at position pos");
	Atanorrawstring::AddMethod(global, "removefirst", &Atanorrawstring::MethodRemovefirst, P_ONE, "removefirst(int nb): remove the first nb characters of a string");
	Atanorrawstring::AddMethod(global, "removelast", &Atanorrawstring::MethodRemovelast, P_ONE, "removelast(int nb): remove the last nb characters of a string");
	Atanorrawstring::AddMethod(global, "isutf8", &Atanorrawstring::MethodIsutf8, P_NONE, "isutf8(): Return true is the string is encoded in UTF8");
	Atanorrawstring::AddMethod(global, "ngrams", &Atanorrawstring::MethodNgrams, P_ONE | P_TWO, "ngrams(int r): Return a vector of all ngrams of rank r");
	Atanorrawstring::AddMethod(global, "utf8", &Atanorrawstring::MethodUtf8, P_NONE | P_ONE, "utf8(int table): convert a LATIN string into UTF8. table is optional, by default it is ISO/IEC 8859 part 1.");
	Atanorrawstring::AddMethod(global, "latin", &Atanorrawstring::MethodLatin, P_NONE, "latin(): convert an UTF8 string in LATIN");
	Atanorrawstring::AddMethod(global, "dos", &Atanorrawstring::MethodDos, P_NONE, "dos(): convert a string into DOS encoding");
	Atanorrawstring::AddMethod(global, "dostoutf8", &Atanorrawstring::MethodDostoutf8, P_NONE, "dostoutf8(): convert a DOS string into UTF8");
	Atanorrawstring::AddMethod(global, "left", &Atanorrawstring::MethodLeft, P_ONE, "left(int nb): return the first nb characters of a string");
	Atanorrawstring::AddMethod(global, "byteposition", &Atanorrawstring::MethodByteposition, P_ONE, "byteposition(int pos): Convert a character position into a byte position");
	Atanorrawstring::AddMethod(global, "charposition", &Atanorrawstring::MethodCharposition, P_ONE, "charposition(int pos): Convert a byte position into a character position");
	Atanorrawstring::AddMethod(global, "right", &Atanorrawstring::MethodRight, P_ONE, "right(int nb): return the last nb characters of a string");
	Atanorrawstring::AddMethod(global, "mid", &Atanorrawstring::MethodMid, P_TWO, "mid(int pos,int nb): return the nb characters starting at position pos of a string");
	Atanorrawstring::AddMethod(global, "isalpha", &Atanorrawstring::MethodIsalpha, P_NONE, "isalpha(): Test if a string only contains only alphabetical characters");
	Atanorrawstring::AddMethod(global, "isconsonant", &Atanorrawstring::MethodIsconsonant, P_NONE, "isconsonant(): Test if a string only contains consonants");
	Atanorrawstring::AddMethod(global, "isvowel", &Atanorrawstring::MethodIsvowel, P_NONE, "isvowel(): Test if a string only contains only vowels");
	Atanorrawstring::AddMethod(global, "ispunctuation", &Atanorrawstring::MethodIspunctuation, P_NONE, "ispunctuation(): Test if the character is a punctuation");
	Atanorrawstring::AddMethod(global, "isdigit", &Atanorrawstring::MethodIsdigit, P_NONE, "isdigit(): Test if a string only contains digits");
	Atanorrawstring::AddMethod(global, "extract", &Atanorrawstring::MethodExtract, P_ATLEASTTHREE, "extract(int pos,string from,string up1,string up2...): extract substrings between 'from' and 'up1'...'upn' (the shortest string is used). Return a vector of strings");
	Atanorrawstring::AddMethod(global, "isupper", &Atanorrawstring::MethodIsupper, P_NONE, "isupper(): Test if a string only contains uppercase characters");
	Atanorrawstring::AddMethod(global, "islower", &Atanorrawstring::MethodIslower, P_NONE, "islower(): Test if a string only contains lowercase characters");
	Atanorrawstring::AddMethod(global, "upper", &Atanorrawstring::MethodUpper, P_NONE, "upper(): Return the string in upper characters");
	Atanorrawstring::AddMethod(global, "deaccentuate", &Atanorrawstring::MethodDeaccentuate, P_NONE, "deaccentuate(): Remove the accents from accented characters");
	Atanorrawstring::AddMethod(global, "lower", &Atanorrawstring::MethodLower, P_NONE, "lower(): Return the string in lower characters");
	Atanorrawstring::AddMethod(global, "trim", &Atanorrawstring::MethodTrim, P_NONE, "trim(): remove the trailing characters");
	Atanorrawstring::AddMethod(global, "trimleft", &Atanorrawstring::MethodTrimleft, P_NONE, "trimleft(): remove the trailing characters on the left");
	Atanorrawstring::AddMethod(global, "trimright", &Atanorrawstring::MethodTrimright, P_NONE, "trimright(): remove the trailing characters on the right");
	Atanorrawstring::AddMethod(global, "last", &Atanorrawstring::MethodLast, P_NONE, "last(): return last character");
	Atanorrawstring::AddMethod(global, "insert", &Atanorrawstring::MethodInsert, P_ONE | P_TWO, "insert(i,s): insert the string s at i. If i is -1, then insert s between each character in the input string");
	Atanorrawstring::AddMethod(global, "clear", &Atanorrawstring::MethodClear, P_NONE, "clear(): Clean the content of a string.");


	global->newInstance[Atanorrawstring::idtype] = new Atanorrawstring(global);
	global->RecordMethods(Atanorrawstring::idtype,Atanorrawstring::exported);

	return true;
}


AtanorIteration* Atanorrawstring::Newiteration(bool direction) {
	
	if (value==NULL)
		return new AtanorIterationrawstring("", direction);

	return new AtanorIterationrawstring((char*)value, direction);
}


Atanor* Atanorrawstring::in(Atanor* context, Atanor* a, short idthread) {
	//Three cases along the container type...
	//It is a Boolean, we expect false or true
	//It is an integer, we expect a position in v
	//It is a container, we are looking for all positions...

	string val = a->String();
	
	if (context->isBoolean()) {
		if (value == NULL)
			return aFALSE;

		if (strstr((char*)value, STR(val)) != NULL)
			return aTRUE;
		return aFALSE;
	}

	if (context->isVectorContainer()) {
		Atanorivector* v = (Atanorivector*)Selectaivector(context);
		if (value == NULL)
			return v;

		string s = (char*)value;
		s_findall(s, val, v->values);
		return v;
	}

	long r = s_find(value, val, 0);
	return globalAtanor->Provideint(r);
}

unsigned int Atanorrawstring::EditDistance(Atanor* e) {
	
	string s2 = e->String();
	unsigned int s1len, s2len, x, y, lastdiag, olddiag;
	s1len = stringsize;
	s2len = s2.size();
	if (stringsize == 0)
		return s2len;

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

Atanor* Atanorrawstring::Put(Atanor* idx, Atanor* ke, short idthread) {
	if (ke == this)
		return aTRUE;


	string s = ke->String();

	if (!idx->isIndex()) {
		if (value == NULL) {
			stringsize = s.size();
			buffersize = stringsize + 1;
			value = new uchar[buffersize];
			strcpy_s((char*)value, buffersize, STR(s));
			return aTRUE;
		}

		if (buffersize <= s.size())
			return globalAtanor->Returnerror("Stack overflow", idthread);

		strcpy((char*)value, STR(s));
		stringsize = s.size();
	}
	else {	
		if (value == NULL) {
			if (globalAtanor->erroronkey)
				return globalAtanor->Returnerror("Wrong key in a container or a string access", idthread);
			return aFALSE;
		}

		int ileft = -1, iright = -1;
		char res = RawStringIndexes((char*)value, stringsize, idx, ileft, iright, idthread);

		if (res == 0) {
			if (globalAtanor->erroronkey)
				return globalAtanor->Returnerror("Wrong key in a container or a string access", idthread);
			return aFALSE;
		}

		if (res == 1)
			iright = ileft + 1;

		//the size is:
		int computesize = stringsize - (iright - ileft) + s.size();
		if (computesize > buffersize)
			return globalAtanor->Returnerror("Stack overflow", idthread);

		stringsize = computesize;
		s += (char*)value + iright;
		value[ileft] = 0;
		strcat((char*)value, STR(s));
		return aTRUE;
	}
	return aTRUE;
}

Atanor* Atanorrawstring::Get(Atanor* context, Atanor* idx, short idthread) {
	if (!idx->isIndex() || context == idx)
		return this;

	if (value == NULL)
		return aNULL;

	int ileft = -1, iright = -1;

	//char res = StringIndexes(v, idx, dom, ileft, iright, idthread, callfunc);
	char res = RawStringIndexes((char*)value, stringsize, idx, ileft, iright, idthread);

	if (res == 0) {
		if (globalAtanor->erroronkey)
			return globalAtanor->Returnerror("Wrong key in a container or a string access", idthread);
		return aNOELEMENT;
	}

	if (res == 1) {
		char v[] = { (char)value[ileft], 0 };
		return globalAtanor->Providestring(v);
	}

	if (iright == stringsize)
		return globalAtanor->Providestring((char*)value + ileft);

	uchar c = value[iright];
	value[iright] = 0;
	idx = globalAtanor->Providestring((char*)value + ileft);
	value[iright] = c;
	return idx;
}

//------------------------------------------------------------------------------------------------------------------
Atanor* Atanorrawstring::MethodOrd(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	wstring s = UString();
	if (s.size() >= 1) {
		if (contextualpattern->isNumber())
			return globalAtanor->Provideint((int)s[0]);

		if (contextualpattern->isVectorContainer() || s.size() > 1) {
			Atanor* kvect = Selectaivector(contextualpattern);
			if (kvect->Type() == a_ivector) {
				Locking _lock((AtanorObject*)kvect);
				for (int i = 0; i < s.size(); i++)
					((Atanorivector*)kvect)->values.push_back((long)s[i]);
			}
			else {
				AtanorConstInt loc(0);
				for (int i = 0; i < s.size(); i++) {
					loc.value = (long)s[i];
					kvect->Push(&loc);
				}
			}
			return kvect;
		}

		return globalAtanor->Provideint((int)s[0]);
	}

	return aNULL;
}

//------------------------------------------------------------------------------------------------------------------
Atanor* Atanorrawstring::MethodIsutf8(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {	
	if (value == NULL)
		return aFALSE;

	if (s_is_utf8(value, stringsize) == true)
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorrawstring::MethodEvaluate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	static string reg("[\\\\[%d%d%d,x%x%x,u%X%X%X%X,?],&[%#%d(%d)(%d)(%d);,[%c,%C]+;],%%%X%X,u%X%X%X%X]");
	static vector<int> vectr;
	static automate metacodes(USTR(reg), (void*)1);

	if (value == NULL)
		return this;

	string sub = (char*)value;

	metacodes.rechercheindexes(USTR(sub), vectr);

	if (vectr.size() == 0)
		return globalAtanor->Providestring(sub);

	size_t i = 0, previous = 0;
	string thestr;
	string s;
	for (i = 0; i < vectr.size(); i += 2) {
		if (previous < vectr[i])
			thestr += sub.substr(previous, vectr[i] - previous);
		previous = vectr[i] + vectr[i + 1];
		s = sub.substr(vectr[i], vectr[i + 1]);
		s_EvaluateMetaCharacters(s);
		thestr += s;
	}

	i = sub.size() - previous;
	if (i) {
		s = sub.substr(previous, i);
		thestr += s;
	}

	return globalAtanor->Providestring(thestr);
}

string replacestringwithautomaton(string &str, string& reg, string& rep);
Atanor* Atanorrawstring::MethodReplaceregexip(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//Replace an element with another

	string str = String();
	string reg = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	string rep = callfunc->Evaluate(1, contextualpattern, idthread)->String();

	string res = replacestringwithautomaton(str, reg, rep);
	return globalAtanor->Providestring(res);
}


void XNBrowse(x_node* xn, Atanor* kf);
Atanor* Atanorrawstring::MethodParse(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	string str = String();
	x_readstring xr(str);
	xr.loadtoken();
	bnf_atanor bnf;
	x_reading* previous = bnf.fx;
	x_node* xn = bnf.x_parsing(&xr, FULL, false);
	string message;
	if (xn == NULL) {
		if (bnf.errornumber != -1) {
			string msg("EVL(100):");
			msg += bnf.x_errormsg(bnf.errornumber);
			char ch[20];
			sprintf_s(ch, 20, " (line:%d)", bnf.lineerror);
			msg += ch;
			return globalAtanor->Returnerror(msg, idthread);
		}
		return globalAtanor->Returnerror("Unknown expression", idthread);
	}

	Atanor* kvect = Selectavector(contextualpattern);
	XNBrowse(xn, kvect);
	return kvect;
}

Atanor* Atanorrawstring::MethodLisp(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	string str = String();
	string copen("(");
	string cclose(")");
	if (callfunc->Size() == 0)
		return globalAtanor->EvaluateLisp(str, copen, cclose, idthread);

	copen = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	cclose = callfunc->Evaluate(1, contextualpattern, idthread)->String();

	if (copen.size() != 1 || cclose.size() != 1)
		return globalAtanor->Returnerror("KIF(831): Wrong opening or closing character in LISP", idthread);
	return globalAtanor->EvaluateLisp(str, copen, cclose, idthread);
}

Atanor* Atanorrawstring::MethodTags(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring str(String());
	string copen = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	string cclose = callfunc->Evaluate(1, contextualpattern, idthread)->String();
	return globalAtanor->EvaluateTags(str, copen, cclose, idthread);
}

Atanor* Atanorrawstring::MethodRegexip(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	agnostring str(String());
	bool utf8 = str.isutf8();
	string reg = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	automate a(USTR(reg), (void*)1);
	if (contextualpattern->isVectorContainer()) {
		Atanor* vect;

		vect = Selectavector(contextualpattern);

		switch (vect->Type()) {
		case a_ivector:
			a.rechercheindexes(USTR(str), ((Atanorivector*)vect)->values);
			if (utf8)
				convertbytepositiontochar(((Atanorivector*)vect)->values, str);
			return vect;
		case a_fvector:
			a.rechercheindexes(USTR(str), ((Atanorfvector*)vect)->values);
			if (utf8)
				convertbytepositiontochar(((Atanorfvector*)vect)->values, str);
			return vect;
		case a_svector:
			a.rechercheavecextractiondansvecteur(USTR(str), ((Atanorsvector*)vect)->values);
			return vect;
		}

		if (vect->Type() == Atanorbvector::idtype) {
			vector<long> vectr;
			a.rechercheindexes(USTR(str), vectr);
			if (utf8)
				convertbytepositiontochar(vectr, str);
			for (int i = 0; i < vectr.size(); i++)
				((Atanorbvector*)vect)->values.push_back((uchar)vectr[i]);
			return vect;
		}

		vector<string> vectr;
		a.rechercheavecextractiondansvecteur(USTR(str), vectr);
		AtanorConstString localstring("");
		for (int iv = 0; iv < vectr.size(); iv++) {
			localstring.value = vectr[iv];
			vect->Push(&localstring);
		}
		return vect;
	}

	if (contextualpattern->Type() == a_int || contextualpattern->Type() == a_float || contextualpattern->Type() == a_string) {
		reg = "";
		int res = a.rechercheavecextraction(USTR(str), reg);
		if (contextualpattern->Type() == a_string)
			return globalAtanor->Providestring(reg);
		return globalAtanor->Provideint(res);
	}

	etat* e = a.recherche(USTR(str));
	if (e == NULL)
		return aFALSE;
	return aTRUE;
}

Atanor* Atanorrawstring::MethodCount(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	int i = 0, nb = 0;

	int mx = -1;
	if (callfunc->Size() >= 2) {
		i = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
		if (callfunc->Size() == 3)
			mx = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
	}

	agnostring str(String());
	if (str == "")
		return aZERO;

	string sub = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	i = str.find(sub, i);
	while (i != -1) {
		if (mx != -1 && i >= mx)
			break;
		nb++;
		i++;
		i = str.find(sub, i);
	}

	return globalAtanor->Provideint(nb);
}

Atanor* Atanorrawstring::MethodFind(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* ksub = callfunc->Evaluate(0, contextualpattern, idthread);

	//we search for a substring starting at position idx
	int i = 0;
	if (callfunc->Size() == 2)
		i = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();

	string str = String();

	if (contextualpattern->isVectorContainer()) {
		Atanor* kvect = Selectavector(contextualpattern);
		int j;
		
		if (str == "")
			return kvect;

		string sub = ksub->String();
		vector<long> v;
		s_findall(str, sub, v);

		int ps = 0, nb = 0;
		for (j = 0; j < v.size(); j++)
			v[j] = c_bytetocharpositionidx(USTR(str), v[j], nb, ps);

		if (v.size()) {
			Locking _lock((AtanorObject*)kvect);

			switch (kvect->Type()) {
			case a_bvector:
				for (j = 0; j < v.size(); j++) {
					if (v[j] >= i)
						((Atanorbvector*)kvect)->values.push_back((uchar)v[j]);
				}
				return kvect;
			case a_ivector:
				for (j = 0; j < v.size(); j++) {
					if (v[j] >= i)
						((Atanorivector*)kvect)->values.push_back((uchar)v[j]);
				}
				return kvect;
			case a_fvector:
				for (j = 0; j < v.size(); j++) {
					if (v[j] >= i)
						((Atanorfvector*)kvect)->values.push_back((double)v[j]);
				}
				return kvect;
			}

			AtanorConstInt loc(0);
			for (j = 0; j < v.size(); j++) {
				if (v[j] >= i) {
					loc.value = v[j];
					kvect->Push(&loc);
				}
			}
		}

		return kvect;
	}
	
	string sub = ksub->String();

	i = s_find(str, sub, i);

	if (i == string::npos)
		return aMINUSONE;

	return globalAtanor->Provideint(i);
}

Atanor* Atanorrawstring::MethodRfind(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* ksub = callfunc->Evaluate(0, contextualpattern, idthread);

	//we search for a substring starting at position idx
	int i = 0;
	if (callfunc->Size() == 2)
		i = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();

	string str = String();
	if (contextualpattern->isVectorContainer()) {
		Atanor* kvect = Selectavector(contextualpattern);
		int j;
				
		if (str == "")
			return kvect;

		string sub = ksub->String();
		vector<long> v;
		s_findall(str, sub, v);

		int ps = 0, nb = 0;
		for (j = 0; j < v.size(); j++)
			v[j] = c_bytetocharpositionidx(USTR(str), v[j], nb, ps);

		if (v.size()) {
			Locking _lock((AtanorObject*)kvect);

			switch (kvect->Type()) {
			case a_bvector:
				for (j = 0; j < v.size(); j++) {
					if (v[j] >= i)
						((Atanorbvector*)kvect)->values.push_back((uchar)v[j]);
				}
				return kvect;
			case a_ivector:
				for (j = 0; j < v.size(); j++) {
					if (v[j] >= i)
						((Atanorivector*)kvect)->values.push_back((uchar)v[j]);
				}
				return kvect;
			case a_fvector:
				for (j = 0; j < v.size(); j++) {
					if (v[j] >= i)
						((Atanorfvector*)kvect)->values.push_back((double)v[j]);
				}
				return kvect;
			}

			AtanorConstInt loc(0);
			
			for (j = 0; j < v.size(); j++) {
				if (v[j] >= i) {
					loc.value = v[j];
					kvect->Push(&loc);
				}
			}
		}

		return kvect;
	}


	string sub = ksub->String();
	i = s_rfind(str, sub, i);

	if (i == string::npos)
		return aMINUSONE;

	return globalAtanor->Provideint(i);
}

Atanor* Atanorrawstring::MethodByteposition(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring s(String());
	return globalAtanor->Provideint(s.chartobyteposition(callfunc->Evaluate(0, contextualpattern, idthread)->Integer()));
}

Atanor* Atanorrawstring::MethodCharposition(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring s(String());
	return globalAtanor->Provideint(s.bytetocharposition(callfunc->Evaluate(0, contextualpattern, idthread)->Integer()));
}

Atanor* Atanorrawstring::MethodTokenize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	
	agnostring thestr(String());
	bool comma = false;
	bool separator = false;
	bool keepwithdigit = false;
	if (callfunc->Size() >= 1) {
		comma = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
		if (callfunc->Size() >= 2) {
			separator = callfunc->Evaluate(1, contextualpattern, idthread)->Boolean();
			if (callfunc->Size() == 3)
				keepwithdigit = callfunc->Evaluate(2, contextualpattern, idthread)->Boolean();
		}
	}

	vector<string> v;
	thestr.tokenize(v, comma, separator, keepwithdigit);

	Atanor* kvect = Selectasvector(contextualpattern);
	if (kvect->Type() == Atanorsvector::idtype) {
		Locking _lock((AtanorObject*)kvect);
		((Atanorsvector*)kvect)->values = v;
	}
	else {
		AtanorConstString localstring("");

		for (int i = 0; i < v.size(); i++) {
			localstring.value = v[i];
			kvect->Push(&localstring);
		}
	}
	return kvect;
}

Atanor* Atanorrawstring::MethodStokenize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	agnostring thestr(String());
	Atanor* keeps = NULL;

	if (callfunc->Size() == 1) {
		keeps = callfunc->Evaluate(0, contextualpattern, idthread);
		if (!keeps->isContainer())
			return globalAtanor->Returnerror("KIF(665): Expecting a container in STOKENIZE", idthread);
	}

	vector<string> v;
	map<string, bool> k;
	if (keeps != NULL) {
		AtanorIteration* itr = (AtanorIteration*)keeps->Newiteration(false);
		for (itr->Begin(); itr->End() == aFALSE; itr->Next())
			k[itr->Valuestring()] = true;
		itr->Release();
	}
	thestr.stokenize(v, k);

	Atanor* kvect = Selectasvector(contextualpattern);
	if (kvect->Type() == Atanorsvector::idtype) {
		Locking _lock((AtanorObject*)kvect);
		((Atanorsvector*)kvect)->values = v;
	}
	else {
		AtanorConstString localstring("");

		for (int i = 0; i < v.size(); i++) {
			localstring.value = v[i];
			kvect->Push(&localstring);
		}
	}
	return kvect;
}

Atanor* Atanorrawstring::MethodSplit(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	//Third parameter can be a vector

	Atanor* kvect = NULL;

	string localvalue;
	string thestr =  String();
	string thesplitter;
	if (callfunc->Size() != 0)
		thesplitter = callfunc->Evaluate(0, contextualpattern, idthread)->String();

	//First parameter is a string to split
	if (callfunc->Size() == 2)
		kvect = callfunc->Evaluate(1, contextualpattern, idthread);
	else {
		if (contextualpattern->Type() == Atanorsvector::idtype || !contextualpattern->isVectorContainer())
			kvect = Selectasvector(contextualpattern);
		else {
			kvect = Selectacontainer(contextualpattern, idthread);
		}
	}

	if (callfunc->Size() == 0) {
		unsigned char c;
		//we split along space characters...
		for (int i = 0; i < thestr.size(); i++) {
			c = thestr[i];
			if (c <= 32) {
				if (thesplitter.size()) {
					kvect->storevalue(thesplitter);
					thesplitter = "";
				}
			}
			else {
				thesplitter += thestr[i];
			}
		}

		if (thesplitter.size())
			kvect->storevalue(thesplitter);
		return kvect;
	}


	//Second parameter is the splitter string
	if (thesplitter == "") {
		for (int i = 0; i < thestr.size(); i++)
			kvect->storevalue((unsigned char)thestr[i]);
		return kvect;
	}

	size_t pos = 0;
	size_t found = 0;
	while (pos != string::npos) {
		found = s_findbyte(thestr, thesplitter, pos);
		if (found != string::npos) {
			localvalue = thestr.substr(pos, found - pos);
			if (localvalue != "")
				kvect->storevalue(localvalue);
			pos = found + thesplitter.size();
		}
		else
			break;
	}

	localvalue = thestr.substr(pos, thestr.size() - pos);
	if (localvalue != "")
		kvect->storevalue(localvalue);

	return kvect;
}

Atanor* Atanorrawstring::MethodSplite(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	//Third parameter can be a vector

	Atanor* kvect = NULL;

	string localvalue;
	string thestr =  String();
	string thesplitter;
	if (callfunc->Size() != 0)
		thesplitter = callfunc->Evaluate(0, contextualpattern, idthread)->String();

	//First parameter is a string to split
	if (callfunc->Size() == 2)
		kvect = callfunc->Evaluate(1, contextualpattern, idthread);
	else {
		if (contextualpattern->Type() == Atanorsvector::idtype || !contextualpattern->isVectorContainer())
			kvect = Selectasvector(contextualpattern);
		else {
			kvect = Selectacontainer(contextualpattern, idthread);
		}
	}

	if (callfunc->Size() == 0) {
		unsigned char c;
		//we split along space characters...
		for (int i = 0; i < thestr.size(); i++) {
			c = thestr[i];
			if (c <= 32) {
				if (thesplitter.size()) {
					kvect->storevalue(thesplitter);
					thesplitter = "";
				}
			}
			else {
				thesplitter += thestr[i];
			}
		}

		if (thesplitter.size())
			kvect->storevalue(thesplitter);
		return kvect;
	}


	//Second parameter is the splitter string
	if (thesplitter == "") {
		for (int i = 0; i < thestr.size(); i++)
			kvect->storevalue((unsigned char)thestr[i]);
		return kvect;
	}

	size_t pos = 0;
	size_t found = 0;
	while (pos != string::npos) {
		found = s_findbyte(thestr, thesplitter, pos);
		if (found != string::npos) {
			localvalue = thestr.substr(pos, found - pos);
			kvect->storevalue(localvalue);
			pos = found + thesplitter.size();
		}
		else
			break;
	}

	localvalue = thestr.substr(pos, thestr.size() - pos);
	if (localvalue != "")
		kvect->storevalue(localvalue);

	return kvect;
}

Atanor* Atanorrawstring::MethodNgrams(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (value==NULL)
		return Selectasvector(contextualpattern);

	int num = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	if (num <= 0)
		return Selectasvector(contextualpattern);

	Atanor* ke = Selectasvector(contextualpattern);
	int i, j, mx;

	string u;

	string sb((char*)value);
	mx = stringsize - num + 1;
	for (i = 0; i < mx; i++) {
		u = "";
		for (j = i; j < i + num; j++)
			u += sb[j];
		ke->storevalue(u);
	}
	return ke;
}

Atanor* Atanorrawstring::MethodHash(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	static const std::collate<char>& coll = std::use_facet<std::collate<char> >(loc);
	string s =  String();
	if (s == "")
		return aNULL;
	long myhash = coll.hash(s.data(), s.data() + s.length());
	return globalAtanor->Provideint(myhash);
}

Atanor* Atanorrawstring::MethodLatin(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring thestr(String());
	return globalAtanor->Providestring(thestr.utf8tolatin());
}

Atanor* Atanorrawstring::MethodUtf8(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	agnostring thestr(String());
	int table = 0;
	if (callfunc->Size() != 0)
		table = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	return globalAtanor->Providestring(thestr.latintoutf8(table));
}

Atanor* Atanorrawstring::MethodDos(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	agnostring thestr(String());
	return globalAtanor->Providestring(thestr.utf8todos());
}

Atanor* Atanorrawstring::MethodDostoutf8(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	agnostring thestr(String());
	return globalAtanor->Providestring(thestr.dostoutf8());
}

Atanor* Atanorrawstring::MethodLeft(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	
	int nbchar = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	agnostring thestr(String());
	//Second parameter is a position
	return globalAtanor->Providestring(thestr.left(nbchar));
}

Atanor* Atanorrawstring::MethodRight(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	int nbchar = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	agnostring thestr(String());
	return globalAtanor->Providestring(thestr.right(nbchar));
}

Atanor* Atanorrawstring::MethodMid(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string	
	//Second parameter is a position
	long pos = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	//Third parameter is a number of characters
	int nbchar = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
	agnostring thestr(String());
	return globalAtanor->Providestring(thestr.extract(pos, nbchar));
}

Atanor* Atanorrawstring::MethodIslower(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring thestr(String());
	if (thestr.islower())
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorrawstring::MethodIsupper(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring thestr(String());
	if (thestr.isupper())
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorrawstring::MethodIsdigit(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	agnostring thestr(String());
	if (thestr.isdigit())
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorrawstring::MethodIsalpha(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	agnostring thestr(String());
	if (thestr.isalpha())
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorrawstring::MethodIsconsonant(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	agnostring thestr(String());
	if (thestr.isconsonant())
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorrawstring::MethodIsvowel(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	agnostring thestr(String());
	if (thestr.isvowel())
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorrawstring::MethodDeaccentuate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	agnostring thestr(String());
	return globalAtanor->Providestring(thestr.deaccentuate());
}

Atanor* Atanorrawstring::MethodIspunctuation(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	agnostring thestr(String());
	if (thestr.ispunctuation())
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorrawstring::MethodLower(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring thestr(String());
	return globalAtanor->Providestring(thestr.tolower());
}

Atanor* Atanorrawstring::MethodUpper(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring thestr(String());
	return globalAtanor->Providestring(thestr.toupper());
}

Atanor* Atanorrawstring::MethodTrim(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring thestr(String());
	return globalAtanor->Providestring(thestr.trim());
}

Atanor* Atanorrawstring::MethodTrimright(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring thestr(String());
	return globalAtanor->Providestring(thestr.trimright());
}

Atanor* Atanorrawstring::MethodTrimleft(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring thestr(String());
	return globalAtanor->Providestring(thestr.trimleft());
}

Atanor* Atanorrawstring::MethodBase(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	static vector<wstring> caracs;
	static hash_bin<wchar_t, int> mcaracs;
	wstring w;
	long n, b = -1;

	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	//We can reset the chararacter list...
	if (ke->isVectorContainer()) {
		caracs.clear();
		mcaracs.clear();
		b = ke->Size();
		for (n = 0; n < b; n++) {
			w = ke->getustring(n);
			mcaracs[w[0]] = caracs.size();
			caracs.push_back(w);
		}
		if (b)
			return globalAtanor->Provideint(b);
	}

	if (caracs.size() == 0) {
		w = L"0";
		for (n = 0; n < 10; n++) {
			mcaracs[w[0]] = caracs.size();
			caracs.push_back(w);
			w[0]++;
		}
		w = L"A";
		for (n = 10; n < 36; n++) {
			mcaracs[w[0]] = caracs.size();
			caracs.push_back(w);
			w[0]++;
		}
		w = L"a";
		for (n = 36; n < 62; n++) {
			mcaracs[w[0]] = caracs.size();
			caracs.push_back(w);
			w[0]++;
		}
		w = L"#";
		mcaracs[w[0]] = caracs.size();
		caracs.push_back(w);
		w = L"@";
		mcaracs[w[0]] = caracs.size();
		caracs.push_back(w);
		w = L"";
		if (!b)
			return aTRUE;
	}

	b = ke->Integer();

	if (b > caracs.size()) {
		char message[1024];
		sprintf_s(message, 1024, "KIF(510): base too large, max is: %d", caracs.size());
		return globalAtanor->Returnerror(message,idthread);
	}

	w = UString();
	long v = 0;
	wchar_t wc;
	for (n = 0; n < w.size(); n++) {
		wc = w[n];
		if (mcaracs.check(wc) == false || mcaracs[wc] >= b)
			return globalAtanor->Returnerror("KIF(511): Cannot analyze this string in this base.", idthread);

		v *= b;
		v += mcaracs[wc];
	}
	return globalAtanor->Provideint(v);
}

Atanor* Atanorrawstring::MethodPadding(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* res = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* kval = callfunc->Evaluate(1, contextualpattern, idthread);
	bool direction = true;
	if (callfunc->Size() == 3)
		direction = callfunc->Evaluate(2, contextualpattern, idthread)->Boolean();


	string sval = kval->String();

	long sz = res->Integer() - stringsize;
	if (sz <= 0)
		return this;

	string sres;
	if (sval != "" && sval.size() == 1) {
		char* s = (char*)malloc(sz + 1);
		char c = ' ';
		c = sval[0];
		memset(s, c, sz);
		s[sz] = 0;
		sres = s;
		free(s);
	}
	else {
		string s;
		while (sz > 0) {
			s += sval;
			sz--;
		}
		sres = s;
	}
	string v = String();

	if (direction)
		v += sres;
	else
		v = sres + v;

	if (v.size() >= buffersize)
		return globalAtanor->Returnerror("Stack overflow", idthread);

	strcpy_s((char*)value, buffersize, STR(v));
	stringsize = v.size();

	return this;
}

Atanor* Atanorrawstring::MethodFill(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	
	Atanor* res = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* kval = callfunc->Evaluate(1, contextualpattern, idthread);
	int sz = res->Integer();
	
	if (sz >= buffersize)
		sz = buffersize - 1;

	if (sz <= 0)
		return globalAtanor->Providestring("");

	string sval = kval->String();
	
	if (sval != "" && sval.size() == 1) {
		char c = ' ';
		c = sval[0];
		memset((char*)value, c, sz);
		value[sz] = 0;
		stringsize = sz;
	}
	else {
		string s;
		while (sz > 0) {
			s += sval;
			sz -= sval.size();
		}
		strcpy_s((char*)value, buffersize, STR(s));
	}
	return this;
}

Atanor* Atanorrawstring::MethodTohtml(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring s(String());
	string val;
	Atanor* res;
	vector<long> values;
	s.unicodes(values);
	if (contextualpattern->isContainer()) {
		Atanor* kvect = Selectasvector(contextualpattern);
		for (int i = 0; i < values.size(); i++) {
			if (values[i] >= 128 || values[i] == 34 || values[i] == 38 || values[i] == 60 || values[i] == 62)
				c_HTMLcode(val, values[i]);
			else
				val = (char)values[i];
			kvect->storevalue(val);
		}
		return kvect;
	}

	res = globalAtanor->Providestring("");
	for (int i = 0; i < values.size(); i++) {
		if (values[i] >= 128 || values[i] == 34 || values[i] == 38 || values[i] == 60 || values[i] == 62)
			c_HTMLcode(val, values[i]);
		else
			val = (char)values[i];
		((Atanorstring*)res)->value += val;
	}
	return res;
}

Atanor* Atanorrawstring::MethodToxml(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring s(String());
	string val;
	Atanor* res;
	vector<long> values;
	s.unicodes(values);
	if (contextualpattern->isContainer()) {
		Atanor* kvect = Selectasvector(contextualpattern);
		for (int i = 0; i < values.size(); i++) {
			if (values[i] >= 128 || values[i] == 34 || values[i] == 38 || values[i] == 60 || values[i] == 62)
				c_XMLcode(val, values[i]);
			else
				val = (char)values[i];
			kvect->storevalue(val);
		}
		return kvect;
	}

	res = globalAtanor->Providestring("");
	for (int i = 0; i < values.size(); i++) {
		if (values[i] >= 128 || values[i] == 34 || values[i] == 38 || values[i] == 60 || values[i] == 62)
			c_XMLcode(val, values[i]);
		else
			val = (char)values[i];
		((Atanorstring*)res)->value += val;
	}
	return res;
}

Atanor* Atanorrawstring::MethodBytes(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (stringsize >= 1) {
		if (contextualpattern->isNumber()) {
			Atanor* a = contextualpattern->Newinstance(idthread);
			a->storevalue((uchar)value[0]);
			return a;
		}

		if (contextualpattern->isVectorContainer() || stringsize > 1) {
			Atanor* kvect = SelectContainer(contextualpattern, idthread);
			if (kvect == NULL)
				kvect = globalAtanor->Provideivector();

			Locking _lock((AtanorObject*)kvect);
			for (size_t i = 0; i < stringsize; i++)
				kvect->storevalue((uchar)value[i]);
			return kvect;
		}

		return new Atanorbyte(value[0]);
	}

	return aNULL;
}

Atanor* Atanorrawstring::MethodGetstd(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//Reading	
	bool catchoutput = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
	if (catchoutput) {
		if (globalAtanor->kstd != NULL)
			return globalAtanor->Returnerror("KIF(543): Cannot intercept the current standard output", idthread);
		globalAtanor->kstd = this;
		return aTRUE;
	}
	else {
		if (globalAtanor->kstd == this)
			globalAtanor->kstd = NULL;
	}
	return aTRUE;
}

Atanor* Atanorrawstring::MethodGeterr(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//Reading	
	bool catchoutput = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
	if (catchoutput) {
		if (globalAtanor->kstd != NULL)
			return globalAtanor->Returnerror("KIF(544): Cannot intercept the current error output", idthread);
		globalAtanor->kerr = this;
		Setreference();
		return aTRUE;
	}
	else {
		if (globalAtanor->kerr == this) {
			globalAtanor->kerr->Resetreference();
			globalAtanor->kerr = NULL;
		}
	}
	return aTRUE;
}

Atanor* Atanorrawstring::MethodInsert(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	size_t idx;

	//First parameter should be a vector

	//second parameter is anything
	idx = -1;
	string val;
	agnostring ret(String());
	if (callfunc->Size() == 2) {
		idx = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
		val = callfunc->Evaluate(1, contextualpattern, idthread)->String();
	}
	else
		val = callfunc->Evaluate(0, contextualpattern, idthread)->String();

	if (idx == -1)
		ret = ret.insertseparator(val);
	else {
		if (ret.insert(val, idx) == false)
			return aNULL;
	}
	return globalAtanor->Providestring(ret);
}

Atanor* Atanorrawstring::MethodClear(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	Clear();
	return aTRUE;
}

Atanor* Atanorrawstring::MethodExtract(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* pos = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* from = callfunc->Evaluate(1, contextualpattern, idthread);

	vector<string> v;
	int i;
	for (i = 2; i < callfunc->Size(); i++)
		v.push_back(callfunc->Evaluate(i, contextualpattern, idthread)->String());

	string init = String();
	i = pos->Integer();
	string sfrom = from->String();
	i = init.find(sfrom, i);
	int lgfrom = sfrom.size();
	Atanor* vect = Selectasvector(contextualpattern);
	int nb, npos;

	while (i != -1) {
		npos = -1;
		for (int j = 0; j < v.size(); j++) {
			int spos = init.find(v[j], i + lgfrom);
			if (spos != -1 && (npos == -1 || spos < npos))
				npos = spos;
		}
		if (npos == -1)
			break;
		nb = npos - i - lgfrom;
		vect->storevalue(init.substr(i + lgfrom, nb));
		i = init.find(sfrom, npos + 1);
	}

	return vect;
}

Atanor* Atanorrawstring::MethodReverse(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring s(String());
	return globalAtanor->Providestring(s.revert());
}

Atanor* Atanorrawstring::MethodRemovefirst(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* knb = callfunc->Evaluate(0, contextualpattern, idthread);
	int id = knb->Integer();
	agnostring s(String());
	//On retire en tete
	int iv = s.sizec();

	if (iv > 0 && id <= iv) {
		s = s.right(iv - id);
		return globalAtanor->Providestring(s);
	}
	return this;
}

Atanor* Atanorrawstring::MethodRemovelast(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* knb = callfunc->Evaluate(0, contextualpattern, idthread);
	int id = knb->Integer();

	agnostring s(String());
	//On retire en tete
	int iv = s.sizec();

	if (iv > 0 && id <= iv) {
		s = s.left(iv - id);
		return globalAtanor->Providestring(s);
	}
	return this;

}

Atanor* Atanorrawstring::MethodFormat(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	char buffer[101];
	string sformat = String();
	//In this case, we might have more than two parameters...
	string e;
	buffer[0] = '%';
	for (long i = callfunc->Size() - 1; i >= 0; i--) {
		e = callfunc->Evaluate(i, contextualpattern, idthread)->String();
		sprintf_s(buffer + 1, 100, "%d", i + 1);
		sformat = s_replacestrings(sformat, buffer, e);
	}
	return globalAtanor->Providestring(sformat);
}

Atanor* Atanorrawstring::MethodReplace(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//Replace an element with another

	string str = String();
	string reg = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	if (reg == "")
		return globalAtanor->Providestring(str);

	string rep = callfunc->Evaluate(1, contextualpattern, idthread)->String();

	str = s_replacestring(str, reg, rep);
	return globalAtanor->Providestring(str);
}

Atanor* Atanorrawstring::MethodEditdistance(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* e1 = this;
	Atanor* e2 = callfunc->Evaluate(0, contextualpattern, idthread);
	if (e1->Type() == Atanorustring::idtype)
		return globalAtanor->Provideint(e1->EditDistance(e2));

	bool bytecall = false;
	if (callfunc->Size() == 3)
		bytecall = callfunc->Evaluate(1, contextualpattern, idthread)->Boolean();

	if (!bytecall) {
		Atanorustring e(e1->UString());
		return globalAtanor->Provideint(e.EditDistance(e2));
	}

	return globalAtanor->Provideint(e1->EditDistance(e2));
}

Atanor* Atanorrawstring::MethodPop(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter should be a vector

	//second parameter is an integer (the number of pop)
	int i = 0;
	int nb = 1;
	if (callfunc->Size() == 2)
		nb = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
	if (callfunc->Size() >= 1)
		i = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	else
		i = -1;

	while (nb > 0) {
		Pop(i);
		nb--;
	}

	return aTRUE;
}

Atanor* Atanorrawstring::MethodLast(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter should be a vector
	string res(String());
	if (res.size() == 0)
		return aNULL;
	res = c_char_index(res, res.size() - 1);
	return globalAtanor->Providestring(res);
}


#ifdef ATANOR_REGEX


Atanor* Atanorrawstring::MethodReplaceRgx(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	Atanor* kreg = callfunc->Evaluate(0, contextualpattern, idthread);

	string str = String();
	string reg = kreg->String();
	string rep = callfunc->Evaluate(1, contextualpattern, idthread)->String();

	try {
		regex pattern(reg);
		string res = regex_replace(str, pattern, rep);
		return globalAtanor->Providestring(res);
	}
	catch (...) {
		str = "RGX(001):Wrong regular expression: ";
		str += reg;
		return globalAtanor->Returnerror(str,idthread);
	}
}

Atanor* Atanorrawstring::MethodRegex(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	
	Atanor* kreg = callfunc->Evaluate(0, contextualpattern, idthread);


	const sregex_token_iterator end;
	string str = String();
	string reg = kreg->String();
	try {
		regex pattern(reg);


		if (contextualpattern->Type() == Atanorvector::idtype) {
			AtanorConstString localstring("");

			Atanor* vect = Selectasvector(contextualpattern);
			for (sregex_token_iterator i(str.begin(), str.end(), pattern); i != end; ++i) {
				localstring.value = *i;
				vect->Push(&localstring);
			}
			return vect;
		}

		if (contextualpattern->isNumber() || contextualpattern->Type() == idtype) {
			smatch result;
			if (regex_search(str, result, pattern) == true) {
				if (contextualpattern->Type() == idtype)
					return globalAtanor->Providestring(result[0]);
				return globalAtanor->Provideint(result.position());
			}
			return aNULL;
		}

		if (regex_match(str, pattern) == false)
			return aFALSE;
	}
	catch (...) {
		str = "RGX(002):Wrong regular expression: ";
		str += reg;
		return globalAtanor->Returnerror(str, idthread);
	}
	return aTRUE;
}

Atanor* Atanorrawstring::MethodSplitRegex(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {


	Atanor* kreg = callfunc->Evaluate(0, contextualpattern, idthread);

	string str = String();
	string reg = kreg->String();
	try {
		regex pattern(reg);


		if (contextualpattern->Type() == Atanorvector::idtype) {
			AtanorConstString localstring("");

			Atanor* vect = Selectasvector(contextualpattern);
			match_results<string::const_iterator> result;
			if (regex_match(str, result, pattern) == true) {
				for (int i = 1; i < result.size(); i++) {
					localstring.value = result[i];
					vect->Push(&localstring);
				}
			}
			return vect;
		}
	}
	catch (...) {
		str = "RGX(003):Wrong regular expression: ";
		str += reg;
		return globalAtanor->Returnerror(str, idthread);
	}

	return aNULL;
}


#endif





