
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
filename   : atanorprimemapl.h
Date       : 2017/04/01
Purpose    : map implementation
Programmer : Claude ROUX (claude.roux@xrce.xerox.com)
    Reviewer   :
*/

#ifndef Atanorprimemapl_h
#define Atanorprimemapl_h

#include "fractalhmap.h"
#include "atanorint.h"
#include "atanorstring.h"
#include "atanorvector.h"
#include "atanorconstants.h"
#include "atanorlvector.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorprimemapl;
//This typedef defines a type "primemaplMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorprimemapl::*primemaplMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------
class Atanorprimemapl : public AtanorObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging hmap<unsigned short, primemaplMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    prime_hash<BLONG, Atanor*> values;
    bool isconst;

    //---------------------------------------------------------------------------------------------------------------------
    Atanorprimemapl(AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
        //Do not forget your variable initialisation
        isconst = false;

    }

    Atanorprimemapl() {
        //Do not forget your variable initialisation
        isconst = false;

    }

    //----------------------------------------------------------------------------------------------------------------------
    Exporting Atanor* Loopin(AtanorInstruction*, Atanor* context, short idthread);
    Exporting Atanor* Put(Atanor* index, Atanor* value, short idthread);
    Exporting Atanor* Get(Atanor* context, Atanor* value, short idthread);

    void SetConst() { isconst = true;}

    short Type() {
        return Atanorprimemapl::idtype;
    }

    string Typename() {
        return "primemapl";
    }

    bool isContainer() {
        return true;
    }

    bool isMapContainer() {
        return true;
    }

    bool duplicateForCall() {
        return isconst;
    }

    void Setprotect(bool n) {
        protect = n;
        prime_hash<BLONG, Atanor*>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            it->second->Setprotect(n);
    }

    void Protectcontainervalues() {
        protect = true;
        prime_hash<BLONG, Atanor*>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            it->second->Setprotect(true);
    }


    Atanor* Atom(bool forced) {
        if (forced) {
            Atanorprimemapl* m = new Atanorprimemapl;
            Locking _lock(this);
            Atanor* v;
            prime_hash<BLONG, Atanor*>::iterator it;
            for (it = values.begin(); it != values.end(); it++) {
                v = it->second->Atom();
                m->values[it->first] = v;
                v->Setreference();
            }
            return m;
        }
        return this;
    }

    double Sum() {
        Locking _lock(this);
        double v = 0;
        prime_hash<BLONG, Atanor*>::iterator itx;
        for (itx = values.begin(); itx != values.end(); itx++)
            v += itx->second->Sum();
        return v;
    }

    double Product() {
        Locking _lock(this);
        double v = 1;
        prime_hash<BLONG, Atanor*>::iterator itx;
        for (itx = values.begin(); itx != values.end(); itx++)
            v *= itx->second->Product();
        return v;
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
        Atanorprimemapl* m = new Atanorprimemapl;
        if (a->isContainer()) {
            AtanorIteration* it = a->Newiteration(false);
            for (it->Begin(); it->End() == aFALSE; it->Next()) {
                m->Push(it->Keylong(),it->IteratorValue());
            }
            it->Release();
        }
        return m;
    }

    Atanor* Newinstance(short idthread, Atanor* f = NULL) {
        return new Atanorprimemapl();
    }

    Exporting AtanorIteration* Newiteration(bool direction);


    static void AddMethod(AtanorGlobal* global, string name, primemaplMethod func, unsigned long arity, string infos);
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
    Exporting void Setreference(short r = 1);
    Exporting void Resetreference(short r = 1);
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
        prime_hash<BLONG, Atanor*>::iterator it;
        for (it = values.begin(); it != values.end(); it++) {
            a = new Atanorlong(it->first);
            contextualpattern->Push(it->second, a);
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
        prime_hash<BLONG, Atanor*>::iterator itr;
        Atanorprimemapl* item;
        Atanor* vect = Selectavector(contextualpattern);
        for (itr=values.begin(); itr!=values.end();itr++) {
            item=new Atanorprimemapl;
            item->Push(itr->first, itr->second);
            vect->Push(item);
        }
        return vect;
    }
    
    Atanor* MethodMerge(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

        
        Atanor* v = callfunc->Evaluate(0, contextualpattern, idthread);
        Merging(v);
        return this;
    }

    Atanor* MethodSum(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Locking _lock(this);
        double v = Sum();
        return globalAtanor->Providefloat(v);
    }

    Atanor* MethodKeys(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Locking _lock(this);
        Atanorlvector* vstr = (Atanorlvector*)Selectalvector(contextualpattern);
        prime_hash<BLONG, Atanor*>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            vstr->values.push_back(it->first);
        return vstr;
    }

    Atanor* MethodValues(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Locking _lock(this);
        Atanorvector* v = (Atanorvector*)Selectavector(contextualpattern);
        prime_hash<BLONG, Atanor*>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            v->Push(it->second);
        return v;
    }

    Atanor* MethodTest(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Locking _lock(this);
        BLONG  v = callfunc->Evaluate(0, contextualpattern, idthread)->Long();
        if (values.find(v) == values.end())
            return aFALSE;
        return aTRUE;
    }

    Atanor* MethodProduct(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        Locking _lock(this);
        double v = Product();
        return globalAtanor->Providefloat(v);
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
        prime_hash<BLONG, Atanor*>::iterator it;
        bool beg = true;
        stringstream res;
        for (it = values.begin(); it != values.end(); it++) {
            if (beg == false)
                res << sep;
            beg = false;
            res << it->first << keysep << it->second->String();
        }

        return globalAtanor->Providestring(res.str());
    }


    //------------------------------------------------------------------------------------------
    Exporting Atanor* Push(Atanor* k, Atanor* v);
    
    
    Exporting Atanor* Pop(Atanor* k);
    Atanor* Push(BLONG k, Atanor* a) {

        Locking _lock(this);
        if (values.find(k) != values.end()) {
            Atanor* v = values[k];
            v->Resetreference(reference + 1);
        }
        a = a->Atom();
        values[k] = a;
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
            BLONG n = itr->Keylong();
            if (values.find(n) == values.end())
                Push(n, itr->Value());
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
        long v = convertlong(n);
        if (values.find(v) == values.end())
            return aNOELEMENT;
        return values[v];
    }

    Atanor* Value(BLONG n) {
        if (values.find(n) == values.end())
            return aNOELEMENT;
        return values[(BLONG)n];
    }

    Atanor* Value(long n) {
        if (values.find(n) == values.end())
            return aNOELEMENT;
        return values[(BLONG)n];
    }

    Atanor* Value(double n) {
        if (values.find(n) == values.end())
            return aNOELEMENT;
        return values[(BLONG)n];
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
    Exporting Atanor* multiply(Atanor* a, bool itself);
    Exporting Atanor* divide(Atanor* a, bool itself);
    Exporting Atanor* power(Atanor* a, bool itself);
    Exporting Atanor* shiftleft(Atanor* a, bool itself);
    Exporting Atanor* shiftright(Atanor* a, bool itself);
    Exporting Atanor* mod(Atanor* a, bool itself);

    Exporting Atanor* same(Atanor* a);

};

//--------------------------------------------------------------------------------------------------
class AtanorIterationprimemapl : public AtanorIteration {
    public:

    prime_hash<BLONG, Atanor*>::iterator it;
    Atanorprimemapl* map;
    Locking _lock;

    AtanorIterationprimemapl(Atanorprimemapl* m, bool d, AtanorGlobal* g = NULL) : map(m), _lock(m), AtanorIteration(d, g) {}

    Atanor* Key() {
        return new Atanorlong(it->first);
    }

    Atanor* Value() {
        return it->second;
    }

    string Keystring() {
        stringstream v;
        v << it->first;
        return v.str();
    }

    string Valuestring() {
        return it->second->String();
    }

    wstring Keyustring() {
        std::wstringstream v;
        v << it->first;
        return v.str();
    }

    wstring Valueustring() {
        return it->second->UString();
    }

    
    BLONG Keylong() {
        return it->first;
    }

    long Keyinteger() {
        return it->first;
    }

    
    BLONG Valuelong() {
        return it->second->Long();
    }

    long Valueinteger() {
        return it->second->Float();
    }

    double Keyfloat() {
        return it->first;
    }

    double Valuefloat() {
        return it->second->Float();
    }

    void Next() {
        it++;
    }

    Atanor* End() {
        if (it == map->values.end())
            return aTRUE;
        return aFALSE;
    }

    Atanor* Begin() {
        it = map->values.begin();
        return aTRUE;
    }

};
//-------------------------------------------------------------------------------------------

#endif
