
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
filename   : atanorwidget.cxx
Date       : 2017/04/01
Purpose    : mother class of all FLTK widgets
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atoms.h"
#include "atanormapsi.h"
#include "atanorwidget.h"

void flredraw(void* data) {
	widge* w = (widge*)data;
	if (Stopall() || globalAtanor->Error(w->idthread)) {
		delete w;
		return;
	}
	w->redraw();
	delete w;
}

Atanor* Atanorwidget::MethodRedraw(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (Stopall())
		return aFALSE;

	Fl_Widget* wdg = Widget();
	//If we are in the main thread, well business as usual		
	if (wdg == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);

	if (callfunc->Size() == 1)
		update = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
	else
		update = true;

	if (idthread)
		Fl::awake(flredraw, (void*)new widge(wdg, this, idtracker, idthread));
	else
		wdg->redraw();
	return aTRUE;
}

Atanor* Atanorwidget::MethodTextFont(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	Atanor* f = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* sz = callfunc->Evaluate(1, contextualpattern, idthread);
	if (f->Type() == a_string) {
		string buf = f->String();
		if (fonts.find(buf) == fonts.end()) {
			buf = "WND(657): Unknown font code";
			return globalAtanor->Returnerror(buf, idthread);
		}
		fl_font(fonts[buf], sz->Integer());
	}
	else {
		int i = f->Integer();
		if (i >= fontmax)
			return globalAtanor->Returnerror("WND(658): unknown font", idthread);
		fl_font(i, sz->Integer());
	}
	return aTRUE;
}

Atanor* Atanorwidget::MethodRGBColor(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	if (callfunc->Size() == 3) {
		Atanor* r = callfunc->Evaluate(0, contextualpattern, idthread);
		Atanor* g = callfunc->Evaluate(1, contextualpattern, idthread);
		Atanor* b = callfunc->Evaluate(2, contextualpattern, idthread);
		return globalAtanor->Provideint(fl_rgb_color(r->Integer(), g->Integer(), b->Integer()));
	}
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	if (ke->isVectorContainer()) {
		if (ke->Size() != 3)
			return globalAtanor->Returnerror("WND(245): Wrong number of elements in the color vector ", idthread);
		return globalAtanor->Provideint(fl_rgb_color(ke->getvalue(0)->Integer(), ke->getvalue(1)->Integer(), ke->getvalue(2)->Integer()));
	}
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
	uchar r, g, b;
	Fl::get_color(color, r, g, b); // Fl_Color to RGB
	Atanorivector* kvect = new Atanorivector;
	kvect->values.push_back(r);
	kvect->values.push_back(g);
	kvect->values.push_back(b);
	return kvect;
}

Atanor* Atanorwidget::MethodShow(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Fl_Widget* wdg = Widget();
	if (wdg == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	wdg->show();
	return aTRUE;
}

Atanor* Atanorwidget::MethodFocus(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Fl_Widget* wdg;
	//If we are in the main thread, well business as usual
	wdg = Widget();
	if (wdg == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	wdg->take_focus();
	return aTRUE;
}

Atanor* Atanorwidget::MethodAlign(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Fl_Widget* wdg = Widget();
	if (wdg == NULL)
		return globalAtanor->Returnerror("WND(805): Widget not initialized", idthread);
	Atanor* ke = callfunc->Evaluate(0, aNULL, idthread);
	if (ke->Type() == a_string) {
		string align = ke->String();
		if (aligns.find(align) == aligns.end()) {
			align = "WND(202): Unknown alignement";
			return globalAtanor->Returnerror(align, idthread);
		}
		wdg->align(aligns[align]);
	}
	else
		wdg->align(ke->Integer());
	return aTRUE;
}

Atanor* Atanorwidget::MethodCreated(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() != NULL)
		return aTRUE;
	return aFALSE;
}

Atanor* Atanorwidget::MethodCopy(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	string tocopy = callfunc->Evaluate(0, aNULL, idthread)->String();
	Fl::copy(STR(tocopy), tocopy.length(), 1);
	return aTRUE;
}

Atanor* Atanorwidget::MethodPaste(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	Fl::paste(*Widget(), 1);
	return aTRUE;
}

Atanor* Atanorwidget::MethodCoordinates(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);


	Fl_Widget* wdg = Widget();
	if (wdg == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	Atanor* kvect;
	if (callfunc->Size() == 0) {
		kvect = Selectaivector(contextualpattern);
		kvect->storevalue((long)wdg->x());
		kvect->storevalue((long)wdg->y());
		kvect->storevalue((long)wdg->w());
		kvect->storevalue((long)wdg->h());
		return kvect;
	}

	int x, y, w, h;
	if (callfunc->Size() == 1) {
		Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
		if (ke->isVectorContainer() == false)
			return globalAtanor->Returnerror("WND(904): Expecting a vector", idthread);
		kvect = ke;
		if (kvect->Size() != 4)
			return globalAtanor->Returnerror("WND(905): Missing coordinates", idthread);
		x = kvect->getvalue(0)->Integer();
		y = kvect->getvalue(1)->Integer();
		w = kvect->getvalue(2)->Integer();
		h = kvect->getvalue(3)->Integer();
	}
	else {
		x = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
		y = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
		w = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
		h = callfunc->Evaluate(3, contextualpattern, idthread)->Integer();
	}
	if (x >= w || y >= h)
		return globalAtanor->Returnerror("WND(905): Incoherent coordinates", idthread);
	wdg->resize(x, y, w, h);
	return aTRUE;
}

Atanor* Atanorwidget::MethodSelectionColor(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	Fl_Widget* wdg = Widget();
	if (wdg == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalAtanor->Provideint(wdg->selection_color());
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
	wdg->selection_color(color);
	return aTRUE;
}

Atanor* Atanorwidget::MethodLabel(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	Fl_Widget* wdg = Widget();
	if (wdg == NULL)
		return globalAtanor->Returnerror("WND(102): wdg not created", idthread);
	string label;
	if (callfunc->Size() == 0) {
		label = wdg->label();
		return globalAtanor->Providestring(label);
	}
	label = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	char* ltxt = (char*)malloc(label.size() + 1);
	memcpy(ltxt, STR(label), label.size());
	ltxt[label.size()] = 0;
	strcpy_s(ltxt, label.size() + 1, label.c_str());
	wdg->label(ltxt);
	return aTRUE;
}

Atanor* Atanorwidget::MethodLabelType(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	Fl_Widget* wdg = Widget();
	if (wdg == NULL)
		return globalAtanor->Returnerror("WND(102): wdg not created", idthread);
	if (callfunc->Size() == 0)
		return globalAtanor->Provideint(wdg->labeltype());
	Atanor* labeltype = callfunc->Evaluate(0, contextualpattern, idthread);
	wdg->labeltype((Fl_Labeltype)labeltype->Integer());
	return aTRUE;
}

Atanor* Atanorwidget::MethodLabelColor(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	//In our example, we have only two parameters
	Fl_Widget* wdg = Widget();
	if (wdg == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalAtanor->Provideint(wdg->labelcolor());
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	if (ke->Type() == a_string) {
		//0 is the first parameter and so on...
		string shortcut = ke->String();
		if (colors.find(shortcut) == colors.end()) {
			shortcut = "WND(657): Unknown color code";
			return globalAtanor->Returnerror(shortcut, idthread);
		}
		wdg->labelcolor(colors[shortcut]);
	}
	else {
		unsigned int c = ke->Integer();
		wdg->labelcolor(c);
	}
	return aTRUE;
}

Atanor* Atanorwidget::MethodLabelFont(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	Fl_Widget* wdg = Widget();
	if (wdg == NULL)
		return globalAtanor->Returnerror("WND(102): wdg not created", idthread);
	if (callfunc->Size() == 0)
		return globalAtanor->Provideint(wdg->labelfont());
	Atanor* labelfont;
	int font = 0;
	for (int i = 0; i < callfunc->Size(); i++) {
		labelfont = callfunc->Evaluate(i, contextualpattern, idthread);
		if (labelfont->Type() == a_string) {
			if (fonts.find(labelfont->String()) == fonts.end()) {
				string mes = "WND(765): Unknown font:";
				mes += labelfont->String();
				return globalAtanor->Returnerror(mes, idthread);
			}
			font = fonts[labelfont->String()];
		}
		else
			font = labelfont->Integer();
	}
	if (font >= fontmax)
		return globalAtanor->Returnerror("WND(658): unknown font", idthread);
	wdg->labelfont(font);
	return aTRUE;
}

Atanor* Atanorwidget::MethodLabelSize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	Fl_Widget* wdg = Widget();
	if (wdg == NULL)
		return globalAtanor->Returnerror("WND(678): wdg not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalAtanor->Provideint(wdg->labelsize());
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	//0 is the first parameter and so on...
	wdg->labelsize(ke->Integer());
	return aTRUE;
}

Atanor* Atanorwidget::MethodDrawText(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
#ifdef WIN32
	//On WIN32, if no font is given beforehand, the whole stuff crashes...
	if (fl_graphics_driver->font_descriptor() == NULL)
		fl_font(FL_HELVETICA, 12);
#endif
	Atanor* t = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* x = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(2, contextualpattern, idthread);
	string buf = t->String();
	char* label = new char[buf.size() + 1];
	strcpy_s(label, buf.size() + 1, buf.c_str());
	fl_draw(label, x->Integer(), y->Integer());
	delete[] label;
	return aTRUE;
}

Atanor* Atanorwidget::MethodDrawColor(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
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
	fl_color(color);
	return aTRUE;
}

Atanor* Atanorwidget::MethodRectangle(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* wx = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* hy = callfunc->Evaluate(3, contextualpattern, idthread);
	if (callfunc->Size() == 5) {
		Fl_Color color;
		Atanor* ke = callfunc->Evaluate(4, contextualpattern, idthread);
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
		fl_rect(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), color);
	}
	else
		fl_rect(x->Integer(), y->Integer(), wx->Integer(), hy->Integer());
	return aTRUE;
}

Atanor* Atanorwidget::MethodVertex(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (!begins.size())
		return globalAtanor->Returnerror("WND(610): You cannot use this function outside a complex shape construction", idthread);
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	fl_vertex(x->Float(), y->Float());
	return aTRUE;
}

Atanor* Atanorwidget::MethodRectangleFill(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* wx = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* hy = callfunc->Evaluate(3, contextualpattern, idthread);
	if (callfunc->Size() == 5) {
		Fl_Color color;
		Atanor* ke = callfunc->Evaluate(4, contextualpattern, idthread);
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
		fl_rectf(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), color);
	}
	else
		fl_rectf(x->Integer(), y->Integer(), wx->Integer(), hy->Integer());
	return aTRUE;
}

Atanor* Atanorwidget::MethodLineShape(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	Atanor* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	int w = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
	//0 is the first parameter and so on...
	if (ke->Type() == a_string) {
		string buf = ke->String();
		if (lineshape.find(buf) == lineshape.end()) {
			buf = "WND(607): Unknown line shape code";
			return globalAtanor->Returnerror(buf, idthread);
		}
		fl_line_style(lineshape[buf], w);
	}
	else
		fl_line_style(ke->Integer(), w);
	return aTRUE;
}

Atanor* Atanorwidget::MethodPolygon(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* x1 = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* y1 = callfunc->Evaluate(3, contextualpattern, idthread);
	Atanor* x2 = callfunc->Evaluate(4, contextualpattern, idthread);
	Atanor* y2 = callfunc->Evaluate(5, contextualpattern, idthread);
	if (callfunc->Size() == 6)
		fl_polygon(x->Integer(), y->Integer(), x1->Integer(), y1->Integer(), x2->Integer(), y2->Integer());
	else {
		Atanor* x3 = callfunc->Evaluate(6, contextualpattern, idthread);
		Atanor* y3 = callfunc->Evaluate(7, contextualpattern, idthread);
		fl_polygon(x->Integer(), y->Integer(), x1->Integer(), y1->Integer(), x2->Integer(), y2->Integer(), x3->Integer(), y3->Integer());
	}
	return aTRUE;
}

Atanor* Atanorwidget::MethodLoop(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* x1 = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* y1 = callfunc->Evaluate(3, contextualpattern, idthread);
	Atanor* x2 = callfunc->Evaluate(4, contextualpattern, idthread);
	Atanor* y2 = callfunc->Evaluate(5, contextualpattern, idthread);
	if (callfunc->Size() == 6)
		fl_loop(x->Integer(), y->Integer(), x1->Integer(), y1->Integer(), x2->Integer(), y2->Integer());
	else {
		Atanor* x3 = callfunc->Evaluate(6, contextualpattern, idthread);
		Atanor* y3 = callfunc->Evaluate(7, contextualpattern, idthread);
		fl_loop(x->Integer(), y->Integer(), x1->Integer(), y1->Integer(), x2->Integer(), y2->Integer(), x3->Integer(), y3->Integer());
	}
	return aTRUE;
}

Atanor* Atanorwidget::MethodLine(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* x1 = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* y1 = callfunc->Evaluate(3, contextualpattern, idthread);
	if (callfunc->Size() == 4)
		fl_line(x->Integer(), y->Integer(), x1->Integer(), y1->Integer());
	else {
		Atanor* x2 = callfunc->Evaluate(4, contextualpattern, idthread);
		Atanor* y2 = callfunc->Evaluate(5, contextualpattern, idthread);
		fl_line(x->Integer(), y->Integer(), x1->Integer(), y1->Integer(), x2->Integer(), y2->Integer());
	}
	return aTRUE;
}

Atanor* Atanorwidget::MethodLineRotation(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	double x = callfunc->Evaluate(0, contextualpattern, idthread)->Float();
	double y = callfunc->Evaluate(1, contextualpattern, idthread)->Float();
	double distance = callfunc->Evaluate(2, contextualpattern, idthread)->Float();
	double angle = callfunc->Evaluate(3, contextualpattern, idthread)->Float();
	bool draw = false;
	if (callfunc->Size() == 5)
		draw = callfunc->Evaluate(4, contextualpattern, idthread)->Boolean();
	double x1, y1;
	x1 = x + cos(angle)*distance;
	y1 = y - sin(angle)*distance;
	if (draw)
		fl_line((long)x, (long)y, (long)x1, (long)y1);
	Atanor* kvect = Selectafvector(contextualpattern);
	kvect->storevalue(x1);
	kvect->storevalue(y1);
	return kvect;
}

Atanor* Atanorwidget::MethodPushMatrix(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	begins.push_back(6);
	fl_push_matrix();
	return aTRUE;
}

Atanor* Atanorwidget::MethodPopMatrix(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (!begins.size() || begins.back() != 6)
		return globalAtanor->Returnerror("WND(612): Cannot call this method here. Have you called beginpoints before?", idthread);
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	begins.pop_back();
	fl_pop_matrix();
	return aTRUE;
}

Atanor* Atanorwidget::MethodScale(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	if (callfunc->Size() == 2) {
		Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
		fl_scale(x->Float(), y->Float());
	}
	else
		fl_scale(x->Float());
	return aTRUE;
}

Atanor* Atanorwidget::MethodTranslate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	fl_translate(x->Float(), y->Float());
	return aTRUE;
}

Atanor* Atanorwidget::MethodRotate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	Atanor* d = callfunc->Evaluate(0, contextualpattern, idthread);
	fl_rotate(d->Float());
	return aTRUE;
}

Atanor* Atanorwidget::MethodMultmatrix(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	Atanor* a = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* b = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* c = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* d = callfunc->Evaluate(3, contextualpattern, idthread);
	Atanor* x = callfunc->Evaluate(4, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(5, contextualpattern, idthread);
	fl_mult_matrix(a->Float(), b->Float(), c->Float(), d->Float(), x->Float(), y->Float());
	return aTRUE;
}

Atanor* Atanorwidget::MethodTransform_x(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	double v = fl_transform_x(x->Float(), y->Float());
	return globalAtanor->Providefloat(v);
}

Atanor* Atanorwidget::MethodTransform_y(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	double v = fl_transform_y(x->Float(), y->Float());
	return globalAtanor->Providefloat(v);
}

Atanor* Atanorwidget::MethodTransform_dx(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	double v = fl_transform_dx(x->Float(), y->Float());
	return globalAtanor->Providefloat(v);
}

Atanor* Atanorwidget::MethodTransform_dy(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	double v = fl_transform_dy(x->Float(), y->Float());
	return globalAtanor->Providefloat(v);
}

Atanor* Atanorwidget::MethodTransform_vertex(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	fl_transformed_vertex(x->Float(), y->Float());
	return aTRUE;
}

Atanor* Atanorwidget::MethodBeginPoints(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	begins.push_back(1);
	fl_begin_points();
	return aTRUE;
}

Atanor* Atanorwidget::MethodEndPoints(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (!begins.size() || begins.back() != 1)
		return globalAtanor->Returnerror("WND(612): Cannot call this method here. Have you called beginpoints before?", idthread);
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	begins.pop_back();
	fl_end_points();
	return aTRUE;
}

Atanor* Atanorwidget::MethodBeginLine(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	begins.push_back(5);
	fl_begin_line();
	return aTRUE;
}

Atanor* Atanorwidget::MethodEndLine(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (!begins.size() || begins.back() != 5)
		return globalAtanor->Returnerror("WND(612): Cannot call this method here. Have you called beginline before?", idthread);
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	begins.pop_back();
	fl_end_line();
	return aTRUE;
}

Atanor* Atanorwidget::MethodBeginLoop(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	begins.push_back(2);
	fl_begin_loop();
	return aTRUE;
}

Atanor* Atanorwidget::MethodEndLoop(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (!begins.size() || begins.back() != 2)
		return globalAtanor->Returnerror("WND(612): Cannot call this method here. Have you called beginloop before?", idthread);
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	begins.pop_back();
	fl_end_loop();
	return aTRUE;
}

Atanor* Atanorwidget::MethodBeginPolygon(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	begins.push_back(3);
	fl_begin_polygon();
	return aTRUE;
}

Atanor* Atanorwidget::MethodEndPolygon(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (!begins.size() || begins.back() != 3)
		return globalAtanor->Returnerror("WND(612): Cannot call this method here. Have you called beginpolygon before?", idthread);
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	begins.pop_back();
	fl_end_polygon();
	return aTRUE;
}

Atanor* Atanorwidget::MethodBeginComplexPolygon(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	begins.push_back(4);
	fl_begin_complex_polygon();
	return aTRUE;
}

Atanor* Atanorwidget::MethodGap(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (!begins.size() || begins.back() != 4)
		return globalAtanor->Returnerror("WND(612): Cannot call this method here. Have you called begincomplexpolygon before?", idthread);
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	fl_gap();
	return aTRUE;
}

Atanor* Atanorwidget::MethodEndComplexPolygon(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (!begins.size() || begins.back() != 4)
		return globalAtanor->Returnerror("WND(612): Cannot call this method here. Have you called begincomplexpolygon before?", idthread);
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	begins.pop_back();
	fl_end_complex_polygon();
	return aTRUE;
}

Atanor* Atanorwidget::MethodArc(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* w = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* h = callfunc->Evaluate(3, contextualpattern, idthread);
	Atanor* a1 = callfunc->Evaluate(4, contextualpattern, idthread);
	if (begins.size() != 0) {
		if (callfunc->Size() == 6)
			return globalAtanor->Returnerror("WND(608): Wrong number of parameters=expected 5 in a complex shape construction", idthread);
		fl_arc(x->Float(), y->Float(), w->Float(), h->Float(), a1->Float());
	}
	else {
		if (callfunc->Size() != 6)
			return globalAtanor->Returnerror("WND(609): Wrong number of parameters=expected 6 in a regular construction", idthread);
		Atanor* a2 = callfunc->Evaluate(5, contextualpattern, idthread);
		fl_arc(x->Integer(), y->Integer(), w->Integer(), h->Integer(), a1->Float(), a2->Float());
	}
	return aTRUE;
}

Atanor* Atanorwidget::MethodCurve(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (!begins.size())
		return globalAtanor->Returnerror("WND(610): You cannot use this function outside a complex shape construction", idthread);
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* x1 = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* y1 = callfunc->Evaluate(3, contextualpattern, idthread);
	Atanor* x2 = callfunc->Evaluate(4, contextualpattern, idthread);
	Atanor* y2 = callfunc->Evaluate(5, contextualpattern, idthread);
	Atanor* x3 = callfunc->Evaluate(6, contextualpattern, idthread);
	Atanor* y3 = callfunc->Evaluate(7, contextualpattern, idthread);
	fl_curve(x->Float(), y->Float(), x1->Float(), y1->Float(), x2->Float(), y2->Float(), x3->Float(), y3->Float());
	return aTRUE;
}

Atanor* Atanorwidget::MethodPushClip(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	//0 is the first parameter and so on...
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* wx = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* hy = callfunc->Evaluate(3, contextualpattern, idthread);
	fl_push_clip(x->Integer(), y->Integer(), wx->Integer(), hy->Integer());
	return aTRUE;
}

Atanor* Atanorwidget::MethodPopClip(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	fl_pop_clip();
	return aTRUE;
}

Atanor* Atanorwidget::MethodPie(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Atanor* w = callfunc->Evaluate(2, contextualpattern, idthread);
	Atanor* h = callfunc->Evaluate(3, contextualpattern, idthread);
	Atanor* a1 = callfunc->Evaluate(4, contextualpattern, idthread);
	Atanor* a2 = callfunc->Evaluate(5, contextualpattern, idthread);
	fl_pie(x->Integer(), y->Integer(), w->Integer(), h->Integer(), a1->Float(), a2->Float());
	return aTRUE;
}

Atanor* Atanorwidget::MethodPoint(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	Atanor* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* y = callfunc->Evaluate(1, contextualpattern, idthread);
	fl_point(x->Integer(), y->Integer());
	return aTRUE;
}

Atanor* Atanorwidget::MethodCircle(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	int x = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	int y = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
	int r = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
	if (callfunc->Size() == 3) {
		fl_circle(x, y, r);
		return aTRUE;
	}
	Atanor* kcolor = callfunc->Evaluate(3, contextualpattern, idthread);
	Fl_Color color;
	if (kcolor->Type() == a_string) {
		//0 is the first parameter and so on...
		string buf = kcolor->String();
		if (colors.find(buf) == colors.end()) {
			buf = "WND(657): Unknown color code";
			return globalAtanor->Returnerror(buf, idthread);
		}
		color = colors[buf];
	}
	else
		color = kcolor->Integer();
	//we set the color
	fl_color(color);
	fl_circle(x, y, r);
	return aTRUE;
}

Atanor* Atanorwidget::MethodTextMeasure(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (Widget() == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	Atanor* t = callfunc->Evaluate(0, contextualpattern, idthread);
	string buf = t->String();
	int w;
	int h;
	fl_measure(buf.c_str(), w, h, 1);
	if (contextualpattern->isVectorContainer()) {
		Atanor* iv = Selectaivector(contextualpattern);
		iv->storevalue((long)w);
		iv->storevalue((long)h);
		return iv;
	}
	Atanormapsi* map = new Atanormapsi;
	map->values["W"] = w;
	map->values["H"] = h;
	return map;
}


Atanor* Atanorwidget::MethodTooltip(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Fl_Widget* wdg = Widget();
	if (wdg == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	string buf = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	char* msg = (char*)malloc(buf.size() + 1);
	memcpy(msg, STR(buf), buf.size());
	msg[buf.size()] = 0;
	wdg->tooltip(msg);
	return aTRUE;
}

Atanor* Atanorwidget::MethodHide(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Fl_Widget* wdg = Widget();
	if (wdg == NULL)
		return globalAtanor->Returnerror("WND(678): Widget not initialized", idthread);
	wdg->hide();
	return aTRUE;
}

Atanor* Atanorwidget::MethodBackgroundColor(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Locking _lockgr(this);

	Fl_Widget* window = Widget();
	if (window == NULL)
		return globalAtanor->Returnerror("WND(805): Widget not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalAtanor->Provideint(window->color());
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
	window->color(color);
	return aTRUE;
}




