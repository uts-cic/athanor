
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
filename   : atanortabs.cxx
Date       : 2017/04/01
Purpose    : window tabs (based on FLTK)
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanortabs.h"
#include "atanorgroup.h"

//We need to declare once again our local definitions.
hmap<short, tabsMethod>  Atanortabs::methods;
hmap<string, string> Atanortabs::infomethods;
bin_hash<unsigned long> Atanortabs::exported;

short Atanortabs::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanortabs::AddMethod(AtanorGlobal* global, string name, tabsMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanortabs::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanortabs::idtype = global->Getid("tabs");

	Atanortabs::AddMethod(global, "begin", &Atanortabs::MethodBegin, P_FIVE, "begin(int x, int y, int w , int h, string title): Create a tab window and begin initialisation");
	Atanortabs::AddMethod(global, "end", &Atanortabs::MethodEnd, P_NONE, "end(): end tab creation");
	Atanortabs::AddMethod(global, "current", &Atanortabs::MethodCurrent, P_NONE | P_ONE, "current(): Return the current active tab");
	Atanortabs::AddMethod(global, "add", &Atanortabs::MethodAdd, P_ONE, "add(wgroup g): Add dynamically a new tab");
	Atanortabs::AddMethod(global, "remove", &Atanortabs::MethodRemove, P_ONE, "remove(wgroup g): Remove a tab");
	Atanortabs::AddMethod(global, "tooltip", &Atanorwidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
	Atanortabs::AddMethod(global, "selectioncolor", &Atanorwidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
	Atanortabs::AddMethod(global, "show", &Atanorwidget::MethodShow, P_NONE, "show(): Show the widget");
	Atanortabs::AddMethod(global, "focus", &Atanorwidget::MethodFocus, P_NONE, "focus(): Get the focus");
	Atanortabs::AddMethod(global, "hide", &Atanorwidget::MethodHide, P_NONE, "hide(): Hide the widget");
	Atanortabs::AddMethod(global, "labelsize", &Atanorwidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
	Atanortabs::AddMethod(global, "labelfont", &Atanorwidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
	Atanortabs::AddMethod(global, "labelcolor", &Atanorwidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
	Atanortabs::AddMethod(global, "labeltype", &Atanorwidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
	Atanortabs::AddMethod(global, "label", &Atanorwidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
	Atanortabs::AddMethod(global, "boundaries", &Atanorwidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "boundaries(ivector crds): return a vector of the widget coordinates");
	Atanortabs::AddMethod(global, "redraw", &Atanorwidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
	Atanortabs::AddMethod(global, "backgroundcolor", &Atanorwidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
	Atanortabs::AddMethod(global, "created", &Atanorwidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
	Atanortabs::AddMethod(global, "align", &Atanorwidget::MethodAlign, P_ONE, "align(int): define the label alignment");


	global->newInstance[Atanortabs::idtype] = new Atanortabs(global);
	global->RecordMethods(Atanortabs::idtype, Atanortabs::exported);

	return true;
}


Atanor* Atanortabs::Put(Atanor* index, Atanor* value, short idthread) {
	return this;
}

Atanor* Atanortabs::Get(Atanor* context, Atanor* index, short idthread) {
	return this;
}

AtanorIteration* Atanortabs::Newiteration(bool direction) {
	return aITERNULL;
}

Atanor* Atanortabs::MethodBegin(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	Atanor* x1 = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y1 = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* x2 = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* y2 = callfunc->Evaluate(3, contextualpattern, idthread);
	Atanor* text = callfunc->Evaluate(4, contextualpattern, idthread);
	if (tabs != NULL)
		return globalAtanor->Returnerror("WND(101): tabs already created", idthread);
	string thetxt = text->String();
	if (thetxt.size() < 1024)
		strcpy_s(txt, 1024, thetxt.c_str());
	tabs = new Fl_Tabs(x1->Integer(), y1->Integer(), x2->Integer(), y2->Integer(), txt);
	SaveItem(this);
	end = false;
	return aTRUE;
}

Atanor* Atanortabs::MethodEnd(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	if (tabs == NULL)
		return globalAtanor->Returnerror("WND(101): tabs not created", idthread);
	end = true;
	tabs->end();
	return aTRUE;
}

Atanor* Atanortabs::MethodCurrent(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (tabs == NULL)
		return globalAtanor->Returnerror("WND(101): tabs not created", idthread);
	if (callfunc->Size() == 0) {
		Atanor_Group* flgroup = (Atanor_Group*)tabs->value();
		if (flgroup == NULL)
			return globalAtanor->Returnerror("WND(800): No active tab", idthread);
		return flgroup->associate;
	}
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	if (ke->Type() != Atanorgroup::idtype)
		return globalAtanor->Returnerror("WND(105): You can only select wgroup object", idthread);
	Atanorgroup* kgroup = (Atanorgroup*)ke;
	if (kgroup->group == NULL)
		return globalAtanor->Returnerror("WND(506): Empty group", idthread);
	if (kgroup->group->parent() != tabs)
		return globalAtanor->Returnerror("WND(507): This group does not belong to that tab", idthread);
	tabs->value(kgroup->group);
	return aTRUE;
}

Atanor* Atanortabs::MethodAdd(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	if (tabs == NULL)
		return globalAtanor->Returnerror("WND(101): tabs not created", idthread);
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	if (ke->Type() == Atanorgroup::idtype) {
		Atanorgroup* kgroup = (Atanorgroup*)ke;
		if (kgroup->group == NULL)
			return globalAtanor->Returnerror("WND(506): Empty group", idthread);
		tabs->add(kgroup->group);
		tabs->value(kgroup->group);
		tabs->hide();
		tabs->show();
		return aTRUE;
	}
	return globalAtanor->Returnerror("WND(105): You can only add wgroup object", idthread);
}

Atanor* Atanortabs::MethodRemove(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	return globalAtanor->Returnerror("WND(800): Wrong number of parameters", idthread);
	if (tabs == NULL)
		return globalAtanor->Returnerror("WND(101): tabs not created", idthread);
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	if (ke->Type() == Atanorgroup::idtype) {
		Atanorgroup* kgroup = (Atanorgroup*)ke;
		if (kgroup->group == NULL)
			return globalAtanor->Returnerror("WND(506): Empty group", idthread);
		if (kgroup->group->parent() != tabs)
			return globalAtanor->Returnerror("WND(507): This group does not belong to that tab", idthread);
		kgroup->group->hide();
		tabs->remove(kgroup->group);
		delete kgroup->group;
		tabs->hide();
		tabs->show();
		return aTRUE;
	}
	return globalAtanor->Returnerror("WND(505): You can only remove wgroup object", idthread);
}





