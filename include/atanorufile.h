
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
filename   : atanorufile.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanorufile_h
#define atanorufile_h
#include "atanorustring.h"
#include "atanorint.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorufile;
//This typedef defines a type "ufileMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorufile::*ufileMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanorufile : public AtanorObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging hmap<unsigned short, ufileMethod> methods;
	static Exchanging hmap<string, string> infomethods;
	static Exchanging bin_hash<unsigned long> exported;

	static Exchanging short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	std::wfstream* thefile;
	string filename;
	string op;
	bool signature;
	bool first;

	//---------------------------------------------------------------------------------------------------------------------
	Atanorufile(AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
		//Do not forget your variable initialisation
		thefile = NULL;
		op = "r";
		signature = false;
		first = false;
	}

	Atanorufile() {
		//Do not forget your variable initialisation
		thefile = NULL;
		op = "r";
		signature = false;
		first = false;
	}

	//----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* v, Atanor* i, short idthread);
	Atanor* Get(Atanor* context, Atanor* v, short idthread);


	short Type() {
		return Atanorufile::idtype;
	}

	string Typename() {
		return "ufile";
	}

	//The next two functions work in conjunction...
	//If your object is very atomic (such as a number or a string)
	//Then when it is passed to a function or stored into a container,
	//it might be worth duplicating it.
	// In that case: duplicateForCall should return true...

	//However if your object is complex and probably unique through out the code
	//The duplicateForCall should return false, and Atom should always reduce to a "return this;" only...
	Atanor* Atom(bool forced = false) {
		return this;
	}

	bool duplicateForCall() {
		return false;
	}

	bool isFile() {
		return true;
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
		return new Atanorufile(NULL);
	}

	AtanorIteration* Newiteration(bool direction) {
		return aITERNULL;
	}

	static void AddMethod(AtanorGlobal* g, string name, ufileMethod func, unsigned long arity, string infos);
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
	//This is a sample  of a function that could be implemented for your needs.
	Atanor* MethodInitial(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (callfunc->Size() == 0)
			return aTRUE;
		filename = callfunc->Evaluate(0, aNULL, idthread)->String();
		op = "r";
		first = false;
		if (callfunc->Size() == 2) {
			op = callfunc->Evaluate(1, aNULL, idthread)->String();
			if (op != "r" && op != "a" && op != "w")
				return globalAtanor->Returnerror("Unknown operation for this file", idthread);
			if (op == "w")
				first = true;
		}

		switch (op[0]) {
		case 'r':
			thefile = new std::wfstream(STR(filename), std::wios::in | std::wios::binary);
			break;
		case 'w':
			thefile = new std::wfstream(STR(filename), std::wios::out | std::wios::binary);
			break;
		case 'a':
			thefile = new std::wfstream(STR(filename), std::wios::app | std::wios::binary);
			break;
		}

		if (thefile->fail()) {
			delete thefile;
			thefile = NULL;
			return globalAtanor->Returnerror("Cannot open this file", idthread);
		}

		return aTRUE;
	}

	Atanor* MethodOpenRead(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (thefile != NULL)
			return globalAtanor->Returnerror("File already open", idthread);
		filename = callfunc->Evaluate(0, aNULL, idthread)->String();
		op = "r";
		thefile = new std::wfstream(STR(filename), std::wios::in | std::wios::binary);
		if (thefile->fail()) {
			delete thefile;
			thefile = NULL;
			return globalAtanor->Returnerror("Cannot open this file", idthread);
		}
		return aTRUE;
	}

	Atanor* MethodOpenWrite(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (thefile != NULL)
			return globalAtanor->Returnerror("File already open", idthread);
		filename = callfunc->Evaluate(0, aNULL, idthread)->String();
		op = "w";
		thefile = new std::wfstream(STR(filename), std::wios::out | std::wios::binary);
		if (thefile->fail()) {
			delete thefile;
			thefile = NULL;
			return globalAtanor->Returnerror("Cannot open this file", idthread);
		}
		return aTRUE;
	}

	Atanor* MethodOpenAppend(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (thefile != NULL)
			return globalAtanor->Returnerror("File already open", idthread);
		filename = callfunc->Evaluate(0, aNULL, idthread)->String();
		op = "a";
		thefile = new std::wfstream(STR(filename), std::wios::app | std::wios::binary);
		if (thefile->fail()) {
			delete thefile;
			thefile = NULL;
			return globalAtanor->Returnerror("Cannot open this file", idthread);
		}
		return aTRUE;
	}


	Atanor* MethodClose(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (thefile != NULL) {
			thefile->close();
			thefile = NULL;
			return aTRUE;
		}
		return aFALSE;
	}

	Atanor* MethodGet(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (thefile == NULL || thefile->eof() || op != "r")
			return globalAtanor->Returnerror("Wrong access to the file", idthread);

		wchar_t c = thefile->get();
		return new Atanorustring(c);
	}

	Atanor* MethodTell(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (thefile == NULL || thefile->eof() || op != "r")
			return globalAtanor->Returnerror("Wrong access to the file", idthread);

		return globalAtanor->Provideint(thefile->tellg());
	}

	Atanor* MethodSeek(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (thefile == NULL || thefile->eof() || op != "r")
			return globalAtanor->Returnerror("Wrong access to the file", idthread);

		long i = callfunc->Evaluate(0, context, idthread)->Integer();

		if (thefile->seekg(i))
			return aTRUE;

		return aFALSE;
	}

	Atanor* MethodUnget(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (thefile == NULL || thefile->eof() || op != "r")
			return globalAtanor->Returnerror("Wrong access to the file", idthread);

		thefile->unget();
		return aTRUE;
	}

	Atanor* MethodWrite(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (thefile == NULL || thefile->eof() || op == "r")
			return globalAtanor->Returnerror("Wrong access to the file", idthread);

		if (first) {
			if (signature) {
				thefile->put(255);
				thefile->put(254);
			}
			first = false;
		}

		wstring s = callfunc->Evaluate(0, aNULL, idthread)->UString();
		*thefile << s;
		return aTRUE;
	}

	Atanor* MethodWriteln(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (thefile == NULL || thefile->eof() || op == "r")
			return globalAtanor->Returnerror("Wrong access to the file", idthread);

		if (first) {
			if (signature) {
				thefile->put(255);
				thefile->put(254);
			}
			first = false;
		}

		wstring s = callfunc->Evaluate(0, aNULL, idthread)->UString();
#ifdef WIN32
		s += L"\r\n";
#else
		s+=L"\n";
#endif
		*thefile << s;
		return aTRUE;
	}

	Atanor* MethodReadoneline(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (thefile == NULL || thefile->eof() || op != "r")
			return globalAtanor->Returnerror("Wrong access to the file", idthread);

		wstring s;
		getline(*thefile, s);
		return globalAtanor->Provideustring(s);
	}

	Atanor* MethodRead(Atanor* context, short idthread, AtanorCall* callfunc);

	Atanor* MethodEof(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (thefile == NULL || thefile->eof())
			return aTRUE;
		return aFALSE;
	}

	Atanor* MethodSignature(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		signature = callfunc->Evaluate(0, context, idthread)->Boolean();
		return aTRUE;
	}

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
	//is returned by the Declaration method.
	Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		return (this->*methods[idname])(contextualpattern, idthread, callfunc);
	}

	void Clear(AtanorGlobal* g) {
		//To set a variable back to empty
	}

	void Clean() {
		//Your cleaning code
	}

	string String() {
		return filename;
	}

	long readnb(wstring& v, long nb) {
		if (first) {
			wchar_t c = thefile->get();
			w_u_char wc;
			wc.c = c;
			if (wc.cc[0] == 255 && wc.cc[1] == 254)
				signature = true;
			else
				v += c;
			first = false;
		}

		wchar_t fbuffer[4096];
		long i;
		long rd;
		long total = 0;
		while (total != nb) {
			if (thefile->eof())
				return total;
			thefile->read(fbuffer, 4096);
			rd = thefile->gcount();
			total += rd;
			for (i = 0; i < rd; i++)
				v += fbuffer[i];
		}

		return total;
	}

	long readnb(vector<wstring>& v, long nb) {
		wstring w;

		if (first) {
			wchar_t c = thefile->get();
			w_u_char wc;
			wc.c = c;
			if (wc.cc[0] == 255 && wc.cc[1] == 254)
				signature = true;
			else {
				w = c;
				v.push_back(w);
			}
			first = false;
		}

		wchar_t fbuffer[4096];
		long i;
		long rd;
		long total = 0;

		while (total != nb) {
			if (thefile->eof())
				return total;
			thefile->read(fbuffer, 4095);
			rd = thefile->gcount();
			total += rd;
			for (i = 0; i < rd; i++) {
				w = fbuffer[i];
				v.push_back(w);
			}
		}

		return total;
	}

	void readall(wstring& v) {
		if (first) {
			wchar_t c = thefile->get();
			w_u_char wc;
			wc.c = c;
			if (wc.cc[0] == 255 && wc.cc[1] == 254)
				signature = true;
			else
				v += c;
			first = false;
		}
		wstring line;
		while (!thefile->eof()) {
			getline(*thefile, line);
			v += line;
		}
	}


	void readall(vector<wstring>& v) {
		wstring w;
		if (first) {
			wchar_t c = thefile->get();
			w_u_char wc;
			wc.c = c;
			if (wc.cc[0] == 255 && wc.cc[1] == 254)
				signature = true;
			else {
				if (c == 13 || c == 10)
					v.push_back(L"");
				else
					w = c;
			}
			first = false;
		}

		wstring line;
		while (!thefile->eof()) {
			getline(*thefile, line);
			if (w != L"") {
				w = w + line;
				v.push_back(w);
				w = L"";
			}
			else
				v.push_back(line);
		}
	}


	wchar_t Get() {
		wchar_t c = thefile->get();
		if (first) {
			w_u_char wc;
			wc.c = c;
			if (wc.cc[0] == 255 && wc.cc[1] == 254) {
				c = thefile->get();
				signature = true;
			}
			first = false;
		}
		return c;
	}


	Atanor* find(Atanor* context, wstring& s) {
		if (thefile == NULL || thefile->eof())
			return aNULL;

		wchar_t c = Get();
		long p = thefile->tellg();
		Atanor* v = NULL;
		if (context->isContainer())
			v = Selectaivector(context);

		s = s_to_lower(s);

		long sz = s.size();
		long i = 0;
		while (!thefile->eof()) {
			if (i == sz) {
				if (v == NULL)
					return globalAtanor->Provideint(p);
				v->storevalue(p);
				i = -1;
			}
			else {
				if (c_to_lower(c) != s[i]) {
					p = thefile->tellg();
					i = -1;
				}
			}
			c = Get();
			i++;
		}
		if (v == NULL)
			return aMINUSONE;
		return v;
	}

	//------------------------------------------------------------------------------------
	Atanor* in(Atanor* context, Atanor* a, short idthread) {
		wstring s = a->UString();
		return find(context, s);
	}

	//------------------------------------------------------------------------------------
	void Clear() {
		//To set a variable back to empty
		if (thefile != NULL)
			thefile->close();
		thefile = NULL;
	}

};
#endif





