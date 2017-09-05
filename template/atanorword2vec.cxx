#include "atanor.h"
#include "atanorversion.h"
#include "atanorword2vec.h"

//We need to declare once again our local definitions.
hmap<unsigned short, word2vecMethod>  Atanorword2vec::methods;
hmap<string, string> Atanorword2vec::infomethods;
bin_hash<unsigned long> Atanorword2vec::exported;

short Atanorword2vec::idtype = 0;

//------------------------------------------------------------------------------------------------------------------
//If you need to implement an external library... Otherwise remove it...
//When ATHANOR loads a library, it looks for this method in particular, which then make this object available into ATHANOR
extern "C" {
    Exporting bool word2vec_InitialisationModule(AtanorGlobal* global, string version) {
        if (version != AtanorVersion())
            return false;

        global->Update();

        return Atanorword2vec::InitialisationModule(global, version);
    }
}
//------------------------------------------------------------------------------------------------------------------

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorword2vec::AddMethod(AtanorGlobal* global, string name, word2vecMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanorword2vec::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    //Each new object has a specific name, which will help recognize it in the code that will exploit word2vec...
    Atanorword2vec::idtype = global->Getid("word2vec");

    //You declare your methods here:
    // Argument 1 is a call to global, which will record your method into ATHANOR
    // Argument 2 is the name of your command
    // Argument 3 links your command with its implementation into the object class
    // Argument 4 is a combination of P_... which defines how many parameters this function can accept.
    //Argument 5 is a description of this method.
    Atanorword2vec::AddMethod(global, "command", &Atanorword2vec::MethodCommand, P_ONE | P_TWO, "command(string test): A sample of how to implement a new method for your object.");


    //We need this code, in order to create new instances of our word2vec object... DO NOT ALTER
    global->newInstance[Atanorword2vec::idtype] = new Atanorword2vec(global);
    global->RecordMethods(Atanorword2vec::idtype,Atanorword2vec::exported);
    
    return true;
}

AtanorIteration* Atanorword2vec::Newiteration(bool direction) {
    //If you do not need an iteration, do:
    //return aITERNULL;
    //and remove the AtanorIterationword2vec class implementation in the include file...
    return new AtanorIterationword2vec(this, direction);
}


Atanor* Atanorword2vec::Put(Atanor* idx, Atanor* kval, short idthread) {
    //If you want to put some element into your object
    return aTRUE;
}

Atanor* Atanorword2vec::Get(Atanor* context, Atanor* idx, short idthread) {
    //if your object is called from within an expression...
    return this;
}

//A sample of how to implement a method for your new class...
Atanor* Atanorword2vec::MethodCommand(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
    
    //This how your get the number of arguments...
    long nbarguments = callfunc->Size();
    Atanor* arg = aNULL;
    //we know that we have one argument or two arguments for this function, as it was declared as such with AddMethod...
    arg = callfunc->Evaluate(0, contextualpattern, idthread);
    
    Atanor* argnext = aNULL;
    string sval;
    double fval = 0;
    //we have a second argument provided with the call...
    if (nbarguments == 2) {
        argnext = callfunc->Evaluate(1, contextualpattern, idthread);
        if (argnext->isString())
            sval=argnext->String();
        if (argnext->isNumber())
            fval=argnext->Float();
    }
    
    //This is how you can access the values of each of the available types...
    long i = arg->Integer();
    short h = arg->Short();
    string s = arg->String();
    wstring w= arg->UString();
    double v= arg->Float();
    float f= arg->Decimal();
    
    
    if (argnext->isVectorContainer()) {
        //In the case of a vector, it is much more effecient to use this kind of loop...
        if (arg->isVectorContainer()) {
            for (long i=0;i<arg->Size();i++) {
                string isval=arg->getstring(i);
                long iival=arg->getinteger(i);
                //etc.. all the other types are available
                Atanor* aval=arg->getvalue(i);
            }
            
            return arg;
        }
        
        //For another type of containers, you can use the generic iterator...
        if (arg->isContainer()) {
            //the parameter defines the order in which the elements are traversed
            AtanorIteration* iter=arg->Newiteration(false);
            for (iter->Begin(); iter->End() == aFALSE; iter->Next()) {
                string isval=iter->Keystring();
                isval=iter->Valuestring();
                long iival=iter->Keyinteger();
                iival=iter->Valueinteger();
                //etc.. all the other types are available
                //Very important, you must use IteratorKey and not Key
                argnext->Push(iter->IteratorKey());
                //Very important, you must use IteratorValue and not Value
                argnext->Push(iter->IteratorValue());
            }
        }
    }
    
    if (argnext->isMapContainer()) {
        argnext->Push(arg,arg);
        return argnext;
    }
    
    return arg;
}
