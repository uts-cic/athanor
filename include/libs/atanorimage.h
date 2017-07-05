
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
filename   : atanorimage.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanorimage_h
#define atanorimage_h

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorimage;
//This typedef defines a type "imageMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorimage::*imageMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanorimage : public AtanorObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static hmap<short, imageMethod> methods;
    static hmap<string, string> infomethods;
    static bin_hash<unsigned long> exported;

    static short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    Fl_Image* image;
	string filename;

    //---------------------------------------------------------------------------------------------------------------------
    Atanorimage(AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
        //Do not forget your variable initialisation
		image = NULL;
    }

    Atanorimage() : AtanorObject() {
        //Do not forget your variable initialisation
		image = NULL;
    }

    //----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* index, Atanor* value, short idthread);
	Atanor* Get(Atanor* context, Atanor* index, short idthread);


    short Type() {
        return Atanorimage::idtype;
    }

    string Typename() {
        return "image";
    }

    Atanor* Atom(bool forced=false) {
        if (forced || !protect)
            return new  Atanorimage;
        return this;
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
		return new Atanorimage;
    }

	AtanorIteration* Newiteration(bool direction);

    static void AddMethod(AtanorGlobal* g, string name, imageMethod func, unsigned long arity, string infos);
    
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

	Atanor* MethodLoadImageGIF(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

		if (image != NULL)
			return globalAtanor->Returnerror("WND(805): Image already loaded", idthread);
		Atanor* kfilename = callfunc->Evaluate(0, contextualpattern, idthread);
		filename = kfilename->String();
		if (filename != "") {
			image = new Fl_GIF_Image(filename.c_str());
			if (image == NULL)
				return globalAtanor->Returnerror("WND(805): Image not loaded", idthread);
			SaveItem(this);
			return aTRUE;
		}
		return aFALSE;
	}

#ifndef FLTKNOJPEG
	Atanor* MethodLoadImageJPEG(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

		if (image != NULL)
			return globalAtanor->Returnerror("WND(805): Image already loaded", idthread);
		Atanor* kfilename = callfunc->Evaluate(0, contextualpattern, idthread);
		filename = kfilename->String();
		if (filename != "") {
			image = new Fl_JPEG_Image(filename.c_str());
			if (image == NULL)
				return globalAtanor->Returnerror("WND(805): Image not loaded", idthread);
			SaveItem(this);
			return aTRUE;
		}
		return aFALSE;
	}
#endif


    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
    //is returned by the Declaration method.
    Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		if (globalAtanor->Error(idthread))
			return globalAtanor->Errorobject(idthread);

        return (this->*methods[idname])(contextualpattern, idthread, callfunc);
    }

    void Clear() {
        //To set a variable back to empty
		
    }

    void Clean() {
        //Your cleaning code
    }

	long Size() {
		return 0;
	}

	uchar Byte() {
		
		return 0;
	}

	long Integer() {
		
		return 0;
	}
	
	double Float() {
		
		return 0;
	}
	
	string String() {
		
		return "";
	}

	
	bool Boolean() {
		
		return true;
	}

    
    //Basic operations    
	Atanor* andset(Atanor* a, bool autoself) {
		
	    return this;
	}

	Atanor* orset(Atanor* a, bool autoself) {
		
	    return this;
	}

	Atanor* xorset(Atanor* a, bool autoself) {
		
	    return this;
	}

	Atanor* plus(Atanor* a, bool autoself) {
		
	    return this;
	}
	Atanor* minus(Atanor* a, bool autoself) {
		
	    return this;
	}

	Atanor* multiply(Atanor* a, bool autoself) {
		
	    return this;
	}

	Atanor* divide(Atanor* a, bool autoself) {
		
	    return this;

	}
	Atanor* mod(Atanor* a, bool autoself) {
		
	    return this;
	}

	Atanor* shiftleft(Atanor* a, bool autoself) {
		
	    return this;
	}


	Atanor* shiftright(Atanor* a, bool autoself) {
		
	    return this;
	}

	Atanor* power(Atanor* a, bool autoself) {
		
	    return this;
	}

	Atanor* less(Atanor* a) {
		return aFALSE;
	}
	Atanor* more(Atanor* a) {
		return aFALSE;
	}
	Atanor* same(Atanor* a) {
		return aFALSE;
	}
	Atanor* different(Atanor* a) {
		return aFALSE;
	}

	Atanor* lessequal(Atanor* a) {
		return aFALSE;
	}

	Atanor* moreequal(Atanor* a) {
		return aFALSE;
	}

	Atanor* plusplus() {
		return this;
	}

	Atanor* minusminus() {
		return this;
	}
};


class ButtonImage : public Fl_Button {
public:
	// CONSTRUCTOR
	Fl_Image* myimage;
	bool isbitmap;
	unsigned int thecolor;
	string mytext;
	int myalign;

	ButtonImage(int X, int Y, int W, int H, const char*L = 0) : Fl_Button(X, Y, W, H, L) {
		myimage = NULL;
		myalign = FL_ALIGN_CENTER;
		isbitmap = false;
		thecolor = FL_BLACK;
	}

	void draw() {
		int X = x() + Fl::box_dx(box());  // area inside the button's frame
		int Y = y() + Fl::box_dy(box());
		int W = w() - Fl::box_dw(box());
		int H = h() - Fl::box_dh(box());
		draw_box(value() ? (down_box() ? down_box() : fl_down(box())) : box(), color());  // button up/dn
		fl_push_clip(X, Y, W, H);                  // clip inside button's frame
		{
			if (myimage != NULL) {
				int imgx = X + ((myimage->w() < W) ? (W - myimage->w()) / 2 : 0);
				// horiz center
				int imgy = Y + ((myimage->h() < H) ? (H - myimage->h()) / 2 : 0);
				// vert center
				if (isbitmap)
					fl_color(thecolor);
				myimage->draw(imgx, imgy);
			}
			if (mytext != "") {
				fl_color(labelcolor());
				fl_font(labelfont(), labelsize());
				fl_draw(mytext.c_str(), x(), y(), w(), h(), myalign);
			}
		}
		fl_pop_clip();
	}
};

#endif








