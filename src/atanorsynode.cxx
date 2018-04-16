
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
filename   : atanorsynode.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "automate.h"
#include "atanorversion.h"
#include "atanorsynode.h"

//We need to declare once again our local definitions.
hmap<unsigned short, synodeMethod>  Atanorsynode::methods;
hmap<string, string> Atanorsynode::infomethods;
bin_hash<unsigned long> Atanorsynode::exported;
Atanormapss* Atanorsynode::validfeatures = NULL;
bool Atanorsynode::testvalid = false;
short Atanorsynode::idtype = 0;

//------------------------------------------------------------------------------------------------------------------

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
Exporting void Atanorsynode::AddMethod(AtanorGlobal* global, string name, synodeMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

Exporting bool Atanorsynode::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();
	validfeatures = NULL;
	Atanorsynode::testvalid = false;
	Atanorsynode::idtype = global->Getid("synode");
	
	Atanorsynode::AddMethod(global, "_initial", &Atanorsynode::MethodInitial, P_ONE, "_initial(map m): Creates a syntactic node with some features.");
	Atanorsynode::AddMethod(global, "test", &Atanorsynode::MethodTest, P_ONE, "test(string attribute): Test if an attribute is part of the feature structure.");
	Atanorsynode::AddMethod(global, "addchild", &Atanorsynode::MethodAddChild, P_ATLEASTONE, "addchild(synode): Add a first child node.");
	Atanorsynode::AddMethod(global, "addprevious", &Atanorsynode::MethodAddPrevious, P_ONE, "addprevious(synode): Add a previous.");
	Atanorsynode::AddMethod(global, "addnext", &Atanorsynode::MethodAddNext, P_ONE, "addnext(synode): Add a next node.");
	Atanorsynode::AddMethod(global, "parent", &Atanorsynode::MethodParent, P_NONE | P_ONE, "parent(): Return the parent node or check it against the parameter.");
	Atanorsynode::AddMethod(global, "child", &Atanorsynode::MethodChild, P_NONE | P_ONE, "child(): Return the first child node or check it against the parameter.");
	Atanorsynode::AddMethod(global, "last", &Atanorsynode::MethodLast, P_NONE | P_ONE, "last(): Return the last child node or check it against the parameter.");
	Atanorsynode::AddMethod(global, "previous", &Atanorsynode::MethodPrevious, P_NONE | P_ONE, "previous(synode): Return the previous node or check it against the parameter.");
	Atanorsynode::AddMethod(global, "next", &Atanorsynode::MethodNext, P_NONE | P_ONE, "next(synode): Return the next node or check it against the parameter.");
	Atanorsynode::AddMethod(global, "after", &Atanorsynode::MethodAfter, P_NONE | P_ONE, "after(synode): Return true if the node is after.");
	Atanorsynode::AddMethod(global, "before", &Atanorsynode::MethodBefore, P_NONE | P_ONE, "before(synode): Return true if the node is before.");
	Atanorsynode::AddMethod(global, "succeed", &Atanorsynode::MethodSucceed, P_NONE | P_ONE, "succeed(synode): Return true if the node is after.");
	Atanorsynode::AddMethod(global, "precede", &Atanorsynode::MethodPrecede, P_NONE | P_ONE, "precede(synode): Return true if the node is before.");
	Atanorsynode::AddMethod(global, "sisters", &Atanorsynode::MethodSisters, P_NONE | P_ONE, "sisters(synode): Return the list of sister nodes or check if the node is a sister node.");
	Atanorsynode::AddMethod(global, "children", &Atanorsynode::MethodChildren, P_NONE | P_ONE, "children(): Return the list of children for a given node or test if the node is a child.");
	Atanorsynode::AddMethod(global, "sibling", &Atanorsynode::MethodSibling, P_NONE | P_ONE, "sibling(synode): test if the node is a sibling (either a sister or a descendant).");
	Atanorsynode::AddMethod(global, "attributes", &Atanorsynode::MethodAttributes, P_NONE, "attributes(): Return the feature attributes as a vector.");
	Atanorsynode::AddMethod(global, "values", &Atanorsynode::MethodValues, P_NONE, "values(): Return the feature values as a vector.");
	Atanorsynode::AddMethod(global, "nbchildren", &Atanorsynode::MethodNbChildren, P_NONE, "nbchildren(): Return the number of direct children.");
	
	global->returntypes[global->Getid("nbchildren")] = a_int;

	Atanorsynode::AddMethod(global, "definitions", &Atanorsynode::MethodDefinitions, P_ONE, "definitions(mapss): Set the valid feature definitions for all 'synodes'.");

	global->newInstance[Atanorsynode::idtype] = new Atanorsynode(-1,global);
	global->RecordMethods(Atanorsynode::idtype,Atanorsynode::exported);
	
	return true;
}

Exporting bool Atanorsynode::Checkattribute(string key) {
	if (testvalid == false)
		return true;

	if (validfeatures->values.find(key) == validfeatures->values.end())
		return false;
	return true;
}

Exporting bool Atanorsynode::Checkfeature(string key, string value) {
	if (testvalid == false)
		return true;

	if (validfeatures->values.find(key) == validfeatures->values.end())
		return false;

	if (validfeatures->values[key] != value) {
		//In the case of a feature with a variable value, we use a blank string
		if (validfeatures->values[key] == "")
			return true;
		return false;
	}

	return true;
}


Exporting Atanor* Atanorsynode::Put(Atanor* idx, Atanor* kval, short idthread) {
	if (!idx->isIndex()) {
		if (kval->isMapContainer()) {
			if (kval == features)
				return aTRUE;

			if (features == aNULL) {
				features = globalAtanor->Providemapss();
				features->Setreference(reference);
			}
			
			if (!Checkfeatures(kval, idthread))
				return globalAtanor->Errorobject(idthread);

			features->Put(aNULL, kval, idthread);
			return aTRUE;
		}

		if (kval->Type() != a_synode)
			return globalAtanor->Returnerror("Expecting a 'synode' or a 'map'", idthread);

		Atanorsynode* as = (Atanorsynode*)kval;
		
		if (as->features == aNULL) {
			if (features != aNULL) {
				features->Resetreference(reference);
				features = aNULL;
			}
			return aTRUE;
		}

		if (features == aNULL) {
			features = globalAtanor->Providemapss();
			features->Setreference(reference);
		}

		features->Put(aNULL, as->features, idthread);
		return aTRUE;
	}

	if (features == aNULL) {
		features = globalAtanor->Providemapss();
		features->Setreference(reference);
	}

	if (kval->String() == "~") {
		((Atanormapss*)features)->values.erase(idx->String());
	}
	else
		features->Put(idx, kval, idthread);

	return aTRUE;
}

Exporting Atanor* Atanorsynode::Merge(Atanor* a) {
	if (a->Type() != Atanormapss::idtype)
		return aFALSE;

	if (features == aNULL)
		features = globalAtanor->Providemapss();
	
	hmap<string, string >& values = ((Atanormapss*)a)->values;
	hmap<string, string >& avalues = ((Atanormapss*)features)->values;
	hmap<string, string> assignation;

	bool aff;
	bool neg;
	bool rgx;

	string key;
	string val;

	for (auto &it: values) {
		aff = neg = rgx = false;

		key = it.first;

		switch (key[0]) {
		case ':':
			rgx = true;
			if (key[1] == '~') {
				neg = true;
				key = &key.c_str()[2];
			}
			else
				key = &key.c_str()[1];
			break;
		case '=':
			aff = true;
			key = &key.c_str()[1];
			break;
		case '~':
			neg = true;
			key = &key.c_str()[1];
		}

		val = it.second;

		if (val == "~") {
			if (avalues.find(key) != avalues.end()) {
				if (!aff) {
					if (!neg)
						return aFALSE;
				}
				else
					assignation[key] = val;
			}
			continue;
		}

		if (rgx) {
			if (avalues.find(key) == avalues.end()) {
				if (!neg)
					return aFALSE;
				continue;
			}

			automate* au = globalAtanor->rules[val];
			if (au == NULL)
				globalAtanor->rules[val] = au = new automate(USTR(val), (void*)1);
			string reg = avalues[key];
			etat* e = au->recherche(USTR(reg));
			if (e == NULL) {
				if (!neg)
					return aFALSE;
			}
			else {
				if (neg)
					return aFALSE;
			}
			continue;
		}

		if (avalues.find(key) != avalues.end()) {
			if (val != avalues[key]) {
				if (!neg)
					return aFALSE;
			}
			else {
				if (neg)
					return aFALSE;
			}
		}
		else {
			if (aff)
				assignation[key] = val;
			else {
				if (!neg)
					return aFALSE;
			}
		}
	}

	for (auto &it : assignation) {
		if (it.second == "~")
			avalues.erase(it.first);
		else
			avalues[it.first] = it.second;
	}

	return aTRUE;
}

Exporting Atanor* Atanorsynode::same(Atanor* a) {
	if (a == this)
		return aTRUE;

	if (a == aNULL && features == aNULL)
		return aTRUE;
	
	return aFALSE;
}

Exporting Atanor* Atanorsynode::different(Atanor* a) {
	if (a == this)
		return aFALSE;

	if (a == aNULL && features == aNULL)
		return aFALSE;

	return aTRUE;
}

Exporting bool Atanorsynode::Unify(AtanorDeclaration* dom, Atanor* a) {
	if (a == this)
		return true;

	if (a->Type() != a_synode) {
		if (a == aNULL && features == aNULL)
			return true;
		return false;
	}

	Atanorsynode* as = (Atanorsynode*)a;

	if (features == aNULL || as->features == aNULL)
		return true;

	if (as->Merge(features) == aTRUE)
		return true;
	return false;
}


Exporting bool Atanorsynode::isUnified(AtanorDeclaration* dom) {
	if (!id || id == a_universal)
		return true;
	return false;
}

Exporting Atanor* Atanorsynode::plus(Atanor* a, bool itself) {
	if (!a->isMapContainer() && a->Type() != a_synode)
		return this;
	
	Atanormapss* feat;
	if (itself) {
		if (features == aNULL) {
			feat = globalAtanor->Providemapss();
			features = feat;
		}
		else
			feat = (Atanormapss*)features;
	}
	else {
		feat = globalAtanor->Providemapss();
		if (features != aNULL)
			feat->values = ((Atanormapss*)features)->values;
	}

	if (a->isMapContainer()) {
		short idthread = globalAtanor->GetThreadid();
		if (!Checkfeatures(a, idthread))
			return globalAtanor->Errorobject(idthread);
	}

	AtanorIteration* itr = a->Newiteration(false);
	string val;
	string key;
	for (itr->Begin(); itr->End() != aTRUE; itr->Next())  {
		key = itr->Keystring();
		if (key[0] == '=')
			key = key.substr(1, key.size() - 1);
		val = itr->Valuestring();
		if (val == "~") {
			if (feat->values.find(key) != feat->values.end())
				feat->values.erase(key);
		}
		else
			feat->values[key] = val;
	}
	itr->Release();
	return feat;
}

Exporting Atanor* AtanorCallsynode::Get(Atanor* actual, Atanor* value, short idthread) {
	if (affectation)
		return this;

	actual = syn->Get(actual, value, idthread);
	if (actual->Type() != a_synode)
		return aFALSE;

	//we might have some action to do over "as"
	return ((Atanorsynode*)actual)->Merge(features);
}

Exporting Atanor* AtanorCallsynode::Put(Atanor* actual, Atanor* value, short idthread) {
	actual = syn->Get(actual, aNULL, idthread);
	if (actual->Type() != a_synode)
		return aFALSE;

	string vstr = value->String();
	Atanorsynode* asyn = (Atanorsynode*)actual;
	if (vstr == "~") {
		if (asyn->features == aNULL)
			return aFALSE;
		hmap<string,string>& asynfeat = ((Atanormapss*)asyn->features)->values;
		for (auto& it : ((Atanormapss*)features)->values) {
			if (asynfeat.find(it.first) != asynfeat.end())
				asynfeat.erase(it.first);
		}
		return aTRUE;
	}
	Atanormapss feats;
	for (auto& it : ((Atanormapss*)features)->values)
		feats.values[it.first] = vstr;
	return asyn->plus(&feats, true);
}




