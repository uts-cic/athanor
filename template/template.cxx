#include "atanor.h"
#include "atanorversion.h"
#include "atanor%1.h"

//We need to declare once again our local definitions.
hmap<unsigned short, %1Method>  Atanor%1::methods;
hmap<string, string> Atanor%1::infomethods;
bin_hash<unsigned long> Atanor%1::exported;

short Atanor%1::idtype = 0;

//------------------------------------------------------------------------------------------------------------------
//If you need to implement an external library... Otherwise remove it...
//When ATHANOR loads a library, it looks for this method in particular, which then make this object available into ATHANOR
extern "C" {
    Exporting bool %1_InitialisationModule(AtanorGlobal* global, string version) {
        if (version != AtanorVersion())
            return false;

        global->Update();

        return Atanor%1::InitialisationModule(global, version);
    }
}
//------------------------------------------------------------------------------------------------------------------

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanor%1::AddMethod(AtanorGlobal* global, string name, %1Method func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Atanor%1::InitialisationModule(AtanorGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    //Each new object has a specific name, which will help recognize it in the code that will exploit %1...
    Atanor%1::idtype = global->Getid("%1");

    //You declare your methods here:
    // Argument 1 is a call to global, which will record your method into ATHANOR
    // Argument 2 is the name of your command
    // Argument 3 links your command with its implementation into the object class
    // Argument 4 is a combination of P_... which defines how many parameters this function can accept.
    //Argument 5 is a description of this method.
    Atanor%1::AddMethod(global, "command", &Atanor%1::MethodCommand, P_ONE | P_TWO, "command(string test): A sample of how to implement a new method for your object.");


    //We need this code, in order to create new instances of our %1 object... DO NOT ALTER
    global->newInstance[Atanor%1::idtype] = new Atanor%1(global);
    global->RecordMethods(Atanor%1::idtype,Atanor%1::exported);
    
    return true;
}

AtanorIteration* Atanor%1::Newiteration(bool direction) {
    //If you do not need an iteration, do:
    //return aITERNULL;
    //and remove the AtanorIteration%1 class implementation in the include file...
    return new AtanorIteration%1(this, direction);
}


Atanor* Atanor%1::Put(Atanor* idx, Atanor* kval, short idthread) {
    //If you want to put some element into your object
    return aTRUE;
}

Atanor* Atanor%1::Get(Atanor* context, Atanor* idx, short idthread) {
    //if your object is called from within an expression...
    return this;
}

//A sample of how to implement a method for your new class...
Atanor* Atanor%1::MethodCommand(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
    
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
