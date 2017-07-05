
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
filename   : atanorgrammar.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorgrammar.h"
#include "atoms.h"
#include "atanorvector.h"
#include "atanormap.h"
#include "atanoruvector.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, grammarMethod>  Atanorgrammar::methods;
Exporting hmap<string, string> Atanorgrammar::infomethods;
Exporting bin_hash<unsigned long> Atanorgrammar::exported;

vector<autobaseatan*> autobaseatan::garbage;

Exporting short Atanorgrammar::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorgrammar::AddMethod(AtanorGlobal* global, string name, grammarMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorgrammar::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorgrammar::idtype = global->Getid("grammar");

	Atanorgrammar::AddMethod(global, "_initial", &Atanorgrammar::MethodInitial, P_ONE | P_TWO, "_initial(string rules, int skipblanks): Compile rules.");
	Atanorgrammar::AddMethod(global, "load", &Atanorgrammar::MethodInitial, P_ONE | P_TWO, "load(svector rules, int skipblanks): Load a grammar out of a svector. Each line must be a rule. 'skipblanks' is optional and is 0 by default. '1' means that white spaces and tabs will be skipped. '2' means that all spaces will be skipped.");
	Atanorgrammar::AddMethod(global, "apply", &Atanorgrammar::MethodApply, P_ONE, "apply(string x) : Apply the grammar to a string.");



	global->newInstance[Atanorgrammar::idtype] = new Atanorgrammar(global);
	global->RecordMethods(Atanorgrammar::idtype,Atanorgrammar::exported);

	return true;
}



Atanor* Atanorgrammar::Put(Atanor* index, Atanor* value, short idthread) {
	return this;
}

Atanor* Atanorgrammar::Get(Atanor* context, Atanor* index, short idthread) {
	return this;
}

//---------------------------------------------------------------------------------
static const unsigned char gbegin = 1;
static const unsigned char gend = 2;

static const unsigned char gmeta = 1;
static const unsigned char grule = 2;
static const unsigned char gloop = 4;
static const unsigned char gmulti = 8;
static const unsigned char gnegation = 16;
static const unsigned char gfunction = 32;
static const unsigned char gkeepout = 64;

//---------------------------------------------------------------------------------

static bool gramtraversestate(GrammarBaseAutomaton* gram, x_node* x, gramstate* s, int i, vector<wstring>& e, hmap<int, gramstate*>& states);

//We add arcs or state according to the value
static gramarc* gramtraversearc(GrammarBaseAutomaton* gram, x_node* x, gramstate* s, vector<wstring>& e) {
	int i = 0;
	if (x->nodes[0]->token == "negation") {
		i = 1;
	}

	if (x->nodes[0]->token == "subgram") {
		subgrammar* a = new subgrammar(gram);
		a->from = s;
		if (i == 1)
			a->status |= gnegation;
		a->status |= gmulti;
		gram->arcs.push_back(a);
		hmap<int, gramstate*> states;
		for (i = 0; i < x->nodes[0]->nodes.size(); i++) {
			gramtraversestate(&a->gram, x->nodes[0]->nodes[i], &a->substate, 0, e, states);
			states.clear();
		}
		s->arcs.push_back(a);
		return a;
	}

	gramarc* a = new gramarc;
	sc_utf8_to_unicode(a->wlabel, STR(x->nodes[i]->value));
	a->from = s;
	if (i == 1)
		a->status |= gnegation;
	gram->arcs.push_back(a);


	if (x->nodes[i]->token == "astringdouble" || x->nodes[i]->token == "astringsimple") {
		//we remove the enclosing characters...
		a->wlabel = a->wlabel.substr(1, a->wlabel.size() - 2);
		//we reuse idrule in this case to keep the label size...
		a->idrule = a->wlabel.size();
		if (!a->idrule)
			return NULL;
		if (a->idrule > 1)
			a->status |= gmulti;
	}
	else {
		if (x->nodes[i]->token == "anumber")
			a->wlabel = atoi(STR(x->nodes[i]->value));
		else {
			if (x->nodes[i]->token == "ameta")
				a->status |= gmeta;
			else
			if (x->nodes[i]->token == "asimple") {
				//we remove the first character, which is a $
				//we reuse idrule in this case to keep the label size...
				a->idrule = a->wlabel.size();
				if (a->idrule > 1)
					a->status |= gmulti;
			}
			else {
				if (x->nodes[i]->token == "amulti") {
					//we remove the first character, which is a $
					a->wlabel = a->wlabel.substr(1, a->wlabel.size() - 1);
					//we reuse idrule in this case to keep the label size...
					a->idrule = a->wlabel.size();
					if (a->idrule > 1)
						a->status |= gmulti;
				}
				else {
					if (x->nodes[i]->token == "word") {
						a->status |= grule;
						e.push_back(a->wlabel);
						if (a->wlabel[a->wlabel.size() - 1] == '_')
							a->status |= gkeepout;
					}
				}
			}
		}
	}
	s->arcs.push_back(a);
	return a;
}

static bool gramtraversestate(GrammarBaseAutomaton* gram, x_node* x, gramstate* s, int i, vector<wstring>& e, hmap<int, gramstate*>& states) {
	//First we jump to the first non disjoined element
	while (i < x->nodes.size() && x->nodes[i]->token == "dj")
		i += 2;

	//we might be at the end of the structure...
	if (i == x->nodes.size()) {
		s->status = gend;
		return true;
	}

	gramarc* a;
	gramstate* gs;
	bool created = false;
	if (states.find(i) == states.end()) {
		created = true;
		gs = new gramstate;
		gram->states.push_back(gs);
		states[i] = gs;
	}
	else
		gs = states[i];

	//we create our node...
	//two cases: optional or kleene
	if (x->nodes[i]->nodes[0]->token == "aoptional" || x->nodes[i]->nodes[0]->token == "aoptitem") {
		a = gramtraversearc(gram, x->nodes[i]->nodes[0]->nodes[0], s, e);
		if (a == NULL)
			return false;
		a->state = gs;
		if (created) {
			if (!gramtraversestate(gram, x, gs, i + 1, e, states))
				return false;
		}
		//then we need to skip it...
		//we copy the states after the new arc...
		for (int i = 0; i < a->state->arcs.size(); i++)
			s->arcs.push_back(a->state->arcs[i]);
		if (gs->status == gend)
			s->status = gend;
	}
	else {
		a = gramtraversearc(gram, x->nodes[i]->nodes[0], s, e);
		if (a == NULL)
			return false;
		a->state = gs;
		if (created) {
			if (!gramtraversestate(gram, x, gs, i + 1, e, states))
				return false;
		}

		if (x->nodes[i]->nodes.size() == 2) {
			char kleene = x->nodes[i]->nodes[1]->value[0];
			//If there is a kleene then we need to be able to loop
			if (kleene == '*') {
				bool pushback = false;
				if (s->arcs.back() == a) {
					s->arcs.pop_back();
					pushback = true;
				}
				//then we might need to skip it...
				for (int i = 0; i < a->state->arcs.size(); i++)
					s->arcs.push_back(a->state->arcs[i]);
				if (pushback)
					s->arcs.push_back(a);

				if (gs->status == gend)
					s->status = gend;
			}

			//we add our local arc to the next state... We add as first...
			if (gs->status != gend)
				a->state->arcs.push_back(a);
			else
				a->state->arcs.insert(a->state->arcs.begin(), a);
			a->status |= gloop;
		}
	}

	if (i < x->nodes.size() - 1 && x->nodes[i + 1]->token == "dj")
		return gramtraversestate(gram, x, s, i + 2, e, states);
	return true;
}

static bool gramtraverse(GrammarBaseAutomaton* gram, x_node* x, gramstate* s, vector<wstring>& e) {
	hmap<int, gramstate*> states;
	return gramtraversestate(gram, x, s, 1, e, states);
}

Atanor* Atanorgrammar::Loader(string& vrule, short idthread) {
	string s;
	vector<wstring> e;

	x_readstring xr(vrule);
	xr.loadtoken();
	bnf_atanor bnf;
	bnf.initialize(&xr);

	string lret;
	x_node* xn = new x_node;

	if (bnf.m_rules(lret, &xn) != 1 || bnf.currentpos != bnf.fx->stack.size()) {
		if (bnf.currentpos < bnf.fx->stack.size()) {
			string content("KIF(411): Error after:<");
			for (int i = 0; i <= bnf.currentpos; i++) {
				content += bnf.fx->stack[i];
				content += " ";
			}
			content += "...>";
			return globalAtanor->Returnerror(content, idthread);
		}
		return globalAtanor->Returnerror("Cannot evaluate this grammar", idthread);
	}

	if (automaton != NULL && clean)
		delete automaton;

	automaton = new GrammarAutomaton;
	automaton->skipblanks = skipblanks;
	automaton->function = function;

	clean = true;
	int id;
	wstring rulename;
	for (int i = 0; i < xn->nodes.size(); i++) {
		//We need to transform it in an automaton...
		//the first node is the rule name... We need to put it in our dictionary
		sc_utf8_to_unicode(rulename, STR(xn->nodes[i]->nodes[0]->value));
		gramstate* gs;
		if (automaton->dictionary.find(rulename) == automaton->dictionary.end()) {
			id = automaton->rules.size();
			automaton->dictionary[rulename] = id;
			automaton->rdictionary[id] = rulename;
			gs = new gramstate;
			automaton->states.push_back(gs);
			gs->status = gbegin; //the head of our rule...
			automaton->rules.push_back(gs);
		}
		else {
			id = automaton->dictionary[rulename];
			gs = automaton->rules[id];
		}

		if (!gramtraverse(automaton, xn->nodes[i], gs, e)) {
			delete xn;
			return globalAtanor->Returnerror("GRM(002): Wrong negation or empty string.", idthread);
		}
	}
	delete xn;

	string el;
	//we check that for each label found in the rule that is a rule, a rule actually exists...
	for (int i = 0; i < e.size(); i++) {
		bool found = true;
		if (automaton->dictionary.find(e[i]) == automaton->dictionary.end()) {
			found = false;
			//is it a function?
			sc_unicode_to_utf8(el, e[i]);
			if (globalAtanor->symbolIds.find(el) != globalAtanor->symbolIds.end()) {
				id = globalAtanor->Getid(el);
				if (globalAtanor->isDeclared(id, idthread)) {
					Atanor* kfunc = globalAtanor->Getdeclaration(id, idthread);
					if (kfunc->isFunction()) {
						found = true;
						//we look for all arcs with this label...
						for (int j = 0; j < automaton->arcs.size(); j++) {
							gramarc* a = automaton->arcs[j];
							if (a->wlabel == e[i]) {
								a->status &= ~grule;
								a->status |= gfunction;
								a->idrule = id;
								if (a->from->arcs.size() == 1)
									a->from->status |= gfunction;
							}
						}
					}
				}
			}

			if (!found) {
				string message = "GRM(001): Unknown rule label:";
				message += " ";
				s_unicode_to_utf8(message, e[i]);
				return globalAtanor->Returnerror(message, idthread);
			}
		}
	}

	for (int i = 0; i<automaton->arcs.size(); i++) {
		gramarc* a = automaton->arcs[i];
		if ((a->status&grule) == grule)
			a->idrule = automaton->dictionary[a->wlabel];
	}

	return aTRUE;
}

Atanor* Atanorgrammar::MethodInitial(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	string vrule = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	if (callfunc->Size() == 2)
		skipblanks = callfunc->Evaluate(1, contextualpattern, idthread)->Byte();

	return Loader(vrule, idthread);
}

//----------------------------------------------------------------------------------------------------------------
static int mx = 0;
static int pmx = 0;

class profond {
public:
	int i;
	profond() {
		i = pmx;
		pmx++;
		if (mx < pmx)
			mx = pmx;
	}

	~profond() {
		pmx--;
	}
};


bool GrammarBaseAutomaton::callfunction(short idthread, wstring& a, Atanor* b, int pos) {	
	AtanorCallFunction kfunc(function);

	AtanorConstUString ba(a);
	AtanorConstInt bpos(pos);

	kfunc.arguments.push_back(&ba);
	kfunc.arguments.push_back(b);
	kfunc.arguments.push_back(&bpos);
	Atanor* res = kfunc.Get(aNULL, aNULL, idthread);
	if (res->isError())
		return false;
	return res->Boolean();
}

bool GrammarBaseAutomaton::callfunction(Atanor* func, short idthread, Atanor* b, int pos) {
	AtanorCallFunction kfunc(func);

	AtanorConstInt bpos(pos);
	kfunc.arguments.push_back(b);
	kfunc.arguments.push_back(&bpos);
	Atanor* res = kfunc.Get(aNULL, aNULL, idthread);
	if (res->isError())
		return false;
	return res->Boolean();
}

gramarc::gramarc() {
	state = NULL;
	from = NULL;
	status = 0;
	idrule = -1;
}


int subgrammar::test(wstring& label, wstring& r, int i) {
	int nxt = i;
	if (substate.compare(globalAtanor->GetThreadid(), &gram, label, nxt, aNULL, true)) {
		r = label.substr(i, nxt - i);
		return (nxt - 1);
	}
	return -1;
}

char subgrammar::test(wstring& label) {
	int i = 0;
	if (substate.compare(globalAtanor->GetThreadid(), &gram, label, i, aNULL, true) && i == label.size())
		return true;
	return false;
}

int gramarc::test(wstring& label, wstring& r, int i) {
	//we check whether it is the same string as the label
	int j = 0;
	while (j<wlabel.size()) {
		if (i == label.size())
			return -1;
		if (label[i++] != wlabel[j++])
			return -1;
	}
	r = wlabel;
	return (i - 1);
}

char gramarc::test(wstring& car) {
	char good = true;
	char bad = false;

	if ((status&gnegation) == gnegation) {
		good = false;
		bad = true;
	}
	if ((status&gmeta) != gmeta) {
		if (wlabel == car)
			return good;
		return bad;
	}

	if (wlabel[0] == '?') {
		if (wlabel[1] == '_')
			return gkeepout;
		return true;
	}

	if (wlabel[2] == '_') {
		if (good)
			good = gkeepout;
		else
			bad = gkeepout;
	}

	switch (wlabel[1]) {
	case '%':
	case '?':
		if (car[0] == wlabel[1])
			return good;
		return bad;
	case 'C':
		if (s_is_upper(car))
			return good;
		return bad;
	case 'a':
		if (s_is_alpha(car))
			return good;
		return bad;
	case 'c':
		if (s_is_lower(car))
			return good;
		return bad;
	case 'd':
		if (s_is_digit(car))
			return good;
		return bad;
	case 'p':
		if (s_is_punctuation(car))
			return good;
		return bad;
	case 'S':
		if (car[0] <= 32 || car[0] == 160)
			return good;
		return bad;
	case 's':
		if (car[0] == 9 || car[0] == 32 || car[0] == 160)
			return good;
		return bad;
	case 'r':
		if (car[0] == 10 || car[0] == 13)
			return good;
		return bad;
	}

	return bad;
}

char gramarc::test(wchar_t car) {
	char good = true;
	char bad = false;

	if ((status&gnegation) == gnegation) {
		good = false;
		bad = true;
	}
	if ((status&gmeta) != gmeta) {
		if (wlabel[0] == car)
			return good;
		return bad;
	}

	if (wlabel[0] == '?') {
		if (wlabel[1] == '_')
			return gkeepout;
		return true;
	}

	if (wlabel[2] == '_') {
		if (good)
			good = gkeepout;
		else
			bad = gkeepout;
	}

	switch (wlabel[1]) {
	case '%':
	case '?':
		if (car == wlabel[1])
			return good;
		return bad;
	case 'C':
		if (c_is_upper(car))
			return good;
		return bad;
	case 'a':
		if (b_alpha(car))
			return good;
		return bad;
	case 'c':
		if (c_is_lower(car))
			return good;
		return bad;
	case 'd':
		if (car >= '0' && car <= '9')
			return good;
		return bad;
	case 'p':
		if (c_is_punctuation(car))
			return good;
		return bad;
	case 'S':
		if (car <= 32 || car == 160)
			return good;
		return bad;
	case 's':
		if (car == 9 || car == 32 || car == 160)
			return good;
		return bad;
	case 'r':
		if (car == 10 || car == 13)
			return good;
		return bad;
	}

	return bad;
}

char gramstate::compare(GrammarBaseAutomaton* gram, vector<wstring>& labels, int& i, autobaseatan* v) {
	if (gram->skipblanks) {
		if (gram->skipblanks == 1) {
			while (i < labels.size() && (labels[i] == L" " || labels[i] == L"\t")) i++;
		}
		else {
			while (i < labels.size() && (labels[i][0] <= 32)) i++;
		}
	}

	if ((status&gend) == gend) {
		if (!arcs.size() || i == labels.size())
			return true;
	}
	else {
		if (i == labels.size()) { //if the next element is a function call, then we still can have a look...
			if ((status&gfunction) != gfunction)
				return false;
		}
	}

	int c = i;
	bool callfunc = false;
	bool topop = false;
	char found;
	char ret;

	if (gram->function != NULL)
		callfunc = true;

	for (int u = 0; u < arcs.size(); u++) {
		gramarc* a = arcs[u];


		if ((a->status&grule) == grule) {
			//then we need to apply a rule to it...
			autobaseatan* sube = NULL;
			autobaseatanvector* sub = new autobaseatanvector;
			sub->storevalue(a->wlabel);
			sube = sub;

			ret = gram->rules[a->idrule]->compare(gram, labels, i, sube);
			found = false;

			if ((a->status&gnegation) == gnegation) {
				if (!ret) {
					found = true;
					if (v->Size() == 1)
						v->storevalue(L"");

					v->addvalue(labels[i++]);
					sub = NULL;
				}
			}
			else {
				if (ret)
					found = true;
			}

			if (found) {
				if (ret == gend)
					return ret;

				if (!(a->status&gkeepout)) {

					if (a->wlabel[0] == '_') {
						//then we merge its content with the current structure
						if (sub->values.size() > 1) {
							sub->Pop(0);
							v->Merge(sub);
						}
						delete sub;
						sub = NULL;
					}
					else
					if (sub != NULL)
						v->Push(sub);
				}

				found = false;
				//This is a loop, and we have gotten to here, it means that we had some succesfull loops...
				//We do not need to go back into recursion...
				if ((a->status&gloop) == gloop && a->state == this) {
					//this could be a final state...
					if (i == labels.size() && (status&gend) == gend)
						found = true;
					else {
						u = -1;
						c = i;
						continue;
					}
				}

				if (found || (ret = a->state->compare(gram, labels, i, v))) {
					if (ret == gend)
						return ret;

					if (a->wlabel[0] == '_' && sub != NULL && !(a->status&gkeepout)) {
						sub = (autobaseatanvector*)v;
						int x;
						//Then we try to merge the value together...
						wstring mrg;
						bool replace = true;
						int sz = sub->values.size();
						for (x = 1; x < sz; x++) {
							if (sub->values[x]->istring()) {
								replace = false;
								break;
							}
							mrg += sub->values[x]->value();
						}
						if (replace) {
							for (x = 1; x < sz; x++)
								v->Pop(-1);
							v->storevalue(mrg);
						}
					}
					return true;
				}
			}
			i = c;
			continue;
		}

		found = a->test(labels[i]);

		if (found) {
			if (found == 1 && v != NULL) {
				if (v->Size() == 1) {
					v->storevalue(L"");
					topop = true;
				}
				if ((a->status&gmulti) == gmulti)
					v->addvalue(a->wlabel);
				else
					v->addvalue(labels[i]);
			}

			if ((a->status&gloop) == gloop && a->state == this) {
				i++;
				if (i == labels.size()) {
					if ((status&gend) == gend)
						return true;
					return false;
				}

				topop = false;
				u = -1;
				c = i;
				continue;
			}

			if ((ret = a->state->compare(gram, labels, ++i, v)))
				return ret;
		}

		if (topop) {
			v->Pop(-1);
			topop = false;
		}
		i = c;
	}

	if ((status&gend) == gend)
		return true;

	return false;
}


char gramstate::compare(short idthread, GrammarBaseAutomaton* gram, vector<wstring>& labels, int& i, Atanor* v, bool asstring) {
	if (globalAtanor->Error(idthread))
		return gend;

	if (gram->skipblanks) {
		if (gram->skipblanks == 1) {
			while (i < labels.size() && (labels[i] == L" " || labels[i] == L"\t")) i++;
		}
		else {
			while (i < labels.size() && (labels[i][0] <= 32)) i++;
		}
	}

	if ((status&gend) == gend) {
		if (!arcs.size() || i == labels.size())
			return true;
	}
	else {
		if (i == labels.size()) { //if the next element is a function call, then we still can have a look...
			if ((status&gfunction) != gfunction)
				return false;
		}
	}

	int c = i;
	bool callfunc = false;
	bool topop = false;
	char found;
	char ret;

	if (gram->function != NULL)
		callfunc = true;

	for (int u = 0; u < arcs.size(); u++) {
		gramarc* a = arcs[u];
		if ((a->status&gfunction) == gfunction) {
			Atanor* kfunc = globalAtanor->Getdeclaration(a->idrule, idthread);
			if (v != aNULL) {
				v->Setreference();
				if (!gram->callfunction(kfunc, idthread, v, i)) {
					v->Setprotect(1);
					v->Resetreference();
					v->Setprotect(1);
					continue;
				}
				v->Setprotect(1);
				v->Resetreference();
				v->Setprotect(1);
			}

			if ((ret = a->state->compare(idthread, gram, labels, i, v, asstring)))
				return ret;

			i = c;
			continue;
		}

		if ((a->status&grule) == grule) {
			//then we need to apply a rule to it...
			Atanor* sube = aNULL;
			Atanorvector* sub = NULL;
			if (v != aNULL) {
				sub = globalAtanor->Providevector();
				sub->storevalue(a->wlabel);
				sube = sub;
			}

			ret = gram->rules[a->idrule]->compare(idthread, gram, labels, i, sube, asstring);
			found = false;

			if ((a->status&gnegation) == gnegation) {
				if (!ret) {
					found = true;
					if (asstring) {
						if (v->Size() == 1)
							v->storevalue("");
						v->addustringto(labels[i++], -1);
					}
					else
						v->storevalue(labels[i++]);
					sub->Clear();
				}
			}
			else {
				if (ret)
					found = true;
				else
					sube->Release();
			}

			if (found) {
				if (ret == gend) {
					sube->Release();
					return ret;
				}

				if (v != aNULL && !(a->status&gkeepout)) {
					if (callfunc) {
						sub->Setreference();
						if (!gram->callfunction(idthread, a->wlabel, sub, i)) {
							sub->Resetreference();
							continue;
						}

						sub->protect = 1;
						sub->Resetreference();
						sub->protect = 1;
					}

					if (a->wlabel[0] == '_') {
						//then we merge its content with the current structure
						if (sub->values.size() > 1) {
							sub->Pop(aZERO);
							v->Merging(sub);
						}
						sub->Clear();
					}
					else
					if (sub->values.size())
						v->Push(sub);
				}

				sube->Release();

				found = false;
				//This is a loop, and we have gotten to here, it means that we had some succesfull loops...
				//We do not need to go back into recursion...
				if ((a->status&gloop) == gloop && a->state == this) {
					//this could be a final state...
					if (i == labels.size() && (status&gend) == gend)
						found = true;
					else {
						u = -1;
						c = i;
						continue;
					}
				}

				if (found || (ret = a->state->compare(idthread, gram, labels, i, v, asstring))) {
					if (ret == gend)
						return ret;

					if (a->wlabel[0] == '_' && sub != NULL && !(a->status&gkeepout)) {
						if (asstring) {
							sub = (Atanorvector*)v;
							int x;
							//Then we try to merge the value together...
							string mrg;
							bool replace = true;
							int sz = sub->values.size();
							for (x = 1; x < sz; x++) {
								if (sub->values[x]->Type() != a_string) {
									replace = false;
									break;
								}
								mrg += ((Atanorstring*)sub->values[x])->value;
							}
							if (replace) {
								for (x = 1; x < sz; x++)
									v->Pop(aMINUSONE);
								v->storevalue(mrg);
							}
						}
					}
					return true;
				}
			}
			i = c;
			continue;
		}

		found = a->test(labels[i]);

		if (found) {
			if (found == 1 && v != aNULL) {
				if (asstring) {
					if (v->Size() == 1) {
						v->storevalue("");
						topop = true;
					}
					if ((a->status&gmulti) == gmulti)
						v->addustringto(a->wlabel, -1);
					else
						v->addustringto(labels[i], -1);
				}
				else
					v->storevalue(labels[i]);
			}

			if ((a->status&gloop) == gloop && a->state == this) {
				i++;
				if (i == labels.size()) {
					if ((status&gend) == gend)
						return true;
					return false;
				}

				topop = false;
				u = -1;
				c = i;
				continue;
			}

			if ((ret = a->state->compare(idthread, gram, labels, ++i, v, asstring)))
				return ret;
		}

		if (topop) {
			v->Pop(aMINUSONE);
			topop = false;
		}
		i = c;
	}

	if ((status&gend) == gend)
		return true;

	return false;
}

char gramstate::compare(short idthread, GrammarBaseAutomaton* gram, wstring& labels, int& i, Atanor* v, bool asstring) {
	if (globalAtanor->Error(idthread))
		return gend;

	if (gram->skipblanks) {
		if (gram->skipblanks == 1) {
			while (i < labels.size() && (labels[i] == L' ' || labels[i] == L'\t')) i++;
		}
		else {
			while (i < labels.size() && (labels[i] <= 32)) i++;
		}
	}

	if ((status&gend) == gend) {
		if (!arcs.size() || i == labels.size())
			return true;
	}
	else {
		if (i == labels.size()) { //if the next element is a function call, then we still can have a look...
			if ((status&gfunction) != gfunction)
				return false;
		}
	}

	int c = i;
	bool callfunc = false;
	bool topop = false;
	char found;
	char ret;

	if (gram->function != NULL)
		callfunc = true;

	for (int u = 0; u < arcs.size(); u++) {
		gramarc* a = arcs[u];
		if ((a->status&gfunction) == gfunction) {
			Atanor* kfunc = globalAtanor->Getdeclaration(a->idrule, idthread);
			if (v != aNULL) {
				v->Setreference();
				if (!gram->callfunction(kfunc, idthread, v, i)) {
					v->Setprotect(1);
					v->Resetreference();
					v->Setprotect(1);
					continue;
				}
				v->Setprotect(1);
				v->Resetreference();
				v->Setprotect(1);
			}

			if ((ret = a->state->compare(idthread, gram, labels, i, v, asstring)))
				return ret;

			i = c;
			continue;
		}

		if ((a->status&grule) == grule) {
			//then we need to apply a rule to it...
			Atanor* sube = aNULL;
			Atanorvector* sub = NULL;
			if (v != aNULL) {
				sub = globalAtanor->Providevector();
				sub->storevalue(a->wlabel);
				sube = sub;
			}


			ret = gram->rules[a->idrule]->compare(idthread, gram, labels, i, sube, asstring);
			found = false;

			if ((a->status&gnegation) == gnegation) {
				if (!ret) {
					found = true;
					if (asstring) {
						if (v->Size() == 1)
							v->storevalue(L"");
						v->addstringto(labels[i++], -1);
					}
					else
						v->storevalue(labels[i++]);
					sub->Clear();
				}
			}
			else {
				if (ret)
					found = true;
				else
					sube->Release();
			}

			if (found) {
				if (ret == gend) {
					sube->Release();
					return ret;
				}

				if (v != aNULL && !(a->status&gkeepout)) {
					if (callfunc) {
						sub->Setreference();
						if (!gram->callfunction(idthread, a->wlabel, sub, i)) {
							sub->Resetreference();
							continue;
						}

						sub->protect = 1;
						sub->Resetreference();
						sub->protect = 1;
					}

					if (a->wlabel[0] == '_') {
						//then we merge its content with the current structure
						if (sub->values.size() > 1) {
							sub->Pop(aZERO);
							v->Merging(sub);
						}
						sub->Clear();
					}
					else
					if (sub->values.size())
						v->Push(sub);
				}

				sube->Release();

				found = false;
				//This is a loop, and we have gotten to here, it means that we had some succesfull loops...
				//We do not need to go back into recursion...
				if ((a->status&gloop) == gloop && a->state == this) {
					//this could be a final state...
					if (i == labels.size() && (status&gend) == gend)
						found = true;
					else {
						u = -1;
						c = i;
						continue;
					}
				}

				if (found || (ret = a->state->compare(idthread, gram, labels, i, v, asstring))) {
					if (ret == gend)
						return ret;

					if (a->wlabel[0] == '_' && sub != NULL && !(a->status&gkeepout)) {
						if (asstring) {
							sub = (Atanorvector*)v;
							int x;
							//Then we try to merge the value together...
							wstring mrg;
							bool replace = true;
							int sz = sub->values.size();
							for (x = 1; x < sz; x++) {
								if (!sub->values[x]->isString()) {
									replace = false;
									break;
								}
								mrg += sub->getustring(x);
							}
							if (replace) {
								for (x = 1; x < sz; x++)
									v->Pop(aMINUSONE);
								v->storevalue(mrg);
							}
						}
					}
					return true;
				}
			}
			i = c;
			continue;
		}

		wstring labelw;
		if ((a->status&gmulti) == gmulti) {
			found = true;
			if (asstring) {
				//a->idrule contains the label size in characters...
				int limit = a->test(labels, labelw, i);
				if (limit == -1)
					found = false;
				else
					i = limit;
			}
			else {
				if (labels[i] != a->wlabel[0])
					found = false;
			}
		}
		else
			found = a->test(labels[i]);

		if (found) {
			if (found == 1 && v != aNULL) {
				if (asstring) {
					if (v->Size() == 1) {
						v->storevalue(L"");
						topop = true;
					}
					if ((a->status&gmulti) == gmulti)
						v->addustringto(labelw, -1);
					else
						v->addstringto(labels[i], -1);
				}
				else
					v->storevalue(labels[i]);
			}

			if ((a->status&gloop) == gloop && a->state == this) {
				i++;
				if (i == labels.size()) {
					if ((status&gend) == gend)
						return true;
					return false;
				}

				topop = false;
				u = -1;
				c = i;
				continue;
			}

			if ((ret = a->state->compare(idthread, gram, labels, ++i, v, asstring)))
				return ret;
		}

		if (topop) {
			v->Pop(aMINUSONE);
			topop = false;
		}
		i = c;
	}

	if ((status&gend) == gend)
		return true;

	return false;
}

char gramstate::comparemap(short idthread, wstring& lkey, GrammarBaseAutomaton* gram, vector<wstring>& labels, int& i, Atanor* m, bool asstring) {
	if (globalAtanor->Error(idthread))
		return gend;

	if (gram->skipblanks) {
		if (gram->skipblanks == 1) {
			while (i < labels.size() && (labels[i] == L" " || labels[i] == L"\t")) i++;
		}
		else {
			while (i < labels.size() && (labels[i][0] <= 32)) i++;
		}
	}

	if ((status&gend) == gend) {
		if (!arcs.size() || i == labels.size())
			return true;
	}
	else {
		if (i == labels.size()) { //if the next element is a function call, then we still can have a look...
			if ((status&gfunction) != gfunction)
				return false;
		}
	}

	int c = i;
	Atanorvector* v = (Atanorvector*)m->Value(lkey);

	bool topop = false;
	bool callfunc = false;
	char found;
	char ret;

	if (gram->function != NULL)
		callfunc = true;

	for (int u = 0; u < arcs.size(); u++) {
		gramarc* a = arcs[u];
		if ((a->status&gfunction) == gfunction) {
			Atanor* kfunc = globalAtanor->Getdeclaration(a->idrule, idthread);
			if (!gram->callfunction(kfunc, idthread, v, i))
				continue;

			if ((ret = a->state->comparemap(idthread, lkey, gram, labels, i, m, asstring)))
				return ret;

			i = c;
			continue;
		}

		if ((a->status&grule) == grule) {
			//then we need to apply a rule to it...
			Atanor* sub = m->Newinstance(idthread);
			Atanorvector* vsub = globalAtanor->Providevector();
			sub->Push(a->wlabel, vsub);

			ret = gram->rules[a->idrule]->comparemap(idthread, a->wlabel, gram, labels, i, sub, asstring);
			found = false;

			if ((a->status&gnegation) == gnegation) {
				if (!ret) {
					found = true;
					if (asstring) {
						if (v->Size() == 1)
							v->storevalue("");
						v->addustringto(labels[i++], -1);
					}
					else
						v->storevalue(labels[i++]);
					sub->Clear();
				}
			}
			else {
				if (ret)
					found = true;
				else
					sub->Release();
			}

			if (found) {
				if (ret == gend) {
					sub->Release();
					return ret;
				}

				if (callfunc) {
					if (!gram->callfunction(idthread, a->wlabel, vsub, i)) {
						sub->Release();
						continue;
					}
				}

				//Different cases:
				//a) asstring --> concatenate strings
				//b) else Pushing strings...
				if (!(a->status&gkeepout)) {
					if (sub->Size()) {
						if (a->wlabel[0] == '_') {
							//then we merge its content with the current structure
							//we check if all elements in vsub are strings...
							v->Merging(vsub);
						}
						else
							v->Push(sub);
					}
				}

				sub->Release();

				found = false;
				//This is a loop, and we have gotten to here, it means that we had some succesfull loops...
				//We do not need to go back into recursion...
				if ((a->status&gloop) == gloop && a->state == this) {
					//this could be a final state...
					if (i == labels.size() && (status&gend) == gend)
						found = true;
					else {
						u = -1;
						c = i;
						continue;
					}
				}

				if (found || (ret = a->state->comparemap(idthread, lkey, gram, labels, i, m, asstring))) {
					if (ret == gend)
						return ret;

					if (!(a->status&gkeepout) && a->wlabel[0] == '_') {
						if (asstring) {
							int x;
							//Then we try to merge the value together...
							string mrg;
							bool replace = true;
							for (x = 0; x < v->values.size(); x++) {
								if (v->values[x]->Type() != a_string) {
									replace = false;
									break;
								}
								mrg += ((Atanorstring*)v->values[x])->value;
							}
							if (replace) {
								v->Clear();
								if (mrg != "")
									v->storevalue(mrg);
							}
						}
					}
					return true;
				}
			}
			i = c;
			continue;
		}

		found = a->test(labels[i]);

		if (found) {
			if (found == 1) {
				if (asstring) {
					if (v->Size() == 0) {
						v->storevalue("");
						topop = true;
					}
					if (v->values.back()->Type() == a_string) {
						if ((a->status&gmulti) == gmulti)
							v->addustringto(a->wlabel, -1);
						else
							v->addustringto(labels[i], -1);
					}
					else {
						if ((a->status&gmulti) == gmulti)
							v->storevalue(a->wlabel);
						else
							v->storevalue(labels[i]);
					}
				}
				else
					v->storevalue(labels[i]);
			}

			if ((a->status&gloop) == gloop && a->state == this) {
				i++;
				if (i == labels.size()) {
					if ((status&gend) == gend)
						return true;
					return false;
				}

				topop = false;
				u = -1;
				c = i;
				continue;
			}

			if ((ret = a->state->comparemap(idthread, lkey, gram, labels, ++i, m, asstring)))
				return ret;
		}

		if (topop) {
			v->Pop(aMINUSONE);
			topop = false;
		}
		i = c;
	}

	if ((status&gend) == gend)
		return true;

	return false;
}

char gramstate::comparemap(short idthread, wstring& lkey, GrammarBaseAutomaton* gram, wstring& labels, int& i, Atanor* m, bool asstring) {
	if (globalAtanor->Error(idthread))
		return gend;

	if (gram->skipblanks) {
		if (gram->skipblanks == 1) {
			while (i < labels.size() && (labels[i] == L' ' || labels[i] == L'\t')) i++;
		}
		else {
			while (i < labels.size() && (labels[i] <= 32)) i++;
		}
	}

	if ((status&gend) == gend) {
		if (!arcs.size() || i == labels.size())
			return true;
	}
	else {
		if (i == labels.size()) { //if the next element is a function call, then we still can have a look...
			if ((status&gfunction) != gfunction)
				return false;
		}
	}

	int c = i;
	Atanorvector* v = (Atanorvector*)m->Value(lkey);

	bool topop = false;
	bool callfunc = false;
	char found;
	char ret;

	if (gram->function != NULL)
		callfunc = true;

	for (int u = 0; u < arcs.size(); u++) {
		gramarc* a = arcs[u];
		if ((a->status&gfunction) == gfunction) {
			Atanor* kfunc = globalAtanor->Getdeclaration(a->idrule, idthread);
			if (!gram->callfunction(kfunc, idthread, v, i))
				continue;

			if ((ret = a->state->comparemap(idthread, lkey, gram, labels, i, m, asstring)))
				return ret;

			i = c;
			continue;
		}

		if ((a->status&grule) == grule) {
			//then we need to apply a rule to it...
			Atanor* sub = m->Newinstance(idthread);
			Atanorvector* vsub = globalAtanor->Providevector();
			sub->Push(a->wlabel, vsub);

			ret = gram->rules[a->idrule]->comparemap(idthread, a->wlabel, gram, labels, i, sub, asstring);
			found = false;

			if ((a->status&gnegation) == gnegation) {
				if (!ret) {
					found = true;
					if (asstring) {
						if (v->Size() == 1)
							v->storevalue("");
						v->addstringto(labels[i++], -1);
					}
					else
						v->storevalue(labels[i++]);
					sub->Clear();
				}
			}
			else {
				if (ret)
					found = true;
				else
					sub->Release();
			}

			if (found) {
				if (ret == gend) {
					sub->Release();
					return ret;
				}

				if (callfunc) {
					if (!gram->callfunction(idthread, a->wlabel, vsub, i)) {
						sub->Release();
						continue;
					}
				}

				//Different cases:
				//a) asstring --> concatenate strings
				//b) else Pushing strings...
				if (!(a->status&gkeepout)) {
					if (sub->Size()) {
						if (a->wlabel[0] == '_') {
							//then we merge its content with the current structure
							//we check if all elements in vsub are strings...
							v->Merging(vsub);
						}
						else
							v->Push(sub);
					}
				}

				sub->Release();

				found = false;
				//This is a loop, and we have gotten to here, it means that we had some succesfull loops...
				//We do not need to go back into recursion...
				if ((a->status&gloop) == gloop && a->state == this) {
					//this could be a final state...
					if (i == labels.size() && (status&gend) == gend)
						found = true;
					else {
						u = -1;
						c = i;
						continue;
					}
				}

				if (found || (ret = a->state->comparemap(idthread, lkey, gram, labels, i, m, asstring))) {
					if (ret == gend)
						return ret;

					if (!(a->status&gkeepout) && a->wlabel[0] == '_') {
						if (asstring) {
							int x;
							//Then we try to merge the value together...
							wstring mrg;
							bool replace = true;
							for (x = 0; x < v->values.size(); x++) {
								if (!v->values[x]->isString()) {
									replace = false;
									break;
								}
								mrg += v->getustring(x);
							}
							if (replace) {
								v->Clear();
								if (mrg != L"")
									v->storevalue(mrg);
							}
						}
					}
					return true;
				}
			}
			i = c;
			continue;
		}

		wstring labelw;
		if ((a->status&gmulti) == gmulti) {
			found = true;
			if (asstring) {
				//a->idrule contains the label size in characters...
				int limit = a->test(labels, labelw, i);
				if (limit == -1)
					found = false;
				else
					i = limit;
			}
			else {
				if (labels[i] != a->wlabel[0])
					found = false;
			}
		}
		else
			found = a->test(labels[i]);

		if (found) {
			if (found == 1) {
				if (asstring) {
					if (v->Size() == 0) {
						v->storevalue(L"");
						topop = true;
					}
					if (v->values.back()->isString()) {
						if ((a->status&gmulti) == gmulti)
							v->addustringto(labelw, -1);
						else
							v->addstringto(labels[i], -1);
					}
					else {
						if ((a->status&gmulti) == gmulti)
							v->storevalue(a->wlabel);
						else
							v->storevalue(labels[i]);
					}
				}
				else
					v->storevalue(labels[i]);
			}

			if ((a->status&gloop) == gloop && a->state == this) {
				i++;
				if (i == labels.size()) {
					if ((status&gend) == gend)
						return true;
					return false;
				}

				topop = false;
				u = -1;
				c = i;
				continue;
			}

			if ((ret = a->state->comparemap(idthread, lkey, gram, labels, ++i, m, asstring)))
				return ret;
		}

		if (topop) {
			v->Pop(aMINUSONE);
			topop = false;
		}
		i = c;
	}

	if ((status&gend) == gend)
		return true;

	return false;
}

Atanor* GrammarAutomaton::Apply(short idthread, Atanor* res, vector<wstring>& labels, bool asstring) {
	wstring lkey;
	mx = 0;
	pmx = 0;

	for (int r = 0; r<rules.size(); r++) {
		lkey = rdictionary[r];
		int i = 0;

		if (res->isMapContainer()) {
			res->Push(lkey, new Atanorvector);
			if (rules[r]->comparemap(idthread, lkey, this, labels, i, res, asstring) && i == labels.size())
				return res;
		}
		else {
			if (res->isVectorContainer())
				res->storevalue(lkey);

			if (rules[r]->compare(idthread, this, labels, i, res, asstring) && i == labels.size()) {
				if (res == aNULL)
					return aTRUE;

				return res;
			}
		}

		res->Clear();
	}

	if (res == aNULL)
		return aFALSE;

	res->Clear();
	return res;
}


bool GrammarAutomaton::Apply(vector<wstring>& labels, autobaseatanvector& res) {
	int i;

	for (int r = 0; r < rules.size(); r++) {
		i = 0;
		res.storevalue(rdictionary[r]);

		wstring s = res.values.back()->value();

		if (rules[r]->compare(this, labels, i, &res) && i == labels.size())
			return true;

		res.Clear();
	}

	return false;
}

Atanor* Atanorgrammar::Apply(short idthread, Atanor* res, vector<wstring>& labels, bool asstring) {
	if (automaton == NULL)
		return res;

	Atanor* r = automaton->Apply(idthread, res, labels, asstring);
	if (globalAtanor->Error(idthread)) {
		r->Clear();
		return globalAtanor->Errorobject(idthread);
	}
	return r;
}


bool Atanorgrammar::Apply(vector<wstring>& labels, autobaseatanvector& r) {
	if (automaton == NULL)
		return false;

	return automaton->Apply(labels, r);
}

//a in b...
Atanor* Atanorgrammar::in(Atanor* context, Atanor* a, short idthread) {
	if (a->Type() == a_uvector)
		return Apply(idthread, aNULL, ((Atanoruvector*)a)->values, false);

	if (a->isVectorContainer()) {
		vector<wstring> v;
		for (int i = 0; i < a->Size(); i++)
			v.push_back(a->getustring(i));
		return Apply(idthread, aNULL, v, false);
	}

	wstring w = a->UString();
	return Apply(idthread, aNULL, w, false);
}


Atanor* GrammarAutomaton::Apply(short idthread, Atanor* res, wstring& labels, bool asstring) {
	wstring lkey;
	mx = 0;
	pmx = 0;

	for (int r = 0; r<rules.size(); r++) {
		lkey = rdictionary[r];
		int i = 0;

		if (res->isMapContainer()) {

			res->Push(lkey, new Atanorvector);
			if (rules[r]->comparemap(idthread, lkey, this, labels, i, res, asstring) && i == labels.size())
				return res;
		}
		else {
			if (res->isVectorContainer())
				res->storevalue(lkey);

			if (rules[r]->compare(idthread, this, labels, i, res, asstring) && i == labels.size()) {
				if (res == aNULL)
					return aTRUE;

				return res;
			}
		}

		res->Clear();
	}

	if (res == aNULL)
		return aFALSE;

	res->Clear();
	return res;
}

Atanor* Atanorgrammar::Apply(short idthread, Atanor* res, wstring& labels, bool asstring) {
	if (automaton == NULL)
		return res;

	Atanor* r = automaton->Apply(idthread, res, labels, asstring);
	if (globalAtanor->Error(idthread)) {
		r->Clear();
		return globalAtanor->Errorobject(idthread);
	}
	return r;
}

//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
Atanor* Atanorgrammar::MethodApply(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//First parameter should be a ATHANOR rule

	Atanor* kvect = Selectacontainer(contextualpattern, idthread);

	Atanor* kval = callfunc->Evaluate(0, contextualpattern, idthread);
	if (kval->Type() == a_uvector)
		return Apply(idthread, kvect, ((Atanoruvector*)kval)->values, false);

	//Else we build our vector out of the characters...
	if (kval->isVectorContainer()) {
		vector<wstring> v;
		for (int i = 0; i < kval->Size(); i++)
			v.push_back(kval->getustring(i));
		return Apply(idthread, kvect, v, false);
	}

	if (kvect->isVectorContainer()) {
		if (kvect->Type() != a_vector) {
			kvect->Release();
			kvect = globalAtanor->Providevector();
		}
	}
	else {
		if (kvect->isMapContainer()) {
			if (kvect->Type() != a_map && kvect->Type() != a_treemap) {
				kvect->Release();
				kvect = globalAtanor->Providemap();
			}
		}
	}

	wstring wstr = kval->UString();
	return Apply(idthread, kvect, wstr, true);
}





