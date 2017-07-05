
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
filename   : atanorwtable.cxx
Date       : 2017/04/01
Purpose    : table implementation (based on FLTK)
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorwtable.h"
#include "atanorint.h"
#include "atanorstring.h"
#include "instructions.h"
#include "atanormap.h"
#include "atanormapss.h"

//We need to declare once again our local definitions.
hmap<short, wtableMethod>  Atanorwtable::methods;
hmap<string, string> Atanorwtable::infomethods;
bin_hash<unsigned long> Atanorwtable::exported;

short Atanorwtable::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorwtable::AddMethod(AtanorGlobal* global, string name, wtableMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorwtable::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorwtable::idtype = global->Getid("wtable");

	Atanorwtable::AddMethod(global, "_initial", &Atanorwidget::MethodInitial, P_ONE, "_initial(variable): Creation of a window with a call back object");

	Atanorwtable::AddMethod(global, "create", &Atanorwtable::MethodCreate, P_FIVE, "create(int x, int y, int w, int h, string label): Create a table of objects and starts adding");
	Atanorwtable::AddMethod(global, "clear", &Atanorwtable::MethodClear, P_NONE, "clear(): Clear the table");
	Atanorwtable::AddMethod(global, "add", &Atanorwtable::MethodAdd, P_THREE, "add(int R, int C, string v): Add a value on row R and column C");
	Atanorwtable::AddMethod(global, "cell", &Atanorwtable::MethodCell, P_TWO, "cell(int R, int C): Return the value of the cell on row R and column C");
	Atanorwtable::AddMethod(global, "cellalign", &Atanorwtable::MethodCellAlignment, P_ONE, "cellalign(align): Set the inner cell alignement");
	Atanorwtable::AddMethod(global, "cellalignheadercol", &Atanorwtable::MethodCellAlignmentHeaderCol, P_ONE, "cellalignheadercol(align): Set the header column alignement");
	Atanorwtable::AddMethod(global, "cellalignheaderrow", &Atanorwtable::MethodCellAlignmentHeaderRow, P_ONE, "cellalignheaderrow(align): Set the header row alignement");
	Atanorwtable::AddMethod(global, "row", &Atanorwtable::MethodRow, P_ONE | P_NONE, "row()|(int nb): Define the number of rows");
	Atanorwtable::AddMethod(global, "column", &Atanorwtable::MethodColumn, P_ONE | P_NONE, "column()|(int nb): Define the number of columns");
	Atanorwtable::AddMethod(global, "rowheight", &Atanorwtable::MethodRowHeight, P_ONE, "rowheight(int height): Define the row height in pixel");
	Atanorwtable::AddMethod(global, "columnwidth", &Atanorwtable::MethodColumnWidth, P_ONE, "columnwidth(int width): Define the column width in pixel");
	Atanorwtable::AddMethod(global, "font", &Atanorwtable::MethodFont, P_ONE | P_NONE, "font(int s): set or return the text font");
	Atanorwtable::AddMethod(global, "fontsize", &Atanorwtable::MethodFontSize, P_ONE | P_NONE, "fontsize(int c): set or return the text font size");
	Atanorwtable::AddMethod(global, "colorbg", &Atanorwtable::MethodColorbg, P_ONE | P_NONE, "colorbg(int c): set or return the cell color background");
	Atanorwtable::AddMethod(global, "colorfg", &Atanorwtable::MethodColorfg, P_ONE | P_NONE, "colorfg(int c): set or return the cell color foreground");
	Atanorwtable::AddMethod(global, "when", &Atanorwtable::MethodWhen, 65534, "when(string when1 string when2...): Type of event to trigger the callback");
	Atanorwtable::AddMethod(global, "selectioncolor", &Atanorwtable::MethodTableSelectionColor, P_ONE | P_NONE, "colorselection(int color): Color for the selected elements");
	Atanorwtable::AddMethod(global, "boxtype", &Atanorwtable::MethodBoxType, P_ONE, "boxtype(int boxtype): box type");
	Atanorwtable::AddMethod(global, "rowheader", &Atanorwtable::MethodRowHeader, P_TWO, "rowheader(int pos, string label): Set the row header label at row pos");
	Atanorwtable::AddMethod(global, "columnheader", &Atanorwtable::MethodColumnHeader, P_TWO, "columnheader(int pos, string label): Set the column header label at column pos");
	Atanorwtable::AddMethod(global, "rowheaderheight", &Atanorwtable::MethodRowHeaderHeight, P_ONE, "rowheaderheight(int height): the size in pixel of the row header");
	Atanorwtable::AddMethod(global, "columnheaderwidth", &Atanorwtable::MethodColumnHeaderWidth, P_ONE, "columnheaderwidth(int width): the size in pixel of the column header");
	Atanorwtable::AddMethod(global, "tooltip", &Atanorwidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
	Atanorwtable::AddMethod(global, "show", &Atanorwidget::MethodShow, P_NONE, "show(): Show the widget");
	Atanorwtable::AddMethod(global, "focus", &Atanorwidget::MethodFocus, P_NONE, "focus(): Get the focus");
	Atanorwtable::AddMethod(global, "hide", &Atanorwidget::MethodHide, P_NONE, "hide(): Hide the widget");
	Atanorwtable::AddMethod(global, "labelsize", &Atanorwidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
	Atanorwtable::AddMethod(global, "labelfont", &Atanorwidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
	Atanorwtable::AddMethod(global, "labelcolor", &Atanorwidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
	Atanorwtable::AddMethod(global, "labeltype", &Atanorwidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
	Atanorwtable::AddMethod(global, "label", &Atanorwidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
	Atanorwtable::AddMethod(global, "boundaries", &Atanorwidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "boundaries(ivector crds): return a vector of the widget coordinates");
	Atanorwtable::AddMethod(global, "redraw", &Atanorwidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
	Atanorwtable::AddMethod(global, "backgroundcolor", &Atanorwidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
	Atanorwtable::AddMethod(global, "created", &Atanorwidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
	Atanorwtable::AddMethod(global, "copy", &Atanorwidget::MethodCopy, P_ONE, "copy(string): Copy string to clipboard");
	Atanorwtable::AddMethod(global, "paste", &Atanorwidget::MethodPaste, P_NONE, "paste(): paste string");
	Atanorwtable::AddMethod(global, "align", &Atanorwidget::MethodAlign, P_ONE, "align(int): define the label alignment");




	global->newInstance[Atanorwtable::idtype] = new Atanorwtable(global);
	global->RecordMethods(Atanorwtable::idtype, Atanorwtable::exported);

	return true;
}


Atanor* Atanorwtable::Put(Atanor* index, Atanor* value, short idthread) {
	return this;
}

Atanor* Atanorwtable::Get(Atanor* context, Atanor* index, short idthread) {
	return this;
}

AtanorIteration* Atanorwtable::Newiteration(bool direction) {
	return aITERNULL;
}


void TableRow::draw_cell(TableContext context,
	int R, int C, int X, int Y, int W, int H)
{
	string buffer;

	switch (context)
	{
	case CONTEXT_STARTPAGE:
		fl_font(font, fontsize);
		return;

	case CONTEXT_COL_HEADER:
		fl_push_clip(X, Y, W, H);
		{
			fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, col_header_color());
			fl_color(FL_BLACK);
			if (colheader.find(R) != colheader.end())
				buffer = colheader[C];
			fl_draw(buffer.c_str(), X, Y, W, H, cell_alignement_header_col);
		}
		fl_pop_clip();
		return;

	case CONTEXT_ROW_HEADER:
		fl_push_clip(X, Y, W, H);
		{
			fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, row_header_color());
			fl_color(FL_BLACK);
			if (rowheader.find(R) != rowheader.end())
				buffer = rowheader[R];
			fl_draw(buffer.c_str(), X, Y, W, H, cell_alignement_header_row);
		}
		fl_pop_clip();
		return;

	case CONTEXT_CELL:
	{
						 fl_push_clip(X, Y, W, H);
						 {
							 // BG COLOR
							 fl_color(is_selected(R, C) ? selection_color() : cell_bgcolor);
							 fl_rectf(X, Y, W, H);

							 // TEXT
							 fl_color(cell_fgcolor);
							 buffer = cell(R, C);
							 fl_draw(buffer.c_str(), X, Y, W, H, cell_alignement);
							 // BORDER
							 fl_color(color());
							 fl_rect(X, Y, W, H);
						 }
						 fl_pop_clip();
						 return;
	}

	case CONTEXT_TABLE:
	case CONTEXT_ENDPAGE:
	case CONTEXT_RC_RESIZE:
	case CONTEXT_NONE:
		return;
	}
}

void TableRow::table_callback(Fl_Widget*, void *data) {
	Atanorwtable* o = (Atanorwtable*)data;
	if (o->function != NULL && o->table != NULL) {
		Atanor* func = o->function;
		AtanorCallFunction kfunc(func);

		Atanormap* kmap = globalAtanor->Providemap();
		int rtop, cleft, rbot, cright;
		o->table->get_selection(rtop, cleft, rbot, cright);
		kmap->Push("top", globalAtanor->Provideint(rtop));
		kmap->Push("left", globalAtanor->Provideint(cleft));
		kmap->Push("bottom", globalAtanor->Provideint(rbot));
		kmap->Push("right", globalAtanor->Provideint(cright));
		Atanormapss* valmap = new Atanormapss;
		kmap->Push("values", valmap);
		char buff[20];
		for (int i = rtop; i <= rbot; i++) {
			for (int j = cleft; j <= cright; j++) {
				sprintf_s(buff, 20, "%d:%d", i, j);
				valmap->values[buff] = o->table->cell(i, j);
			}
		}

		o->Setreference();
		o->object->Setreference();
		kmap->Setreference();

		kfunc.arguments.push_back(o);
		kfunc.arguments.push_back(kmap);
		kfunc.arguments.push_back(o->object);
		kfunc.Get(aNULL, aNULL, globalAtanor->GetThreadid());

		o->Resetreference();
		o->object->Resetreference();
		kmap->Resetreference();

	}
}


Atanor* Atanorwtable::MethodCreate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	
	Atanor* x1 = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y1 = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* x2 = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* y2 = callfunc->Evaluate(3, contextualpattern, idthread);
	Atanor* text = callfunc->Evaluate(4, contextualpattern, idthread);
	if (table != NULL)
		return globalAtanor->Returnerror("WND(101): table already created", idthread);
	if (function != NULL && function->Size() != 3)
		return globalAtanor->Returnerror("KIF(881): Error, wrong function arguments", idthread);
	string thetxt = text->String();
	if (thetxt.size() < 1024)
		strcpy_s(txt, 1024, thetxt.c_str());
	
	table = new TableRow(this, x1->Integer(), y1->Integer(), x2->Integer(), y2->Integer(), txt);

	SaveItem(this);
	table->selection_color(FL_YELLOW);
	table->when(FL_WHEN_RELEASE);
	table->table_box(FL_NO_BOX);
	table->col_resize_min(4);
	table->row_resize_min(4);
	table->row_header(0);
	table->col_header(0);
	table->row_resize(1);
	table->col_resize(1);
	table->end();
	return aTRUE;
}

Atanor* Atanorwtable::MethodClear(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (table == NULL)
		return globalAtanor->Returnerror("WND(102): table has not been created yet", idthread);
	table->clear();
	return aTRUE;
}

Atanor* Atanorwtable::MethodAdd(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (table == NULL)
		return globalAtanor->Returnerror("WND(102): table has not been created yet", idthread);
	Atanor* kR = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* kC = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* kvalue = callfunc->Evaluate(2, contextualpattern, idthread);
	table->cell(kR->Integer(), kC->Integer(), kvalue->String());
	return aTRUE;
}

Atanor* Atanorwtable::MethodCell(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (table == NULL)
		return globalAtanor->Returnerror("WND(102): table has not been created yet", idthread);
	Atanor* kR = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* kC = callfunc->Evaluate(1, contextualpattern, idthread);
	string buff = table->cell(kR->Integer(), kC->Integer());
	return globalAtanor->Providestring(buff);
}

Atanor* Atanorwtable::MethodCellAlignment(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (table == NULL)
		return globalAtanor->Returnerror("WND(102): table has not been created yet", idthread);
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	if (ke->Type() == a_string) {
		string align = ke->String();
		if (aligns.find(align) == aligns.end()) {
			align = "WND(202): Unknown alignement";
			return globalAtanor->Returnerror(align, idthread);
		}
		table->setalignment(aligns[align]);
	}
	else
		table->setalignment(ke->Integer());
	return aTRUE;
}

Atanor* Atanorwtable::MethodCellAlignmentHeaderCol(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (table == NULL)
		return globalAtanor->Returnerror("WND(102): table has not been created yet", idthread);
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	if (ke->Type() == a_string) {
		string align = ke->String();
		if (aligns.find(align) == aligns.end()) {
			align = "WND(202): Unknown alignement";
			return globalAtanor->Returnerror(align, idthread);
		}
		table->setalignmentheader(aligns[align], true);
	}
	else
		table->setalignmentheader(ke->Integer(), true);
	return aTRUE;
}

Atanor* Atanorwtable::MethodCellAlignmentHeaderRow(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (table == NULL)
		return globalAtanor->Returnerror("WND(102): table has not been created yet", idthread);
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	if (ke->Type() == a_string) {
		string align = ke->String();
		if (aligns.find(align) == aligns.end()) {
			align = "WND(202): Unknown alignement";
			return globalAtanor->Returnerror(align, idthread);
		}
		table->setalignmentheader(aligns[align], false);
	}
	else
		table->setalignmentheader(ke->Integer(), false);
	return aTRUE;
}

Atanor* Atanorwtable::MethodRow(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (table == NULL)
		return globalAtanor->Returnerror("WND(102): table has not been created yet", idthread);
	if (callfunc->Size() == 0)
		return globalAtanor->Provideint(table->rows());
	Atanor* kR = callfunc->Evaluate(0, contextualpattern, idthread);
	table->rows(kR->Integer());
	return aTRUE;
}

Atanor* Atanorwtable::MethodColumn(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (table == NULL)
		return globalAtanor->Returnerror("WND(102): table has not been created yet", idthread);
	if (callfunc->Size() == 0)
		return globalAtanor->Provideint(table->cols());
	Atanor* kC = callfunc->Evaluate(0, contextualpattern, idthread);
	table->cols(kC->Integer());
	return aTRUE;
}

Atanor* Atanorwtable::MethodRowHeight(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (table == NULL)
		return globalAtanor->Returnerror("WND(102): table has not been created yet", idthread);
	Atanor* kHeight = callfunc->Evaluate(0, contextualpattern, idthread);
	table->row_height_all(kHeight->Integer());
	return aTRUE;
}

Atanor* Atanorwtable::MethodColumnWidth(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (table == NULL)
		return globalAtanor->Returnerror("WND(102): table has not been created yet", idthread);
	Atanor* kWidth = callfunc->Evaluate(0, contextualpattern, idthread);
	table->col_width_all(kWidth->Integer());
	return aTRUE;
}

Atanor* Atanorwtable::MethodFont(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (table == NULL)
		return globalAtanor->Returnerror("WND(678): table not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalAtanor->Provideint(table->Font());
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	//0 is the first parameter and so on...
	if (ke->Type() == a_string) {
		string buf = ke->String();
		if (fonts.find(buf) == fonts.end()) {
			buf = "WND(657): Unknown font code";
			return globalAtanor->Returnerror(buf, idthread);
		}
		table->Font(fonts[buf]);
	}
	else {
		int font = ke->Integer();
		if (font >= fontmax)
			return globalAtanor->Returnerror("WND(658): unknown font", idthread);
		table->Font(font);
	}
	return aTRUE;
}

Atanor* Atanorwtable::MethodFontSize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (table == NULL)
		return globalAtanor->Returnerror("WND(678): table not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalAtanor->Provideint(table->Fontsize());
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	//0 is the first parameter and so on...
	table->Fontsize(ke->Integer());
	return aTRUE;
}

Atanor* Atanorwtable::MethodColorbg(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (table == NULL)
		return globalAtanor->Returnerror("WND(678): table not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalAtanor->Provideint(table->GetCellBGColor());
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	Fl_Color color;
	if (ke->Type() == a_string) {
		//0 is the first parameter and so on...
		string buf = ke->String();
		if (colors.find(buf) == colors.end()) {
			buf = "WND(657): Unknown color code";
			return globalAtanor->Returnerror(buf, idthread);
		}
		color = colors[buf];
	}
	else
		color = ke->Integer();
	table->SetCellBGColor(color);
	return aTRUE;
}

Atanor* Atanorwtable::MethodColorfg(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (table == NULL)
		return globalAtanor->Returnerror("WND(678): table not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalAtanor->Provideint(table->GetCellFGColor());
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	Fl_Color color;
	if (ke->Type() == a_string) {
		//0 is the first parameter and so on...
		string buf = ke->String();
		if (colors.find(buf) == colors.end()) {
			buf = "WND(657): Unknown color code";
			return globalAtanor->Returnerror(buf, idthread);
		}
		color = colors[buf];
	}
	else
		color = ke->Integer();
	table->SetCellFGColor(color);
	return aTRUE;
}

Atanor* Atanorwtable::MethodWhen(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (table == NULL)
		return globalAtanor->Returnerror("WND(805): table not created", idthread);
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
	table->when(action);
	return aTRUE;
}

Atanor* Atanorwtable::MethodTableSelectionColor(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (table == NULL)
		return globalAtanor->Returnerror("WND(678): table not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalAtanor->Provideint(table->GetCellBGColor());
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	Fl_Color color;
	if (ke->Type() == a_string) {
		//0 is the first parameter and so on...
		string buf = ke->String();
		if (colors.find(buf) == colors.end()) {
			buf = "WND(657): Unknown color code";
			return globalAtanor->Returnerror(buf, idthread);
		}
		color = colors[buf];
	}
	else
		color = ke->Integer();
	table->selection_color(color);
	return aTRUE;
}

Atanor* Atanorwtable::MethodBoxType(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (table == NULL)
		return globalAtanor->Returnerror("WND(102): table not created", idthread);
	Atanor* pos = callfunc->Evaluate(0, contextualpattern, idthread);
	if (pos->Type() == a_string) {
		if (boxTypes.find(pos->String()) == boxTypes.end()) {
			string mes = "WND(767): Unknown type:";
			mes += pos->String();
			return globalAtanor->Returnerror(mes, idthread);
		}
		else
			table->table_box((Fl_Boxtype)boxTypes[pos->String()]);
	}
	else
		table->table_box((Fl_Boxtype)pos->Integer());
	return aTRUE;
}

Atanor* Atanorwtable::MethodRowHeader(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (table == NULL)
		return globalAtanor->Returnerror("WND(102): table has not been created yet", idthread);
	Atanor* position = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* label = callfunc->Evaluate(1, contextualpattern, idthread);
	//we then activate the row header, since we have something in it to show
	table->row_header(1);
	table->addrowheader(position->Integer(), label->String());
	return aTRUE;
}

Atanor* Atanorwtable::MethodColumnHeader(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (table == NULL)
		return globalAtanor->Returnerror("WND(102): table has not been created yet", idthread);
	Atanor* position = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* label = callfunc->Evaluate(1, contextualpattern, idthread);
	table->addcolumnheader(position->Integer(), label->String());
	//we activate then column header
	table->col_header(1);
	return aTRUE;
}

Atanor* Atanorwtable::MethodRowHeaderHeight(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (table == NULL)
		return globalAtanor->Returnerror("WND(102): table has not been created yet", idthread);
	Atanor* kwidth = callfunc->Evaluate(0, contextualpattern, idthread);
	table->row_header_width(kwidth->Integer());
	return aTRUE;
}

Atanor* Atanorwtable::MethodColumnHeaderWidth(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (table == NULL)
		return globalAtanor->Returnerror("WND(102): table has not been created yet", idthread);
	Atanor* kheight = callfunc->Evaluate(0, contextualpattern, idthread);
	table->col_header_height(kheight->Integer());
	return aTRUE;
}





