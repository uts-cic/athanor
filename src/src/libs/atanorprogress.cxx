
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
filename   : atanorprogress.cxx
Date       : 2017/04/01
Purpose    : progress bar (based on FLTK)
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorprogress.h"
#include "instructions.h"

static void progress_callback(Fl_Widget*, void* data) {
	Atanorprogress* cnt = (Atanorprogress*)data;
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
hmap<short, progressMethod>  Atanorprogress::methods;
hmap<string, string> Atanorprogress::infomethods;
bin_hash<unsigned long> Atanorprogress::exported;

short Atanorprogress::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorprogress::AddMethod(AtanorGlobal* global, string name, progressMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorprogress::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorprogress::idtype = global->Getid("progress");

	Atanorprogress::AddMethod(global, "_initial", &Atanorwidget::MethodInitial, P_ONE, "_initial(variable): Creation of a window with a call back object");

	Atanorprogress::AddMethod(global, "create", &Atanorprogress::MethodCreate, P_FIVE, "create(int x, int y, int w, int h, string label): Create a progress bar");
	Atanorprogress::AddMethod(global, "minimum", &Atanorprogress::MethodMinimum, P_ONE | P_NONE, "minimum(float x): defines the minimum or return the minimum");
	Atanorprogress::AddMethod(global, "maximum", &Atanorprogress::MethodMaximum, P_ONE | P_NONE, "maximum(float x): defines the maximum or return the maximum");
	Atanorprogress::AddMethod(global, "value", &Atanorprogress::MethodValue, P_ONE | P_NONE, "value(float): define the value for the progress bar or return its value");
	Atanorprogress::AddMethod(global, "tooltip", &Atanorwidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
	Atanorprogress::AddMethod(global, "show", &Atanorwidget::MethodShow, P_NONE, "show(): Show the widget");
	Atanorprogress::AddMethod(global, "focus", &Atanorwidget::MethodFocus, P_NONE, "focus(): Get the focus");
	Atanorprogress::AddMethod(global, "hide", &Atanorwidget::MethodHide, P_NONE, "hide(): Hide the widget");
	Atanorprogress::AddMethod(global, "barcolor", &Atanorwidget::MethodSelectionColor, P_ONE | P_NONE, "barcolor(int color): Color for the progression bar");
	Atanorprogress::AddMethod(global, "boundaries", &Atanorwidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "boundaries(ivector crds): return a vector of the widget coordinates");
	Atanorprogress::AddMethod(global, "redraw", &Atanorwidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
	Atanorprogress::AddMethod(global, "backgroundcolor", &Atanorwidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
	Atanorprogress::AddMethod(global, "created", &Atanorwidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");



	global->newInstance[Atanorprogress::idtype] = new Atanorprogress(global);
	global->RecordMethods(Atanorprogress::idtype,Atanorprogress::exported);

	return true;
}


Atanor* Atanorprogress::Put(Atanor* index, Atanor* value, short idthread) {
	return this;
}

Atanor* Atanorprogress::Get(Atanor* context, Atanor* index, short idthread) {
	return this;
}

AtanorIteration* Atanorprogress:: Newiteration(bool direction) {
	return aITERNULL;
}

Atanor* Atanorprogress::MethodCreate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	if (progress != NULL)
		return globalAtanor->Returnerror("WND(220): Cannot recreate an existing wprogress bar", idthread);
	//0 is the first parameter and so on...
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* wx = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* hy = callfunc->Evaluate(3, contextualpattern, idthread);
	string label = callfunc->Evaluate(4, contextualpattern, idthread)->String();
	strcpy_s(txt, 1024, label.c_str());
	progress = new Fl_Progress(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt);
	
	if (function != NULL) {
		if (function->Size() != 2)
			return globalAtanor->Returnerror("KIF(881): Error, wrong function arguments", idthread);
		progress->callback(progress_callback, this);
	}
	return aTRUE;
}

Atanor* Atanorprogress::MethodMinimum(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (progress == NULL)
		return globalAtanor->Returnerror("WND(221): No wprogress bar available", idthread);
	double a;
	if (callfunc->Size() == 0) {
		a = progress->minimum();
		return globalAtanor->Providefloat(a);
	}
	a = callfunc->Evaluate(0, contextualpattern, idthread)->Float();
	progress->minimum(a);
	return aTRUE;
}

Atanor* Atanorprogress::MethodMaximum(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (progress == NULL)
		return globalAtanor->Returnerror("WND(221): No wprogress bar available", idthread);
	double a;
	if (callfunc->Size() == 0) {
		a = progress->maximum();
		return globalAtanor->Providefloat(a);
	}
	a = callfunc->Evaluate(0, contextualpattern, idthread)->Float();
	progress->maximum(a);
	return aTRUE;
}

Atanor* Atanorprogress::MethodValue(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (progress == NULL)
		return globalAtanor->Returnerror("WND(221): No wprogress bar available", idthread);
	double a;
	if (callfunc->Size() == 0) {
		a = progress->value();
		return globalAtanor->Providefloat(a);
	}
	a = callfunc->Evaluate(0, contextualpattern, idthread)->Float();
	progress->value(a);
	return aTRUE;
}





