
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
filename   : atanorwindow.cxx
Date       : 2017/04/01
Purpose    : window implementation (based on FLTK)
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorwindow.h"
#include "atanorwidget.h"
#include "atoms.h"
#include "vectors.h"
#include "atanorbitmap.h"
#include "atanorimage.h"
#include "instructions.h"

#ifdef WIN32
void FlResetTimer();
#endif


#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
//-----------------------------------------------------------------------------------

bool Ready();
//-------------------------------------------------------------------
//This index is used to detect the redraw method
short redraw_index = 0;
//-------------------------------------------------------------------
static vector<Atanorwindow*> current_window;
static ThreadLock windowLock(NULL, false);
static Atanorwindow* mainwindow = NULL;

void Clearwindows() {
	Locking _lock(windowLock);
	mainwindow = NULL;
}

void TerminateMainWindow() {
	Locking _lock(windowLock);
	if (mainwindow != NULL) {
		if (mainwindow->window != NULL) {
			delete mainwindow->window;
		}
		mainwindow = NULL;
	}		
}

//-------------------------------------------------------------------
static void menu_callback(Fl_Widget *w, void *data) {
	FLCallback* wn = (FLCallback*)data;
	Atanorwindow* kwnd = wn->window;
	AtanorCallFunction kfunc(wn->function);

	kwnd->Setreference();
	wn->object->Setreference();
	kfunc.arguments.push_back(kwnd);
	kfunc.arguments.push_back(wn->object);
	kfunc.Get(aNULL, aNULL, globalAtanor->GetThreadid());
	kwnd->Resetreference();
	wn->object->Resetreference();
}

static void close_callback(Fl_Widget *w, void *data) {
	Locking _lock(windowLock);
	Atanorwindow* kwnd = (Atanorwindow*)data;
	if (kwnd->window!=NULL)
		delete kwnd->window;
}

static void window_callback(Fl_Widget *w, void *data) {
	Locking _lock(windowLock);
	FLCallback* wn = (FLCallback*)data;
	Atanorwindow* kwnd = wn->window;
	AtanorCallFunction kfunc(wn->function);

	kwnd->Setreference();
	wn->object->Setreference();
	kfunc.arguments.push_back(kwnd);
	kfunc.arguments.push_back(wn->object);
	Atanor* ret = kfunc.Get(aNULL, aNULL, globalAtanor->GetThreadid());
	kwnd->Resetreference();
	wn->object->Resetreference();

	if (ret->Boolean() == false)
		return;

	kwnd->closed = true;
	delete kwnd->window;
}

static void timeout_callback(void *data) {
	if (Stopall())
		return;

	FLCallback* wn = (FLCallback*)data;
	Atanorwindow* kwnd = wn->window;
	if (!Ready()) {
		Fl::repeat_timeout(wn->timevalue, timeout_callback, data);
		return;
	}

	if (wn->function == aNULL) {
		if (Stopall())
			return;

		wn->window->window->redraw();
		Fl::repeat_timeout(wn->timevalue, timeout_callback, data);
		return;
	}

	AtanorCallFunction kfunc(wn->function);
	kwnd->Setreference();
	wn->object->Setreference();
	kfunc.arguments.push_back(kwnd);
	kfunc.arguments.push_back(wn->object);
	Atanor* ret = kfunc.Get(aNULL, aNULL, globalAtanor->GetThreadid());
	kwnd->Resetreference();
	wn->object->Resetreference();

	double i = ret->Float();
	ret->Release();
	if (i <= 0 || Stopall())
		return;
	Fl::repeat_timeout(i, timeout_callback, data);
}

void SaveItem(Atanor* e) {
	Locking _lock(windowLock);
	if (current_window.size() != 0) {
		Atanorwindow* w = current_window.back();
		long id = e->idtracker;
		w->items.push_back(id);
	}
}

//-------------------------------------------------------------------
//We need to declare once again our local definitions.
hmap<short, windowMethod>  Atanorwindow::methods;
hmap<string, string> Atanorwindow::infomethods;
bin_hash<unsigned long> Atanorwindow::exported;
basebin_hash<bool> Atanorwindow::nothreads;


short Atanorwindow::idtype = 0;
short Atanorwidget::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorwindow::AddMethod(AtanorGlobal* global, string name, windowMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorwindow::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();
	nothreads.clear();
	
	Atanorwindow::idtype = global->Getid("window");
	Atanorwidget::idtype = global->Getid("widget");
	Initstopall(false);
	mainwindow = NULL;

	//We record a specific id for the redraw method (used everywhere, but whose behaviour depends on whether we are in a thread)
	redraw_index = globalAtanor->Getid("redraw");

	Atanorwindow::AddMethod(global, "_initial", &Atanorwidget::MethodInitial, P_ONE, "_initial(variable): Creation of a window with a call back object");
	Atanorwindow::AddMethod(global, "begin", &Atanorwindow::MethodBegin, P_THREE | P_FIVE, "begin(int x,int y,int w, int h,string title): Create a window and begin initialisation, w and h are optional");
	Atanorwindow::AddMethod(global, "end", &Atanorwindow::MethodEnd, P_NONE, "end(): end creation");
	Atanorwindow::AddMethod(global, "run", &Atanorwindow::MethodRun, P_NONE, "run(): Launch the GUI");
	Atanorwindow::AddMethod(global, "create", &Atanorwindow::MethodCreate, P_THREE | P_FIVE, "create(int x,int y,int w, int h,string title): Create a window without widgets w and h are optional");

	Atanorwindow::AddMethod(global, "onclose", &Atanorwindow::MethodOnClose, P_TWO, "onclose(function, object): Callback to call on close");
	Atanorwindow::AddMethod(global, "redrawing", &Atanorwindow::MethodRedrawing, P_ONE, "redrawing(double timelapse): Set a time out to redraw the window every 'timelapase'");
	Atanorwindow::AddMethod(global, "ontime", &Atanorwindow::MethodOntime, P_THREE, "ontime(function,double timelapse, object): Set a time out function");
	Atanorwindow::AddMethod(global, "initializefonts", &Atanorwindow::MethodSetFonts, P_NONE, "initializefonts(): load fonts from system. Return the number of available fonts");
	Atanorwindow::AddMethod(global, "getfont", &Atanorwindow::MethodGetFont, P_ONE, "getfont(int num): get font name.");
	Atanorwindow::AddMethod(global, "getfontsizes", &Atanorwindow::MethodGetFontSizes, P_ONE, "getfontsizes(int num): return a vector of available font sizes.");
	Atanorwindow::AddMethod(global, "fontnumber", &Atanorwindow::MethodNbFonts, P_NONE, "fontnumber(): Return the number of available fonts.");
	Atanorwindow::AddMethod(global, "plot", &Atanorwindow::MethodPlot, P_TWO | P_THREE, "plot(fvector xy,int thickness,fvector landmarks): Plot a graph from a table of successive xy points according to window size. If thickness===0 then points are continuously plotted else defines the diameter of the point. Return a float vector which is used with plotcoords. The landmark vector is optional it is has the following structure: [XmaxWindow,YmaxWindow,XminValue,YminValue,XmaxValue,YmaxValue,incX,incY]. incX,incY are also optional.");
	Atanorwindow::AddMethod(global, "plotcoords", &Atanorwindow::PlotCoords, P_THREE, "plotcoords(fvector inits,float x,float y): Compute the coordinates of a point(xy) according to the previous scale computed with plot. Returns a vector of two elements [xsys] corresponding to the screen coordinates in the current window.");
	Atanorwindow::AddMethod(global, "ask", &Atanorwindow::MethodAsk, P_THREE | P_FOUR | P_FIVE, "ask(string msg,string buttonmsg1,string buttonmsg2,string buttonmsg3): Pop up window to pose a question");
	Atanorwindow::AddMethod(global, "alert", &Atanorwindow::MethodAlert, P_ONE, "alert(string msg): Pop up window to display an alert");
	Atanorwindow::AddMethod(global, "password", &Atanorwindow::MethodPassword, P_ONE, "password(string msg): Pop up window to input a password");
	Atanorwindow::AddMethod(global, "get", &Atanorwindow::MethodInput, P_NONE | P_ONE, "get(string msg): Pop up window to get a value from the user");
	Atanorwindow::AddMethod(global, "menu", &Atanorwindow::MethodMenu, P_FIVE, "menu(vector,int x,int y,int w, int h): initialize a menu with its callback functions");
	Atanorwindow::AddMethod(global, "resizable", &Atanorwindow::MethodResizable, P_ONE | P_NONE, "resizable(object): make the object resizable");
	Atanorwindow::AddMethod(global, "sizerange", &Atanorwindow::MethodSizerange, P_FOUR, "sizerange(int minw,int minh, int maxw,int maxh): define range in which the size of the window can evolve");
	Atanorwindow::AddMethod(global, "scrollbar", &Atanorwindow::MethodScrollbar, P_SIX, "scrollbar(int minw,int minh, int maxw,int maxh,int maxcanvasw, int maxcanvash): Insert scroll bars");
	Atanorwindow::AddMethod(global, "modal", &Atanorwindow::MethodModal, P_ONE | P_NONE, "modal(bool b): If true make the window modal. If no parameter return if the window is modal");
	Atanorwindow::AddMethod(global, "border", &Atanorwindow::MethodBorder, P_ONE | P_NONE, "border(bool b): If true add or remove borders. If no parameter return if the window has borders");
	Atanorwindow::AddMethod(global, "backgroundcolor", &Atanorwindow::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
	Atanorwindow::AddMethod(global, "drawcolor", &Atanorwidget::MethodDrawColor, P_ONE, "drawcolor(int c|string c): set the color for the next drawings");
	Atanorwindow::AddMethod(global, "rectangle", &Atanorwidget::MethodRectangle, P_FOUR | P_FIVE, "rectangle(int x, int y, int w , int h , string c|int c): Draw a rectangle with optional color c");
	Atanorwindow::AddMethod(global, "vertex", &Atanorwidget::MethodVertex, P_TWO, "vertex(float x, float y): Add a vertex to a complex structure");
	Atanorwindow::AddMethod(global, "rectanglefill", &Atanorwidget::MethodRectangleFill, P_FOUR | P_FIVE, "rectanglefill(int x, int y, int w , int h , string c|int c): Fill a rectangle with optional color c");
	Atanorwindow::AddMethod(global, "lineshape", &Atanorwidget::MethodLineShape, P_TWO, "lineshape(string type, int width): Select the line shape and its thikness");
	Atanorwindow::AddMethod(global, "polygon", &Atanorwidget::MethodPolygon, P_SIX | P_EIGHT, "polygon(int x, int y, int x1, int y1,int x2, int y2, int x3, int y3): Draw a polygon x3 and y3 are optional");
	Atanorwindow::AddMethod(global, "loop", &Atanorwidget::MethodLoop, P_SIX | P_EIGHT, "loop(int x, int y, int x1, int y1,int x2, int y2, int x3, int y3): Draw a series of lines x3 and y3 are optional");
	Atanorwindow::AddMethod(global, "line", &Atanorwidget::MethodLine, P_FOUR | P_SIX, "line(int x, int y, int x1, int y1,int x2, int y2): Draw a line between points x2 and y2 are optional");
	Atanorwindow::AddMethod(global, "rotation", &Atanorwidget::MethodLineRotation, P_FOUR | P_FIVE, "rotation(float x, float y, float distance , float angle , bool draw): Compute the coordinate of a rotated point from point xy using a distance and an angle. Return a vector of floats with the new coordinates.");
	Atanorwindow::AddMethod(global, "pushmatrix", &Atanorwidget::MethodPushMatrix, P_NONE, "pushmatrix(): Save the current transformation");
	Atanorwindow::AddMethod(global, "popmatrix", &Atanorwidget::MethodPopMatrix, P_NONE, "popmatrix(): Restore the current transformation");
	Atanorwindow::AddMethod(global, "scale", &Atanorwidget::MethodScale, P_TWO | P_ONE, "scale(float x, float y)|scale(float x): Scale the current transformation");
	Atanorwindow::AddMethod(global, "translate", &Atanorwidget::MethodTranslate, P_TWO, "translate(float x, float y): translate the current transformation");
	Atanorwindow::AddMethod(global, "rotate", &Atanorwidget::MethodRotate, P_ONE, "rotate(float d): rotate of degree d the current transformation");
	Atanorwindow::AddMethod(global, "multmatrix", &Atanorwidget::MethodMultmatrix, P_SIX, "multmatrix(float a , float b , float c , float d , float x , float y): combine transformations");
	Atanorwindow::AddMethod(global, "transformx", &Atanorwidget::MethodTransform_x, P_TWO, "transformx(float x , float y): Transform a coordinate X using the current transformation matrix.");
	Atanorwindow::AddMethod(global, "transformy", &Atanorwidget::MethodTransform_y, P_TWO, "transformy(float x , float y): Transform a coordinate Y using the current transformation matrix.");
	Atanorwindow::AddMethod(global, "transformdx", &Atanorwidget::MethodTransform_dx, P_TWO, "transformdx(float x , float y): Transform a distance DX using the current transformation matrix.");
	Atanorwindow::AddMethod(global, "transformdy", &Atanorwidget::MethodTransform_dy, P_TWO, "transformdy(float x , float y): Transform a distance DY using the current transformation matrix.");
	Atanorwindow::AddMethod(global, "transformedvertex", &Atanorwidget::MethodTransform_vertex, P_TWO, "transformedvertex(float x , float y): add transformations to vertices list.");
	Atanorwindow::AddMethod(global, "beginpoints", &Atanorwidget::MethodBeginPoints, P_NONE, "beginpoints(): Start accumulating vertices");
	Atanorwindow::AddMethod(global, "endpoints", &Atanorwidget::MethodEndPoints, P_NONE, "endpoints(): Stop accumulating vertices");
	Atanorwindow::AddMethod(global, "beginline", &Atanorwidget::MethodBeginLine, P_NONE, "beginline(): Start drawing lines.");
	Atanorwindow::AddMethod(global, "endline", &Atanorwidget::MethodEndLine, P_NONE, "endline(): End drawing lines.");
	Atanorwindow::AddMethod(global, "beginloop", &Atanorwidget::MethodBeginLoop, P_NONE, "beginloop(): Start drawing a closed sequence of lines.");
	Atanorwindow::AddMethod(global, "endloop", &Atanorwidget::MethodEndLoop, P_NONE, "endloop(): End drawing a closed sequence of lines.");
	Atanorwindow::AddMethod(global, "beginpolygon", &Atanorwidget::MethodBeginPolygon, P_NONE, "beginpolygon(): Start drawing a convex filled polygon.");
	Atanorwindow::AddMethod(global, "endpolygon", &Atanorwidget::MethodEndPolygon, P_NONE, "endpolygon(): End drawing a convex filled polygon.");
	Atanorwindow::AddMethod(global, "begincomplexpolygon", &Atanorwidget::MethodBeginComplexPolygon, P_NONE, "begincomplexpolygon(): Start drawing a complex filled polygon.");
	Atanorwindow::AddMethod(global, "gap", &Atanorwidget::MethodGap, P_NONE, "gap(): Fill the gap while drawing a complex polygon.");
	Atanorwindow::AddMethod(global, "endcomplexpolygon", &Atanorwidget::MethodEndComplexPolygon, P_NONE, "endcomplexpolygon(): End drawing a complex filled polygon.");
	Atanorwindow::AddMethod(global, "arc", &Atanorwidget::MethodArc, P_FIVE | P_SIX, "arc(int x, int y, int x1, int y1, float a1 , float a2): Draw an arc.\rarc(float xfloat yfloat radfloat a1float a2): Add a series of points to the current path on the arc of a circle;");
	Atanorwindow::AddMethod(global, "curve", &Atanorwidget::MethodCurve, P_EIGHT, "curve(float x, float y, float x1, float y1, float x1, float y2, float x3, float y3): Add a series of points on a Bezier curve to the path. The curve ends (and two of the points) are at xy and x3y3.");
	Atanorwindow::AddMethod(global, "pushclip", &Atanorwidget::MethodPushClip, P_FOUR, "pushclip(int x, int y, int w , int h): Insert a clip region with the following coordinates");
	Atanorwindow::AddMethod(global, "popclip", &Atanorwidget::MethodPopClip, P_NONE, "popclip(): Release a clip region");
	Atanorwindow::AddMethod(global, "pie", &Atanorwidget::MethodPie, P_SIX, "pie(int x, int y, int x1, int y1, float a1 , float a2): Draw a pie");
	Atanorwindow::AddMethod(global, "point", &Atanorwidget::MethodPoint, P_TWO, "po, int(int x, int y): Draw a pixel");
	Atanorwindow::AddMethod(global, "circle", &Atanorwidget::MethodCircle, P_THREE | P_FOUR, "circle(int x, int y, int r, int color): Draw a circle. 'color' is optional.");
	Atanorwindow::AddMethod(global, "textsize", &Atanorwidget::MethodTextMeasure, P_ONE, "textsize(string l): Return a map with w and h as key to denote width and height of the string in pixels");
	Atanorwindow::AddMethod(global, "drawtext", &Atanorwidget::MethodDrawText, P_THREE, "drawtext(string l,int x,int y): Put a text at position xy");
	Atanorwindow::AddMethod(global, "redraw", &Atanorwidget::MethodRedraw, P_NONE | P_ONE, "redraw(): Redraw the window");
	Atanorwindow::AddMethod(global, "rgbcolor", &Atanorwindow::MethodRGBColor, P_ONE | P_THREE, "rgbcolor(string|int color)|(int r, int g, int b)|(vector color): either return a vector of the color decomposition or return the value of rgb values combined");
	Atanorwindow::AddMethod(global, "font", &Atanorwindow::MethodTextFont, P_TWO, "font(string f,int sz): Set the font name and its size");
	Atanorwindow::AddMethod(global, "ondragdrop", &Atanorwindow::MethodOnDragDrop, P_TWO, "ondragdrop(function, object): Set the call back function for drag and drop action.");
	Atanorwindow::AddMethod(global, "onmouse", &Atanorwindow::MethodOnMouse, P_THREE, "onmouse(int action function, object): Set the call back function on a mouse action with a given object as parameter");
	Atanorwindow::AddMethod(global, "onkey", &Atanorwindow::MethodOnKey, P_THREE, "onkey(int action function, object): Set the call back function on a keyboard action with a given object as parameter");
	Atanorwindow::AddMethod(global, "cursorstyle", &Atanorwindow::MethodCursorStyle, P_THREE, "cursorstyle(int cursortype , int color, int color): Set the cursor shape");
	Atanorwindow::AddMethod(global, "hide", &Atanorwindow::MethodWindowHide, P_ONE, "hide(bool h): Hide the window if h is true");
	Atanorwindow::AddMethod(global, "show", &Atanorwindow::MethodShow, P_NONE, "show(): Show the window");
	Atanorwindow::AddMethod(global, "focus", &Atanorwindow::MethodFocus, P_NONE, "focus(): Get the focus");
	Atanorwindow::AddMethod(global, "flush", &Atanorwindow::MethodFlush, P_NONE, "flush(): force redrawing of window");
	Atanorwindow::AddMethod(global, "position", &Atanorwindow::MethodPosition, P_NONE | P_TWO, "position()|(int x, int y): Return the window position or set the window position");
	Atanorwindow::AddMethod(global, "size", &Atanorwindow::MethodSize, P_NONE | P_FOUR, "size()|(int x, int y, int w , int h): Return the window size or set the window size");
	Atanorwindow::AddMethod(global, "image", &Atanorwindow::MethodImage, P_FIVE, "image(image image, int x , int y , int w , int h): Display an image");
	Atanorwindow::AddMethod(global, "bitmap", &Atanorwindow::MethodBitmap, P_FOUR | P_SIX, "bitmap(bitmap image, int color, int x , int y , int w , int h): Display a bitmap");
	Atanorwindow::AddMethod(global, "labelsize", &Atanorwindow::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
	Atanorwindow::AddMethod(global, "labelfont", &Atanorwindow::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
	Atanorwindow::AddMethod(global, "labelcolor", &Atanorwindow::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
	Atanorwindow::AddMethod(global, "labeltype", &Atanorwindow::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
	Atanorwindow::AddMethod(global, "label", &Atanorwindow::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
	Atanorwindow::AddMethod(global, "selectioncolor", &Atanorwindow::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
	Atanorwindow::AddMethod(global, "boundaries", &Atanorwindow::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "boundaries(ivector crds): return a vector of the widget coordinates");
	Atanorwindow::AddMethod(global, "created", &Atanorwindow::MethodCreated, P_NONE, "created(): Return true if the object has been created");
	Atanorwindow::AddMethod(global, "copy", &Atanorwindow::MethodCopy, P_ONE, "copy(string): Copy string to clipboard");
	Atanorwindow::AddMethod(global, "paste", &Atanorwindow::MethodPaste, P_NONE, "paste(): paste string");
	Atanorwindow::AddMethod(global, "align", &Atanorwindow::MethodAlign, P_ONE, "align(int): define the label alignment");


	nothreads[global->Getid("getfont")] = true; //atanorwindow
	nothreads[global->Getid("getfontsizes")] = true; //atanorwindow
	nothreads[global->Getid("textsize")] = true; //atanorwindow
	nothreads[global->Getid("rgbcolor")] = true; //atanorwindow

	global->newInstance[Atanorwindow::idtype] = new Atanorwindow(global);
	global->RecordMethods(Atanorwindow::idtype, Atanorwindow::exported);

	fltkinitialisationtype();
	fltk_create_variables(global);
	return true;
}

void AtanorCanvas::draw() {	
	if (function != NULL) {
		fl_color(FL_BLACK); //we set FL_BLACK as the default color, it can be modified with drawcolor in the code...
		AtanorCallFunction call(function);
		call.arguments.push_back(window);
		call.arguments.push_back(object);
		Atanor* a = call.Get(aNULL, aNULL, globalAtanor->GetThreadid());
		a->Release();		
		window->scroll->scrollbar.redraw();
		window->scroll->hscrollbar.redraw();
	}
}

Atanorwindow::Atanorwindow(AtanorGlobal* g, Atanor* parent) : Atanorwidget(g, parent) {
	//Do not forget your variable initialisation
	window = NULL;
	scrollcanvas = NULL;
	scroll = NULL;
	end = false;
	flmenu = NULL;
	flmenunbelements = 0;
	onclose = NULL;
	timeout = NULL;
	closed = false;
	Setstopall(false);
}

Atanorwindow::~Atanorwindow() {
	itemclose();
	if (window != NULL)
		delete window;
}


void Atanorwindow::itemclose() {
	if (flmenunbelements) {
		for (int i = 0; i < flmenunbelements; i++) {
			if (flmenu[i].text != NULL) {
				free((char*)flmenu[i].text);
				delete (FLCallback*)flmenu[i].user_data();
			}
		}
		delete[] flmenu;
		flmenunbelements = 0;
	}

	Atanor* ke;
	for (int i = 0; i < items.size(); i++) {
		ke = globalAtanor->GetFromTracker(items[i]);
		if (ke != NULL)
			((Atanorwidget*)ke)->itemclose();
	}
	
	items.clear();
	if (mainwindow == this)
		Setstopall(true);
	
	if (onclose != NULL)
		delete onclose;
	if (timeout != NULL)
		delete timeout;
	
	if (scrollcanvas != NULL)
		delete scrollcanvas;
	
	if (scroll != NULL)
		delete scroll;

	scroll = NULL;
	scrollcanvas = NULL;
	onclose = NULL;
	timeout = NULL;
	function = NULL;
}


Atanor* Atanorwindow::Put(Atanor* index, Atanor* value, short idthread) {
	return this;
}

Atanor* Atanorwindow::Get(Atanor* context, Atanor* index, short idthread) {
	return this;
}

Atanor* Atanorwindow::MethodCreate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	if (window != NULL)
		return globalAtanor->Returnerror("Cannot recreate an existing window", idthread);

	//0 is the first parameter and so on...
	Atanor* x = callfunc->Evaluate(0, aNULL, idthread);
	Atanor* y = callfunc->Evaluate(1, aNULL, idthread);
	if (callfunc->Size() == 3) {
		label = callfunc->Evaluate(2, aNULL, idthread)->String();
		strcpy_s(txt, 1024, label.c_str());
		window = new Doublewindow(x->Integer(), y->Integer(), x->Integer() + 100, y->Integer() + 100, txt, function, object, contextualpattern, this);
		window->size_range(x->Integer(), y->Integer());
	}
	else {
		Atanor* wx = callfunc->Evaluate(2, aNULL, idthread);
		Atanor* hy = callfunc->Evaluate(3, aNULL, idthread);
		label = callfunc->Evaluate(4, aNULL, idthread)->String();
		strcpy_s(txt, 1024, label.c_str());
		window = new Doublewindow(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt, function, object, contextualpattern, this);
	}

	current_window.push_back(this);
	end = true;
	window->callback(close_callback, this);
	window->end();
	window->show();
	return aTRUE;
}

Atanor* Atanorwindow::MethodRun(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (window == NULL)
		return globalAtanor->Returnerror("No window available", idthread);

	if (end == false) {
		end = true;
		window->end();
		window->show();
	}
	
	if (mainwindow == NULL)
		mainwindow = this;

	if (timeout != NULL) {
#ifdef WIN32
		FlResetTimer();
#endif
		Fl::add_timeout(timeout->timevalue, timeout_callback, (void*)timeout);
	}

	Fltkrun(idthread);
	mainwindow = NULL;
	return aTRUE;
}

Atanor* Atanorwindow::MethodEnd(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (window == NULL)
		return globalAtanor->Returnerror("No window available", idthread);
	if (end)
		return globalAtanor->Returnerror("Window already instantiated", idthread);

	end = true;

	window->end();
	if (scrollcanvas != NULL)
		window->resizable(scrollcanvas);

	window->show();
	current_window.pop_back();

	return aTRUE;
}

Atanor* Atanorwindow::MethodBegin(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (window != NULL)
		return globalAtanor->Returnerror("Cannot recreate an existing window", idthread);

	//0 is the first parameter and so on...
	Atanor* x = callfunc->Evaluate(0, aNULL, idthread);
	Atanor* y = callfunc->Evaluate(1, aNULL, idthread);

	if (callfunc->Size() == 3) {
		label = callfunc->Evaluate(2, aNULL, idthread)->String();
		strcpy_s(txt, 1024, label.c_str());
		window = new Doublewindow(x->Integer(), y->Integer(), txt, function, object, contextualpattern, this);
		window->size_range(x->Integer(), y->Integer());
	}
	else {
		Atanor* wx = callfunc->Evaluate(2, aNULL, idthread);
		Atanor* hy = callfunc->Evaluate(3, aNULL, idthread);
		label = callfunc->Evaluate(4, aNULL, idthread)->String();
		strcpy_s(txt, 1024, label.c_str());
		window = new Doublewindow(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt, function, object, contextualpattern, this);
	}
	current_window.push_back(this);
	window->callback(close_callback, this);
	return aTRUE;

}

Atanor* Atanorwindow::MethodBitmap(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (window == NULL)
		return globalAtanor->Returnerror("WND(303): No window available", idthread);
	Atanor* kbitmap = callfunc->Evaluate(0, contextualpattern, idthread);
	if (kbitmap->Type() != Atanorbitmap::idtype)
		return globalAtanor->Returnerror("WND(169): Unknown bitmap", idthread);
	Atanor* ke = callfunc->Evaluate(1, contextualpattern, idthread);
	Fl_Color color;
	if (ke->Type() == a_string) {
		//0 is the first parameter and so on...
		string buf = ke->String();
		if (colors.find(buf) == colors.end()) {
			buf = "WND(657): Unknown color code";
			return globalAtanor->Returnerror(buf, idthread);
		}
		color = colors[buf];
	}
	else
		color = ke->Integer();
	int x = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
	int y = callfunc->Evaluate(3, contextualpattern, idthread)->Integer();
	int w = ((Atanorbitmap*)kbitmap)->szw;
	int h = ((Atanorbitmap*)kbitmap)->szh;
	if (callfunc->Size() == 6) {
		w = callfunc->Evaluate(4, contextualpattern, idthread)->Integer();
		h = callfunc->Evaluate(5, contextualpattern, idthread)->Integer();
	}
	Fl_Bitmap* fl = ((Atanorbitmap*)kbitmap)->bitmap;
	if (fl == NULL)
		return globalAtanor->Returnerror("WND(169): Unknown bitmap", idthread);
	if (!end) {
		Fl_Box* box = new Fl_Box(x, y, w, h);
		box->labelcolor(color);
		box->image(fl);
		return aTRUE;
	}
	fl_color(color);
	fl->draw(x, y, w, h);
	return aTRUE;
}

Atanor* Atanorwindow::MethodImage(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (window == NULL)
		return globalAtanor->Returnerror("WND(303): No window available", idthread);
	Atanor* kimage = callfunc->Evaluate(0, contextualpattern, idthread);
	if (kimage->Type() != Atanorimage::idtype)
		return globalAtanor->Returnerror("WND(167): Unknown image", idthread);
	Atanor* kx = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* ky = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* kw = callfunc->Evaluate(3, contextualpattern, idthread);
	Atanor* kh = callfunc->Evaluate(4, contextualpattern, idthread);
	Fl_Image* fl = ((Atanorimage*)kimage)->image;
	if (fl == NULL)
		return globalAtanor->Returnerror("WND(168): Unloaded image", idthread);
	if (!end) {
		Fl_Box* box = new Fl_Box(kx->Integer(), ky->Integer(), kw->Integer(), kh->Integer());
		box->image(fl);
		return aTRUE;
	}
	fl->draw(kx->Integer(), ky->Integer(), kw->Integer(), kh->Integer());
	return aTRUE;
}


Atanor* Atanorwindow::MethodAwake(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	if (window == NULL)
		return globalAtanor->Returnerror("WND(303): No window available", idthread);
	Fl::awake();
	return aTRUE;
}

Atanor* Atanorwindow::MethodLock(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	Fl::lock();
	return aTRUE;
}

Atanor* Atanorwindow::MethodUnlock(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	Fl::unlock();
	return aTRUE;
}

Atanor* Atanorwindow::MethodSize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	//In our example, we have only two parameters
	if (window == NULL)
		return globalAtanor->Returnerror("WND(303): No window available", idthread);
	if (callfunc->Size() == 0) {
		Atanorivector* kvect = new Atanorivector;
		kvect->values.push_back(window->x());
		kvect->values.push_back(window->y());
		kvect->values.push_back(window->w());
		kvect->values.push_back(window->h());
		return kvect;
	}
	//0 is the first parameter and so on...
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* wx = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* hy = callfunc->Evaluate(3, contextualpattern, idthread);
	window->resize(x->Integer(), y->Integer(), wx->Integer(), hy->Integer());
	return aTRUE;
}

Atanor* Atanorwindow::MethodPosition(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	//In our example, we have only two parameters
	if (window == NULL)
		return globalAtanor->Returnerror("WND(303): No window available", idthread);
	if (callfunc->Size() == 0) {
		Atanorivector* kvect = new Atanorivector;
		kvect->values.push_back(window->x_root());
		kvect->values.push_back(window->y_root());
		return kvect;
	}
	//0 is the first parameter and so on...
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	window->position(x->Integer(), y->Integer());
	return aTRUE;
}

Atanor* Atanorwindow::MethodFlush(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//If we are in the main thread, well business as usual
	if (window == NULL)
		return globalAtanor->Returnerror("WND(303): No window available", idthread);
	window->flush();
	return aTRUE;
}

Atanor* Atanorwindow::MethodWindowHide(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (window == NULL)
		return globalAtanor->Returnerror("WND(303): No window available", idthread);
	//0 is the first parameter and so on...
	bool kaction = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
	if (kaction == true) {
		if (window->shown())
			window->hide();
	}
	else
	if (!window->shown())
		window->show();
	return aTRUE;
}


Atanor* Atanorwindow::MethodOnKey(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (window == NULL)
		return globalAtanor->Returnerror("WND(303): No window available", idthread);
	if (end)
		return globalAtanor->Returnerror("WND(102): Window already instantiated", idthread);
	//0 is the first parameter and so on...
	Atanor* kaction = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* function = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* object = callfunc->arguments[2];
	if (!object->isCallVariable())
		object = object->Get(aNULL, aNULL, idthread);

	int act;
	if (kaction->Type() == a_string) {
		string action = kaction->String();
		if (actions.find(action) == actions.end())
			return globalAtanor->Returnerror("WND(344): Unknown keyboard action", idthread);
		act = actions[action];
	}
	else
		act = kaction->Integer();
	if (window->Setkeyaction(act, function, object) == false)
		return globalAtanor->Returnerror("WND(343): Wrong parameter in your keyboard callback", idthread);
	return aTRUE;
}

Atanor* Atanorwindow::MethodCursorStyle(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (window == NULL)
		return globalAtanor->Returnerror("WND(303): No window available", idthread);
	//0 is the first parameter and so on...
	Atanor* kaction = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* kcolor1 = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* kcolor2 = callfunc->Evaluate(2, contextualpattern, idthread);
	Fl_Cursor act;
	if (kaction->Type() == a_string) {
		string action = kaction->String();
		if (cursors.find(action) == cursors.end())
			return globalAtanor->Returnerror("WND(344): Unknown keyboard action", idthread);
		act = (Fl_Cursor)actions[action];
	}
	else
		act = (Fl_Cursor)kaction->Integer();
	window->cursor(act, kcolor1->Integer(), kcolor2->Integer());
	return aTRUE;
}

Atanor* Atanorwindow::MethodOnDragDrop(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (window == NULL)
		return globalAtanor->Returnerror("WND(303): No window available", idthread);
	//0 is the first parameter and so on...
	Atanor* function = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* object = callfunc->arguments[1];
	if (!object->isCallVariable())
		object = object->Get(aNULL, aNULL, idthread);

	window->Setdragdrop(function, object);
	return aTRUE;
}

Atanor* Atanorwindow::MethodOnMouse(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (window == NULL)
		return globalAtanor->Returnerror("WND(303): No window available", idthread);
	if (end)
		return globalAtanor->Returnerror("WND(102): Window already instantiated", idthread);
	//0 is the first parameter and so on...
	Atanor* kaction = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* function = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* object = callfunc->arguments[2];
	if (!object->isCallVariable())
		object = object->Get(aNULL, aNULL, idthread);

	int act;
	if (kaction->Type() == a_string) {
		string action = kaction->String();
		if (actions.find(action) == actions.end())
			return globalAtanor->Returnerror("WND(341): Unknown mouse action", idthread);
		act = actions[action];
	}
	else
		act = kaction->Integer();
	if (window->Setmouseaction(act, function, object) == false)
		return globalAtanor->Returnerror("WND(342): Wrong parameter in your mouse callback", idthread);
	return aTRUE;
}

Atanor* Atanorwindow::MethodMenu(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (window == NULL)
		return globalAtanor->Returnerror("WND(303): No window available", idthread);
	if (end)
		return globalAtanor->Returnerror("WND(102): Window already instantiated", idthread);
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	//we expect a vector
	if (ke->isVectorContainer() == false)
		return globalAtanor->Returnerror("WND(109): Wrong parameter, vector expected", idthread);
	Atanor* menu = ke;
	vector<Fl_Menu_Item*> elements;
	Fl_Menu_Item* fl;
	for (int i = 0; i < menu->Size(); i++) {
		//First level
		//each element is a vector
		ke = menu->getvalue(i);
		if (ke->isVectorContainer() == false)
			return globalAtanor->Returnerror("WND(109): Wrong element, vector expected", idthread);
		Atanor* element = ke;
		//the first element should be the name of our element
		if (element->Size() != 0) {
			ke = element->getvalue(0);
			string nn = ke->String();
			char* n = strdup(STR(nn));
			fl = new Fl_Menu_Item;
			fl->text = n;
			fl->shortcut_ = 0;
			fl->callback_ = NULL;
			fl->user_data_ = 0;
			fl->flags = FL_SUBMENU;
			fl->labeltype_ = 0;
			fl->labelfont_ = 0;
			fl->labelsize_ = 12;
			fl->labelcolor_ = FL_BLACK;
			elements.push_back(fl);
			//then we analyse the sub-elements, the next element should be a vector
			for (int i = 1; i < element->Size(); i++) {
				if (element->getvalue(i)->isVectorContainer() == false)
					return globalAtanor->Returnerror("WND(109): Wrong element, vector expected", idthread);
				Atanor* v = element->getvalue(i);
				//First element should be the name
				if (v->Size() != 5)
					return globalAtanor->Returnerror("WND(109): Wrong element, missing field in a sub-menu", idthread);
				nn = v->getvalue(0)->String();
				n = strdup(STR(nn));
				//second element shoudl be a vector
				if (v->getvalue(1)->isVectorContainer() == false)
					return globalAtanor->Returnerror("WND(109): Wrong element, second field in a sub-menu should be a vector", idthread);
				long code = 0;
				Atanor* vv = v->getvalue(1);
				for (int j = 0; j < vv->Size(); j++) {
					Atanor* kcom = vv->getvalue(j);
					if (kcom->Type() == a_string) {
						string scom = kcom->String();
						if (commands.find(scom) != commands.end())
							code += commands[scom];
						else
							code += scom[0];
					}
					else
						code += kcom->Integer();
				}
				Atanor* function = v->getvalue(2);
				if (!function->isFunction())
					return globalAtanor->Returnerror("WND(109): Wrong element, third field in a sub-menu should be a function", idthread);
				Atanor* object = v->getvalue(3);
				if (!object->isCallVariable())
					object = object->Get(aNULL, aNULL, idthread);

				bool sep = v->getvalue(4)->Boolean();
				fl = new Fl_Menu_Item;
				fl->labeltype_ = 0;
				fl->labelfont_ = 0;
				fl->labelsize_ = 12;
				fl->labelcolor_ = FL_BLACK;
				fl->text = n;
				fl->shortcut_ = code;
				fl->callback_ = menu_callback;
				fl->user_data_ = new FLCallback(function, object, this);
				fl->flags = 0;
				if (sep == true)
					fl->flags = FL_MENU_DIVIDER;
				elements.push_back(fl);
			}
		}
		elements.push_back(NULL);
	}
	elements.push_back(NULL);
	flmenunbelements = elements.size();
	flmenu = new Fl_Menu_Item[flmenunbelements];
	int i;
	for (i = 0; i < flmenunbelements; i++) {
		if (elements[i] != NULL)
			flmenu[i] = *elements[i];
		else {
			flmenu[i].text = 0;
			flmenu[i].shortcut_ = 0;
			flmenu[i].callback_ = NULL;
			flmenu[i].user_data_ = 0;
			flmenu[i].flags = 0;
			flmenu[i].labeltype_ = 0;
			flmenu[i].labelfont_ = 0;
			flmenu[i].labelsize_ = 12;
			flmenu[i].labelcolor_ = FL_BLACK;
		}
		delete elements[i];
	}
	Atanor* x = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* wx = callfunc->Evaluate(3, contextualpattern, idthread);
	Atanor* hy = callfunc->Evaluate(4, contextualpattern, idthread);
	Fl_Menu_Bar *m = new Fl_Menu_Bar(x->Integer(), y->Integer(), wx->Integer(), hy->Integer());
	m->copy(flmenu);
	return aTRUE;
}

Atanor* Atanorwindow::MethodResizable(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (window == NULL)
		return globalAtanor->Returnerror("WND(303): No window available", idthread);
	if (end == false)
		return globalAtanor->Returnerror("WND(123): Operation not available", idthread);
	if (callfunc->Size() == 0) {
		window->resizable(NULL);
		return aTRUE;
	}
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	if (ke->Yourowntype() == Atanorwidget::idtype) {
		Fl_Widget* fl = ((Atanorwidget*)ke)->Widget();
		if (fl == NULL)
			return globalAtanor->Returnerror("WND(134): Widget has not been created yet", idthread);
		window->resizable(fl);
		return aTRUE;
	}
	return globalAtanor->Returnerror("WND(133): Widget expected as parameter", idthread);
}

Atanor* Atanorwindow::MethodSizerange(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (window == NULL)
		return globalAtanor->Returnerror("WND(303): No window available", idthread);
	if (end)
		return globalAtanor->Returnerror("WND(102): Window already instantiated", idthread);
	Atanor* minw = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* minh = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* maxw = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* maxh = callfunc->Evaluate(3, contextualpattern, idthread);
	window->size_range(minw->Integer(), minh->Integer(), maxw->Integer(), maxh->Integer());
	return aTRUE;
}

Atanor* Atanorwindow::MethodScrollbar(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (window == NULL)
		return globalAtanor->Returnerror("WND(303): No window available", idthread);
	if (end)
		return globalAtanor->Returnerror("WND(102): Window already instantiated", idthread);
	if (scrollcanvas != NULL)
		return globalAtanor->Returnerror("WND(108): Scrollbar already instantiated", idthread);

	Atanor* minw = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* minh = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* maxw = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* maxh = callfunc->Evaluate(3, contextualpattern, idthread);
	Atanor* maxcw = callfunc->Evaluate(4, contextualpattern, idthread);
	Atanor* maxch = callfunc->Evaluate(5, contextualpattern, idthread);
	scroll = new Fl_Scroll(minw->Integer(), minh->Integer(), maxw->Integer(), maxh->Integer());
	scrollcanvas = new AtanorCanvas(maxw->Integer(), maxh->Integer(), minw->Integer(), minh->Integer(), maxcw->Integer(), maxch->Integer());
	scrollcanvas->window = this;
	scrollcanvas->function = function;
	scrollcanvas->object = object;
	function = NULL;
	scroll->end();	
	return aTRUE;
}

Atanor* Atanorwindow::MethodModal(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (window == NULL)
		return globalAtanor->Returnerror("WND(303): No window available", idthread);
	if (callfunc->Size() == 0) {
		if (window->modal() == 0)
			return aFALSE;
		return aTRUE;
	}
	bool m = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
	if (m)
		window->set_modal();
	else
		window->set_non_modal();
	return aTRUE;
}

Atanor* Atanorwindow::MethodBorder(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (window == NULL)
		return globalAtanor->Returnerror("WND(303): No window available", idthread);
	if (end)
		return globalAtanor->Returnerror("WND(102): Window already instantiated", idthread);
	if (callfunc->Size() == 0) {
		if (window->border() == 0)
			return aFALSE;
		return aTRUE;
	}
	bool m = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
	window->border(m);
	return aTRUE;
}

Atanor* Atanorwindow::MethodBackgroundColor(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	Fl_Widget* window = Widget();
	if (window == NULL)
		return globalAtanor->Returnerror("WND(805): Widget not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalAtanor->Provideint(window->color());
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	Fl_Color color;
	if (ke->Type() == a_string) {
		//0 is the first parameter and so on...
		string buf = ke->String();
		if (colors.find(buf) == colors.end()) {
			buf = "WND(657): Unknown color code";
			return globalAtanor->Returnerror(buf, idthread);
		}
		color = colors[buf];
	}
	else
		color = ke->Integer();
	window->color(color);
	return aTRUE;
}

Atanor* Atanorwindow::MethodPlot(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (window == NULL)
		return globalAtanor->Returnerror("WND(303): No window available", idthread);
	Atanor* points = callfunc->Evaluate(0, contextualpattern, idthread);
	if (!points->isVectorContainer())
		return globalAtanor->Returnerror("WND(872): We expect a vector as first parameter.", idthread);
	int thickness = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
	double x, y, a, b;
	double maxX, maxY, minX, minY;
	double minx = 0;
	double miny = 0;
	double maxx = window->w();
	double maxy = window->h();
	double incx;
	double incy;
	char action = 0;
	Atanor* klandmark = NULL;
	if (callfunc->Size() == 3) {
		klandmark = callfunc->Evaluate(2, contextualpattern, idthread);
		if (!klandmark->isVectorContainer())
			return globalAtanor->Returnerror("WND(873): We expect a vector as third parameter", idthread);
		if (klandmark->Size() >= 4) {
			minx = klandmark->getfloat(0);
			miny = klandmark->getfloat(1);
			maxx = klandmark->getfloat(2);
			maxy = klandmark->getfloat(3);
			if (klandmark->Size() >= 8) {
				action = 1;
				minX = klandmark->getfloat(4);
				minY = klandmark->getfloat(5);
				maxX = klandmark->getfloat(6);
				maxY = klandmark->getfloat(7);
				if (klandmark->Size() >= 10) {
					action = 2;
					incx = klandmark->getfloat(8);
					incy = klandmark->getfloat(9);
				}
			}
		}
	}

	Atanor* table = points;
	long sz = points->Size();
	Atanor* kvect = Selectafvector(contextualpattern);

	long i;
	maxX = 0; minX = 0; maxY = 0; minY = 0;

	if (points->Type() == a_vector && points->getvalue(0)->isVectorContainer()) {
		Atanorfvector* fv = new Atanorfvector;
		Atanor* a;
		for (i = 0; i < sz; i++) {
			a = points->getvalue(i);
			if (!a->isVectorContainer() || a->Size() != 2) {
				fv->Release();
				return globalAtanor->Returnerror("WND(871): The vector should contain vectors of two elements.", idthread);
			}

			x = a->getfloat(0);
			y = a->getfloat(1);

			fv->values.push_back(x);
			fv->values.push_back(y);
			if (!action) {
				if (!i) {
					maxX = x;
					minX = x;
					minY = y;
					maxY = y;
				}
				else {
					maxX = max(x, maxX);
					minX =  min(x, minX);
					minY =  min(y, minY);
					maxY = max(y, maxY);
				}
			}
		}		
		
		if (maxX == minX || maxY == minY) {
			fv->Release();
			return kvect;
		}

		table = fv;
		sz = fv->values.size();
	}
	else {
		if (sz % 2 != 0)
			return globalAtanor->Returnerror("WND(871): The vector should contain an even number of elements.", idthread);

		if (!action) {
			for (i = 0; i < sz; i += 2) {
				x = table->getfloat(i);
				y = table->getfloat(i + 1);
				if (!i) {
					maxX = x;
					minX = maxX;
					minY = x;
					maxY = minY;
				}
				else {
					minX =  min(minX, x);
					maxX = max(maxX, x);
					minY =  min(minY, y);
					maxY = max(maxY, y);
				}
			}
			if (maxX == minX || maxY == minY)
				return kvect;
		}
	}

	kvect->storevalue(minx); //0
	kvect->storevalue(miny); //1
	kvect->storevalue(maxx); //2
	kvect->storevalue(maxy); //3
	if (thickness > 1) {
		minx += thickness;
		miny += thickness >> 1;
		maxx -= thickness;
		maxy -= (thickness << 1) - (thickness >> 1);
	}
	if (action != 2) {
		incx = (maxx - minx) / (maxX - minX);
		incy = (maxy - miny) / (maxY - minY);
	}

	for (i = 0; i < sz; i += 2) {
		x = table->getfloat(i);
		y = table->getfloat(i + 1);

		x = minx + incx*x - incx*minX;
		y = miny + maxy - incy*y + incy*minY;
		if (!thickness) {
			if (i)
				fl_line((int)a, (int)b, (int)x, (int)y);
			a = x;
			b = y;			
		}
		else {
			if (thickness == 1)
				fl_point((int)x, (int)y);
			else
				fl_circle((int)x, (int)y, thickness);
		}
	}

	kvect->storevalue(minX); //4
	kvect->storevalue(minY); //5
	kvect->storevalue(maxX); //6
	kvect->storevalue(maxY); //7
	kvect->storevalue(incx); //8
	kvect->storevalue(incy); //9
	kvect->storevalue((double)thickness); //10

	if (table != points)
		table->Release();

	return kvect;
}

Atanor* Atanorwindow::PlotCoords(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* kvect = callfunc->Evaluate(0, contextualpattern, idthread);
	if (kvect->Size() != 11)
		return globalAtanor->Returnerror("WND(862): Wrong values to compute coordinates", idthread);
	double x = callfunc->Evaluate(1, contextualpattern, idthread)->Float();
	double y = callfunc->Evaluate(2, contextualpattern, idthread)->Float();
	double minx = kvect->getfloat(0);
	double miny = kvect->getfloat(1);
	double maxy = kvect->getfloat(3);
	double minX = kvect->getfloat(4);
	double minY = kvect->getfloat(5);
	double incx = kvect->getfloat(8);
	double incy = kvect->getfloat(9);
	int thickness = kvect->getfloat(10);
	if (thickness > 1) {
		minx += thickness;
		miny += thickness >> 1;
		maxy -= (thickness << 1) - (thickness >> 1);
	}
	x = minx + incx*x - incx*minX;
	y = miny + maxy - incy*y + incy*minY;
	Atanor* kres = Selectafvector(contextualpattern);
	kres->storevalue(x);
	kres->storevalue(y);
	return kres;
}

Atanor* Atanorwindow::MethodAsk(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	int a;
	string msg = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	string msg1 = callfunc->Evaluate(1, contextualpattern, idthread)->String();
	string msg2 = callfunc->Evaluate(2, contextualpattern, idthread)->String();
	if (callfunc->Size() == 3)
		a = fl_choice(msg.c_str(), msg1.c_str(), msg2.c_str(), NULL);
	else {
		string msg3 = callfunc->Evaluate(3, contextualpattern, idthread)->String();
		if (callfunc->Size() == 4)
			a = fl_choice(msg.c_str(), msg1.c_str(), msg2.c_str(), msg3.c_str());
		else {
			string msg4 = callfunc->Evaluate(4, contextualpattern, idthread)->String();
			a = fl_choice(msg.c_str(), msg1.c_str(), msg2.c_str(), msg3.c_str(), msg4.c_str());
		}
	}
	return globalAtanor->Provideint(a);
}

Atanor* Atanorwindow::MethodAlert(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	string msg = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	fl_alert(msg.c_str());
	return aTRUE;
}

Atanor* Atanorwindow::MethodPassword(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	string msg = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	const char* pass = fl_password(msg.c_str());
	if (pass == NULL)
		return aNULL;
	return globalAtanor->Providestring(pass);
}

Atanor* Atanorwindow::MethodInput(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	string msg;
	if (callfunc->Size() == 1)
		msg = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	else
		msg = "get";
	const char* ret = fl_input(msg.c_str());
	if (ret == NULL)
		return aNULL;
	return globalAtanor->Providestring(ret);
}

Atanor* Atanorwindow::MethodOnClose(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (window == NULL)
		return globalAtanor->Returnerror("WND(303): No window available", idthread);
	Atanor* function = callfunc->Evaluate(0, contextualpattern, idthread);
	if (!function->isFunction())
		return globalAtanor->Returnerror("WND(109): Wrong element, the first parameter should be a function", idthread);
	Atanor* object = callfunc->arguments[1];
	if (!object->isCallVariable())
		object = object->Get(aNULL, aNULL, idthread);

	onclose = new FLCallback(function, object, this);
	window->callback(window_callback, onclose);
	return aTRUE;
}

Atanor* Atanorwindow::MethodOntime(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (window == NULL)
		return globalAtanor->Returnerror("WND(303): No window available", idthread);
	Atanor* function = callfunc->Evaluate(0, contextualpattern, idthread);
	if (!function->isFunction())
		return globalAtanor->Returnerror("WND(109): Wrong element, the first parameter should be a function", idthread);
	Atanor* object = callfunc->arguments[2];
	if (!object->isCallVariable())
		object = object->Get(aNULL, aNULL, idthread);

	if (timeout != NULL)
		timeout->Set(function, object, this);
	else
		timeout = new FLCallback(function, object, this);

	timeout->timevalue = callfunc->Evaluate(1, contextualpattern, idthread)->Float();

	return aTRUE;
}

Atanor* Atanorwindow::MethodRedrawing(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (window == NULL)
		return globalAtanor->Returnerror("WND(303): No window available", idthread);

	if (timeout != NULL)
		timeout->Set(aNULL, aNULL, this);
	else
		timeout = new FLCallback(aNULL, aNULL, this);

	timeout->timevalue = callfunc->Evaluate(0, contextualpattern, idthread)->Float();

	return aTRUE;
}

Atanor* Atanorwindow::MethodSetFonts(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	fontmax = Fl::set_fonts("*");
	return globalAtanor->Provideint(fontmax);
}

Atanor* Atanorwindow::MethodGetFont(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	int i = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	if (i >= fontmax)
		return globalAtanor->Returnerror("WND(431): Unavailable font id", idthread);
	return globalAtanor->Providestring(Fl::get_font_name(i));
}

Atanor* Atanorwindow::MethodGetFontSizes(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	int i = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	if (i >= fontmax)
		return globalAtanor->Returnerror("WND(431): Unavailable font id", idthread);
	int* sizes;
	int nb = Fl::get_font_sizes(i, sizes);
	Atanor* kvect = Selectaivector(contextualpattern);
	kvect->Clear();
	if (nb == 0)
		return kvect;
	for (int i = 0; i < nb; i++)
		kvect->storevalue((long)sizes[i]);
	return kvect;
}

Atanor* Atanorwindow::MethodNbFonts(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	return globalAtanor->Provideint(fontmax);
}




