
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
filename   : atanorfltk.cxx
Date       : 2017/04/01
Purpose    : Main FLTK loop
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorwidget.h"
#include "atanorwindow.h"
#include "instructions.h"
#include "atanormapsi.h"
#include "constobjects.h"


void Clearwindows();
void TerminateMainWindow();

#ifdef WIN32
//The two following functions were added to the FLTK library to solve two issues: FlResetTimer and Flresetmainthread
//First issue: the timer window in WINDOWS was not reset after a relaunch (see Fl_win32.cxx)

//Code added to Fl_win32.cxx
//void FlResetTimer() {
//	if (s_TimerWnd)
//		s_TimerWnd = 0;
//}

void FlResetTimer(); //see Atanorwindow

//-------------------------------------------------------------------------

//We launch FLTK in a thread, but in that case the mainthread variable (see Fl_lock.cxx) is no longer set to the proper value
//This function is used to do that...

//Code added to Fl_lock.cxx
//void Flresetmainthread() {
//	main_thread = GetCurrentThreadId();
//}

//void Fltkclearmainthread() {
//	main_thread = 0;
//}

void Flresetmainthread();
void Fltkclearmainthread();

#endif

static bool stopall = false;
static bool firstwait = false;

bool Stopall() {
	return stopall;
}

bool Ready() {
	return firstwait;
}

void Initstopall(bool s) {
	stopall = s;
	firstwait = false;
}

void Setstopall(bool s) {
	stopall = s;
}

void widge::redraw() {
	if (globalAtanor->Checktracker(kw, id)) {
		if (kw->Widget() == wdg)
			wdg->redraw();
	}
}


int fontmax = 16;

map<int, string> ifonts;
map<string, Fl_Color> colors;

map<string, unsigned int> boxTypes;
map<string, unsigned int> fonts;
map<string, unsigned int> buttonShapes;
map<string, unsigned int> buttonAction;
map<string, unsigned int> keyCodes;
map<string, unsigned int> aligns;
map<string, unsigned int> lineshape;
map<string, unsigned int> slidertype;
map<string, unsigned int> actions;
map<string, unsigned int> labeltypes;
map<string, unsigned int> buttontypes;
map<string, unsigned int> cursors;

map<string, unsigned long> commands;

//---------------------------------------------------------------------------
void fltkinitialisationtype() {
	static bool init = false;
	if (init == false) {
		colors["FOREGROUND_COLOR"] = FL_FOREGROUND_COLOR;
		colors["BACKGROUND2_COLOR"] = FL_BACKGROUND2_COLOR;
		colors["BACKGROUND_COLOR"] = FL_BACKGROUND_COLOR;
		colors["INACTIVE_COLOR"] = FL_INACTIVE_COLOR;
		colors["SELECTION_COLOR"] = FL_SELECTION_COLOR;
		colors["GRAY0"] = FL_GRAY0;
		colors["DARK3"] = FL_DARK3;
		colors["DARK2"] = FL_DARK2;
		colors["DARK1"] = FL_DARK1;
		colors["LIGHT1"] = FL_LIGHT1;
		colors["LIGHT2"] = FL_LIGHT2;
		colors["LIGHT3"] = FL_LIGHT3;
		colors["BLACK"] = FL_BLACK;
		colors["RED"] = FL_RED;
		colors["GREEN"] = FL_GREEN;
		colors["YELLOW"] = FL_YELLOW;
		colors["BLUE"] = FL_BLUE;
		colors["MAGENTA"] = FL_MAGENTA;
		colors["CYAN"] = FL_CYAN;
		colors["DARK_RED"] = FL_DARK_RED;
		colors["DARK_GREEN"] = FL_DARK_GREEN;
		colors["DARK_YELLOW"] = FL_DARK_YELLOW;
		colors["DARK_BLUE"] = FL_DARK_BLUE;
		colors["DARK_MAGENTA"] = FL_DARK_MAGENTA;
		colors["DARK_CYAN"] = FL_DARK_CYAN;
		colors["WHITE"] = FL_WHITE;

		buttontypes["Regular"] = 0;
		buttontypes["Check"] = 1;
		buttontypes["Light"] = 2;
		buttontypes["Repeat"] = 3;
		buttontypes["Return"] = 4;
		buttontypes["Round"] = 5;
		buttontypes["Image"] = 6;


		actions["PUSH"] = FL_PUSH;
		actions["RELEASE"] = FL_RELEASE;
		actions["DRAG"] = FL_DRAG;
		actions["MOVE"] = FL_MOVE;
		actions["MOUSEWHEEL"] = FL_MOUSEWHEEL;
		actions["KEYDOWN"] = FL_KEYDOWN;
		actions["KEYUP"] = FL_KEYUP;

		slidertype["VERT_SLIDER"] = FL_VERT_SLIDER;
		slidertype["HOR_SLIDER"] = FL_HOR_SLIDER;
		slidertype["VERT_FILL_SLIDER"] = FL_VERT_FILL_SLIDER;
		slidertype["HOR_FILL_SLIDER"] = FL_HOR_FILL_SLIDER;
		slidertype["VERT_NICE_SLIDER"] = FL_VERT_NICE_SLIDER;
		slidertype["HOR_NICE_SLIDER"] = FL_HOR_NICE_SLIDER;

		lineshape["SOLID"] = FL_SOLID;
		lineshape["DASH"] = FL_DASH;
		lineshape["DOT"] = FL_DOT;
		lineshape["DASHDOT"] = FL_DASHDOT;
		lineshape["DASHDOTDOT"] = FL_DASHDOTDOT;
		lineshape["CAP_FLAT"] = FL_CAP_FLAT;
		lineshape["CAP_ROUND"] = FL_CAP_ROUND;
		lineshape["CAP_SQUARE"] = FL_CAP_SQUARE;
		lineshape["JOIN_MITER"] = FL_JOIN_MITER;
		lineshape["JOIN_ROUND"] = FL_JOIN_ROUND;
		lineshape["JOIN_BEVEL"] = FL_JOIN_BEVEL;

		aligns["ALIGN_CENTER"] = FL_ALIGN_CENTER;
		aligns["ALIGN_TOP"] = FL_ALIGN_TOP;
		aligns["ALIGN_BOTTOM"] = FL_ALIGN_BOTTOM;
		aligns["ALIGN_LEFT"] = FL_ALIGN_LEFT;
		aligns["ALIGN_RIGHT"] = FL_ALIGN_RIGHT;
		aligns["ALIGN_INSIDE"] = FL_ALIGN_INSIDE;
		aligns["ALIGN_TEXT_OVER_IMAGE"] = FL_ALIGN_TEXT_OVER_IMAGE;
		aligns["ALIGN_IMAGE_OVER_TEXT"] = FL_ALIGN_IMAGE_OVER_TEXT;
		aligns["ALIGN_CLIP"] = FL_ALIGN_CLIP;
		aligns["ALIGN_WRAP"] = FL_ALIGN_WRAP;
		aligns["ALIGN_IMAGE_NEXT_TO_TEXT"] = FL_ALIGN_IMAGE_NEXT_TO_TEXT;
		aligns["ALIGN_TEXT_NEXT_TO_IMAGE"] = FL_ALIGN_TEXT_NEXT_TO_IMAGE;
		aligns["ALIGN_IMAGE_BACKDROP"] = FL_ALIGN_IMAGE_BACKDROP;
		aligns["ALIGN_TOP_LEFT"] = FL_ALIGN_TOP_LEFT;
		aligns["ALIGN_TOP_RIGHT"] = FL_ALIGN_TOP_RIGHT;
		aligns["ALIGN_BOTTOM_LEFT"] = FL_ALIGN_BOTTOM_LEFT;
		aligns["ALIGN_BOTTOM_RIGHT"] = FL_ALIGN_BOTTOM_RIGHT;
		aligns["ALIGN_LEFT_TOP"] = FL_ALIGN_LEFT_TOP;
		aligns["ALIGN_RIGHT_TOP"] = FL_ALIGN_RIGHT_TOP;
		aligns["ALIGN_LEFT_BOTTOM"] = FL_ALIGN_LEFT_BOTTOM;
		aligns["ALIGN_RIGHT_BOTTOM"] = FL_ALIGN_RIGHT_BOTTOM;
		aligns["ALIGN_NOWRAP"] = FL_ALIGN_NOWRAP;
		aligns["ALIGN_POSITION_MASK"] = FL_ALIGN_POSITION_MASK;
		aligns["ALIGN_IMAGE_MASK"] = FL_ALIGN_IMAGE_MASK;

		labeltypes["NORMAL_LABEL"] = FL_NORMAL_LABEL;
		labeltypes["NO_LABEL"] = FL_NO_LABEL;
		labeltypes["SHADOW_LABEL"] = _FL_SHADOW_LABEL;
		labeltypes["ENGRAVED_LABEL"] = _FL_ENGRAVED_LABEL;
		labeltypes["EMBOSSED_LABEL"] = _FL_EMBOSSED_LABEL;

		cursors["CURSOR_DEFAULT"] = FL_CURSOR_DEFAULT;
		cursors["CURSOR_ARROW"] = FL_CURSOR_ARROW;
		cursors["CURSOR_CROSS"] = FL_CURSOR_CROSS;
		cursors["CURSOR_WAIT"] = FL_CURSOR_WAIT;
		cursors["CURSOR_INSERT"] = FL_CURSOR_INSERT;
		cursors["CURSOR_HAND"] = FL_CURSOR_HAND;
		cursors["CURSOR_HELP"] = FL_CURSOR_HELP;
		cursors["CURSOR_MOVE"] = FL_CURSOR_MOVE;
		cursors["CURSOR_NS"] = FL_CURSOR_NS;
		cursors["CURSOR_WE"] = FL_CURSOR_WE;
		cursors["CURSOR_NWSE"] = FL_CURSOR_NWSE;
		cursors["CURSOR_NESW"] = FL_CURSOR_NESW;
		cursors["CURSOR_NONE"] = FL_CURSOR_NONE;
		cursors["CURSOR_N"] = FL_CURSOR_N;
		cursors["CURSOR_NE"] = FL_CURSOR_NE;
		cursors["CURSOR_E"] = FL_CURSOR_E;
		cursors["CURSOR_SE"] = FL_CURSOR_SE;
		cursors["CURSOR_S"] = FL_CURSOR_S;
		cursors["CURSOR_SW"] = FL_CURSOR_SW;
		cursors["CURSOR_W"] = FL_CURSOR_W;
		cursors["CURSOR_NW"] = FL_CURSOR_NW;

		cursors["NORMAL_CURSOR"] = Fl_Text_Display::NORMAL_CURSOR;
		cursors["CARET_CURSOR"] = Fl_Text_Display::CARET_CURSOR;
		cursors["DIM_CURSOR"] = Fl_Text_Display::DIM_CURSOR;
		cursors["BLOCK_CURSOR"] = Fl_Text_Display::BLOCK_CURSOR;
		cursors["HEAVY_CURSOR"] = Fl_Text_Display::HEAVY_CURSOR;
		cursors["LINE_CURSOR"] = 5;

		commands["SHIFT"] = FL_SHIFT;
		commands["CAPS_LOCK"] = FL_CAPS_LOCK;
		commands["CTRL"] = FL_CTRL;
		commands["ALT"] = FL_ALT;
		commands["NUM_LOCK"] = FL_NUM_LOCK;
		commands["META"] = FL_META;
		commands["SCROLL_LOCK"] = FL_SCROLL_LOCK;
		commands["BUTTON1"] = FL_BUTTON1;
		commands["BUTTON2"] = FL_BUTTON2;
		commands["BUTTON3"] = FL_BUTTON3;
		commands["BUTTONS"] = FL_BUTTONS;
		commands["KEY_MASK"] = FL_KEY_MASK;
		commands["COMMAND"] = FL_COMMAND;
		commands["CONTROL"] = FL_CONTROL;

		keyCodes["Button"] = 0xfee8;
		keyCodes["BackSpace"] = 0xff08;
		keyCodes["Tab"] = 0xff09;
		keyCodes["Enter"] = 0xff0d;
		keyCodes["Pause"] = 0xff13;
		keyCodes["Scroll_Lock"] = 0xff14;
		keyCodes["Escape"] = 0xff1b;
		keyCodes["Home"] = 0xff50;
		keyCodes["Left"] = 0xff51;
		keyCodes["Up"] = 0xff52;
		keyCodes["Right"] = 0xff53;
		keyCodes["Down"] = 0xff54;
		keyCodes["Page_Up"] = 0xff55;
		keyCodes["Page_Down"] = 0xff56;
		keyCodes["End"] = 0xff57;
		keyCodes["Print"] = 0xff61;
		keyCodes["Insert"] = 0xff63;
		keyCodes["Menu"] = 0xff67;
		keyCodes["Help"] = 0xff68;
		keyCodes["Num_Lock"] = 0xff7f;
		keyCodes["KP"] = 0xff80;
		keyCodes["KP_Enter"] = 0xff8d;
		keyCodes["KP_Last"] = 0xffbd;
		keyCodes["F"] = 0xffbd;
		keyCodes["F_Last"] = 0xffe0;
		keyCodes["Shift_L"] = 0xffe1;
		keyCodes["Shift_R"] = 0xffe2;
		keyCodes["Control_L"] = 0xffe3;
		keyCodes["Control_R"] = 0xffe4;
		keyCodes["Caps_Lock"] = 0xffe5;
		keyCodes["Meta_L"] = 0xffe7;
		keyCodes["Meta_R"] = 0xffe8;
		keyCodes["Alt_L"] = 0xffe9;
		keyCodes["Alt_R"] = 0xffea;
		keyCodes["Delete"] = 0xffff;

		boxTypes["NO_BOX"] = 0;
		boxTypes["FLAT_BOX"] = 1;
		boxTypes["UP_BOX"] = 2;
		boxTypes["DOWN_BOX"] = 3;
		boxTypes["UP_FRAME"] = 4;
		boxTypes["DOWN_FRAME"] = 5;
		boxTypes["THIN_UP_BOX"] = 6;
		boxTypes["THIN_DOWN_BOX"] = 7;
		boxTypes["THIN_UP_FRAME"] = 8;
		boxTypes["THIN_DOWN_FRAME"] = 9;
		boxTypes["ENGRAVED_BOX"] = 10;
		boxTypes["EMBOSSED_BOX"] = 11;
		boxTypes["ENGRAVED_FRAME"] = 12;
		boxTypes["EMBOSSED_FRAME"] = 13;
		boxTypes["BORDER_BOX"] = 14;
		boxTypes["SHADOW_BOX"] = 15;
		boxTypes["BORDER_FRAME"] = 16;
		boxTypes["SHADOW_FRAME"] = 17;
		boxTypes["ROUNDED_BOX"] = 18;
		boxTypes["RSHADOW_BOX"] = 19;
		boxTypes["ROUNDED_FRAME"] = 20;
		boxTypes["RFLAT_BOX"] = 21;
		boxTypes["ROUND_UP_BOX"] = 22;
		boxTypes["ROUND_DOWN_BOX"] = 23;
		boxTypes["DIAMOND_UP_BOX"] = 24;
		boxTypes["DIAMOND_DOWN_BOX"] = 25;
		boxTypes["OVAL_BOX"] = 26;
		boxTypes["OSHADOW_BOX"] = 27;
		boxTypes["OVAL_FRAME"] = 28;
		boxTypes["OFLAT_BOX"] = 29;
		boxTypes["PLASTIC_UP_BOX"] = 30;
		boxTypes["PLASTIC_DOWN_BOX"] = 31;
		boxTypes["PLASTIC_UP_FRAME"] = 32;
		boxTypes["PLASTIC_DOWN_FRAME"] = 33;
		boxTypes["PLASTIC_THIN_UP_BOX"] = 34;
		boxTypes["PLASTIC_THIN_DOWN_BOX"] = 35;
		boxTypes["PLASTIC_ROUND_UP_BOX"] = 36;
		boxTypes["PLASTIC_ROUND_DOWN_BOX"] = 37;
		boxTypes["GTK_UP_BOX"] = 38;
		boxTypes["GTK_DOWN_BOX"] = 39;
		boxTypes["GTK_UP_FRAME"] = 40;
		boxTypes["GTK_DOWN_FRAME"] = 41;
		boxTypes["GTK_THIN_UP_BOX"] = 42;
		boxTypes["GTK_THIN_DOWN_BOX"] = 43;
		boxTypes["GTK_THIN_UP_FRAME"] = 44;
		boxTypes["GTK_THIN_DOWN_FRAME"] = 45;
		boxTypes["GTK_ROUND_UP_BOX"] = 46;
		boxTypes["GTK_ROUND_DOWN_BOX"] = 47;
		boxTypes["FREE_BOXTYPE"] = 48;

		fonts["HELVETICA"] = 0;
		fonts["HELVETICA_BOLD"] = 1;
		fonts["HELVETICA_ITALIC"] = 2;
		fonts["HELVETICA_BOLD_ITALIC"] = 3;
		fonts["COURIER"] = 4;
		fonts["COURIER_BOLD"] = 5;
		fonts["COURIER_ITALIC"] = 6;
		fonts["COURIER_BOLD_ITALIC"] = 7;
		fonts["TIMES"] = 8;
		fonts["TIMES_BOLD"] = 9;
		fonts["TIMES_ITALIC"] = 10;
		fonts["TIMES_BOLD_ITALIC"] = 11;
		fonts["SYMBOL"] = 12;
		fonts["SCREEN"] = 13;
		fonts["SCREEN_BOLD"] = 14;
		fonts["ZAPF_DINGBATS"] = 15;
		fonts["FREE_FONT"] = 16;
		fonts["BOLD"] = 1;
		fonts["ITALIC"] = 2;
		fonts["BOLD_ITALIC"] = 3;

		ifonts[0] = "HELVETICA";
		ifonts[1] = "HELVETICA_BOLD";
		ifonts[2] = "HELVETICA_ITALIC";
		ifonts[3] = "HELVETICA_BOLD_ITALIC";
		ifonts[4] = "COURIER";
		ifonts[5] = "COURIER_BOLD";
		ifonts[6] = "COURIER_ITALIC";
		ifonts[7] = "COURIER_BOLD_ITALIC";
		ifonts[8] = "TIMES";
		ifonts[9] = "TIMES_BOLD";
		ifonts[10] = "TIMES_ITALIC";
		ifonts[11] = "TIMES_BOLD_ITALIC";
		ifonts[12] = "SYMBOL";
		ifonts[13] = "SCREEN";
		ifonts[14] = "SCREEN_BOLD";
		ifonts[15] = "ZAPF_DINGBATS";
		ifonts[16] = "FREE_FONT";

		buttonShapes["NORMAL_BUTTON"] = FL_NORMAL_BUTTON;
		buttonShapes["TOGGLE_BUTTON"] = FL_TOGGLE_BUTTON;
		buttonShapes["RADIO_BUTTON"] = FL_RADIO_BUTTON;
		buttonShapes["HIDDEN_BUTTON"] = FL_HIDDEN_BUTTON;

		buttonAction["WHEN_NEVER"] = FL_WHEN_NEVER;
		buttonAction["WHEN_CHANGED"] = FL_WHEN_CHANGED;
		buttonAction["WHEN_RELEASE"] = FL_WHEN_RELEASE;
		buttonAction["WHEN_RELEASE_ALWAYS"] = FL_WHEN_RELEASE_ALWAYS;
		buttonAction["WHEN_ENTER_KEY"] = FL_WHEN_ENTER_KEY;
		buttonAction["WHEN_ENTER_KEY_ALWAYS"] = FL_WHEN_ENTER_KEY_ALWAYS;
		init = true;

	}
}
bool fltk_create_variables(AtanorGlobal* global) {

	map<string, unsigned int>::iterator it;
	string name;
	//then we create its value
	Atanor* a;
	string lastname;

	name = "FL_NORMAL_SIZE";
	//If the symbols have already been declared...
	if (global->symbolIds.find(name) != global->symbolIds.end())
		return false;

	global->Createid(name);

	a = new AtanorSystemVariable(global, new AtanorConstInt(FL_NORMAL_SIZE, global), global->Createid(name), a_int);

	for (it = boxTypes.begin(); it != boxTypes.end(); it++) {
		name = "FL_";
		name += it->first;
		a = new AtanorSystemVariable(global, new AtanorConstInt(it->second, global), global->Createid(name), a_int);
	}
	for (it = fonts.begin(); it != fonts.end(); it++) {
		name = "FL_";
		name += it->first;
		a = new AtanorSystemVariable(global, new AtanorConstInt(it->second, global), global->Createid(name), a_int);
	}

	for (it = labeltypes.begin(); it != labeltypes.end(); it++) {
		name = "FL_";
		name += it->first;
		a = new AtanorSystemVariable(global, new AtanorConstInt(it->second, global), global->Createid(name), a_int);
	}

	for (it = buttontypes.begin(); it != buttontypes.end(); it++) {
		name = "FL_";
		name += it->first;
		a = new AtanorSystemVariable(global, new AtanorConstInt(it->second, global), global->Createid(name), a_int);
	}

	for (it = buttonShapes.begin(); it != buttonShapes.end(); it++) {
		name = "FL_";
		name += it->first;
		a = new AtanorSystemVariable(global, new AtanorConstInt(it->second, global), global->Createid(name), a_int);
	}
	for (it = buttonAction.begin(); it != buttonAction.end(); it++) {
		name = "FL_";
		name += it->first;
		a = new AtanorSystemVariable(global, new AtanorConstInt(it->second, global), global->Createid(name), a_int);
	}
	for (it = keyCodes.begin(); it != keyCodes.end(); it++) {
		name = "FL_";
		name += it->first;
		a = new AtanorSystemVariable(global, new AtanorConstInt(it->second, global), global->Createid(name), a_int);
	}
	for (it = cursors.begin(); it != cursors.end(); it++) {
		name = "FL_";
		name += it->first;
		a = new AtanorSystemVariable(global, new AtanorConstInt(it->second, global), global->Createid(name), a_int);
	}
	for (it = aligns.begin(); it != aligns.end(); it++) {
		name = "FL_";
		name += it->first;
		a = new AtanorSystemVariable(global, new AtanorConstInt(it->second, global), global->Createid(name), a_int);
	}
	for (it = lineshape.begin(); it != lineshape.end(); it++) {
		name = "FL_";
		name += it->first;
		a = new AtanorSystemVariable(global, new AtanorConstInt(it->second, global), global->Createid(name), a_int);
	}
	for (it = slidertype.begin(); it != slidertype.end(); it++) {
		name = "FL_";
		name += it->first;
		a = new AtanorSystemVariable(global, new AtanorConstInt(it->second, global), global->Createid(name), a_int);
	}
	for (it = actions.begin(); it != actions.end(); it++) {
		name = "FL_";
		name += it->first;
		a = new AtanorSystemVariable(global, new AtanorConstInt(it->second, global), global->Createid(name), a_int);
	}

	map<string, unsigned long>::iterator its;
	for (its = commands.begin(); its != commands.end(); its++) {
		name = "FL_";
		name += its->first;
		a = new AtanorSystemVariable(global, new AtanorConstInt(its->second, global), global->Createid(name), a_int);
	}
	map<string, Fl_Color>::iterator itx;
	for (itx = colors.begin(); itx != colors.end(); itx++) {
		name = "FL_";
		name += itx->first;
		a = new AtanorSystemVariable(global, new AtanorConstInt(itx->second, global), global->Createid(name), a_int);
	}

	name = "FL_DIR_SINGLE";
	a = new AtanorSystemVariable(global, new AtanorConstInt(0, global), global->Createid(name), a_int);

	name = "FL_DIR_MULTI";
	a = new AtanorSystemVariable(global, new AtanorConstInt(1, global), global->Createid(name), a_int);

	name = "FL_DIR_CREATE";
	a = new AtanorSystemVariable(global, new AtanorConstInt(2, global), global->Createid(name), a_int);

	name = "FL_DIR_DIRECTORY";
	a = new AtanorSystemVariable(global, new AtanorConstInt(4, global), global->Createid(name), a_int);

	//Tree flags
	name = "FL_TREE_SELECT_NONE";
	a = new AtanorSystemVariable(global, new AtanorConstInt(FL_TREE_SELECT_NONE, global), global->Createid(name), a_int);

	name = "FL_TREE_SELECT_SINGLE";
	a = new AtanorSystemVariable(global, new AtanorConstInt(FL_TREE_SELECT_SINGLE, global), global->Createid(name), a_int);

	name = "FL_TREE_SELECT_MULTI";
	a = new AtanorSystemVariable(global, new AtanorConstInt(FL_TREE_SELECT_MULTI, global), global->Createid(name), a_int);

	name = "FL_TREE_SORT_NONE";
	a = new AtanorSystemVariable(global, new AtanorConstInt(FL_TREE_SORT_NONE, global), global->Createid(name), a_int);

	name = "FL_TREE_SORT_ASCENDING";
	a = new AtanorSystemVariable(global, new AtanorConstInt(FL_TREE_SORT_ASCENDING, global), global->Createid(name), a_int);

	name = "FL_TREE_SORT_DESCENDING";
	a = new AtanorSystemVariable(global, new AtanorConstInt(FL_TREE_SORT_DESCENDING, global), global->Createid(name), a_int);

	name = "FL_TREE_CONNECTOR_NONE";
	a = new AtanorSystemVariable(global, new AtanorConstInt(FL_TREE_CONNECTOR_NONE, global), global->Createid(name), a_int);

	name = "FL_TREE_CONNECTOR_DOTTED";
	a = new AtanorSystemVariable(global, new AtanorConstInt(FL_TREE_CONNECTOR_DOTTED, global), global->Createid(name), a_int);

	name = "FL_TREE_CONNECTOR_SOLID";
	a = new AtanorSystemVariable(global, new AtanorConstInt(FL_TREE_CONNECTOR_SOLID, global), global->Createid(name), a_int);

	name = "FL_TREE_REASON_NONE";
	a = new AtanorSystemVariable(global, new AtanorConstInt(FL_TREE_REASON_NONE, global), global->Createid(name), a_int);

	name = "FL_TREE_REASON_SELECTED";
	a = new AtanorSystemVariable(global, new AtanorConstInt(FL_TREE_REASON_SELECTED, global), global->Createid(name), a_int);

	name = "FL_TREE_REASON_DESELECTED";
	a = new AtanorSystemVariable(global, new AtanorConstInt(FL_TREE_REASON_DESELECTED, global), global->Createid(name), a_int);

	name = "FL_TREE_REASON_OPENED";
	a = new AtanorSystemVariable(global, new AtanorConstInt(FL_TREE_REASON_OPENED, global), global->Createid(name), a_int);

	name = "FL_TREE_REASON_CLOSED";
	a = new AtanorSystemVariable(global, new AtanorConstInt(FL_TREE_REASON_CLOSED, global), global->Createid(name), a_int);

	return true;
}

//---------------------------------------------------------------------------
//when in a FLTK environment, we replace kget with a window version
//---------------------------------------------------------------------------
Atanor* ProcEditor(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In this case, we create a window on the fly to read the value
	string label;
	if (callfunc->Size())
		label = callfunc->Evaluate(0, aNULL, idthread)->String();

	
	const char* buff = fl_input(label.c_str());
	if (buff == NULL)
		return aNULL;

	return globalAtanor->Providestring(buff);
}

void ResetGetEditor() {
	globalAtanor->RecordOneProcedure("kget", ProcEditor, P_NONE | P_ONE);
}

//---------------------------------------------------------------------------
static ThreadLock recordingGUI(NULL, false);

class guiObject {
public:
	AtanorCallMethod* call;
	Atanor* object;
	short idobject;

	guiObject(AtanorCallMethod* c, Atanor* o) {
		call = c;
		object = o;
		idobject = globalAtanor->RecordInTrackerProtected(object);
	}

	void Execute(short idthread) {		
		//we check if the object still exists...
		if (globalAtanor->Checktracker(object, idobject)) {
			Atanor* a = call->Get(aNULL, object, idthread);
			a->Setreference();
			call->arguments.push_back(a);
		}
	}

	~guiObject() {
		for (size_t i = 0; i < call->arguments.size(); i++)
			call->arguments[i]->Resetreference();
		call->arguments.clear();
		delete call;
	}
};

static vector<guiObject*> guiObjects;

//This function postpones methods called in a thread...
void Atanorwidget::RecordGUIObject(AtanorCall* callfunc, short idthread) {
	Locking _lock(recordingGUI);
	if (!firstwait)
		return;

	if (callfunc->Name() == redraw_index) {
		MethodRedraw(aNULL, idthread, callfunc);
		return;
	}

	AtanorCallMethod* call = new AtanorCallMethod(callfunc->Name());
	Atanor* a;
	for (size_t i = 0; i < callfunc->Size(); i++) {
		a = callfunc->arguments[i]->Get(aNULL, aNULL, idthread)->Atom();
		a->Setreference();
		call->arguments.push_back(a);
	}

	guiObjects.push_back(new guiObject(call, this));
}


//We use this function to execute objects that were called in a thread...
static void ExecuteGUIObjects(short idthread) {
	Locking _lock(recordingGUI);

	for (size_t i = 0; i < guiObjects.size(); i++) {
		guiObjects[i]->Execute(idthread);
		delete guiObjects[i];
	}
	guiObjects.clear();
}

static void ClearGUIObjects() {
	Locking _lock(recordingGUI);

	for (size_t i = 0; i < guiObjects.size(); i++)
		delete guiObjects[i];

	guiObjects.clear();
}

//---------------------------------------------------------------------------

void Idcouple::Use(Atanor* e) {
	if (e != NULL) {
		o = e;
		id = globalAtanor->RecordInTrackerProtected(o);
		if (e->Type() != Atanorwindow::idtype) {
			count++;
			e->Setreference();
		}
	}
}

Doublewindow::Doublewindow(int x, int y, const char* l, Atanor* f, Atanor* o, Atanor* c, Atanorwindow* wn) : 
ready(false), iwindow(-1), Fl_Double_Window(x, y, l) {
	function = f;
	dragdrop = false;
	object = o;
	window = wn;
	iwindow = globalAtanor->RecordInTrackerProtected(window);
}

;
Doublewindow::Doublewindow(int x, int y, int w, int h, const char* l, Atanor* f, Atanor* o, Atanor* c, Atanorwindow* wn) : 
ready(false), iwindow(-1), Fl_Double_Window(x, y, w, h, l) {
	dragdrop = false;
	function = f;
	object = o;
	window = wn;
	iwindow = globalAtanor->RecordInTrackerProtected(window);
}

Doublewindow::~Doublewindow() {
	if (window != NULL) {
		//already in deletion
		window->itemclose();
		window->window = NULL;
	}
}

bool Doublewindow::Setmouseaction(int e, Atanor* f, Atanor* object) {
	if (!f->isFunction() || f->Size() != 3 || f->Parameter(0)->Typevariable() != Atanorwindow::idtype)
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

bool Doublewindow::Setkeyaction(int e, Atanor* f, Atanor* object) {
	if (!f->isFunction() || f->Size() != 5 || f->Parameter(0)->Typevariable() != Atanorwindow::idtype)
		return false;
	if (e != FL_KEYDOWN && e != FL_KEYUP)
		return false;
	functionkey[e].Use(f);
	objectkey[e].Use(object);
	return true;
}

void Doublewindow::draw() {
	if (stopall)
		return;

	if (globalAtanor->Checktracker(window, iwindow)) {
		if (window->update)
			Fl_Double_Window::draw();
	}
	else
		return;

	if (function != NULL) {
		fl_color(FL_BLACK); //we set FL_BLACK as the default color, it can be modified with drawcolor in the code...
		AtanorCallFunction call(function);
		call.arguments.push_back(window);
		call.arguments.push_back(object);
		Atanor* a = call.Get(aNULL, aNULL, globalAtanor->GetThreadid());
		a->Release();
	}
	ready = true;
}

int Doublewindow::handle(int e) {
	if (stopall || !ready || !globalAtanor->Checktracker(window, iwindow))
		return 1;
	switch (e) {
	case FL_DND_ENTER:
	case FL_DND_RELEASE:
	case FL_DND_LEAVE:
	case FL_DND_DRAG:
		dragdrop = true;
		return 1;
	case FL_PASTE:
		if (dragdrop) {
			eventtext = Fl::event_text();
			dragdropcall_back();
		}
		dragdrop = false;
		return 1;
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
	if (stopall)
		return 1;
	return Fl_Double_Window::handle(e);
}

int Doublewindow::dragdropcall_back() {
	if (functiondragdrop.Function() == NULL)
		return 0;

	static bool encours = false;
	if (encours == true)
		return false;
	encours = true;

	AtanorCallFunction call(functiondragdrop.Function());

	Atanorstring* kstr = globalAtanor->Providestring(eventtext);
	kstr->Setreference();
	Idcouple thewnd(window, true);
	call.arguments.push_back(window);
	call.arguments.push_back(kstr);
	call.arguments.push_back(objectdragdrop.Object());
	Atanor* ret = call.Get(aNULL, aNULL, globalAtanor->GetThreadid());
	kstr->Resetreference();
	thewnd.Reset();
	encours = false;

	return ret->Boolean();
}

int Doublewindow::mouse_callback(int e) {
	if (functionmouse.find(e) == functionmouse.end())
		return 0;

	static bool encours = false;
	if (encours == true)
		return false;
	encours = true;

	Atanormapsi* kmouse = new Atanormapsi;
	kmouse->values["button"] = Fl::event_button();
	kmouse->values["wheelx"] = Fl::event_dx();
	kmouse->values["wheely"] = Fl::event_dy();
	kmouse->values["x"] = Fl::event_x();
	kmouse->values["y"] = Fl::event_y();
	kmouse->values["xroot"] = Fl::event_x_root();
	kmouse->values["yroot"] = Fl::event_y_root();

	AtanorCallFunction call(functionmouse[e].Function());

	Idcouple thewnd(window, true);
	kmouse->Setreference();
	Idcouple themouse(objectmouse[e].o, true);
	call.arguments.push_back(window);
	call.arguments.push_back(kmouse);
	call.arguments.push_back(objectmouse[e].Object());

	Atanor* ret = call.Get(aNULL, aNULL, globalAtanor->GetThreadid());
	thewnd.Reset();
	kmouse->Resetreference();
	themouse.Reset();

	encours = false;
	return ret->Boolean();
}

int Doublewindow::key_callback(int e) {
	if (functionkey.find(e) == functionkey.end())
		return 0;


	AtanorCallFunction call(functionkey[e].Function());
	Idcouple thewnd(window, true);
	Idcouple thekey(objectkey[e].o, true);

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
	call.arguments.push_back(window);
	call.arguments.push_back(ks);
	call.arguments.push_back(ki);
	call.arguments.push_back(kcombine);
	call.arguments.push_back(objectkey[e].Object());
	Atanor* ret = call.Get(aNULL, aNULL, globalAtanor->GetThreadid());

	thewnd.Reset();
	thekey.Reset();
	ki->Resetreference();
	ks->Resetreference();
	kcombine->Resetreference();
	return ret->Boolean();
}

//--------------------------------------------------------------------------------------

#define FOREVER 1e05
#ifdef MACUI
void fltk_reinit() {
	Fl::lock();
	Fl::awake_ring_head_ = 0;
	Fl::awake_ring_tail_ = 0;
	Fl::unlock();
}
#endif

int Fltkrun(short idthread) {
#ifdef MACUI
	fltk_reinit();
#endif


#ifdef WIN32
	Fltkclearmainthread();
#endif

	Fl::lock();

#ifdef WIN32
	Fl::unlock();
	Flresetmainthread();
	Fl::lock();
#endif

	int tt = 1;
	stopall = false;
	firstwait = false;

	ResetGetEditor();

#ifdef UNIX	
	while (!stopall) {
#else
	while (!stopall && tt != 0) {
#endif
		if (globalAtanor->Error(0)) {
			stopall = true;
			TerminateMainWindow();
			break;
		}
		else {
			ExecuteGUIObjects(0);
			tt = Fl::wait(FOREVER);
			firstwait = true;
		}
	}

	Fl::unlock();
	ClearGUIObjects();
	stopall = true;
	firstwait = false;
	return 0;
}






