
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
filename   : atanorstring.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorhaskell.h"
#include "atanorstring.h"
#include "constobjects.h"
#include "atanorivector.h"
#include "atanorfvector.h"
#include "atanorbvector.h"
#include "atanorsvector.h"
#include "automate.h"
#include "atanorvector.h"
#include "atanorlvector.h"
#include "atanorconstants.h"
#include "atanorbyte.h"
#include "atanorustring.h"
#include "fractalhmap.h"
#include "instructions.h"
#include "atanormap.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, stringMethod>  Atanorstring::methods;
Exporting hmap<string, string> Atanorstring::infomethods;
Exporting bin_hash<unsigned long> Atanorstring::exported;

Exporting short Atanorstring::idtype = 0;

#ifdef DOSOUTPUT
static bool dosoutput = true;
static void setdosoutput(bool d) { dosoutput = d; }
#define conversion2Dos(x) dosoutput?s_utf8_to_dos(USTR(x)):x
#endif


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorstring::AddMethod(AtanorGlobal* global, string name, stringMethod func, unsigned long arity, string infos, short returntype) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
	
	if (returntype != a_null)
		global->returntypes[idname] = returntype;
}

bool Atanorstring::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorstring::idtype = global->Getid("string");

	Atanorstring::AddMethod(global, "hash", &Atanorstring::MethodHash, P_NONE, "hash(): Return the hash value of a string.", a_int);
	Atanorstring::AddMethod(global, "ord", &Atanorstring::MethodOrd, P_NONE, "ord(): return the ASCII code of the first character, or a list of all ASCII code if the recipient is a vector", a_null);
	Atanorstring::AddMethod(global, "bytes", &Atanorstring::MethodBytes, P_NONE, "bytes(): Return the string as a vector of bytes", a_ivector);
	Atanorstring::AddMethod(global, "base", &Atanorstring::MethodBase, P_ONE, "base(int b): Return the value corresponding to the string in base b", a_null);
	Atanorstring::AddMethod(global, "parse", &Atanorstring::MethodParse, P_NONE | P_TWO, "parse(): Parse a string as a piece of code and returns the evaluation as a vector.", a_vector);
	Atanorstring::AddMethod(global, "sizeb", &Atanorstring::MethodSizeb, P_NONE, "sizeb(): Return the size in bytes of the string", a_int);
	Atanorstring::AddMethod(global, "lisp", &Atanorstring::MethodLisp, P_NONE | P_TWO, "lisp(): Parse a string as a parenthetic expressions, which is returned as a vector.|lisp(string o,string c): Parse a string as a parenthetic expressions, where the opening and closing characters are provided.", a_null);
	Atanorstring::AddMethod(global, "tags", &Atanorstring::MethodTags, P_TWO, "tags(string o,string c): Parse a string as a parenthetic expression, where the opening and closing strings are provided.", a_null);
	Atanorstring::AddMethod(global, "gram", &Atanorstring::MethodGrammar, P_ONE, "gram(sub): Find the substrings matching sub, using the same formalism as in grammar", a_null);
	Atanorstring::AddMethod(global, "getstd", &Atanorstring::MethodGetstd, P_ONE, "getstd(bool): catch or release the standard output", a_null);
	Atanorstring::AddMethod(global, "geterr", &Atanorstring::MethodGeterr, P_ONE, "geterr(bool): catch or release the error output", a_null);

	Atanorstring::AddMethod(global, "levenshtein", &Atanorstring::MethodEditdistance, P_ONE | P_TWO, "levenshtein(string s,bool byte): Return the edit distance with 's' according to Levenshtein algorithm. If byte is true, force a byte level comparison. byte is optionnal.", a_int);
	Atanorstring::AddMethod(global, "editdistance", &Atanorstring::MethodEditdistance, P_ONE | P_TWO | P_THREE, "editdistance(string s,bool byte): Return the edit distance with 's'. If byte is true, force a byte level comparison. byte is optionnal.", a_int);

	Atanorstring::AddMethod(global, "tokenize", &Atanorstring::MethodTokenize, P_NONE | P_ONE | P_TWO | P_THREE, "tokenize(bool comma,bool separator,bool concatenate): Segment a string into words and punctuations. If 'comma' is true, then the decimal character is ',' otherwise it is '.'. If 'separator' is true then '1,000' is accepted as a number. If 'concatenate' is true then '3a' is a valid token.", a_null);
	Atanorstring::AddMethod(global, "stokenize", &Atanorstring::MethodStokenize, P_NONE | P_ONE, "stokenize(map keeps): Segment a string into words and punctuations, with a keep.", a_null);
	Atanorstring::AddMethod(global, "split", &Atanorstring::MethodSplit, P_TWO | P_ONE | P_NONE, "split(string splitter,vector vect): split a string along splitter and store the results  in a vector. If splitter=='', then the string is split into a vector of characters", a_svector);
	Atanorstring::AddMethod(global, "splite", &Atanorstring::MethodSplite, P_TWO | P_ONE | P_NONE, "splite(string splitter,vector vect): split a string along splitter and store the results  in a vector. If splitter=='', then the string is split into a vector of characters. Empty strings are kept in the result.", a_svector);
	Atanorstring::AddMethod(global, "ngrams", &Atanorstring::MethodNgrams, P_ONE | P_TWO, "ngrams(int r): Return a vector of all ngrams of rank r", a_svector);
	Atanorstring::AddMethod(global, "extract", &Atanorstring::MethodExtract, P_ATLEASTTHREE, "extract(int pos,string from,string up1,string up2...): extract substrings between 'from' and 'up1'...'upn' (the shortest string is used). Return a vector of strings", a_svector);

	Atanorstring::AddMethod(global, "find", &Atanorstring::MethodFind, P_TWO | P_ONE, "find(string sub,int pos): Return the position of substring sub starting at position pos", a_int);
	Atanorstring::AddMethod(global, "rfind", &Atanorstring::MethodRfind, P_TWO | P_ONE, "rfind(string sub,int pos): Return the position of substring sub backward starting at position pos", a_int);

	Atanorstring::AddMethod(global, "count", &Atanorstring::MethodCount, P_THREE | P_TWO | P_ONE | P_NONE, "count(string sub,int pos,int mx): Count the number of substrings starting at position pos, ending at mx", a_int);
	Atanorstring::AddMethod(global, "byteposition", &Atanorstring::MethodByteposition, P_ONE, "byteposition(int pos): Convert a character position into a byte position", a_int);
	Atanorstring::AddMethod(global, "charposition", &Atanorstring::MethodCharposition, P_ONE, "charposition(int pos): Convert a byte position into a character position", a_int);

	Atanorstring::AddMethod(global, "isalpha", &Atanorstring::MethodIsalpha, P_NONE, "isalpha(): Test if a string only contains only alphabetical characters", a_boolean);
	Atanorstring::AddMethod(global, "isconsonant", &Atanorstring::MethodIsconsonant, P_NONE, "isconsonant(): Test if a string only contains consonants", a_boolean);
	Atanorstring::AddMethod(global, "isvowel", &Atanorstring::MethodIsvowel, P_NONE, "isvowel(): Test if a string only contains only vowels", a_boolean);
	Atanorstring::AddMethod(global, "ispunctuation", &Atanorstring::MethodIspunctuation, P_NONE, "ispunctuation(): Test if the character is a punctuation", a_boolean);
	Atanorstring::AddMethod(global, "isdigit", &Atanorstring::MethodIsdigit, P_NONE, "isdigit(): Test if a string only contains digits", a_boolean);
	Atanorstring::AddMethod(global, "isupper", &Atanorstring::MethodIsupper, P_NONE, "isupper(): Test if a string only contains uppercase characters", a_boolean);
	Atanorstring::AddMethod(global, "islower", &Atanorstring::MethodIslower, P_NONE, "islower(): Test if a string only contains lowercase characters", a_boolean);
	Atanorstring::AddMethod(global, "isemoji", &Atanorstring::MethodIsemoji, P_NONE, "isemoji(): Test if a string only contains emoji characters", a_boolean);
	Atanorstring::AddMethod(global, "isutf8", &Atanorstring::MethodIsutf8, P_NONE, "isutf8(): Return true is the string is encoded in UTF8", a_boolean);


	Atanorstring::AddMethod(global, "reverse", &Atanorstring::MethodReverse, P_NONE, "reverse(): reverse the string", a_string);
	Atanorstring::AddMethod(global, "format", &Atanorstring::MethodFormat, P_ATLEASTONE, "format(p1,p2,p3): Create a new string from the current string in which each '%x' is associated to one of the parameters, 'x' being the position of that parameter in the argument list. 'x' starts at 1.", a_string);
	Atanorstring::AddMethod(global, "fill", &Atanorstring::MethodFill, P_TWO, "fill(int nb,string c): create a string of nb characters c", a_string);
	Atanorstring::AddMethod(global, "padding", &Atanorstring::MethodPadding, P_TWO | P_THREE, "padding(int nb,string c,bool paddattheend): add nb characters c to the current string. Last parameter is optional", a_string);
	Atanorstring::AddMethod(global, "pop", &Atanorstring::MethodPop, P_NONE | P_ONE | P_TWO, "pop(): remove last character", a_string);
	Atanorstring::AddMethod(global, "evaluate", &Atanorstring::MethodEvaluate, P_NONE, "evaluate(): evaluate the meta-characters within a string and return the evaluated string.", a_string);
	Atanorstring::AddMethod(global, "html", &Atanorstring::MethodTohtml, P_NONE, "html(): Return the string into an HTML compatible string or as a vector of strings", a_string);
	Atanorstring::AddMethod(global, "tohtml", &Atanorstring::MethodTohtml, P_NONE, "tohtml(): Return the string into an HTML compatible string or as a vector of strings", a_string);
	Atanorstring::AddMethod(global, "toxml", &Atanorstring::MethodToxml, P_NONE, "toxml(): Return the string into an XML compatible string or as a vector of strings", a_string);
	Atanorstring::AddMethod(global, "replacegram", &Atanorstring::MethodReplaceGrammar, P_TWO, "replacegram(sub,str): Replace the substrings matching sub with str, with the same formalism as grammar", a_string);
	Atanorstring::AddMethod(global, "replace", &Atanorstring::MethodReplace, P_TWO, "replace(sub,str): Replace the substrings matching sub with str", a_string);
	Atanorstring::AddMethod(global, "removefirst", &Atanorstring::MethodRemovefirst, P_ONE, "removefirst(int nb): remove the first nb characters of a string", a_string);
	Atanorstring::AddMethod(global, "removelast", &Atanorstring::MethodRemovelast, P_ONE, "removelast(int nb): remove the last nb characters of a string", a_string);
	Atanorstring::AddMethod(global, "utf8", &Atanorstring::MethodUtf8, P_NONE | P_ONE, "utf8(int table): convert a LATIN string into UTF8. table is optional, by default it is ISO/IEC 8859 part 1.", a_string);
	Atanorstring::AddMethod(global, "latin", &Atanorstring::MethodLatin, P_NONE, "latin(): convert an UTF8 string in LATIN", a_string);
	Atanorstring::AddMethod(global, "dos", &Atanorstring::MethodDos, P_NONE, "dos(): convert a string into DOS encoding", a_string);
	Atanorstring::AddMethod(global, "dostoutf8", &Atanorstring::MethodDostoutf8, P_NONE, "dostoutf8(): convert a DOS string into UTF8", a_string);
	Atanorstring::AddMethod(global, "left", &Atanorstring::MethodLeft, P_ONE, "left(int nb): return the first nb characters of a string", a_string);
	Atanorstring::AddMethod(global, "right", &Atanorstring::MethodRight, P_ONE, "right(int nb): return the last nb characters of a string", a_string);
	Atanorstring::AddMethod(global, "mid", &Atanorstring::MethodMid, P_TWO, "mid(int pos,int nb): return the nb characters starting at position pos of a string", a_string);
	Atanorstring::AddMethod(global, "emoji", &Atanorstring::MethodEmoji, P_NONE, "emoji(): Return the textual description of an emoji", a_string);
	Atanorstring::AddMethod(global, "upper", &Atanorstring::MethodUpper, P_NONE, "upper(): Return the string in upper characters", a_string);
	Atanorstring::AddMethod(global, "deaccentuate", &Atanorstring::MethodDeaccentuate, P_NONE, "deaccentuate(): Remove the accents from accented characters", a_string);
    Atanorstring::AddMethod(global, "indent", &Atanorstring::MethodIndent, P_NONE | P_ONE, "indent(int nbblanks): Format a piece of code.", a_string);
    Atanorstring::AddMethod(global, "lower", &Atanorstring::MethodLower, P_NONE, "lower(): Return the string in lower characters", a_string);
	Atanorstring::AddMethod(global, "trim", &Atanorstring::MethodTrim, P_NONE, "trim(): remove the trailing characters", a_string);
	Atanorstring::AddMethod(global, "trimleft", &Atanorstring::MethodTrimleft, P_NONE, "trimleft(): remove the trailing characters on the left", a_string);
	Atanorstring::AddMethod(global, "trimright", &Atanorstring::MethodTrimright, P_NONE, "trimright(): remove the trailing characters on the right", a_string);
	Atanorstring::AddMethod(global, "last", &Atanorstring::MethodLast, P_NONE, "last(): return last character", a_string);
	Atanorstring::AddMethod(global, "insert", &Atanorstring::MethodInsert, P_ONE | P_TWO, "insert(i,s): insert the string s at i. If i is -1, then insert s between each character in the input string", a_string);

	Atanorstring::AddMethod(global, "clear", &Atanorstring::MethodClear, P_NONE, "clear(): Clean the content of a string.", a_null);

#ifdef ATANOR_REGEX
    Atanorstring::AddMethod(global, "regex", &Atanorstring::MethodRegex, P_ONE, "regex(string rgx): regex expression 'rgx' applied against string. Returns Boolean values or positions according to recipient variable.", a_null);
    Atanorstring::AddMethod(global, "splitrgx", &Atanorstring::MethodSplitRegex, P_ONE, "splitregex(string rgx): splitting along a regex expression 'rgx'.", a_svector);
    Atanorstring::AddMethod(global, "replacergx", &Atanorstring::MethodReplaceRgx, P_TWO, "replaceregex(string rgx,string replacement): replacing according to a regular expression 'rgx' with 'replacement'.", a_string);
#endif


	global->newInstance[Atanorstring::idtype] = new Atanorstring("", global);
	global->newInstance[a_stringthrough] = global->newInstance[Atanorstring::idtype];
	global->RecordMethods(Atanorstring::idtype, Atanorstring::exported);
	global->RecordMethods(a_stringthrough, Atanorstring::exported);
	return true;
}


Exporting AtanorIteration* Atanorstring::Newiteration(bool direction) {
	Locking _lock(this);
	return new AtanorIterationstring(value, direction);
}


Exporting Atanor* Atanorstring::in(Atanor* context, Atanor* a, short idthread) {
	//Three cases along the container type...
	//It is a Boolean, we expect false or true
	//It is an integer, we expect a position in v
	//It is a container, we are looking for all positions...

	string val = a->String();
	Locking _lock(this);
	if (context->isBoolean()) {
		if (strstr(STR(value), STR(val)) != NULL)
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

Exporting unsigned int Atanorstring::EditDistance(Atanor* e) {
	Locking _lock(this);
	string s2 = e->String();
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

Exporting Atanor* Atanorstring::Put(Atanor* idx, Atanor* v, short idthread) {
	if (v == this)
		return aFALSE;

	if (!idx->isIndex()) {
		Locking _lock(this);
		value = v->String();
		return aTRUE;
	}

	string s;
	if (v != aNULL)
		s = v->String();

	int ileft, iright;
	Locking _lock(this);
	char res = StringIndexes(value, idx, ileft, iright, idthread);

	if (res == 0) {
		if (globalAtanor->erroronkey)
			return globalAtanor->Returnerror("Wrong key in a container or a string access", idthread);
		return aFALSE;
	}

	if (res == 1) {
		iright = ileft;
		iright = c_char_next(USTR(value), iright);
	}
	else
		iright = iright - ileft;
	value.erase(ileft, iright);
	if (s != "")
		value.insert(ileft, s);
	return aTRUE;

}

Exporting Atanor* Atanorstring::Get(Atanor* context, Atanor* idx, short idthread) {
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

	if (res == 1)
		return globalAtanor->Providestring(c_char_get(USTR(value), ileft));

	return globalAtanor->Providestring(value.substr(ileft, iright - ileft));
}

//------------------------------------------------------------------------------------------------------------------
Atanor* Atanorstring::MethodOrd(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
    wstring s = UString();
    if (s.size() >= 1) {
        if (contextualpattern->isNumber()) {
            Atanor* a=contextualpattern->Newinstance(idthread);
            a->storevalue(s[0]);
            return a;
        }
        
        if (contextualpattern->isVectorContainer() || s.size()>1) {
            Atanor* kvect=SelectContainer(contextualpattern,idthread);
            if (kvect==NULL)
                kvect=new Atanorlvector;
            
            Locking _lock((AtanorObject*)kvect);
            for (size_t i = 0; i < s.size(); i++)
                kvect->storevalue(s[i]);
            return kvect;
        }
        
        return new Atanorlong(s[0]);
    }
    
    return aNULL;
}

//------------------------------------------------------------------------------------------------------------------
Atanor* Atanorstring::MethodIsutf8(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _slock(this);
	if (s_is_utf8(USTR(value), value.size()) == true)
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorstring::MethodEvaluate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	static string reg("[\\\\[%d%d%d,x%x%x,u%X%X%X%X,?],&[%#%d(%d)(%d)(%d);,[%c,%C]+;],%%%X%X,u%X%X%X%X]");
	static vector<int> vectr;
	static automate metacodes(USTR(reg), (void*)1);

	string sub = value;

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
Atanor* Atanorstring::MethodReplaceGrammar(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//Replace an element with another

	string str = value;
	string reg = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	string rep = callfunc->Evaluate(1, contextualpattern, idthread)->String();

	string res = replacestringwithautomaton(str, reg, rep);
	return globalAtanor->Providestring(res);
}


void XNBrowse(x_node* xn, Atanor* kf);
Atanor* Atanorstring::MethodParse(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	string str = value;
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

Atanor* Atanorstring::MethodLisp(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	string str = value;
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

Atanor* Atanorstring::MethodTags(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring str(String());
	string copen = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	string cclose = callfunc->Evaluate(1, contextualpattern, idthread)->String();
	return globalAtanor->EvaluateTags(str, copen, cclose, idthread);
}

Atanor* Atanorstring::MethodGrammar(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

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

Atanor* Atanorstring::MethodCount(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
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

Atanor* Atanorstring::MethodFind(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* ksub = callfunc->Evaluate(0, contextualpattern, idthread);

	//we search for a substring starting at position idx
	int i = 0;
	if (callfunc->Size() == 2)
		i = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();

	string str = value;

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

Atanor* Atanorstring::MethodRfind(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* ksub = callfunc->Evaluate(0, contextualpattern, idthread);

	//we search for a substring starting at position idx
	int i = 0;
	if (callfunc->Size() == 2)
		i = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();

	string str = value;
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

Atanor* Atanorstring::MethodByteposition(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring s(String());
	return globalAtanor->Provideint(s.chartobyteposition(callfunc->Evaluate(0, contextualpattern, idthread)->Integer()));
}

Atanor* Atanorstring::MethodCharposition(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring s(String());
	return globalAtanor->Provideint(s.bytetocharposition(callfunc->Evaluate(0, contextualpattern, idthread)->Integer()));
}

Atanor* Atanorstring::MethodTokenize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

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

Atanor* Atanorstring::MethodStokenize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

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

Atanor* Atanorstring::MethodSplit(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	//Third parameter can be a vector

	Atanor* kvect = NULL;

	string localvalue;
	string thestr = value;
	string thesplitter;
	if (callfunc->Size() != 0)
		thesplitter = callfunc->Evaluate(0, contextualpattern, idthread)->String();

	//First parameter is a string to split
	if (callfunc->Size() == 2)
		kvect = callfunc->Evaluate(1, contextualpattern, idthread);
	else {
		if (contextualpattern->Type() == a_svector || !contextualpattern->isVectorContainer())
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
		if (kvect->Type() == Atanorbvector::idtype) {
			for (int i = 0; i < thestr.size(); i++)
				kvect->storevalue((unsigned char)thestr[i]);
			return kvect;
		}

		agnostring athestr(thestr);
		athestr.begin();
		if (kvect->Type() == Atanorivector::idtype) {
			while (!athestr.end())
				kvect->storevalue((long)athestr.nextcode());
			return kvect;
		}

		while (!athestr.end())
			kvect->storevalue(athestr.next());
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

Atanor* Atanorstring::MethodSplite(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	//Third parameter can be a vector

	Atanor* kvect = NULL;

	string localvalue;
	string thestr = value;
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
		if (kvect->Type() == Atanorbvector::idtype) {
			for (int i = 0; i < thestr.size(); i++)
				kvect->storevalue((unsigned char)thestr[i]);
			return kvect;
		}

		agnostring athestr(thestr);
		athestr.begin();
		if (kvect->Type() == Atanorivector::idtype) {
			while (!athestr.end())
				kvect->storevalue((long)athestr.nextcode());
			return kvect;
		}

		while (!athestr.end())
			kvect->storevalue(athestr.next());
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

Atanor* Atanorstring::MethodNgrams(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	int num = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	if (num <= 0)
		return Selectasvector(contextualpattern);

	int i, j, mx;

	agnostring s(String());
	Atanor* ke = Selectasvector(contextualpattern);
	string u;

	if (s.isutf8()) {
		vector<string> v;
		s.begin();
		while (!s.end())
			v.push_back(s.next());
		mx = v.size() - num + 1;
		for (i = 0; i < mx; i++) {
			u = "";
			for (j = i; j < i + num; j++)
				u += v[j];
			ke->storevalue(u);
		}
		return ke;
	}
	string& sb = (string&)s;
	mx = s.size() - num + 1;
	for (i = 0; i < mx; i++) {
		u = "";
		for (j = i; j < i + num; j++)
			u += sb[j];
		ke->storevalue(u);
	}
	return ke;
}

Atanor* Atanorstring::MethodHash(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	static const std::collate<char>& coll = std::use_facet<std::collate<char> >(loc);
	string s = value;
	if (s == "")
		return aNULL;
	long myhash = coll.hash(s.data(), s.data() + s.length());
	return globalAtanor->Provideint(myhash);
}

Atanor* Atanorstring::MethodLatin(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring thestr(String());
	return globalAtanor->Providestring(thestr.utf8tolatin());
}

Atanor* Atanorstring::MethodUtf8(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	agnostring thestr(String());
	int table = 0;
	if (callfunc->Size() != 0)
		table = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	return globalAtanor->Providestring(thestr.latintoutf8(table));
}

Atanor* Atanorstring::MethodDos(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	agnostring thestr(String());
	return globalAtanor->Providestring(thestr.utf8todos());
}

Atanor* Atanorstring::MethodDostoutf8(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	agnostring thestr(String());
	return globalAtanor->Providestring(thestr.dostoutf8());
}

Atanor* Atanorstring::MethodLeft(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string

	int nbchar = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	agnostring thestr(String());
	//Second parameter is a position
	return globalAtanor->Providestring(thestr.left(nbchar));
}

Atanor* Atanorstring::MethodRight(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	int nbchar = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	agnostring thestr(String());
	return globalAtanor->Providestring(thestr.right(nbchar));
}

Atanor* Atanorstring::MethodMid(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string	
	//Second parameter is a position
	long pos = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	//Third parameter is a number of characters
	int nbchar = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
	agnostring thestr(String());
	return globalAtanor->Providestring(thestr.extract(pos, nbchar));
}

Atanor* Atanorstring::MethodIslower(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring thestr(String());
	if (thestr.islower())
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorstring::MethodIsupper(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring thestr(String());
	if (thestr.isupper())
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorstring::MethodIsemoji(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring thestr(String());
	if (thestr.isemoji())
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorstring::MethodEmoji(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	string s = Emoji(value);
	return globalAtanor->Providestring(s);
}

Atanor* Atanorstring::MethodIsdigit(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	agnostring thestr(String());
	if (thestr.isdigit())
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorstring::MethodIsalpha(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	agnostring thestr(String());
	if (thestr.isalpha())
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorstring::MethodIsconsonant(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	agnostring thestr(String());
	if (thestr.isconsonant())
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorstring::MethodIsvowel(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	agnostring thestr(String());
	if (thestr.isvowel())
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorstring::MethodDeaccentuate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	agnostring thestr(String());
	return globalAtanor->Providestring(thestr.deaccentuate());
}

Atanor* Atanorstring::MethodIspunctuation(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	agnostring thestr(String());
	if (thestr.ispunctuation())
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorstring::MethodLower(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring thestr(String());
	return globalAtanor->Providestring(thestr.tolower());
}

Atanor* Atanorstring::MethodUpper(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring thestr(String());
	return globalAtanor->Providestring(thestr.toupper());
}

Atanor* Atanorstring::MethodTrim(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring thestr(String());
	return globalAtanor->Providestring(thestr.trim());
}

Atanor* Atanorstring::MethodTrimright(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring thestr(String());
	return globalAtanor->Providestring(thestr.trimright());
}

Atanor* Atanorstring::MethodTrimleft(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring thestr(String());
	return globalAtanor->Providestring(thestr.trimleft());
}

Atanor* Atanorstring::MethodBase(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
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

Atanor* Atanorstring::MethodPadding(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* res = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* kval = callfunc->Evaluate(1, contextualpattern, idthread);
	bool direction = true;
	if (callfunc->Size() == 3)
		direction = callfunc->Evaluate(2, contextualpattern, idthread)->Boolean();


	string sval = kval->String();

	Locking _locks(this);
	long sz = res->Integer() - value.size();
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
	if (direction)
		value += sres;
	else
		value = sres + value;

	return this;
}

Atanor* Atanorstring::MethodFill(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	Atanor* res = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* kval = callfunc->Evaluate(1, contextualpattern, idthread);
	int sz = res->Integer();
	if (sz <= 0)
		return globalAtanor->Providestring("");

	string sval = kval->String();

	Locking _locks(this);
	if (sval != "" && sval.size() == 1) {
		char* s = (char*)malloc(sz + 1);
		char c = ' ';
		c = sval[0];
		memset(s, c, sz);
		s[sz] = 0;
		value = s;
		free(s);
	}
	else {
		string s;
		while (sz > 0) {
			s += sval;
			sz--;
		}
		value = s;
	}
	return this;
}

Atanor* Atanorstring::MethodTohtml(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
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

Atanor* Atanorstring::MethodToxml(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
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

Atanor* Atanorstring::MethodBytes(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	int sz = value.size();
	if (sz >= 1) {
        if (contextualpattern->isNumber()) {
            Atanor* a=contextualpattern->Newinstance(idthread);
            a->storevalue((uchar)value[0]);
            return a;
        }

		if (contextualpattern->isVectorContainer() || sz > 1) {
            Atanor* kvect=SelectContainer(contextualpattern,idthread);
            if (kvect==NULL)
				kvect = globalAtanor->Provideivector();
            
            Locking _lock((AtanorObject*)kvect);
            for (size_t i = 0; i < value.size(); i++)
                kvect->storevalue((uchar)value[i]);
            return kvect;
		}

		return new Atanorbyte(value[0]);
	}

	return aNULL;
}

Atanor* Atanorstring::MethodGetstd(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
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

Atanor* Atanorstring::MethodGeterr(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
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

Atanor* Atanorstring::MethodInsert(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
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

Atanor* Atanorstring::MethodClear(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lock(this);
	value = "";
	return aTRUE;
}

Atanor* Atanorstring::MethodExtract(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* pos = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* from = callfunc->Evaluate(1, contextualpattern, idthread);

	vector<string> v;
	int i;
	for (i = 2; i < callfunc->Size(); i++)
		v.push_back(callfunc->Evaluate(i, contextualpattern, idthread)->String());

	string init = value;
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

Atanor* Atanorstring::MethodReverse(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	agnostring s(String());
	return globalAtanor->Providestring(s.revert());
}

Atanor* Atanorstring::MethodRemovefirst(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
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

Atanor* Atanorstring::MethodRemovelast(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
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

Atanor* Atanorstring::MethodFormat(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	char buffer[101];
	string sformat = value;
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

Atanor* Atanorstring::MethodReplace(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//Replace an element with another

	string str = value;
	string reg = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	if (reg == "")
		return globalAtanor->Providestring(str);

	string rep = callfunc->Evaluate(1, contextualpattern, idthread)->String();

	str = s_replacestring(str, reg, rep);
	return globalAtanor->Providestring(str);
}

Atanor* Atanorstring::MethodEditdistance(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
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

Atanor* Atanorstring::MethodPop(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
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

Atanor* Atanorstring::MethodLast(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter should be a vector
	string res(String());
	if (res.size() == 0)
		return aNULL;
	res = c_char_index(res, res.size() - 1);
	return globalAtanor->Providestring(res);
}

Atanor* Atanorstring::MethodIndent(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
    int blanks = 5;
    if (callfunc->Size() == 1)
        blanks = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    string codeindente;
    IndentCode(value, codeindente, blanks);
	return globalAtanor->Providestring(codeindente);
}

#ifdef ATANOR_REGEX


Atanor* Atanorstring::MethodReplaceRgx(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	Atanor* kreg = callfunc->Evaluate(0, contextualpattern, idthread);

	string str = value;
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
		return globalAtanor->Returnerror(str, idthread);
	}
}

Atanor* Atanorstring::MethodRegex(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	Atanor* kreg = callfunc->Evaluate(0, contextualpattern, idthread);


	const sregex_token_iterator end;
	string str = value;
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


Atanor* Atanorstring::MethodSplitRegex(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {


	Atanor* kreg = callfunc->Evaluate(0, contextualpattern, idthread);

	string str = value;
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


Exporting Atanor* Atanorstring::Loophaskell(Atanor* recipient, Atanor* context, Atanor* environment, AtanorFunctionLambda* bd, short idthread) {
	Atanor* a;

	Atanorstring* it = globalAtanor->Providestring("");
	it->Setreference();

	char addvalue = 0;
	Locking* lock = NULL;
	if (context != aNOELEMENT) {
		addvalue = Selecttype(context);
		if (addvalue)
			lock = new Locking((AtanorObject*)context);
	}


	recipient->Putvalue( it, idthread);

	agnostring v(value);
	v.begin();
	while (!v.end()) {
		it->value = v.next();

		a = bd->Get(environment, aNULL, idthread);

		if (globalAtanor->Error(idthread) || a == aBREAK) {
			environment->Cleaning(idthread);
			if (a == aBREAK)
				break;

			it->Resetreference();
			recipient->Forcedclean();
			a->Release();
			context->Release();
			if (lock != NULL)
				delete lock;
			return globalAtanor->Errorobject(idthread);
		}

		if (!a->Reference())
			environment->Cleaning(idthread);
		else {
			a->Setreference();
			environment->Cleaning(idthread);
			a->Protect();
		}

		if (a != aNULL) {
			context = Storealongtype(context, a, idthread, addvalue);
			a->Release();
		}
	}

	it->Resetreference();
	recipient->Forcedclean();

	if (lock != NULL)
		delete lock;
	return context;
}

Exporting Atanor* Atanorstring::Filter(short idthread, Atanor* env, AtanorFunctionLambda* bd, Atanor* var, Atanor* kcont, Atanor* accu, Atanor* init, bool direct) {

	Atanor* returnval;

	bool first = false;
	Atanorstring* key = globalAtanor->Providestring("");
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
	agnostring v(value);
	v.begin();
	while (!v.end()) {
		key->value = v.next();
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

		if (!returnval->Reference())
			env->Cleaning(idthread);
		else {
			returnval->Setreference();
			env->Cleaning(idthread);
			returnval->Protect();
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


void AtanorLoopString::Callfunction() {

	AtanorCallFunction kfunc(function);

	Atanorint* ki = globalAtanor->Provideint(position);
	ki->Setreference();
	kfunc.arguments.push_back(this);
	kfunc.arguments.push_back(ki);
	Atanor* ke = kfunc.Get(aNULL, aNULL, globalAtanor->GetThreadid());
	ke->Release();
	ki->Resetreference();
}

Atanor* AtanorLoopString::Put(Atanor* context, Atanor* ke, short idthread) {
	if (ke->isVectorContainer()) {
		position = 0;
		interval.clear();
		for (int i = 0; i < ke->Size(); i++)
			interval.push_back(ke->getstring(i));
		value = interval[0];
		return aTRUE;
	}

	if (ke->Type() == a_sloop) {
		AtanorLoopString* kl = (AtanorLoopString*)ke;
		interval = kl->interval;
		value = kl->value;
		position = kl->position;
		return aTRUE;
	}

	if (interval.size() == 0) {
		position = 0;
		value = "";
		return aTRUE;
	}

	position = ke->Integer();
	if (position >= interval.size())
		position = position % interval.size();
	else
	if (position < 0)
		position = (interval.size() + position) % interval.size();
	value = interval[position];
	return aTRUE;
}


Atanor* AtanorLoopString::Vector(short idthread) {
	Atanorsvector* kvect = globalAtanor->Providesvector();
	kvect->values = interval;
	return kvect;
}



