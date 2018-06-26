
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
filename   : atanormp3.cxx
Date       : 2017/04/01
Purpose    :
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "instructions.h"
#include "atanormp3.h"

//We need to declare once again our local definitions.
hmap<short, mp3Method>  Atanormp3::methods;
hmap<string, string> Atanormp3::infomethods;
bin_hash<unsigned long> Atanormp3::exported;

short Atanormp3::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanormp3::AddMethod(AtanorGlobal* global, string name, mp3Method func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanormp3::InitialisationModule(AtanorGlobal* global, string version) {
	Atanormp3::idtype = global->Getid("mp3");

	Atanormp3::AddMethod(global, "_initial", &Atanormp3::MethodLoad, P_ONE, "_initial(string pathname): Load the sound pathname.");
	Atanormp3::AddMethod(global, "load", &Atanormp3::MethodLoad, P_ONE, "load(string pathname): Load the sound pathname.");
	Atanormp3::AddMethod(global, "play", &Atanormp3::MethodPlay, P_NONE, "play(): play the sound");
	Atanormp3::AddMethod(global, "stop", &Atanormp3::MethodStop, P_NONE, "stop(): stop the sound");

	global->newInstance[Atanormp3::idtype] = new Atanormp3(global);
	global->RecordMethods(Atanormp3::idtype, Atanormp3::exported);

	return true;
}

#ifdef MACSOUND
void completionCallback(SystemSoundID  mySSID, void* data) {
	Atanormp3* ksnd = (Atanormp3*)data;
	AtanorFunction* func = (AtanorFunction*)ksnd->function;
	AtanorCallFunction kfunc(func);

	ksnd->Setreference();
	kfunc.arguments.push_back(ksnd);
	Atanor* ret = kfunc.Get(aNULL, aNULL, globalAtanor->GetThreadid());
	ksnd->Resetreference();
	ret->Release();
}
#endif

Atanor* Atanormp3::Put(Atanor* index, Atanor* value, short idthread) {
	return this;
}

Atanor* Atanormp3::Get(Atanor* context, Atanor* index, short idthread) {
	return this;
}


Atanor* Atanormp3::MethodLoad(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
#ifdef MACSOUND
	if (created) {
		if (function != NULL)
			removeSoundFunction(&sound);
		cleanSoundId(&sound);
	}
#endif
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	path = callfunc->Evaluate(0, contextualpattern, idthread)->String();
#ifdef MACSOUND
	if (createSoundId(path.c_str(), &sound) == false)
		return globalAtanor->Returnerror("SND(001): Unknown file", idthread);
	if (function != NULL)
		addSoundFunction(&sound, completionCallback, (void*)this);
#endif
#ifdef WIN32
	wstring base;
	s_utf8_to_unicode(base, (char*)path.c_str());
	if (sound.Load((WCHAR*)base.c_str()) == false)
		return globalAtanor->Returnerror("SND(005): Error during loading", idthread);
#endif
	loaded = true;
	created = true;
	return aTRUE;
}

Atanor* Atanormp3::MethodPlay(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (!loaded)
		return globalAtanor->Returnerror("SND(003): Please load a file first", idthread);
#ifdef MACSOUND
	playSoundId(&sound);
#endif
#ifdef WIN32
	sound.Play();
#endif
	return aTRUE;
}

Atanor* Atanormp3::MethodStop(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (!loaded)
		return globalAtanor->Returnerror("SND(003): Please load a file first", idthread);
#ifdef MACSOUND
	if (created) {
		if (function != NULL)
			removeSoundFunction(&sound);
		cleanSoundId(&sound);
		if (createSoundId(path.c_str(), &sound) == false)
			return globalAtanor->Returnerror("SND(001): Unknown file", idthread);
		if (function != NULL)
			addSoundFunction(&sound, completionCallback, (void*)this);
	}
#endif
#ifdef WIN32
	sound.Stop();
#endif
	return aTRUE;
}




