
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
filename   : atanorwoutput.cxx
Date       : 2017/04/01
Purpose    : window output widget (based on FLTK)
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorwoutput.h"
#include "atoms.h"

//We need to declare once again our local definitions.
hmap<short, woutputMethod>  Atanorwoutput::methods;
hmap<string, string> Atanorwoutput::infomethods;
bin_hash<unsigned long> Atanorwoutput::exported;

short Atanorwoutput::idtype = 0;



//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorwoutput::AddMethod(AtanorGlobal* global, string name, woutputMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorwoutput::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorwoutput::idtype = global->Getid("woutput");

	Atanorwoutput::AddMethod(global, "create", &Atanorwoutput::MethodCreate, P_SIX, "create(int x, int y, int w, int h, boolean multiline, string label): Create an output");
	Atanorwoutput::AddMethod(global, "value", &Atanorwoutput::MethodValue, P_NONE | P_ONE, "value(string s): set the output buffer");
	Atanorwoutput::AddMethod(global, "wrap", &Atanorwoutput::MethodWrap, P_ONE, "wrap(bool): Set the wrapping mode");
	Atanorwoutput::AddMethod(global, "color", &Atanorwoutput::MethodColor, P_ONE | P_NONE, "color(string c|int c): set or return the text color");
	Atanorwoutput::AddMethod(global, "font", &Atanorwoutput::MethodFont, P_ONE | P_NONE, "font(string s): set or return the text font");
	Atanorwoutput::AddMethod(global, "fontsize", &Atanorwoutput::MethodFontSize, P_ONE | P_NONE, "fontsize(int c): set or return the text font size");
	Atanorwoutput::AddMethod(global, "tooltip", &Atanorwidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
	Atanorwoutput::AddMethod(global, "show", &Atanorwidget::MethodShow, P_NONE, "show(): Show the widget");
	Atanorwoutput::AddMethod(global, "hide", &Atanorwidget::MethodHide, P_NONE, "hide(): Hide the widget");
	Atanorwoutput::AddMethod(global, "labelsize", &Atanorwidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
	Atanorwoutput::AddMethod(global, "labelfont", &Atanorwidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
	Atanorwoutput::AddMethod(global, "labelcolor", &Atanorwidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
	Atanorwoutput::AddMethod(global, "labeltype", &Atanorwidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
	Atanorwoutput::AddMethod(global, "label", &Atanorwidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
	Atanorwoutput::AddMethod(global, "selectioncolor", &Atanorwidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
	Atanorwoutput::AddMethod(global, "boundaries", &Atanorwidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "boundaries(ivector crds): return a vector of the widget coordinates");
	Atanorwoutput::AddMethod(global, "redraw", &Atanorwidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
	Atanorwoutput::AddMethod(global, "backgroundcolor", &Atanorwidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
	Atanorwoutput::AddMethod(global, "created", &Atanorwidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
	Atanorwoutput::AddMethod(global, "copy", &Atanorwidget::MethodCopy, P_ONE, "copy(string): Copy string to clipboard");
	Atanorwoutput::AddMethod(global, "paste", &Atanorwidget::MethodPaste, P_NONE, "paste(): paste string");
	Atanorwoutput::AddMethod(global, "align", &Atanorwidget::MethodAlign, P_ONE, "align(int): define the label alignment");



	global->newInstance[Atanorwoutput::idtype] = new Atanorwoutput(global);
	global->RecordMethods(Atanorwoutput::idtype, Atanorwoutput::exported);

	return true;
}


Atanor* Atanorwoutput::Put(Atanor* index, Atanor* value, short idthread) {
	return this;
}

Atanor* Atanorwoutput::Get(Atanor* context, Atanor* index, short idthread) {
	return this;
}

AtanorIteration* Atanorwoutput::Newiteration(bool direction) {
	return aITERNULL;
}

Atanor* Atanorwoutput::MethodCreate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//0 is the first parameter and so on...
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* w = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* h = callfunc->Evaluate(3, contextualpattern, idthread);
	multiline = callfunc->Evaluate(4, contextualpattern, idthread)->Boolean();
	Atanor* label = callfunc->Evaluate(5, contextualpattern, idthread);
	if (output != NULL)
		return globalAtanor->Returnerror("WND(101): output already created", idthread);
	string thetxt = label->String();
	if (thetxt.size() < 1024)
		strcpy_s(txt, 1024, thetxt.c_str());
	if (multiline)
		output = new Fl_Multiline_Output(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
	else
		output = new Fl_Output(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
	output->cursor_color(FL_WHITE);
	SaveItem(this);
	return aTRUE;
}

Atanor* Atanorwoutput::MethodValue(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (output == NULL)
		return globalAtanor->Returnerror("WND(679): Output not initialized", idthread);
	if (callfunc->Size() == 0) {
		buf = output->value();
		return globalAtanor->Providestring(buf);
	}
	Atanor* label = callfunc->Evaluate(0, contextualpattern, idthread);
	buf = label->String();
	output->value((char*)buf.c_str());
	return aTRUE;
}

Atanor* Atanorwoutput::MethodWrap(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (output == NULL)
		return globalAtanor->Returnerror("WND(679): Output not initialized", idthread);
	bool mode = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
	if (mode)
		output->align(FL_ALIGN_WRAP);
	else
		output->align(FL_ALIGN_NOWRAP);
	return aTRUE;
}

Atanor* Atanorwoutput::MethodColor(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (output == NULL)
		return globalAtanor->Returnerror("WND(679): Output not initialized", idthread);
	if (callfunc->Size() == 0) {
		Fl_Color color = output->textcolor();
		return globalAtanor->Provideint(color);
	}
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	if (ke->Type() == a_string) {
		//0 is the first parameter and so on...
		buf = ke->String();
		if (colors.find(buf) == colors.end()) {
			buf = "WND(657): Unknown color code";
			return globalAtanor->Returnerror(buf, idthread);
		}
		output->textcolor(colors[buf]);
	}
	else {
		unsigned int c = ke->Integer();
		output->textcolor(c);
	}
	return aTRUE;
}

Atanor* Atanorwoutput::MethodFont(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (output == NULL)
		return globalAtanor->Returnerror("WND(679): Output not initialized", idthread);
	if (callfunc->Size() == 0) {
		int i = output->textfont();
		if (ifonts.find(i) != ifonts.end())
			return globalAtanor->Providestring(ifonts[i]);
		return globalAtanor->Provideint(i);
	}
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	//0 is the first parameter and so on...
	if (ke->Type() == a_string) {
		buf = ke->String();
		if (fonts.find(buf) == fonts.end()) {
			buf = "WND(657): Unknown font code";
			return globalAtanor->Returnerror(buf, idthread);
		}
		output->textfont(fonts[buf]);
	}
	else {
		int font = ke->Integer();
		if (font >= fontmax)
			return globalAtanor->Returnerror("WND(658): unknown font", idthread);
		output->textfont(font);
	}
	return aTRUE;
}

Atanor* Atanorwoutput::MethodFontSize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (output == NULL)
		return globalAtanor->Returnerror("WND(679): Output not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalAtanor->Provideint(output->textsize());
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	//0 is the first parameter and so on...
	output->textsize(ke->Integer());
	return aTRUE;
}





