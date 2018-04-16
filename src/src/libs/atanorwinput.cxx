
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
filename   : atanorwinput.cxx
Date       : 2017/04/01
Purpose    : window input widget (based on FLTK)
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorwinput.h"
#include "atoms.h"
#include "instructions.h"
//We need to declare once again our local definitions.
hmap<short, winputMethod>  Atanorwinput::methods;
hmap<string, string> Atanorwinput::infomethods;
bin_hash<unsigned long> Atanorwinput::exported;

short Atanorwinput::idtype = 0;

static void input_callback(Fl_Widget *w, void *data) {
	Atanorwinput* input = (Atanorwinput*)data;
	Atanor* func = input->function;

	AtanorCallFunction kfunc(func);
	input->Setreference();
	input->object->Setreference();

	kfunc.arguments.push_back(input);
	kfunc.arguments.push_back(input->object);
	kfunc.Get(aNULL, aNULL, globalAtanor->GetThreadid());

	input->Resetreference();
	input->object->Resetreference();
}


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorwinput::AddMethod(AtanorGlobal* global, string name, winputMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorwinput::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorwinput::idtype = global->Getid("winput");

	Atanorwinput::AddMethod(global, "_initial", &Atanorwidget::MethodInitial, P_ONE, "_initial(variable): Creation of a window with a call back object");

	Atanorwinput::AddMethod(global, "create", &Atanorwinput::MethodCreate, P_SIX, "create(int x, int y, int w, int h, boolean multiline, string label): Create an input");
	Atanorwinput::AddMethod(global, "value", &Atanorwinput::MethodValue, P_ONE | P_NONE, "value()|(string v): return the input buffer or set the initial buffer");
	Atanorwinput::AddMethod(global, "insert", &Atanorwinput::MethodInsert, P_TWO, "insert(string s, int p): insert s at position p in the input");
	Atanorwinput::AddMethod(global, "selection", &Atanorwinput::MethodSelection, P_NONE, "selection(): return the selected text in the input");
	Atanorwinput::AddMethod(global, "color", &Atanorwinput::MethodColor, P_ONE | P_NONE, "color(string c|int c): set or return the text color");
	Atanorwinput::AddMethod(global, "word", &Atanorwinput::MethodWord, P_ONE, "word(int pos): Return the word at position pos");
	Atanorwinput::AddMethod(global, "font", &Atanorwinput::MethodFont, P_ONE | P_NONE, "font(string s): set or return the text font");
	Atanorwinput::AddMethod(global, "fontsize", &Atanorwinput::MethodFontSize, P_ONE | P_NONE, "fontsize(int c): set or return the text font size");
	Atanorwinput::AddMethod(global, "tooltip", &Atanorwidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
	Atanorwinput::AddMethod(global, "show", &Atanorwidget::MethodShow, P_NONE, "show(): Show the widget");
	Atanorwinput::AddMethod(global, "focus", &Atanorwidget::MethodFocus, P_NONE, "focus(): Get the focus");
	Atanorwinput::AddMethod(global, "hide", &Atanorwidget::MethodHide, P_NONE, "hide(): Hide the widget");
	Atanorwinput::AddMethod(global, "labelsize", &Atanorwidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
	Atanorwinput::AddMethod(global, "labelfont", &Atanorwidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
	Atanorwinput::AddMethod(global, "labelcolor", &Atanorwidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
	Atanorwinput::AddMethod(global, "labeltype", &Atanorwidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
	Atanorwinput::AddMethod(global, "label", &Atanorwidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
	Atanorwinput::AddMethod(global, "selectioncolor", &Atanorwidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
	Atanorwinput::AddMethod(global, "boundaries", &Atanorwidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "boundaries(ivector crds): return a vector of the widget coordinates");
	Atanorwinput::AddMethod(global, "redraw", &Atanorwidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
	Atanorwinput::AddMethod(global, "backgroundcolor", &Atanorwidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
	Atanorwinput::AddMethod(global, "created", &Atanorwidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
	Atanorwinput::AddMethod(global, "copy", &Atanorwidget::MethodCopy, P_ONE, "copy(string): Copy string to clipboard");
	Atanorwinput::AddMethod(global, "paste", &Atanorwidget::MethodPaste, P_NONE, "paste(): paste string");
	Atanorwinput::AddMethod(global, "align", &Atanorwidget::MethodAlign, P_ONE, "align(int): define the label alignment");


	global->newInstance[Atanorwinput::idtype] = new Atanorwinput(global);
	global->RecordMethods(Atanorwinput::idtype, Atanorwinput::exported);

	return true;
}


Atanor* Atanorwinput::Put(Atanor* index, Atanor* value, short idthread) {
	return this;
}

Atanor* Atanorwinput::Get(Atanor* context, Atanor* idx, short idthread) {
	if (input == NULL)
		return this;

	if (idx == NULL || idx == aNULL)
		return this;

	string value = input->value();
	int ileft, iright;
	char res = StringIndexes(value, idx, ileft, iright, idthread);

	if (res == 0) {
		if (globalAtanor->erroronkey)
			globalAtanor->Returnerror("Wrong key in a container or a string access", idthread);
		return aNOELEMENT;
	}

	if (res == 1)
		return globalAtanor->Providestring(c_char_get(USTR(value), ileft));

	return globalAtanor->Providestring(value.substr(ileft, iright - ileft));
}

AtanorIteration* Atanorwinput::Newiteration(bool direction) {
	return aITERNULL;
}

Atanor* Atanorwinput::MethodCreate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* w = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* h = callfunc->Evaluate(3, contextualpattern, idthread);
	multiline = callfunc->Evaluate(4, contextualpattern, idthread)->Boolean();
	Atanor* label = callfunc->Evaluate(5, contextualpattern, idthread);
	if (input != NULL)
		return globalAtanor->Returnerror("WND(101): input already created", idthread);
	string thetxt = label->String();
	if (thetxt.size() < 1024)
		strcpy_s(txt, 1024, thetxt.c_str());
	if (multiline)
		input = new Fl_Multiline_Input(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
	else
		input = new Fl_Input(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);

	

	if (function != NULL)
		input->callback(input_callback, this);

	SaveItem(this);
	return aTRUE;
}

Atanor* Atanorwinput::MethodValue(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (input == NULL)
		return globalAtanor->Returnerror("WND(677): Input not initialized", idthread);
	if (callfunc->Size() == 0) {
		buf = input->value();
		return globalAtanor->Providestring(buf);
	}
	buf = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	input->value(buf.c_str());
	return aTRUE;
}

Atanor* Atanorwinput::MethodInsert(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (input == NULL)
		return globalAtanor->Returnerror("WND(677): Input not initialized", idthread);
	buf = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	Atanor* pos = callfunc->Evaluate(1, contextualpattern, idthread);
	input->insert(buf.c_str(), pos->Integer());
	return aTRUE;
}

Atanor* Atanorwinput::MethodSelection(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (input == NULL)
		return globalAtanor->Returnerror("WND(677): Input not initialized", idthread);
	int pos = input->position();
	int nb = input->mark();
	buf = input->value();
	buf = buf.substr(pos, nb);
	return globalAtanor->Providestring(buf);
}

Atanor* Atanorwinput::MethodColor(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (input == NULL)
		return globalAtanor->Returnerror("WND(677): Input not initialized", idthread);
	if (callfunc->Size() == 0) {
		Fl_Color color = input->textcolor();
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
		input->textcolor(colors[buf]);
	}
	else {
		unsigned int c = ke->Integer();
		input->textcolor(c);
	}
	return aTRUE;
}

Atanor* Atanorwinput::MethodWord(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (input == NULL)
		return globalAtanor->Returnerror("WND(677): Input not initialized", idthread);
	//first we get our position
	int pos = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	//then we try to extract the word string
	int start = pos;
	const char* ibuf = input->value();
	string res;
	while (start > 0 && c_is_alpha((uchar*)ibuf, pos)) {
		start--;
		pos = start;
	}
	pos = ++start;
	while (pos <= input->size() && c_is_alpha((uchar*)ibuf, start)) {
		res += c_char_get((uchar*)ibuf, pos);
		pos++;
		start = pos;
	}
	return globalAtanor->Providestring(res);
}

Atanor* Atanorwinput::MethodFont(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (input == NULL)
		return globalAtanor->Returnerror("WND(677): Input not initialized", idthread);
	if (callfunc->Size() == 0) {
		int i = input->textfont();
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
		input->textfont(fonts[buf]);
	}
	else {
		int font = ke->Integer();
		if (font >= fontmax)
			return globalAtanor->Returnerror("WND(658): unknown font", idthread);
		input->textfont(font);
	}
	return aTRUE;
}

Atanor* Atanorwinput::MethodFontSize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (input == NULL)
		return globalAtanor->Returnerror("WND(677): Input not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalAtanor->Provideint(input->textsize());
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	//0 is the first parameter and so on...
	input->textsize(ke->Integer());
	return aTRUE;
}




