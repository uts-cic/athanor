
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
filename   : atanorwtable.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef atanorwtable_h
#define atanorwtable_h

#include "atanorwidget.h"
class Atanorwtable;

class TableRow : public Fl_Table {
private:
	Fl_Color cell_bgcolor;				// color of cell's bg color
	Fl_Color cell_fgcolor;				// color of cell's fg color
	Fl_Font font;
	int fontsize;
	hmap<int, hmap<int, string> > rowcolarray;
	hmap<int, string> rowheader;
	hmap<int, string> colheader;
	int maxrow;
	int maxcol;
	int cell_alignement;
	int cell_alignement_header_col;
	int cell_alignement_header_row;

protected:
	void draw_cell(TableContext context, int R = 0, int C = 0, int X = 0, int Y = 0, int W = 0, int H = 0);
	static void table_callback(Fl_Widget*, void*);

public:
	string cell(int R, int C) {
		if (rowcolarray.find(R) != rowcolarray.end())
		if (rowcolarray[R].find(C) != rowcolarray[R].end())
			return rowcolarray[R][C];
		return "";
	}


	void setalignmentheader(int c, bool header) {
		if (header)
			cell_alignement_header_col = c;
		else
			cell_alignement_header_row = c;
	}

	void setalignment(int c) {
		cell_alignement = c;
	}

	void clear() {
		rowheader.clear();
		colheader.clear();
		rowcolarray.clear();
		maxrow = 0;
		maxcol = 0;
		rows(0);		// implies clearing selection
		cols(0);
		Fl_Table::clear();	// clear the table
	}

	void addrowheader(int i, string value) {
		rowheader[i] = value;
	}

	void addcolumnheader(int i, string value) {
		colheader[i] = value;
	}

	void cell(int R, int C, string value) {
		if (rowcolarray.find(R) == rowcolarray.end()) {
			hmap<int, string> col;
			rowcolarray[R] = col;
		}
		if (R > maxrow) {
			maxrow = R;
			rows(R + 1);
		}
		if (C > maxcol) {
			maxcol = C;
			cols(C + 1);
		}
		rowcolarray[R][C] = value;
	}


	TableRow(Atanorwtable* table, int x, int y, int w, int h, const char *l = 0) : Fl_Table(x, y, w, h, l) {

		cell_alignement_header_col = FL_ALIGN_LEFT;
		cell_alignement_header_row = FL_ALIGN_LEFT;
		cell_alignement = FL_ALIGN_LEFT;
		cell_bgcolor = FL_WHITE;
		cell_fgcolor = FL_BLACK;
		font = FL_HELVETICA;
		fontsize = 16;
		maxrow = 0;
		maxcol = 0;
		callback(&table_callback, (void*)table);
	}

	Fl_Color GetCellFGColor() const { return(cell_fgcolor); }
	Fl_Color GetCellBGColor() const { return(cell_bgcolor); }
	void SetCellFGColor(Fl_Color val) { cell_fgcolor = val; }
	void SetCellBGColor(Fl_Color val) { cell_bgcolor = val; }
	void Font(Fl_Font val) {
		font = val;
	}
	void Fontsize(int sz) {
		fontsize = sz;
	}
	Fl_Font Font() {
		return font;
	}
	int Fontsize() {
		return fontsize;
	}
};

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorwtable;
//This typedef defines a type "wtableMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorwtable::*wtableMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanorwtable : public Atanorwidget {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static hmap<short, wtableMethod> methods;
    static hmap<string, string> infomethods;
    static bin_hash<unsigned long> exported;

    static short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
	
	TableRow* table;
	char txt[1024];

    //---------------------------------------------------------------------------------------------------------------------
	Atanorwtable(AtanorGlobal* g, Atanor* parent = NULL) : Atanorwidget(g, parent) {
        //Do not forget your variable initialisation
		table = NULL;
		

    }

	void itemclose() {
		if (table != NULL)
			delete table;
		table = NULL;
		clean();
	}

	Fl_Widget* Widget() {
		return table;
	}

	void ResetWidget() {
		table = NULL;
	}

    //----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* index, Atanor* value, short idthread);
	Atanor* Get(Atanor* context, Atanor* index, short idthread);


    short Type() {
        return Atanorwtable::idtype;
    }

    string Typename() {
        return "wtable";
    }

    Atanor* Atom(bool forced=false) {
        return this;
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
		Atanorwtable* a = new Atanorwtable(globalAtanor);
		a->function = f;
		return a;
    }

	AtanorIteration* Newiteration(bool direction);

    static void AddMethod(AtanorGlobal* g, string name, wtableMethod func, unsigned long arity, string infos);
    
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
    //Atanor* MethodSize(AtanorGlobal* global,Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {return aZERO;}
	Atanor* MethodCreate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodClear(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodAdd(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodCell(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodCellAlignment(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodCellAlignmentHeaderCol(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodCellAlignmentHeaderRow(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodRow(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodColumn(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodRowHeight(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodColumnWidth(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodFont(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodFontSize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodColorbg(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodColorfg(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodWhen(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodTableSelectionColor(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodBoxType(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodRowHeader(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodColumnHeader(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodRowHeaderHeight(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodColumnHeaderWidth(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

    

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
		if (idthread && callfunc->Size() != 0) {
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
		return 0;
	}

	uchar Byte() {
		
		return 0;
	}

	long Integer() {
		
		return 0;
	}
	
	double Float() {
		
		return 0;
	}
	
	string String() {
		
		return "";
	}

	
	bool Boolean() {
		
		return true;
	}

    
    //Basic operations    
	Atanor* andset(Atanor* a, bool autoself) {
		
	    return this;
	}

	Atanor* orset(Atanor* a, bool autoself) {
		
	    return this;
	}

	Atanor* xorset(Atanor* a, bool autoself) {
		
	    return this;
	}

	Atanor* plus(Atanor* a, bool autoself) {
		
	    return this;
	}
	Atanor* minus(Atanor* a, bool autoself) {
		
	    return this;
	}

	Atanor* multiply(Atanor* a, bool autoself) {
		
	    return this;
	}

	Atanor* divide(Atanor* a, bool autoself) {
		
	    return this;

	}
	Atanor* mod(Atanor* a, bool autoself) {
		
	    return this;
	}

	Atanor* shiftleft(Atanor* a, bool autoself) {
		
	    return this;
	}


	Atanor* shiftright(Atanor* a, bool autoself) {
		
	    return this;
	}

	Atanor* power(Atanor* a, bool autoself) {
		
	    return this;
	}

	Atanor* less(Atanor* a) {
		return aFALSE;
	}
	Atanor* more(Atanor* a) {
		return aFALSE;
	}
	Atanor* same(Atanor* a) {
		return aFALSE;
	}
	Atanor* different(Atanor* a) {
		return aFALSE;
	}

	Atanor* lessequal(Atanor* a) {
		return aFALSE;
	}

	Atanor* moreequal(Atanor* a) {
		return aFALSE;
	}

	Atanor* plusplus() {
		return this;
	}

	Atanor* minusminus() {
		return this;
	}
};

#endif








