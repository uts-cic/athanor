
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
filename   : atanoreditor.cxx
Date       : 2017/04/01
Purpose    : editor widget (based on FLTK)
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanoreditor.h"
#include "instructions.h"
#include "automate.h"
#include "atanormapsi.h"

void style_update(int pos,          // I - Position of update
	int        nInserted,    // I - Number of inserted chars
	int        nDeleted,     // I - Number of deleted chars
	int        nRestyled,    // I - Number of restyled chars
	const char *deletedText, // I - Text that was deleted
	void       *stylarg) {     // I - Callback data



	Fl_Text_Buffer *stylebuf = ((struct Stylearg*)stylarg)->stylebuf;
	AtanorTextEditor* editor = ((struct Stylearg*)stylarg)->editor;
	unsigned char currentstyle = 'A';

	// If this is just a selection change, just unselect the style buffer...
	if (nInserted == 0 && nDeleted == 0) {
		stylebuf->unselect();
		editor->modified = false;
		return;
	}

	editor->modified = true;
	// Just delete characters in the style buffer...
	if (nDeleted > 0)
		stylebuf->remove(pos, pos + nDeleted);

	// Track changes in the text buffer...
	if (nInserted > 0) {
		// Insert characters into the style buffer...
		if (pos > 0) {
			char* cstyle = stylebuf->text_range(pos - 1, pos);
			currentstyle = cstyle[0];
			free(cstyle);
		}
		char* style = new char[nInserted + 1];
		memset(style, currentstyle, nInserted);
		style[nInserted] = '\0';
		stylebuf->replace(pos, pos + nDeleted, style);
		delete[] style;
	}
}

static void editor_callback(int pos, int nInserted, int nDeleted, int nRestyled, const char* deletedText, void* data) {

	Atanoreditor* input = (Atanoreditor*)data;
	Atanor* func = input->function;
	AtanorCallFunction kfunc(func);



	Atanorint* k1 = globalAtanor->Provideint(pos);
	Atanorint* k2 = globalAtanor->Provideint(nInserted);
	Atanorint* k3 = globalAtanor->Provideint(nDeleted);
	Atanorint* k4 = globalAtanor->Provideint(nRestyled);
	Atanorstring* ks = globalAtanor->Providestring("");
	if (deletedText != NULL)
		ks->value = deletedText;

	input->Setreference();
	input->object->Setreference();
	k1->Setreference();
	k2->Setreference();
	k3->Setreference();
	k4->Setreference();
	ks->Setreference();

	kfunc.arguments.push_back(input);
	kfunc.arguments.push_back(k1);
	kfunc.arguments.push_back(k2);
	kfunc.arguments.push_back(k3);
	kfunc.arguments.push_back(k4);
	kfunc.arguments.push_back(ks);
	kfunc.arguments.push_back(input->object);

	kfunc.Get(aNULL, aNULL, globalAtanor->GetThreadid());

	input->Resetreference();
	input->object->Resetreference();
	k1->Resetreference();
	k2->Resetreference();
	k3->Resetreference();
	k4->Resetreference();
	ks->Resetreference();

}


//We need to declare once again our local definitions.
hmap<short, editorMethod>  Atanoreditor::methods;
hmap<string, string> Atanoreditor::infomethods;
bin_hash<unsigned long> Atanoreditor::exported;
bin_hash<bool> Atanoreditor::nothreads;

short Atanoreditor::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanoreditor::AddMethod(AtanorGlobal* global, string name, editorMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanoreditor::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();
	nothreads.clear();

	Atanoreditor::idtype = global->Getid("editor");

	Atanoreditor::AddMethod(global, "_initial", &Atanorwidget::MethodInitial, P_ONE, "_initial(variable): Creation of a window with a call back object");

	Atanoreditor::AddMethod(global, "create", &Atanoreditor::MethodCreate, P_FIVE, "create(int x, int y, int w, int h, string label): Create an editor");
	Atanoreditor::AddMethod(global, "selection", &Atanoreditor::MethodSelection, P_NONE, "selection(): return the selected text in the editor or the coordinates of the selection according to the recipient variable");
	Atanoreditor::AddMethod(global, "value", &Atanoreditor::MethodValue, P_ONE | P_NONE, "value()|(string v): return the text in the editor or initialize the editor");
	Atanoreditor::AddMethod(global, "color", &Atanoreditor::MethodColor, P_ONE | P_NONE, "color(string c|int c): set or return the text color");
	Atanoreditor::AddMethod(global, "font", &Atanoreditor::MethodFont, P_ONE | P_NONE, "font(string s)|(): set or return the text font");
	Atanoreditor::AddMethod(global, "fontsize", &Atanoreditor::MethodFontSize, P_ONE | P_NONE, "fontsize(int c)|(): set or return the text font size");
	Atanoreditor::AddMethod(global, "setstyle", &Atanoreditor::MethodSetStyle, P_THREE, "setstyle(int start, int end, string style): set the style of a portion of text");
	Atanoreditor::AddMethod(global, "load", &Atanoreditor::MethodLoad, P_ONE, "load(string f): load file into editor");
	Atanoreditor::AddMethod(global, "save", &Atanoreditor::MethodSave, P_ONE, "save(string f): save buffer to file f");
	Atanoreditor::AddMethod(global, "replace", &Atanoreditor::MethodReplace, P_FOUR | P_THREE | P_TWO, "replace(string s, string sub, int i, bool matchcase): replace s with sub according to matchcase starting at i");
	Atanoreditor::AddMethod(global, "replaceall", &Atanoreditor::MethodReplaceAll, P_TWO | P_THREE, "replaceall(string s, string sub, bool matchcase): replace all occurrences of s with sub according to matchcase");
	Atanoreditor::AddMethod(global, "find", &Atanoreditor::MethodFind, P_ONE | P_TWO | P_THREE, "find(string s, int i, bool matchcase): find the position of a substring from i");
	Atanoreditor::AddMethod(global, "rfind", &Atanoreditor::MethodRFind, P_ONE | P_TWO | P_THREE, "rfind(string s, int i, bool matchcase): find the position of a substring from i backward");
	Atanoreditor::AddMethod(global, "highlight", &Atanoreditor::MethodHighlight, P_NONE | P_TWO, "highlight(int start, int end)|(): highlight a portion of text. Return true if the text is highlighted or the highlighted text");
	Atanoreditor::AddMethod(global, "unhighlight", &Atanoreditor::MethodUnHighlight, P_NONE, "unhighlight(): unhighlight a portion of text");
	Atanoreditor::AddMethod(global, "cursorstyle", &Atanoreditor::MethodCursorStyle, P_ONE, "cursorstyle(int cursorshape): Set the cursor shape");
	Atanoreditor::AddMethod(global, "onmouse", &Atanoreditor::MethodOnMouseClick, P_THREE, "onmouse(int actionfunction fobject o): callback when clicking");
	Atanoreditor::AddMethod(global, "onkey", &Atanoreditor::MethodOnKey, P_THREE, "onkey(int actionfunction fobject o): callback when a key is pressed");
	Atanoreditor::AddMethod(global, "onvscroll", &Atanoreditor::MethodOnVScroll, P_TWO, "onvscroll(function fobject o): callback when scrolling vertically");
	Atanoreditor::AddMethod(global, "onhscroll", &Atanoreditor::MethodOnHScroll, P_TWO, "onhscroll(function fobject o): callback when scrolling horizontally");
	Atanoreditor::AddMethod(global, "getstyle", &Atanoreditor::MethodGetStyle, P_TWO, "getstyle(int start, int end): return a vector of style for each character of the text section");
	Atanoreditor::AddMethod(global, "addstyle", &Atanoreditor::MethodAddStyles, P_ONE, "addstyle(map style): Initialize styles");
	Atanoreditor::AddMethod(global, "delete", &Atanoreditor::MethodDelete, P_NONE, "delete(): delete selected text from editor");
	Atanoreditor::AddMethod(global, "cut", &Atanoreditor::MethodCut, P_NONE, "cut(): cut selected text from editor into clipboard");
	Atanoreditor::AddMethod(global, "copy", &Atanoreditor::MethodCopy, P_NONE | P_ONE, "copy(string s): copy selected text from editor into clipboard (string s is optional)");
	Atanoreditor::AddMethod(global, "paste", &Atanoreditor::MethodPaste, P_NONE, "paste(): paste selected text from clipboard");
	Atanoreditor::AddMethod(global, "align", &Atanorwidget::MethodAlign, P_ONE, "align(int): define the label alignment");
	Atanoreditor::AddMethod(global, "select", &Atanoreditor::MethodSelect, P_TWO | P_NONE, "select(): Return selected , string.\rselect(int posfirst, int poslast): Select the string between posfirst and posend");
	Atanoreditor::AddMethod(global, "line", &Atanoreditor::MethodLine, P_ONE | P_NONE, "line(int l): return the current line position of the caret or return the line corresponding to a position");
	Atanoreditor::AddMethod(global, "linebounds", &Atanoreditor::MethodLineBoundaries, P_ONE | P_NONE, "linebounds(int l): return the beginning and the end at the current line position of the caret or return the line boundaries corresponding to a position");
	Atanoreditor::AddMethod(global, "linecharbounds", &Atanoreditor::MethodLineBoundariesChar, P_ONE | P_NONE, "linecharbounds(): return the beginning and the end at the current line position of the caret or return the line boundaries corresponding to a position as characters");
	Atanoreditor::AddMethod(global, "cursor", &Atanoreditor::MethodCursor, P_ONE | P_NONE, "cursor(int l): return the current cursor position of the caret or set the cursor position");
	Atanoreditor::AddMethod(global, "cursorchar", &Atanoreditor::MethodCursorChar, P_NONE, "cursorchar(): return the current cursor position of the caret as characters");
	Atanoreditor::AddMethod(global, "gotoline", &Atanoreditor::MethodGotoLine, P_TWO, "gotoline(int l, bool highlight): Goto line l. If true highlight the line");
	Atanoreditor::AddMethod(global, "annotate", &Atanoreditor::MethodAnnotate, P_TWO | P_THREE, "annotate(string s|vector v, string style, bool matchcase): Annotate each occurence of s with style (matchcase is a default argument)");
	Atanoreditor::AddMethod(global, "word", &Atanoreditor::MethodWord, P_ONE, "word(int pos): Return the word at position pos");
	Atanoreditor::AddMethod(global, "wrap", &Atanoreditor::MethodWrap, P_ONE, "wrap(bool): Set the wrapping mode");
	Atanoreditor::AddMethod(global, "annotateregexip", &Atanoreditor::MethodAnnotateXIPRgx, P_TWO, "annotateregexip(string reg, string style): Annotate each string matching reg with style");
	Atanoreditor::AddMethod(global, "tooltip", &Atanorwidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
	Atanoreditor::AddMethod(global, "show", &Atanorwidget::MethodShow, P_NONE, "show(): Show the widget");
	Atanoreditor::AddMethod(global, "focus", &Atanorwidget::MethodFocus, P_NONE, "focus(): Get the focus");
	Atanoreditor::AddMethod(global, "hide", &Atanorwidget::MethodHide, P_NONE, "hide(): Hide the widget");
	Atanoreditor::AddMethod(global, "labelsize", &Atanorwidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
	Atanoreditor::AddMethod(global, "labelfont", &Atanorwidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
	Atanoreditor::AddMethod(global, "labelcolor", &Atanorwidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
	Atanoreditor::AddMethod(global, "labeltype", &Atanorwidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
	Atanoreditor::AddMethod(global, "label", &Atanorwidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
	Atanoreditor::AddMethod(global, "selectioncolor", &Atanorwidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
	Atanoreditor::AddMethod(global, "boundaries", &Atanorwidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "boundaries(ivector crds): return a vector of the widget coordinates");
	Atanoreditor::AddMethod(global, "redraw", &Atanorwidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
	Atanoreditor::AddMethod(global, "textsize", &Atanoreditor::MethodTextMeasure, P_ONE, "textsize(string l): Return a map with w and h as key to denote width and height of the string in pixels");
	Atanoreditor::AddMethod(global, "backgroundcolor", &Atanorwidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
	Atanoreditor::AddMethod(global, "insert", &Atanoreditor::MethodInsert, P_TWO, "insert(string s, int p): insert s at position p in the input");
	Atanoreditor::AddMethod(global, "append", &Atanoreditor::MethodAppend, P_ONE, "append(string s): append s at the end of the input");
	Atanoreditor::AddMethod(global, "byteposition", &Atanoreditor::MethodBytePosition, P_ONE, "byteposition(int i): return the position in byte from a character position");
	Atanoreditor::AddMethod(global, "charposition", &Atanoreditor::MethodCharacterPosition, P_ONE, "charposition(int i): return the position in character from a byte position");
	Atanoreditor::AddMethod(global, "created", &Atanorwidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");

	nothreads[global->Getid("replace")] = true; //atanoreditor
	nothreads[global->Getid("find")] = true; //atanoreditor
	nothreads[global->Getid("rfind")] = true; //atanoreditor
	nothreads[global->Getid("getstyle")] = true; //atanoreditor
	nothreads[global->Getid("gotoline")] = true; //atanoreditor
	nothreads[global->Getid("word")] = true; //atanoreditor
	nothreads[global->Getid("textsize")] = true; //atanoreditor
	nothreads[global->Getid("byteposition")] = true; //atanoreditor
	nothreads[global->Getid("charposition")] = true; //atanoreditor


	global->newInstance[Atanoreditor::idtype] = new Atanoreditor(global);
	global->RecordMethods(Atanoreditor::idtype, Atanoreditor::exported);

	return true;
}


Atanor* Atanoreditor::Put(Atanor* index, Atanor* value, short idthread) {
	return this;
}

Atanor* Atanoreditor::Get(Atanor* context, Atanor* idx, short idthread) {
	if (editor == NULL)
		return this;

	if (idx == NULL || idx == aNULL)
		return this;

	string value = (char*)textbuf->internalbuffer();

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

AtanorIteration* Atanoreditor::Newiteration(bool direction) {
	return aITERNULL;
}

bool AtanorTextEditor::Setmouseaction(int e, Atanor* f, Atanor* object) {
	if (!f->isFunction() || f->Size() != 3 || f->Parameter(0)->Type() != Atanoreditor::idtype)
		return false;
	switch (e) {
	case FL_PUSH:
	case FL_RELEASE:
	case FL_MOVE:
	case FL_DRAG:
	case FL_MOUSEWHEEL:
		functionmouse[e].Use(f);
		objectmouse[e].Use(object);
		return true;
	}
	return false;
}

bool AtanorTextEditor::Setkeyaction(int e, Atanor* f, Atanor* object) {
	if (!f->isFunction() || f->Size() != 5 || f->Parameter(0)->Type() != Atanoreditor::idtype)
		return false;
	if (e != FL_KEYDOWN && e != FL_KEYUP)
		return false;
	functionkey[e].Use(f);
	objectkey[e].Use(object);
	return true;
}


Atanor* Atanoreditor::MethodCreate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two arguments
	//0 is the first parameter and so on...
	

	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* w = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* h = callfunc->Evaluate(3, contextualpattern, idthread);
	Atanor* label = callfunc->Evaluate(4, contextualpattern, idthread);
	if (editor != NULL)
		return globalAtanor->Returnerror("WND(101): editor already created", idthread);
	string thetxt = label->String();
	if (thetxt.size() < 1024)
		strcpy_s(txt, 1024, thetxt.c_str());
	editor = new AtanorTextEditor(this, x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
	textbuf = new AtanorTextBuf();
	editor->buffer(textbuf);
	if (function != NULL) {
		if (function->Size() != 7)
			return globalAtanor->Returnerror("KIF(881): Error, wrong function arguments", idthread);
		textbuf->add_modify_callback(editor_callback, this);
	}
	SaveItem(this);
	return aTRUE;
}

Atanor* Atanoreditor::MethodSelection(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	int start = 0, end = 0;
	if (contextualpattern->Type() == a_int || contextualpattern->Type() == a_float) {
		textbuf->selection_position(&start, &end);
		return globalAtanor->Provideint(start);
	}
	if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
		textbuf->selection_position(&start, &end);
		contextualpattern->Clear();
		contextualpattern->Push(globalAtanor->Provideint(start));
		contextualpattern->Push(globalAtanor->Provideint(end));
		return contextualpattern;
	}
	if (contextualpattern->isMapContainer()) {
		contextualpattern->Clear();
		textbuf->selection_position(&start, &end);
		contextualpattern->Push("start", globalAtanor->Provideint(start));
		contextualpattern->Push("end", globalAtanor->Provideint(end));
		return contextualpattern;
	}
	char* str = textbuf->selection_text();
	buf = str;
	free(str);
	return globalAtanor->Providestring(buf);
}

Atanor* Atanoreditor::MethodValue(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	if (callfunc->Size() == 0) {
		char* ltxt = textbuf->text();
		Atanor* kval = globalAtanor->Providestring(ltxt);
		free(ltxt);
		return kval;
	}
	buf = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	textbuf->text(buf.c_str());
	return aTRUE;
}

Atanor* Atanoreditor::MethodColor(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	if (callfunc->Size() == 0) {
		Fl_Color color = editor->textcolor();
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
		editor->textcolor(colors[buf]);
	}
	else {
		unsigned int c = ke->Integer();
		editor->textcolor(c);
	}
	return aTRUE;
}

Atanor* Atanoreditor::MethodFont(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	if (callfunc->Size() == 0) {
		int i = editor->textfont();
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
		editor->textfont(fonts[buf]);
	}
	else {
		int font = ke->Integer();
		if (font >= fontmax)
			return globalAtanor->Returnerror("WND(658): unknown font", idthread);
		editor->textfont(font);
	}
	return aTRUE;
}

Atanor* Atanoreditor::MethodFontSize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalAtanor->Provideint(editor->textsize());
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	//0 is the first parameter and so on...
	editor->textsize(ke->Integer());
	return aTRUE;
}

Atanor* Atanoreditor::AddStyle(short idthread, long start, long end, uchar stylecode) {

	if (start<0 || end>stylebuf->length() || start >= end)
		return globalAtanor->Returnerror("WND(689): Out of range", idthread);

	//char* style = new char[end-start+1];
	//memset(style,stylecode,end-start);
	//style[end-start]=0;
	//stylebuf->replace(start,end,style);
	//editor->redisplay_range(start, end);
	//delete[] style;

	stylebuf->setstyle(start, end, stylecode);
	editor->redisplay_range(start, end);
	return aTRUE;
}


Atanor* Atanoreditor::MethodSetStyle(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	if (styletable == NULL)
		return globalAtanor->Returnerror("WND(679): Style table not initialized", idthread);
	//we need:
	Atanor* kstart = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* kend = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* kstyle = callfunc->Evaluate(2, contextualpattern, idthread);
	string stylekey = kstyle->String();
	if (stylekeys.find(stylekey) == stylekeys.end())
		return globalAtanor->Returnerror("WND(679): Unknown style key", idthread);
	unsigned char stylecode = stylekeys[stylekey][0];
	return AddStyle(idthread, kstart->Integer(), kend->Integer(), stylecode);
}

Atanor* Atanoreditor::MethodLoad(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	//we need:
	string f = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	textbuf->loadfile(STR(f));
	return aTRUE;
}

Atanor* Atanoreditor::MethodSave(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	//we need:
	string f = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	textbuf->savefile(STR(f));
	return aTRUE;
}

Atanor* Atanoreditor::MethodReplace(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	int i = 0;
	//we need:
	string sfind = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	string rep = callfunc->Evaluate(1, contextualpattern, idthread)->String();
	int matchcase = 0;
	if (callfunc->Size() >= 3)
		i = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
	if (callfunc->Size() == 4)
		matchcase = callfunc->Evaluate(3, contextualpattern, idthread)->Integer();
	char ret = textbuf->search_forward(i, STR(sfind), &i, matchcase);
	if (ret) {
		textbuf->replace(i, i + sfind.size(), STR(rep));
		return globalAtanor->Provideint(i);
	}
	return aMINUSONE;
}

Atanor* Atanoreditor::MethodReplaceAll(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	int i = 0;
	//we need:
	string sfind = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	string rep = callfunc->Evaluate(1, contextualpattern, idthread)->String();
	int matchcase = 0;
	if (callfunc->Size() == 3)
		matchcase = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
	char ret = textbuf->search_forward(i, STR(sfind), &i, matchcase);
	bool made = ret;
	while (ret == 1) {
		textbuf->replace(i, i + sfind.size(), STR(rep));
		i += rep.size();
		ret = textbuf->search_forward(i, STR(sfind), &i, matchcase);
	}
	if (made)
		return aTRUE;
	return aFALSE;
}

Atanor* Atanoreditor::MethodFind(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	int i = 0;
	//we need:
	string sfind = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	if (callfunc->Size() == 2)
		i = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
	int matchcase = 0;
	if (callfunc->Size() == 3)
		matchcase = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
	if (contextualpattern->isVectorContainer()) {
		Atanor* kvect = Selectaivector(contextualpattern);
		char ret = textbuf->search_forward(i, STR(sfind), &i, matchcase);
		while (ret == 1) {
			kvect->storevalue((long)i);
			i++;
			ret = textbuf->search_forward(i, STR(sfind), &i, matchcase);
		}
		return kvect;
	}
	if (textbuf->search_forward(i, STR(sfind), &i, matchcase) == 1)
		return globalAtanor->Provideint(i);
	return aMINUSONE;
}

Atanor* Atanoreditor::MethodRFind(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	int i = 0;
	//we need:
	string sfind = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	if (callfunc->Size() == 2)
		i = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
	int matchcase = 0;
	if (callfunc->Size() == 3)
		matchcase = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
	if (contextualpattern->isVectorContainer()) {
		Atanor* kvect = Selectaivector(contextualpattern);
		char ret = textbuf->search_backward(i, STR(sfind), &i, matchcase);
		while (ret == 1) {
			kvect->storevalue((long)i);
			i++;
			ret = textbuf->search_backward(i, STR(sfind), &i, matchcase);
		}
		return kvect;
	}
	if (textbuf->search_backward(i, STR(sfind), &i, matchcase) == 1)
		return globalAtanor->Provideint(i);
	return globalAtanor->Provideint(-1);
}

Atanor* Atanoreditor::MethodHighlight(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	if (callfunc->Size() == 0) {
		if (contextualpattern->Type() == a_string) {
			string s = textbuf->highlight_text();
			return globalAtanor->Providestring(s);
		}
		return globalAtanor->Provideint(textbuf->highlight());
	}
	//we need:
	int start = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	int end = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
	if (start < 0 || end >= textbuf->length() || start >= end)
		return globalAtanor->Returnerror("WND(689): Out of range", idthread);
	textbuf->highlight(start, end);
	editor->insert_position(end);
	editor->show_insert_position();
	return aNULL;
}

Atanor* Atanoreditor::MethodUnHighlight(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	textbuf->unhighlight();
	return aTRUE;
}

Atanor* Atanoreditor::MethodCursorStyle(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	//0 is the first parameter and so on...
	Atanor* kaction = callfunc->Evaluate(0, contextualpattern, idthread);
	Fl_Cursor act;
	if (kaction->Type() == a_string) {
		string action = kaction->String();
		if (cursors.find(action) == cursors.end())
			return globalAtanor->Returnerror("WND(344): Unknown keyboard action", idthread);
		act = (Fl_Cursor)actions[action];
	}
	else
		act = (Fl_Cursor)kaction->Integer();
	editor->cursor_style(act);
	return aTRUE;
}

Atanor* Atanoreditor::MethodOnMouseClick(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (editor == NULL)
		return globalAtanor->Returnerror("WND(101): No editor available", idthread);
	Atanor* kaction = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* function = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* object = callfunc->arguments[2];
	if (!object->isCallVariable())
		object = object->Get(aNULL, aNULL, idthread);

	int act;
	if (kaction->Type() == a_string) {
		string action = kaction->String();
		if (actions.find(action) == actions.end())
			return globalAtanor->Returnerror("WND(341): Unknown mouse action", idthread);
		act = actions[action];
	}
	else
		act = kaction->Integer();
	if (editor->Setmouseaction(act, function, object) == false)
		return globalAtanor->Returnerror("WND(342): Wrong parameter in your mouse callback", idthread);
	return aTRUE;
}

Atanor* Atanoreditor::MethodOnKey(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (editor == NULL)
		return globalAtanor->Returnerror("WND(101): No editor available", idthread);
	Atanor* kaction = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* function = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* object = callfunc->arguments[2];
	if (!object->isCallVariable())
		object = object->Get(aNULL, aNULL, idthread);

	int act;
	if (kaction->Type() == a_string) {
		string action = kaction->String();
		if (actions.find(action) == actions.end())
			return globalAtanor->Returnerror("WND(341): Unknown mouse action", idthread);
		act = actions[action];
	}
	else
		act = kaction->Integer();
	if (editor->Setkeyaction(act, function, object) == false)
		return globalAtanor->Returnerror("WND(342): Wrong parameter in your key callback", idthread);
	return aTRUE;
}

Atanor* Atanoreditor::MethodOnVScroll(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (editor == NULL)
		return globalAtanor->Returnerror("WND(101): No editor available", idthread);
	Atanor* function = callfunc->Evaluate(0, contextualpattern, idthread);
	if (!function->isFunction())
		return globalAtanor->Returnerror("WND(109): Wrong element, the first parameter should be a function", idthread);
	Atanor* object = callfunc->arguments[1];
	if (!object->isCallVariable())
		object = object->Get(aNULL, aNULL, idthread);

	editor->functionvscroll = function;
	editor->objectvscroll = object;
	return aTRUE;
}

Atanor* Atanoreditor::MethodOnHScroll(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (editor == NULL)
		return globalAtanor->Returnerror("WND(101): No editor available", idthread);
	Atanor* function = callfunc->Evaluate(0, contextualpattern, idthread);
	if (!function->isFunction())
		return globalAtanor->Returnerror("WND(109): Wrong element, the first parameter should be a function", idthread);
	Atanor* object = callfunc->arguments[1];
	if (!object->isCallVariable())
		object = object->Get(aNULL, aNULL, idthread);

	editor->functionhscroll = function;
	editor->objecthscroll = object;
	return aTRUE;
}

Atanor* Atanoreditor::MethodGetStyle(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	if (styletable == NULL)
		return globalAtanor->Returnerror("WND(679): Style table not initialized", idthread);
	//we need:
	Atanor* kstart = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* kend = callfunc->Evaluate(1, contextualpattern, idthread);
	int start = kstart->Integer();
	int end = kend->Integer();
	if (contextualpattern->isVectorContainer()) {
		Atanor* kvect = Selectasvector(contextualpattern);
		if (start < 0 || end >= stylebuf->length() || start >= end)
			return kvect;
		char* style = stylebuf->text_range(start, end);
		char c[] = { 0, 0 };
		for (int i = 0; i < end - start; i++) {
			c[0] = style[i];
			kvect->storevalue(keystyles[c]);
		}
		free(style);
		return kvect;
	}
	if (start < 0 || end >= stylebuf->length() || start >= end)
		return aNULL;
	string styles;
	char* style = stylebuf->text_range(start, end);
	char c[] = { 0, 0 };
	for (int i = 0; i < end - start; i++) {
		c[0] = style[i];
		styles += keystyles[c];
	}
	free(style);
	return globalAtanor->Providestring(styles);
}


bool Atanoreditor::InitializeItemMenu(string ckey, Atanor* kvect, unsigned char* key, int& ikey) {
	if (kvect->Size()<3 || kvect->Size()>5) {
		buf = "EDT(768): Wrong number of elements in the vector... 3 or 4 expected";
		return false;
	}

	while (key[ikey] == '0') ikey++;

	string skey;
	skey = key[ikey];
	Atanor* kcolor = kvect->getvalue(0);
	Atanor* kfont = kvect->getvalue(1);
	Atanor* ksize = kvect->getvalue(2);
	string message = "";

	if (kvect->Size() >= 4)
		message = kvect->getvalue(3)->String();

	long font;
	if (kfont->Type() == a_string) {
		buf = kfont->String();
		if (fonts.find(buf) == fonts.end()) {
			buf = "WND(657): Unknown font code";
			return(false);
		}
		font = fonts[buf];
	}
	else
		font = kfont->Integer();

	if (font >= fontmax) {
		buf = "WND(658): unknown font";
		return false;
	}

	Fl_Color color;
	if (kcolor->Type() == a_string) {
		//0 is the first parameter and so on...
		buf = kcolor->String();
		if (colors.find(buf) == colors.end()) {
			buf = "WND(657): Unknown color code";
			return false;
		}
		color = colors[buf];
	}
	else
		color = kcolor->Integer();

	Fl_Color localcolor = color;
	if (kvect->Size() == 5) {
		kcolor = kvect->getvalue(4);
		if (kcolor->Type() == a_string) {
			//0 is the first parameter and so on...
			buf = kcolor->String();
			if (colors.find(buf) == colors.end()) {
				buf = "WND(657): Unknown color code";
				return false;
			}
			localcolor = colors[buf];
		}
		else
			localcolor = kcolor->Integer();
	}

	int i;
	if (stylekeys.find(ckey) != stylekeys.end()) {
		skey = stylekeys[ckey];
		i = skey[0] - 'A';
	}
	else {
		if (key[ikey] == 0) {
			buf = "WND(627): Maximum number of styles reached";
			return false;
		}

		stylekeys[ckey] = skey;
		keystyles[skey] = ckey;
		i = ikey;
		key[ikey++] = '0';
	}

	if (i >= nbstyle) {
		Fl_Text_Display::Style_Table_Entry* prev = styletable;
		styletable = new Fl_Text_Display::Style_Table_Entry[i + 1];
		//first we copy the previous style...
		int p;
		for (p = 0; p < nbstyle; p++)
			styletable[p] = prev[p];

		delete[] prev;
		//second we initialize the intermediary style...
		for (p = nbstyle; p < i; p++) {
			styletable[p].color = 56;
			styletable[p].font = 0;
			styletable[p].size = 12;
			styletable[p].attr = 0;
		}
		nbstyle = i + 1;
	}

	styletable[i].color = color;
	styletable[i].font = font;
	styletable[i].size = ksize->Integer();
	styletable[i].attr = 0;
	//a message can be associated with the style
	if (message != "") {
		messagestyle[skey] = message;
		messagecolor[skey] = localcolor;
	}
	return true;
}

Atanor* Atanoreditor::MethodAddStyles(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	bool first = true;
	if (stylebuf != NULL)
		first = false;
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	if (ke->isMapContainer() == false)
		return globalAtanor->Returnerror("EDT(766): Wrong input: map expected", idthread);
	int key;
	Atanor* kmap = ke;
	nbstyle = kmap->Size();
	if (nbstyle < stylekeys.size())
		nbstyle = stylekeys.size();
	if (nbstyle == 0)
		return globalAtanor->Returnerror("EDT(770): Wrong input: empty map", idthread);
	if (first)
		stylebuf = new AtanorTextBuf();
	else
		delete[] styletable;
	styletable = new Fl_Text_Display::Style_Table_Entry[nbstyle];
	for (key = 0; key < nbstyle; key++) {
		styletable[key].color = 56;
		styletable[key].font = 0;
		styletable[key].size = 12;
		styletable[key].attr = 0;
	}
	string skey = "#";
	ke = kmap->Value(skey);
	if (ke == aNULL)
		return globalAtanor->Returnerror("EDT(767): Missing '#' key in the map. '#' is the initial key and is mandatory", idthread);
	if (ke->isVectorContainer() == false)
		return globalAtanor->Returnerror("EDT(767): Wrong input: vector expected", idthread);
	unsigned char allowedkeys[60];
	unsigned char c;
	for (c = 'A'; c <= 'z'; c++)
		allowedkeys[c - 'A'] = c;
	allowedkeys[c - 'A'] = 0;
	key = 0;
	if (!first) {
		//We remove the non active keys
		map<string, string>::iterator ikey;
		map<string, string> removes;
		for (ikey = stylekeys.begin(); ikey != stylekeys.end(); ikey++) {
			if (kmap->Value(ikey->first) == aNULL)
				removes[ikey->first] = ikey->second;
		}
		for (ikey = removes.begin(); ikey != removes.end(); ikey++) {
			stylekeys.erase(ikey->first);
			keystyles.erase(ikey->second);
			messagestyle.erase(ikey->second);
			messagecolor.erase(ikey->second);
		}
		unsigned char base = 'B';
		for (ikey = stylekeys.begin(); ikey != stylekeys.end(); ikey++) {
			if (ikey->first == "#")
				ikey->second[0] = 'A';
			else {
				ikey->second[0] = base;
				c = base - 'A';
				allowedkeys[c] = '0';
				base++;
			}
		}
	}
	if (!InitializeItemMenu(skey, ke, allowedkeys, key))
		return globalAtanor->Returnerror(buf, idthread);
	allowedkeys[0] = '0';
	key = 1;
	AtanorIteration* itr = kmap->Newiteration(false);
	for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
		if (itr->Keystring() == "#")
			continue;
		ke = itr->Value();
		if (ke->isVectorContainer() == false) {
			ke->Release();
			itr->Release();
			return globalAtanor->Returnerror("EDT(767): Wrong input: vector expected", idthread);
		}
		if (!InitializeItemMenu(itr->Keystring(), ke, allowedkeys, key)) {
			ke->Release();
			itr->Release();
			return globalAtanor->Returnerror(buf, idthread);
		}
		ke->Release();
	}
	itr->Release();
	editor->highlight_data(stylebuf, styletable, nbstyle, 'A', NULL, 0);
	stylearg.editor = editor;
	stylearg.stylebuf = stylebuf;
	stylearg.currentstyle = 'A';
	if (first)
		textbuf->add_modify_callback(style_update, &stylearg);
	else
		editor->redraw();
	return aTRUE;
}

Atanor* Atanoreditor::MethodDelete(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	Fl_Text_Editor::kf_delete(0, editor);
	return aTRUE;
}

Atanor* Atanoreditor::MethodCut(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	Fl_Text_Editor::kf_cut(0, editor);
	return aTRUE;
}

Atanor* Atanoreditor::MethodCopy(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	if (callfunc->Size() == 0)
		Fl_Text_Editor::kf_copy(0, editor);
	else {
		string tocopy = callfunc->Evaluate(0, contextualpattern, idthread)->String();
		Fl::copy(STR(tocopy), tocopy.length(), 1);
	}
	return aTRUE;
}

Atanor* Atanoreditor::MethodPaste(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	Fl_Text_Editor::kf_paste(0, editor);
	return aTRUE;
}

Atanor* Atanoreditor::MethodSelect(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	if (callfunc->Size() == 0) {
		char* str = textbuf->selection_text();
		Atanor* ke = globalAtanor->Providestring(str);
		free(str);
		return ke;
	}
	int posfirst = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	int posend = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
	textbuf->select(posfirst, posend);
	return aTRUE;
}

Atanor* Atanoreditor::MethodLine(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	int pos;
	Atanor* ke;
	if (callfunc->Size() == 0)
		pos = editor->insert_position();
	else
		pos = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	if (contextualpattern->Type() == a_int) {
		pos = editor->Line(pos);
		return globalAtanor->Provideint(pos);
	}
	else
	if (contextualpattern->Type() == a_float) {
		pos = editor->Line(pos);
		return globalAtanor->Providefloat(pos);
	}
	//Else we extract the string
	int start = editor->line_start(pos);
	int end = editor->line_end(start, 1);
	ke = globalAtanor->Providestring(textbuf->subrange(start, end));
	return ke;
}

Atanor* Atanoreditor::MethodLineBoundaries(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	int pos;
	if (callfunc->Size() == 0)
		pos = editor->insert_position();
	else
		pos = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	Atanor* kvect = Selectaivector(contextualpattern);
	//Else we extract the string
	int start = editor->line_start(pos);
	int end = editor->line_end(start, 1);
	kvect->storevalue((long)start);
	kvect->storevalue((long)end);
	return kvect;
}

Atanor* Atanoreditor::MethodLineBoundariesChar(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	int pos;
	if (callfunc->Size() == 0)
		pos = editor->insert_position();
	else
		pos = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	Atanor* kvect = Selectaivector(contextualpattern);
	//Else we extract the string
	int start = editor->line_start(pos);
	int end = editor->line_end(start, 1);
	uchar* value = textbuf->internalbuffer();
	//the position is in bytes
	//we convert it into characters
	int nb = 0, i = 0;
	start = c_bytetocharpositionidx(value, start, nb, i);
	end = c_bytetocharpositionidx(value, end, nb, i);
	kvect->storevalue((long)start);
	kvect->storevalue((long)end);
	return kvect;
}

Atanor* Atanoreditor::MethodCursor(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	//we need:
	if (callfunc->Size() == 0)
		return globalAtanor->Provideint(editor->insert_position());
	int end = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	if (end<0)
		end = 1;
	else
	if (end>textbuf->length())
		end = textbuf->length();
	editor->insert_position(end);
	editor->show_insert_position();
	return aNULL;
}

Atanor* Atanoreditor::MethodCursorChar(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	uchar* value = textbuf->internalbuffer();
	//the position is in bytes
	int pos = editor->insert_position();
	//we convert it into characters
	pos = c_bytetocharposition(value, pos);
	return globalAtanor->Provideint(pos);
}

Atanor* Atanoreditor::MethodGotoLine(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	bool hg = callfunc->Evaluate(1, contextualpattern, idthread)->Boolean();
	//0 is the first parameter and so on...
	long line = ke->Integer();
	int pos = 0;
	int end;
	if (line > 1) {
		int ret = 1;
		int nb = 1;
		while (ret != 0 && nb < line) {
			ret = textbuf->search_forward(pos, "\n", &pos, 0);
			pos++;
			nb++;
		}
		editor->scroll(line, 0);
	}
	if (hg) {
		end = pos;
		if (textbuf->search_forward(end, "\n", &end, 0) == 1)
			textbuf->highlight(pos, end);
	}
	end = editor->Line(end);
	editor->insert_position(pos);
	editor->show_insert_position();
	return globalAtanor->Provideint(pos);
}

Atanor* Atanoreditor::MethodAnnotate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	if (styletable == NULL)
		return globalAtanor->Returnerror("WND(679): Style table not initialized", idthread);
	//we need:
	Atanor* kfind = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* kstyle = callfunc->Evaluate(1, contextualpattern, idthread);
	int matchcase = 0;
	if (callfunc->Size() == 3)
		matchcase = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
	string stylekey = kstyle->String();
	if (stylekeys.find(stylekey) == stylekeys.end())
		return globalAtanor->Returnerror("WND(679): Unknown style key", idthread);
	unsigned char stylecode = stylekeys[stylekey][0];
	string sfind;
	string s;
	char* buf;
	char* fbuf;
	bool tofree = false;
	if (matchcase == 1) {
		fbuf = textbuf->text();
		buf = fbuf;
	}
	else {
		if (editor->modified || lowercase == "") {
			fbuf = textbuf->text();
			c_to_lower(lowercase, (uchar*)fbuf, textbuf->length());
			editor->modified = false;
		}
		buf = STR(lowercase);
	}
	if (kfind->isVectorContainer()) {
		Atanor* kvect = kfind;
		for (int i = 0; i < kvect->Size(); i++) {
			sfind = kvect->getvalue(i)->String();
			if (!matchcase) {
				s = sfind;
				c_to_lower(sfind, USTR(s), s.size());
			}
			Annotate(idthread, buf, STR(sfind), sfind.size(), stylecode, matchcase);
		}
	}
	else {
		sfind = kfind->String();
		if (!matchcase) {
			s = sfind;
			c_to_lower(sfind, USTR(s), s.size());
		}
		Annotate(idthread, buf, STR(sfind), sfind.size(), stylecode, matchcase);
	}
	if (tofree)
		free(fbuf);
	return aTRUE;
}

Atanor* Atanoreditor::MethodWord(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	//first we get our position
	long pos = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	//then we try to extract the word string
	int start = editor->word_start(pos);
	int end = editor->word_end(pos);
	Atanorstring* ke = globalAtanor->Providestring(textbuf->subrange(start, end));
	return ke;
}

Atanor* Atanoreditor::MethodWrap(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	bool mode = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
	if (mode)
		editor->wrap_mode(3, -1);
	else
		editor->wrap_mode(0, -1);
	return aTRUE;
}

Atanor* Atanoreditor::MethodAnnotateXIPRgx(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	if (styletable == NULL)
		return globalAtanor->Returnerror("WND(679): Style table not initialized", idthread);
	//we need:
	Atanor* krgx = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* kstyle = callfunc->Evaluate(1, contextualpattern, idthread);
	string stylekey = kstyle->String();
	if (stylekeys.find(stylekey) == stylekeys.end())
		return globalAtanor->Returnerror("WND(679): Unknown style key", idthread);
	unsigned char stylecode = stylekeys[stylekey][0];
	string reg = krgx->String();
	inittableutf8();
	automate a(USTR(reg), (void*)1);
	VECTA<string*> vectr;
	char* ltxt = textbuf->text();
	a.rechercheavecextractiondansvecteur((uchar*)ltxt, vectr);
	char* ret = ltxt;
	int i;
	for (int iv = 0; iv < vectr.dernier; iv++) {
		ret = strstr(ret, (char*)vectr[iv]->c_str());
		i = ret - ltxt;
		AddStyle(idthread, i, i + vectr[iv]->size(), stylecode);
		ret++;
	}
	vectr.nettoie();
	free(ltxt);
	return aTRUE;
}

Atanor* Atanoreditor::MethodTextMeasure(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (editor == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	Atanor* t = callfunc->Evaluate(0, contextualpattern, idthread);
	string buf = t->String();
	int w;
	int h;
	fl_measure(buf.c_str(), w, h, 1);
	Atanormapsi* map = new Atanormapsi;
	map->values["W"] = w;
	map->values["H"] = h;
	return map;
}

Atanor* Atanoreditor::MethodInsert(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	//we need:
	string str = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	long pos = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
	if (pos >= 0 && pos < textbuf->length()) {
		textbuf->insert(pos, STR(str));
		return aTRUE;
	}
	return aFALSE;
}

Atanor* Atanoreditor::MethodAppend(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (editor == NULL)
		return globalAtanor->Returnerror("WND(676): Editor not initialized", idthread);
	int start = editor->insert_position();
	string str = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	textbuf->append(STR(str));
	editor->redisplay_range(start, start + str.size());
	return aTRUE;
}

Atanor* Atanoreditor::MethodBytePosition(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	uchar* value = textbuf->internalbuffer();
	long i = callfunc->Evaluate(0, aNULL, idthread)->Integer();
	i = c_chartobyteposition(value, i);
	return globalAtanor->Provideint(i);
}

Atanor* Atanoreditor::MethodCharacterPosition(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	uchar* value = textbuf->internalbuffer();
	long i = callfunc->Evaluate(0, aNULL, idthread)->Integer();
	i = c_bytetocharposition(value, i);
	return globalAtanor->Provideint(i);
}

void AtanorTextEditor::loc_v_scrollbar_cb(Fl_Scrollbar* b, AtanorTextEditor* textD) {
	if (textD->functionvscroll != NULL) {
		Atanoreditor* input = (Atanoreditor*)textD->editor;
		Atanor* func = textD->functionvscroll;
		AtanorCallFunction kfunc(func);

		input->Setreference();
		textD->objectvscroll->Setreference();
		kfunc.arguments.push_back(input);
		kfunc.arguments.push_back(textD->objectvscroll);

		kfunc.Get(aNULL, aNULL, globalAtanor->GetThreadid());

		input->Resetreference();
		textD->objectvscroll->Resetreference();
	}
	v_scrollbar_cb(b, textD);
}

void AtanorTextEditor::loc_h_scrollbar_cb(Fl_Scrollbar* b, AtanorTextEditor* textD) {
	if (textD->functionhscroll != NULL) {
		Atanoreditor* input = (Atanoreditor*)textD->editor;
		Atanor* func = textD->functionhscroll;
		AtanorCallFunction kfunc(func);

		input->Setreference();
		textD->objecthscroll->Setreference();
		kfunc.arguments.push_back(input);
		kfunc.arguments.push_back(textD->objecthscroll);

		kfunc.Get(aNULL, aNULL, globalAtanor->GetThreadid());

		input->Resetreference();
		textD->objecthscroll->Resetreference();
	}
	h_scrollbar_cb(b, textD);
}

int AtanorTextEditor::handle(int e) {
	switch (e) {
	case FL_PUSH:
	case FL_RELEASE:
	case FL_DRAG:
	case FL_MOVE:
	case FL_MOUSEWHEEL:
		if (mouse_callback(e) == 1)
			return 1;
		break;
	case FL_KEYDOWN:
	case FL_KEYUP:
		if (key_callback(e) == 1)
			return 1;
		break;
	}
	return Fl_Text_Editor::handle(e);
}

int AtanorTextEditor::mouse_callback(int e) {
	static bool encours = false;

	if (encours == true)
		return false;

	encours = true;
	int x, y, xroot, yroot, pos = -1;
	if (editor->messagestyle.size() != 0) {
		x = Fl::event_x();
		y = Fl::event_y();
		xroot = Fl::event_x_root();
		yroot = Fl::event_y_root();
		pos = editor->editor->xy_to_position(x, y);
		//in this case, we might display a little sticky note
		char c[] = { 0, 0 };
		c[0] = editor->stylebuf->byte_at(pos);
		if (editor->messagestyle.find(c) != editor->messagestyle.end()) {
			if (editor->sticky == NULL) {
				int w, h;
				fl_measure(STR(editor->messagestyle[c]), w, h, 1);
				editor->sticky = new Fl_Double_Window(xroot, yroot, w + 30, h + 12);
				editor->sticky->border(0);
				editor->sticky->color(editor->messagecolor[c]);
				Fl_Box* bbox = new Fl_Box(10, 5, w + 5, h + 7, STR(editor->messagestyle[c]));
				editor->sticky->end();
				editor->sticky->show();
			}
		}
		else {
			if (editor->sticky != NULL) {
				Fl_Double_Window* wnd = editor->sticky;
				editor->sticky = NULL;
				delete wnd;
			}
		}
	}

	if (functionmouse.find(e) == functionmouse.end()) {
		encours = false;
		return 0;
	}

	if (pos == -1) {
		x = Fl::event_x();
		y = Fl::event_y();
		xroot = Fl::event_x_root();
		yroot = Fl::event_y_root();
		pos = editor->editor->xy_to_position(x, y);
	}
	Atanor* func = functionmouse[e].Function();
	AtanorCallFunction kfunc(func);


	Atanormapsi* kmouse = new Atanormapsi;
	kmouse->values["button"] = Fl::event_button();
	kmouse->values["wheelx"] = Fl::event_dx();
	kmouse->values["wheely"] = Fl::event_dy();
	kmouse->values["x"] = x;
	kmouse->values["y"] = y;
	kmouse->values["xroot"] = xroot;
	kmouse->values["yroot"] = yroot;
	kmouse->values["cursor"] = pos;

	editor->Setreference();
	objectmouse[e].Set();
	kmouse->Setreference();
	kfunc.arguments.push_back(editor);
	kfunc.arguments.push_back(kmouse);
	kfunc.arguments.push_back(objectmouse[e].Object());
	Atanor* ret = kfunc.Get(aNULL, aNULL, globalAtanor->GetThreadid());
	editor->Resetreference();
	objectmouse[e].Reset();
	kmouse->Resetreference();

	encours = false;
	return ret->Boolean();
}

int AtanorTextEditor::key_callback(int e) {
	if (functionkey.find(e) == functionkey.end())
		return 0;

	Atanor* func = functionkey[e].Function();
	AtanorCallFunction kfunc(func);

	editor->Setreference();
	objectkey[e].Set();
	Atanor* ks = globalAtanor->Providestring(Fl::event_text());
	Atanor* ki = globalAtanor->Provideint(Fl::event_key());
	Atanorint* kcombine = globalAtanor->Provideint(0);
	if (Fl::event_ctrl())
		kcombine->value |= 1;
	if (Fl::event_alt())
		kcombine->value |= 2;
	if (Fl::event_command())
		kcombine->value |= 4;
	if (Fl::event_shift())
		kcombine->value |= 8;

	kcombine->Setreference();
	ks->Setreference();
	ki->Setreference();
	kfunc.arguments.push_back(editor);
	kfunc.arguments.push_back(ks);
	kfunc.arguments.push_back(ki);
	kfunc.arguments.push_back(kcombine);
	kfunc.arguments.push_back(objectkey[e].Object());
	Atanor* ret = kfunc.Get(aNULL, aNULL, globalAtanor->GetThreadid());

	editor->Resetreference();
	objectkey[e].Reset();
	ki->Resetreference();
	ks->Resetreference();
	kcombine->Resetreference();
	return ret->Boolean();
}





