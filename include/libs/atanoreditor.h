
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
filename   : atanoreditor.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanoreditor_h
#define atanoreditor_h

#include "atanorwidget.h"

class AtanorTextEditor;
struct Stylearg {
	AtanorTextEditor* editor;
	Fl_Text_Buffer *stylebuf;
	char currentstyle;
};


class AtanorTextBuf : public Fl_Text_Buffer {
public:
	char* wbuffer;
	int internalsize;

	AtanorTextBuf() : Fl_Text_Buffer() {
		internalsize = 1024;
		wbuffer = new char[1024];
		canUndo(1);
	}

	~AtanorTextBuf() {
		delete[] wbuffer;
	}

	void setstyle(int start, int end, uchar style) {
		int l = end - start;
		if (l >= internalsize) {
			delete[] wbuffer;
			internalsize = l + 100;
			wbuffer = new char[internalsize];
		}
		memset(wbuffer, style, l);
		wbuffer[l] = 0;
		replace(start, end, wbuffer);
	}

	char getstyle(int pos) {
		if (pos < 0 || pos >= length())
			return 'A';
		return byte_at(pos);
	}

	unsigned char* internalbuffer() {
		if (internalsize < length()) {
			delete[] wbuffer;
			internalsize = length() + 100;
			wbuffer = new char[internalsize];
		}
		char* t = text();
		strcpy(wbuffer, t);
		free(t);
		return (uchar*)wbuffer;
	}

	string subrange(int s, int e) {
		if (s > length())
			return "";
		string code;
		char* str = text_range(s, e);
		code = str;
		free(str);
		return code;
	}
};

class Atanoreditor;
class AtanorTextEditor : public Fl_Text_Editor {
public:
	bool modified;
	Atanor* functionvscroll;
	Atanor* objectvscroll;
	Atanor* functionhscroll;
	Atanor* objecthscroll;
	Atanoreditor* editor;
	map<int, Idcouple> functionmouse;
	map<int, Idcouple> functionkey;
	map<int, Idcouple> objectmouse;
	map<int, Idcouple> objectkey;

	static void loc_v_scrollbar_cb(Fl_Scrollbar* b, AtanorTextEditor* textD);
	static void loc_h_scrollbar_cb(Fl_Scrollbar* b, AtanorTextEditor* textD);

	AtanorTextEditor(Atanoreditor* e, int x, int y, int w, int h, const char* txt) : Fl_Text_Editor(x, y, w, h, txt) {
		modified = true;
		editor = e;
		mVScrollBar->callback((Fl_Callback*)loc_v_scrollbar_cb, this);
		mHScrollBar->callback((Fl_Callback*)loc_h_scrollbar_cb, this);
		functionvscroll = NULL;
		objectvscroll = aNULL;
		functionhscroll = NULL;
		objecthscroll = aNULL;
	}


	int Firstchar() {
		return mFirstChar;
	}

	int Line(int pos) {
		int linenum = -1;
		position_to_line(pos, &linenum);
		linenum += get_absolute_top_line_number();
		return linenum;
	}

	int Topline() {
		return mTopLineNum;
	}

	int PositionFromLine(Fl_Text_Buffer* buf, int maxLines) {
		int retPos;
		int retLines;
		int retLineStart;
		int retLineEnd;
		wrapped_line_counter(buf, 0, buf->length(), maxLines, true, 0, &retPos, &retLines, &retLineStart, &retLineEnd, true);
		return retPos;
	}

	int mouse_callback(int e);
	int key_callback(int e);

	bool Setmouseaction(int e, Atanor* f, Atanor* object);
	bool Setkeyaction(int e, Atanor* f, Atanor* object);

	int handle(int e);
	void draw_cursor(int X, int Y);
};

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanoreditor;
//This typedef defines a type "editorMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanoreditor::*editorMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanoreditor : public Atanorwidget {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static hmap<short, editorMethod> methods;
	static hmap<string, string> infomethods;
	static bin_hash<unsigned long> exported;
	static bin_hash<bool> nothreads;

	static short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	AtanorTextEditor* editor;
	

	string buf;
	int nbstyle;
	AtanorTextBuf* textbuf;
	AtanorTextBuf* stylebuf;
	string lowercase;
	struct Stylearg stylearg;
	map<string, string> stylekeys;
	map<string, string> keystyles;
	map<string, string> messagestyle;
	map<string, int> messagecolor;
	Fl_Text_Display::Style_Table_Entry* styletable;
	Fl_Double_Window* sticky;
	char txt[1024];

	Fl_Widget* Widget() {
		return editor;
	}
	//---------------------------------------------------------------------------------------------------------------------
	Atanoreditor(AtanorGlobal* g, Atanor* parent = NULL) : Atanorwidget(g, parent) {
		//Do not forget your variable initialisation
		editor = NULL;
		stylebuf = NULL;
		nbstyle = 0;
		styletable = NULL;
		sticky = NULL;
	}

	//----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* index, Atanor* value, short idthread);
	Atanor* Get(Atanor* context, Atanor* index, short idthread);


	short Type() {
		return Atanoreditor::idtype;
	}

	string Typename() {
		return "editor";
	}

	Atanor* Atom(bool forced = false) {
		return this;
	}

	void itemclose() {
		if (editor != NULL)
			delete editor;
		editor = NULL;
		clean();
	}

	//---------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	bool isDeclared(short n) {
		if (exported.find(n) != exported.end())
			return true;
		return false;
	}

	Atanor* Newinstance(short, Atanor* f = NULL) {
		Atanoreditor* a = new Atanoreditor(globalAtanor);
		a->function = f;
		return a;
	}

	AtanorIteration* Newiteration(bool direction);

	static void AddMethod(AtanorGlobal* g, string name, editorMethod func, unsigned long arity, string infos);

	static bool InitialisationModule(AtanorGlobal* g, string version);


	void Methods(Atanor* v) {
		hmap<string, string>::iterator it;
		for (it = infomethods.begin(); it != infomethods.end(); it++)
			v->storevalue(it->first);
	}

	string Info(string n) {

		if (infomethods.find(n) != infomethods.end())
			return infomethods[n];
		return "Unknown method";
	}



	bool isString() {
		return false;
	}

	bool isNumber() {
		return false;
	}

	bool isContainer() {
		return false;
	}

	virtual bool isValueContainer() {
		return false;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	bool InitializeItemMenu(string ckey, Atanor* kvect, unsigned char* key, int& ikey);
	Atanor* AddStyle(short idthread, long start, long end, uchar stylecode);
	Atanor* MethodCreate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodSelection(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodValue(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodColor(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodFont(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodFontSize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodSetStyle(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodLoad(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodSave(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodReplace(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodReplaceAll(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodFind(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodRFind(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodHighlight(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodUnHighlight(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodCursorStyle(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodOnMouseClick(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodOnKey(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodOnVScroll(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodOnHScroll(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodGetStyle(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodAddStyles(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodDelete(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodCut(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodCopy(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodPaste(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodSelect(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodLine(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodLineBoundaries(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodLineBoundariesChar(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodCursor(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodCursorChar(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodGotoLine(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodAnnotate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodWord(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodWrap(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodAnnotateXIPRgx(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodTextMeasure(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodInsert(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodAppend(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodBytePosition(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodCharacterPosition(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

	void Annotate(short idthread, char* buf, char* sfind, int sz, unsigned char stylecode, int matchcase) {
		if (sz == 0)
			return;

		int i = 0;
		char* ret = strstr(buf, sfind);
		while (ret != NULL) {
			i = ret - buf;
			AddStyle(idthread, i, i + sz, stylecode);
			ret = strstr(++ret, sfind);
		}
	}



	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
	//is returned by the Declaration method.
	Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition		
		if (globalAtanor->Error(idthread))
			return globalAtanor->Errorobject(idthread);

		if (Stopall())
			return aNULL;

		///Specific section to handle threads... One exception the redraw function...
		if (idthread && callfunc->Size() != 0 && !nothreads.check(idname)) {
			//In this case, we do not want to process the method, we postpone its activation
			RecordGUIObject(callfunc, idthread);
			return aNULL;
		}

		return (this->*methods[idname])(contextualpattern, idthread, callfunc);
	}

	void Clear() {
		//To set a variable back to empty

	}

	void Clean() {
		//Your cleaning code
	}

	long Size() {
		return String().size();
	}

	long Integer() {

		return String().size();
	}

	double Float() {

		return String().size();
	}

	string String() {
		return (char*)textbuf->internalbuffer();
	}


	bool Boolean() {
		return true;
	}

};

#endif








