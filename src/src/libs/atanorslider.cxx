
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
filename   : atanorslider.cxx
Date       : 2017/04/01
Purpose    : slider widget (based on FLTK)
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorslider.h"
#include "atanorint.h"
#include "instructions.h"

//We need to declare once again our local definitions.
hmap<short, sliderMethod>  Atanorslider::methods;
hmap<string, string> Atanorslider::infomethods;
bin_hash<unsigned long> Atanorslider::exported;

short Atanorslider::idtype = 0;

static void slider_callback(Fl_Widget*, void* data) {
	Atanorslider* slid = (Atanorslider*)data;
	Atanor* func = slid->function;
	AtanorCallFunction kfunc(func);
	slid->Setreference();
	slid->object->Setreference();

	kfunc.arguments.push_back(slid);
	kfunc.arguments.push_back(slid->object);
	kfunc.Get(aNULL, aNULL, globalAtanor->GetThreadid());
	slid->Resetreference();
	slid->object->Resetreference();

}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorslider::AddMethod(AtanorGlobal* global, string name, sliderMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorslider::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorslider::idtype = global->Getid("slider");

	Atanorslider::AddMethod(global, "_initial", &Atanorwidget::MethodInitial, P_ONE, "_initial(variable): Creation of a window with a call back object");
	Atanorslider::AddMethod(global, "create", &Atanorslider::MethodCreate, P_SEVEN, "create(int x, int y, int w, int h, int alignment, bool valueslider, string label): Create a slider or a valueslider");
	Atanorslider::AddMethod(global, "resize", &Atanorslider::MethodResize, P_FOUR, "resize(object): make the object resizable");
	Atanorslider::AddMethod(global, "bounds", &Atanorslider::MethodBoundary, P_TWO, "bounds(int x, int y): defines the slider boundary");
	Atanorslider::AddMethod(global, "type", &Atanorslider::MethodType, P_ONE, "type(int x): Value slider type");
	Atanorslider::AddMethod(global, "boxtype", &Atanorslider::MethodBoxType, P_ONE, "boxtype(int x): Define the slider box type");
	Atanorslider::AddMethod(global, "align", &Atanorwidget::MethodAlign, P_ONE, "align(int): define the slider alignement");
	Atanorslider::AddMethod(global, "step", &Atanorslider::MethodStep, P_ONE, "step(int): define the slider step");
	Atanorslider::AddMethod(global, "value", &Atanorslider::MethodValue, P_ONE | P_NONE, "value(int): define the value for the slider or return its value");
	Atanorslider::AddMethod(global, "tooltip", &Atanorwidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
	Atanorslider::AddMethod(global, "show", &Atanorwidget::MethodShow, P_NONE, "show(): Show the widget");
	Atanorslider::AddMethod(global, "focus", &Atanorwidget::MethodFocus, P_NONE, "focus(): Get the focus");
	Atanorslider::AddMethod(global, "hide", &Atanorwidget::MethodHide, P_NONE, "hide(): Hide the widget");
	Atanorslider::AddMethod(global, "labelsize", &Atanorwidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
	Atanorslider::AddMethod(global, "labelfont", &Atanorwidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
	Atanorslider::AddMethod(global, "labelcolor", &Atanorwidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
	Atanorslider::AddMethod(global, "labeltype", &Atanorwidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
	Atanorslider::AddMethod(global, "label", &Atanorwidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
	Atanorslider::AddMethod(global, "selectioncolor", &Atanorwidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
	Atanorslider::AddMethod(global, "boundaries", &Atanorwidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "boundaries(ivector crds): return a vector of the widget coordinates");
	Atanorslider::AddMethod(global, "redraw", &Atanorwidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
	Atanorslider::AddMethod(global, "backgroundcolor", &Atanorwidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
	Atanorslider::AddMethod(global, "created", &Atanorwidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");



	global->newInstance[Atanorslider::idtype] = new Atanorslider(global);
	global->RecordMethods(Atanorslider::idtype, Atanorslider::exported);

	return true;
}


Atanor* Atanorslider::Put(Atanor* index, Atanor* value, short idthread) {
	return this;
}

Atanor* Atanorslider::Get(Atanor* context, Atanor* index, short idthread) {
	return this;
}

AtanorIteration* Atanorslider::Newiteration(bool direction) {
	return aITERNULL;
}


Atanor* Atanorslider::MethodCreate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	if (slider != NULL)
		return globalAtanor->Returnerror("WND(100): Cannot recreate an existing slider", idthread);
	//0 is the first parameter and so on...
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* wx = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* hy = callfunc->Evaluate(3, contextualpattern, idthread);
	Atanor* align = callfunc->Evaluate(4, contextualpattern, idthread);
	bool valueslider = callfunc->Evaluate(5, contextualpattern, idthread)->Boolean();
	string label = callfunc->Evaluate(6, contextualpattern, idthread)->String();
	strcpy_s(txt, 1024, label.c_str());
	if (valueslider)
		slider = new Fl_Value_Slider(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt);
	else
		slider = new Fl_Slider(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt);
	slider->align(align->Integer());
	slider->type(1);
	slider->step(1);
	
	if (function != NULL)
		slider->callback(slider_callback, this);
	SaveItem(this);
	return aTRUE;
}

Atanor* Atanorslider::MethodResize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (slider == NULL)
		return globalAtanor->Returnerror("WND(101): No slider available", idthread);
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* wx = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* hy = callfunc->Evaluate(3, contextualpattern, idthread);
	slider->resize(x->Integer(), y->Integer(), wx->Integer(), hy->Integer());
	return aTRUE;
}

Atanor* Atanorslider::MethodBoundary(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (slider == NULL)
		return globalAtanor->Returnerror("WND(101): No slider available", idthread);
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	slider->bounds(x->Integer(), y->Integer());
	return aTRUE;
}

Atanor* Atanorslider::MethodType(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (slider == NULL)
		return globalAtanor->Returnerror("WND(101): No slider available", idthread);
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	if (x->Type() == a_string) {
		string buf = x->String();
		if (slidertype.find(buf) == slidertype.end())
			return globalAtanor->Returnerror("WND(206): Wrong slider type", idthread);
		slider->type(slidertype[buf]);
	}
	else
		slider->type(x->Integer());
	return aTRUE;
}

Atanor* Atanorslider::MethodBoxType(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (slider == NULL)
		return globalAtanor->Returnerror("WND(101): No slider available", idthread);
	Atanor* pos = callfunc->Evaluate(0, contextualpattern, idthread);
	if (pos->Type() == a_string) {
		if (boxTypes.find(pos->String()) == boxTypes.end()) {
			string mes = "WND(767): Unknown type:";
			mes += pos->String();
			return globalAtanor->Returnerror(mes, idthread);
		}
		else
			slider->box((Fl_Boxtype)boxTypes[pos->String()]);
	}
	else
		slider->box((Fl_Boxtype)pos->Integer());
	return aTRUE;
}

Atanor* Atanorslider::MethodStep(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (slider == NULL)
		return globalAtanor->Returnerror("WND(101): No slider available", idthread);
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	int i = x->Integer();
	slider->step(i);
	return aTRUE;
}

Atanor* Atanorslider::MethodValue(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (slider == NULL)
		return globalAtanor->Returnerror("WND(101): No slider available", idthread);
	if (callfunc->Size() == 0)
		return globalAtanor->Provideint(slider->value());
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	slider->value(x->Integer());
	return aTRUE;
}





