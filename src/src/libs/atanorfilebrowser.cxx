
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
filename   : atanorfilebrowser.cxx
Date       : 2017/04/01
Purpose    : File browser (based on FLTK)
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorfilebrowser.h"
#include "instructions.h"

static void filebrowser_callback(Fl_File_Chooser *fc, void* data) {
	Atanorfilebrowser* slid = (Atanorfilebrowser*)data;
	Atanor* func = slid->function;
	AtanorCallFunction kfunc(func);
	slid->Setreference();
	slid->object->Setreference();

	kfunc.arguments.push_back(slid);
	kfunc.arguments.push_back(slid->object);
	kfunc.Get(aNULL, aNULL, globalAtanor->GetThreadid());
	slid->Resetreference();
	slid->object->Resetreference();
}


//We need to declare once again our local definitions.
hmap<short, filebrowserMethod>  Atanorfilebrowser::methods;
hmap<string, string> Atanorfilebrowser::infomethods;
bin_hash<unsigned long> Atanorfilebrowser::exported;

short Atanorfilebrowser::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorfilebrowser::AddMethod(AtanorGlobal* global, string name, filebrowserMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorfilebrowser::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorfilebrowser::idtype = global->Getid("filebrowser");

	Atanorfilebrowser::AddMethod(global, "_initial", &Atanorwidget::MethodInitial, P_ONE, "_initial(variable): Creation of a window with a call back object");
	Atanorfilebrowser::AddMethod(global, "create", &Atanorfilebrowser::MethodCreate, P_FOUR, "create(string , intialdirectory, string filter, bool directory, string label): Open a file browser select directories if directory is true");
	Atanorfilebrowser::AddMethod(global, "value", &Atanorfilebrowser::MethodValue, P_NONE, "value(): Return the selected file");
	Atanorfilebrowser::AddMethod(global, "close", &Atanorfilebrowser::MethodClose, P_NONE, "close(): Close the file browser");
	Atanorfilebrowser::AddMethod(global, "ok", &Atanorfilebrowser::MethodOk, P_NONE, "ok(): return true if ok was pressed");


	global->newInstance[Atanorfilebrowser::idtype] = new Atanorfilebrowser(global);
	global->RecordMethods(Atanorfilebrowser::idtype, Atanorfilebrowser::exported);

	return true;
}


Atanor* Atanorfilebrowser::Put(Atanor* index, Atanor* value, short idthread) {
	return this;
}

Atanor* Atanorfilebrowser::Get(Atanor* context, Atanor* index, short idthread) {
	return this;
}

AtanorIteration* Atanorfilebrowser::Newiteration(bool direction) {
	return aITERNULL;
}


Atanor* Atanorfilebrowser::MethodCreate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	if (filebrowser != NULL)
		return globalAtanor->Returnerror("WND(100): Cannot recreate an existing filebrowser", idthread);

	

	//0 is the first parameter and so on...
	string d = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	string f = callfunc->Evaluate(1, contextualpattern, idthread)->String();
	int t = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
	string label = callfunc->Evaluate(3, contextualpattern, idthread)->String();
	strcpy_s(txt, 1024, label.c_str());
	strcpy_s(directory, 1024, d.c_str());
	strcpy_s(filter, 1024, f.c_str());
	filebrowser = new Fl_File_Chooser(directory, filter, t, txt);
	//filebrowser->callback(filebrowser_callback,this);
	filebrowser->show();
	while (filebrowser->shown())
		Fl::wait();
	if (function != NULL) {
		filebrowser_callback(filebrowser, this);
		return aTRUE;
	}
	string buf;
	if (filebrowser->value() != NULL)
		buf = filebrowser->value();
	delete filebrowser;
	return globalAtanor->Providestring(buf);
}

Atanor* Atanorfilebrowser::MethodValue(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	if (filebrowser == NULL)
		return globalAtanor->Returnerror("WND(101): File browser not initialized", idthread);
	if (filebrowser->value() == NULL)
		return globalAtanor->Providestring("");
	string buf = filebrowser->value();
	return globalAtanor->Providestring(buf);
}

Atanor* Atanorfilebrowser::MethodClose(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	if (filebrowser == NULL)
		return aFALSE;
	delete filebrowser;
	filebrowser = NULL;
	return aTRUE;
}

Atanor* Atanorfilebrowser::MethodOk(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	if (filebrowser == NULL)
		return globalAtanor->Returnerror("WND(101): File browser not initialized", idthread);
	if (filebrowser->shown())
		return aFALSE;
	return aTRUE;
}





