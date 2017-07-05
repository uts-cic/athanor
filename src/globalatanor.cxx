
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
filename   : globalatanor.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/


//This code is only used in the context of a GUI...
#include "atanor.h"
#include "compilecode.h"

static string _fullcode;
//----------------------------------------------------------------------------------
Exporting AtanorGlobal* AtanorCreate(long nbthreads) {
	if (globalAtanor != NULL)
		return NULL;
	_fullcode = "";
	globalAtanor = new AtanorGlobal(nbthreads);
	globalAtanor->linereference = 1;		
	return globalAtanor;
}

Exporting bool AtanorExtinguish() {
	if (globalAtanor != NULL) {
		if (globalAtanor->isRunning())
			return false;
		delete globalAtanor;
		globalAtanor = NULL;		
	}
	return true;
}

Exporting void AtanorAllObjects(vector<string>& vs) {
	if (globalAtanor!=NULL)
		globalAtanor->AtanorAllObjects(vs);
}

Exporting short AtanorLoad(string filename) {
	if (globalAtanor == NULL)
		return -1;

	//The system variables...
	globalAtanor->SystemInitialization(filename);

	AtanorCode* a = globalAtanor->Loadfile(filename);
	if (a == NULL)
		return -1;
	return a->idcode;
}

Exporting short AtanorCompile(string& codeinit, string filename, bool dsp) {
	AtanorCode* a = globalAtanor->Getcode(0);
	if (a == NULL)
		a = globalAtanor->GetNewCodeSpace(filename);
		
	//The system variables...
	globalAtanor->SystemInitialization(filename);

	bool add = false;
	string code;
	if (dsp) {
		code = Trim(codeinit);
		if (code.find(";", 0) == string::npos &&  code.find("{", 0) == string::npos && code.find("}", 0) == string::npos) {
			code = "println";
			code += "(";
			code += codeinit;
			code += ");";
		}
		else {
			code = codeinit;
			add = true;
		}
	}
	else {
		code = codeinit;
		add = true;
	}

	if (!a->Compile(code))
		return -1;
	
	if (add) {
		_fullcode += code;
		_fullcode += Endl;
	}

	return a->idcode;
}


Exporting string AtanorListing() {
	return _fullcode;
}

Exporting size_t AtanorCompile(string& code, short idcode) {
	AtanorCode* a = globalAtanor->Getcode(idcode);
	if (a == NULL || !a->Compile(code))
		return -1;
	return a->firstinstruction;
}

Exporting bool AtanorLoading(short idcode) {
	AtanorCode* a = globalAtanor->Getcode(idcode);
	if (a == NULL)
		return false;
	globalAtanor->spaceid = idcode;

	Atanor* v = a->Loading();

	if (globalAtanor->GenuineError(0))
		return false;
	return true;
}

Exporting bool AtanorRun(short idcode) {
	AtanorCode* a = globalAtanor->Getcode(idcode);
	if (a == NULL)
		return false;
	globalAtanor->spaceid = idcode;

	Atanor* v = a->Run();

	if (globalAtanor->GenuineError(0))
		return false;
	return true;
}

Exporting short AtanorLastCodeSpace() {
	return (short)globalAtanor->spaceid;
}

Exporting AtanorCode* AtanorCurrentSpace() {
	if (globalAtanor->spaceid == -1)
		return NULL;
	return globalAtanor->spaces[globalAtanor->spaceid];
}

Exporting AtanorCode* AtanorCodeSpace(short idcode) {
	if (globalAtanor == NULL || idcode >= globalAtanor->spaces.size() || idcode < 0)
		return NULL;

	return globalAtanor->spaces[idcode];
}

Exporting string AtanorCurrentFilename() {
	if (globalAtanor == NULL)
		return "";

	AtanorCode* c = globalAtanor->Getcurrentcode();
	if (c == NULL)
		return "";
	return c->filename;
}

Exporting long AtanorCurrentLine() {
	if (globalAtanor == NULL)
		return 0;

	AtanorCode* c = globalAtanor->Getcurrentcode();
	if (c == NULL)
		return 0;
	return c->Getcurrentline();
}

Exporting void AtanorSetinitline(long i) {
    if (globalAtanor==NULL)
        return;
    globalAtanor->linereference = i;
}

static ostringstream* gl_os=NULL;
Exporting void AtanorSetos() {
    if (globalAtanor==NULL)
        return;
    if (gl_os==NULL)
        delete gl_os;
    gl_os=new std::ostringstream;
    globalAtanor->os = gl_os;
}

Exporting string AtanorDisplayValue() {
    if (globalAtanor==NULL || gl_os==NULL)
        return "";
    return gl_os->str();
}

Exporting string AtanorErrorMessage() {
	if (globalAtanor->threads[0].errorraised != NULL) {
		string filename = globalAtanor->Getcurrentfile();
		stringstream message;		
		message << globalAtanor->threads[0].errorraised->String() << " at " << globalAtanor->Getinstructionline(0);
		string msg = message.str();
		if (msg.find(filename) == string::npos)
			message << " in " << filename;

		return message.str();
	}
	return globalAtanor->threads[0].message.str();
}

Exporting AtanorGlobal* GlobalAtanor() {
	return globalAtanor;
}

Exporting bool AtanorRunning() {
	if (globalAtanor == NULL)
		return false;

	return globalAtanor->isRunning();
}

Exporting short AtanorCurrentThreadId() {
	return globalAtanor->GetThreadid();
}

Exporting string AtanorIndentation(string& codestr,string blanc) {	
	vector <int> iblancs;
	vector<string> vargs;
	cr_normalise(codestr);
	v_split_indent(codestr, vargs);
	string codeindente = "";
	IndentationCode(codeindente, vargs, iblancs, (int)blanc.size(), true, "else", "elif", "if");
	Trimright(codeindente);
	codeindente += "\n";
	if (codeindente.find("/@") != string::npos || codeindente.find("@\"") != string::npos)
		cr_normalise(codeindente);
	return codeindente.c_str();
}


Exporting bool AtanorStop() {
	if (globalAtanor == NULL)
		return true;
	globalAtanor->executionbreak = true;
	globalAtanor->Releasevariables();
	return true;
}

Exporting void AtanorDisplayFunction(DisplayFunctionCall call, void* o) {
	if (globalAtanor == NULL)
		return;
	globalAtanor->displayfunction = call;
	globalAtanor->displayobject = o;
}


Exporting void AtanorSetArguments(string args) {
	if (globalAtanor == NULL)
		return;
	globalAtanor->Setarguments(args);
}


Exporting void AtanorSetArguments(vector<string>& args) {
	if (globalAtanor == NULL)
		return;
	globalAtanor->arguments = args;
}



