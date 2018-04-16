
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
filename   : atanorufile.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorversion.h"
#include "atanorufile.h"
#include "atanoruvector.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, ufileMethod>  Atanorufile::methods;
Exporting hmap<string, string> Atanorufile::infomethods;
Exporting bin_hash<unsigned long> Atanorufile::exported;

Exporting short Atanorufile::idtype = 0;

//------------------------------------------------------------------------------------------------------------------

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorufile::AddMethod(AtanorGlobal* global, string name, ufileMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorufile::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorufile::idtype = global->Getid("ufile");

	Atanorufile::AddMethod(global, "_initial", &Atanorufile::MethodInitial, P_NONE | P_ONE | P_TWO, "_initial(name,op): load a file in read/write/append mode. Default is read.");
	Atanorufile::AddMethod(global, "close", &Atanorufile::MethodClose, P_NONE, "close(): close a file.");
	Atanorufile::AddMethod(global, "openread", &Atanorufile::MethodOpenRead, P_ONE, "openread(string filename): open a file in reading mode.");
	Atanorufile::AddMethod(global, "ropen", &Atanorufile::MethodOpenRead, P_ONE, "ropen(string filename): open a file in reading mode.");
	Atanorufile::AddMethod(global, "openwrite", &Atanorufile::MethodOpenWrite, P_ONE, "openwrite(string filename): open a file in writing mode.");
	Atanorufile::AddMethod(global, "wopen", &Atanorufile::MethodOpenWrite, P_ONE, "wopen(string filename): open a file in writing mode.");
	Atanorufile::AddMethod(global, "openappend", &Atanorufile::MethodOpenAppend, P_ONE, "openappend(string filename): open a file in append mode.");
	Atanorufile::AddMethod(global, "aopen", &Atanorufile::MethodOpenAppend, P_ONE, "aopen(string filename): open a file in append mode.");
	Atanorufile::AddMethod(global, "get", &Atanorufile::MethodGet, P_NONE, "get(): get a character.");
	Atanorufile::AddMethod(global, "tell", &Atanorufile::MethodTell, P_NONE, "tell(): return position in the file.");
	Atanorufile::AddMethod(global, "seek", &Atanorufile::MethodSeek, P_ONE, "seek(int p): position the curson at p in the file.");
	Atanorufile::AddMethod(global, "unget", &Atanorufile::MethodUnget, P_NONE, "unget(): return last read character in the file.");
	Atanorufile::AddMethod(global, "write", &Atanorufile::MethodWrite, P_ONE, "write(string c): write string s in the file.");
	Atanorufile::AddMethod(global, "writeln", &Atanorufile::MethodWriteln, P_ONE, "writeln(string c): write string s in the file, with a final carriage return.");
	Atanorufile::AddMethod(global, "read", &Atanorufile::MethodRead, P_NONE | P_ONE, "read(int nb): read nb characters from the file or all if nb is not supplied.");
	Atanorufile::AddMethod(global, "readline", &Atanorufile::MethodReadoneline, P_NONE, "readline(): read a line from the document.");
	Atanorufile::AddMethod(global, "eof", &Atanorufile::MethodEof, P_NONE, "eof(): return true, if the end has been reached.");
	Atanorufile::AddMethod(global, "signature", &Atanorufile::MethodSignature, P_NONE, "signature(bool utf8): Set the UTF8 signature mode for files.");

	global->newInstance[Atanorufile::idtype] = new Atanorufile(global);
	global->RecordMethods(Atanorufile::idtype,Atanorufile::exported);
	
	short idother;
	idother = global->Getid("wfile");
	global->newInstance[idother] = new Atanorufile(global);
	global->RecordMethods(idother, Atanorufile::exported);

	return true;
}



Atanor* Atanorufile::Put(Atanor* idx, Atanor* kval, short idthread) {
	return aTRUE;
}

Atanor* Atanorufile::Get(Atanor* context, Atanor* idx, short idthread) {
	Locking _lock(this);
	if (thefile == NULL || thefile->eof() || op != "r")
		return aFALSE;

	if (context->isNumber()) {
		wchar_t c = thefile->get();
		context->storevalue(c);
		return aTRUE;
	}

	wstring line;
	getline(*thefile, line);
	line += L"\n";
	context->storevalue(line);

	return aTRUE;

}

//A sample of how to implement a method for your new class...
Atanor* Atanorufile::MethodRead(Atanor* context, short idthread, AtanorCall* callfunc) {
	Locking _lock(this);
	if (thefile == NULL || thefile->eof() || op != "r")
		return globalAtanor->Returnerror("Wrong access to the file", idthread);

	wstring bf;
	long nb = -1;
	if (callfunc->Size() == 1) {
		nb = callfunc->Evaluate(0, aNULL, idthread)->Integer();
		if (nb != -1) {
			if (context->isVectorContainer()) {
				Atanor* vect = Selectauvector(context);
				readnb(((Atanoruvector*)vect)->values, nb);
				return vect;
			}

			readnb(bf, nb);
			return globalAtanor->Provideustring(bf);
		}
	}

	if (context->isContainer()) {
		Atanor* vect = Selectauvector(context);
		readall(((Atanoruvector*)vect)->values);
		return vect;
	}

	if (context->isNumber())
		return globalAtanor->Provideint(Get());


	readall(bf);
	return globalAtanor->Provideustring(bf);

}



