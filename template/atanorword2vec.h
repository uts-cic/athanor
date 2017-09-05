#ifndef atanorword2vec_h
#define atanorword2vec_h

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorword2vec;
//This typedef defines a type "word2vecMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorword2vec::*word2vecMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------
class AtanorIterationword2vec : public AtanorIteration {
    public:

    AtanorIterationword2vec(Atanorword2vec* v, bool d, AtanorGlobal* g = NULL) : AtanorIteration(d, g) {}

    Atanor* Key() {
        return aNULL;
    }

    Atanor* Value() {
        return aNULL;
    }

    string Keystring() {
        return "";
    }

    string Valuestring() {
        return "";
    }

    wstring Keyustring() {
        return L"";
    }

    wstring Valueustring() {
        return L"";
    }

    long Keyinteger() {
        return 0;
    }

    long Valueinteger() {
        return 0;
    }

    double Keyfloat() {
        return 0;
    }

    double Valuefloat() {
        return 0;
    }

    void Next() {}

    Atanor* End() {
        return aTRUE;
    }

    Atanor* Begin() {
        return aFALSE;
    }

};
//---------------------------------------------------------------------------------------------------------------------

class Atanorword2vec : public AtanorObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static hmap<unsigned short, word2vecMethod> methods;
    static hmap<string, string> infomethods;
    static bin_hash<unsigned long> exported;

    static short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...

    //---------------------------------------------------------------------------------------------------------------------
    Atanorword2vec(AtanorGlobal* g = NULL, Atanor* parent = NULL) : AtanorObject(g, parent) {
        //Do not forget your variable initialisation
    }

    //----------------------------------------------------------------------------------------------------------------------
    Atanor* Put(Atanor* v, Atanor* i, short idthread);
    
    Atanor* Get(Atanor* context, Atanor* v, short idthread);


    short Type() {
        return Atanorword2vec::idtype;
    }

    string Typename() {
        return "word2vec";
    }

    //The next two functions work in conjunction...
    //If your object is very atomic (such as a number or a string)
    //Then when it is passed to a function or stored into a container,
    //it might be worth duplicating it.
    // In that case: duplicateForCall should return true...
    
    //However if your object is complex and probably unique through out the code
    //Then duplicateForCall should return false, and Atom should always reduce to a "return this;" only...
    Atanor* Atom(bool forced=false) {
        if (forced || !protect)
            return new  Atanorword2vec;
        return this;
    }

    bool duplicateForCall() {
        return true;
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
        return new Atanorword2vec;
    }

    AtanorIteration* Newiteration(bool direction);

    static void AddMethod(AtanorGlobal* g, string name, word2vecMethod func, unsigned long arity, string infos);
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


    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is a sample  of a function that could be implemented for your needs.
    
    Atanor* MethodCommand(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);


    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
    //is returned by the Declaration method.
    Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods[idname])(contextualpattern, idthread, callfunc);
    }

    void Clear(AtanorGlobal* g) {
        //To set a variable back to empty
    }

    void Clean() {
        //Your cleaning code
    }

    string String() {
        return "";
    }

    wstring UString() {
        return L"";
    }

    long Integer() { return 0; }
    short Short() { return 0;}
    double Float() { return 0; }
    bool Boolean() { return false; }
    BLONG Long() { return 0; }
    unsigned char Byte() {
        return 0;
    }

    long Size() { return 0; }

    //Basic operations: we add the current value with "a"
        Atanor* andset(Atanor* a, bool itself) {
            return this;
        }

        Atanor* orset(Atanor* a, bool itself) {
            return this;
        }

        Atanor* xorset(Atanor* a, bool itself) {
            return this;
        }

        Atanor* in(Atanor* context, Atanor* a, short idthread) {
            return aNULL;
        }

        Atanor* plusplus() {
            return this;
        }

        Atanor* minusminus() {
            return this;
        }

        double Sum() {
            return 0;
        }

        double Product() {
            return 1;
        }

        Atanor* plus(Atanor* a, bool itself) {
            return this;
        }
        Atanor* minus(Atanor* a, bool itself) {
            return this;
        }
        Atanor* multiply(Atanor* a, bool itself) {
            return this;
        }
        Atanor* divide(Atanor* a, bool itself) {
            return this;
        }
        Atanor* power(Atanor* a, bool itself) {
            return this;
        }
        Atanor* shiftleft(Atanor* a, bool itself) {
            return this;
        }
        Atanor* shiftright(Atanor* a, bool itself) {
            return this;
        }
        Atanor* mod(Atanor* a, bool itself) {
            return this;
        }

        Atanor* less(Atanor* a) {
            return aFALSE;
        }

        Atanor* more(Atanor* a) {
            return aFALSE;
        }

        Atanor* same(Atanor* a) {
            if (a == this)
                return aTRUE;
            return aFALSE;
        }

        Atanor* different(Atanor* a) {
            if (same(a) == aFALSE)
                return aTRUE;
            return aFALSE;
        }

        Atanor* lessequal(Atanor* a) {
            return aFALSE;
        }

        Atanor* moreequal(Atanor* a) {
            return aFALSE;
        }


    };
    #endif
