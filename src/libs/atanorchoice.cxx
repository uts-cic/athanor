
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
filename   : atanorchoice.cxx
Date       : 2017/04/01
Purpose    : choice window (based on FLTK)
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorchoice.h"
#include "atoms.h"
#include "instructions.h"

static void choice_callback(Fl_Widget*, void* data) {
	ChoiceCallback* wn = (ChoiceCallback*)data;
	Atanorchoice* kwnd = wn->choice;
	Atanor* func = wn->function;
	AtanorCallFunction kfunc(func);

	kwnd->Setreference();
	wn->object->Setreference();
	kfunc.arguments.push_back(kwnd);
	kfunc.arguments.push_back(wn->object);
	kfunc.Get(aNULL, aNULL, globalAtanor->GetThreadid());
	kwnd->Resetreference();
	wn->object->Resetreference();
}

//We need to declare once again our local definitions.

//We need to declare once again our local definitions.
hmap<short, choiceMethod>  Atanorchoice::methods;
hmap<string, string> Atanorchoice::infomethods;
bin_hash<unsigned long> Atanorchoice::exported;

short Atanorchoice::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorchoice::AddMethod(AtanorGlobal* global, string name, choiceMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorchoice::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorchoice::idtype = global->Getid("choice");

	Atanorchoice::AddMethod(global, "_initial", &Atanorwidget::MethodInitial, P_ONE, "_initial(variable): Creation of a window with a call back object");
	Atanorchoice::AddMethod(global, "create", &Atanorchoice::MethodCreate, P_FIVE, "create(int x, int y, int w, int h, string label): Create an choice");
	Atanorchoice::AddMethod(global, "value", &Atanorchoice::MethodValue, P_ONE | P_NONE, "value(int s): set the choice initialization value");
	Atanorchoice::AddMethod(global, "font", &Atanorchoice::MethodFont, P_ONE | P_NONE, "font(string s): set or return the text font");
	Atanorchoice::AddMethod(global, "menu", &Atanorchoice::MethodMenu, P_ONE, "menu(vector s): Initialize the menu");
	Atanorchoice::AddMethod(global, "fontsize", &Atanorchoice::MethodFontSize, P_ONE | P_NONE, "fontsize(int c): set or return the text font size");
	Atanorchoice::AddMethod(global, "tooltip", &Atanorwidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
	Atanorchoice::AddMethod(global, "focus", &Atanorwidget::MethodFocus, P_NONE, "focus(): Get the focus");
	Atanorchoice::AddMethod(global, "show", &Atanorwidget::MethodShow, P_NONE, "show(): Show the widget");
	Atanorchoice::AddMethod(global, "hide", &Atanorwidget::MethodHide, P_NONE, "hide(): Hide the widget");
	Atanorchoice::AddMethod(global, "labelsize", &Atanorwidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
	Atanorchoice::AddMethod(global, "labelfont", &Atanorwidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
	Atanorchoice::AddMethod(global, "labelcolor", &Atanorwidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
	Atanorchoice::AddMethod(global, "labeltype", &Atanorwidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
	Atanorchoice::AddMethod(global, "label", &Atanorwidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
	Atanorchoice::AddMethod(global, "selectioncolor", &Atanorwidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
	Atanorchoice::AddMethod(global, "boundaries", &Atanorwidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "boundaries(ivector crds): return a vector of the widget coordinates");
	Atanorchoice::AddMethod(global, "redraw", &Atanorwidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
	Atanorchoice::AddMethod(global, "backgroundcolor", &Atanorwidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
	Atanorchoice::AddMethod(global, "created", &Atanorwidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
	Atanorchoice::AddMethod(global, "align", &Atanorwidget::MethodAlign, P_ONE, "align(int): define the label alignment");



	global->newInstance[Atanorchoice::idtype] = new Atanorchoice(global);
	global->RecordMethods(Atanorchoice::idtype,Atanorchoice::exported);

	return true;
}


Atanor* Atanorchoice::Put(Atanor* index, Atanor* value, short idthread) {
	return this;
}

Atanor* Atanorchoice::Get(Atanor* context, Atanor* index, short idthread) {
	return this;
}

AtanorIteration* Atanorchoice:: Newiteration(bool direction) {
	return aITERNULL;
}

Atanor* Atanorchoice::MethodCreate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	if (choice != NULL)
		return globalAtanor->Returnerror("WND(100): Cannot recreate an existing choice", idthread);
	//0 is the first parameter and so on...
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* wx = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* hy = callfunc->Evaluate(3, contextualpattern, idthread);
	string label = callfunc->Evaluate(4, contextualpattern, idthread)->String();
	strcpy_s(txt, 1024, label.c_str());
	choice = new Fl_Choice(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt);
	SaveItem(this);
	return aTRUE;
}

Atanor* Atanorchoice::MethodValue(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (choice == NULL)
		return globalAtanor->Returnerror("WND(678): choice not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalAtanor->Provideint(choice->value());
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	choice->value(ke->Integer());
	return aTRUE;
}

Atanor* Atanorchoice::MethodFont(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (choice == NULL)
		return globalAtanor->Returnerror("WND(678): choice not initialized", idthread);
	if (callfunc->Size() == 0) {
		int i = choice->textfont();
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
		font = fonts[buf];
	}
	else
		font = ke->Integer();
	if (font >= fontmax)
		return globalAtanor->Returnerror("WND(658): unknown font", idthread);
	choice->textfont(font);
	return aTRUE;
}

Atanor* Atanorchoice::MethodMenu(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (choice == NULL)
		return globalAtanor->Returnerror("WND(303): No window available", idthread);
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	//we expect a vector
	if (ke->isVectorContainer() == false)
		return globalAtanor->Returnerror("WND(109): Wrong parameter, vector expected", idthread);
	Atanor* menu = (Atanor*)ke;
	vector<Fl_Menu_Item*> elements;
	Fl_Menu_Item* fl;
	for (int i = 0; i < menu->Size(); i++) {
		//First level
		//each element is a vector
		ke = menu->getvalue(i);
		if (ke->isVectorContainer() == false)
			return globalAtanor->Returnerror("WND(109): Wrong element, vector expected", idthread);
		Atanor* v = ke;
		//then we analyse the sub-elements, the next element should be a vector
		if (v->Size() != 3)
			return globalAtanor->Returnerror("WND(109): Wrong element, missing field in a sub-menu", idthread);
		string nn = v->getvalue(0)->String();
		char* n = strdup(STR(nn));
		//second element shoudl be a Callback
		Atanor* function = v->getvalue(1);
		if (!function->isFunction())
			return globalAtanor->Returnerror("WND(109): Wrong element, third field in a sub-menu should be a function", idthread);
		Atanor* object = v->getvalue(2);
		if (!object->isCallVariable())
			object = object->Get(aNULL, aNULL, idthread);

		fl = new Fl_Menu_Item;
		fl->labeltype_ = 0;
		fl->labelfont_ = font;
		fl->labelsize_ = fontsize;
		fl->labelcolor_ = fontcolor;
		fl->text = n;
		fl->shortcut_ = 0;
		fl->callback_ = choice_callback;
		fl->user_data_ = new ChoiceCallback(function, object, this);
		fl->flags = 0;
		elements.push_back(fl);
	}
	flmenu = new Fl_Menu_Item[elements.size() + 1];
	flnbelements = elements.size();
	int i;
	for (i = 0; i < elements.size(); i++) {
		flmenu[i] = *elements[i];
		delete elements[i];
	}
	flmenu[i].text = 0;
	flmenu[i].shortcut_ = 0;
	flmenu[i].callback_ = NULL;
	flmenu[i].user_data_ = 0;
	flmenu[i].flags = 0;
	flmenu[i].labeltype_ = 0;
	flmenu[i].labelfont_ = 0;
	flmenu[i].labelsize_ = 12;
	flmenu[i].labelcolor_ = FL_BLACK;
	choice->menu(flmenu);
	return aTRUE;
}

Atanor* Atanorchoice::MethodFontSize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (choice == NULL)
		return globalAtanor->Returnerror("WND(678): choice not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalAtanor->Provideint(choice->textsize());
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	//0 is the first parameter and so on...
	fontsize = ke->Integer();
	return aTRUE;
}





