
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
filename   : x_node.h
Date       : 2017/04/01
Purpose    :
Programmer : Claude ROUX
Reviewer   :
*/

//Kif Formalism
// we use the following rules for pre-tokenization: <,>,^,=,&  double quote string (<), simple quote string(>), token (^), number (=),punctuation (&)
// '!' is used to not keep tracks of these specific characters in the final output...
// '%' is used to not store substrings within the analysis tree
//^ --> we use tokens
//For predicate vectors...
//For predicate map... The order is important for the unification
//< --> we use the doublestring map
//> --> we use the simplestring map
//= --> we use numbers


#ifndef x_node_h
#define x_node_h

#include <stdio.h>
#include <string.h>
#include <ostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include <sys/stat.h>

using std::stringstream;
using std::string;
using std::map;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::fstream;
using std::cout;
using std::cerr;
using std::endl;


typedef enum { PARTIAL, FULL } x_parsing_mode;

class x_node {
public:

	string token;
	string value;
	char init;
	vector<x_node*> nodes;
	long start;
	long end;
	bool recursive;

	x_node(bool rec = true) {
		recursive = rec;
		init = 0;
		start = 0;
		end = 0;
	}

	x_node(string a, string v, x_node* x = NULL) {
		recursive = true;
		token = a;
		value = v;
		init = 0;
		if (x == NULL) {
			start = 0;
			end = 0;
		}
		else {
			start = x->start;
			end = x->end;
		}
	}

	~x_node() {
		if (recursive) {
			for (size_t i = 0; i < nodes.size(); i++)
				delete nodes[i];
		}
	}

	void set(string& t, string& v) {
		token = t;
		value = v;
		init = 1;
	}


	char append(x_node* n) {
		if (n != NULL && (n->init == 1 || n->nodes.size() > 0)) {
			if (n->init == 1)
				nodes.push_back(n);
			else {
				for (size_t i = 0; i < n->nodes.size(); i++)
					nodes.push_back(n->nodes[i]);
				n->nodes.clear();
				delete n;
			}
			return 1;
		}
		return 0;
	}

	void pop(int i) {
		while (i > 0) {
			x_node* n = nodes[nodes.size() - 1];
			nodes.pop_back();
			delete n;
			i--;
		}
	};
};

class x_couple {
public:
	long pos;
	int intoken;
	x_node* tree;
	string values;

	x_couple(long p, int itok, x_node* s, string& lret) {
		pos = p;
		intoken = itok;
		tree = s;
		values = lret;
	}
	void set(string& lret) {
		lret = values;
	}
};


class x_reading {
public:

	vector<string> stack;
	vector<long> stackln;
	vector<char> stacktype;

	long pos;
	string currentbuffer;
	string name;
	bool readstrings;
	bool concatenatedots;

	unsigned char table[255];

	virtual void Initable() {
		int i = 0;
		unsigned char c;
		for (c = 65; c <= 90; c++)
			table[i++] = c;
		for (c = 97; c <= 122; c++)
			table[i++] = c;
		for (c = 128; c < 255; c++)
			table[i++] = c;
		table[i++] = '_';
		table[i++] = '#';
		table[i] = 0;
	}

	virtual void seek(long p) {
		pos = p;
	}

	virtual char eof() { return 0; }

	long tell(void) {
		return pos;
	}

	virtual unsigned char peek() {
		return 0;
	}

	virtual char get() {
		return -1;
	}

	virtual string read(int sz) { return ""; }
	virtual void close() {}
	virtual char error() {
		if (pos == -1)
			return 1;
		return 0;
	}
	virtual void reset(long p) {}

	virtual bool digitonly(string& thestr) {
		size_t i = 0;
		static char tabs[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', 0 };
		if (thestr[i] == '-')
			i++;
		for (; i < thestr.size(); i++)
		if (strchr(tabs, thestr[i]) == NULL)
			return false;
		return true;
	}

	virtual long Length() {
		return -1;
	}

	virtual void loadtoken(bool keepspace = false) {
		unsigned char s, c;
		Initable();
		char ch[] = { 0, 0 };
		string token;
		stack.clear();
		stackln.clear();
		stacktype.clear();

		long locpos = Length();

		if (locpos != -1) {
			stack.reserve(locpos / 2);
			stackln.reserve(locpos / 2);
			stacktype.reserve(locpos / 2);
		}
		char decimal = 0;
		bool digit = false;
		bool exposant = false;

		long line = 0;
		char lastc = 0;
		char consumed = 1;
		s = get();
		locpos = tell();
		bool concatdot = false;
		size_t sze;
		unsigned char cloc, ccloc;

		while (!eof()) {
			consumed = 1;
			if (s == '/' && token == "" && readstrings) {
				s = get();
				if (s == '/') {
					//this is a comment
					s = get();
					while (s != 10 && s != 13) {
						if (eof())
							return;
						s = get();
					}
					token.clear();
					line++;
					lastc = s;
					s = get();
					locpos = tell();
					continue;
				}

				if (s == '@') {
					//this is a long comment up to */
					lastc = 0;
					while (1) {
						if (eof())
							return;
						s = get();
						if (s == '@') {
							s = get();
							if (s == '/')
								break;
						}

						if (s == 10 && lastc == 13)
							lastc = 0;
						else
						if (s == 10 || s == 13) {
							lastc = s;
							line++;
						}
					}

					token.clear();
					s = get();
					locpos = tell();
					continue;
				}
				seek(locpos);
				s = '/';
			}

			c = s;
			if ((token == "" || token == "u") && readstrings) {
				char cd;
				if (c == '@') {
					s = get();
					if (s == '"') {
						cd = 5;
						if (token == "u") {
							token = "";
							cd = 8;
						}
						lastc = 0;
						token += s;
						c = get();
						while (!eof()) {
							if (c == 10 && lastc == 13)
								lastc = 0;
							else {
								if (c == 10 || c == 13) {
									lastc = c;
									line++;
								}
							}
							token += c;
							bool fd = false;
							if (c == '"')
								fd = true;
							c = get();
							if (fd && c == '@')
								break;
						}
						stack.push_back(token);
						stackln.push_back(line);
						stacktype.push_back(cd);
						token.clear();
						s = get();
						locpos = tell();
						continue;
					}
					seek(locpos);
				}
				else {
					if (c == '"') {
						cd = 1;
						if (token == "u") {
							token = "";
							cd = 6;
						}

						lastc = 0;
						token += c;
						c = get();
						while (c != '"' && !eof()) {
							if (c == 10 && lastc == 13)
								lastc = 0;
							else
							if (c == 10 || c == 13) {
								lastc = c;
								line++;
							}
							if (c == '\\') {
								token += c;
								c = get();
							}
							token += c;
							c = get();
						}
						token += '"';
						stack.push_back(token);
						stackln.push_back(line);
						stacktype.push_back(cd);
						token.clear();
						s = get();
						locpos = tell();
						continue;
					}
					if (c == '\'') {
						cd = 2;
						if (token == "u") {
							token = "";
							cd = 7;
						}

						token += c;
						c = get();
						while (c != '\'' && !eof()) {
							if (c == 10 && lastc == 13)
								lastc = 0;
							else
							if (c == 10 || c == 13) {
								lastc = c;
								line++;
							}
							token += c;
							c = get();
						}
						token += "'";
						stack.push_back(token);
						stackln.push_back(line);
						stacktype.push_back(cd);
						token.clear();
						s = get();
						locpos = tell();
						continue;
					}
				}
			}

			if (c != '.')
				concatdot = false;

			if (strchr((char*)table, c) && (!digit || (decimal && c == 'e'))) {
				token += c;
				if (c == 'e' && decimal)
					exposant = true;
				else {
					if (c == 160 || c == 168 || c == 167) {
						sze = token.size();
						if (sze >= 3) {
							cloc = token[sze - 2];
							ccloc = token[sze - 3];
							if (ccloc == 226) {
								if ((c == 160 && cloc == 137) || (cloc == 136 && c >= 167)) { //this is a ≠, ∨, ∧
									if (sze != 3) {
										token = token.substr(0, sze - 3);
										stack.push_back(token);
										stackln.push_back(line);
										stacktype.push_back(4);
									}

									token = ccloc;
									token += cloc;
									token += c;
									stack.push_back(token);
									stackln.push_back(line);
									stacktype.push_back(0);
									token = "";
								}
							}
						}
					}
					else {
						if (c == 183 || c == 151 || c == 178 || c == 179 || c == 172) {
							sze = token.size();
							if (sze >= 2) {
								cloc = token[sze - 2];
								if ((cloc == 195 && (c == 151 || c == 183)) || (cloc == 194 && (c == 172 || c == 178 || c == 179))) { // operators: "÷","×","²","³","¬"
									if (sze != 2) {
										token = token.substr(0, sze - 2);
										stack.push_back(token);
										stackln.push_back(line);
										stacktype.push_back(4);
									}

									token = cloc;
									token += c;
									stack.push_back(token);
									stackln.push_back(line);
									stacktype.push_back(0);
									token = "";
								}
							}
						}
					}
				}
			}
			else {
				bool normaltoken = true;
				if (digit) {
					switch (c) {
					case 48:
					case 49:
					case 50:
					case 51:
					case 52:
					case 53:
					case 54:
					case 55:
					case 56:
					case 57:
						token += c;
						normaltoken = false;
						if (decimal)
							decimal = 2;
						break;
					case '.':
						if (!decimal) {
							token += c;
							decimal = 1;
							normaltoken = false;
						}
						break;
					case '+':
					case '-':
						if (exposant) {
							token += c;
							normaltoken = false;
						}
					}
				}
				else {
					if (c >= 48 && c <= 57) {
						normaltoken = false;
						if (token == "")
							digit = true;
						else {
							if (token == "#") {
								stack.push_back(token);
								stackln.push_back(line);
								stacktype.push_back(0);
								token = "";
								digit = true;
							}
						}
						token += c;
					}
				}

				if (normaltoken) {
					if (token != "") {
						if (digit) {
							if (decimal == 1)
								token = token.substr(0, token.size() - 1);
							stack.push_back(token);
							stacktype.push_back(3);
							if (decimal == 1) {
								ch[0] = '.';
								concatdot = concatenatedots;
								stack.push_back(ch);
								stackln.push_back(line);
								stacktype.push_back(0);
							}
						}
						else {
							stack.push_back(token);
							stacktype.push_back(4);
						}
						stackln.push_back(line);
						token.clear();
						digit = false;
						exposant = false;
						decimal = false;
					}

					if (c > 32 || keepspace) {
						if (strchr((char*)table, c))
							token = c;
						else {
							ch[0] = c;
							if (c == '.' && concatdot && stack.size() && ch == stack.back() && line == stackln.back() && stacktype.back() == 0) {
								stack.back() += ch; // in order to detect ..
							}
							else {
								if (c == '.')
									concatdot = concatenatedots;

								stack.push_back(ch);
								stackln.push_back(line);
								stacktype.push_back(0);
							}
						}
					}

					if (c == 10 && lastc == 13)
						lastc = 0;
					else
					if (c == 10 || c == 13) {
						lastc = c;
						line++;
					}
				}
			}
			consumed = 0;
			s = get();
			locpos = tell();
		}

		if (!consumed && s > 32 && s < 128) {
			ch[0] = s;
			stack.push_back(ch);
			stackln.push_back(line);
			stacktype.push_back(0);
		}
		else {
			if (token != "") {
				stack.push_back(token);
				if (digit)
					stacktype.push_back(3);
				else
					stacktype.push_back(4);
				stackln.push_back(line);
			}
		}
	}
};

class x_readstring : public x_reading {
public:
	string parcours;
	long length;


	x_readstring(string s) {
		concatenatedots = true;
		readstrings = true;
		name = "%%STRING";
		pos = 0;
		parcours = s;
		length = (long)s.size();
	}

	unsigned char peek() {
		if (pos < length)
			return parcours[pos + 1];
		else
			return 0;
	}

	long Length() {
		return length;
	}

	char eof() {
		if (pos > length)
			return 1;
		return 0;
	}

	char get() {
		if (pos > length)
			return -1;
		if (pos == length) {
			pos++;
			return -1;
		}
		return parcours[pos++];
	}

	string read(int sz) {
		long borne = sz;
		if (borne >= length)
			borne = length;
		if (eof())
			return "";
		currentbuffer = parcours.substr(pos, borne);
		pos += borne;
		return currentbuffer;
	}

	void reset(long p) {
		pos = p;
	}
};

class x_readfile : public x_reading {
public:
	ifstream* fichier;
	char* buf;
	int taille;
	long size;
	bool localfile;

	x_readfile(string& n, long p = 0) {
		concatenatedots = true;
		readstrings = true;
		localfile = true;
		buf = new char[258];
		taille = 256;
		name = n;
		size = -1;
		fichier = new ifstream(n.c_str(), ios::in | ios::binary);
		if (fichier->fail())
			pos = -1;
		else {
			Size();
			pos = p;
			if (pos)
				fichier->seekg(pos);
		}
	}

	long Size() {
		FILE* flocal;
		struct stat scible;
		int stcible = -1;
#ifdef WIN32
		fopen_s(&flocal, (char*)(name.c_str()), "rb");
#else
		flocal = fopen((char*)(name.c_str()), "rb");
#endif

		if (flocal != NULL) {

#if (_MSC_VER >= 1900)
			stcible = fstat(_fileno(flocal), &scible);
#else
#if  defined(WIN32) | defined(APPLE)
			stcible = fstat(flocal->_file, &scible);
#else
			stcible = fstat(flocal->_fileno, &scible);
#endif
#endif

			if (stcible >= 0)
				size = scible.st_size;
			fclose(flocal);
		}
		return size;
	}

	long Length() {
		if (size == -1)
			return Size();
		return size;
	}

	x_readfile(ifstream* fic, char* n, long p) {
		concatenatedots = true;
		readstrings = true;
		buf = new char[258];
		localfile = false;
		taille = 256;
		name = n;
		fichier = fic;
		pos = p;
		size = -1;
	}

	~x_readfile() {
		if (localfile) {
			if (fichier != NULL)
				delete fichier;
		}
		delete[] buf;
	}

	char error() {
		if (fichier == NULL || pos == -1)
			return 1;
		return 0;
	}

	void seek(long p) {
		if (fichier->eof())
			size = pos;
		pos = p;
		fichier->seekg(p);
		fichier->clear();
	}

	unsigned char peek() {
		return (unsigned char)fichier->peek();
	}

	char eof() {
		if (fichier == NULL || (size != -1 && pos >= size))
			return 1;
		return fichier->eof();
	}

	char get() {
		if (fichier->eof())
			return -1;
		pos++;
		return fichier->get();
	}

	string read(int sz) {
		if (taille < sz) {
			delete buf;
			buf = new char[sz + 2];
			taille = sz;
		}
		buf[sz] = 0;
		if (fichier->eof())
			return "";
		fichier->read(buf, sz);
		std::streamsize lz = fichier->gcount();
		buf[lz] = 0;
		pos += sz;
		currentbuffer = buf;
		return buf;
	}

	void close() {
		fichier->close();
	}

	void reset(long p) {
		if (fichier == NULL)
			return;
		pos = p;
		fichier->clear();
		fichier->seekg(pos);
		size = -1;
	}
};




#endif






