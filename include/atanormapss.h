
/*
*  Athanor: Le langage des Alpages m�de � Grenoble
*
*  Copyright (C) 2017: ATHANOR Language
* This file can only be used with the ATHANOR library or the executable
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : ATHANOR
Version    : ATANOR 0.26
filename   : atanormapss.h
Date       : 2017/04/01
Purpose    : map implementation
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef Atanormapss_h
#define Atanormapss_h

//#prime
#include "atanorint.h"
#include "atanorstring.h"
#include "atanorvector.h"
#include "atanorconstants.h"
#include "atanorsvector.h"
#include "atanorsvector.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanormapss;
//This typedef defines a type "mapssMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanormapss::*mapssMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------
class Atanormapss : public AtanorObject{
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging hmap<unsigned short, mapssMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    hmap<string, string > values;
    bool isconst;

    //---------------------------------------------------------------------------------------------------------------------
    Atanormapss(AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
        //Do not forget your variable initialisation
        isconst = false;

    }

    Atanormapss() {
        //Do not forget your variable initialisation
        isconst = false;

    }

    //----------------------------------------------------------------------------------------------------------------------
    Exporting Atanor* Loopin(AtanorInstruction* ins, Atanor* context, short idthread);
    Exporting Atanor* Put(Atanor* index, Atanor* value, short idthread);
    Exporting Atanor* Get(Atanor* context, Atanor* value, short idthread);

    void SetConst() { isconst = true;}

    short Type() {
        return Atanormapss::idtype;
    }

    string Typename() {
        return "mapss";
    }

    bool isString() {
        return true;
    }

    bool isContainer() {
        return true;
    }

    bool isMapContainer() {
        return true;
    }

    bool isValueContainer() {
        return true;
    }

    Atanor* Atom(bool forced) {
        if (forced || !protect || reference) {
            Locking _lock(this);
            Atanormapss * m = globalAtanor->Providemapss();
            m->values = values;
            return m;
        }
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


    Atanor* Newvalue(Atanor* a, short idthread) {
        Atanormapss* m = globalAtanor->Providemapss();
        if (a->isContainer()) {
            AtanorIteration* it = a->Newiteration(false);
            for (it->Begin(); it->End() == aFALSE; it->Next()) {
                m->values[it->Keystring()] = it->Valuestring();
            }
            it->Release();
        }
        return m;
    }

    Atanor* Newinstance(short idthread, Atanor* f = NULL) {
        return globalAtanor->Providemapss();
    }

    Exporting AtanorIteration* Newiteration(bool direction);

    bool duplicateForCall() {
        return isconst;
    }




    static void AddMethod(AtanorGlobal* global, string name, mapssMethod func, unsigned long arity, string infos);
    static bool InitialisationModule(AtanorGlobal* global, string version);

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



    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.

    
    Atanor*
    MethodInvert(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        if (contextualpattern == this || !contextualpattern->isMapContainer() || !contextualpattern->isAffectation())
            contextualpattern = Newinstance(idthread);
        else
            contextualpattern->Clear();

        Atanor* a;

        for (auto& it : values) {
            a = globalAtanor->Providestring(it.first);
            contextualpattern->Push(it.second, a);
            a->Release();
        }

        return contextualpattern;
    }

    Exporting Atanor* MethodFind(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);


    Atanor* MethodClear(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Clear();
        return aTRUE;
    }



    Atanor* MethodItems(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

        Atanormapss* item;
        Atanor* vect = Selectavector(contextualpattern);
        for (auto& itr : values) {
            item = globalAtanor->Providemapss();
            item->values[itr.first] = itr.second;
            vect->Push(item);
        }
        return vect;
    }

    Atanor* MethodMerge(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {


        Atanor* v = callfunc->Evaluate(0, contextualpattern, idthread);
        Merging(v);
        return this;
    }

    Atanor* MethodKeys(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Locking _lock(this);
        Atanorsvector * vstr = (Atanorsvector *)Selectasvector(contextualpattern);
        hmap<string, string > ::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            vstr->values.push_back(it->first);
        return vstr;
    }

    Atanor* MethodValues(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Locking _lock(this);
        Atanorsvector * vstr = (Atanorsvector *)Selectasvector(contextualpattern);
        hmap<string, string > ::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            vstr->values.push_back(it->second);
        return vstr;
    }

    Atanor* MethodTest(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Locking _lock(this);
        string  v = callfunc->Evaluate(0, contextualpattern, idthread)->String();
        if (values.find(v) == values.end())
            return aFALSE;
        return aTRUE;
    }

    Atanor* MethodPop(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Locking _lock(this);
        Atanor* pos = callfunc->Evaluate(0, contextualpattern, idthread);
        return Pop(pos);
    }

    Atanor* MethodJoin(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Locking _lock(this);
        //The separator between keys
        string keysep = callfunc->Evaluate(0, contextualpattern, idthread)->String();
        //The separator between values
        string sep = callfunc->Evaluate(1, contextualpattern, idthread)->String();

        bool beg = true;
        stringstream res;
        for (auto& it : values) {
            if (beg == false)
                res << sep;
            beg = false;
            res << it.first << keysep << it.second;
        }

        return globalAtanor->Providestring(res.str());
    }


    //------------------------------------------------------------------------------------------
    Exporting Atanor* Push(Atanor* k, Atanor* v);
    Exporting Atanor* Pop(Atanor* kkey);

    Atanor* Push(string k, Atanor* a) {
        Locking _lock(this);
        a = a->Atom();
        values[k] = a->String();
        a->Setreference(reference + 1);
        return this;
    }

    Atanor* Merging(Atanor* ke) {
        //Three cases:
        if (!ke->isContainer())
            return this;
        Doublelocking(this, ke);


        AtanorIteration* itr = ke->Newiteration(false);
        for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
            string n = itr->Keystring();
            if (values.find(n) == values.end())
                values[n] = itr->Valuestring();
        }
        itr->Release();
        return this;
    }

    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
    //is returned by the Declaration method.
    Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods[idname])(contextualpattern, idthread, callfunc);
    }

    Exporting void Clear();
    Exporting void Clean();

    Exporting string String();
    Exporting string JSonString();

    Atanor* Value(string n) {
        if (values.find(n) == values.end())
            return aNOELEMENT;
        return globalAtanor->Providestring(values[n]);
    }

    Atanor* Value(long n) {
        stringstream s;
        s << n;
        if (values.find(s.str()) == values.end())
            return aNOELEMENT;
        return globalAtanor->Providestring(values[s.str()]);
    }

    Atanor* Value(double n) {
        stringstream s;
        s << n;
        if (values.find(s.str()) == values.end())
            return aNOELEMENT;
        return globalAtanor->Providestring(values[s.str()]);
    }

    Exporting long Integer();
    Exporting double Float();
    Exporting BLONG Long();
    Exporting bool Boolean();


    //Basic operations
    Exporting long Size();

    Exporting Atanor* in(Atanor* context, Atanor* a, short idthread);

    Exporting Atanor* andset(Atanor* a, bool itself);
    Exporting Atanor* orset(Atanor* a, bool itself);
    Exporting Atanor* xorset(Atanor* a, bool itself);
    Exporting Atanor* plus(Atanor* a, bool itself);
    Exporting Atanor* minus(Atanor* a, bool itself);

    Exporting Atanor* same(Atanor* a);

};

//--------------------------------------------------------------------------------------------------
class AtanorIterationmapss : public AtanorIteration{
    public:

    hmap<string, string > ::iterator it;
    Atanormapss map;

    AtanorIterationmapss(Atanormapss * m, bool d, AtanorGlobal* g = NULL) : AtanorIteration(d, g) {
        map.values = m->values;
    }

    Atanor* Key() {
        return globalAtanor->Providestring(it->first);
    }

    Atanor* Value() {
        return globalAtanor->Providestring(it->second);
    }

    string Keystring() {
        return it->first;
    }

    string Valuestring() {
        return it->second;
    }

    long Keyinteger() {
        return convertinteger(it->first);
    }


    double Keyfloat() {
        return convertfloat(it->first);
    }

    long Valueinteger() {
        return convertinteger(it->second);
    }


    double Valuefloat() {
        return convertfloat(it->second);
    }

    void Next() {
        it++;
    }

    Atanor* End() {
        if (it == map.values.end())
            return aTRUE;
        return aFALSE;
    }

    Atanor* Begin() {
        it = map.values.begin();
        return aTRUE;
    }

};
//-------------------------------------------------------------------------------------------
class Atanormapssbuff : public Atanormapss {
    public:
    long idx;
    bool used;

    Atanormapssbuff(long i)  {
        //Do not forget your variable initialisation
        isconst = false;

        idx = i;
        used = false;
    }

    void Setreference(short r) {
        Locking _lock(this);
        reference += r;
        protect = false;
    }

    void Resetreference(short r) {
        bool clr = false;
        {
            Locking _lock(this);
            reference -= r;
            if (reference <= 0) {
                if (!protect) {
                    values.clear();
                    reference = 0;
                    protect = true;
                    clr = true;
                }
            }
        }

        if (clr) {
            Locking _lock(globalAtanor->mapsslock);
            used = false;
            globalAtanor->mapssempties.push_back(idx);
        }
    }

};
//-------------------------------------------------------------------------------------------
#endif