
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
filename   : atanordate.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanordate.h"
#include "atoms.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, dateMethod>  Atanordate::methods;
Exporting hmap<string, string> Atanordate::infomethods;
Exporting bin_hash<unsigned long> Atanordate::exported;

Exporting short Atanordate::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanordate::AddMethod(AtanorGlobal* global, string name, dateMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanordate::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanordate::idtype = global->Getid("date");

	Atanordate::AddMethod(global, "_initial", &Atanordate::MethodInitial, P_NONE | P_ONE | P_TWO | P_THREE | P_FOUR | P_FIVE | P_SIX | P_SEVEN, "_initial(): initialize a date.");
	Atanordate::AddMethod(global, "setdate", &Atanordate::MethodInitial, P_NONE | P_ONE | P_TWO | P_THREE | P_FOUR | P_FIVE | P_SIX | P_SEVEN, "setdate(y,m,d,h,m,s): Defines each of the elements of a date");
	Atanordate::AddMethod(global, "date", &Atanordate::MethodDate, P_NONE, "date(): the date itself");
	Atanordate::AddMethod(global, "year", &Atanordate::MethodYear, P_NONE | P_ONE, "year(int d): return the year or defines the year");
	Atanordate::AddMethod(global, "month", &Atanordate::MethodMonth, P_NONE | P_ONE, "month(int d): return the month or defines the month");
	Atanordate::AddMethod(global, "day", &Atanordate::MethodDay, P_NONE | P_ONE, "day(int d): return the day or defines the day");
	Atanordate::AddMethod(global, "yearday", &Atanordate::MethodYearday, P_NONE, "yearday(): return the number of the day in the year");
	Atanordate::AddMethod(global, "weekday", &Atanordate::MethodWeekday, P_NONE, "weekday(): return the day of the week");
	Atanordate::AddMethod(global, "hour", &Atanordate::MethodHour, P_NONE | P_ONE, "hour(int d): return the hour or defines the hour");
	Atanordate::AddMethod(global, "minute", &Atanordate::MethodMinute, P_NONE | P_ONE, "minute(int d): return the minute or defines the minute");
	Atanordate::AddMethod(global, "format", &Atanordate::MethodFormat, P_ONE, "format(string pattern): display the year with a C pattern");
	Atanordate::AddMethod(global, "second", &Atanordate::MethodSecond, P_NONE | P_ONE, "second(int d): return the second or defines the second");


	global->newInstance[Atanordate::idtype] = new Atanordate(global);
	global->RecordMethods(Atanordate::idtype,Atanordate::exported);

	return true;
}



Atanor* Atanordate::Put(Atanor* index, Atanor* value, short idthread) {
	return this;
}

Atanor* Atanordate::Get(Atanor* context, Atanor* index, short idthread) {
	return this;
}

string Atanordate::String() {
	char buffer[100];
#ifdef WIN32
	struct tm temps;
	localtime_s(&temps, &value);
	strftime(buffer, 100, "%Y/%m/%d %H:%M:%S", &temps);
#else
	struct tm* temps = localtime(&value);
	strftime(buffer, 100, "%Y/%m/%d %H:%M:%S", temps);
#endif
	return buffer;
}

#ifdef WIN32
Atanor* Atanordate::MethodInitial(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	if (callfunc->Size() == 0) {
		time(&value);
		return aTRUE;
	}

	Atanor* kinit = callfunc->Evaluate(0, aNULL, idthread);

	if (kinit->Type() == idtype) {
		memcpy((void*)&value, (void*)&((Atanordate*)kinit)->value, sizeof(time_t));
		kinit->Release();
		return aTRUE;
	}

	time_t x = 0;
	struct tm temps; 
	localtime_s(&temps, &x);

	int fulldate = 0;


	//Year
	long res = kinit->Integer();
	kinit->Release();

	if (res > 0) {
		temps.tm_year = res - 1900;
		fulldate = 100;
	}


	//Month
	if (callfunc->Size() >= 2) {
		res = callfunc->Evaluate(1, aNULL, idthread)->Integer();
		if (res > 0) {
			fulldate += 10;
			temps.tm_mon = res - 1;
		}
	}

	//Day
	if (callfunc->Size() >= 3) {
		res = callfunc->Evaluate(2,  aNULL, idthread)->Integer();
		if (res > 0) {
			fulldate += 1;
			temps.tm_mday = res;
		}
	}

	//Hour
	if (callfunc->Size() >= 4) {
		res = callfunc->Evaluate(3,  aNULL, idthread)->Integer();
		if (res >= 0 && res < 24) {
			//reference is 1AM
			if (fulldate == 0)
				res += 2;
			else
			if (fulldate == 1)
				res += 1;
			temps.tm_hour = res;
		}
	}


	//Minute
	if (callfunc->Size() >= 5) {
		res = callfunc->Evaluate(4,  aNULL, idthread)->Integer();
		if (res >= 0 && res < 60)
			temps.tm_min = res;
	}

	//Second
	if (callfunc->Size() >= 6) {
		res = callfunc->Evaluate(5,  aNULL, idthread)->Integer();
		if (res >= 0 && res < 60)
			temps.tm_sec = res;
	}



	x = mktime(&temps);
	if (x <= 0)
		return aFALSE;
	value = x;
	return aTRUE;
}
#else
Atanor* Atanordate::MethodInitial(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	if (callfunc->Size() == 0) {
		time(&value);
		return aTRUE;
	}

	Atanor* kinit = callfunc->Evaluate(0, aNULL, idthread);

	if (kinit->Type() == idtype) {
		memcpy((void*)&value, (void*)&((Atanordate*)kinit)->value, sizeof(time_t));
		kinit->Release();
		return aTRUE;
	}

	time_t x = 0;
	struct tm* temps = localtime(&x);

	int fulldate = 0;


	//Year
	long res = kinit->Integer();
	kinit->Release();

	if (res > 0) {
		temps->tm_year = res - 1900;
		fulldate = 100;
	}


	//Month
	if (callfunc->Size() >= 2) {
		res = callfunc->Evaluate(1, aNULL, idthread)->Integer();
		if (res > 0) {
			fulldate += 10;
			temps->tm_mon = res - 1;
		}
	}

	//Day
	if (callfunc->Size() >= 3) {
		res = callfunc->Evaluate(2, aNULL, idthread)->Integer();
		if (res > 0) {
			fulldate += 1;
			temps->tm_mday = res;
		}
	}

	//Hour
	if (callfunc->Size() >= 4) {
		res = callfunc->Evaluate(3, aNULL, idthread)->Integer();
		if (res >= 0 && res < 24) {
			//reference is 1AM
			if (fulldate == 0)
				res += 2;
			else
			if (fulldate == 1)
				res += 1;
			temps->tm_hour = res;
		}
	}


	//Minute
	if (callfunc->Size() >= 5) {
		res = callfunc->Evaluate(4, aNULL, idthread)->Integer();
		if (res >= 0 && res < 60)
			temps->tm_min = res;
	}

	//Second
	if (callfunc->Size() >= 6) {
		res = callfunc->Evaluate(5, aNULL, idthread)->Integer();
		if (res >= 0 && res < 60)
			temps->tm_sec = res;
	}



	x = mktime(temps);
	if (x <= 0)
		return aFALSE;
	value = x;
	return aTRUE;
}
#endif

Atanor* Atanordate::MethodDate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	string d = String();
	return globalAtanor->Providestring(d);
}

#ifdef WIN32
Atanor* Atanordate::MethodYear(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	struct tm temps; 
	localtime_s(&temps, &value);
	long vl;
	if (callfunc->Size() == 2) {
		vl = callfunc->Evaluate(1, aNULL, idthread)->Integer();
		if (vl < 100)
			return globalAtanor->Returnerror("Wrong value for a year", idthread);
		temps.tm_year = vl - 1900;
		value = mktime(&temps);
		return aTRUE;
	}

	vl = temps.tm_year + 1900;
	return globalAtanor->Provideint(vl);
}

Atanor* Atanordate::MethodMonth(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	struct tm temps; 
	localtime_s(&temps, &value);
	long vl;
	if (callfunc->Size() == 2) {
		vl = callfunc->Evaluate(1, aNULL, idthread)->Integer();
		if (vl < 1 || vl>12)
			return globalAtanor->Returnerror("Wrong value for a day", idthread);
		temps.tm_mon = vl - 1;
		value = mktime(&temps);
		return aTRUE;
	}
	vl = temps.tm_mon + 1;
	return globalAtanor->Provideint(vl);
}

Atanor* Atanordate::MethodDay(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	struct tm temps; 
	localtime_s(&temps, &value);
	long vl;
	if (callfunc->Size() == 2) {
		vl = callfunc->Evaluate(1, aNULL, idthread)->Integer();
		if (vl < 1 || vl>31)
			return globalAtanor->Returnerror("Wrong value for a day", idthread);
		temps.tm_mday = vl;
		value = mktime(&temps);
		return aTRUE;
	}

	vl = temps.tm_mday;
	return globalAtanor->Provideint(vl);
}

Atanor* Atanordate::MethodHour(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	struct tm temps; 
	localtime_s(&temps, &value);
	if (callfunc->Size() == 2) {
		long vl = callfunc->Evaluate(1, aNULL, idthread)->Integer();
		if (vl < 0 || vl>23)
			return globalAtanor->Returnerror("Wrong value for an hour", idthread);
		temps.tm_hour = vl;
		value = mktime(&temps);
		return aTRUE;
	}

	return globalAtanor->Provideint(temps.tm_hour);
}

Atanor* Atanordate::MethodMinute(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	struct tm temps; 
	localtime_s(&temps, &value);
	if (callfunc->Size() == 2) {
		long vl = callfunc->Evaluate(1, aNULL, idthread)->Integer();
		if (vl < 0 || vl>59)
			return globalAtanor->Returnerror("Wrong value for a minute", idthread);
		temps.tm_min = vl;
		value = mktime(&temps);
		return aTRUE;
	}
	return globalAtanor->Provideint(temps.tm_min);
}

Atanor* Atanordate::MethodSecond(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	struct tm temps; 
	localtime_s(&temps, &value);
	if (callfunc->Size() == 2) {
		long vl = callfunc->Evaluate(1, aNULL, idthread)->Integer();
		if (vl < 0 || vl>59)
			return globalAtanor->Returnerror("Wrong value for a second", idthread);
		temps.tm_sec = vl;
		value = mktime(&temps);
		return aTRUE;
	}
	return globalAtanor->Provideint(temps.tm_sec);
}

Atanor* Atanordate::MethodWeekday(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	struct tm temps; 
	localtime_s(&temps, &value);
	long vl = temps.tm_wday;
	return globalAtanor->Provideint(vl);
}

Atanor* Atanordate::MethodYearday(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	struct tm temps; 
	localtime_s(&temps, &value);
	long vl = temps.tm_yday;
	return globalAtanor->Provideint(vl);
}

Atanor* Atanordate::MethodFormat(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* kformat = callfunc->Evaluate(0, aNULL, idthread);
	char buffer[101];
	string sformat = kformat->String();
	struct tm temps; localtime_s(&temps, &value);
	strftime(buffer, 100, STR(sformat), &temps);
	return globalAtanor->Providestring(buffer);
}
#else
Atanor* Atanordate::MethodYear(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	struct tm* temps = localtime(&value);
	long vl;
	if (callfunc->Size() == 2) {
		vl = callfunc->Evaluate(1, aNULL, idthread)->Integer();
		if (vl < 100)
			return globalAtanor->Returnerror("Wrong value for a year", idthread);
		temps->tm_year = vl - 1900;
		value = mktime(temps);
		return aTRUE;
	}

	vl = temps->tm_year + 1900;
	return globalAtanor->Provideint(vl);
}

Atanor* Atanordate::MethodMonth(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	struct tm* temps = localtime(&value);
	long vl;
	if (callfunc->Size() == 2) {
		vl = callfunc->Evaluate(1, aNULL, idthread)->Integer();
		if (vl < 1 || vl>12)
			return globalAtanor->Returnerror("Wrong value for a day", idthread);
		temps->tm_mon = vl - 1;
		value = mktime(temps);
		return aTRUE;
	}
	vl = temps->tm_mon + 1;
	return globalAtanor->Provideint(vl);
}

Atanor* Atanordate::MethodDay(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	struct tm* temps = localtime(&value);
	long vl;
	if (callfunc->Size() == 2) {
		vl = callfunc->Evaluate(1, aNULL, idthread)->Integer();
		if (vl < 1 || vl>31)
			return globalAtanor->Returnerror("Wrong value for a day", idthread);
		temps->tm_mday = vl;
		value = mktime(temps);
		return aTRUE;
	}

	vl = temps->tm_mday;
	return globalAtanor->Provideint(vl);
}

Atanor* Atanordate::MethodHour(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	struct tm* temps = localtime(&value);
	if (callfunc->Size() == 2) {
		long vl = callfunc->Evaluate(1, aNULL, idthread)->Integer();
		if (vl < 0 || vl>23)
			return globalAtanor->Returnerror("Wrong value for an hour", idthread);
		temps->tm_hour = vl;
		value = mktime(temps);
		return aTRUE;
	}

	return globalAtanor->Provideint(temps->tm_hour);
}

Atanor* Atanordate::MethodMinute(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	struct tm* temps = localtime(&value);
	if (callfunc->Size() == 2) {
		long vl = callfunc->Evaluate(1, aNULL, idthread)->Integer();
		if (vl < 0 || vl>59)
			return globalAtanor->Returnerror("Wrong value for a minute", idthread);
		temps->tm_min = vl;
		value = mktime(temps);
		return aTRUE;
	}
	return globalAtanor->Provideint(temps->tm_min);
}

Atanor* Atanordate::MethodSecond(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	struct tm* temps = localtime(&value);
	if (callfunc->Size() == 2) {
		long vl = callfunc->Evaluate(1, aNULL, idthread)->Integer();
		if (vl < 0 || vl>59)
			return globalAtanor->Returnerror("Wrong value for a second", idthread);
		temps->tm_sec = vl;
		value = mktime(temps);
		return aTRUE;
	}
	return globalAtanor->Provideint(temps->tm_sec);
}

Atanor* Atanordate::MethodWeekday(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	struct tm* temps = localtime(&value);
	long vl = temps->tm_wday;
	return globalAtanor->Provideint(vl);
}

Atanor* Atanordate::MethodYearday(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter is a string
	struct tm* temps = localtime(&value);
	long vl = temps->tm_yday;
	return globalAtanor->Provideint(vl);
}

Atanor* Atanordate::MethodFormat(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* kformat = callfunc->Evaluate(0, aNULL, idthread);
	char buffer[101];
	string sformat = kformat->String();
	struct tm* temps = localtime(&value);
	strftime(buffer, 100, STR(sformat), temps);
	return globalAtanor->Providestring(buffer);
}

#endif




