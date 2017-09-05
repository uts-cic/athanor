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
filename   : atanormain.cxx
Date       : 2017/04/01
Purpose    : Main of the Athanor executable
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorversion.h"
#include "globalatanor.h"

#include <stdio.h>


#ifdef DOSOUTPUT
static bool dosoutput = true;
static void Setdosoutput(bool d) { dosoutput = d; }
string conversiontodos(char* c) {
	if (dosoutput)
		return s_utf8_to_dos(c);
	return c;
}
#else
#define conversiontodos(X) X
#endif

#ifdef WITHFLTK
extern "C" {
	Exporting bool Fltk_window_editor(string filename);
}
#endif

static void displayhelp(string wh) {
	cout << conversiontodos(STR(wh)) << Endl;
#ifdef WITHFLTK
	cout << conversiontodos("-console: open the console editor") << endl;
#endif
	cout << conversiontodos("-a 'source code' with _args fully piped") << endl;
	cout << conversiontodos("-p 'source code' with _args piped one string at a time") << endl;
	cout << conversiontodos("-c 'source code' without piped data") << endl;
	cout << conversiontodos("Predeclared variables:") << endl;
	cout << "\t";
	cout << conversiontodos("_args: argument vector") << endl;
	cout << "\t";
	cout << conversiontodos("_paths: _paths[0] is the current directory") << endl;
	cout << "\t";
	cout << conversiontodos("i,j,k: int") << endl;
	cout << "\t";
	cout << conversiontodos("f,g,h: float") << endl;
	cout << "\t";
	cout << conversiontodos("s,t,u: string") << endl;
	cout << "\t";
	cout << conversiontodos("m: map") << endl;
	cout << "\t";
	cout << conversiontodos("v: vector") << endl;
	exit(-1);
}


string Normalizefilename(string path) {
	char localpath[4096];
#ifdef WIN32
	_fullpath(localpath, STR(path), 4096);
#else
	realpath(STR(path), localpath);
#endif
	return localpath;
}

int main(int argc, char *argv[]) {
	vector<string> arguments;
	string lnstr;

	string wh = "atanor: ";
	wh += AtanorVersion();
	wh += Endl;

	if (argc <= 1) {
		cerr << "64 bits" << endl;
		cerr << "REGEX available" << endl;
		cerr << conversiontodos("Curl library Embedded") << endl;
		cerr << conversiontodos("Fast Light ToolKit (FLTK) available") << endl;

		cerr << endl;
		displayhelp(wh);
	}

	char piped = -1;
	bool debugkif = false;
	//argv[1] is the file name
	string name = argv[1];
	string code = "";
	string filename;
	bool console = false;
	string args;
	long i;
	long begin = -1;
	for (i = 1; i < argc; i++) {
		args = argv[i];
		if (args[0] != '-') {
			filename = args;
			i++;
			while (i < argc) {
				arguments.push_back(argv[i]);
				i++;
			}
			break;
		}
		
		if (console) {
			cerr << "Unknown command:" << args << endl;
			exit(-1);
		}

		if (args == "-h") {
			displayhelp(wh);
			exit(-1);
		}

#ifdef DOSOUTPUT
		if (args == "-nodos") {
			Setdosoutput(false);
			continue;
		}
#endif

#ifdef WITHFLTK
		if (args == "-console") {
			console = true;
			continue;
		}
#endif			
		
		if (args == "-a")
			piped = 1;
		else
		if (args == "-p")
			piped = 2;
		else
		if (args == "-c")
			piped = 0;

		if (piped != -1) {
			if (i + 1 < argc) {
				if (argv[i + 1][0] != '-')
					code = argv[++i];
			}

			if (code == "") {
				cerr << conversiontodos("Error: missing code or unknown argument: ") << args << endl;
				exit(-1);
			}
			break;
		}

		cerr << "Unknown command:" << args << endl;
		exit(-1);
	}

	if (piped == 1) {
		while (!cin.eof()) {
			getline(cin, lnstr);
			if (lnstr.size())
				arguments.push_back(lnstr);
			lnstr = "";
		}
	}

	AtanorIgnition();
	AtanorSetArguments(arguments);

#ifdef WITHFLTK
	//If the console has been requested...
	if (console) {
		//We record the libraries...
		Fltk_window_editor(filename);
		exit(0);
	}
#endif

	short idcode;

	//executing a file
	if (code == "") {
		if (name[0] == '-') {
			cerr << conversiontodos("Missing filename...") << endl;
			exit(-1);
		}

		name = Normalizefilename(argv[1]);
		try {
			idcode = AtanorLoad(name);
		}
		catch (AtanorRaiseError* err) {
			cerr << err->message << " at line: " << err->left << endl;
			delete err;
			exit(-1);
		}

		if (!AtanorRun(idcode))
			cerr << AtanorErrorMessage() << endl;

		exit(0);
	}

	string predeclarations = "int i,j,k;\nfloat f,g,h;\nstring s,t,u;\nmap m;\nvector v;\n\n";
	code = predeclarations + code;

	try {
		idcode = AtanorCompile(code, "MAIN");
	}
	catch (AtanorRaiseError* err) {
		cerr << err->message << endl;
		delete err;
		exit(-1);
	}

	//just a bit of code...
	if (piped != 2) {
		if (!AtanorRun(idcode))
			cerr << AtanorErrorMessage() << endl;
		exit(0);
	}

	vector<string> thepiped;
	lnstr = "";
	while (!cin.eof()) {
		getline(cin, lnstr);
		if (lnstr.size())
			thepiped.push_back(lnstr);
		lnstr = "";
	}


	//we loop for each argument...
	for (int i = 0; i < thepiped.size(); i++) {
		arguments.push_back(thepiped[i]);
		if (i)
			AtanorIgnition(10);
		AtanorSetArguments(arguments);
		try {
			idcode = AtanorCompile(code, "MAIN");
		}
		catch (AtanorRaiseError* err) {
			cerr << err->message << endl;
			delete err;
			exit(-1);
		}

		if (!AtanorRun(idcode))
			cerr << AtanorErrorMessage() << endl;

		arguments.pop_back();
		AtanorExtinguish();
	}
}





