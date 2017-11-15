
/*
* Xerox Research Centre Europe - Grenoble Laboratory (XRCE)
*
* ATHANOR Language
* This file can only be used with the ATANOR library or the executable
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : ATHANOR
Version    : ATANOR 0.26
filename   : atanortable.h
Date       : 2017/04/01
Purpose    : table implementation
Programmer : Claude ROUX (claude.roux@xrce.xerox.com)
Reviewer   :
*/

#ifndef atanortable_h
#define atanortable_h

#include "atanorint.h"
#include "atanorvector.h"
#include "atanorfloat.h"
#include "atanorstring.h"

class AtanorCallFunctionHaskell;
class AtanorFunctionLambda;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanortable;
//This typedef defines a type "tableMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanortable::*tableMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------
class AtanorIterationtable;

//---------------------------------------------------------------------------------------------------------------------

class Atanortable : public AtanorObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging hmap<unsigned short, tableMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    Atanor** values;
	long size;

    //---------------------------------------------------------------------------------------------------------------------
    Atanortable(AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
        //Do not forget your variable initialisation
		values = NULL;
		size = 0;
    }

	Atanortable(long sz = 0) {
        //Do not forget your variable initialisation
		values = NULL;
		size = 0;
		if (sz)
			Resize(sz);		
    }

	~Atanortable() {
		if (values != NULL)
			delete[] values;
	}

    //----------------------------------------------------------------------------------------------------------------------
    Exporting Atanor* Put(Atanor* index, Atanor* value, short idthread);
    Exporting Atanor* Get(Atanor* context, Atanor* value, short idthread);


    Exporting Atanor* Loophaskell(Atanor* recipient, Atanor* context, Atanor* env, AtanorFunctionLambda* bd, short idthread);
    Exporting Atanor* Filter(short idthread, Atanor* env, AtanorFunctionLambda* bd, Atanor* var, Atanor* kcont, Atanor* accu, Atanor* init, bool direct);

    short Type() {
        return Atanortable::idtype;
    }

    string Typename() {
        return "table";
    }

    bool isContainer() {
        return true;
    }

	bool Resize(long sz) {
		if (sz <= size)
			return true;

		long i;

		if (values == NULL) {
			values = new Atanor*[sz];
			for (i = 0; i < sz; i++)
				values[i] = aNOELEMENT;
			size = sz;
			return true;
		}

		if (globalAtanor->globalLOCK)
			return false;

		Atanor** v = new Atanor*[sz];
		for (i = 0; i < sz; i++) {
			if (i < size)
				v[i] = values[i];
			else
				v[i] = aNOELEMENT;
		}

		size = sz;
		delete[] values;
		values = v;
		return true;
	}

    Atanor* Atom(bool forced = false) {
        if (forced) {
            Atanortable* v = new Atanortable;
			if (size) {
				v->Resize(size);
				Atanor* a;
				for (size_t i = 0; i < size; i++) {
					a = values[i]->Atom(true);
					a->Setreference();
					v->values[i] = a;
				}
			}
            return v;
        }
        return this;
    }

    bool duplicateForCall() {
        return false;
    }

    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in kifexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        if (exported.find(n) != exported.end())
            return true;
        return false;
    }

    Atanor* Newinstance(short idthread, Atanor* f = NULL) {
        return new Atanortable();
    }

    Atanor* Newvalue(Atanor* a, short idthread) {
        Atanortable* v = new Atanortable();
        if (a->isContainer()) {
			v->Resize(a->Size());
            AtanorIteration* it = a->Newiteration(false);
			long i = 0;
			for (it->Begin(); it->End() == aFALSE; it->Next()) {
				v->values[i] = it->IteratorValue();
				i++;
            }
            it->Release();
        }
        return v;
    }


    Exporting AtanorIteration* Newiteration(bool direction);

    static void AddMethod(AtanorGlobal* g, string name, tableMethod func, unsigned long arity, string infos);
    
    static bool InitialisationModule(AtanorGlobal* g, string version);

    
    void Methods(Atanor* v) {

        for (auto& it : infomethods)
            v->storevalue(it.first);
    }
    string Info(string n) {
        if (infomethods.find(n) != infomethods.end())
            return infomethods[n];
        return "Unknown method";
    }
    //---------------------------------------------------------------------------------------------------------------------
    Atanor* getvalue(BLONG i) {
        
        if (i < 0 || i >= size)
            return aNOELEMENT;
        return values[i];
    }
	Exporting void storevalue(string u);
	Exporting void storevalue(float u);
	Exporting void storevalue(short u);
	Exporting void storevalue(wstring u);
	Exporting void storevalue(long u);
	Exporting void storevalue(BLONG u);
	Exporting void storevalue(double u);
	Exporting void storevalue(unsigned char u);
	Exporting void storevalue(wchar_t u);
    //---------------------------------------------------------------------------------------------------------------------
	Exporting unsigned int EditDistance(Atanor* e);

    Exporting void Setreference(short r = 1);
    Exporting void Resetreference(short r = 1);

	void Setprotect(bool n) {
		protect = n;
		for (size_t i = 0; i < size; i++)
			values[i]->Setprotect(n);
	}

	void Protectcontainervalues() {
		protect = true;
		for (size_t i = 0; i < size; i++)
			values[i]->Setprotect(true);
	}
	
    void Popping() {       
        protect = false;
        if (reference <= 0)
            protect = true;
        for (size_t i = 0; i < size; i++)
            values[i]->Popping();
    }

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
    Atanor* MethodClear(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Clear();
        return aTRUE;
    }

	Atanor* MethodFlatten(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		Atanor* res = Selectavector(contextualpattern);
		Flattening(res, this);
		return res;
	}

	Atanor* MethodResize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		long v = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
		if (!Resize(v))
			return aFALSE;
		return aTRUE;
	}

	Atanor* MethodPush(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Atanor* v;
        for (size_t i = 0; i < callfunc->Size(); i++) {
            v = callfunc->Evaluate(i, contextualpattern, idthread);
			if (v->isError())
				return v;
            Push(v);
            v->Release();
        }
        return aTRUE;
    }

    Atanor* MethodLast(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        if (size == 0)
            return aNOELEMENT;
		long i = 0;
		return Last(i);
    }

    Atanor* MethodMerge(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Atanor* v = callfunc->Evaluate(0, contextualpattern, idthread);
        return Merging(v);
    }

    Atanor* MethodEditDistance(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Atanor* v = callfunc->Evaluate(0, contextualpattern, idthread);
        
        unsigned int dst = EditDistance(v);
        return globalAtanor->Provideint(dst);
    }

    Atanor* MethodSum(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        double v = Sum();
        return globalAtanor->Providefloat(v);
    }

    Atanor* MethodProduct(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        double v = Product();
        return globalAtanor->Providefloat(v);
    }

    Atanor* MethodInsert(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        long i = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        Atanor* v = callfunc->Evaluate(1, contextualpattern, idthread);
        Insert(i, v);
        return aTRUE;
    }

    Atanor* MethodPop(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Atanor* pos = aMINUSONE;
        if (callfunc->Size() == 1)
            pos = callfunc->Evaluate(0, contextualpattern, idthread);
        return Pop(pos);
    }


    Atanor* MethodReserve(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		long sz = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
		if (!Resize(sz))
			return aFALSE;

		return aTRUE;
	}

    Atanor* MethodJoin(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        //The separator between values
        string sep = callfunc->Evaluate(0, contextualpattern, idthread)->String();
        
        bool beg = true;
        string res;
        for (size_t it = 0; it<size;it++) {
            if (beg == false)
                res += sep;
            beg = false;
            res += values[it]->String();
        }

        return globalAtanor->Providestring(res);
    }

    Atanor* MethodUnique(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        return Unique();
    }

    Atanor* MethodShuffle(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Shuffle();
        return this;
    }

    Atanor* MethodReverse(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        return Inverse();
    }

    Atanor* MethodRemove(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Atanor* a = callfunc->Evaluate(0, contextualpattern, idthread);
        size_t found = -1;
        for (size_t i = 0; i < size; i++) {
            if (values[i]->same(a)->Boolean()) {
                found = i;
                break;
            }
        }
        if (found != -1) {
            values[found]->Resetreference(reference + 1);
			values[found] = aNOELEMENT;
            return aTRUE;
        }
        return aFALSE;
    }

    //---------------------------------------------------------------------------------------------------------

    //Raw push

	Atanor* Last(long& idx) {
		if (size == 0)
			return aNOELEMENT;

		for (long i = size - 1; i >= 0; i++) {
			if (values[i] != aNOELEMENT) {
				idx = i;
				return values[i];
			}
		}

		return aNOELEMENT;
	}
	
    Atanor* push(Atanor* v) {
		if (size == 0)
			return aNOELEMENT;

		for (long i = size - 1; i >= 0; i++) {
			if (values[i] == aNOELEMENT) {
				v = v->Atom();
				values[i] = v;
				v->Setreference(reference + 1);
				return this;
			}
		}

		return aNOELEMENT;
    }

	bool Pushing(Atanor* a, long i) {
		if (i < 0 || i >= size)
			return false;
		
		a = a->Atom();
		values[i] = a;
		a->Setreference(reference + 1);
		return true;
	}

    Exporting Atanor* Push(Atanor*);
    Exporting Atanor* Push(AtanorGlobal* g, Atanor* a, short idhtread);

    Exporting Atanor* Pop(Atanor*);
   
    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
    //is returned by the Declaration method.
    Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods[idname])(contextualpattern, idthread, callfunc);
    }

    Exporting Atanor* Sort(AtanorFunction* comp, short idthread, bool d);Exporting Atanor* Sortstring(short idthread, bool d);Exporting Atanor* Sortustring(short idthread, bool d);Exporting Atanor* Sortint(short idthread, bool d);Exporting Atanor* Sortfloat(short idthread, bool d);Exporting Atanor* Unique();
    Atanor* Inverse() {
        
        Atanortable* vect = new Atanortable;
        for (long i = size - 1; i >= 0; i--) {
            vect->Push(values[i]);
        }
        return vect;
    }

    Exporting void Shuffle();Exporting Atanor* Merging(Atanor*);
    Exporting void Insert(int idx, Atanor* ke);

    Exporting Atanor* Map(short idthread);

    Atanor* Vector(short idthread) {
		Atanorvector* vect = new Atanorvector;
		for (long i = 0; i < size; i++)
			vect->Push(values[i]);
		return vect;
    }

	Atanor* Thesum(long i, long j) {
		long sz = size;
		if (!sz)
			return aNULL;

		if (i < 0)
			i += sz;
		if (j < 0)
			j += sz;

		if (i < 0)
			i = 0;
		else
		if (i >= sz)
			i = sz - 1;

		if (j <= i)
			j = i + 1;
		else
		if (j>sz)
			j = sz;

		if (values[0]->isNumber()) {
			double v = 0;
			for (; i < j; i++)
				v += values[i]->Float();

			return new Atanorfloat(v);
		}

		string v;
		for (; i < j; i++)
			v += values[i]->String();
		return new Atanorstring(v);
	}

	Atanor* Theproduct(long i, long j) {
		long sz = size;
		if (!sz)
			return aNULL;

		if (i < 0)
			i += sz;
		if (j < 0)
			j += sz;

		if (i < 0)
			i = 0;
		else
		if (i >= sz)
			i = sz - 1;

		if (j <= i)
			j = i + 1;
		else
		if (j>sz)
			j = sz;

		double v = 1;
		for (; i < j; i++)
			v *= values[i]->Float();

		return new Atanorfloat(v);
	}

    double Sum() {
        
        double v = 0;
        for (int i = 0; i < size; i++)
            v += values[i]->Float();
        return v;
    }

    double Product() {
        
        if (size == 0)
            return 0;
        double v = values[0]->Float();
        for (int i = 1; i < size; i++)
            v *= values[i]->Float();
        return v;
    }


    Exporting void Clear();
    Exporting void Clean();

    Exporting string JSonString();
    Exporting string String();

    //wstring UString() {}

    Exporting long Integer();
    Exporting double Float();
    Exporting BLONG Long();
    Exporting bool Boolean();


    Exporting long Size();

    Exporting Atanor* in(Atanor* context, Atanor* a, short idthread);

    Exporting Atanor* andset(Atanor* a, bool itself);
    Exporting Atanor* orset(Atanor* a, bool itself);
    Exporting Atanor* xorset(Atanor* a, bool itself);
    Exporting Atanor* plus(Atanor* a, bool itself);
    Exporting Atanor* minus(Atanor* a, bool itself);
    Exporting Atanor* multiply(Atanor* a, bool itself);
    Exporting Atanor* divide(Atanor* a, bool itself);
    Exporting Atanor* power(Atanor* a, bool itself);
    Exporting Atanor* shiftleft(Atanor* a, bool itself);
    Exporting Atanor* shiftright(Atanor* a, bool itself);
    Exporting Atanor* mod(Atanor* a, bool itself);

    Exporting Atanor* same(Atanor* a);

};
//---------------------------------------------------------------------------------
class AtanorIterationtable : public AtanorIteration {
    public:
    BLONG itx;
    Atanortable* ref;
    Locking _lock;

    AtanorIterationtable(Atanortable* v, bool d, AtanorGlobal* g = NULL) : _lock(v), AtanorIteration(d, g) {
        ref = v;
        itx = 0;
    }

    Atanor* Key() {
        return globalAtanor->Provideint(itx);
    }

    Atanor* Value() {
        return ref->values[itx];
    }

    string Keystring() {
        stringstream s;
        s << itx;
        return s.str();
    }

    string Valuestring() {
        return ref->values[itx]->String();
    }

    long Keyinteger() {
        return itx;
    }

    long Valueinteger() {
        return ref->values[itx]->Integer();
    }

    double Keyfloat() {
        return itx;
    }

    float Valuedecimal() {
        return ref->values[itx]->Decimal();
    }

    double Valuefloat() {
        return ref->values[itx]->Float();
    }

    void Next() {
        if (reverse)
            itx--;
        else
            itx++;
    }

    Atanor* End() {
        if (reverse) {
            if (itx < 0)
                return aTRUE;
        }
        else {
            if (itx >= ref->size)
                return aTRUE;
        }
        return aFALSE;
    }

    Atanor* Begin() {
        if (reverse) {
            itx = ref->size - 1;
        }
        else
            itx = 0;
        return aTRUE;
    }

};

#endif


