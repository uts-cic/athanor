
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
filename   : atanorbox.cxx
Date       : 2017/04/01
Purpose    : boxes (based on FLTK)
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorbox.h"

//We need to declare once again our local definitions.
hmap<short, boxMethod>  Atanorbox::methods;
hmap<string, string> Atanorbox::infomethods;
bin_hash<unsigned long> Atanorbox::exported;

short Atanorbox::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorbox::AddMethod(AtanorGlobal* global, string name, boxMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorbox::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorbox::idtype = global->Getid("box");


	Atanorbox::AddMethod(global, "create", &Atanorbox::MethodCreate, P_FIVE, "create(int x, int y, int w, int h, string label): Create a box");
	Atanorbox::AddMethod(global, "type", &Atanorbox::MethodType, P_ONE, "type(string|int boxtype): Define the box type");
	Atanorbox::AddMethod(global, "tooltip", &Atanorwidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
	Atanorbox::AddMethod(global, "show", &Atanorwidget::MethodShow, P_NONE, "show(): Show the widget");
	Atanorbox::AddMethod(global, "hide", &Atanorwidget::MethodHide, P_NONE, "hide(): Hide the widget");
	Atanorbox::AddMethod(global, "labelsize", &Atanorwidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
	Atanorbox::AddMethod(global, "labelfont", &Atanorwidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
	Atanorbox::AddMethod(global, "labelcolor", &Atanorwidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
	Atanorbox::AddMethod(global, "labeltype", &Atanorwidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
	Atanorbox::AddMethod(global, "label", &Atanorwidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
	Atanorbox::AddMethod(global, "selectioncolor", &Atanorwidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
	Atanorbox::AddMethod(global, "boundaries", &Atanorwidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "boundaries(ivector crds): return a vector of the widget coordinates");
	Atanorbox::AddMethod(global, "redraw", &Atanorwidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
	Atanorbox::AddMethod(global, "backgroundcolor", &Atanorwidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
	Atanorbox::AddMethod(global, "created", &Atanorwidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
	Atanorbox::AddMethod(global, "align", &Atanorwidget::MethodAlign, P_ONE, "align(int): define the label alignment");


	global->newInstance[Atanorbox::idtype] = new Atanorbox(global);
	global->RecordMethods(Atanorbox::idtype,Atanorbox::exported);

	return true;
}


Atanor* Atanorbox::Put(Atanor* index, Atanor* value, short idthread) {
	return this;
}

Atanor* Atanorbox::Get(Atanor* context, Atanor* index, short idthread) {
	return this;
}

AtanorIteration* Atanorbox:: Newiteration(bool direction) {
	return aITERNULL;
}

Atanor* Atanorbox::MethodCreate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	Atanor* x1 = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y1 = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* x2 = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* y2 = callfunc->Evaluate(3, contextualpattern, idthread);
	Atanor* text = callfunc->Evaluate(4, contextualpattern, idthread);
	if (box != NULL)
		return globalAtanor->Returnerror("WND(101): box already created", idthread);
	string thetxt = text->String();
	if (thetxt.size() < 1024)
		strcpy_s(txt, 1024, thetxt.c_str());
	box = new Fl_Box(x1->Integer(), y1->Integer(), x2->Integer(), y2->Integer(), txt);
	SaveItem(this);
	return aTRUE;
}

Atanor* Atanorbox::MethodType(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (box == NULL)
		return globalAtanor->Returnerror("WND(102): box not created", idthread);
	Atanor* pos = callfunc->Evaluate(0, contextualpattern, idthread);
	if (pos->Type() == a_string) {
		if (boxTypes.find(pos->String()) == boxTypes.end()) {
			string mes = "WND(767): Unknown type:";
			mes += pos->String();
			return globalAtanor->Returnerror(mes, idthread);
		}
		else
			box->box((Fl_Boxtype)boxTypes[pos->String()]);
	}
	else
		box->box((Fl_Boxtype)pos->Integer());
	return aTRUE;
}





