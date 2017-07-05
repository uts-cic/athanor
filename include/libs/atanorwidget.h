
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
filename   : atanorwidget.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanorwidget_h
#define atanorwidget_h

#include "atanorfltk.h"
//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorwidget;
//This typedef defines a type "widgetMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorwidget::*widgetMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanorwidget : public AtanorObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static hmap<short, widgetMethod> methods;
    static hmap<string, string> infomethods;
    static bin_hash<unsigned long> exported;

    static short idtype; //see Atanorwindow for its implementation...

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
	Atanor* function;
	Atanor* object;
	int calledthread;
	bool update;
	vector<char> begins;

    //---------------------------------------------------------------------------------------------------------------------
	Atanorwidget(AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
		calledthread = 0;
		update = true;
		function = NULL;
		object = aNULL;
		g->RecordInTrackerProtected(this);
	}

	void Addfunction(Atanor* a) {
		function = a;
	}

	virtual void itemclose() {}

	void clean() {
		Setprotect(true);
		Resetreference();
	}

	short Yourowntype() {
		return Atanorwidget::idtype;
	}

	//For widget, we do not need to store them in a variable...
	bool duplicateForCall() {
		return false;
	}

	//----------------------------------------------------------------------------------------------------------------------

	void RecordGUIObject(AtanorCall* callfunc, short idthread);

	virtual Fl_Widget* Widget() {
		return NULL;
	}

    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in kifexportedmethods... See MethodInitialization below
  
    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
    //Atanor* MethodSize(AtanorGlobal* global,Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {return aZERO;}
	Atanor* MethodInitial(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		//We are interested by the variable...
		object = callfunc->arguments[0];
		if (!object->isCallVariable())
			object = callfunc->Evaluate(0, aNULL, idthread);
		return aTRUE;
	}


	Atanor* MethodRedraw(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodTextFont(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodRGBColor(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodShow(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodFocus(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodAlign(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodCreated(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodCopy(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodPaste(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodCoordinates(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodSelectionColor(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodLabel(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodLabelType(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodLabelColor(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodLabelFont(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodLabelSize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodDrawText(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
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
	Atanor* MethodTooltip(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodHide(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodBackgroundColor(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
    //is returned by the Declaration method.

	virtual Atanor* same(Atanor* a) {
		if (a == this)
			return aTRUE;
		return aFALSE;
	}
};

#endif







