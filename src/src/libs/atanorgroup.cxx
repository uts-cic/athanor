
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
filename   : atanorgroup.cxx
Date       : 2017/04/01
Purpose    : Widget grouping (based on FLTK)
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorgroup.h"
#include "instructions.h"

Atanor_Group::Atanor_Group(Atanorgroup* kg, int x, int y, int w, int h, const char* txt) : associate(kg), Fl_Group(x, y, w, h, txt) { 
	iassociate = globalAtanor->RecordInTrackerProtected(kg);
}

Atanor_Group::~Atanor_Group() {
	if (associate != NULL) {
		if (associate->end == false)
			end();
		associate->itemclose();
		associate->group = NULL;
	}
}

void Atanor_Group::draw() {
	if (globalAtanor->Checktracker(associate,iassociate)) {
		if (associate->update)
			Fl_Group::draw();
	}

	if (Stopall())
		return;


	if (associate->function != NULL) {
#ifndef WIN32
		Locking _lock(globalAtanor->_printlock);
#endif
		Atanor* func = associate->function;
		AtanorCallFunction kfunc(func);

		associate->Setreference();
		associate->object->Setreference();
		kfunc.arguments.push_back(associate);
		kfunc.arguments.push_back(associate->object);
		kfunc.Get(aNULL, aNULL, globalAtanor->GetThreadid());
		associate->Resetreference();
		associate->object->Resetreference();		
	}
}

//We need to declare once again our local definitions.
hmap<short, groupMethod>  Atanorgroup::methods;
hmap<string, string> Atanorgroup::infomethods;
bin_hash<unsigned long> Atanorgroup::exported;

short Atanorgroup::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorgroup::AddMethod(AtanorGlobal* global, string name, groupMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorgroup::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorgroup::idtype = global->Getid("group");

	Atanorgroup::AddMethod(global, "begin", &Atanorgroup::MethodBegin, P_FIVE, "begin(int x, int y, int w , int h, string title): Create a group within a tab and begin initialisation");
	Atanorgroup::AddMethod(global, "end", &Atanorgroup::MethodEnd, P_NONE, "end(): end group creation");
	Atanorgroup::AddMethod(global, "selectioncolor", &Atanorwidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
	Atanorgroup::AddMethod(global, "tooltip", &Atanorwidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
	Atanorgroup::AddMethod(global, "activate", &Atanorgroup::MethodActivate, P_NONE, "activate(): Active this tab");
	Atanorgroup::AddMethod(global, "show", &Atanorwidget::MethodShow, P_NONE, "show(): Show the widget");
	Atanorgroup::AddMethod(global, "focus", &Atanorwidget::MethodFocus, P_NONE, "focus(): Get the focus");
	Atanorgroup::AddMethod(global, "hide", &Atanorwidget::MethodHide, P_NONE, "hide(): Hide the widget");
	Atanorgroup::AddMethod(global, "labelsize", &Atanorwidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
	Atanorgroup::AddMethod(global, "labelfont", &Atanorwidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
	Atanorgroup::AddMethod(global, "labelcolor", &Atanorwidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
	Atanorgroup::AddMethod(global, "labeltype", &Atanorwidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
	Atanorgroup::AddMethod(global, "label", &Atanorwidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
	Atanorgroup::AddMethod(global, "boundaries", &Atanorwidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "boundaries(ivector crds): return a vector of the widget coordinates");
	Atanorgroup::AddMethod(global, "drawcolor", &Atanorgroup::MethodDrawColor, P_ONE, "drawcolor(int c|string c): set the color for the next drawings");
	Atanorgroup::AddMethod(global, "rectangle", &Atanorgroup::MethodRectangle, P_FOUR | P_FIVE, "rectangle(int x, int y, int w , int h , string c|int c): Draw a rectangle with optional color c");
	Atanorgroup::AddMethod(global, "rectanglefill", &Atanorgroup::MethodRectangleFill, P_FOUR | P_FIVE, "rectanglefill(int x, int y, int w , int h , string c|int c): Fill a rectangle with optional color c");
	Atanorgroup::AddMethod(global, "lineshape", &Atanorgroup::MethodLineShape, P_TWO, "lineshape(string type, int width): Select the line shape and its thikness");
	Atanorgroup::AddMethod(global, "polygon", &Atanorgroup::MethodPolygon, P_SIX | P_EIGHT, "polygon(int x, int y, int x1 , int y1, int x2 , int y2 , int x3 , int y3): Draw a polygon x3 and y3 are optional");
	Atanorgroup::AddMethod(global, "line", &Atanorgroup::MethodLine, P_FOUR | P_SIX, "line(int x, int y, int x1 , int y1, int x2 , int y2): Draw a line between points x2 and y2 are optional");
	Atanorgroup::AddMethod(global, "rotation", &Atanorgroup::MethodLineRotation, P_FOUR | P_FIVE, "rotation(float x, float y, float distance , float angle , bool draw): Compute the coordinate of a rotated point from point xy using a distance and an angle. Return a vector of floats with the new coordinates.");
	Atanorgroup::AddMethod(global, "pushmatrix", &Atanorgroup::MethodPushMatrix, P_NONE, "pushmatrix(): Save the current transformation");
	Atanorgroup::AddMethod(global, "popmatrix", &Atanorgroup::MethodPopMatrix, P_NONE, "popmatrix(): Restore the current transformation");
	Atanorgroup::AddMethod(global, "scale", &Atanorgroup::MethodScale, P_TWO | P_ONE, "scale(float x, float y)|scale(float x): Scale the current transformation");
	Atanorgroup::AddMethod(global, "translate", &Atanorgroup::MethodTranslate, P_TWO, "translate(float x, float y): translate the current transformation");
	Atanorgroup::AddMethod(global, "rotate", &Atanorgroup::MethodRotate, P_ONE, "rotate(float d): rotate of degree d the current transformation");
	Atanorgroup::AddMethod(global, "multmatrix", &Atanorgroup::MethodMultmatrix, P_SIX, "multmatrix(float a , float b , float c , float d , float x , float y): combine transformations");
	Atanorgroup::AddMethod(global, "transformx", &Atanorgroup::MethodTransform_x, P_TWO, "transformx(float x , float y): Transform a coordinate X using the current transformation matrix.");
	Atanorgroup::AddMethod(global, "transformy", &Atanorgroup::MethodTransform_y, P_TWO, "transformy(float x , float y): Transform a coordinate Y using the current transformation matrix.");
	Atanorgroup::AddMethod(global, "transformdx", &Atanorgroup::MethodTransform_dx, P_TWO, "transformdx(float x , float y): Transform a distance DX using the current transformation matrix.");
	Atanorgroup::AddMethod(global, "transformdy", &Atanorgroup::MethodTransform_dy, P_TWO, "transformdy(float x , float y): Transform a distance DY using the current transformation matrix.");
	Atanorgroup::AddMethod(global, "transformedvertex", &Atanorgroup::MethodTransform_x, P_TWO, "transformedvertex(float x , float y): add transformations to vertices list.");
	Atanorgroup::AddMethod(global, "beginpoints", &Atanorgroup::MethodBeginPoints, P_NONE, "beginpoints(): Start accumulating vertices");
	Atanorgroup::AddMethod(global, "endpoints", &Atanorgroup::MethodEndPoints, P_NONE, "endpoints(): Stop accumulating vertices");
	Atanorgroup::AddMethod(global, "beginline", &Atanorgroup::MethodBeginLine, P_NONE, "beginline(): Start drawing lines.");
	Atanorgroup::AddMethod(global, "endline", &Atanorgroup::MethodEndLine, P_NONE, "endline(): End drawing lines.");
	Atanorgroup::AddMethod(global, "beginloop", &Atanorgroup::MethodBeginLoop, P_NONE, "beginloop(): Start drawing a closed sequence of lines.");
	Atanorgroup::AddMethod(global, "endloop", &Atanorgroup::MethodEndLoop, P_NONE, "endloop(): End drawing a closed sequence of lines.");
	Atanorgroup::AddMethod(global, "beginpolygon", &Atanorgroup::MethodBeginPolygon, P_NONE, "beginpolygon(): Start drawing a convex filled polygon.");
	Atanorgroup::AddMethod(global, "endpolygon", &Atanorgroup::MethodEndPolygon, P_NONE, "endpolygon(): End drawing a convex filled polygon.");
	Atanorgroup::AddMethod(global, "begincomplexpolygon", &Atanorgroup::MethodBeginComplexPolygon, P_NONE, "begincomplexpolygon(): Start drawing a complex filled polygon.");
	Atanorgroup::AddMethod(global, "gap", &Atanorgroup::MethodGap, P_NONE, "gap(): Fill the gap while drawing a complex polygon.");
	Atanorgroup::AddMethod(global, "endcomplexpolygon", &Atanorgroup::MethodEndComplexPolygon, P_NONE, "endcomplexpolygon(): End drawing a complex filled polygon.");
	Atanorgroup::AddMethod(global, "arc", &Atanorgroup::MethodArc, P_FIVE | P_SIX, "arc(int x, int y, int x1 , int y1 , float a1 , float a2): Draw an arc.\rarc(float xfloat yfloat radfloat a1float a2): Add a series of points to the current path on the arc of a circle;");
	Atanorgroup::AddMethod(global, "curve", &Atanorgroup::MethodCurve, P_EIGHT, "curve(float x, float y, float x1, float y1, float x1, float y2, float x3, float y3): Add a series of points on a Bezier curve to the path. The curve ends (and two of the points) are at xy and x3y3.");
	Atanorgroup::AddMethod(global, "pushclip", &Atanorgroup::MethodPushClip, P_FOUR, "pushclip(int x, int y, int w , int h): Insert a clip region with the following coordinates");
	Atanorgroup::AddMethod(global, "popclip", &Atanorgroup::MethodPopClip, P_NONE, "popclip(): Release a clip region");
	Atanorgroup::AddMethod(global, "pie", &Atanorgroup::MethodPie, P_SIX, "pie(int x, int y, int x1 , int y1 , float a1 , float a2): Draw a pie");
	Atanorgroup::AddMethod(global, "point", &Atanorgroup::MethodPoint, P_FOUR, "po, int(int x, int y): Draw a pixel");
	Atanorgroup::AddMethod(global, "circle", &Atanorgroup::MethodCircle, P_THREE | P_FOUR, "circle(int x, int y, int r, int color): Draw a circle. 'color' is optional.");
	Atanorgroup::AddMethod(global, "textsize", &Atanorgroup::MethodTextMeasure, P_ONE, "textsize(string l): Return a map with w and h as key to denote width and height of the string in pixels");
	Atanorgroup::AddMethod(global, "drawtext", &Atanorgroup::MethodDrawText, P_THREE, "drawtext(string l, int x, int y): Put a text at position xy");
	Atanorgroup::AddMethod(global, "redraw", &Atanorgroup::MethodRedraw, P_NONE, "redraw(): Redraw the window");
	Atanorgroup::AddMethod(global, "rgbcolor", &Atanorgroup::MethodRGBColor, P_ONE | P_THREE, "rgbcolor(string|int color)|(int r, int g, int b)|(vector color): either return a vector of the color decomposition or return the value of rgb values combined");
	Atanorgroup::AddMethod(global, "redraw", &Atanorwidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
	Atanorgroup::AddMethod(global, "backgroundcolor", &Atanorwidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
	Atanorgroup::AddMethod(global, "created", &Atanorwidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
	Atanorgroup::AddMethod(global, "align", &Atanorwidget::MethodAlign, P_ONE, "align(int): define the label alignment");




	global->newInstance[Atanorgroup::idtype] = new Atanorgroup(global);
	global->RecordMethods(Atanorgroup::idtype,Atanorgroup::exported);

	return true;
}


Atanor* Atanorgroup::Put(Atanor* index, Atanor* value, short idthread) {
	return this;
}

Atanor* Atanorgroup::Get(Atanor* context, Atanor* index, short idthread) {
	return this;
}

AtanorIteration* Atanorgroup:: Newiteration(bool direction) {
	return aITERNULL;
}

Atanor* Atanorgroup::MethodBegin(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {	
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	Atanor* x1 = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y1 = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* x2 = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* y2 = callfunc->Evaluate(3, contextualpattern, idthread);
	Atanor* text = callfunc->Evaluate(4, contextualpattern, idthread);
	if (group != NULL)
		return globalAtanor->Returnerror("WND(101): group already created", idthread);
	string thetxt = text->String();
	if (thetxt.size() < 1024)
		strcpy_s(txt, 1024, thetxt.c_str());
	{
		Locking _lock(this);
		group = new Atanor_Group(this, x1->Integer(), y1->Integer(), x2->Integer(), y2->Integer(), txt);
	}
	
	SaveItem(this);	
	return aTRUE;
}

Atanor* Atanorgroup::MethodEnd(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	if (group == NULL)
		return globalAtanor->Returnerror("WND(101): group not created", idthread);
	group->end();
	end = true;	
	return aTRUE;
}

Atanor* Atanorgroup::MethodActivate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	if (group == NULL)
		return globalAtanor->Returnerror("WND(101): group not created", idthread);
	Fl_Widget* tabs = group->parent();
	if (tabs == NULL)
		return globalAtanor->Returnerror("WND(507): This group does not belong to a tab structure", idthread);
	((Fl_Tabs*)tabs)->value(group);
	return aTRUE;
}





