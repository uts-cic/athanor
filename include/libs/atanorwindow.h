
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
filename   : atanorwindow.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanorwindow_h
#define atanorwindow_h

#include "atanorwidget.h"

class Doublewindow;
class Fl_Widget;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorwindow;
//This typedef defines a type "windowMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorwindow::*windowMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------
class AtanorCanvas : public Fl_Widget {
public:
	Atanor* function;
	Atanorwindow* window;
	Atanor* object;
	int x, y, w, h;
	int sw, sh;

	AtanorCanvas(int SW, int SH, int X, int Y, int W, int H) : Fl_Widget(X, Y, W, H, NULL) {
		sw = SW;
		sh = SH;
		x = X;
		y = Y;
		w = W;
		h = H;
		function = NULL;
		window = NULL;
		object = aNULL;
	}
	void draw();
};

class Atanorwindow : public Atanorwidget {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static hmap<short, windowMethod> methods;
	static hmap<string, string> infomethods;
	static bin_hash<unsigned long> exported;
	static basebin_hash<bool> nothreads;


	static short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	Doublewindow* window;
	AtanorCanvas* scrollcanvas;
	Fl_Scroll* scroll;

	char txt[1024];
	string label;
	bool end;
	vector<int> items;
	Fl_Menu_Item* flmenu;
	int flmenunbelements;
	FLCallback* onclose;
	FLCallback* timeout;
	int ionclose;
	int itimeout;
	bool closed;

	//---------------------------------------------------------------------------------------------------------------------
	Atanorwindow(AtanorGlobal* g, Atanor* parent = NULL);
	~Atanorwindow();

	//----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* index, Atanor* window, short idthread);
	Atanor* Get(Atanor* context, Atanor* index, short idthread);

	short Type() {
		return Atanorwindow::idtype;
	}

	string Typename() {
		return "window";
	}

	Atanor* Atom(bool forced = false) {
		return this;
	}

	Fl_Widget* Widget() {
		if (scrollcanvas != NULL)
			return scrollcanvas;
		return window;
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
		globalAtanor->windowmode = true;
		Atanorwindow* w = new Atanorwindow(globalAtanor);
		w->function = f;
		return w;
	}

	AtanorIteration* Newiteration(bool direction) {
		return aITERNULL;
	}

	static void AddMethod(AtanorGlobal* g, string name, windowMethod func, unsigned long arity, string infos);

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

	virtual bool iswindowContainer() {
		return false;
	}

	void itemclose();

	bool Update() {
		return update;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	Atanor* MethodInitial(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodBegin(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodEnd(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodRun(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodCreate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

	Atanor* MethodOnClose(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodOntime(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodRedrawing(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodSetFonts(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodGetFont(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodGetFontSizes(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodNbFonts(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodPlot(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* PlotCoords(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodAsk(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodAlert(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodPassword(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodInput(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodMenu(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodResizable(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodSizerange(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodScrollbar(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodModal(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodBorder(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodBackgroundColor(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodDrawColor(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodRectangle(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodVertex(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodRectangleFill(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodLineShape(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodPolygon(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodLoop(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodLine(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodLineRotation(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodPushMatrix(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodPopMatrix(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodScale(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodTranslate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodRotate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodMultmatrix(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodTransform_x(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodTransform_y(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodTransform_dx(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodTransform_dy(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodTransform_vertex(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodBeginPoints(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodEndPoints(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodBeginLine(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodEndLine(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodBeginLoop(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodEndLoop(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodBeginPolygon(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodEndPolygon(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodBeginComplexPolygon(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodGap(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodEndComplexPolygon(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodArc(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodCurve(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodPushClip(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodPopClip(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodPie(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodPoint(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodCircle(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodTextMeasure(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodDrawText(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodOnDragDrop(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodOnMouse(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodOnKey(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodCursorStyle(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodWindowHide(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodFlush(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodPosition(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodSize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodAwake(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodLock(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodUnlock(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodImage(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodBitmap(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
	//is returned by the Declaration method.
	Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		if (globalAtanor->Error(idthread))
			return globalAtanor->Errorobject(idthread);
		
		if (Stopall())
			return aNULL;

		///Specific section to handle threads... One exception the redraw function...
		if (idthread && callfunc->Size() != 0 && !nothreads.check(idname)) {
			//In this case, we do not want to process the method, we postpone its activation
			RecordGUIObject(callfunc, idthread);
			return aNULL;
		}

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

		return label;
	}


	bool Boolean() {

		return true;
	}

	Atanor* same(Atanor* a) {
		if (a->Type() == Atanorwindow::idtype && ((Atanorwindow*)a)->window == window)
			return aTRUE;
		return aFALSE;
	}
};

#endif








