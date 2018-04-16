
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
filename   : atanoratanor.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanoratanor.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, atanorMethod>  Atanoratanor::methods;
Exporting hmap<string, string> Atanoratanor::infomethods;
Exporting bin_hash<unsigned long> Atanoratanor::exported;

Exporting hmap<string, Atanoratanor*> Atanoratanor::recorded;

Atanor* AtanorRecordedFiles(string filename) {
	filename = NormalizeFileName(filename);
	if (Atanoratanor::recorded.find(filename) != Atanoratanor::recorded.end())
		return Atanoratanor::recorded[filename];

	return NULL;
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanoratanor::AddMethod(AtanorGlobal* global, string name, atanorMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanoratanor::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();
	recorded.clear();


	Atanoratanor::AddMethod(global, "method", &Atanoratanor::MethodMethod, P_FULL, "method(...): Execute a method within the local space.");



	global->newInstance[a_atanor] = new Atanoratanor(global);
	global->RecordMethods(a_atanor, Atanoratanor::exported);

	return true;
}


bool Atanoratanor::InitialisationModule(short name, AtanorGlobal* global) {
	idtype = name;

	if (acode == NULL)
		return false;

	bin_hash<Atanor*>::iterator it;
	AtanorFrame* frame = (AtanorFrame*)acode->Mainframe();
	for (it = frame->declarations.begin(); it != frame->declarations.end(); it++) {
		if (it->second->isFunction())
			declared[it->first] = globalAtanor->arities[it->second->Name()];
	}

	global->newInstance[idtype] = new Atanoratanor(global);
	global->RecordMethods(idtype, declared);

	return true;
}


Atanor* Atanoratanor::Put(Atanor* index, Atanor* value, short idthread) {
	filename = value->String();	
	return Load();
}

Atanor* Atanoratanor::Get(Atanor* context, Atanor* index, short idthread) {
	return this;
}


Atanor* Atanoratanor::Load() {
	filename = NormalizeFileName(filename);
	recorded[filename] = this;
	acode = globalAtanor->Loadfile(filename);
	AtanorFrame* frame = (AtanorFrame*)acode->Mainframe();
	Atanor* res;
	for (size_t i = 0; i < frame->instructions.size(); i++) {
		if (frame->instructions[i]->isVariable()) {
			res = frame->instructions[i]->Get(frame, aNULL, 0);
			if (res->isError())
				return res;
		}
	}
	return this;
}




