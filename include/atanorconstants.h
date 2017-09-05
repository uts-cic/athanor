
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
filename   : atanorconstants.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanorconstants_h
#define atanorconstants_h

//--------------------------------------------------------------------
class AtanorGlobal;
class AtanorConstiteration;
class AtanorPredicate;

//--------------------------------------------------------------------
const uchar b_none = 0;
const uchar b_short = 1;
const uchar b_int = 2;
const uchar b_long = 4;
const uchar b_forcedlong = 8;
const uchar b_decimal = 16;
const uchar b_float = 32;
const uchar b_string = 64;
const uchar b_ustring = 128;
const uchar b_allstrings = b_string | b_ustring;
const uchar b_allnumbers = b_short | b_int | b_long | b_forcedlong | b_decimal | b_float;
const uchar b_longdecimal = b_long | b_decimal;
const uchar b_floats = b_decimal | b_float;
//--------------------------------------------------------------------
#define _debuginit(i,j) Atanordebug* _d=NULL;if (globalAtanor->debugmode) _d=new Atanordebug(i,j)
#define _debuginitmin(i) Atanordebug* _d=NULL;if (globalAtanor->debugmode) _d=new Atanordebug(i)
#define _debugpush(i) globalAtanor->Current(i, idthread); if (_d) _d->push()
#define _debugpop() if (_d) _d->pop()
#define _cleandebug if (_d) delete _d
//--------------------------------------------------------------------
Exporting Atanor* SelectContainer(Atanor* context, short idthread);
Exporting Atanor* Selectacontainer(Atanor* context, short idthread);
Exporting Atanor* Selectavector(Atanor* context);
Exporting Atanor* Selectamap(Atanor* context);
Exporting Atanor* Selectasvector(Atanor* context);
Exporting Atanor* Selectauvector(Atanor* context);
Exporting Atanor* Selectaivector(Atanor* context);
Exporting Atanor* Selectahvector(Atanor* context);
Exporting Atanor* Selectalvector(Atanor* context);
Exporting Atanor* Selectadvector(Atanor* context);
Exporting Atanor* Selectafvector(Atanor* context);
Exporting Atanor* Selectabvector(Atanor* context);
Exporting Atanor* Selectaprimemapss(Atanor* context);

void convertbytepositiontochar(vector<long>& v, agnostring& s);
void convertbytepositiontochar(vector<double>& v, agnostring& s);
//-----------------------------------------------------------------------------
Atanor* Mapcompare(Atanor*a, Atanor*b, AtanorGlobal* global);
//-----------------------------------------------------------------------------
Exporting char StringIndexes(string& svalue, Atanor* index, int& ileft, int& iright, short idthread);
Exporting char StringIndexes(wstring& svalue, Atanor* index, int& ileft, int& iright, short idthread);
//-----------------------------------------------------------------------------
unsigned long StringEditDistance(wstring& value, wstring& s2);
unsigned long StringEditDistance(string& value, string& s2);
string StringMinus(string v, string sub);
wstring StringMinus(wstring v, wstring sub);
//-----------------------------------------------------------------------------
bool IsLong(BLONG v);
bool IsShort(BLONG v);
//-----------------------------------------------------------------------------
bool v_comma_split_string(string& thestr, vector<string>& v);
bool v_comma_split_string(wstring& thestr, vector<wstring>& v);
bool v_comma_split_decimal(string& thestr, vector<float>& v);
bool v_comma_split_float(string& thestr, vector<double>& v);
bool v_comma_split_int(string& thestr, vector<long>& v);
bool v_comma_split_long(string& thestr, vector<BLONG>& v);
bool v_comma_split_byte(string& thestr, vector<uchar>& v);
bool v_comma_split_short(string& thestr, vector<short>& v);
//-----------------------------------------------------------------------------
#define ATANORCASEFUNCTION -9876
//-----------------------------------------------------------------------------
inline bool Arity(const unsigned long arity, long sz) {
	unsigned long a = 1 << sz;
	if ((arity&a) != a)
		return false;
	return true;
}
//-----------------------------------------------------------------------------
void Flattening(Atanor* ret, Atanor* res);
//-----------------------------------------------------------------------------
char restate_output(int o, long output);
char redirect_output(string& filename, long output);
void clear_output();
//-----------------------------------------------------------------------------
#ifdef ATANOR_REGEX
#if defined(WIN32) || defined(MAVERICK) 
#include <regex>
#else
#include <boost/regex.hpp>
#endif

#ifdef WIN32 
using namespace std::tr1;
#else
#ifndef MAVERICK
using namespace boost;
#endif
#endif

#ifdef MAVERICK
using std::regex;
using std::sregex_token_iterator;
using std::smatch;
using std::match_results;
using std::wregex;
using std::wsregex_token_iterator;
using std::wsmatch;
#elif WIN32
using std::regex;
using std::sregex_token_iterator;
using std::smatch;
using std::match_results;
#else
using boost::regex;
using boost::sregex_token_iterator;
using boost::smatch;
using boost::match_results;
#endif
#endif
//-----------------------------------------------------------------------------------
double localrandom(long mx);
//-----------------------------------------------------------------------------------
#ifdef WIN32
long gettimeofday(struct timeval *tv, struct timezone *tz);
#endif

typedef bool(*PauseBack)(void*);
bool WaitingFor(double tm, PauseBack pb, void* data);
long timeminus(double init, timeval& tempsfinal);
double plustime(timeval& tempsinitial, timeval& tempsfinal);
double minustime(timeval& tempsfinal, timeval& tempsinitial);
void Wait(double tm);
//-----------------------------------------------------------------------------------
bool DirectoryCreation(char* noms, long pos);
string NormalizeFileName(string n);
//-----------------------------------------------------------------------------------
char Selecttype(Atanor* context);
Atanor* Storealongtype(Atanor* context, Atanor* a, short idthread, char& addvalue);
//-----------------------------------------------------------------------------------
union w_u_char {
	wchar_t c;
	unsigned char cc[sizeof(wchar_t)];
};
//-----------------------------------------------------------------------------------
extern bool errorsprintf;
void wrongSprintf(const wchar_t* expression,
	const wchar_t* function,
	const wchar_t* file,
	unsigned int line,
	uintptr_t pReserved);
//-----------------------------------------------------------------------------------


//Open mode to open files with ifstream
#define openMode ios::in|ios::binary

const unsigned long P_NONE = 1 << 0;
const unsigned long P_ONE = 1 << 1;
const unsigned long P_TWO = 1 << 2;
const unsigned long P_THREE = 1 << 3;
const unsigned long P_FOUR = 1 << 4;
const unsigned long P_FIVE = 1 << 5;
const unsigned long P_SIX = 1 << 6;
const unsigned long P_SEVEN = 1 << 7;
const unsigned long P_EIGHT = 1 << 8;
const unsigned long P_NINE = 1 << 9;
const unsigned long P_TEN = 1 << 10;
const unsigned long P_ELEVEN = 1 << 11;
const unsigned long P_TWELVE = 1 << 12;
const unsigned long P_THIRTEEN = 1 << 13;
const unsigned long P_FOURTEEN = 1 << 14;
const unsigned long P_FIFTEEN = 1 << 15;
const unsigned long P_FULL = -1;
const unsigned long P_ATLEASTONE = P_FULL^P_NONE;
const unsigned long P_ATLEASTTWO = P_ATLEASTONE^P_ONE;
const unsigned long P_ATLEASTTHREE = P_ATLEASTTWO^P_TWO;
const unsigned long P_ATLEASTFOUR = P_ATLEASTTHREE^P_THREE;
const unsigned long P_ATLEASTFIVE = P_ATLEASTFOUR^P_FOUR;
const unsigned long P_ATLEASTSIX = P_ATLEASTFIVE^P_FIVE;
const unsigned long P_ATLEASTSEVEN = P_ATLEASTSIX^P_SIX;
const unsigned long P_ATLEASTEIGHT = P_ATLEASTSEVEN^P_SEVEN;
const unsigned long P_ATLEASTNINE = P_ATLEASTEIGHT^P_EIGHT;
const unsigned long P_ATLEASTTEN = P_ATLEASTNINE^P_NINE;
const unsigned long P_ATLEASTELEVEN = P_ATLEASTTEN^P_TEN;
const unsigned long P_ATLEASTTWELVE = P_ATLEASTELEVEN^P_ELEVEN;
const unsigned long P_ATLEASTTHIRTEEN = P_ATLEASTTWELVE^P_TWELVE;
const unsigned long P_ATLEASTFOURTEEN = P_ATLEASTTHIRTEEN^P_THIRTEEN;
const unsigned long P_ATLEASTFIFTEEN = P_ATLEASTFOURTEEN^P_FOURTEEN;

//--------------------------------------------------------------------------------
const long CIRCULARDETECTION = -25;
//--------------------------------------------------------------------------------
//Some external variables...
#define aNULL globalAtanor->gNULL
#define aUNIVERSAL globalAtanor->gUNIVERSAL
#define aTRUE globalAtanor->gTRUE
#define aFALSE globalAtanor->gFALSE

#define aMINUSTEN globalAtanor->gMINUSTEN
#define aMINUSNINE globalAtanor->gMINUSNINE
#define aMINUSEIGHT globalAtanor->gMINUSEIGHT
#define aMINUSSEVEN globalAtanor->gMINUSSEVEN
#define aMINUSSIX globalAtanor->gMINUSSIX
#define aMINUSFIVE globalAtanor->gMINUSFIVE
#define aMINUSFOUR globalAtanor->gMINUSFOUR
#define aMINUSTHREE globalAtanor->gMINUSTHREE
#define aMINUSTWO globalAtanor->gMINUSTWO
#define aMINUSONE globalAtanor->gMINUSONE
#define aZERO globalAtanor->gZERO
#define aONE globalAtanor->gONE
#define aTWO globalAtanor->gTWO
#define aTHREE globalAtanor->gTHREE
#define aFOUR globalAtanor->gFOUR
#define aFIVE globalAtanor->gFIVE
#define aSIX globalAtanor->gSIX
#define aSEVEN globalAtanor->gSEVEN
#define aEIGHT globalAtanor->gEIGHT
#define aNINE globalAtanor->gNINE
#define aTEN globalAtanor->gTEN

#define aEMPTYSTRING globalAtanor->gEMPTYSTRING

#define aBREAK globalAtanor->gBREAK
#define aCONTINUE globalAtanor->gCONTINUE
#define aRETURN globalAtanor->gRETURN
#define aPIPE globalAtanor->gPIPE
#define aNOELEMENT globalAtanor->gNOELEMENT
#define aDEFAULT globalAtanor->gDEFAULT
#define aEND globalAtanor->gEND
#define aRAISEERROR globalAtanor->gRAISEERROR
#define aBREAKFALSE globalAtanor->gBREAKFALSE
#define aBREAKTRUE globalAtanor->gBREAKTRUE
#define aAFFECTATION globalAtanor->gAFFECTATION
#define aITERNULL globalAtanor->gITERNULL
#define aFAIL globalAtanor->gFAIL
#define aCUT globalAtanor->gCUT
#define aSTOP globalAtanor->gSTOP
#define aHASKELL globalAtanor->gHASKELL

#define aNULLDECLARATION globalAtanor->gNULLDECLARATION

//---------------------------------------------------------------------------
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
//---------------------------------------------------------------------------
#ifdef DOSOUTPUT
#define conversion2dos(x) s_utf8_to_dos(USTR(x))
#else
#define conversion2dos(X) X
#endif

//---------------------------------------------------------------------------
const short u_kifintsz = sizeof(long);
const short u_kifdoublesz = sizeof(double);
const short u_kiflongsz = sizeof(BLONG);

union u_kifdouble {
	double v;
	unsigned short t[u_kifdoublesz];
	u_kifdouble(double e) {
		v = e;
	}
};


union u_kifinteger {
	long v;
	unsigned short t[u_kifintsz];
	u_kifinteger(long e) {
		v = e;
	}
};

union u_kiflong {
	BLONG v;
	unsigned short t[u_kiflongsz];
	u_kiflong(BLONG e) {
		v = e;
	}
};
//---------------------------------------------------------------------------
//The order should follow the order in which Createid are implemented in AtanorGlobal::RecordConstantNames
typedef enum{
	a_null,
	a_true, a_false, a_zero, a_one, a_boolean, a_minusone, a_byte, a_short, a_int, a_long, a_decimal, a_fraction, a_float,
	a_bloop, a_iloop, a_lloop, a_dloop, a_floop, 
	a_intthrough, a_longthrough, a_decimalthrough, a_floatthrough, 
	a_string, a_ustring, a_sloop, a_uloop, a_stringthrough, a_ustringthrough,
	a_constvector, a_vector, a_bvector, a_fvector, a_ivector, a_hvector, a_svector, a_uvector, a_dvector, a_lvector, a_list, a_vectorthrough,
	a_constmap, a_map, a_treemap, a_primemap, a_binmap, a_mapss, a_mapthrough,
	a_error, a_const, a_none, a_pipe,
	a_break, a_continue, a_return,
	a_empty, a_mainframe,
	a_call, a_callfunction, a_callthread, a_callmethod, a_callprocedure, a_callindex, a_callhaskell, a_lambda,
	a_variable,	a_declarations, a_instructions, a_function, a_frame, a_frameinstance, a_extension, 
	a_initial, a_iteration, a_default,
	a_forinrange, a_sequence, a_self, 
	a_idreturnvariable, a_breaktrue, a_breakfalse, a_vectormerge, 
	a_instructionequ, a_let, a_assign, a_atanor, 	
	a_this, a_index, a_interval, a_type, a_final,
	a_infinitive, a_cycle, a_replicate,
	a_fail, a_cut, a_stop, a_predicateentree, a_universal, a_asserta, a_assertz, a_retract, a_remove, a_predicatevar, a_predicate,
	a_term, a_instance, a_predicateruleelement, a_predicatecontainer, a_predicaterule, a_predicateinstruction, a_knowledgebase, 
	a_dependencybase, a_predicatedomain, a_predicatelaunch,
	a_predicateelement, a_parameterpredicate, a_predicateevaluate, a_dependency,
	a_stream, a_affectation,
	a_plusequ, a_minusequ, a_multiplyequ, a_divideequ, a_modequ, a_powerequ, a_shiftleftequ, a_shiftrightequ, a_orequ, a_xorequ, a_andequ, a_mergeequ, a_addequ,
	a_plus, a_minus, a_multiply, a_divide, a_power, a_shiftleft, a_shiftright, a_mod, a_or, a_xor, a_and, a_merge, a_add,
	a_less, a_more, a_same, a_different, a_lessequal, a_moreequal, 
	a_plusplus, a_minusminus, a_in, a_notin, a_match, a_bloc, a_blocloopin, a_filein, a_blocboolean, a_parameter,
	a_if, a_try, a_switch, a_while, a_for, a_catchbloc, a_conjunction, a_disjunction, a_haskell, a_forcedaffectation,
	a_square, a_cube, a_counter, a_synode, 
	a_modifydependency, a_actionvariable, a_haskelldeclaration
} atanorbasictypes;

inline bool Isnumber(short a) {
	if (a >= a_short && a <= a_floatthrough)
		return true;
	return false;
}

inline bool Istring(short a) {
	if (a >= a_string || a <= a_ustringthrough)
		return true;
	return false;
}

inline void jstringing(string& res, string& value) {
	if (value.find("\\") != -1)
		value = s_replacestrings(value, "\\", "\\\\");

	if (value.find("\"") == -1) {
		res += "\"";
		res += value;
		res += "\"";
	}
	else {
		string v = s_replacestrings(value, "\"", "\\\"");
		res += "\"";
		res += v;
		res += "\"";
	}
}

inline void stringing(string& res, string& value) {
	if (value.find("'") != -1) {
		if (value.find("\"") != -1) {
			string v = s_replacestrings(value, "\"", "\\\"");
			res += "\"";
			res += v;
			res += "\"";
		}
		else {
			res += "\"";
			res += value;
			res += "\"";
		}
	}
	else {
		res += "'";
		res += value;
		res += "'";
	}
}

inline void ustringing(wstring& res, wstring& value) {
	if (value.find(L"'") != -1) {
		if (value.find(L"\"") != -1) {
			wstring v = s_replacestring(value, L"\"", L"\\\"");
			res += L"\"";
			res += v;
			res += L"\"";
		}
		else {
			res += L"\"";
			res += value;
			res += L"\"";
		}
	}
	else {
		res += L"'";
		res += value;
		res += L"'";
	}
}

inline void sstringing(string& res, string value) {
	if (value.find("'") != -1) {
		if (value.find("\"") != -1) {
			string v = s_replacestrings(value, "\"", "\\\"");
			res += "\"";
			res += v;
			res += "\"";
		}
		else {
			res += "\"";
			res += value;
			res += "\"";
		}
	}
	else {
		res += "'";
		res += value;
		res += "'";
	}
}

inline void jstringing(stringstream& res, string& value) {
	if (value.find("\"") == -1)
		res << "\"" << value << "\"";
	else {
		string v = s_replacestrings(value, "\"", "\\\"");
		res << "'" << v << "'";
	}
}


inline void stringing(stringstream& res, string& value) {
	if (value.find("'") != -1) {
		if (value.find("\"") != -1)
			res << "@\"" << value << "\"@";
		else
			res << "\"" << value << "\"";
	}
	else
		res << "'" << value << "'";
}

inline void ujstringing(std::wstringstream& res, wstring& value) {
	if (value.find(L"\"") == -1)
		res << L"\"" << value << L"\"";
	else {
		wstring v = s_replacestring(value, L"\"", L"\\\"");
		res << L"'" << v << L"'";
	}
}


inline void ustringing(std::wstringstream& res, wstring& value) {
	if (value.find(L"'") != -1) {
		if (value.find(L"\"") != -1)
			res << L"@\"" << value << L"\"@";
		else
			res << L"\"" << value << L"\"";
	}
	else
		res << L"'" << value << L"'";
}
#endif









