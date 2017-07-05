
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
filename   : atanorsys.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorsys.h"
#include "atoms.h"
#include "atanormap.h"
#include "atanordate.h"
#include "instructions.h"

#ifdef WIN32
#include "Windows.h"
#include <io.h>
#endif

#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "osdir.h"

#ifdef WIN32
#define DUP _dup
#define DUP2 _dup2
#define FILENO _fileno
#else
#include <unistd.h>
#define FILENO fileno
#define DUP dup
#define DUP2 dup2
#endif

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, sysMethod>  Atanorsys::methods;
Exporting hmap<string, string> Atanorsys::infomethods;
Exporting bin_hash<unsigned long> Atanorsys::exported;

Exporting short Atanorsys::idtype = 0;

#ifdef WIN32
char* Getenv(char* name);
#else
#define Getenv getenv
#endif


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorsys::AddMethod(AtanorGlobal* global, string name, sysMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorsys::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorsys::idtype = global->Getid("sys");

	Atanorsys::AddMethod(global, "command", &Atanorsys::MethodCommand, P_ONE | P_TWO, "command(string comstring outputfile): execute the command 'com'. outputfile is optional and can be used to redirect output to a file.");
	Atanorsys::AddMethod(global, "isdirectory", &Atanorsys::MethodisDirectory, P_ONE, "isdirectory(string path): return 'true' if path is a directory");
	Atanorsys::AddMethod(global, "createdirectory", &Atanorsys::MethodCreateDirectory, P_ONE, "createdirectory(string path): create the directory of path 'path'");
	Atanorsys::AddMethod(global, "mkdir", &Atanorsys::MethodCreateDirectory, P_ONE, "mkdir(string path): create the directory of path 'path'");
	Atanorsys::AddMethod(global, "listdirectory", &Atanorsys::MethodListDirectory, P_ONE, "listdirectory(string path): return the content of the directory 'path'");
	Atanorsys::AddMethod(global, "ls", &Atanorsys::MethodListDirectory, P_ONE, "ls(string path): return the content of the directory 'path'");
	Atanorsys::AddMethod(global, "realpath", &Atanorsys::MethodRealPath, P_ONE, "realpath(path relpath): return the actual path matching 'realpath'");
	Atanorsys::AddMethod(global, "fileinfo", &Atanorsys::MethodFileInfo, P_ONE, "fileinfo(string pathname): returns the info of the file 'pathname'");
	Atanorsys::AddMethod(global, "env", &Atanorsys::MethodEnv, P_ONE | P_TWO, "env(string var)|env(string varstring val): return or set the content of the environment variable 'var'");



	global->newInstance[Atanorsys::idtype] = new Atanorsys(global);
	global->RecordMethods(Atanorsys::idtype,Atanorsys::exported);

	Atanor* a = new AtanorSystemVariable(global, new AtanorConstsys(global), global->Createid("_sys"), Atanorsys::idtype);
	
	return true;
}

void SetEnvironmentVariable(string command, string val) {
#ifdef WIN32
	command += "=";
	command += val;
	_putenv(STR(command));
#else
	setenv(command.c_str(), val.c_str(), 1);
#endif
}

Atanor* Atanorsys::Put(Atanor* idx, Atanor* kval, short idthread) {
	if (idx != NULL && !idx->isConst()) {
		lastcommand = idx->String();
		string val = kval->String();
#ifdef WIN32
		lastcommand += "=";
		lastcommand += val;
		_putenv(STR(lastcommand));
#else
		setenv(lastcommand.c_str(), val.c_str(), 1);
#endif

	}
	return aTRUE;
}

Atanor* Atanorsys::Get(Atanor* context, Atanor* idx, short idthread) {
	if (idx != NULL && !idx->isConst()) {
		lastcommand = idx->String();
		char* rep = Getenv(STR(lastcommand));
		if (rep == NULL)
			return globalAtanor->Providestring("");
		return globalAtanor->Providestring(rep);
	}
	return this;
}


Atanor* Atanorsys::MethodCommand(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//0 is the first parameter and so on...
	Atanor* kcmd = callfunc->Evaluate(0, aNULL, idthread);
	string filename;
	if (callfunc->Size() == 2)
		filename = callfunc->Evaluate(1, aNULL, idthread)->String();
	lastcommand = kcmd->String();
	if (lastcommand != "") {
		if (filename != "") {
			int o = DUP(1);
			FILE* stream = fopen(STR(filename), "w");
			if (stream == NULL)
				return globalAtanor->Returnerror("SYS(001): Cannot open output file", idthread);
			DUP2(FILENO(stream), 1);
			system(lastcommand.c_str());
			fflush(stdout);
			fclose(stream);
			DUP2(o, 1);
			stream = fopen(STR(filename), "r");
			string content;
			char buffer[2049];
			size_t nb;
			while (!feof(stream)) {
				nb = fread(buffer, 1, 2048, stream);
				buffer[nb] = 0;
				content += buffer;
			}
			fclose(stream);
			return globalAtanor->Providestring(content);
		}
		else
			system(lastcommand.c_str());
	}
	//you may return any value of course...
	return aTRUE;
}

Atanor* Atanorsys::MethodisDirectory(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	string dirName_in = callfunc->Evaluate(0, aNULL, idthread)->String();
#ifdef WIN32
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return aFALSE;  //something is wrong with your path!
	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return aTRUE;   // this is a directory!
#else
	struct stat st;
	if (stat(STR(dirName_in), &st) == 0)
	if ((st.st_mode & S_IFMT) == S_IFDIR)
		return aTRUE;
#endif
	return aFALSE;    // this is not a directory!
}

Atanor* Atanorsys::MethodCreateDirectory(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* kpath = callfunc->Evaluate(0, aNULL, idthread);
	string path = kpath->String();
	if (DirectoryCreation(STR(path), 0) == false)
		return aFALSE;
	return aTRUE;
}

Atanor* Atanorsys::MethodListDirectory(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* kpath = callfunc->Evaluate(0, aNULL, idthread);
	string path = kpath->String();
	oslink::directory dir(path);
	if (contextualpattern->Type() == a_float || contextualpattern->Type() == a_int || contextualpattern->Type() == a_long) {
		long i = 0;
		while (dir) {
			i++;
			dir.next();
		}
		return globalAtanor->Provideint(i);
	}
	Atanor* kvect = Selectasvector(contextualpattern);
	while (dir) {
		if (kvect->Type() == a_svector)
			((Atanorsvector*)kvect)->values.push_back(dir.next());
		else
			kvect->Push(globalAtanor->Providestring(dir.next()));
	}
	return kvect;
}

Atanor* Atanorsys::MethodRealPath(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* kpath = callfunc->Evaluate(0, aNULL, idthread);
	char localpath[4096];
	string path = kpath->String();
#ifdef WIN32
	_fullpath(localpath, STR(path), 4096);
#else
	realpath(STR(path), localpath);
#endif
	return globalAtanor->Providestring(localpath);
}

Atanor* Atanorsys::MethodFileInfo(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* kpath = callfunc->Evaluate(0, aNULL, idthread);
	string filename = kpath->String();
#ifdef WIN32
	struct _stat scible;
	int stcible = _stat(STR(filename), &scible);
#else
	struct stat scible;
	int stcible = stat(STR(filename), &scible);
#endif
	if (stcible >= 0) {
		Atanor* size = globalAtanor->Provideint(scible.st_size);
		Atanor* change = new Atanordate(scible.st_mtime);
		Atanor* adate = new Atanordate(scible.st_atime);
		Atanor* cdate = new Atanordate(scible.st_ctime);
		kpath = globalAtanor->Providestring(filename);
		Atanormap* mp = (Atanormap*)Selectamap(contextualpattern);
		mp->Push("size", size);
		mp->Push("date", change);
		mp->Push("access", adate);
		mp->Push("change", cdate);
		mp->Push("pathname", kpath);
		if ((scible.st_mode & S_IFMT) == S_IFDIR)
			mp->Push("directory", aTRUE);
		else
			mp->Push("directory", aFALSE);
		return mp;
	}
	return aNULL;
}

Atanor* Atanorsys::MethodEnv(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//0 is the first parameter and so on...
	Atanor* kname = callfunc->Evaluate(0, aNULL, idthread);
	lastcommand = kname->String();
	if (callfunc->Size() == 1) {
		if (lastcommand != "") {
			char* rep = Getenv(STR(lastcommand));
			if (rep == NULL)
				return globalAtanor->Providestring("");
			return globalAtanor->Providestring(rep);
		}
		//you may not return any value of course...
		return aNULL;
	}
	else {
		Atanor* kvalue = callfunc->Evaluate(1, aNULL, idthread);
		if (lastcommand != "") {
			string val = kvalue->String();
#ifdef WIN32
			lastcommand += "=";
			lastcommand += val;
			_putenv(STR(lastcommand));
#else
			setenv(lastcommand.c_str(), val.c_str(), 1);
#endif 			
			return aTRUE;
		}
	}
	//you may return any value of course...
	return aFALSE;
}





