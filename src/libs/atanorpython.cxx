
/*
*  Athanor: Le langage des Alpages mède à Grenoble
*
* Copyright (C) 2010 - 2017 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
* This file can only be used with the ATHANOR library or the executable
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : ATHANOR
Version    : ATANOR 0.26
filename   : atanorpython.cxx
Date       : 2017/04/01
Purpose    : it is a bdirectional library, allowing on the one hand to  execute Python from within Athanor, or Athanor from within Python
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "globalatanor.h"
#include "compilecode.h"
#include "atanorversion.h"
#include "atanorint.h"
#include "atanorfloat.h"
#include "atanorlong.h"
#include "atanormap.h"
#include "atanorvector.h"
#include "atanorbool.h"
#include "atanorstring.h"

#include <Python.h>

#if PY_MAJOR_VERSION >= 3
#define PyCheck_String PyUnicode_Check
#define ConvertToPythonLong(i)	PyLong_FromLong(i)
#define PyCheck_Number	PyLong_Check
#define PyAsNumber	PyLong_AsLong

PyObject* ConvertToPythonString(string s) {
	string res;
	s_latin_to_utf8(res, USTR(s));
	const char* buff = res.c_str();
	return PyUnicode_FromStringAndSize(buff, res.size());
}

string PyAsString(PyObject* po) {
	string s;
	Py_ssize_t sz = PyUnicode_GetSize(po);
	Py_UNICODE* ibuff = PyUnicode_AsUnicode(po);
	for (int i = 0; i < sz; i++)
		s += s_unicode_to_utf8(ibuff[i]);
	return s;
}

#else
#define ConvertToPythonString(s)  PyString_FromString(s)
#define ConvertToPythonLong(i)	PyInt_FromLong(i)
#define PyCheck_Number	PyInt_Check
#define PyAsNumber PyInt_AsLong
#define PyAsString PyString_AsString
#define PyCheck_String PyString_Check

#if PY_MINOR_VERSION==4
#define Py_ssize_t int
#define OLDPYTHON 1
#endif

#endif

#include "atanorpython.h"


static bool init_python = false;

//We need to declare once again our local definitions.
hmap<unsigned short, pythonMethod>  Atanorpython::methods;
hmap<string, string> Atanorpython::infomethods;
bin_hash<unsigned long> Atanorpython::exported;

short Atanorpython::idtype = 0;

//------------------------------------------------------------------------------------------------------------------
//If you want to implement an external library... Otherwise remove it...
extern "C" {
	Exporting bool python_InitialisationModule(AtanorGlobal* global, string version) {
		if (version != AtanorVersion())
			return false;

		//Global variables are quite complex to export to dlls
		global->Update();

		return Atanorpython::InitialisationModule(global, version);
	}
}
//------------------------------------------------------------------------------------------------------------------
static Atanor* toAtanor(PyObject* po) {
	if (PyBool_Check(po) == 1) {
		if (po == Py_True)
			return new Atanorbool(true);
		return new Atanorbool(false);
	}

	if (PyCheck_Number(po) == 1) {
		long l = PyLong_AsLong(po);
		return globalAtanor->Provideint(l);
	}

	if (PyLong_Check(po) == 1) {
		BLONG l = PyLong_AsLong(po);
		return new Atanorlong(l);
	}

	if (PyFloat_Check(po) == 1) {
		double d = PyFloat_AsDouble(po);
		return globalAtanor->Providefloat(d);
	}

	if (PyCheck_String(po) == 1) {
		string s = PyAsString(po);
		return globalAtanor->Providestring(s);
	}

	PyObject* pelement;
	Atanor* e;
	if (PyList_Check(po) == 1) {
		Atanorvector* kvect = globalAtanor->Providevector();
		Py_ssize_t tail = PyList_GET_SIZE(po);
		for (Py_ssize_t i = 0; i < tail; i++) {
			//Puis chacun des objets, kj est l'index de l'element
			pelement = PyList_GetItem(po, i);
			e = toAtanor(pelement);
			if (e == aRAISEERROR) {
				kvect->Release();
				return aRAISEERROR;
			}
			kvect->Push(e);
		}
		return kvect;
	}

	if (PyDict_Check(po) == 1) {
		Atanormap* kmap = globalAtanor->Providemap();
		Py_ssize_t pos = 0;
		Atanor* k;
		PyObject* key;
		string kval;
		while (PyDict_Next(po, &pos, &key, &pelement)) {
			if (key != NULL && pelement != NULL) {
				k = toAtanor(key);
				if (k == aRAISEERROR) {
					kmap->Release();
					return aRAISEERROR;
				}
				e = toAtanor(pelement);
				if (e == aRAISEERROR) {
					kmap->Release();
					return aRAISEERROR;
				}
				kval = k->String();
				kmap->Push(kval, e);
				k->Release();
			}
		}
		return kmap;
	}

#ifdef OLDPYTHON
	if (PyUnicode_Check(po)) {
		Py_ssize_t sz = PyUnicode_GET_DATA_SIZE(po);
		const char* sbuff = PyUnicode_AS_DATA(po);
		string s;
		for (Py_ssize_t i = 0; i < sz; i++) {
			if (sbuff[i])
				s += sbuff[i];
		}
		return globalAtanor->Providestring(s);
	}
#else
	if (PyUnicode_Check(po)) {
		Py_ssize_t sz = PyUnicode_GetSize(po);
		Py_UNICODE* ibuff = PyUnicode_AsUnicode(po);
		string s;
		for (int i = 0; i < sz; i++)
			s += s_unicode_to_utf8(ibuff[i]);
		return globalAtanor->Providestring(s);
	}
#endif

	PyObject* perr = PyObject_Str(po);
	if (perr != NULL) {
		string s = PyAsString(perr);
		e = globalAtanor->Providestring(s);
		Py_DECREF(perr);
		return e;
	}

	return aRAISEERROR;
}

static PyObject* toPython(Atanor* resultat) {
	if (resultat->Type() == a_boolean) {
		if (resultat == aTRUE)
			return Py_True;
		return Py_False;
	}

	if (resultat->Type() == a_int || resultat->Type() == a_long) {
		long l = resultat->Integer();
		return ConvertToPythonLong(l);
	}

	if (resultat->isNumber()) {
		double d = resultat->Float();
		return PyFloat_FromDouble(d);
	}

	PyObject* pcourant;
	PyObject* liste;

	if (resultat->isVectorContainer() || resultat->Type() == a_list) {
		int i = 0;
		liste = PyList_New(0);
		AtanorIteration* itr = resultat->Newiteration(false);
		for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
			pcourant = toPython(itr->IteratorValue());
			PyList_Insert(liste, i, pcourant);
			Py_DECREF(pcourant);
			i++;
		}
		itr->Release();
		return liste;
	}

	agnostring value;
	if (resultat->isMapContainer()) {
		liste = PyDict_New();
		AtanorIteration* itr = resultat->Newiteration(false);
		for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
			pcourant = toPython(itr->IteratorValue());
			value = itr->Keystring();
			PyDict_SetItemString(liste, STR(value), pcourant);
			Py_DECREF(pcourant);
		}
		itr->Release();
		return liste;
	}

	value = resultat->String();
#ifdef OLDPYTHON
	return PyUnicode_DecodeUTF8(value.c_str(), value.size(), NULL);
#else
	return PyUnicode_FromString(value.c_str());
#endif
	//return ConvertToPythonString(value.c_str());
}

static string python_error_string() {
	string err;
	PyObject *ptype = NULL;
	PyObject *pvalue = NULL;
	PyObject *ptraceback = NULL;
	PyErr_Fetch(&ptype, &pvalue, &ptraceback);
	if (pvalue != NULL) {
		PyObject* perr = PyObject_Str(pvalue);
		err = PyAsString(perr);
		Py_DECREF(perr);
		Py_XDECREF(pvalue);
	}
	if (ptype != NULL)
		Py_XDECREF(ptype);
	if (ptraceback != NULL)
		Py_XDECREF(ptraceback);

	return err;
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorpython::AddMethod(AtanorGlobal* global, string name, pythonMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorpython::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	init_python = false;

	Atanorpython::idtype = global->Getid("python");

	Atanorpython::AddMethod(global, "setpath", &Atanorpython::MethodSetpath, P_ONE, "setpath(path1,path2...): Set paths to look for Python imports.");
	Atanorpython::AddMethod(global, "run", &Atanorpython::MethodRun, P_ONE, "run(string code): Execute python code");
	Atanorpython::AddMethod(global, "import", &Atanorpython::MethodImport, P_ONE, "import(string python): import a python file");
	Atanorpython::AddMethod(global, "execute", &Atanorpython::MethodExecute, P_ATLEASTONE, "execute(string funcname,p1,p2...): execute a python function with p1,p2 as parameters");
	Atanorpython::AddMethod(global, "close", &Atanorpython::MethodClose, P_NONE, "close(): close the current session");

	global->newInstance[Atanorpython::idtype] = new Atanorpython(global);
	global->RecordMethods(Atanorpython::idtype, Atanorpython::exported);

	return true;
}



Atanor* Atanorpython::Put(Atanor* idx, Atanor* kval, short idthread) {
	return aTRUE;
}

Atanor* Atanorpython::Get(Atanor* context, Atanor* idx, short idthread) {
	return this;
}

//A sample of how to implement a method for your new class...
Atanor* Atanorpython::MethodSetpath(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	if (!init_python) {
		Py_Initialize();
		init_python = true;
	}

	string path;
	stringstream code;
	code << "import sys\n";
#ifdef WIN32
	string local("\\");
	string with("\\\\");
#endif
	for (int i = 0; i < callfunc->Size(); i++) {
		//0 is the first parameter and so on...
		path = callfunc->Evaluate(i, contextualpattern, idthread)->String();
#ifdef WIN32
		path = s_replacestring(path, local, with);
		code << "sys.path.append('" << path << "')\n";
#else
		code << "sys.path.append('" << path << "')\n";
#endif
	}
	PyRun_SimpleString(code.str().c_str());

	if (PyErr_Occurred()) {
		string err = "PYT(996):";
		err += python_error_string();
		return globalAtanor->Returnerror(err, idthread);
	}
	//you may return any value of course...
	return aTRUE;
}

Atanor* Atanorpython::MethodClose(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (init_python == false)
		return aFALSE;
	Py_Finalize();
	init_python = false;
	pModule = NULL;
	return aTRUE;
}

Atanor* Atanorpython::MethodRun(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	if (!init_python) {
		Py_Initialize();
		init_python = true;
	}

	//0 is the first parameter and so on...
	Atanor* kcmd = callfunc->Evaluate(0, contextualpattern, idthread);
	string code = kcmd->String();
	if (code != "") {
		PyRun_SimpleString(code.c_str());

		if (PyErr_Occurred()) {
			string err = "PYT(997):";
			err += python_error_string();
			return globalAtanor->Returnerror(err, idthread);
		}
	}
	//you may return any value of course...
	return aTRUE;
}

Atanor* Atanorpython::MethodImport(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (pModule != NULL)
		return globalAtanor->Returnerror("PYT(020): Module already imported", idthread);

	if (!init_python) {
		Py_Initialize();
		init_python = true;
	}

	//0 is the first parameter and so on...
	Atanor* kcmd = callfunc->Evaluate(0, contextualpattern, idthread);
	pythonfilename = kcmd->String();
	if (pythonfilename != "") {
		PyObject* pName = ConvertToPythonString(pythonfilename.c_str());
		pModule = PyImport_Import(pName);
		Py_DECREF(pName);
	}

	if (PyErr_Occurred()) {
		string err = "PYT(998):";
		err += python_error_string();
		return globalAtanor->Returnerror(err, idthread);
	}

	//you may return any value of course...
	return aTRUE;
}

Atanor* Atanorpython::MethodExecute(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (pModule == NULL)
		return globalAtanor->Returnerror("PYT(002): No Python file in memory", idthread);
	int nbelements = callfunc->Size();
	if (nbelements == 0)
		return globalAtanor->Returnerror("PYT(003): Missing parameters", idthread);
	string funcname = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	PyObject* pFunc = PyObject_GetAttrString(pModule, STR(funcname));
	if (pFunc == NULL || !PyCallable_Check(pFunc))
		return globalAtanor->Returnerror("PYT(004): Unknown Python function", idthread);
	PyObject* pArgs = PyTuple_New(nbelements - 1);
	PyObject* pe;
	if (nbelements > 1) {
		for (int i = 1; i < nbelements; i++) {
			pe = toPython(callfunc->Evaluate(i, contextualpattern, idthread));
			PyTuple_SetItem(pArgs, i - 1, pe);
		}
	}
	pe = PyObject_CallObject(pFunc, pArgs);
	Py_DECREF(pArgs);
	if (PyErr_Occurred()) {
		if (pe != NULL)
			Py_DECREF(pe);
		string err = "PYT(999):";
		err += python_error_string();
		return globalAtanor->Returnerror(err, idthread);
	}
	if (pe != NULL) {
		Atanor* o = toAtanor(pe);
		Py_DECREF(pe);
		if (o == aRAISEERROR)
			return aNULL;
		return o;
	}
	return aTRUE;
}

//------- Python Lib section ---------------------------------
extern "C" {
	static PyObject* atanor_load(PyObject* self, PyObject* args) {
		char* filename;
		char* pargs;
		int mapping = 0;
		int nbelements = PyTuple_Size(args);
		if (nbelements == 2) {
			if (!PyArg_ParseTuple(args, "ss", &filename, &pargs)) {
				PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
				return ConvertToPythonLong(-1);
			}
		}
		else {
			if (!PyArg_ParseTuple(args, "ssi", &filename, &pargs, &mapping)) {
				PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
				return ConvertToPythonLong(-1);
			}
		}

		AtanorCreate(1000);
		AtanorSetArguments(pargs);
		string flname(filename);
		int handler = AtanorLoad(flname);
		if (handler == -1)
			return ConvertToPythonLong(-100);

		AtanorCode* atancode = AtanorCodeSpace(handler);

		short idthread = globalAtanor->GetThreadid();
		atancode->Execute(atancode->firstinstruction, idthread);

		if (globalAtanor->Error(globalAtanor->GetThreadid())) {
			string message = globalAtanor->Errorstring(idthread);
			cerr << STR(message) << ":" << handler << endl;
			PyErr_SetString(PyExc_AttributeError, STR(message));
			return ConvertToPythonLong(-50);
		}

		if (mapping == 1) {
			stringstream code;


			if (atancode == NULL) {
				PyErr_SetString(PyExc_AttributeError, "Unknown atanor space");
				return ConvertToPythonLong(-1);
			}

			basebin_hash<Atanor*>::iterator it;
			AtanorMainFrame* amf = (AtanorMainFrame*)atancode->Mainframe();
			Atanor* kfunc;
			string name;
			char var;
			for (it = amf->declarations.begin(); it != amf->declarations.end(); it++) {
				kfunc = it->second;
				if (kfunc != NULL && kfunc->isFunction()) {
					name = globalAtanor->Getsymbol(it->first);
					code << "def " << name << "(";
					var = 97;
					int i;
					for (i = 0; i < kfunc->Size(); i++) {
						if (i)
							code << ",";
						code << var;
						var++;
					}
					code << "):\n\treturn pyatan.execute(" << handler << ",'" << name << "'";
					var = 97;
					for (i = 0; i < kfunc->Size(); i++) {
						code << "," << var;
						var++;
					}
					code << ")\n";
				}
			}

			if (code.str().size() != 0)
				PyRun_SimpleString(code.str().c_str());
		}

		return ConvertToPythonLong(handler);
	}


	static PyObject* atanor_execute(PyObject* self, PyObject* args) {
		int nbelements = PyTuple_Size(args);

		if (nbelements < 2) {
			PyErr_SetString(PyExc_AttributeError, "Wrong parameters, at least two parameters are required: kif handler and function name");
			return ConvertToPythonLong(-1);
		}

		PyObject* pelement;
		pelement = PyTuple_GetItem(args, 0);
		if (!PyCheck_Number(pelement)) {
			PyErr_SetString(PyExc_AttributeError, "Expecting a kif handler");
			return ConvertToPythonLong(-2);
		}

		int handler = PyAsNumber(pelement);

		AtanorCode* atancode = AtanorCodeSpace(handler);
		if (atancode == NULL) {
			PyErr_SetString(PyExc_AttributeError, "This handler does not match an existing ATANOR space");
			return ConvertToPythonLong(-3);
		}


		pelement = PyTuple_GetItem(args, 1);
		if (!PyCheck_String(pelement)) {
			PyErr_SetString(PyExc_AttributeError, "Expecting a string");
			return ConvertToPythonLong(-4);
		}

		string functionname = PyAsString(pelement);

		vector<Atanor*> params;
		Atanor* ke;

		for (int i = 2; i < nbelements; i++) {
			pelement = PyTuple_GetItem(args, i);
			ke = toAtanor(pelement);
			if (ke == aRAISEERROR) {
				for (i = 0; i < params.size(); i++)
					params[i]->Release();
				PyErr_SetString(PyExc_AttributeError, "No mapping for this python element");
				return ConvertToPythonLong(-5);
			}
			params.push_back(ke);
		}

		short idthread = globalAtanor->GetThreadid();
		Atanor* resultat = AtanorExecute(atancode, functionname, params);

		if (globalAtanor->Error(idthread)) {
			resultat->Release();
			string value = globalAtanor->Errorstring(idthread);
			PyErr_SetString(PyExc_AttributeError, STR(value));
			return ConvertToPythonLong(-6);
		}

		pelement = toPython(resultat);

		resultat->Release();
		return pelement;
	}

	static PyObject* atanor_clean(PyObject* self, PyObject* args) {
		AtanorExtinguish();
		return ConvertToPythonLong(0);
	}

	static PyObject* pyatanError;

	static PyMethodDef pyatanMethods[] = {

		{ "load", atanor_load, METH_VARARGS, "load(filename,arguments) load a program and return a handler" },
		{ "execute", atanor_execute, METH_VARARGS, "execute(handler,function_name,arguments) Execute an ATANOR function  function, the argument should be a vector of strings" },
		{ "clean", atanor_clean, METH_VARARGS, "clean() Clean the Atanor space" },
		{ NULL, NULL, 0, NULL }
	};


#if PY_MAJOR_VERSION >= 3
	static struct PyModuleDef pyatanmodule = {
		PyModuleDef_HEAD_INIT,
		"pyatan",   /* name of module */
		NULL, /* module documentation, may be NULL */
		-1,       /* size of per-interpreter state of the module,
				  or -1 if the module keeps state in global variables. */
	    pyatanMethods
	};

	Exporting PyMODINIT_FUNC PyInit_pyatan(void) {
		PyObject *m;

		m = PyModule_Create(&pyatanmodule);
		if (m == NULL)
			return NULL;

		pyatanError = PyErr_NewException("pyatan.error", NULL, NULL);
		Py_INCREF(pyatanError);
		PyModule_AddObject(m, "error", pyatanError);

		return m;
	}

#else
	Exporting void initpyatan() {
		PyObject* module = Py_InitModule("pyatan", pyatanMethods);
		PyObject* pDict = PyModule_GetDict(module);
		pyatanError = PyErr_NewException("pyatan.error", NULL, NULL);
		PyDict_SetItemString(pDict, "error", pyatanError);
	}
#endif
}





