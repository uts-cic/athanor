
/*
* Xerox Research Centre Europe - Grenoble Laboratory (XRCE)
*
* Copyright (C) 2016 - 2017 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
* This file can only be used with the ATANOR library or the executable
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : ATHANOR
Version    : ATANOR 0.26
filename   : atanorsqlite.h
Date       : 2017/02/08
Purpose    : 
Programmer : Claude ROUX (claude.roux@xrce.xerox.com)
Reviewer   :
*/

#ifndef atanorsqlite_h
#define atanorsqlite_h

#include "sqlite3.h"
#include "atanorvector.h"
#include "atanormap.h"
#include "atanorint.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorsqlite;
//This typedef defines a type "sqliteMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorsqlite::*sqliteMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

class Atanorsqlite;

class AtanorIterationsqlite : public AtanorIteration {
public:
	Atanorsqlite* value;
	sqlite3_stmt *stmt;
	int returncode;
	vector<string> columnnames;
	int columnCount;
	Atanormap* currentrow;
	int nb;
	long garbage_i;

	void Storevalue();

	AtanorIterationsqlite(Atanorsqlite* v, AtanorGlobal* g);

	Atanor* Key() {
		return new Atanorint(nb);
	}

	Atanor* Value();
	void Next();

	Atanor* End();
	Atanor* Begin();
};

//---------------------------------------------------------------------------------------------------------------------

class Atanorsqlite : public AtanorObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static hmap<short, sqliteMethod> methods;
    static hmap<string, string> infomethods;
    static bin_hash<unsigned long> exported;

    static short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
	
	string command;
	string dbname;
	sqlite3 *db;
	string sqlcommand;
	string bindcommand;
	sqlite3_stmt* stmt;
	vector<int> insertstructure;


    //---------------------------------------------------------------------------------------------------------------------
    Atanorsqlite(AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
        //Do not forget your variable initialisation
		
		dbname = "";
		db = NULL;
		sqlcommand = "";
		stmt = NULL;
	}

	~Atanorsqlite() {
		if (db != NULL)
			sqlite3_close(db);
	}


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


    //----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* v, Atanor* i, short idthread);
    
	Atanor* Get(Atanor* context, Atanor* v, short idthread);


    short Type() {
        return Atanorsqlite::idtype;
    }

    string Typename() {
        return "sqlite";
    }

    bool isString() {
        return false;
    }

    bool isNumber() {
        return false;
    }

    bool isBoolean() {
        return false;
    }

    bool isFrame() {
        return false;
    }

    Atanor* Atom(bool forced=false) {
        return this;
    }

	//If it is too complex to duplicate an element (for instance when passed to a function)
	//then you can use return false...  Usually in that case, Atom should always be just: return this;
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

    Atanor* Newinstance(short, Atanor* f = NULL) {
		return new Atanorsqlite(globalAtanor);
    }

	AtanorIteration* Newiteration(bool direction) {
		return new AtanorIterationsqlite(this, globalAtanor);
    }

    static void AddMethod(AtanorGlobal* g, string name, sqliteMethod func, unsigned long arity, string infos);
    static bool InitialisationModule(AtanorGlobal* g, string version);

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.

	Atanor* MethodOpen(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodClose(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodCreate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodInsert(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodInsertThroughBind(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodRun(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodExecute(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodBegin(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodCommit(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodMemory(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);


    

    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
    //is returned by the Declaration method.
    Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods[idname])(contextualpattern, idthread, callfunc);
    }

    void Clear() {
        //To set a variable back to empty
    }

    void Clean() {
        //Your cleaning code
    }

    string String() {
        Locking _lock(this);
        return "";
    }

    //wstring UString() {
    //   Locking _lock(this);
    //  return L"";
    //	}

    long Integer() {
        Locking _lock(this);
        return 0;
    }
    double Float() {
        Locking _lock(this);
        return 0;
    }
    BLONG Long() {
        Locking _lock(this);
        return 0;
    }

    bool Boolean() {
        Locking _lock(this);
        return false;
    }

    
    //Basic operations
    long Size() {
        Locking _lock(this);
        return 0;
    }
    
};

#endif


