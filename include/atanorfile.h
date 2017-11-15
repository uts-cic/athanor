
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
filename   : atanorfile.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanorfile_h
#define atanorfile_h

#include "atanorint.h"
#include "atanorivector.h"
#include "atanorsvector.h"
#include "atanoruvector.h"
#include "atanorstring.h"
#include "atanorustring.h"
#include "atanorbyte.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorfile;
//This typedef defines a type "fileMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorfile::*fileMethod)(Atanor* context, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanorfile : public AtanorObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging hmap<unsigned short, fileMethod> methods;
	static Exchanging hmap<string, string> infomethods;
	static Exchanging bin_hash<unsigned long> exported;

	static Exchanging short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	FILE* thefile;
	string filename;
	string op;
	bool signature;
	bool first;
	unsigned char fbuffer[4096];
	vector<uchar> fchars;

	//---------------------------------------------------------------------------------------------------------------------
	Atanorfile(string f, AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
		//Do not forget your variable initialisation
		filename = f;
		op = "r";
		thefile = NULL;
		signature = false;
		first = false;
	}

	Atanorfile(FILE* v = NULL) {
		//Do not forget your variable initialisation
		thefile = v;
		op = "r";
		signature = false;
		first = false;
	}

	~Atanorfile() {
		if (thefile != NULL)
			fclose(thefile);
	}

	//----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* value, short idthread) {

		return this;
	}

	Atanor* Get(Atanor* context, Atanor* value, short idthread) {
		Locking _lock(this);
		if (thefile == NULL || feof(thefile) || op != "rb")
			return aFALSE;

		if (context->isNumber()) {
			unsigned char c = fgetc(thefile);
			context->storevalue(c);
			return aTRUE;
		}

		string line = readoneline();
		context->storevalue(line);

		return aTRUE;
	}


	short Type() {
		return Atanorfile::idtype;
	}

	string Typename() {
		return "file";
	}

	bool isAtom() {
		return true;
	}

	bool isFile() {
		return true;
	}

	Atanor* Atom(bool forced = false) {
		return this;
	}

	bool duplicateForCall() {
		return false;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	bool isDeclared(short n) {
		if (exported.find(n) != exported.end())
			return true;
		return false;
	}

	Atanor* Newinstance(short, Atanor* f = NULL) {
		return new Atanorfile(NULL);
	}

	AtanorIteration* Newiteration(bool direction) {
		return aITERNULL;
	}

	static void AddMethod(AtanorGlobal* g, string name, fileMethod func, unsigned long arity, string infos);
	
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
	//Atanor* MethodSize(AtanorGlobal* global,Atanor* context, short idthread, AtanorCall* callfunc) {return aZERO;}
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

		if (op == "r" || op == "w" || op == "a")
			op += "b";

#ifdef WIN32
		fopen_s(&thefile, STR(filename), STR(op));
#else
		thefile=fopen(STR(filename),STR(op));
#endif

		if (thefile == NULL)
			return globalAtanor->Returnerror("Cannot open this file", idthread);

		if (op[0] == 'r')
			consume_header();

		return aTRUE;
	}

	Atanor* MethodOpenRead(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (thefile != NULL)
			return globalAtanor->Returnerror("File already open", idthread);
		filename = callfunc->Evaluate(0, aNULL, idthread)->String();
		op = "rb";
#ifdef WIN32
		fopen_s(&thefile, STR(filename), STR(op));
#else
		thefile = fopen(STR(filename), STR(op));
#endif
		if (thefile == NULL)
			return globalAtanor->Returnerror("Cannot open this file", idthread);

		consume_header();
		return aTRUE;
	}

	Atanor* MethodOpenWrite(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (thefile != NULL)
			return globalAtanor->Returnerror("File already open", idthread);
		filename = callfunc->Evaluate(0, aNULL, idthread)->String();
		op = "wb";
		first = true;
#ifdef WIN32
		fopen_s(&thefile, STR(filename), STR(op));
#else
		thefile = fopen(STR(filename), STR(op));
#endif
		if (thefile == NULL)
			return globalAtanor->Returnerror("Cannot open this file", idthread);
		return aTRUE;
	}

	Atanor* MethodOpenAppend(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (thefile != NULL)
			return globalAtanor->Returnerror("File already open", idthread);
		filename = callfunc->Evaluate(0, aNULL, idthread)->String();
		op = "ab";
#ifdef WIN32
		fopen_s(&thefile, STR(filename), STR(op));
#else
		thefile = fopen(STR(filename), STR(op));
#endif
		if (thefile == NULL)
			return globalAtanor->Returnerror("Cannot open this file", idthread);
		return aTRUE;
	}


	Atanor* MethodClose(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (thefile != NULL) {
			fclose(thefile);
			thefile = NULL;
			return aTRUE;
		}
		return aFALSE;
	}

	Atanor* MethodSignature(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);

		if (callfunc->Size() == 1) {
			signature = callfunc->Evaluate(0, context, idthread)->Boolean();
			return aTRUE;
		}

		if (thefile != NULL) {
			uchar c = fgetc(thefile);
			uchar cc = fgetc(thefile);

			if (c == 255 && cc == 254) {
				ungetc(cc, thefile);
				ungetc(c, thefile);
				return globalAtanor->Provideint(16);
			}

			uchar ccc = fgetc(thefile);

			ungetc(ccc, thefile);
			ungetc(cc, thefile);
			ungetc(c, thefile);

			if (c == 239 && cc == 187 && ccc == 191) {
				signature = true;
				return aEIGHT;
			}
		}

		return aZERO;
	}


	Atanor* MethodGet(Atanor* context, short idthread, AtanorCall* callfunc) {
		if (thefile == NULL || feof(thefile) || op != "rb")
			return globalAtanor->Returnerror("Wrong access to the file", idthread);

		if (context->isString()) {
			wchar_t c = getc(true);
			if (context->Type() == a_ustring)
				return new Atanorustring(c);

			agnostring s(c);
			return globalAtanor->Providestring(s);
		}

		uchar c = get();
		return new Atanorbyte(c);
	}

	Atanor* MethodTell(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (thefile == NULL || feof(thefile) || op != "rb")
			return globalAtanor->Returnerror("Wrong access to the file", idthread);

		return globalAtanor->Provideint(tell());
	}

	Atanor* MethodSeek(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (thefile == NULL || feof(thefile) || op != "rb")
			return globalAtanor->Returnerror("Wrong access to the file", idthread);
		long i = callfunc->Evaluate(0, context, idthread)->Integer();
		if (seek(i))
			return aTRUE;
		return aFALSE;
	}

	Atanor* MethodUnget(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (thefile == NULL || feof(thefile) || op != "rb")
			return globalAtanor->Returnerror("Wrong access to the file", idthread);

		long c = callfunc->Evaluate(0, aNULL, idthread)->Integer();
		ungetc(c, thefile);
		return aTRUE;
	}

	Atanor* MethodWrite(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (thefile == NULL || feof(thefile) || op == "rb")
			return globalAtanor->Returnerror("Wrong access to the file", idthread);

		if (first) {
			if (signature) {
				putc(239, thefile);
				putc(187, thefile);
				putc(191, thefile);
			}
			first = false;
		}

		string s = callfunc->Evaluate(0, aNULL, idthread)->String();
		fwrite(STR(s), 1, s.size(), thefile);
		return aTRUE;
	}

	Atanor* MethodWriteln(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (thefile == NULL || feof(thefile) || op == "rb")
			return globalAtanor->Returnerror("Wrong access to the file", idthread);

		if (first) {
			if (signature) {
				putc(239, thefile);
				putc(187, thefile);
				putc(191, thefile);
			}
			first = false;
		}

		string s = callfunc->Evaluate(0, aNULL, idthread)->String();
		s += Endl;
		fwrite(STR(s), 1, s.size(), thefile);
		return aTRUE;
	}

	Atanor* MethodWritebin(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (thefile == NULL || feof(thefile) || op == "rb")
			return globalAtanor->Returnerror("Wrong access to the file", idthread);

		if (first) {
			if (signature) {
				putc(239, thefile);
				putc(187, thefile);
				putc(191, thefile);
			}
			first = false;
		}

		uchar s = callfunc->Evaluate(0, aNULL, idthread)->Byte();
		fputc(s, thefile);
		return aTRUE;
	}

	Atanor* MethodReadoneline(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (thefile == NULL || feof(thefile) || op != "rb")
			return globalAtanor->Returnerror("Wrong access to the file", idthread);

		string s = readoneline();
		return globalAtanor->Providestring(s);
	}

	Atanor* MethodRead(Atanor* context, short idthread, AtanorCall* callfunc);

	Atanor* MethodEof(Atanor* context, short idthread, AtanorCall* callfunc) {
		Locking _lock(this);
		if (thefile == NULL || feof(thefile))
			return aTRUE;
		return aFALSE;
	}

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
	//is returned by the Declaration method.
	Atanor* CallMethod(short idname, Atanor* context, short idthread, AtanorCall* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		return (this->*methods[idname])(context, idthread, callfunc);
	}


	//---------------------------------------------------------------------------------------------------------------------

	void unget(uchar c) {
		ungetc(c, thefile);
	}

	void consume_header() {
		uchar c = fgetc(thefile);
		uchar cc = fgetc(thefile);
		uchar ccc = fgetc(thefile);

		if (c == 239 && cc == 187 && ccc == 191) {
			signature = true;
			return;
		}

		ungetc(ccc, thefile);
		ungetc(cc, thefile);
		ungetc(c, thefile);
	}


	unsigned char get() {
		return fgetc(thefile);
	}

	BULONG getc(bool utf8) {
		if (!utf8)
			return get();

		unsigned char c = get();
		if (c < 0x0080)
			return c;

		unsigned char utf[] = { 0, 0, 0, 0, 0, 0 };
		BULONG code = c;
		if ((c & 0xE0) == 0xC0) {
			//We take two bytes
			utf[1] = get();
			if ((utf[1] & 0xC0) != 0x80) {
				unget(utf[1]);
				return c;
			}
			code = (c ^ 0xc0) << 6;
			code |= utf[1] ^ 0x80;
			return code;
		}
		//we take three
		if ((c & 0xF0) == 0xE0) {
			utf[1] = get();
			if ((utf[1] & 0xC0) != 0x80) {
				unget(utf[1]);
				return c;
			}
			utf[2] = get();
			if ((utf[2] & 0xC0) != 0x80) {
				unget(utf[1]);
				unget(utf[2]);
				return c;
			}
			code = (c ^ 0xE0) << 12;
			code |= (utf[1] ^ 0x80) << 6;
			code |= (utf[2] ^ 0x80);
			return code;
		}

		//we take four
		if ((c & 0xF8) == 0xF0) {
			utf[1] = get();
			if ((utf[1] & 0xC0) != 0x80) {
				unget(utf[1]);
				return c;
			}
			utf[2] = get();
			if ((utf[2] & 0xC0) != 0x80) {
				unget(utf[1]);
				unget(utf[2]);
				return c;
			}
			utf[3] = get();
			if ((utf[3] & 0xC0) != 0x80) {
				unget(utf[1]);
				unget(utf[2]);
				unget(utf[3]);
				return c;
			}
			code = (c ^ 0xF0) << 18;
			code |= (utf[1] ^ 0x80) << 12;
			code |= (utf[2] ^ 0x80) << 6;
			code |= (utf[3] ^ 0x80);
			return code;
		}

		//we take five
		if ((c & 0xFC) == 0xF8) {
			utf[1] = get();
			if ((utf[1] & 0xC0) != 0x80) {
				unget(utf[1]);
				return c;
			}
			utf[2] = get();
			if ((utf[2] & 0xC0) != 0x80) {
				unget(utf[1]);
				unget(utf[2]);
				return c;
			}
			utf[3] = get();
			if ((utf[3] & 0xC0) != 0x80) {
				unget(utf[1]);
				unget(utf[2]);
				unget(utf[3]);
				return c;
			}
			utf[4] = get();
			if ((utf[4] & 0xC0) != 0x80) {
				unget(utf[1]);
				unget(utf[2]);
				unget(utf[3]);
				unget(utf[4]);
				return c;
			}
			code = (c ^ 0xF8) << 24;
			code |= (utf[1] ^ 0x80) << 18;
			code |= (utf[2] ^ 0x80) << 12;
			code |= (utf[3] ^ 0x80) << 6;
			code |= (utf[4] ^ 0x80);
			return code;
		}

		//we take six
		if ((c & 0xFF) == 0xFC) {
			utf[1] = get();
			if ((utf[1] & 0xC0) != 0x80) {
				unget(utf[1]);
				return c;
			}
			utf[2] = get();
			if ((utf[2] & 0xC0) != 0x80) {
				unget(utf[1]);
				unget(utf[2]);
				return c;
			}
			utf[3] = get();
			if ((utf[3] & 0xC0) != 0x80) {
				unget(utf[1]);
				unget(utf[2]);
				unget(utf[3]);
				return c;
			}
			utf[4] = get();
			if ((utf[4] & 0xC0) != 0x80) {
				unget(utf[1]);
				unget(utf[2]);
				unget(utf[3]);
				unget(utf[4]);
				return c;
			}
			utf[5] = get();
			if ((utf[4] & 0xC0) != 0x80) {
				unget(utf[1]);
				unget(utf[2]);
				unget(utf[3]);
				unget(utf[4]);
				unget(utf[5]);
				return c;
			}
			code = (c ^ 0xFC) << 30;
			code |= (utf[1] ^ 0x80) << 24;
			code |= (utf[2] ^ 0x80) << 18;
			code |= (utf[3] ^ 0x80) << 12;
			code |= (utf[4] ^ 0x80) << 6;
			code |= (utf[5] ^ 0x80);
			return code;
		}
		return c;
	}

	long readnb(long nb) {
		fchars.clear();
		fbuffer[0] = 0;

		long nbread, i;
		long total = 0;

		while (nb) {
			if (feof(thefile))
				return total;

			nbread = fread(fbuffer, 1, 4095, thefile);
			total += nbread;

			if (nbread == 0)
				return total;

			if (nb != -1) {
				if (nbread >= nb) {
					nbread = nb;
					nb = 0;
				}
				else
					nb -= nbread;
			}

			fbuffer[nbread] = 0;
			i = 0;
			if (first) {
				if (signature) {
					if (fbuffer[0] == 239 && fbuffer[1] == 187 && fbuffer[2] == 191)
						i = 3;
				}
				first = false;
			}
			for (; i < nbread; i++)
				fchars.push_back(fbuffer[i]);
		}
		return total;
	}

	string read(long nb) {
		if (first) {
			//to read the signature...
			uchar c = get();
			unget(c);
		}

		string str;

		long nbread;

		while (nb) {
			if (feof(thefile))
				return str;

			fbuffer[0] = 0;
			nbread = fread(fbuffer, 1, 4095, thefile);

			if (nbread == 0)
				return str;

			if (nb != -1) {
				if (nbread >= nb) {
					nbread = nb;
					nb = 0;
				}
				else
					nb -= nbread;
			}

			fbuffer[nbread] = 0;
			if (first) {
				first = false;
				if (signature) {
					if (fbuffer[0] == 239 && fbuffer[1] == 187 && fbuffer[2] == 191) {
						str += (char*)fbuffer + 3;
						continue;
					}
				}
			}

			str += (char*)fbuffer;
		}
		return str;
	}

	wstring wread(long nb) {
		wstring str;

		BULONG b;
		while (nb) {
			if (feof(thefile))
				return str;
			b = getc(true);
			str += b;
		}
		return str;
	}

	string readoneline() {
		fbuffer[0] = 0;
		fgets((char*)fbuffer, 4095, thefile);
		if (first) {
			if (signature) {
				if (fbuffer[0] == 239 && fbuffer[1] == 187 && fbuffer[2] == 191)
					memcpy(fbuffer, fbuffer + 3, strlen((char*)fbuffer) - 3);
			}
			first = false;
		}
		return (char*)fbuffer;
	}

	void readall(vector<string>& fstrings) {
		fstrings.clear();

		while (!feof(thefile)) {
			fbuffer[0] = 0;
			fgets((char*)fbuffer, 4095, thefile);
			if (first) {
				if (signature) {
					if (fbuffer[0] == 239 && fbuffer[1] == 187 && fbuffer[2] == 191)
						memcpy(fbuffer, fbuffer + 3, strlen((char*)fbuffer) - 3);
				}
				first = false;
			}
			fstrings.push_back((char*)fbuffer);
		}
	}

	void readall(vector<wstring>& fstrings) {
		fstrings.clear();
		agnostring s;

		while (!feof(thefile)) {
			fbuffer[0] = 0;
			fgets((char*)fbuffer, 4095, thefile);
			if (first) {
				if (signature) {
					if (fbuffer[0] == 239 && fbuffer[1] == 187 && fbuffer[2] == 191)
						memcpy(fbuffer, fbuffer + 3, strlen((char*)fbuffer) - 3);
				}
				first = false;
			}
			s = fbuffer;
			fstrings.push_back(s.latintounicode());
		}
	}

	void readnb(long nb, vector<string>& fstrings) {
		fstrings.clear();

		size_t total = 0;
		while (!feof(thefile)) {
			fbuffer[0] = 0;
			fgets((char*)fbuffer, 4095, thefile);
			if (first) {
				if (signature) {
					if (fbuffer[0] == 239 && fbuffer[1] == 187 && fbuffer[2] == 191)
						memcpy(fbuffer, fbuffer + 3, strlen((char*)fbuffer) - 3);
				}
				first = false;
			}
			fstrings.push_back((char*)fbuffer);
			total += fstrings.back().size();
			if (total >= nb)
				return;
		}
	}

	void readnb(long nb, vector<wstring>& fstrings) {
		fstrings.clear();
		agnostring s;

		long total = 0;
		while (!feof(thefile)) {
			fbuffer[0] = 0;
			fgets((char*)fbuffer, 4095, thefile);
			if (first) {
				if (signature) {
					if (fbuffer[0] == 239 && fbuffer[1] == 187 && fbuffer[2] == 191)
						memcpy(fbuffer, fbuffer + 3, strlen((char*)fbuffer) - 3);
				}
				first = false;
			}
			s = (char*)fbuffer;
			fstrings.push_back(s.latintounicode());
			total += s.size();
			if (total >= nb)
				return;
		}
	}

	long tell() {
		if (thefile == NULL || feof(thefile) || feof(thefile))
			return -1;
		return ftell(thefile);
	}

	bool seek(long i) {
		if (thefile == NULL || feof(thefile) || feof(thefile))
			return false;

		fseek(thefile, i, SEEK_SET);
		return true;
	}

	Atanor* find(Atanor* context, agnostring& sx, bool u) {
		if (thefile == NULL || feof(thefile) || feof(thefile))
			return aNULL;

		BULONG c;
		bool utf8 = false;
		if (u) {
			sx = sx.tolower();
			utf8 = sx.isutf8();
		}
		string& s = sx;
		int i = 0;
		long p = tell();
		Atanor* v = NULL;
		if (context->isVectorContainer())
			v = Selectaivector(context);

		c = getc(utf8);
		while (!feof(thefile)) {
			if (utf8) {
				if (sx.end()) {
					if (v == NULL)
						return globalAtanor->Provideint(p);
					v->storevalue(p);
					sx.charpos = 0;
					sx.bytepos = 0;
				}
				c = c_to_lower(c);
				if (c != sx.nextcode()) {
					sx.charpos = 0;
					sx.bytepos = 0;
					p = tell();
				}
			}
			else {
				if (i >= s.size()) {
					if (v == NULL)
						return globalAtanor->Provideint(p);
					v->storevalue(p);
					i = 0;
				}
				if (u)
					c = c_to_lower(c);
				if ((uchar)c == (uchar)s[i])
					i++;
				else {
					i = 0;
					p = tell();
				}
			}
			c = getc(utf8);
		}
		if (v == NULL)
			return aMINUSONE;
		return v;
	}

	//------------------------------------------------------------------------------------
	Atanor* in(Atanor* context, Atanor* a, short idthread) {
		agnostring s(a->String());
		Atanor* res = find(context, s, s.isutf8());
		if (context->isBoolean()) {
			if (res == aMINUSONE)
				return aFALSE;
			res->Release();
			return aTRUE;
		}
		return res;
	}

	//------------------------------------------------------------------------------------
	long Size() {
		Locking _lock(this);
		struct stat scible;
		int stcible = -1;
		long size = -1;

		if (thefile != NULL) {
#if (_MSC_VER >= 1900)
			stcible = fstat(_fileno(thefile), &scible);
#else
#if  defined(WIN32) | defined(APPLE)
			stcible = fstat(thefile->_file, &scible);
#else
			stcible = fstat(thefile->_fileno, &scible);
#endif
#endif
			if (stcible >= 0)
				size = scible.st_size;
		}
		return size;
	}

	void Clear() {
		//To set a variable back to empty
		if (thefile != NULL)
			fclose(thefile);
		thefile = NULL;
	}

	void Clean() {
		//Your cleaning code
	}

	string String() {
		return filename;
	}

	long Integer() {
		return Size();
	}

	float Decimal() {
		return (float)Size();
	}

	double Float() {
		return Size();
	}

	BLONG Long() {
		return Size();
	}

	bool Boolean() {
		if (thefile == NULL)
			return false;
		return true;
	}

};

#endif







