
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
filename   : atanorcounter.cxx
Date       : 2017/04/01
Purpose    : counter display (based on FLTK)
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorcounter.h"
#include "instructions.h"
#include "atanorfvector.h"

static void counter_callback(Fl_Widget*, void* data) {
	Atanorcounter* cnt = (Atanorcounter*)data;
	Atanor* func = cnt->function;
	AtanorCallFunction kfunc(func);

	cnt->Setreference();
	cnt->object->Setreference();
	kfunc.arguments.push_back(cnt);
	kfunc.arguments.push_back(cnt->object);
	kfunc.Get(aNULL, aNULL, globalAtanor->GetThreadid());
	cnt->Resetreference();
	cnt->object->Resetreference();
}


//We need to declare once again our local definitions.
hmap<short, counterMethod>  Atanorcounter::methods;
hmap<string, string> Atanorcounter::infomethods;
bin_hash<unsigned long> Atanorcounter::exported;

short Atanorcounter::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorcounter::AddMethod(AtanorGlobal* global, string name, counterMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorcounter::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorcounter::idtype = global->Getid("counter");

	Atanorcounter::AddMethod(global, "_initial", &Atanorwidget::MethodInitial, P_ONE, "_initial(variable): Creation of a window with a call back object");
	Atanorcounter::AddMethod(global, "create", &Atanorcounter::MethodCreate, P_FIVE, "create(int x, int y, int w, int h, string label): Create a wcounter");
	Atanorcounter::AddMethod(global, "bounds", &Atanorcounter::MethodBoundary, P_NONE | P_TWO, "bounds(float x, float y): defines the wcounter boundary");
	Atanorcounter::AddMethod(global, "type", &Atanorcounter::MethodType, P_ONE, "type(bool normal): if 'true' then normal wcounter or simple wcounter");
	Atanorcounter::AddMethod(global, "step", &Atanorcounter::MethodStep, P_ONE, "step(double): define the wcounter step");
	Atanorcounter::AddMethod(global, "lstep", &Atanorcounter::MethodLargeStep, P_ONE, "lstep(double): define the large wcounter step");
	Atanorcounter::AddMethod(global, "steps", &Atanorcounter::MethodSteps, P_TWO, "steps(double): define the wcounter steps normal and large.");
	Atanorcounter::AddMethod(global, "value", &Atanorcounter::MethodValue, P_ONE | P_NONE, "value(float): define the value for the wcounter or return its value");
	Atanorcounter::AddMethod(global, "tooltip", &Atanorwidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
	Atanorcounter::AddMethod(global, "show", &Atanorwidget::MethodShow, P_NONE, "show(): Show the widget");
	Atanorcounter::AddMethod(global, "focus", &Atanorwidget::MethodFocus, P_NONE, "focus(): Get the focus");
	Atanorcounter::AddMethod(global, "hide", &Atanorwidget::MethodHide, P_NONE, "hide(): Hide the widget");
	Atanorcounter::AddMethod(global, "labelsize", &Atanorwidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
	Atanorcounter::AddMethod(global, "labelfont", &Atanorwidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
	Atanorcounter::AddMethod(global, "labelcolor", &Atanorwidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
	Atanorcounter::AddMethod(global, "labeltype", &Atanorwidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
	Atanorcounter::AddMethod(global, "label", &Atanorwidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
	Atanorcounter::AddMethod(global, "selectioncolor", &Atanorwidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
	Atanorcounter::AddMethod(global, "boundaries", &Atanorwidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "boundaries(ivector crds): return a vector of the widget coordinates");
	Atanorcounter::AddMethod(global, "redraw", &Atanorwidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
	Atanorcounter::AddMethod(global, "backgroundcolor", &Atanorwidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
	Atanorcounter::AddMethod(global, "created", &Atanorwidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
	Atanorcounter::AddMethod(global, "textsize", &Atanorcounter::MethodFontSize, P_ONE | P_NONE, "textsize(string l): Return a map with w and h as key to denote width and height of the string in pixels");
	Atanorcounter::AddMethod(global, "font", &Atanorcounter::MethodFont, P_ONE | P_NONE, "font(int s): set or return the text font");
	Atanorcounter::AddMethod(global, "textcolor", &Atanorcounter::MethodTextColor, P_ONE | P_NONE, "textcolor(string code|int code): Set the color of the text");


	global->newInstance[Atanorcounter::idtype] = new Atanorcounter(global);
	global->RecordMethods(Atanorcounter::idtype,Atanorcounter::exported);

	return true;
}


Atanor* Atanorcounter::Put(Atanor* index, Atanor* value, short idthread) {
	return this;
}

Atanor* Atanorcounter::Get(Atanor* context, Atanor* index, short idthread) {
	return this;
}

AtanorIteration* Atanorcounter:: Newiteration(bool direction) {
	return aITERNULL;
}

Atanor* Atanorcounter::MethodCreate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	if (counter != NULL)
		return globalAtanor->Returnerror("WND(210): Cannot recreate an existing wcounter", idthread);
	
	//0 is the first parameter and so on...
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* wx = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* hy = callfunc->Evaluate(3, contextualpattern, idthread);
	string label = callfunc->Evaluate(4, contextualpattern, idthread)->String();
	strcpy_s(txt, 1024, label.c_str());
	counter = new Fl_Counter(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt);
	if (function != NULL) {
		if (function->Size() != 2)
			return globalAtanor->Returnerror("KIF(881): Error, wrong function arguments", idthread);
		counter->callback(counter_callback, this);
	}
	return aTRUE;
}

Atanor* Atanorcounter::MethodBoundary(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (counter == NULL)
		return globalAtanor->Returnerror("WND(211): No wcounter available", idthread);
	double a, b;
	if (callfunc->Size() == 0) {
		a = counter->minimum();
		b = counter->maximum();
		Atanorfvector* v = new Atanorfvector;
		v->values.push_back(a);
		v->values.push_back(b);
		return v;
	}
	a = callfunc->Evaluate(0, contextualpattern, idthread)->Float();
	b = callfunc->Evaluate(1, contextualpattern, idthread)->Float();
	counter->bounds(a, b);
	return aTRUE;
}

Atanor* Atanorcounter::MethodType(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (counter == NULL)
		return globalAtanor->Returnerror("WND(211): No wcounter available", idthread);
	bool nstep = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
	if (nstep)
		counter->type(FL_NORMAL_COUNTER);
	else
		counter->type(FL_SIMPLE_COUNTER);
	return aTRUE;
}

Atanor* Atanorcounter::MethodStep(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (counter == NULL)
		return globalAtanor->Returnerror("WND(211): No wcounter available", idthread);
	double kstep = callfunc->Evaluate(0, contextualpattern, idthread)->Float();
	counter->step(kstep);
	return aTRUE;
}

Atanor* Atanorcounter::MethodLargeStep(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (counter == NULL)
		return globalAtanor->Returnerror("WND(211): No wcounter available", idthread);
	double kstep = callfunc->Evaluate(0, contextualpattern, idthread)->Float();
	counter->lstep(kstep);
	return aTRUE;
}

Atanor* Atanorcounter::MethodSteps(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (counter == NULL)
		return globalAtanor->Returnerror("WND(211): No wcounter available", idthread);
	double nstep = callfunc->Evaluate(0, contextualpattern, idthread)->Float();
	double Lstep = callfunc->Evaluate(1, contextualpattern, idthread)->Float();
	counter->step(nstep, Lstep);
	return aTRUE;
}

Atanor* Atanorcounter::MethodValue(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (counter == NULL)
		return globalAtanor->Returnerror("WND(211): No wcounter available", idthread);
	double a;
	if (callfunc->Size() == 0) {
		a = counter->value();
		return globalAtanor->Providefloat(a);
	}
	a = callfunc->Evaluate(0, contextualpattern, idthread)->Float();
	counter->value(a);
	return aTRUE;
}

Atanor* Atanorcounter::MethodFontSize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (counter == NULL)
		return globalAtanor->Returnerror("WND(211): No wcounter available", idthread);
	if (callfunc->Size() == 0)
		return globalAtanor->Provideint(counter->textsize());
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	//0 is the first parameter and so on...
	counter->textsize(ke->Integer());
	return aTRUE;
}

Atanor* Atanorcounter::MethodFont(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (counter == NULL)
		return globalAtanor->Returnerror("WND(211): No wcounter available", idthread);
	if (callfunc->Size() == 0) {
		int i = counter->textfont();
		if (ifonts.find(i) != ifonts.end())
			return globalAtanor->Providestring(ifonts[i]);
		return globalAtanor->Provideint(i);
	}
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	//0 is the first parameter and so on...
	if (ke->Type() == a_string) {
		string buf = ke->String();
		if (fonts.find(buf) == fonts.end()) {
			buf = "WND(657): Unknown font code";
			return globalAtanor->Returnerror(buf, idthread);
		}
		counter->textfont(fonts[buf]);
	}
	else {
		int font = ke->Integer();
		if (font >= fontmax)
			return globalAtanor->Returnerror("WND(658): unknown font", idthread);
		counter->textfont(font);
	}
	return aTRUE;
}

Atanor* Atanorcounter::MethodTextColor(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (counter == NULL)
		return globalAtanor->Returnerror("WND(211): No wcounter available", idthread);
	if (callfunc->Size() == 0) {
		Fl_Color color = counter->textcolor();
		return globalAtanor->Provideint(color);
	}
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	if (ke->Type() == a_string) {
		//0 is the first parameter and so on...
		string buf = ke->String();
		if (colors.find(buf) == colors.end()) {
			buf = "WND(657): Unknown color code";
			return globalAtanor->Returnerror(buf, idthread);
		}
		counter->textcolor(colors[buf]);
	}
	else {
		unsigned int c = ke->Integer();
		counter->textcolor(c);
	}
	return aTRUE;
}





