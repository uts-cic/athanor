
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
filename   : atanorfile.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorfile.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, fileMethod>  Atanorfile::methods;
Exporting hmap<string, string> Atanorfile::infomethods;
Exporting bin_hash<unsigned long> Atanorfile::exported;

Exporting short Atanorfile::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorfile::AddMethod(AtanorGlobal* global, string name, fileMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorfile::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorfile::idtype = global->Getid("file");

	Atanorfile::AddMethod(global, "_initial", &Atanorfile::MethodInitial, P_NONE | P_ONE | P_TWO, "_initial(name,op): load a file in read/write/append mode. Default is read.");
	Atanorfile::AddMethod(global, "close", &Atanorfile::MethodClose, P_NONE, "close(): close a file.");
	Atanorfile::AddMethod(global, "openread", &Atanorfile::MethodOpenRead, P_ONE, "openread(string filename): open a file in reading mode.");
	Atanorfile::AddMethod(global, "ropen", &Atanorfile::MethodOpenRead, P_ONE, "ropen(string filename): open a file in reading mode.");
	Atanorfile::AddMethod(global, "openwrite", &Atanorfile::MethodOpenWrite, P_ONE, "openwrite(string filename): open a file in writing mode.");
	Atanorfile::AddMethod(global, "wopen", &Atanorfile::MethodOpenWrite, P_ONE, "wopen(string filename): open a file in writing mode.");
	Atanorfile::AddMethod(global, "openappend", &Atanorfile::MethodOpenAppend, P_ONE, "openappend(string filename): open a file in append mode.");
	Atanorfile::AddMethod(global, "aopen", &Atanorfile::MethodOpenAppend, P_ONE, "aopen(string filename): open a file in append mode.");
	Atanorfile::AddMethod(global, "get", &Atanorfile::MethodGet, P_NONE, "get(): get a character.");
	Atanorfile::AddMethod(global, "tell", &Atanorfile::MethodTell, P_NONE, "tell(): return position in the file.");
	Atanorfile::AddMethod(global, "seek", &Atanorfile::MethodSeek, P_ONE, "seek(int p): position the curson at p in the file.");
	Atanorfile::AddMethod(global, "unget", &Atanorfile::MethodUnget, P_ONE, "unget(int c): return character c in the file.");
	Atanorfile::AddMethod(global, "write", &Atanorfile::MethodWrite, P_ONE, "write(string c): write string s in the file.");
	Atanorfile::AddMethod(global, "writeln", &Atanorfile::MethodWriteln, P_ONE, "writeln(string c): write string s in the file, with a final carriage return.");
	Atanorfile::AddMethod(global, "writebin", &Atanorfile::MethodWritebin, P_ONE, "writebin(byte c): write one byte to the file.");
	Atanorfile::AddMethod(global, "read", &Atanorfile::MethodRead, P_NONE | P_ONE, "read(int nb): read nb characters from the file or all if nb is not supplied.");
	Atanorfile::AddMethod(global, "readline", &Atanorfile::MethodReadoneline, P_NONE, "readline(): read a line from the document.");
	Atanorfile::AddMethod(global, "eof", &Atanorfile::MethodEof, P_NONE, "eof(): return true, if the end has been reached.");
	Atanorfile::AddMethod(global, "signature", &Atanorfile::MethodSignature, P_NONE, "signature(bool utf8): Set the UTF8 signature mode for files.");

	global->newInstance[Atanorfile::idtype] = new Atanorfile("", global);
	global->RecordMethods(Atanorfile::idtype,Atanorfile::exported);

	return true;
}





Atanor* Atanorfile::MethodRead(Atanor* context, short idthread, AtanorCall* callfunc) {
	Locking _lock(this);
	if (thefile == NULL || feof(thefile) || op != "rb")
		return globalAtanor->Returnerror("Wrong access to the file", idthread);

	string bf;
	long nb = -1;
	long total;
	if (callfunc->Size() == 1) {
		nb = callfunc->Evaluate(0, aNULL, idthread)->Integer();
		if (nb != -1) {
			if (context->isVectorContainer()) {
				Atanor* vect = Selectavector(context);

				if (vect->Type() == a_vector) {
					vector<string> v;
					readnb(nb, v);
					for (size_t i = 0; i < v.size(); i++)
						vect->storevalue(v[i]);
					return vect;
				}

				switch (vect->Type()) {
				case a_svector:
					readnb(nb, ((Atanorsvector*)vect)->values);
					break;
				case a_uvector:
					readnb(nb, ((Atanoruvector*)vect)->values);
					break;
				default:
					total = readnb(nb);
					for (nb = 0; nb < fchars.size(); nb++)
						vect->storevalue(fchars[nb]);
				}
				return vect;
			}

			bf = read(nb);
			return globalAtanor->Providestring(bf);
		}
	}

	if (context->isNumber())
		return globalAtanor->Provideint(get());

	if (context->isContainer()) {
		Atanor* vect = Selectavector(context);
		
		if (vect->Type() == a_vector) {
			vector<string> v;
			readall(v);
			for (size_t i = 0; i < v.size(); i++)
				vect->storevalue(v[i]);
			return vect;
		}

		switch (vect->Type()) {
		case a_svector:
			readall(((Atanorsvector*)vect)->values);
			break;
		case a_uvector:
			readall(((Atanoruvector*)vect)->values);
			break;
		default:
			total = readnb(-1);
			for (nb = 0; nb < fchars.size(); nb++)
				vect->storevalue(fchars[nb]);
		}
		return vect;

	}

	bf = read(-1);
	return globalAtanor->Providestring(bf);
}



