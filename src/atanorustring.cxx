
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
filename   : atanorustring.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "codeparse.h"
#include "atanor.h"
#include "atanorhaskell.h"
#include "atanorustring.h"
#include "constobjects.h"
#include "atanorivector.h"
#include "atanoruvector.h"
#include "atanorbyte.h"
#include "atanorbvector.h"
#include "atanorlvector.h"
#include "atanorfvector.h"
#include "atanorsvector.h"
#include "automate.h"
#include "fractalhmap.h"
#include "instructions.h"
#include "atanormap.h"


//We need to declare once again our local definitions.
Exporting hmap<unsigned short, ustringMethod>  Atanorustring::methods;
Exporting hmap<string, string> Atanorustring::infomethods;
Exporting bin_hash<unsigned long> Atanorustring::exported;

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifdef UNIX
#define swprintf_s swprintf
#endif

Exporting short Atanorustring::idtype = 0;

#ifdef DOSOUTPUT
static bool dosoutput = true;
static void setdosoutput(bool d) { dosoutput = d; }
#define conversion2Dos(x) dosoutput?s_utf8_to_dos(USTR(x)):x
#endif


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorustring::AddMethod(AtanorGlobal* global, string name, ustringMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorustring::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorustring::idtype = global->Getid("ustring");

	Atanorustring::AddMethod(global, "succ", &Atanorustring::MethodSucc, P_NONE, "succ(): Return the successor of a character.");
	Atanorustring::AddMethod(global, "pred", &Atanorustring::MethodPred, P_NONE, "pred(): Return the predecessor of a byte.");


	Atanorustring::AddMethod(global, "ord", &Atanorustring::MethodOrd, P_NONE, "ord(): Return the character unicode.");
	Atanorustring::AddMethod(global, "hash", &Atanorustring::MethodHash, P_NONE, "hash(): Return the hash value of a string.");
	Atanorustring::AddMethod(global, "reverse", &Atanorustring::MethodReverse, P_NONE, "reverse(): reverse the string");
	Atanorustring::AddMethod(global, "ord", &Atanorustring::MethodOrd, P_NONE, "ord(): return the UNICODE code of the first character, or a list of all UNCODE code if the recipient is a vector");
	Atanorustring::AddMethod(global, "format", &Atanorustring::MethodFormat, P_ATLEASTONE, "format(p1,p2,p3): Create a new string from the current string in which each '%x' is associated to one of the parameters, 'x' being the position of that parameter in the argument list. 'x' starts at 1.");
	Atanorustring::AddMethod(global, "base", &Atanorustring::MethodBase, P_ONE, "base(int b): Return the value corresponding to the string in base b");
	Atanorustring::AddMethod(global, "fill", &Atanorustring::MethodFill, P_TWO, "fill(int nb,string c): create a string of nb characters c");
	Atanorustring::AddMethod(global, "padding", &Atanorustring::MethodPadding, P_TWO | P_THREE, "padding(int nb,string c,bool paddattheend): add nb characters c to the current string. Last parameter is optional");
	Atanorustring::AddMethod(global, "parse", &Atanorustring::MethodParse, P_NONE | P_TWO, "parse(): Parse a string as a piece of code and returns the evaluation as a vector.");
	Atanorustring::AddMethod(global, "pop", &Atanorustring::MethodPop, P_NONE | P_ONE | P_TWO, "pop(): remove last character");
	Atanorustring::AddMethod(global, "sizeb", &Atanorustring::MethodSizeb, P_NONE, "sizeb(): Return the size in bytes of the string");
	Atanorustring::AddMethod(global, "lisp", &Atanorustring::MethodLisp, P_NONE | P_TWO, "lisp(): Parse a string as a parenthetic expressions, which is returned as a vector.|lisp(string o,string c): Parse a string as a parenthetic expressions, where the opening and closing characters are provided.");
	Atanorustring::AddMethod(global, "tags", &Atanorustring::MethodTags, P_TWO, "tags(string o,string c): Parse a string as a parenthetic expression, where the opening and closing strings are provided.");
	Atanorustring::AddMethod(global, "gram", &Atanorustring::MethodGrammar, P_ONE, "gram(sub): Find the substrings matching sub, with the same formalism as grammar");
	Atanorustring::AddMethod(global, "evaluate", &Atanorustring::MethodEvaluate, P_NONE, "evaluate(): evaluate the meta-characters within a string and return the evaluated string.");
	Atanorustring::AddMethod(global, "html", &Atanorustring::MethodTohtml, P_NONE, "html(): Return the string into an HTML compatible string or as a vector of strings");
	Atanorustring::AddMethod(global, "tohtml", &Atanorustring::MethodTohtml, P_NONE, "tohtml(): Return the string into an HTML compatible string or as a vector of strings");
	Atanorustring::AddMethod(global, "toxml", &Atanorustring::MethodToxml, P_NONE, "toxml(): Return the string into an XML compatible string or as a vector of strings");
	Atanorustring::AddMethod(global, "replacegram", &Atanorustring::MethodReplaceGrammar, P_TWO, "replacegram(sub,str): Replace the substrings matching sub with str, with the same formalism as grammar");
	Atanorustring::AddMethod(global, "levenshtein", &Atanorustring::MethodEditdistance, P_ONE | P_TWO, "levenshtein(string s,bool byte): Return the edit distance with 's' according to Levenshtein algorithm. If byte is true, force a byte level comparison. byte is optionnal.");
	Atanorustring::AddMethod(global, "editdistance", &Atanorustring::MethodEditdistance, P_ONE | P_TWO | P_THREE, "editdistance(string s,bool byte): Return the edit distance with 's'. If byte is true, force a byte level comparison. byte is optionnal.");
	Atanorustring::AddMethod(global, "replace", &Atanorustring::MethodReplace, P_TWO, "replace(sub,str): Replace the substrings matching sub with str");
	Atanorustring::AddMethod(global, "multisplit", &Atanorustring::MethodMultiSplit, P_ATLEASTONE, "multisplit(string splitter1,string splitter2..): split a string along different splitters. Return a uvector");	
	Atanorustring::AddMethod(global, "split", &Atanorustring::MethodSplit, P_TWO | P_ONE | P_NONE, "split(string splitter,vector vect): split a string along splitter and store the results  in a vector. If splitter=='', then the string is split into a vector of characters");
	Atanorustring::AddMethod(global, "splite", &Atanorustring::MethodSplite, P_TWO | P_ONE | P_NONE, "splite(string splitter,vector vect): split a string along splitter and store the results  in a vector. If splitter=='', then the string is split into a vector of characters. Empty strings are kept in the result.");
	Atanorustring::AddMethod(global, "tokenize", &Atanorustring::MethodTokenize, P_NONE | P_ONE | P_TWO | P_THREE, "tokenize(bool comma,bool separator,bool concatenate): Segment a string into words and punctuations. If 'comma' is true, then the decimal character is ',' otherwise it is '.'. If 'separator' is true then '1,000' is accepted as a number. If 'concatenate' is true then '3a' is a valid token.");
	Atanorustring::AddMethod(global, "stokenize", &Atanorustring::MethodStokenize, P_NONE | P_ONE, "stokenize(map keeps): Segment a string into words and punctuations, with a keep.");
	Atanorustring::AddMethod(global, "count", &Atanorustring::MethodCount, P_THREE | P_TWO | P_ONE | P_NONE, "count(string sub,int pos,int mx): Count the number of substrings starting at position pos, ending at mx");
	Atanorustring::AddMethod(global, "find", &Atanorustring::MethodFind, P_TWO | P_ONE, "find(string sub,int pos): Return the position of substring sub starting at position pos");
	Atanorustring::AddMethod(global, "rfind", &Atanorustring::MethodRfind, P_TWO | P_ONE, "rfind(string sub,int pos): Return the position of substring sub backward starting at position pos");
	Atanorustring::AddMethod(global, "removefirst", &Atanorustring::MethodRemovefirst, P_ONE, "removefirst(int nb): remove the first nb characters of a string");
	Atanorustring::AddMethod(global, "removelast", &Atanorustring::MethodRemovelast, P_ONE, "removelast(int nb): remove the last nb characters of a string");
	Atanorustring::AddMethod(global, "isutf8", &Atanorustring::MethodIsutf8, P_NONE, "isutf8(): Return true is the string is encoded in UTF8");
	Atanorustring::AddMethod(global, "ngrams", &Atanorustring::MethodNgrams, P_ONE | P_TWO, "ngrams(int r): Return a vector of all ngrams of rank r");
	Atanorustring::AddMethod(global, "latin", &Atanorustring::MethodLatin, P_NONE, "latin(): convert an UTF8 string in LATIN");
	Atanorustring::AddMethod(global, "dos", &Atanorustring::MethodDos, P_NONE, "dos(): convert a string into DOS encoding");
	Atanorustring::AddMethod(global, "dostoutf8", &Atanorustring::MethodDostoutf8, P_NONE, "dostoutf8(): convert a DOS string into UTF8");
	Atanorustring::AddMethod(global, "left", &Atanorustring::MethodLeft, P_ONE, "left(int nb): return the first nb characters of a string");
	Atanorustring::AddMethod(global, "right", &Atanorustring::MethodRight, P_ONE, "right(int nb): return the last nb characters of a string");
	Atanorustring::AddMethod(global, "mid", &Atanorustring::MethodMid, P_TWO, "mid(int pos,int nb): return the nb characters starting at position pos of a string");
	Atanorustring::AddMethod(global, "isalpha", &Atanorustring::MethodIsalpha, P_NONE, "isalpha(): Test if a string only contains only alphabetical characters");
	Atanorustring::AddMethod(global, "isconsonant", &Atanorustring::MethodIsconsonant, P_NONE, "isconsonant(): Test if a string only contains consonants");
	Atanorustring::AddMethod(global, "isvowel", &Atanorustring::MethodIsvowel, P_NONE, "isvowel(): Test if a string only contains only vowels");
	Atanorustring::AddMethod(global, "ispunctuation", &Atanorustring::MethodIspunctuation, P_NONE, "ispunctuation(): Test if the character is a punctuation");
	Atanorustring::AddMethod(global, "isdigit", &Atanorustring::MethodIsdigit, P_NONE, "isdigit(): Test if a string only contains digits");
	Atanorustring::AddMethod(global, "extract", &Atanorustring::MethodExtract, P_ATLEASTTHREE, "extract(int pos,string from,string up1,string up2...): extract substrings between 'from' and 'up1'...'upn' (the shortest string is used). Return a vector of strings");
	Atanorustring::AddMethod(global, "isemoji", &Atanorustring::MethodIsemoji, P_NONE, "isemoji(): Test if a string only contains emoji characters");
	Atanorustring::AddMethod(global, "emoji", &Atanorustring::MethodEmoji, P_NONE, "emoji(): Return the textual description of an emoji");
	Atanorustring::AddMethod(global, "isupper", &Atanorustring::MethodIsupper, P_NONE, "isupper(): Test if a string only contains uppercase characters");
	Atanorustring::AddMethod(global, "islower", &Atanorustring::MethodIslower, P_NONE, "islower(): Test if a string only contains lowercase characters");
	Atanorustring::AddMethod(global, "upper", &Atanorustring::MethodUpper, P_NONE, "upper(): Return the string in upper characters");
	Atanorustring::AddMethod(global, "deaccentuate", &Atanorustring::MethodDeaccentuate, P_NONE, "deaccentuate(): Remove the accents from accented characters");
	Atanorustring::AddMethod(global, "lower", &Atanorustring::MethodLower, P_NONE, "lower(): Return the string in lower characters");
	Atanorustring::AddMethod(global, "trim", &Atanorustring::MethodTrim, P_NONE, "trim(): remove the trailing characters");
	Atanorustring::AddMethod(global, "trimleft", &Atanorustring::MethodTrimleft, P_NONE, "trimleft(): remove the trailing characters on the left");
	Atanorustring::AddMethod(global, "trimright", &Atanorustring::MethodTrimright, P_NONE, "trimright(): remove the trailing characters on the right");
	Atanorustring::AddMethod(global, "last", &Atanorustring::MethodLast, P_NONE, "last(): return last character");
	Atanorustring::AddMethod(global, "insert", &Atanorustring::MethodInsert, P_ONE | P_TWO, "insert(i,s): insert the string s at i. If i is -1, then insert s between each character in the input string");
	Atanorustring::AddMethod(global, "clear", &Atanorustring::MethodClear, P_NONE, "clear(): Clean the content of a string.");
	
	Atanorustring::AddMethod(global, "jamo", &Atanorustring::MethodJamo, P_NONE | P_ONE, "jamo(bool combine): if 'combine' is false split a Korean jamo into its main components, else combine contents into a jamo.");
	Atanorustring::AddMethod(global, "isjamo", &Atanorustring::MethodIsJamo, P_NONE, "isjamo(): return true if it is a Hangul jamo.");
	Atanorustring::AddMethod(global, "ishangul", &Atanorustring::MethodIsHangul, P_NONE, "ishangul(): return true if it is a Hangul character.");
	Atanorustring::AddMethod(global, "normalizehangul", &Atanorustring::MethodNormalizeHangul, P_NONE, "normalizehangul(): Normalize Hangul characters.");
	Atanorustring::AddMethod(global, "romanization", &Atanorustring::MethodTransliteration, P_NONE, "romanization(): romanization of Hangul characters.");

#ifdef ATANOR_REGEX
	Atanorustring::AddMethod(global, "regex", &Atanorustring::MethodRegex, P_ONE, "regex(string rgx): regex expression 'rgx' applied against string. Returns Boolean values or positions according to recipient variable.");
	Atanorustring::AddMethod(global, "splitrgx", &Atanorustring::MethodSplitRegex, P_ONE, "splitregex(string rgx): splitting along a regex expression 'rgx'.");
	Atanorustring::AddMethod(global, "replacergx", &Atanorustring::MethodReplaceRgx, P_TWO, "replaceregex(string rgx,string replacement): replacing according to a regular expression 'rgx' with 'replacement'.");
#endif

	global->newInstance[Atanorustring::idtype] = new Atanorustring(L"", global);
	global->newInstance[a_ustringthrough] = global->newInstance[Atanorustring::idtype];
	global->RecordMethods(Atanorustring::idtype, Atanorustring::exported);
	global->RecordMethods(a_ustringthrough, Atanorustring::exported);

	return true;
}

AtanorIteration* Atanorustring::Newiteration(bool direction) {
	Locking _lock(this);
	return new AtanorIterationustring(value, direction);
}

Atanor* Atanorustring::in(Atanor* context, Atanor* a, short idthread) {
	//Three cases along the container type...
	//It is a Boolean, we expect false or true
	//It is an integer, we expect a position in v
	//It is a container, we are looking for all positions...

	wstring val = a->UString();
	Locking _lock(this);
	if (context->isBoolean()) {
		if (wcsstr(WSTR(value), WSTR(val)) != NULL)
			return aTRUE;
		return aFALSE;
	}

	if (context->isVectorContainer()) {
		Atanorivector* v = (Atanorivector*)Selectaivector(context);
		s_findall(value, val, v->values);
		return v;
	}

	long r = s_find(value, val, 0);
	return globalAtanor->Provideint(r);
}

unsigned int Atanorustring::EditDistance(Atanor* e) {
	Locking _lock(this);
	wstring s2 = e->UString();
	unsigned int s1len, s2len, x, y, lastdiag, olddiag;
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


Atanor* Atanorustring::MethodOrd(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
    wstring s = value;
    if (s.size() >= 1) {
        if (contextualpattern->isVectorContainer() || s.size()>1) {
            Atanor* kvect=SelectContainer(contextualpattern,idthread);
            if (kvect==NULL)
                kvect=new Atanorlvector;
            
            Locking _lock((AtanorObject*)kvect);
            for (size_t i = 0; i < s.size(); i++)
                kvect->storevalue(s[i]);
            return kvect;
        }
        
		if (contextualpattern->isNumber()) {
			Atanor* a = contextualpattern->Newinstance(idthread);
			a->storevalue(s[0]);
			return a;
		}

		return new Atanorlong(s[0]);
    }
    
    return aNULL;
}


Atanor* Atanorustring::Put(Atanor* idx, Atanor* v, short idthread) {
	if (v == this)
		return aFALSE;
	wstring s;
	if (v != aNULL)
		s = v->UString();

	if (!idx->isIndex()) {
		Locking _lock(this);
		value = s;
		return aTRUE;
	}

	int ileft, iright;
	Locking _lock(this);
	char res = StringIndexes(value, idx, ileft, iright, idthread);

	if (res == 0) {
		if (globalAtanor->erroronkey)
			globalAtanor->Returnerror("Wrong key in a container or a string access", idthread);
		return aFALSE;
	}

	if (res == 1) {
		iright = ileft;
		iright = c_char_next(USTR(value), iright);
	}
	else
		iright = iright - ileft;
	value.erase(ileft, iright);
	if (s != L"")
		value.insert(ileft, s);
	return aTRUE;
}

Atanor* Atanorustring::Get(Atanor* context, Atanor* idx, short idthread) {
	if (!idx->isIndex() || context == idx)
		return this;

	int ileft, iright;
	Locking _lock(this);
	char res = StringIndexes(value, idx, ileft, iright, idthread);

	if (res == 0) {
		if (globalAtanor->erroronkey)
			globalAtanor->Returnerror("Wrong key in a container or a string access", idthread);
		return aNOELEMENT;
	}

	if (res == 1) {
		Atanorustring* s = globalAtanor->Provideustring(L"");
		s->value = value[ileft];
		return s;
	}

	return globalAtanor->Provideustring(value.substr(ileft, iright - ileft));
}

#ifdef ATANOR_REGEX
Atanor* Atanorustring::MethodReplaceRgx(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	Atanor* kreg = callfunc->Evaluate(0, contextualpattern, idthread);

	wstring str = value;
	wstring reg = kreg->UString();
	wstring rep = callfunc->Evaluate(1, contextualpattern, idthread)->UString();

	try {
		wregex pattern(reg);
		wstring res = regex_replace(str, pattern, rep);
		return globalAtanor->Provideustring(res);
	}
	catch (...) {
		string estr = "RGX(001):Wrong regular expression: ";
		estr += kreg->String();
		return globalAtanor->Returnerror(estr, idthread);
	}
}

Atanor* Atanorustring::MethodRegex(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {


	Atanor* kreg = callfunc->Evaluate(0, contextualpattern, idthread);

	const wsregex_token_iterator end;
	wstring str = value;
	wstring reg = kreg->UString();
	try {
		wregex pattern(reg);


		if (contextualpattern->isVectorContainer()) {
			AtanorConstUString localstring(L"");

			Atanor* vect = Selectauvector(contextualpattern);
			for (wsregex_token_iterator i(str.begin(), str.end(), pattern); i != end; ++i) {
				localstring.value = *i;
				vect->Push(&localstring);
			}
			return vect;
		}

		if (contextualpattern->isNumber() || contextualpattern->isString()) {
			wsmatch result;
			if (regex_search(str, result, pattern) == true) {
				if (contextualpattern->isString())
					return globalAtanor->Provideustring(result[0]);
				return globalAtanor->Provideint(result.position());
			}
			return aNULL;
		}

		if (regex_match(str, pattern) == false)
			return aFALSE;
	}
	catch (...) {
		string estr = "RGX(002):Wrong regular expression: ";
		estr += kreg->String();
		return globalAtanor->Returnerror(estr, idthread);
	}
	return aTRUE;
}

Atanor* Atanorustring::MethodSplitRegex(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {


	Atanor* kreg = callfunc->Evaluate(0, contextualpattern, idthread);

	wstring str = value;
	wstring reg = kreg->UString();
	try {
		wregex pattern(reg);


		if (contextualpattern->isVectorContainer()) {
			AtanorConstUString localstring(L"");

			Atanor* vect = Selectauvector(contextualpattern);
			match_results<wstring::const_iterator> result;
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
		string estr = "RGX(003):Wrong regular expression: ";
		estr += kreg->String();
		return globalAtanor->Returnerror(estr, idthread);
	}

	return aNULL;
}
#endif


Atanor* Atanorustring::MethodCount(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	int i = 0, nb = 0;

	int mx = -1;
	if (callfunc->Size() >= 2) {
		i = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
		if (callfunc->Size() == 3)
			mx = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
	}

	wstring str = value;
	if (str == L"")
		return aZERO;
	wstring sub = callfunc->Evaluate(0, contextualpattern, idthread)->UString();

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


Atanor* Atanorustring::MethodFind(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* ksub = callfunc->Evaluate(0, contextualpattern, idthread);
	//we search for a substring starting at position idx
	int i = 0;
	if (callfunc->Size() == 2)
		i = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();

	wstring str = value;
	wstring sub = ksub->UString();
	i = s_find(str, sub, i);
	if (i == wstring::npos)
		return aMINUSONE;
	return globalAtanor->Provideint(i);
}

Atanor* Atanorustring::MethodRfind(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* ksub = callfunc->Evaluate(0, contextualpattern, idthread);
	wstring str = value;
	wstring sub = ksub->UString();
	int i = 0;
	i = s_rfind(str, sub, i);
	if (i == wstring::npos)
		return aMINUSONE;
	return globalAtanor->Provideint(i);
}

Atanor* Atanorustring::MethodTokenize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	wstring thestr = value;
	short flag = 0;
	bool comma = false;
	bool separator = false;
	bool keepwithdigit = false;
	if (callfunc->Size() >= 2) {
		comma = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
		if (comma)
			flag |= token_comma;
		if (callfunc->Size() >= 3) {
			separator = callfunc->Evaluate(1, contextualpattern, idthread)->Boolean();
			if (separator)
				flag |= token_separator;
			if (callfunc->Size() == 4) {
				keepwithdigit = callfunc->Evaluate(2, contextualpattern, idthread)->Boolean();
				if (keepwithdigit)
					flag |= token_keepwithdigit;
			}
		}
	}

	vector<wstring> v;
	vw_tokenize(v, thestr, flag);

	Atanor* kvect = Selectauvector(contextualpattern);
	if (kvect->Type() == a_uvector) {
		Locking _lock((AtanorObject*)kvect);
		((Atanoruvector*)kvect)->values = v;
	}
	else {
		AtanorConstUString localstring(L"");

		for (int i = 0; i < v.size(); i++) {
			localstring.value = v[i];
			kvect->Push(&localstring);
		}
	}
	return kvect;
}

Atanor* Atanorustring::MethodStokenize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	wstring thestr = value;
	Atanor* keeps = NULL;

	if (callfunc->Size() == 2) {
		keeps = callfunc->Evaluate(0, contextualpattern, idthread);
		if (!keeps->isContainer())
			return globalAtanor->Returnerror("KIF(665): Expecting a container in STOKENIZE", idthread);
	}

	vector<wstring> v;
	map<wstring, bool> k;
	if (keeps != NULL) {
		AtanorIteration* itr = keeps->Newiteration(false);
		for (itr->Begin(); itr->End() == aFALSE; itr->Next())
			k[itr->Valueustring()] = true;
		itr->Release();
	}
	s_tokenize(v, thestr, k);

	Atanor* kvect = Selectauvector(contextualpattern);
	if (kvect->Type() == a_uvector) {
		Locking _lock((AtanorObject*)kvect);
		((Atanoruvector*)kvect)->values = v;
	}
	else {
		AtanorConstUString localstring(L"");

		for (int i = 0; i < v.size(); i++) {
			localstring.value = v[i];
			kvect->Push(&localstring);
		}
	}
	return kvect;
}

Atanor* Atanorustring::MethodJamo(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	wstring w;
	if (callfunc->Size() == 0 || callfunc->Evaluate(0, contextualpattern, idthread)->Boolean() == false) {
		w = s_split_jamo(value);
		if (contextualpattern->isVectorContainer()) {
			Atanoruvector* vs = (Atanoruvector*)Selectauvector(contextualpattern);
			wstring s;
			for (long i = 0; i < w.size(); i++) {
				s = w[i];
				vs->values.push_back(s);
			}
			return vs;
		}

		return globalAtanor->Provideustring(w);
	}
	//Else combine...
	w = s_combine_jamo(value);
	return globalAtanor->Provideustring(w);
}

Atanor* Atanorustring::MethodIsJamo(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (s_is_jamo(value))
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorustring::MethodIsHangul(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (s_is_hangul(value))
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorustring::MethodNormalizeHangul(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	return globalAtanor->Provideustring(s_hangul_normalize(value));

}

Atanor* Atanorustring::MethodTransliteration(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	string c;
	if (contextualpattern->isVectorContainer()) {
		Atanorsvector* vs = (Atanorsvector*)Selectasvector(contextualpattern);
		for (int i = 0; i < value.size(); i++) {
			c = c_translate(value[i]);
			vs->values.push_back(c);
		}
		return vs;
	}

	if (value.size())
		c = c_translate(value[0]);

	return globalAtanor->Providestring(c);
}


static bool comp(long s1, long s2) {
	if (s1<s2)
		return true;
	return false;
}


Atanor* Atanorustring::MethodMultiSplit(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	//Third parameter can be a vector

	Atanor* kvect = NULL;
	wstring thesplitter;
	vector<long> ipositions;
	size_t pos = 0, found;
	int i = 0;
	//We first fill our vector with initial values...
	while (!pos && i < callfunc->Size()) {
		thesplitter = callfunc->Evaluate(i, contextualpattern, idthread)->UString();
		pos = 0;
		while (pos != string::npos) {
			found = s_find(value, thesplitter, pos);
			if (found != string::npos) {
				ipositions.push_back(found);
				pos = found + thesplitter.size();
				ipositions.push_back(pos);
			}
			else
				break;
		}
		i++;
	}
	//we have three forbidden cases: 
	//a) x'<x and y'>x
	//b) x'>x and y'<y
	//c) x'>x and y'<y


	for (; i < callfunc->Size(); i++) {
		thesplitter = callfunc->Evaluate(i, contextualpattern, idthread)->UString();
		pos = 0;
		while (pos != string::npos) {
			found = s_find(value, thesplitter, pos);
			if (found != string::npos) {
				bool add = true;
				pos = found + thesplitter.size();
				for (int j = 0; j < ipositions.size(); j += 2) {
					if (found <= ipositions[j] && pos >= ipositions[j]) {
						add = false;
						break;
					}
					if (found >= ipositions[j] && (pos <= ipositions[j + 1] || found <= ipositions[j + 1])) {
						add = false;
						break;
					}
				}

				if (add) {
					ipositions.push_back(found);
					ipositions.push_back(pos);
				}
			}
			else
				break;
		}
	}
	sort(ipositions.begin(), ipositions.end(), comp);


	//First parameter is a string to split
	if (contextualpattern->Type() == a_uvector || !contextualpattern->isVectorContainer())
		kvect = Selectauvector(contextualpattern);
	else
		kvect = Selectacontainer(contextualpattern, idthread);


	pos = 0;
	for (i = 0; i < ipositions.size(); i += 2) {
		if (pos != ipositions[i]) {
			thesplitter = value.substr(pos, ipositions[i] - pos);
			kvect->storevalue(thesplitter);
		}
		pos = ipositions[i + 1];
	}

	if (ipositions.back() < value.size())
		kvect->storevalue(value.substr(ipositions.back(), value.size() - ipositions.back()));


	return kvect;
}


Atanor* Atanorustring::MethodSplit(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	//Third parameter can be a vector

	Atanor* kvect = NULL;
	wstring localvalue;
	wstring thestr = value;
	wstring thesplitter;
	if (callfunc->Size() != 1)
		thesplitter = callfunc->Evaluate(0, contextualpattern, idthread)->UString();

	//First parameter is a string to split
	if (callfunc->Size() == 3)
		kvect = callfunc->Evaluate(1, contextualpattern, idthread);
	else {
		if (contextualpattern->Type() == a_uvector || !contextualpattern->isVectorContainer())
			kvect = Selectauvector(contextualpattern);
		else {
			kvect = Selectacontainer(contextualpattern, idthread);
		}
	}

	if (callfunc->Size() == 0) {
		wchar_t c;
		//we split along space characters...
		for (int i = 0; i < thestr.size(); i++) {
			c = thestr[i];
			if (c <= 32) {
				if (thesplitter.size()) {
					kvect->storevalue(thesplitter);
					thesplitter = L"";
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
	if (thesplitter == L"") {
		if (kvect->Type() == a_bvector) {
			for (int i = 0; i < thestr.size(); i++)
				kvect->storevalue((unsigned char)thestr[i]);
			return kvect;
		}

		if (kvect->Type() == a_ivector) {
			for (int i = 0; i < thestr.size(); i++)
				kvect->storevalue((long)thestr[i]);
			return kvect;
		}

		for (int i = 0; i < thestr.size(); i++)
			kvect->storevalue((wchar_t)thestr[i]);

		return kvect;
	}

	size_t pos = 0;
	size_t found = 0;
	while (pos != string::npos) {
		found = s_find(thestr, thesplitter, pos);
		if (found != string::npos) {
			localvalue = thestr.substr(pos, found - pos);
			if (localvalue != L"")
				kvect->storevalue(localvalue);
			pos = found + thesplitter.size();
		}
		else
			break;
	}

	localvalue = thestr.substr(pos, thestr.size() - pos);
	if (localvalue != L"")
		kvect->storevalue(localvalue);

	return kvect;
}

Atanor* Atanorustring::MethodSplite(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	//Third parameter can be a vector

	Atanor* kvect = NULL;
	wstring localvalue;
	wstring thestr = value;
	wstring thesplitter;
	if (callfunc->Size() != 1)
		thesplitter = callfunc->Evaluate(0, contextualpattern, idthread)->UString();

	//First parameter is a string to split
	if (callfunc->Size() == 3)
		kvect = callfunc->Evaluate(1, contextualpattern, idthread);
	else {
		if (contextualpattern->Type() == a_uvector || !contextualpattern->isVectorContainer())
			kvect = Selectauvector(contextualpattern);
		else {
			kvect = Selectacontainer(contextualpattern, idthread);
		}
	}

	if (callfunc->Size() == 0) {
		wchar_t c;
		//we split along space characters...
		for (int i = 0; i < thestr.size(); i++) {
			c = thestr[i];
			if (c <= 32) {
				kvect->storevalue(thesplitter);
				thesplitter = L"";
			}
			else {
				thesplitter += thestr[i];
			}
		}

		kvect->storevalue(thesplitter);
		return kvect;
	}


	//Second parameter is the splitter string
	if (thesplitter == L"") {
		if (kvect->Type() == a_bvector) {
			for (int i = 0; i < thestr.size(); i++)
				kvect->storevalue((unsigned char)thestr[i]);
			return kvect;
		}

		if (kvect->Type() == a_ivector) {
			for (int i = 0; i < thestr.size(); i++)
				kvect->storevalue((long)thestr[i]);
			return kvect;
		}

		for (int i = 0; i < thestr.size(); i++)
			kvect->storevalue((wchar_t)thestr[i]);

		return kvect;
	}

	size_t pos = 0;
	size_t found = 0;
	while (pos != string::npos) {
		found = s_find(thestr, thesplitter, pos);
		if (found != string::npos) {
			localvalue = thestr.substr(pos, found - pos);
			kvect->storevalue(localvalue);
			pos = found + thesplitter.size();
		}
		else
			break;
	}

	localvalue = thestr.substr(pos, thestr.size() - pos);
	if (localvalue != L"")
		kvect->storevalue(localvalue);

	return kvect;
}

Atanor* Atanorustring::MethodNgrams(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	int num = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	if (num <= 0)
		return Selectasvector(contextualpattern);

	int i, j, mx;

	wstring  s = value;
	wstring u;
	Atanor* ke = Selectauvector(contextualpattern);

	mx = s.size() - num + 1;
	for (i = 0; i < mx; i++) {
		u = L"";
		for (j = i; j < i + num; j++)
			u += s[j];
		ke->storevalue(u);
	}
	return ke;
}

Atanor* Atanorustring::MethodLatin(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring thestr(UString());
	return globalAtanor->Providestring(thestr.utf8tolatin());
}


Atanor* Atanorustring::MethodDos(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	agnostring thestr(UString());
	return globalAtanor->Providestring(thestr.utf8todos());
}

Atanor* Atanorustring::MethodDostoutf8(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	agnostring thestr(UString());
	return globalAtanor->Providestring(thestr.dostoutf8());
}


Atanor* Atanorustring::MethodLeft(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	int nbchar = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	wstring thestr = value;
	thestr = s_left(thestr, nbchar);
	return globalAtanor->Provideustring(thestr);
}

Atanor* Atanorustring::MethodRight(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	int nbchar = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	wstring thestr = value;
	thestr = s_right(thestr, nbchar);
	return globalAtanor->Provideustring(thestr);
}

Atanor* Atanorustring::MethodMid(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	//Second parameter is a position
	long pos = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	//Third parameter is a number of characters
	int nbchar = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
	wstring thestr = value;
	thestr = s_middle(thestr, pos, nbchar);
	return globalAtanor->Provideustring(thestr);
}

Atanor* Atanorustring::MethodIslower(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (s_is_lower(value))
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorustring::MethodIsupper(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (s_is_upper(value))
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorustring::MethodIsemoji(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (s_is_emoji(value))
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorustring::MethodEmoji(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	string s = Emoji(value);
	return globalAtanor->Providestring(s);
}


Atanor* Atanorustring::MethodIsdigit(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	agnostring thestr(UString());
	if (thestr.isdigit())
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorustring::MethodIsalpha(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	wstring str = value;
	if (s_is_alpha(str))
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorustring::MethodIsconsonant(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	wstring str = value;
	if (s_is_consonant(str))
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorustring::MethodIsvowel(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	wstring str = value;
	if (s_is_vowel(str))
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorustring::MethodDeaccentuate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	wstring str = value;
	return globalAtanor->Provideustring(s_deaccentuate(str));
}

Atanor* Atanorustring::MethodIspunctuation(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	wstring str = value;
	if (s_is_punctuation(str))
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorustring::MethodLower(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	wstring str = value;
	return globalAtanor->Provideustring(s_to_lower(str));
}

Atanor* Atanorustring::MethodUpper(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	wstring str = value;
	return globalAtanor->Provideustring(s_to_upper(str));
}

Atanor* Atanorustring::MethodTrim(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string

	wstring str = value;
	return globalAtanor->Provideustring(Trim(str));
}

Atanor* Atanorustring::MethodTrimright(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string

	wstring str = value;
	return globalAtanor->Provideustring(Trimright(str));
}

Atanor* Atanorustring::MethodTrimleft(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string

	wstring str = value;
	return globalAtanor->Provideustring(Trimleft(str));
}

Atanor* Atanorustring::MethodPadding(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* res = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* kval = callfunc->Evaluate(1, contextualpattern, idthread);
	bool direction = true;
	if (callfunc->Size() == 3)
		direction = callfunc->Evaluate(2, contextualpattern, idthread)->Boolean();


	wstring sval = kval->UString();
	long sz = res->Integer() - Size();
	if (sz <= 0)
		return this;

	wstring sres;
	wstring s;
	while (sz > 0) {
		s += sval;
		sz--;
	}
	sres = s;
	Locking _lock(this);
	if (direction)
		value += sres;
	else
		value = sres + value;

	return this;
}

Atanor* Atanorustring::MethodFill(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* res = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* kval = callfunc->Evaluate(1, contextualpattern, idthread);
	int sz = res->Integer();
	if (sz <= 0)
		return globalAtanor->Provideustring(L"");

	wstring sval = kval->UString();
	Locking _lock(this);
	value = L"";
	while (sz > 0) {
		value += sval;
		sz--;
	}
	return this;
}

Atanor* Atanorustring::MethodExtract(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	Atanor* pos = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* from = callfunc->Evaluate(1, contextualpattern, idthread);
	vector<wstring> v;
	int i;
	for (i = 3; i < callfunc->Size(); i++)
		v.push_back(callfunc->Evaluate(i, contextualpattern, idthread)->UString());
	wstring init = value;
	i = pos->Integer();
	wstring sfrom = from->UString();
	i = init.find(sfrom, i);
	int lgfrom = sfrom.size();
	Atanor* vect = Selectauvector(contextualpattern);
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

Atanor* Atanorustring::MethodReverse(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	wstring s = value;
	return globalAtanor->Provideustring(s_revert(s));
}


Atanor* Atanorustring::MethodRemovefirst(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* knb = callfunc->Evaluate(0, contextualpattern, idthread);
	int id = knb->Integer();
	wstring s = value;
	//On retire en tete
	int iv = s.size();
	if (iv > 0 && id <= iv) {
		s = s_right(s, iv - id);
		return globalAtanor->Provideustring(s);
	}

	return this;
}

Atanor* Atanorustring::MethodRemovelast(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* knb = callfunc->Evaluate(0, contextualpattern, idthread);
	int id = knb->Integer();

	wstring s = value;
	int iv = s.size();

	if (iv > 0 && id <= iv) {
		s = s_left(s, iv - id);
		return globalAtanor->Provideustring(s);
	}

	return this;
}

Atanor* Atanorustring::MethodFormat(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	wchar_t buffer[101];
	wstring sformat = value;
	//In this case, we might have more than two parameters...
	wstring e;
	buffer[0] = L'%';
	for (long i = callfunc->Size() - 1; i >= 0; i--) {
		e = callfunc->Evaluate(i, contextualpattern, idthread)->UString();
		swprintf_s(buffer + 1, 100, L"%d", i + 1);
		sformat = s_replacestring(sformat, buffer, e);
	}
	return globalAtanor->Provideustring(sformat);

}

Atanor* Atanorustring::MethodReplace(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//Replace an element with another
	Atanor* kstr = callfunc->Evaluate(-1, contextualpattern, idthread);

	if (kstr->Type() == Atanorustring::idtype) {
		wstring str = kstr->UString();
		wstring reg = callfunc->Evaluate(0, contextualpattern, idthread)->UString();
		if (reg == L"")
			return globalAtanor->Provideustring(str);

		wstring rep = callfunc->Evaluate(1, contextualpattern, idthread)->UString();

		str = s_replacestring(str, reg, rep);
		return globalAtanor->Provideustring(str);
	}

	string str = kstr->String();
	string reg = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	if (reg == "")
		return globalAtanor->Providestring(str);

	string rep = callfunc->Evaluate(1, contextualpattern, idthread)->String();

	str = s_replacestring(str, reg, rep);
	return globalAtanor->Providestring(str);
}

Atanor* Atanorustring::MethodEditdistance(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* e2 = callfunc->Evaluate(0, contextualpattern, idthread);
	return globalAtanor->Provideint(EditDistance(e2));
}


Atanor* Atanorustring::MethodLast(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter should be a vector
	wstring w = value;
	if (w.size() == 0)
		return globalAtanor->Provideustring(L"");
	w = w[w.size() - 1];
	return globalAtanor->Provideustring(w);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------


Atanor* Atanorustring::MethodEvaluate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	static string reg("[\\\\[%d%d%d,x%x%x,u%X%X%X%X,?],&[%#%d(%d)(%d)(%d);,[%c,%C]+;],%%%X%X,u%X%X%X%X]");
	static vector<int> vectr;
	static automate metacodes(USTR(reg), (void*)1);

	string sub = String();

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
Atanor* Atanorustring::MethodReplaceGrammar(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//Replace an element with another

	string str = String();
	string reg = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	string rep = callfunc->Evaluate(1, contextualpattern, idthread)->String();

	string res = replacestringwithautomaton(str, reg, rep);
	return globalAtanor->Providestring(res);
}


void XNBrowse(x_node* xn, Atanor* kf);
Atanor* Atanorustring::MethodParse(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	string str = String();
	x_readstring xr(str);
	xr.loadtoken();
	bnf_atanor bnf;
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

Atanor* Atanorustring::MethodLisp(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
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

Atanor* Atanorustring::MethodTags(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring str(String());
	string copen = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	string cclose = callfunc->Evaluate(1, contextualpattern, idthread)->String();
	return globalAtanor->EvaluateTags(str, copen, cclose, idthread);
}

Atanor* Atanorustring::MethodGrammar(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

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

Atanor* Atanorustring::MethodHash(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	static const std::collate<char>& coll = std::use_facet<std::collate<char> >(loc);
	string s = String();
	if (s == "")
		return aNULL;
	long myhash = coll.hash(s.data(), s.data() + s.length());
	return globalAtanor->Provideint(myhash);
}

Atanor* Atanorustring::MethodBase(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
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
		return globalAtanor->Returnerror(message, idthread);
	}

	w = value;
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

Atanor* Atanorustring::MethodTohtml(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
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

Atanor* Atanorustring::MethodToxml(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
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

Atanor* Atanorustring::MethodInsert(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	size_t idx;

	//First parameter should be a vector

	//second parameter is anything
	idx = -1;
	string val;
	agnostring ret(UString());
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
	return globalAtanor->Provideustring(ret.utf8tounicode());
}

Atanor* Atanorustring::MethodClear(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lock(this);
	value = L"";
	return aTRUE;
}


Atanor* Atanorustring::MethodPop(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
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


Atanor* Atanorustring::Loophaskell(Atanor* recipient, Atanor* context, Atanor* environment, AtanorFunctionLambda* bd, short idthread) {
	Atanor* a;
	
	Atanorustring* it = globalAtanor->Provideustring(L"");
	it->Setreference();

	char addvalue = 0;

	if (context != aNOELEMENT)
		addvalue = Selecttype(context);

	recipient->Putvalue( it, idthread);

	for (size_t i = 0; i < value.size(); i++) {

		it->value = value[i];

		a = bd->Get(environment, aNULL, idthread);

		if (globalAtanor->Error(idthread) || a == aBREAK) {
			environment->Cleaning(idthread);
			if (a == aBREAK)
				break;
			it->Resetreference();
			recipient->Forcedclean();
			a->Release();
			context->Release();
			return globalAtanor->Errorobject(idthread);
		}

		if (a != aNULL) {
			context = Storealongtype(context, a, idthread, addvalue);
			a->Release();
		}
	}

	it->Resetreference();
	recipient->Forcedclean();

	return context;
}

Atanor* Atanorustring::Filter(short idthread, Atanor* env, AtanorFunctionLambda* bd, Atanor* var, Atanor* kcont, Atanor* accu, Atanor* init, bool direct) {

	Atanor* returnval;

	bool first = false;
	Atanorustring* key = globalAtanor->Provideustring(L"");
	key->Setreference();

	if (init != aNOELEMENT) {
		accu->Putvalue(init, idthread);
		if (kcont != NULL) {
			if (direct)
				kcont->Insert(0, init);
			else
				kcont->Push(init);
		}
	}
	else
		first = true; //we are dealing with a foldr1 or a foldl1

	for (size_t i = 0; i < value.size(); i++) {
		key->value = value[i];
		if (first) {
			returnval = key->Atom();//We use the first value of the list to seed our accumulator variable
			first = false;
		}
		else {
			var->Putvalue(key, idthread);
			returnval = bd->Get(env, aNULL, idthread);

			if (returnval == aBREAK) {
				accu = returnval;
				break;
			}

			if (globalAtanor->Error(idthread)) {
				env->Cleaning(idthread);
				key->Resetreference();
				var->Forcedclean();
				if (kcont != NULL)
					kcont->Release();
				return globalAtanor->Errorobject(idthread);
			}
		}

		if (returnval != aNULL) {
			
			accu->Putvalue(returnval, idthread);
			if (kcont != NULL) {
				if (direct)
					kcont->Insert(0, returnval);
				else
					kcont->Push(returnval);
			}
			returnval->Release();
		}
	}

	key->Resetreference();
	var->Forcedclean();
	if (kcont != NULL)
		return kcont;
	return accu->Value();
}


Atanor* Atanorustring::andset(Atanor* a, bool autoself) {
	wstring s = a->UString();
	wstring u;
	long m = min(s.size(), value.size());
	for (long i = 0; i < m; i++) {
		if (s[i] == value[i])
			u += s[i];
	}
	if (autoself) {
		value = u;
		return this;
	}

	return globalAtanor->Provideustring(u);
}

Atanor* Atanorustring::xorset(Atanor* a, bool autoself) {
	wstring s = a->UString();
	wstring u;
	long m = min(s.size(), value.size());
	for (long i = 0; i < m; i++) {
		if (s[i] != value[i])
			u += value[i];
	}
	if (autoself) {
		value = u;
		return this;
	}

	return globalAtanor->Provideustring(u);
}

void AtanorLoopUString::Callfunction() {

	AtanorCallFunction kfunc(function);

	Atanorint* ki = globalAtanor->Provideint(position);
	ki->Setreference();
	kfunc.arguments.push_back(this);
	kfunc.arguments.push_back(ki);
	Atanor* ke = kfunc.Get(aNULL, aNULL, globalAtanor->GetThreadid());
	ke->Release();
	ki->Resetreference();
}

Atanor* AtanorLoopUString::Put(Atanor* context, Atanor* ke, short idthread) {
	if (ke->isVectorContainer()) {
		position = 0;
		interval.clear();
		for (int i = 0; i < ke->Size(); i++)
			interval.push_back(ke->getustring(i));
		value = interval[0];
		return aTRUE;
	}

	if (ke->Type() == a_uloop) {
		AtanorLoopUString* kl = (AtanorLoopUString*)ke;
		interval = kl->interval;
		value = kl->value;
		position = kl->position;
		return aTRUE;
	}

	if (interval.size() == 0) {
		position = 0;
		value = L"";
		return aTRUE;
	}

	position = ke->Integer();
	if (position >= interval.size())
		position = position % interval.size();
	else
	if (position<0)
		position = (interval.size() + position) % interval.size();
	value = interval[position];
	return aTRUE;
}


Atanor* AtanorLoopUString::Vector(short idthread) {
	Atanoruvector* kvect = globalAtanor->Provideuvector();
	kvect->values = interval;
	return kvect;
}


Atanor* AtanorLoopUString::andset(Atanor* a, bool autoself) {
	wstring s = a->UString();
	wstring u;
	long m = min(s.size(), value.size());
	for (long i = 0; i < m; i++) {
		if (s[i] == value[i])
			u += s[i];
	}
	if (autoself) {
		value = u;
		return this;
	}

	return globalAtanor->Provideustring(u);
}

Atanor* AtanorLoopUString::xorset(Atanor* a, bool autoself) {
	wstring s = a->UString();
	wstring u;
	long m = min(s.size(), value.size());
	for (long i = 0; i < m; i++) {
		if (s[i] != value[i])
			u += value[i];
	}
	if (autoself) {
		value = u;
		return this;
	}

	return globalAtanor->Provideustring(u);
}


