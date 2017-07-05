
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
filename   : atanorbutton.cxx
Date       : 2017/04/01
Purpose    : buttons (based on FLTK)
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorbutton.h"
#include "atanorbitmap.h"
#include "atanorimage.h"
#include "atoms.h"
#include "instructions.h"

//We need to declare once again our local definitions.
hmap<short, buttonMethod>  Atanorbutton::methods;
hmap<string, string> Atanorbutton::infomethods;
bin_hash<unsigned long> Atanorbutton::exported;

short Atanorbutton::idtype = 0;


static void button_callback(Fl_Widget *w, void *data) {
	Atanorbutton* button = (Atanorbutton*)data;
	Atanor* func = button->function;
	AtanorCallFunction kfunc(func);

	button->Setreference();
	button->object->Setreference();
	kfunc.arguments.push_back(button);
	kfunc.arguments.push_back(button->object);
	kfunc.Get(aNULL, aNULL, globalAtanor->GetThreadid());
	button->Resetreference();
	button->object->Resetreference();

}



//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorbutton::AddMethod(AtanorGlobal* global, string name, buttonMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorbutton::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorbutton::idtype = global->Getid("button");

	Atanorbutton::AddMethod(global, "_initial", &Atanorwidget::MethodInitial, P_ONE, "_initial(variable): Creation of a window with a callback object");
	Atanorbutton::AddMethod(global, "create", &Atanorbutton::MethodCreate, P_SEVEN | P_SIX | P_FIVE, "create(int x, int y, int w, int h, string type, string shape, string label): Create a button. If type is not provided then it is: FL_Regular.");
	Atanorbutton::AddMethod(global, "when", &Atanorbutton::MethodWhen, 65534, "when(string when1 string when2...): Type of event for a button which triggers the callback");
	Atanorbutton::AddMethod(global, "shortcut", &Atanorbutton::MethodShortcut, P_ONE, "shortcut(string keycode): Set a shortcut to activate the button from the keyboard");
	Atanorbutton::AddMethod(global, "color", &Atanorbutton::MethodColor, P_ONE, "color(string code|int code): Set the color of the button");
	Atanorbutton::AddMethod(global, "value", &Atanorbutton::MethodValue, P_ONE | P_NONE, "value(): return the value of the current button");
	Atanorbutton::AddMethod(global, "align", &Atanorwidget::MethodAlign, P_ONE, "align(int): define the button label alignment");
	Atanorbutton::AddMethod(global, "image", &Atanorbutton::MethodImage, P_ONE | P_TWO | P_THREE, "image(image im, string label, int labelalign): Use the image as a button image");
	Atanorbutton::AddMethod(global, "bitmap", &Atanorbutton::MethodBitmap, P_TWO | P_THREE | P_FOUR, "bitmap(bitmap im, int color, string label, int labelalign): Use the bitmap as a button image");
	Atanorbutton::AddMethod(global, "tooltip", &Atanorwidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
	Atanorbutton::AddMethod(global, "show", &Atanorwidget::MethodShow, P_NONE, "show(): Show the widget");
	Atanorbutton::AddMethod(global, "focus", &Atanorwidget::MethodFocus, P_NONE, "focus(): Get the focus");
	Atanorbutton::AddMethod(global, "hide", &Atanorwidget::MethodHide, P_NONE, "hide(): Hide the widget");
	Atanorbutton::AddMethod(global, "labelsize", &Atanorwidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
	Atanorbutton::AddMethod(global, "labelfont", &Atanorwidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
	Atanorbutton::AddMethod(global, "labelcolor", &Atanorwidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
	Atanorbutton::AddMethod(global, "labeltype", &Atanorwidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
	Atanorbutton::AddMethod(global, "label", &Atanorwidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
	Atanorbutton::AddMethod(global, "selectioncolor", &Atanorwidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
	Atanorbutton::AddMethod(global, "boundaries", &Atanorwidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "boundaries(ivector crds): return a vector of the widget coordinates");
	Atanorbutton::AddMethod(global, "redraw", &Atanorwidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
	Atanorbutton::AddMethod(global, "backgroundcolor", &Atanorwidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
	Atanorbutton::AddMethod(global, "created", &Atanorwidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");



	global->newInstance[Atanorbutton::idtype] = new Atanorbutton(global);
	global->RecordMethods(Atanorbutton::idtype,Atanorbutton::exported);

	return true;
}


Atanor* Atanorbutton::Put(Atanor* index, Atanor* value, short idthread) {
	return this;
}

Atanor* Atanorbutton::Get(Atanor* context, Atanor* index, short idthread) {
	return this;
}

AtanorIteration* Atanorbutton:: Newiteration(bool direction) {
	return aITERNULL;
}


Atanor* Atanorbutton::MethodCreate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* w = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* h = callfunc->Evaluate(3, contextualpattern, idthread);
	Atanor* ktype = NULL;
	Atanor* shape = NULL;
	Atanor* label;
	if (callfunc->Size() == 7) {
		ktype = callfunc->Evaluate(4, contextualpattern, idthread);
		shape = callfunc->Evaluate(5, contextualpattern, idthread);
		label = callfunc->Evaluate(6, contextualpattern, idthread);
	}
	else {
		if (callfunc->Size() == 6) {
			shape = callfunc->Evaluate(4, contextualpattern, idthread);
			label = callfunc->Evaluate(5, contextualpattern, idthread);
		}
		else
			label = callfunc->Evaluate(4, contextualpattern, idthread);
	}
	if (button != NULL)
		return globalAtanor->Returnerror("WND(101): button already created", idthread);
	int shap;
	if (shape == NULL)
		shap = 0;
	else {
		if (shape->Type() == a_string) {
			if (buttonShapes.find(shape->String()) == buttonShapes.end()) {
				string mes = "WND(675): Unknwown button shape: ";
				mes += shape->String();
				return globalAtanor->Returnerror(mes, idthread);
			}
			shap = buttonShapes[shape->String()];
		}
		else
			shap = shape->Integer();
	}
	string thetxt = label->String();
	if (thetxt.size() < 1024)
		strcpy_s(txt, 1024, thetxt.c_str());
	if (ktype == NULL)
		thetype = 0;
	else
	if (ktype->Type() == a_string) {
		string btype = ktype->String();
		if (buttontypes.find(btype) == buttontypes.end()) {
			string mes = "WND(675): Unknwown button type: ";
			mes += btype;
			return globalAtanor->Returnerror(mes, idthread);
		}
		thetype = buttontypes[btype];
	}
	else
		thetype = ktype->Integer();

	switch (thetype) {
	case 1: //check
		button = new Fl_Check_Button(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
		break;
	case 2://light
		button = new Fl_Light_Button(x->Integer(), y->Integer(), w->Integer(), h->Integer());
		break;
	case 3://Repeat
		button = new Fl_Repeat_Button(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
		break;
	case 4://Return
		button = new Fl_Return_Button(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
		break;
	case 5://Round
		button = new Fl_Round_Button(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
		break;
	case 6://Image
		button = new ButtonImage(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
		image = true;
		break;
	default://regular
		button = new Fl_Button(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
	}
	button->type(shap);
	if (function != NULL)
		button->callback(button_callback, this);

	SaveItem(this);
	return aTRUE;
}

Atanor* Atanorbutton::MethodWhen(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	if (button == NULL)
		return globalAtanor->Returnerror("WND(805): Button not initialized", idthread);
	Atanor* ke;
	int action = 0;
	for (int i = 0; i < callfunc->Size(); i++) {
		ke = callfunc->Evaluate(i, contextualpattern, idthread);
		if (ke->Type() == a_string) {
			if (buttonAction.find(ke->String()) == buttonAction.end()) {
				string mess = "WND(654): Unknown action";
				return globalAtanor->Returnerror(mess, idthread);
			}
			action |= buttonAction[ke->String()];
		}
		else
			action |= ke->Integer();
	}
	button->when(action);
	return aTRUE;
}

Atanor* Atanorbutton::MethodShortcut(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	if (button == NULL)
		return globalAtanor->Returnerror("WND(805): Button not initialized", idthread);
	//0 is the first parameter and so on...
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	if (ke->Type() == a_string) {
		string shortcut = ke->String();
		if (keyCodes.find(shortcut) == keyCodes.end()) {
			shortcut = "WND(656): Unknown key code";
			return globalAtanor->Returnerror(shortcut, idthread);
		}
		button->shortcut(keyCodes[shortcut]);
	}
	else
		button->shortcut(ke->Integer());
	return aTRUE;
}

Atanor* Atanorbutton::MethodColor(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	if (button == NULL)
		return globalAtanor->Returnerror("WND(805): Button not initialized", idthread);
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	if (ke->Type() == a_string) {
		//0 is the first parameter and so on...
		string shortcut = ke->String();
		if (colors.find(shortcut) == colors.end()) {
			shortcut = "WND(657): Unknown color code";
			return globalAtanor->Returnerror(shortcut, idthread);
		}
		button->color(colors[shortcut]);
	}
	else {
		unsigned int c = ke->Integer();
		button->color(c);
	}
	return aTRUE;
}

Atanor* Atanorbutton::MethodValue(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (button == NULL)
		return globalAtanor->Returnerror("WND(805): Button not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalAtanor->Provideint(button->value());
	Atanor* v = callfunc->Evaluate(0, contextualpattern, idthread);
	button->value(v->Integer());
	return aTRUE;
}

Atanor* Atanorbutton::MethodImage(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (button == NULL)
		return globalAtanor->Returnerror("WND(805): Button not initialized", idthread);
	if (!image)
		return globalAtanor->Returnerror("WND(809): Image button required", idthread);
	Atanor* kimage = callfunc->Evaluate(0, contextualpattern, idthread);
	if (kimage->Type() != Atanorimage::idtype)
		return globalAtanor->Returnerror("WND(813): image object required", idthread);
	Fl_Image* fl = ((Atanorimage*)kimage)->image;
	if (callfunc->Size() >= 2) {
		((ButtonImage*)button)->mytext = callfunc->Evaluate(1, contextualpattern, idthread)->String();
		if (callfunc->Size() == 3)
			((ButtonImage*)button)->myalign = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
	}
	((ButtonImage*)button)->myimage = fl;
	return aTRUE;
}

Atanor* Atanorbutton::MethodBitmap(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (button == NULL)
		return globalAtanor->Returnerror("WND(805): Button not initialized", idthread);
	if (!image)
		return globalAtanor->Returnerror("WND(809): Image button required", idthread);
	Atanor* kbitmap = callfunc->Evaluate(0, contextualpattern, idthread);
	if (kbitmap->Type() != Atanorbitmap::idtype)
		return globalAtanor->Returnerror("WND(813): image object required", idthread);
	Fl_Bitmap* fl = ((Atanorbitmap*)kbitmap)->bitmap;
	Atanor* kcolor = callfunc->Evaluate(1, contextualpattern, idthread);
	unsigned int c = FL_BLACK;
	if (kcolor->Type() == a_string) {
		//0 is the first parameter and so on...
		string shortcut = kcolor->String();
		if (colors.find(shortcut) == colors.end()) {
			shortcut = "WND(657): Unknown color code";
			return globalAtanor->Returnerror(shortcut, idthread);
		}
		c = colors[shortcut];
	}
	else
		c = kcolor->Integer();
	if (callfunc->Size() >= 3) {
		((ButtonImage*)button)->mytext = callfunc->Evaluate(2, contextualpattern, idthread)->String();
		if (callfunc->Size() == 4)
			((ButtonImage*)button)->myalign = callfunc->Evaluate(3, contextualpattern, idthread)->Integer();
	}
	((ButtonImage*)button)->thecolor = c;
	((ButtonImage*)button)->isbitmap = true;
	((ButtonImage*)button)->myimage = fl;
	return aTRUE;
}




