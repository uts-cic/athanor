
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
filename   : globalatanor.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef globalatanor_h
#define globalatanor_h

#ifdef WIN32
const long DEFAUTTHREADMAX = 1000;
#else
const long DEFAUTTHREADMAX = 500;
#endif

//------------------------------------------------------------------------
// We use these methods to create different Jatanor objects...
Exporting int AtanorCreateGlobal(long nbthreads = DEFAUTTHREADMAX);
Exporting bool AtanorDeleteGlobal(int idx);
Exporting bool AtanorSelectglobal(int idx);
Exporting bool AtanorReleaseglobal(int idx);
Exporting void AtanorCleanAllGlobals();
Exporting AtanorGlobal* GlobalAtanor(int idx);
//------------------------------------------------------------------------
//These methods are used to create an atanor engine...

Exporting AtanorGlobal* AtanorCreate(long nbthreads = DEFAUTTHREADMAX);
Exporting bool AtanorIgnition(long nbthreads = DEFAUTTHREADMAX); //declared in libsrecording.cxx
Exporting bool AtanorExtinguish();
Exporting short AtanorLoad(string filename);
Exporting short AtanorCompile(string& code, string filename, bool display = false);
Exporting size_t AtanorCompile(string& code, short idcode);
Exporting bool AtanorRun(short icode);
Exporting bool AtanorLoading(short icode);
Exporting short AtanorLastCodeSpace();
Exporting AtanorCode* AtanorCurrentSpace();
Exporting AtanorCode* AtanorCodeSpace(short idcode);
Exporting string AtanorCurrentFilename();
Exporting string AtanorErrorMessage();
Exporting void AtanorSetinitline(long i);
Exporting long AtanorCurrentLine();
Exporting bool AtanorRunning();
Exporting AtanorGlobal* GlobalAtanor();
Exporting short AtanorCurrentThreadId();
Exporting string AtanorIndentation(string& codestr,string blanc);
Exporting bool AtanorStop();
Exporting void AtanorDisplayFunction(DisplayFunctionCall call, void* o);
Exporting void AtanorSetArguments(string args);
Exporting void AtanorSetArguments(vector<string>& args);
Exporting void AtanorAllObjects(vector<string>& vs);
Exporting Atanor* AtanorExecute(AtanorCode* code, string name, vector<Atanor*>& params);
Exporting Atanor* AtanorExecute(AtanorCode* code, string name, vector<Atanor*>& params, short idthread);
Exporting void AtanorSetos();
Exporting string AtanorDisplayValue();
Exporting string AtanorListing();
#endif





