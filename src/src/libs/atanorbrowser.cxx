
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
filename   : atanorbrowser.cxx
Date       : 2017/04/01
Purpose    : generic browser (based on FLTK)
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorbrowser.h"
#include "instructions.h"

static void browser_callback(Fl_Widget*, void* data) {
	Atanorbrowser* cnt = (Atanorbrowser*)data;
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
hmap<short, browserMethod>  Atanorbrowser::methods;
hmap<string, string> Atanorbrowser::infomethods;
bin_hash<unsigned long> Atanorbrowser::exported;

short Atanorbrowser::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorbrowser::AddMethod(AtanorGlobal* global, string name, browserMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorbrowser::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorbrowser::idtype = global->Getid("browser");

	Atanorbrowser::AddMethod(global, "_initial", &Atanorwidget::MethodInitial, P_ONE, "_initial(variable): Creation of a window with a call back object");
	Atanorbrowser::AddMethod(global, "create", &Atanorbrowser::MethodCreate, P_FIVE, "create(xywhlabel): Create a browser");
	Atanorbrowser::AddMethod(global, "add", &Atanorbrowser::MethodAdd, P_ONE, "add(label): Add a string to the browser");
	Atanorbrowser::AddMethod(global, "load", &Atanorbrowser::MethodLoad, P_ONE, "load(filename): Load a file into the browser");
	Atanorbrowser::AddMethod(global, "insert", &Atanorbrowser::MethodInsert, P_TWO, "insert(llabel): Insert a label before line l");
	Atanorbrowser::AddMethod(global, "clear", &Atanorbrowser::MethodClear, P_NONE, "clear(): Clear the browser from all values");
	Atanorbrowser::AddMethod(global, "value", &Atanorbrowser::MethodValue, P_NONE, "value(): return the current selected value");
	Atanorbrowser::AddMethod(global, "select", &Atanorbrowser::MethodSelect, P_ONE | P_NONE, "select(): Return selected , string.\rselect(int i): Return string in position i");
	Atanorbrowser::AddMethod(global, "deselect", &Atanorbrowser::MethodDeselect, P_NONE | P_ONE, "deselect(): Deselect all items\rdeselect(int i): Deselect item i");
	Atanorbrowser::AddMethod(global, "columnchar", &Atanorbrowser::MethodColumnchar, P_ONE | P_NONE, "columnchar(): Return the column char separator.\rcolumnchar(string): Set the column char separator");
	Atanorbrowser::AddMethod(global, "formatchar", &Atanorbrowser::MethodFormatchar, P_ONE | P_NONE, "formatchar(): Return the format char.\rformatchar(string): Set the format char");
	Atanorbrowser::AddMethod(global, "show", &Atanorwidget::MethodShow, P_NONE, "show(): Show the widget");
	Atanorbrowser::AddMethod(global, "tooltip", &Atanorwidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
	Atanorbrowser::AddMethod(global, "show", &Atanorwidget::MethodShow, P_NONE, "show(): Show the widget");
	Atanorbrowser::AddMethod(global, "focus", &Atanorwidget::MethodFocus, P_NONE, "focus(): Get the focus");
	Atanorbrowser::AddMethod(global, "hide", &Atanorwidget::MethodHide, P_NONE, "hide(): Hide the widget");
	Atanorbrowser::AddMethod(global, "labelsize", &Atanorwidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
	Atanorbrowser::AddMethod(global, "labelfont", &Atanorwidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
	Atanorbrowser::AddMethod(global, "labelcolor", &Atanorwidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
	Atanorbrowser::AddMethod(global, "labeltype", &Atanorwidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
	Atanorbrowser::AddMethod(global, "label", &Atanorwidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
	Atanorbrowser::AddMethod(global, "boundaries", &Atanorwidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "boundaries(ivector crds): return a vector of the widget coordinates");
	Atanorbrowser::AddMethod(global, "redraw", &Atanorwidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
	Atanorbrowser::AddMethod(global, "backgroundcolor", &Atanorwidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
	Atanorbrowser::AddMethod(global, "created", &Atanorwidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
	Atanorbrowser::AddMethod(global, "copy", &Atanorwidget::MethodCopy, P_ONE, "copy(string): Copy string to clipboard");
	Atanorbrowser::AddMethod(global, "paste", &Atanorwidget::MethodPaste, P_NONE, "paste(): paste string");
	Atanorbrowser::AddMethod(global, "align", &Atanorwidget::MethodAlign, P_ONE, "align(int): define the label alignment");



	global->newInstance[Atanorbrowser::idtype] = new Atanorbrowser(global);
	global->RecordMethods(Atanorbrowser::idtype,Atanorbrowser::exported);

	return true;
}


Atanor* Atanorbrowser::Put(Atanor* index, Atanor* value, short idthread) {
	return this;
}

Atanor* Atanorbrowser::Get(Atanor* context, Atanor* index, short idthread) {
	return this;
}

AtanorIteration* Atanorbrowser:: Newiteration(bool direction) {
	return aITERNULL;
}


Atanor* Atanorbrowser::MethodCreate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	if (browser != NULL)
		return globalAtanor->Returnerror("WND(100): Cannot recreate an existing browser", idthread);
	//0 is the first parameter and so on...

	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* wx = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* hy = callfunc->Evaluate(3, contextualpattern, idthread);
	string label = callfunc->Evaluate(4, contextualpattern, idthread)->String();
	strcpy_s(txt, 1024, label.c_str());
	if (function != NULL) {
		browser = new Fl_Select_Browser(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt);
		browser->callback(browser_callback, this);
	}
	else
		browser = new Fl_Browser(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt);
	return aTRUE;
}

Atanor* Atanorbrowser::MethodAdd(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (browser == NULL)
		return globalAtanor->Returnerror("WND(121): Browser widget has not been created yet", idthread);
	string v = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	browser->add(STR(v));
	return aTRUE;
}

Atanor* Atanorbrowser::MethodLoad(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (browser == NULL)
		return globalAtanor->Returnerror("WND(121): Browser widget has not been created yet", idthread);
	string v = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	browser->load(STR(v));
	return aTRUE;
}

Atanor* Atanorbrowser::MethodInsert(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (browser == NULL)
		return globalAtanor->Returnerror("WND(121): Browser widget has not been created yet", idthread);
	int i = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	string v = callfunc->Evaluate(1, contextualpattern, idthread)->String();
	browser->insert(i, STR(v), 0);
	return aTRUE;
}

Atanor* Atanorbrowser::MethodClear(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (browser == NULL)
		return globalAtanor->Returnerror("WND(121): Browser widget has not been created yet", idthread);
	browser->clear();
	return aTRUE;
}

Atanor* Atanorbrowser::MethodValue(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (browser == NULL)
		return globalAtanor->Returnerror("WND(121): Browser widget has not been created yet", idthread);
	string v;
	int i = browser->value();
	return globalAtanor->Provideint(i);
}

Atanor* Atanorbrowser::MethodSelect(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (browser == NULL)
		return globalAtanor->Returnerror("WND(121): Browser widget has not been created yet", idthread);
	string v;
	int select;
	if (callfunc->Size() == 0)
		select = browser->value();
	else
		select = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	if (select >= 1 && select <= browser->size())
		v = browser->text(select);
	return globalAtanor->Providestring(v);
}

Atanor* Atanorbrowser::MethodDeselect(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (browser == NULL)
		return globalAtanor->Returnerror("WND(121): Browser widget has not been created yet", idthread);
	if (callfunc->Size() == 0)
		browser->deselect();
	else {
		int select = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
		browser->deselect(select);
	}
	return aTRUE;
}

Atanor* Atanorbrowser::MethodColumnchar(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (browser == NULL)
		return globalAtanor->Returnerror("WND(121): Browser widget has not been created yet", idthread);
	string select;
	if (callfunc->Size() == 0) {
		select = browser->column_char();
		return globalAtanor->Providestring(select);
	}
	select = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	if (select.size() != 0)
		browser->column_char(select[0]);
	return aTRUE;
}

Atanor* Atanorbrowser::MethodFormatchar(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (browser == NULL)
		return globalAtanor->Returnerror("WND(121): Browser widget has not been created yet", idthread);
	string select;
	if (callfunc->Size() == 0) {
		select = browser->format_char();
		return globalAtanor->Providestring(select);
	}
	select = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	if (select.size() != 0)
		browser->format_char(select[0]);
	return aTRUE;
}





