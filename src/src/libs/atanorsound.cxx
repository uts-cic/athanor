
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
filename   : atanorsound.cxx
Date       : 2017/04/01
Purpose    : sound players based on the libraries mpg123 and ao
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorsound.h"
#include "atoms.h"
#include "atanorivector.h"
#include "atanormapsi.h"

//We need to declare once again our local definitions.
hmap<short, soundMethod>  Atanorsound::methods;
hmap<string, string> Atanorsound::infomethods;
bin_hash<unsigned long> Atanorsound::exported;

short Atanorsound::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorsound::AddMethod(AtanorGlobal* global, string name, soundMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorsound::InitialisationModule(AtanorGlobal* global, string version) {
	Atanorsound::idtype = global->Getid("sound");

	Atanorsound::AddMethod(global, "_initial", &Atanorsound::MethodLoad, P_ONE, "_initial(string pathname): Load a sound file");
	Atanorsound::AddMethod(global, "load", &Atanorsound::MethodLoad, P_ONE, "load(string pathname): Load a sound file");
	Atanorsound::AddMethod(global, "play", &Atanorsound::MethodPlay, P_NONE | P_ONE, "play(bool rst): play the sound. If rst is true start back to beginning");
	Atanorsound::AddMethod(global, "stop", &Atanorsound::MethodStop, P_NONE, "stop(): stop the sound");
	Atanorsound::AddMethod(global, "parameters", &Atanorsound::MethodInfos, P_NONE | P_ONE, "parameters(): Return the sound configuration values as a simap. Parameters rate and channels can be modified");
	Atanorsound::AddMethod(global, "clear", &Atanorsound::MethodReset, P_NONE, "clear(): Reset the audio channel");
	Atanorsound::AddMethod(global, "open", &Atanorsound::MethodOpen, P_ONE, "open(simap p): Open a sound channel with parameters p (server side)");
	Atanorsound::AddMethod(global, "close", &Atanorsound::MethodClose, P_NONE, "close(): close the sound channel (server side)");
	Atanorsound::AddMethod(global, "decode", &Atanorsound::MethodDecode, P_ONE, "decode(ivector): decode a sound channel fill in an ivector (client side)");
	Atanorsound::AddMethod(global, "encode", &Atanorsound::MethodEncode, P_ONE, "encode(ivector): play a sound channel from a decoding sessions (server side)");



	global->newInstance[Atanorsound::idtype] = new Atanorsound(global);
	global->RecordMethods(Atanorsound::idtype,Atanorsound::exported);

	return true;
}



Atanor* Atanorsound::Put(Atanor* index, Atanor* value, short idthread) {
	return this;
}

Atanor* Atanorsound::Get(Atanor* context, Atanor* index, short idthread) {
	return this;
}

Atanor* Atanorsound::loading(short idthread) {

	SF_INFO sfinfo;
	//For sampling sound files other than MP3		

#ifdef USEMPG123
	if (path.find(".mp3") != string::npos || path.find(".MP3") != string::npos) {
		int channels, encoding, err;
		long rate;

		//we test if it is a MP3 file...
		mh = mpg123_new(NULL, &err);
		mp3_buffer_size = mpg123_outblock(mh);
		mp3_buffer = (unsigned char*)malloc(mp3_buffer_size * sizeof(unsigned char));

		if (mpg123_open(mh, STR(path)) != MPG123_OK)
			return globalAtanor->Returnerror("SND(003): Loading error", idthread);

		if (mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK)
			return globalAtanor->Returnerror("SND(013): Unknown format", idthread);

		/* set the output format and open the output device */
		format.bits = mpg123_encsize(encoding) * BITS;
		format.rate = rate;
		format.channels = channels;
		format.byte_format = AO_FMT_NATIVE;
		format.matrix = 0;
		mp3 = true;

		device = ao_open_live(driver, &format, NULL);

		//In this case, we need to do it live from the buffer initialized with the mp3 decoder
		if (device == NULL)
			return globalAtanor->Returnerror("SND(004): Cannot open mp3 audio", idthread);

		return aTRUE;
	}
#endif
	//For non MP3 files (wav or others)
	//Sampling with libsndfile
	if (!(soundfile = sf_open(STR(path), SFM_READ, &sfinfo))) {
		sf_perror(NULL);
		return globalAtanor->Returnerror("SND(003): Loading error", idthread);
	}

	//Basically, we have now all the right information about this file
	switch (sfinfo.format & SF_FORMAT_SUBMASK) {
	case SF_FORMAT_PCM_16:
		format.bits = 16;
		break;
	case SF_FORMAT_PCM_24:
		format.bits = 24;
		break;
	case SF_FORMAT_PCM_32:
		format.bits = 32;
		break;
	case SF_FORMAT_PCM_S8:
		format.bits = 8;
		break;
	case SF_FORMAT_PCM_U8:
		format.bits = 8;
		break;
	default:
		format.bits = 16;
		break;
	}

	format.channels = sfinfo.channels;
	format.rate = sfinfo.samplerate;
	format.byte_format = AO_FMT_NATIVE;
	format.matrix = 0;

	buffer = (short*)calloc(BUFFER_SIZE, sizeof(short));
	device = ao_open_live(driver, &format, NULL);
	//In this case, we need to do it live from the buffer initialized with the mp3 decoder
	if (device == NULL)
		return globalAtanor->Returnerror("SND(004): Cannot open audio", idthread);

	return aTRUE;
}

Atanor* Atanorsound::MethodLoad(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lock(this);
	if (loaded)
		return globalAtanor->Returnerror("SND(001): Already loaded", idthread);
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	path = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	Atanor* res = loading(idthread);
	if (res != aTRUE)
		return res;
	loaded = true;
	return aTRUE;
}

Atanor* Atanorsound::MethodPlay(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lock(this);
	bool rset = false;
	if (callfunc->Size() == 1) {
		Atanor* k = callfunc->Evaluate(0, contextualpattern, idthread);
		if (k->Type() == a_ivector)
			return MethodEncode(contextualpattern, idthread, callfunc);
		rset = k->Boolean();
	}
	if (!loaded)
		return globalAtanor->Returnerror("SND(002): Load your file first", idthread);
	stop = false;

#ifdef USEMPG123
	if (mp3) {
		//In case, the user has requested the stream to be reset to 0
		if (rset)
			mpg123_seek(mh, 0, SEEK_SET);
		size_t done = 1;
		/* decode and play */
		while (!stop && mpg123_read(mh, mp3_buffer, mp3_buffer_size, &done) == MPG123_OK && !globalAtanor->Error(idthread) && done != 0)
			ao_play(device, (char*)mp3_buffer, done);
		return aTRUE;
	}
#endif

	bool error = false;
	int read;
	if (rset)
		sf_seek(soundfile, 0, SEEK_SET); //We reset the stream to the beginning
	read = sf_read_short(soundfile, buffer, BUFFER_SIZE);
	while (read != 0 && !stop && !globalAtanor->Error(idthread)) {
		if (ao_play(device, (char *)buffer, (uint_32)(read * sizeof(short))) == 0) {
			error = true;
			break;
		}
		read = sf_read_short(soundfile, buffer, BUFFER_SIZE);
	}
	if (error) {
		//We reset the device
		ao_close(device);
		device = ao_open_live(driver, &format, NULL);
		return globalAtanor->Returnerror("SND(005): Erreur while playing sound", idthread);
	}
	return aTRUE;
}

Atanor* Atanorsound::MethodStop(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//Of course to be able to stop a sound, you need to play your SOUND file IN A THREAD...
	stop = true;
	return aTRUE;
}

Atanor* Atanorsound::MethodInfos(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lock(this);
	if (!loaded)
		return globalAtanor->Returnerror("SND(002): Load your file first", idthread);
	string k;
	long v;
	if (callfunc->Size() == 0) {
		Atanormapsi* kimap = new Atanormapsi;
		k = "channels"; v = format.channels;
		kimap->values[k] = v;
		k = "rate"; v = format.rate;
		kimap->values[k] = v;
		k = "byte_format"; v = format.byte_format;
		kimap->values[k] = v;
		k = "mp3"; v = mp3;
		kimap->values[k] = v;
		k = "bits"; v = format.bits;
		kimap->values[k] = v;
		k = "size";
		if (mp3) {
#ifdef USEMPG123
			v = mp3_buffer_size;
#endif
		}
		else
			v = BUFFER_SIZE;
		kimap->values[k] = v;
		return kimap;
	}

	Atanor* kmap = callfunc->Evaluate(0, contextualpattern, idthread);
	if (!kmap->isMapContainer())
		return globalAtanor->Returnerror("SND(008): The argument shoud be a map", idthread);
	AtanorIteration* itrloc = kmap->Newiteration(false);
	for (itrloc->Begin(); itrloc->End() != aTRUE; itrloc->Next()) {
		k = itrloc->Keystring();
		v = itrloc->Valueinteger();
		if (k == "channels")
			format.channels = v;
		else
		if (k == "rate")
			format.rate = v;
	}
	itrloc->Release();
	//We reset the device
	ao_close(device);
	device = ao_open_live(driver, &format, NULL);
	return aTRUE;
}

Atanor* Atanorsound::MethodReset(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lock(this);

#ifdef USEMPG123	
	if (mp3) {
		if (mh != NULL)
			mpg123_seek(mh, 0, SEEK_SET);
		return aTRUE;
	}
#endif

	if (soundfile != NULL)
		sf_seek(soundfile, 0, SEEK_SET); //We reset the stream to the beginning
	return aTRUE;
}

Atanor* Atanorsound::MethodOpen(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lock(this);
	string k;
	long v;
	Atanor* kmap = callfunc->Evaluate(0, contextualpattern, idthread);
	if (!kmap->isMapContainer())
		return globalAtanor->Returnerror("SND(008): The argument shoud be a map", idthread);
	int sz = BUFFER_SIZE;
	AtanorIteration* itrloc = kmap->Newiteration(false);
	for (itrloc->Begin(); itrloc->End() != aTRUE; itrloc->Next()) {
		k = itrloc->Keystring();
		v = itrloc->Valueinteger();
		if (k == "channels")
			format.channels = v;
		else if (k == "rate")
			format.rate = v;
		else if (k == "byte_format")
			format.byte_format = v;
		else if (k == "bits")
			format.bits = v;
		else if (k == "mp3")
			mp3 = v;
		else if (k == "size")
			sz = v;
	}
	itrloc->Release();
	if (mp3) {
#ifdef USEMPG123
		mp3_buffer_size = sz;
		if (mp3_buffer != NULL)
			free(mp3_buffer);
		mp3_buffer = (unsigned char*)malloc(mp3_buffer_size * sizeof(unsigned char));
#endif
	}
	else {
		if (buffer == NULL)
			buffer = (short*)calloc(BUFFER_SIZE, sizeof(short));
	}
	format.matrix = 0;
	//We reset the device
	if (device != NULL)
		ao_close(device);
	//We open it...
	device = ao_open_live(driver, &format, NULL);
	return aTRUE;
}

Atanor* Atanorsound::MethodClose(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lock(this);
	if (device != NULL) {
		ao_close(device);
		device = NULL;
		return aTRUE;
	}
	return aFALSE;
}

Atanor* Atanorsound::MethodDecode(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lock(this);
	if (!loaded)
		return globalAtanor->Returnerror("SND(002): Load your file first", idthread);
	Atanor* k = callfunc->Evaluate(0, contextualpattern, idthread);
	if (k->Type() != a_ivector)
		return globalAtanor->Returnerror("SND(010): The argument should be an ivector", idthread);
	Atanorivector* kvect = (Atanorivector*)k;
	size_t read, i;

#ifdef USEMPG123
	if (mp3) {
		/* decode only*/
		if (mpg123_read(mh, mp3_buffer, mp3_buffer_size, &read) == MPG123_OK) {
			Locking _lockvect(kvect);
			kvect->values.clear();
			for (i = 0; i < read; i++)
				kvect->values.push_back(mp3_buffer[i]);
			if (!read)
				return aFALSE;
			return aTRUE;
		}
		return aFALSE;
	}
#endif

	read = sf_read_short(soundfile, buffer, BUFFER_SIZE);
	Locking _lockvect(kvect);
	kvect->values.clear();
	for (i = 0; i < read; i++)
		kvect->values.push_back(buffer[i]);
	if (!read)
		return aFALSE;
	return aTRUE;
}

Atanor* Atanorsound::MethodEncode(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lock(this);
	if (device == NULL)
		return globalAtanor->Returnerror("SND(011): Please open the audio first", idthread);
	Atanor* k = callfunc->Evaluate(0, contextualpattern, idthread);
	if (k->Type() != a_ivector)
		return globalAtanor->Returnerror("SND(010): The argument should be an ivector", idthread);
	Atanorivector* kvect = (Atanorivector*)k;
	stop = false;
	int read = kvect->Size();
	int i;

#ifdef USEMPG123
	if (mp3) {
		{
			Locking _lockvect(kvect);
			read = kvect->values.size();
			for (i = 0; i < read; i++)
				mp3_buffer[i] = (char)kvect->values[i];
		}
		if (!read)
			return aFALSE;
		if (ao_play(device, (char*)mp3_buffer, read) == 0)
			return globalAtanor->Returnerror("SND(005): Erreur while playing sound", idthread);
		return aTRUE;
	}
#endif

	{
		Locking _lockvect(kvect);
		read = kvect->values.size();
		for (i = 0; i < read; i++)
			buffer[i] = (short)kvect->values[i];
	}
	if (!read)
		return aFALSE;
	if (ao_play(device, (char *)buffer, (uint_32)(read * sizeof(short))) == 0)
		return globalAtanor->Returnerror("SND(005): Erreur while playing sound", idthread);
	return aTRUE;
}




