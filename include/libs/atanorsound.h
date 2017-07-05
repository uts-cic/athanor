
/*
* Xerox Research Centre Europe - Grenoble Laboratory (XRCE)
*
* Copyright (C) 2016 - 2017 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
* This file can only be used with the ATANOR library or the executable
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : ATHANOR
Version    : ATANOR 0.26
filename   : atanorsound.h
Date       : 2017/02/06
Purpose    : 
Programmer : Claude ROUX (claude.roux@xrce.xerox.com)
Reviewer   :
*/

#ifndef atanorsound_h
#define atanorsound_h


//YOU NEED TO DEFINE: SOUND on the compiling line

//We use libao, libmpg123 and libsndfile sur le raspberry
/*
sudo apt-get install libao-dev
sudo apt-get install libmpg123-dev
sudo apt-get install libsndfile1-dev
*/

//Documentations: 
/*
http://www.mega-nerd.com/libsndfile/
http://snippets.khromov.se/raspberry-pi-shairport-build-instructions/

For instance, to solve an error message that occurs on raspberry pi (but does not prevent the system from playing the music)

“Unknown PCM cards.pcm.front”

You can modify:

nano /usr/share/alsa/alsa.conf
Change the line “pcm.front cards.pcm.front” to “pcm.front cards.pcm.default”
*/

//We have used the following examples to implement this library
/*
http://hzqtc.github.io/2012/05/play-mp3-with-libmpg123-and-libao.html
http://www.music.columbia.edu/pipermail/music-dsp/2002-January/047060.html
https://gist.github.com/maxammann/52d6b65b42d8ce23512a
*/

#ifdef WIN32
#define ssize_t size_t
#endif

//To play files
#include <ao/ao.h>

#ifdef USEMPG123
//Used to decode MP3 files...
#include <mpg123.h>
#endif

//To sample sound file before hand and get the right information to ao_play
#include <sndfile.h>

#define BITS 8

static void intialization() {
	static bool init = false;
	if (!init) {
		ao_initialize();
#ifdef USEMPG123
		mpg123_init();
#endif
		init = true;
	}
}

const int BUFFER_SIZE = 8192;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorsound;
//This typedef defines a type "soundMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorsound::*soundMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanorsound : public AtanorObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static hmap<short, soundMethod> methods;
    static hmap<string, string> infomethods;
    static bin_hash<unsigned long> exported;

    static short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
#ifdef USEMPG123
	mpg123_handle *mh;
	unsigned char *mp3_buffer;
	size_t mp3_buffer_size;
#endif

	ao_sample_format format;

	SNDFILE* soundfile;
	short *buffer;

	int driver;

	//KiF variable
	string path;
	bool loaded;
	bool stop;
	bool mp3;

	ao_device *device;

    //---------------------------------------------------------------------------------------------------------------------
    Atanorsound(AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
        //Do not forget your variable initialisation
		intialization();
		loaded = false;
		stop = false;
		mp3 = false;
		soundfile = NULL;
		buffer = NULL;
		driver = ao_default_driver_id();
		device = NULL;
#ifdef USEMPG123
		mp3_buffer_size = 0;
		mh = NULL;
		mp3_buffer = NULL;
#endif
	}

	~Atanorsound() {
		if (device != NULL)
			ao_close(device);

		if (soundfile != NULL)
			sf_close(soundfile);

		if (buffer != NULL)
			free(buffer);

#ifdef USEMPG123
		if (mp3_buffer != NULL)
			free(mp3_buffer);

		if (mh != NULL) {
			mpg123_close(mh);
			mpg123_delete(mh);
		}
#endif
	}


	void Methods(Atanor* v) {
		hmap<string, string>::iterator it;
		for (it = infomethods.begin(); it != infomethods.end(); it++)
			v->storevalue(it->first);
	}

	string Info(string n) {

		if (infomethods.find(n) != infomethods.end())
			return infomethods[n];
		return "Unknown method";
	}


    //----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* v, Atanor* i, short idthread);
    
	Atanor* Get(Atanor* context, Atanor* v, short idthread);


    short Type() {
        return Atanorsound::idtype;
    }

    string Typename() {
        return "sound";
    }

    bool isString() {
        return false;
    }

    bool isNumber() {
        return false;
    }

    bool isBoolean() {
        return false;
    }

    bool isFrame() {
        return false;
    }

    Atanor* Atom(bool forced=false) {
        return this;
    }

	//If it is too complex to duplicate an element (for instance when passed to a function)
	//then you can use return false...  Usually in that case, Atom should always be just: return this;
	bool duplicateForCall() {
		return false;
	}


    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in kifexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        if (exported.find(n) != exported.end())
            return true;
        return false;
    }

    Atanor* Newinstance(short, Atanor* f = NULL) {
        return new Atanorsound(NULL);
    }

    AtanorIteration* Newiteration(bool direction) {
        return aITERNULL;
    }

    static void AddMethod(AtanorGlobal* g, string name, soundMethod func, unsigned long arity, string infos);
    static bool InitialisationModule(AtanorGlobal* g, string version);

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
	Atanor* MethodLoad(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodPlay(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodStop(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodInfos(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodReset(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodOpen(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodClose(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodDecode(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodEncode(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

    

    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
    //is returned by the Declaration method.
    Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods[idname])(contextualpattern, idthread, callfunc);
    }

	Atanor* loading(short idthread);

    void Clear() {
        //To set a variable back to empty
    }

    void Clean() {
        //Your cleaning code
    }

    string String() {
        Locking _lock(this);
        return path;
    }

  
};

#endif


