
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
filename   : atanorsqlite.cxx
Date       : 2017/04/01
Purpose    : SQLite library encapsulation
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorsqlite.h"
#include "atanormapss.h"

//We need to declare once again our local definitions.
hmap<short, sqliteMethod>  Atanorsqlite::methods;
hmap<string, string> Atanorsqlite::infomethods;
bin_hash<unsigned long> Atanorsqlite::exported;

short Atanorsqlite::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorsqlite::AddMethod(AtanorGlobal* global, string name, sqliteMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorsqlite::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorsqlite::idtype = global->Getid("sqlite");

	Atanorsqlite::AddMethod(global, "open", &Atanorsqlite::MethodOpen, P_ONE, "open(string pathname): open a database");
	Atanorsqlite::AddMethod(global, "close", &Atanorsqlite::MethodClose, P_NONE, "close(): close a database");
	Atanorsqlite::AddMethod(global, "create", &Atanorsqlite::MethodCreate, 65534, "create(x1x2x3): create a table in a database with the arguments x1x2x3.\rEx. mydb.create('table1''nom TEXT PRIMARY KEY''age INTEGER');");
	Atanorsqlite::AddMethod(global, "insert", &Atanorsqlite::MethodInsert, 65534, "insert(string tablestring columnint value...): insert a line in a table.\rEx. mydb.insert('table1''nom'nm'age'i);");
	Atanorsqlite::AddMethod(global, "insertbind", &Atanorsqlite::MethodInsertThroughBind, 65534, "insertbind(string tableint v1...): insert a line in a table through a begin/commit mode.\rEx. mydb.insert('table1'nmi);");
	Atanorsqlite::AddMethod(global, "run", &Atanorsqlite::MethodRun, P_ONE, "run(string sqlcommand): execute a sql command. If the input variable is a vector then all possible values will be stored in it. It the input variable is an iterator then it is possible to iterate on the results of the sql command. Each result is a map where each key is a column name.");
	Atanorsqlite::AddMethod(global, "execute", &Atanorsqlite::MethodExecute, P_ONE, "execute(string sqlcommand): execute a raw sql command.");
	Atanorsqlite::AddMethod(global, "begin", &Atanorsqlite::MethodBegin, P_NONE | P_ONE, "begin(string mode): to enter the commit mode: default mode is DEFERRED other modes are: IMMEDIATE and EXCLUSIVE");
	Atanorsqlite::AddMethod(global, "commit", &Atanorsqlite::MethodCommit, P_NONE, "commit(): the SQL command are then processed. It should finish a series of commands initiated with a begin");
	Atanorsqlite::AddMethod(global, "memory", &Atanorsqlite::MethodMemory, P_NONE, "memory(): return how much memory SQLite uses");

	global->newInstance[Atanorsqlite::idtype] = new Atanorsqlite(global);
	global->RecordMethods(Atanorsqlite::idtype,Atanorsqlite::exported);

	return true;
}

static string Quotting(string& si) {
	string s = "'";
	for (int i = 0; i < si.size(); i++) {
		if (si[i] == '\'')
			s += "'";
		s += si[i];
	}
	s += "'";
	s = conversion_latin_to_utf8(s);
	return s;
}


static int callback(void *asql, int argc, char **argv, char **azColName){
	int i;
	//We use our first parameter as the place where to store our stuff
	Atanorsqlite* ksql = (Atanorsqlite*)asql;
	Atanormapss* kmap = new Atanormapss;
	for (i = 0; i < argc; i++) {
		if (argv[i] == NULL)
			kmap->values[azColName[i]] = "";
		else
			kmap->values[azColName[i]] = argv[i];
	}
	ksql->vresults->Push(kmap);
	return 0;
}



Atanor* Atanorsqlite::Put(Atanor* index, Atanor* value, short idthread) {
	return this;
}

Atanor* Atanorsqlite::Get(Atanor* context, Atanor* index, short idthread) {
	return this;
}


AtanorIterationsqlite::AtanorIterationsqlite(Atanorsqlite* v, AtanorGlobal* g) : AtanorIteration(false) {
	value = v;
	currentrow = NULL;
	columnCount = 0;
	nb = -1;
	garbage_i = g->RecordInTrackerProtected(value);
}

Atanor* AtanorIterationsqlite::Value() {
	if (currentrow == NULL)
		return aNULL;
	return currentrow;
}

void AtanorIterationsqlite::Next() {
	Storevalue();
}

Atanor* AtanorIterationsqlite::End() {
	if (returncode != SQLITE_ROW) {
		sqlite3_finalize(stmt);
		if (currentrow != NULL)
			currentrow->Resetreference();
		return aTRUE;
	}
	return aFALSE;
}

Atanor* AtanorIterationsqlite::Begin() {
	nb = -1;
	int rc = sqlite3_prepare_v2(value->db, STR(value->sqlcommand), -1, &stmt, 0);
	if (rc != SQLITE_OK) {
		string message = "Wrong statement: ";
		message += value->sqlcommand;
		return globalAtanor->Returnerror(message, globalAtanor->GetThreadid());
	}

	//we get the number of columns matching the sql command
	columnCount = sqlite3_column_count(stmt);

	//we then store their names
	char* name;
	char supername[100];
	for (int i = 0; i < columnCount; i++) {
		name = (char*)sqlite3_column_name(stmt, i);
		if (name == NULL) {
			sprintf_s(supername, 100, "column%d", i);
			columnnames.push_back(supername);
		}
		else
			columnnames.push_back(name);
	}

	Storevalue();
	return aTRUE;
}

void AtanorIterationsqlite::Storevalue() {
	//We can then relieve the previous element	
	returncode = sqlite3_step(stmt);
	if (currentrow != NULL) {
		currentrow->Resetreference();
		currentrow = NULL;
	}

	if (returncode != SQLITE_ROW)
		return;

	currentrow = globalAtanor->Providemap();
	currentrow->Setreference();
	Atanorint* kint;
	Atanorfloat* kfloat;
	Atanorstring* ktext;
	string txt;

	for (int i = 0; i < columnCount; i++) {

		switch (sqlite3_column_type(stmt, i)) {
		case SQLITE_INTEGER:
			kint = globalAtanor->Provideint(sqlite3_column_int(stmt, i));
			currentrow->Push(columnnames[i], kint);
			break;
		case SQLITE_BLOB:
		case SQLITE_TEXT:
			txt = (char*)sqlite3_column_text(stmt, i);
			ktext = globalAtanor->Providestring(txt);
			currentrow->Push(columnnames[i], ktext);
			break;
		case SQLITE_FLOAT:
			kfloat = globalAtanor->Providefloat(sqlite3_column_double(stmt, i));
			currentrow->Push(columnnames[i], kfloat);
			break;
		default:
			currentrow->Push(columnnames[i], aNULL);
		}
	}
	nb++;
}

Atanor* Atanorsqlite::MethodOpen(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (db != NULL)
		return globalAtanor->Returnerror("SQLite(800): A database has already been opened with this object", idthread);
	//the first parameter is the dbname
	Atanor* kelement = callfunc->Evaluate(0, contextualpattern, idthread);
	dbname = kelement->String();
	int rc = sqlite3_open(STR(dbname), &db);
	if (rc) {
		string message = "SQLite(801): Error opening database='";
		message += dbname;
		message += "' --> ";
		message += sqlite3_errmsg(db);
		return globalAtanor->Returnerror(message, idthread);
	}
	return aTRUE;
}

Atanor* Atanorsqlite::MethodClose(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (db == NULL)
		return globalAtanor->Returnerror("SQLite(802): Cannot close this database", idthread);
	sqlite3_close(db);
	db = NULL;
	return aTRUE;
}

Atanor* Atanorsqlite::MethodCreate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (db == NULL)
		return globalAtanor->Returnerror("SQLite(803): Cannot use this database", idthread);
	//A typical call would be:
	// mydb.create("table1","name TEXT","age INTEGER",12);
	command = "create table ";
	//The first parameter is the table name
	Atanor* table = callfunc->Evaluate(0, contextualpattern, idthread);
	command += table->String();
	command += " (";
	//The next parameters are the rest of the table description
	for (int i = 1; i < callfunc->Size(); i++) {
		table = callfunc->Evaluate(i, contextualpattern, idthread);
		if (i != 1)
			command += ",";
		command += table->String();
	}
	command += ");";
	char *zErrMsg = NULL;
	int rc = sqlite3_exec(db, STR(command), NULL, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		command += "SQLite(805): Create error=";
		command += zErrMsg;
		sqlite3_free(zErrMsg);
		return globalAtanor->Returnerror(command, idthread);
	}
	if (zErrMsg != NULL)
		sqlite3_free(zErrMsg);
	return aTRUE;
}

Atanor* Atanorsqlite::MethodInsert(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (db == NULL)
		return globalAtanor->Returnerror("SQLite(803): Cannot use this database", idthread);
	if (callfunc->Size() == 0 || (callfunc->Size() % 2) == 0)
		return globalAtanor->Returnerror("SQLite(808): Missing parameters in 'INSERT'", idthread);
	string lacommande = "insert into ";
	//A typical call would be:
	// mydb.insert("table1","name","toto","age",12);
	//The first parameter is the table name
	Atanor* table = callfunc->Evaluate(0, contextualpattern, idthread);
	lacommande += table->String();
	lacommande += " (";
	//One parameter our of two is column name
	for (int i = 1; i < callfunc->Size(); i += 2) {
		table = callfunc->Evaluate(i, contextualpattern, idthread);
		if (i != 1)
			lacommande += ",";
		lacommande += table->String();
	}
	lacommande += ") values (";
	string s;
	//One parameter out of two is a value associated to a column name
	for (int i = 2; i < callfunc->Size(); i += 2) {
		table = callfunc->Evaluate(i, contextualpattern, idthread);
		if (i != 2)
			lacommande += ",";
		s = table->String();
		s = Quotting(s);
		lacommande += s;
	}
	lacommande += ");";
	char *zErrMsg = NULL;
	int rc = sqlite3_exec(db, STR(lacommande), NULL, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		lacommande += "SQLite(809): Create error=";
		lacommande += zErrMsg;
		sqlite3_free(zErrMsg);
		return globalAtanor->Returnerror(lacommande, idthread);
	}
	if (zErrMsg != NULL)
		sqlite3_free(zErrMsg);
	return aTRUE;
}

Atanor* Atanorsqlite::MethodInsertThroughBind(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (db == NULL)
		return globalAtanor->Returnerror("SQLite(803): Cannot use this database", idthread);
	if (callfunc->Size() < 2)
		return globalAtanor->Returnerror("SQLite(808): Missing parameters in 'INSERTBIND'", idthread);
	char *zErrMsg = NULL;
	int pos, rc;
	if (bindcommand == "ok") {
		Atanor* table = callfunc->Evaluate(0, contextualpattern, idthread);
		sqlcommand = "PRAGMA table_info(";
		sqlcommand += table->String();
		sqlcommand += ")";
		vresults->Clear();
		rc = sqlite3_exec(db, STR(sqlcommand), callback, this, &zErrMsg);
		if (rc != SQLITE_OK)
			return globalAtanor->Returnerror("SQLite(897): Unknown table", idthread);
		insertstructure.clear();
		bindcommand = "INSERT INTO ";
		bindcommand += table->String();
		bindcommand += " VALUES (";
		for (pos = 0; pos < vresults->values.size(); pos++) {
			command = ((Atanormapss*)vresults->values[pos])->values["type"];
			if (command == "TEXT" || command == "BLOB" || command == "VARCHAR")
				insertstructure.push_back(SQLITE_TEXT);
			else
			if (command == "INTEGER")
				insertstructure.push_back(SQLITE_INTEGER);
			else
			if (command == "REAL" || command == "FLOAT")
				insertstructure.push_back(SQLITE_FLOAT);
			else
				insertstructure.push_back(SQLITE_TEXT);
			if (pos != 0)
				bindcommand += ",";
			bindcommand += "?";
		}
		bindcommand += ")";
		rc = sqlite3_prepare_v2(db, STR(bindcommand), bindcommand.size(), &stmt, NULL);
		if (rc != SQLITE_OK)
			return globalAtanor->Returnerror("SQLite(899): Wrong value list", idthread);
	}
	if (stmt == NULL)
		return globalAtanor->Returnerror("SQLite(893): You need to do a BEGIN first", idthread);
	Atanor* kelement;
	for (pos = 1; pos <= insertstructure.size(); pos++) {
		kelement = callfunc->Evaluate(pos, contextualpattern, idthread);
		switch (insertstructure[pos - 1]) {
		case SQLITE_TEXT:
			command = kelement->String();
			rc = sqlite3_bind_text(stmt, pos, STR(command), command.size(), SQLITE_STATIC);
			break;
		case SQLITE_INTEGER:
			rc = sqlite3_bind_int(stmt, pos, kelement->Integer());
			break;
		case SQLITE_FLOAT:
			rc = sqlite3_bind_double(stmt, pos, kelement->Float());
		}
		if (rc != SQLITE_OK) {
			command = "SQLite(895): Wrong value for:";
			command += insertstructure[pos];
			return globalAtanor->Returnerror(command, idthread);
		}
	}
	bool failure = false;
	if (sqlite3_step(stmt) != SQLITE_DONE)
		failure = true;
	sqlite3_reset(stmt);
	if (failure)
		return globalAtanor->Returnerror("SQLite(812): Transaction failed!", idthread);
	return aTRUE;
}

Atanor* Atanorsqlite::MethodRun(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (db == NULL)
		return globalAtanor->Returnerror("SQLite(803): Cannot use this database", idthread);
	Atanor* kcommand = callfunc->Evaluate(0, contextualpattern, idthread);
	sqlcommand = kcommand->String();
	//If we are dealing with an iterator, then we simply return our local object, in order
	//to build an iterator. sqlcommand will keep the sql command to pass to the iterator.
	if (contextualpattern != NULL && contextualpattern->Type() == a_iteration)
		return this;
	char* errmsg;
	vresults->Clear();
	int rc = sqlite3_exec(db, STR(sqlcommand), callback, this, &errmsg);
	if (rc != SQLITE_OK) {
		sqlcommand += "SQLite(811): Execute error=";
		sqlcommand += errmsg;
		return globalAtanor->Returnerror(sqlcommand, idthread);
	}
	return vresults;
}

Atanor* Atanorsqlite::MethodExecute(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (db == NULL)
		return globalAtanor->Returnerror("SQLite(803): Cannot use this database", idthread);
	Atanor* kcommand = callfunc->Evaluate(0, contextualpattern, idthread);
	sqlcommand = kcommand->String();
	char* errmsg;
	int rc = sqlite3_exec(db, STR(sqlcommand), NULL, 0, &errmsg);
	if (rc != SQLITE_OK) {
		sqlcommand += "SQLite(811): Execute error=";
		sqlcommand += errmsg;
		return globalAtanor->Returnerror(sqlcommand, idthread);
	}
	return aTRUE;
}

Atanor* Atanorsqlite::MethodBegin(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (db == NULL)
		return globalAtanor->Returnerror("SQLite(803): Cannot use this database", idthread);
	string mode;
	if (callfunc->Size() != 0) {
		mode = callfunc->Evaluate(0, contextualpattern, idthread)->String();
		Trim(mode);
	}
	//A typical call would be:
	// mydb.create("table1","name TEXT","age INTEGER",12);
	command = "BEGIN";
	if (mode != "") {
		command += " ";
		command += mode;
	}
	command += " TRANSACTION;";
	bindcommand = "ok";
	stmt = NULL;
	//command+=" TRANSACTION;";
	char *zErrMsg = NULL;
	int rc = sqlite3_exec(db, STR(command), NULL, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		command += "SQLite(805): 'BEGIN' error=";
		command += zErrMsg;
		sqlite3_exec(db, "ROLLBACK", 0, 0, 0);
		sqlite3_free(zErrMsg);
		return globalAtanor->Returnerror(command, idthread);
	}
	if (zErrMsg != NULL)
		sqlite3_free(zErrMsg);
	return aTRUE;
}

Atanor* Atanorsqlite::MethodCommit(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (db == NULL)
		return globalAtanor->Returnerror("SQLite(803): Cannot use this database", idthread);
	//A typical call would be:
	// mydb.create("table1","name TEXT","age INTEGER",12);
	command = "COMMIT;";
	char *zErrMsg = NULL;
	int rc = sqlite3_exec(db, STR(command), NULL, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		command += "SQLite(807): 'COMMIT' error=";
		command += zErrMsg;
		sqlite3_free(zErrMsg);
		return globalAtanor->Returnerror(command, idthread);
	}
	bindcommand = "";
	if (stmt != NULL)
		sqlite3_finalize(stmt);
	stmt = NULL;
	if (zErrMsg != NULL)
		sqlite3_free(zErrMsg);
	return aTRUE;
}

Atanor* Atanorsqlite::MethodMemory(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (db == NULL)
		return globalAtanor->Returnerror("SQLite(803): Cannot use this database", idthread);
	long l = sqlite3_memory_used();
	return globalAtanor->Provideint(l);
}





