
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
filename   : atanorfltk.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef doublewindow_h
#define doublewindow_h

#ifdef uchar
#undef uchar
#endif

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Table.H>
#include <FL/Fl_Table_Row.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Multiline_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Repeat_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Menu.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/fl_draw.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>
#include <FL/Fl_GIF_Image.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Select_Browser.H>
#include <FL/Fl_Tree.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Progress.H>

#ifndef FLTKNOJPEG
#include <FL/Fl_JPEG_Image.H>
#endif

//--------------------------------------------------------------------------------------
class Atanorwidget;
//--------------------------------------------------------------------------------------
int Fltkrun(short idthread);
bool Stopall();
void Initstopall(bool s);
void Setstopall(bool s);
//--------------------------------------------------------------------------------------
void fltkinitialisationtype();
bool fltk_create_variables(AtanorGlobal* global);
void SaveItem(Atanor* e);
//--------------------------------------------------------------------------------------
extern int fontmax;
extern short redraw_index;
extern map<int, string> ifonts;

extern map<string, unsigned int> boxTypes;
extern map<string, unsigned int> fonts;
extern map<string, unsigned int> buttonShapes;
extern map<string, unsigned int> buttonAction;
extern map<string, unsigned int> keyCodes;
extern map<string, Fl_Color> colors;
extern map<string, unsigned int> aligns;
extern map<string, unsigned int> lineshape;
extern map<string, unsigned int> slidertype;
extern map<string, unsigned int> actions;
extern map<string, unsigned int> labeltypes;
extern map<string, unsigned int> buttontypes;
extern map<string, unsigned int> cursors;
extern map<string, unsigned long> commands;

class widge {
public:
	Fl_Widget* wdg;
	short idthread;
	int id;
	Atanorwidget* kw;

	widge(Fl_Widget* w, Atanorwidget* k, int idg, int i) {
		wdg = w;
		kw = k;
		id = idg;
		idthread = i;
	}

	void redraw();
};

void flredraw(void* data);
//--------------------------------------------------------------------------------------

class Idcouple {
public:

	Atanor* o;
	int id;
	int count;

	Idcouple(Atanor* e = NULL, bool set = false) {
		count = 0;
		o = e;
		if (e != NULL) {
			id = globalAtanor->RecordInTrackerProtected(e);
			if (set) {
				count++;
				o->Setreference();
			}
		}
		else
			id = -1;
	}

	void Use(Atanor* e);

	void Set() {
		if (o != NULL) {
			count++;
			o->Setreference();
		}
	}

	void Reset() {
		if (o != NULL) {
			while (count > 0) {
				globalAtanor->ResetWithTracker(o, id);
				count--;
			}
		}
	}

	AtanorFunction* Function() {
		return (AtanorFunction*)o;
	}

	Atanor* Object() {
		return o;
	}
};

class Atanorwindow;
class Doublewindow : public Fl_Double_Window {
public:
	Atanor* function;
	Atanor* object;
	bool ready;
	Atanorwindow* window;
	int iwindow;

	map<int, Idcouple> functionmouse;
	map<int, Idcouple> functionkey;
	map<int, Idcouple> objectmouse;
	map<int, Idcouple> objectkey;
	bool dragdrop;
	Idcouple functiondragdrop;
	Idcouple objectdragdrop;
	string eventtext;
	AtanorGlobal* global;

	Doublewindow(int x, int y, const char* l, Atanor* f, Atanor* o, Atanor* c, Atanorwindow* wn);
	Doublewindow(int x, int y, int w, int h, const char* l, Atanor* f, Atanor* o, Atanor* c, Atanorwindow* wn);

	~Doublewindow();

	int mouse_callback(int e);
	int key_callback(int e);
	int dragdropcall_back();

	bool Setdragdrop(Atanor* f, Atanor* o) {
		functiondragdrop.Use(f);
		objectdragdrop.Use(o);
		return true;
	}

	bool Setmouseaction(int e, Atanor* f, Atanor* object);

	bool Setkeyaction(int e, Atanor* f, Atanor* object);

	void draw();
	int handle(int e);

};

class FLCallback {
public:

	Atanor* function;
	Atanorwindow* window;
	Atanor* object;
	int iobject;
	double timevalue;

	FLCallback(Atanor* func, Atanor* o, Atanorwindow* win) {
		timevalue = 0;
		function = func;
		window = win;
		object = o;
		object->Setreference();
		iobject = globalAtanor->RecordInTrackerProtected(object);
	}

	void Set(Atanor* func, Atanor* o, Atanorwindow* win) {
		globalAtanor->ResetWithTracker(object, iobject);

		timevalue = 0;
		function = func;
		window = win;
		object = o;
		object->Setreference();
		iobject = globalAtanor->RecordInTrackerProtected(object);
	}

	~FLCallback() {
		globalAtanor->ResetWithTracker(object, iobject);
	}
};


#endif



