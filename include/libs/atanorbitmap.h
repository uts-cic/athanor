
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
filename   : atanorbitmap.h
Date       : 2017/04/01
Purpose    : map implementation
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanorbitmap_h
#define atanorbitmap_h

#include "atanorfltk.h"
//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorbitmap;
//This typedef defines a type "bitmapMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorbitmap::*bitmapMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanorbitmap : public AtanorObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static hmap<short, bitmapMethod> methods;
    static hmap<string, string> infomethods;
    static bin_hash<unsigned long> exported;

    static short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    Fl_Bitmap* bitmap;
	uchar* bm;
	int szw, szh;

    //---------------------------------------------------------------------------------------------------------------------
    Atanorbitmap(Fl_Bitmap* v, AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
        //Do not forget your variable initialisation
        bitmap=v;
		bm = NULL;
    }

    Atanorbitmap() : AtanorObject() {
        //Do not forget your variable initialisation
        bitmap=NULL;
		szw = 0;
		szh = 0;
		bm = NULL;
    }

	~Atanorbitmap() {
		if (bitmap != NULL)
			delete bitmap;
		if (bm != NULL)
			delete[] bm;
	}

    //----------------------------------------------------------------------------------------------------------------------

    short Type() {
        return Atanorbitmap::idtype;
    }

    string Typename() {
        return "bitmap";
    }

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
		return new Atanorbitmap;
    }

    static void AddMethod(AtanorGlobal* g, string name, bitmapMethod func, unsigned long arity, string infos);
    
	static bool InitialisationModule(AtanorGlobal* g, string version);

	
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



	bool isString() {
		return false;
	}

	bool isNumber() {
		return false;
	}

	bool isContainer() {
		return false;
	}

	virtual bool isValueContainer() {
		return false;
	}

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
    //Atanor* MethodSize(AtanorGlobal* global,Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {return aZERO;}

	Atanor* MethodInitial(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		//If you do not have any parameters, then your function might return whatever you want... Here itself
		Atanor* kbitmaps = callfunc->Evaluate(0, aNULL, idthread);
		if (!kbitmaps->isVectorContainer())
			return globalAtanor->Returnerror("WND(809): Wrong type for the first parameter", idthread);

		Atanor* ke = callfunc->Evaluate(1, aNULL, idthread);

		szw = ke->Integer();
		ke->Release();
		ke = callfunc->Evaluate(2, aNULL, idthread);
		szh = ke->Integer();
		ke->Release();

		int sz = kbitmaps->Size();

		int tst = (szw*szh) >> 3;
		if (tst != sz)
			return globalAtanor->Returnerror("WND(811): Wrong size for the bitmap. sz=(width*height)/8", idthread);

		int rowBytes = (szw + 7) >> 3;
		bm = new uchar[rowBytes*szh];
		for (int i = 0; i < sz; i++)
			bm[i] = (uchar)kbitmaps->getinteger(i);
		bitmap = new Fl_Bitmap(bm, szw, szh);
		kbitmaps->Release();
		return this;
	}

	Atanor* MethodLoad(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		if (bitmap != NULL)
			return globalAtanor->Returnerror("WND(808): Bitmap already loaded", idthread);
		Atanor* kbitmaps = callfunc->Evaluate(0, contextualpattern, idthread);
		szw = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
		szh = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
		if (!kbitmaps->isVectorContainer())
			return globalAtanor->Returnerror("WND(809): Wrong type for the first parameter", idthread);

		int sz = kbitmaps->Size();
		int tst = (szw*szh) >> 3;
		if (tst != sz)
			return globalAtanor->Returnerror("WND(811): Wrong size for the bitmap. sz=(width*height)/8", idthread);

		int rowBytes = (szw + 7) >> 3;
		bm = new uchar[rowBytes*szh];

		for (int i = 0; i < sz; i++)
			bm[i] = (uchar)kbitmaps->getinteger(i);
		bitmap = new Fl_Bitmap(bm, szw, szh);
		//delete[] bm;
		return aTRUE;
	}

    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
    //is returned by the Declaration method.
    Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods[idname])(contextualpattern, idthread, callfunc);
    }
};

#endif








