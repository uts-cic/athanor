
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
filename   : automaton.cxx
Date       : 2017/04/01
Purpose    : Automaton implementation to handle lexicons
Programmer : Claude ROUX
Reviewer   :
*/

//----------------------------------------------------------------------------------
//------------ AtanorAutomaton Section -----------------------------------------------
//----------------------------------------------------------------------------------
#include "atanor.h"
#include "automaton.h"

ostream* flot_erreur = &cerr;

#define action_first 1
#define action_change 2
#define action_delete 4
#define action_insert 8
#define action_switch 16
#define action_nocase 32

const short action_suball = action_change | action_insert;

#define isaction(a) (flags & a) == a
#define isnotaction(a) (flags & a) == 0

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//----------------------------------------------------------------------------------

bool compileautomaton(string intrans, string outtrans, int latintable, bool norm) {
	AtanorAutomaton a;
	if (!valid_latin_table(latintable))
		return false;

	a.encoding_table = latintable;
	a.normalize = norm;
	return a.compile(intrans, outtrans);
}

bool compileAutomaton(AtanorAutomaton& a, string intrans, string outtrans, int latintable, bool norm) {
	if (!valid_latin_table(latintable))
		return false;

	a.encoding_table = latintable;
	a.normalize = norm;
	return a.compile(intrans, outtrans);
}

AtanorFst::AtanorFst(AtanorAutomaton& a) {
	id = a.garbage.size();
	a.garbage.push_back(this);
	status = 0;
}

AtanorFst::AtanorFst(wstring& w, wstring& lf, int posw, int posl, AtanorAutomaton& a) {
	id = a.garbage.size();
	a.garbage.push_back(this);
	status = 0;
	add(w, lf, posw, posl, a);
}

void AtanorFst::loadarcs(ifstream& dump, hmap<int, long>& allarcs, AtanorAutomaton& a) {
	char buff[2];
	dump.read(buff, 1);
	status = buff[0];

	if (status & xflast)
		return;

	dump.read(buff, 1);
	arcs.nb = buff[0];

	arcs.redimension(arcs.nb + 1);

	octet4 v4;
	octet2 v2;

	unsigned short vx = 1 << 14;
	unsigned short vxx = 1 << 15;
	long val, vbuff;
	for (int i = 0; i < arcs.nb; i++) {
		dump.read(v2.b, 2);
		if ((v2.v & vxx) == vxx) {
			v2.v &= ~vxx;
			arcs.indexes[i] = allarcs[v2.v];
			dump.read(v4.b, 4);
			arcs.table[i] = a.garbage[v4.v];
		}
		else {
			if ((v2.v & vx) == vx) {
				v2.v &= ~vx;
				arcs.indexes[i] = allarcs[v2.v];
				dump.read(buff, 1);
				dump.read(v2.b, 2);
				val = v2.v;
				val <<= 8;
				vbuff = (uchar)buff[0];
				val |= vbuff;
				arcs.table[i] = a.garbage[val];
			}
			else {
				arcs.indexes[i] = allarcs[v2.v];
				dump.read(v2.b, 2);
				arcs.table[i] = a.garbage[v2.v];
			}
		}
	}
}

bool AtanorFst::load(string name, AtanorAutomaton& a) {
	ifstream dump(STR(name), openMode);
	if (dump.fail())
		return false;
	octet2 v;

	//we read the signature+the table encoding... 24,9,16
	char buff[2048];
	dump.read(buff, 5);
	if (buff[0] != 24 || buff[1] != 9 || buff[2] != 16)
		return false;

	a.encoding_table = buff[3];
	a.normalize = buff[4];

	//the size
	dump.read(v.b, 2);
	int sz = v.v;
	int i;
	for (i = 1; i <= sz; i++) {
		dump.read(v.b, 2); //the string size
		dump.read(buff, v.v);
		buff[v.v] = 0;
		a.ialphabet[i] = buff;
		a.alphabet[buff] = i;
	}
	a.ialphabet[0] = "";

	a.fillencoding(true);
	octet4 vv;
	dump.read(vv.b, 4);
	//We read the number of elements...
	AtanorFst* trans;

	int nbarcs = vv.v;
	for (i = 1; i < nbarcs; i++)
		trans = new AtanorFst(a);

	//Nb arc labels
	dump.read(v.b, 2);
	hmap<int, long> allarcs;
	for (i = 0; i < v.v; i++) {
		dump.read(vv.b, 4);
		allarcs[i] = vv.v;
	}

	for (i = 0; i < nbarcs; i++) {
		if ((i % 5000) == 0)
			*flot_erreur << "<";
		a.garbage[i]->loadarcs(dump, allarcs, a);
	}

	*flot_erreur << endl;
	return true;
}

void AtanorFst::savearc(ofstream& dump, hmap<long, int>& allarcs) {
	//we check first if all indexes are identities
	char buff[] = { 0, 0, 0 };

	if (arcs.nb == 0) {
		buff[0] = status | xflast;
		dump.write(buff, 1);
		return;
	}

	octet2 v2;
	octet4 v4;

	buff[0] = status; buff[1] = arcs.nb;
	dump.write(buff, 2);
	unsigned long idx;
	buff[0] = 0; buff[1] = 0;
	for (int i = 0; i < arcs.nb; i++) {
		idx = arcs.table[i]->id;
		v2.v = allarcs[arcs.indexes[i]];
		if ((idx & 0xFFFF) == idx) {
			dump.write(v2.b, 2);
			v2.v = idx;
			dump.write(v2.b, 2);
		}
		else {
			if ((idx & 0xFFFFFF) == idx) {
				buff[0] = idx & 0xFF;
				v2.v |= (1 << 14);
				dump.write(v2.b, 2);
				dump.write(buff, 1);
				v2.v = idx >> 8;
				dump.write(v2.b, 2);
			}
			else {
				v2.v |= (1 << 15);
				dump.write(v2.b, 2);
				v4.v = idx;
				dump.write(v4.b, 4);
			}
		}
	}
}

//we suppose our automaton without any loops... A pure lexicon
bool AtanorFst::compile(string name, AtanorAutomaton& a) {
	map<unsigned short, string> sorted;

	hash_bin<unsigned short, string>::iterator it;
	for (it = a.ialphabet.begin(); it != a.ialphabet.end(); it++) {
		if (it->first != 0)
			sorted[it->first] = it->second;
	}

	ofstream dump(STR(name), ios::binary);
	
	//We write the signature at the beginning of the file
	char buff[5] = { 24, 9, 16, 1, 0 };

	if (a.encoding_table != 1)
		buff[3] = a.encoding_table;
	buff[4] = a.normalize;

	dump.write(buff, 5);

	octet2 v;
	v.v = sorted.size();
	dump.write(v.b, 2);

	for (auto& its : sorted) {
		v.v = its.second.size();
		dump.write(v.b, 2);
		dump.write(its.second.c_str(), its.second.size());
	}

	//traverse(dump);
	//We reindex to skip the NULL...
	int idx = 0;
	int i;
	for (i = 0; i < a.garbage.size(); i++) {
		if (a.garbage[i] != NULL)
			a.garbage[i]->id = idx++;
	}

	octet4 vv;
	vv.v = idx;
	dump.write(vv.b, 4);

	int total = 0;
	hmap<long, int> allarcs;
	vector<long> varcs;
	long e;
	for (i = 0; i < a.garbage.size(); i++)  {
		if (a.garbage[i] == NULL)
			continue;

		for (idx = 0; idx < a.garbage[i]->arcs.nb; idx++) {
			e = a.garbage[i]->arcs.indexes[idx];
			if (allarcs.find(e) == allarcs.end())  {
				allarcs[e] = varcs.size();
				varcs.push_back(e);
			}
			total++;
		}
	}

	v.v = varcs.size();
	dump.write(v.b, 2);
	for (i = 0; i < varcs.size(); i++) {
		vv.v = varcs[i];
		dump.write(vv.b, 4);
	}

	for (i = 0; i < a.garbage.size(); i++) {
		if (a.garbage[i] != NULL)
			a.garbage[i]->savearc(dump, allarcs);
	}

	return true;

}
//-----------------------------------------------------------
//We add a pair of characters to the automaton...
//We conflate the character code of the lower and upper sides on one single 32 bits value...
void AtanorFst::add(wstring& w, wstring& lf, int posw, int posl, AtanorAutomaton& a) {
	unsigned long cw;

	if (posw < w.size()) {
		cw = w[posw++];
		cw = a.index(cw);
		//we merge the bits of the two characters as a key
		if (posl < lf.size()) {
			unsigned long  clf = lf[posl++];
			//the last character is the feature structure, which we do not want to reinterpret
			if (posl < lf.size())
				clf = a.index(clf);
			//the upper side is stored on the upper bits
			cw |= clf << 16;
		}
		else
			status |= xfepsilonupper;

		if (!arcs.find(cw))
			arcs[cw] = new AtanorFst(w, lf, posw, posl, a);
		else
			arcs[cw]->add(w, lf, posw, posl, a);
		return;
	}


	status |= xfarcend;
	if (posl < lf.size()) {
		cw = lf[posl++];
		//the last character is the feature structure, which we do not want to reinterpret
		if (posl < lf.size())
			cw = a.index(cw);
		cw <<= 16;
		if (!arcs.find(cw)) {
			if (a.features.find(cw) != a.features.end())
				arcs[cw] = a.features[cw];
			else
				arcs[cw] = new AtanorFst(w, lf, posw, posl, a);
		}
		else
			arcs[cw]->add(w, lf, posw, posl, a);
		status |= xfepsilonlower;
	}
}

static bool novel(AtanorFst* xf, vector<AtanorFst*>& v) {
	for (int i = 0; i < v.size(); i++) {
		if (v[i] == xf)
			return false;
	}
	v.push_back(xf);
	return true;
}

static bool comparenodes(AtanorFst* x1, AtanorFst* x2) {
	//All arcs should be in both elements
	if (x1 == NULL || x2 == NULL)
		return false;

	if (x1 == x2)
		return true;

	if (x1->status != x2->status)
		return false;

	if (x1->arcs.nb != x2->arcs.nb)
		return false;

	int i;
	for (i = 0; i < x1->arcs.nb; i++) {
		if (x2->arcs.find(x1->arcs.indexes[i]) == false)
			return false;
	}

	for (i = 0; i < x1->arcs.nb; i++) {
		if (!comparenodes(x1->arcs.table[i], x2->arcs[x1->arcs.indexes[i]]))
			return false;
	}

	return true;
}


static void fstsize(int& idx, AtanorFst* xf) {
	if (xf == NULL)
		return;

	idx += xf->arcs.nb;
	for (int i = 0; i < xf->arcs.nb; i++)
		fstsize(idx, xf->arcs.table[i]);
}

static void simplify(hmap<int, vector<AtanorFst*> >& pile, hmap<int, int>& toberemoved, bool);

//In order to remove redundant arcs, we shuffle arcs according either to their first next arc, or the second next one or their size
//The idea here is to create very small vectors of arcs, with a certain level of similarity (either according to their size or to their next arcs)
//to reduce unnecessary complexity in comparing arcs together.
static bool shuffle(vector<AtanorFst*>& v, hmap<int, int>& toberemoved) {
	AtanorFst* xf;
	int idx;
	vector<AtanorFst*> ref;
	hmap<int, vector<AtanorFst*> > subpile1;
	hmap<int, vector<AtanorFst*> > subpile2;
	hmap<int, vector<AtanorFst*> > subpile3;
	hmap<int, vector<AtanorFst*> > subpile4;
	for (int i = 0; i < v.size(); i++) {
		xf = v[i];

		idx = 0;
		//we index on the number of arcs stemming from xf
		fstsize(idx, xf);
		if (subpile3.find(idx) == subpile3.end())
			subpile3[idx] = ref;
		subpile3[idx].push_back(xf);

		//we index on the next arc from the next arc
		if (xf->arcs.nb) {
			if (xf->arcs.table[0]->arcs.nb)
				idx = xf->arcs.table[0]->arcs.indexes[0];
			else
				idx = xf->arcs.indexes[0];
		}
		if (subpile4.find(idx) == subpile4.end())
			subpile4[idx] = ref;
		subpile4[idx].push_back(xf);

		//we index on the first next arc
		if (xf->arcs.nb >= 1)
			idx = xf->arcs.indexes[0];
		else
			idx = 0;

		if (subpile1.find(idx) == subpile1.end())
			subpile1[idx] = ref;
		subpile1[idx].push_back(xf);


		//we index on the second next arc
		if (xf->arcs.nb > 1)
			idx = xf->arcs.indexes[1];
		else
			idx = 0;

		if (subpile2.find(idx) == subpile2.end())
			subpile2[idx] = ref;
		subpile2[idx].push_back(xf);
	}

	//we then detect the largest number of vectors
	//since this value is usually correlated with smaller vectors inside...
	idx = max(subpile1.size(), subpile3.size());
	idx = max(idx, (int)subpile4.size());
	idx = max(idx, (int)subpile2.size());

	if (idx == 1)
		return false;

	if (idx == subpile1.size())
		simplify(subpile1, toberemoved, false);
	else {
		if (idx == subpile2.size())
			simplify(subpile2, toberemoved, false);
		else {
			if (idx == subpile3.size())
				simplify(subpile3, toberemoved, false);
			else
				simplify(subpile4, toberemoved, false);
		}
	}
	return true;
}

//simplify compares lists of arcs gathered according to their size or the ids of the next arcs.
static void simplify(hmap<int, vector<AtanorFst*> >& pile, hmap<int, int>& toberemoved, bool top) {
	int i, n;
	AtanorFst* xe;
	AtanorFst* xf;
	vector<int> ref;


	for (auto& it : pile) {
		if (top)
			*flot_erreur << ".";
		vector<AtanorFst*>& v = it.second;
		if (v.size() > 1) {
			//if the size of this vector is too large
			//we shuffle its elements into smaller vectors.
			if (v.size() > 1000) {
				if (shuffle(v, toberemoved))
					continue;
			}

			for (i = 0; i < v.size(); i++) {
				xf = v[i];

				if (toberemoved.find(xf->id) != toberemoved.end())
					continue;

				for (n = i + 1; n < v.size(); n++) {
					xe = v[n];
					if (comparenodes(xf, xe))
						toberemoved[xe->id] = xf->id;
				}
			}
		}
	}
}


bool AtanorFst::loadtext(string name, AtanorAutomaton& a) {
	ifstream f(name.c_str(), ios::in | ios::binary);
	if (f.fail())
		return false;

	agnostring w;
	agnostring l;
	wstring wstr;
	wstring lstr;

	string feats;
	int compte = 0;
	int fpos = 0;

	while (!f.eof()) {
		getline(f, w);
		getline(f, l);
		w = w.trim();
		l = l.trim();
		if (w.size() && l.size()) {
			fpos = l.rfind("\t", l.size());
			if (fpos != string::npos) {
				feats = l.substr(fpos, l.size() - fpos);
				l = l.substr(0, fpos);
				fpos = a.index(feats);
				a.addfeature(fpos << 16);

				wstr = w.latintounicode(a.encoding_table);
				lstr = l.latintounicode(a.encoding_table);

				lstr += fpos;
				add(wstr, lstr, 0, 0, a);
				compte++;
				if ((compte % 10000) == 0)
					*flot_erreur << "<";
			}
		}
	}

	return factorize(a);
}



bool AtanorFst::addmap(hmap<string, string>& lexicon, AtanorAutomaton& a) {

	agnostring w;
	agnostring l;
	wstring wstr;
	wstring lstr;

	string feats;
	int compte = 0;
	int fpos = 0;

	for (auto& it : lexicon) {
		w = it.first;
		l = it.second;
		if (w.size() && l.size()) {
			fpos = l.rfind("\t", l.size());
			if (fpos != string::npos) {
				feats = l.substr(fpos, l.size() - fpos);
				l = l.substr(0, fpos);
				fpos = a.index(feats);
				a.addfeature(fpos << 16);

				wstr = w.latintounicode(a.encoding_table);
				lstr = l.latintounicode(a.encoding_table);

				lstr += fpos;
				add(wstr, lstr, 0, 0, a);
				compte++;
				if ((compte % 10000) == 0)
					*flot_erreur << "<";
			}
		}
	}

	return factorize(a);
}



bool AtanorFst::factorize(AtanorAutomaton& a) {
	*flot_erreur << endl << a.garbage.size() << " arcs" << endl;
	int i, n;
	AtanorFst* xf;
	hmap<int, vector<AtanorFst*> > pile;
	vector<AtanorFst*> ref;
	for (i = 1; i < a.garbage.size(); i++) {
		xf = a.garbage[i];
		if (xf == NULL)
			continue;
		if (pile.find(xf->arcs.nb) == pile.end())
			pile[xf->arcs.nb] = ref;
		pile[xf->arcs.nb].push_back(xf);
	}

	hmap<int, int> toberemoved;
	//For 0 size arcs, we do not need the full analysis
	if (pile.find(0) != pile.end()) {
		vector<AtanorFst*>& v = pile[0];
		xf = v[0];
		for (i = 1; i < v.size(); i++)
			toberemoved[v[i]->id] = xf->id;

		pile[0].clear();
	}

	*flot_erreur << endl;
	simplify(pile, toberemoved, true);
	*flot_erreur << endl;


	for (auto& et : toberemoved) {
		//First we mark the heads...
		a.garbage[et.second]->status |= xfmark;
	}

	a.start.status |= xfmark;

	AtanorFst* xe;
	//We have identified with simplify the arcs, which are similar. For each of these arcs, we have kept one as
	//the replacement. This procedure replaces each of these arcs with their replacement.
	//The arcs that we want to keep are marked
	for (i = 0; i < a.garbage.size(); i++) {
		xf = a.garbage[i];
		if (xf == NULL)
			continue;

		if (toberemoved.find(xf->id) != toberemoved.end())
			continue;

		for (n = 0; n < xf->arcs.nb; n++) {
			xe = xf->arcs.table[n];
			if (toberemoved.find(xe->id) != toberemoved.end())
				xf->arcs.table[n] = a.garbage[toberemoved[xe->id]];
			else
				xe->status |= xfmark;
		}
	}

	//we then delete the unmarked arcs
	n = 0;
	for (i = 0; i < a.garbage.size(); i++) {
		xf = a.garbage[i];
		if (xf == NULL)
			continue;

		if (xf->status & xfmark) {
			xf->status &= ~xfmark;
			n++;
		}
		else {
			delete xf;
			a.garbage[i] = NULL;
		}
	}

	*flot_erreur << n << " factorized arcs remain" << endl;

	return true;
}


//-----------------------------------------------------------
bool AtanorFst::parse(AtanorAutomaton& a, vector<wstring>& vs, int& i, vector<unsigned long>& indexes) {
	if (i == vs.size())
		return true;

	int j;
	int subpos = -1;
	unsigned long cw;

	AtanorFst* sub = NULL;
	if (vs[i] == L"(") {
		status |= xfarcend;
		subpos = a.garbage.size();
		sub = new AtanorFst(a);
		if (!sub->parse(a, vs, ++i, indexes))
			return false;

		if (i == vs.size() || vs[i++] != L")")
			return false;

		for (j = 0; j < sub->arcs.nb; j++) {
			cw = sub->arcs.indexes[j];
			arcs.add(cw, sub->arcs.table[j]);
		}

		if (i == vs.size()) {
			delete sub;
			a.garbage[subpos] = NULL;
			return true;
		}
	}

	if (vs[i] == L")")
		return true;

	AtanorFst* loop;
	AtanorFst* element;

	if (vs[i] == L"!") {
		i++;
		if (i == vs.size())
			return false;
		j = vs[i][0] - 49;
		cw = indexes[j];
		cw <<= 16;
		loop = new AtanorFst(a);
		loop->status |= xfarcend;
		status |= xfepsilonlower | xfarcend;
		arcs.add(cw, loop);
		return parse(a, vs, ++i, indexes);
	}

	if (vs[i][0] == '$') {
		j = 1;
		if (vs[i].size() == 1) {
			i++;
			if (i == vs.size())
				return false;
			j = 0;
		}

		loop = this;
		AtanorFst* first = NULL;
		unsigned long cwfirst = 0;
		for (; j < vs[i].size(); j++) {
			cw = a.index(vs[i][j]);
			cw |= cw << 16;
			element = new AtanorFst(a);
			if (first == NULL) {
				first = element;
				cwfirst = cw;
			}
			loop->arcs.add(cw, element);
			loop = element;
		}


		if (sub != NULL && first != NULL) {
			for (int e = 0; e < sub->arcs.nb; e++) {
				element = arcs[sub->arcs.indexes[e]];
				element->arcs[cwfirst] = first;
			}
			delete sub;
			a.garbage[subpos] = NULL;
		}

		return loop->parse(a, vs, ++i, indexes);
	}

	if (vs[i][0] == '%') {
		//the character should be next...
		if (vs[i].size() == 1) {
			i++;
			if (i == vs.size())
				return false;

			cw = a.index(vs[i][0]);
		}
		else
			cw = a.index(vs[i][1]);

		cw |= cw << 16;
		loop = new AtanorFst(a);
		if (sub != NULL) {
			for (j = 0; j < sub->arcs.nb; j++) {
				element = arcs[sub->arcs.indexes[j]];
				element->arcs.add(cw, loop);
			}
			delete sub;
			a.garbage[subpos] = NULL;
		}

		arcs.add(cw, loop);
		return loop->parse(a, vs, ++i, indexes);
	}

	if (vs[i] == L"{") {
		i++;
		if (i == vs.size())
			return false;
		vector<AtanorFst*> vl;
		vector<int> gloop;
		while (vs[i] != L"}") {
			j = 0;
			vector<wstring> vsub;
			vsub.push_back(L"$" + vs[i]);
			gloop.push_back(a.garbage.size());
			loop = new AtanorFst(a);
			loop->parse(a, vsub, j, indexes);
			vl.push_back(loop);
			i++;
			if (i == vs.size())
				return false;
		}

		bool reloop = false;
		if ((i + 1) < vs.size() && vs[i + 1] == L"+") {
			i++;
			reloop = true;
		}

		if (vl.size()) {
			loop = vl[0];
			//we are looking for the last arc at the end of this structure			
			while (loop->arcs.size() != 0)
				loop = loop->arcs.table[0];
			//we then parse from the end of this state
			if (!loop->parse(a, vs, ++i, indexes))
				return false;

			if (reloop)
				//we need to add a loop to go back to this currrent arc...
				loop->arcs.add(0, this);

			if (loop->arcs.nb) {
				cw = loop->arcs.indexes[0];
				for (j = 1; j < vl.size(); j++) {
					element = vl[j];
					while (element->arcs.size() != 0)
						element = element->arcs.table[0];
					element->status |= loop->status;
					element->arcs.add(cw, loop->arcs[cw]);
					if (reloop)
						element->arcs.add(0, this);
				}
			}
			for (j = 0; j < vl.size(); j++) {
				loop = vl[j];
				if (loop->arcs.nb) {
					cw = loop->arcs.indexes[0];
					arcs.add(cw, loop->arcs.table[0]);
					if (sub != NULL) {
						for (int e = 0; e < sub->arcs.nb; e++) {
							element = arcs[sub->arcs.indexes[e]];
							element->arcs.add(cw, loop);
						}
					}
				}
				delete loop;
				a.garbage[gloop[j]] = NULL;
			}
		}

		if (sub != NULL) {
			delete sub;
			a.garbage[subpos] = NULL;
		}
		return true;
	}

	if (vs[i] == L"[") {
		i++;
		if (i == vs.size())
			return false;
		vector<unsigned long> vl;
		while (i<vs.size() && vs[i] != L"]") {
			if (vs[i].size() == 3 && vs[i][1] == '-') {
				wchar_t c = vs[i][0];
				wchar_t f = vs[i][2];
				if (c > f)
					return false;
				while (c <= f) {
					cw = a.index(c);
					cw |= cw << 16;
					vl.push_back(cw);
					c++;
				}
				i++;
			}
			else {
				if (vs[i].size() != 1)
					return false;

				if ((i + 1) < vs.size() && vs[i + 1] == L"-") {
					if ((i + 2) >= vs.size() || vs[i + 2].size() != 1)
						return false;

					wchar_t c = vs[i][0];
					wchar_t f = vs[i + 2][0];
					if (c > f)
						return false;
					while (c <= f) {
						cw = a.index(c);
						cw |= cw << 16;
						vl.push_back(cw);
						c++;
					}
					i += 3;
				}
				else {
					cw = a.index(vs[i][0]);
					cw |= cw << 16;
					vl.push_back(cw);
					i++;
				}
			}
		}

		if (i == vs.size())
			return false;

		loop = new AtanorFst(a);

		if ((i + 1) < vs.size() && vs[i + 1] == L"+") {
			i++;
			for (j = 0; j < vl.size(); j++)
				loop->arcs.add(vl[j], loop);
		}

		for (j = 0; j < vl.size(); j++) {
			cw = vl[j];
			arcs.add(cw, loop);

			if (sub != NULL) {
				for (int e = 0; e < sub->arcs.nb; e++) {
					element = arcs[sub->arcs.indexes[e]];
					element->arcs.add(cw, loop);
				}
			}
		}

		if (sub != NULL) {
			delete sub;
			a.garbage[subpos] = NULL;
		}
		return loop->parse(a, vs, ++i, indexes);
	}
	return false;
}

bool AtanorFst::parse(AtanorAutomaton& a, agnostring& expression, vector<unsigned long>& indexes) {
	vector<wstring> vs;
	expression.tokenize(vs, false, false, true);
	int i = 0;

    AtanorFst* xf = new AtanorFst(a);
	if (!xf->parse(a, vs, i, indexes))
		return false;

	arcs.resize(arcs.sz + xf->arcs.nb + 1);
	status |= xf->status;

	for (int i = 0; i < xf->arcs.nb; i++)
		arcs.add(xf->arcs.indexes[i], xf->arcs.table[i]);

	factorize(a);
	return true;
}

void AtanorFst::regulars(AtanorAutomaton& a) {
	//Our features
	vector<unsigned long> indexes;

	string e = "\t+Dig+Card";
	indexes.push_back(a.index(e));
	e = "\t+Dig+Dec";
	indexes.push_back(a.index(e));
	e = "\t+Exp+Dec";
	indexes.push_back(a.index(e));
	e = "\t+Dig+Ord";
	indexes.push_back(a.index(e));
	e = "\t+Dig+Rom";
	indexes.push_back(a.index(e));

	agnostring expression("([- +])[0 1 2 3 4 5 6 7 8 9]+!1(%.[0 1 2 3 4 5 6 7 8 9]+!2([e E]([- +])[0 1 2 3 4 5 6 7 8 9]+!3))");
	parse(a, expression, indexes);

	expression = "[D M C L X V I]+!5";
	parse(a, expression, indexes);

	expression = "{1st 2nd 3rd}!4";
	parse(a, expression, indexes);

	expression = "[0 1 2 3 4 5 6 7 8 9]([0 1 2 3 4 5 6 7 8 9]+)$th!4";
	parse(a, expression, indexes);
}

//----------------------------------TOKENIZE AND MORPHO-ANALYSYS IN ONE SINGE STEP----------------
bool AtanorAutomaton::process(charRead& w, vector<string>& readings, bool parseur) {
	if (!finalize) {
		fillencoding(true);
		start.shuffle();
	}

	string s;
	w.init(w.w);
	w.encoding_table = encoding_table;

	if (start.vprocess(w, s, readings, *this)) {
		if (readings.size() > 1) {
			for (int i = readings.size() - 1; i >= 0; i--) {
				if (w.bends[i] < w.bend)
					readings.erase(readings.begin() + i);
			}
		}

		w.setpos(w.bend, w.cend);
		if (w.addoffsets) {
			agnostring surface;
			surface = w.extract();
			if (surface.trim() == "")
				return false;
			agnostring feat;
			agnostring lem;
			int ps;
			string off = w.offsets();
			if (!readings.size()) {
				readings.push_back(surface);
				readings.push_back(surface);
				readings.push_back(off);
			}
			else {
				surface = surface.latintoutf8(encoding_table);
				vector<string> areading;
				areading.push_back(surface);
				for (int i = 0; i < readings.size(); i++) {
					feat = readings[i];
					ps = feat.find("\t", 0);
					if (ps == string::npos) {
						feat = feat.latintoutf8(encoding_table);
						areading.push_back(feat);
						areading.push_back(off);
						continue;
					}

					ps = feat.find("\t", 0);
					lem = feat.substr(0, ps);
					areading.push_back(lem.latintoutf8(encoding_table));
					lem = off + feat.substr(ps, feat.size() - ps);
					lem = lem.replace("\t", "");
					areading.push_back(lem);
				}
				readings = areading;
			}
		}
		return true;
	}

	//If we are calling this process from GlobalParseurTrans, then we might have other lexicons to check against
	if (parseur)
		return false;

	//Otherwise this is a call from a kif program

	//No value found....
	w.setpos(w.bbegin, w.cbegin);
	w.eset(w.bbegin, w.cbegin);

	BULONG cr = w.nextcode();
	if (c_is_punctuation(cr)) {
		w.eset(w.w);
		s = w.extract();
		s += "\t";
		if (w.addoffsets) {
			string off = w.offsets();
			s += off;
		}
		s += "+PUNCT";
	}
	else {
		if (w.end())
			return false;

		while (!w.end() && !c_is_space(cr) && !c_is_punctuation(cr)) {
			w.eset(w.w);
			cr = w.nextcode();
		}
		if (!w.end())
			w.setpos(w.bend, w.cend);

		s = w.extract();
		if (s == "")
			return false;

		s += "\t";
		if (w.addoffsets) {
			string off = w.offsets();
			s += off;
		}
		s += "+Unknown+NOUN";
	}
	readings.push_back(s);
	return true;
}

bool AtanorFst::process(charRead& w, string& s, vector<string>& res, bool punct, AtanorAutomaton& a) {
	BULONG cr;
	unsigned long cw, u;
	bool endtoken = false;
	bool found = false;
	int bpos, cpos, ubpos, ucpos;
	int i;

	string v;

	if (w.end())
		endtoken = true;
	else {
		ubpos = w.bytepos();
		ucpos = w.charpos();
		cr = w.nextcode();
		bpos = w.bytepos();
		cpos = w.charpos();

		cw = a.code(cr);

		if (c_is_space(cr) || c_is_punctuation(cr) || punct) {
			//we need then to check if we can carry on with arcs...
			if (s != "") {
				if (!cw && c_is_punctuation(cr)) {
					//In this case, we try to convert it to a knowledgable one
					cr = c_unicode_to_latin(cr);
					if (cr != 0)
						cw = a.code(cr);
				}

				if (cw) {
					i = -1;
					while (arcs.checkup(cw, i)) {
						if (!(arcs.indexes[i] & 0xFFFF))
							w.setpos(ubpos, ucpos);

						u = arcs.code(i);
						if (u) {
							v = s + a.ialphabet[u];
							if (arcs.found(i)->process(w, v, res, punct, a))
								found = true;
						}
						else {
							if (arcs.found(i)->process(w, s, res, punct, a))
								found = true;
						}
						w.setpos(bpos, cpos);
					}

					if (found)
						return true;
				}

				//In this case, the character does not belong to the string
				//We try to analyze it...
				w.setpos(ubpos, ucpos);
				endtoken = true;
			}
			else {
				if (c_is_punctuation(cr))
					punct = true;
			}
		}
	}

	if (endtoken) {
		if (status&xfarcend) {
			if (!finals(s, res, a))
				return false;
			w.eset(w.w);
			return true;
		}
		return false;
	}

	if (!arcs.nb)
		return false;

	i = -1;
	if (cw) {
		while (arcs.checkup(cw, i)) {
			if (!(arcs.indexes[i] & 0xFFFF))
				w.setpos(ubpos, ucpos);

			u = arcs.code(i);
			if (u) {
				v = s + a.ialphabet[u];
				if (arcs.found(i)->process(w, v, res, punct, a))
					found = true;
			}
			else {
				if (arcs.found(i)->process(w, s, res, punct, a))
					found = true;
			}
			w.setpos(bpos, cpos);
		}

		if (a.normalize && c_is_upper(cr)) {
			unsigned long cwl = a.code(c_to_lower(cr));
			i = -1;
			while (arcs.checkup(cwl, i)) {
				//the espsilon arcs have already been covered above...
				if (!(arcs.indexes[i] & 0xFFFF))
					continue;

				u = arcs.code(i);
				if (u) {
					v = s + a.ialphabet[u];
					if (arcs.found(i)->process(w, v, res, punct, a))
						found = true;
				}
				else {
					if (arcs.found(i)->process(w, s, res, punct, a))
						found = true;
				}
				w.setpos(bpos, cpos);
			}
		}
	}
	else {
		if (punct) {
			cr = c_unicode_to_latin(cr);
			cw = a.code(cr);
			if (cw)
				return found;

			i = -1;
			while (arcs.checkup(cw, i)) {
				if (!(arcs.indexes[i] & 0xFFFF))
					w.setpos(ubpos, ucpos);

				u = arcs.code(i);
				if (u) {
					v = s + a.ialphabet[u];
					if (arcs.found(i)->process(w, v, res, punct, a))
						found = true;
				}
				else {
					if (arcs.found(i)->process(w, s, res, punct, a))
						found = true;
				}
				w.setpos(bpos, cpos);
			}
		}
	}

	return found;
}

bool AtanorFst_Vectorized::vprocess(charRead& w, string& s, vector<string>& res, AtanorAutomaton& a) {
	BULONG cr;
	unsigned long cw, u;
	bool endtoken = false;
	bool found = false, punct = false;
	int ubpos, ucpos, bpos, cpos;

	if (w.end())
		endtoken = true;
	else {
		ubpos = w.bytepos();
		ucpos = w.charpos();
		cr = w.nextcode();
		bpos = w.bytepos();
		cpos = w.charpos();

		if (c_is_space(cr)) {
			w.eset(bpos, cpos);
			return true;
		}

		cw = a.code(cr);

		if (c_is_punctuation(cr))
			punct = true;
	}

	if (endtoken) {
		if (status&xfarcend) {
			if (!finals(s, res, a))
				return false;
			w.eset(w.w);
			return true;
		}
		return false;
	}


	string v;

	int i, ia;
	if (cw) {
		//then we are in the vectorized zone...
		//Then we use the code as an entry point to the arcs...
		if (a.start.arcsv.check(cw)) {
			for (i = 0; i < a.start.arcsv[cw].size(); i++) {
				ia = a.start.arcsv[cw][i];
				u = arcs.code(ia);
				if (u) {
					v = s + a.ialphabet[u];
					if (arcs.found(ia)->process(w, v, res, punct, a))
						found = true;
				}
				else {
					if (arcs.found(ia)->process(w, s, res, punct, a))
						found = true;
				}
				w.setpos(bpos, cpos);
			}
		}

		if (a.normalize && c_is_upper(cr)) {
			unsigned long cwl = a.code(c_to_lower(cr));
			if (a.start.arcsv.check(cwl)) {
				for (i = 0; i < a.start.arcsv[cwl].size(); i++) {
					ia = a.start.arcsv[cwl][i];
					u = arcs.code(ia);
					if (u) {
						v = s + a.ialphabet[u];
						if (arcs.found(ia)->process(w, v, res, punct, a))
							found = true;
					}
					else {
						if (arcs.found(ia)->process(w, s, res, punct, a))
							found = true;
					}
					w.setpos(bpos, cpos);
				}
			}
		}
	}
	else {
		if (punct) {
			cr = c_unicode_to_latin(cr);
			cw = a.code(cr);
			if (cw) {
				if (a.start.arcsv.check(cw)) {
					for (i = 0; i < a.start.arcsv[cw].size(); i++) {
						ia = a.start.arcsv[cw][i];
						u = arcs.code(ia);
						if (u) {
							v = s + a.ialphabet[u];
							if (arcs.found(ia)->process(w, v, res, punct, a))
								found = true;
						}
						else {
							if (arcs.found(ia)->process(w, s, res, punct, a))
								found = true;
						}
						w.setpos(bpos, cpos);
					}
				}
			}
		}
	}

	if (a.start.arcsv.check(0)) {
		w.setpos(ubpos, ucpos);
		for (i = 0; i < a.start.arcsv[0].size(); i++) {
			ia = a.start.arcsv[0][i];
			u = arcs.code(ia);
			if (u) {
				v = s + a.ialphabet[u];
				if (arcs.found(ia)->process(w, v, res, punct, a))
					found = true;
			}
			else {
				if (arcs.found(ia)->process(w, s, res, punct, a))
					found = true;
			}
			w.setpos(ubpos, ucpos);
		}
	}

	return found;
}


bool AtanorFst::finals(string& s, vector<string>& res, AtanorAutomaton& a) {
	bool found = false;
	if (status&xfarcend) {
		if (status&xfepsilonlower) {
			string v;
			int i = -1;
			while (arcs.checkup(0, i)) {
				v = s + a.ialphabet[arcs.code(i)];
				if (arcs.found(i)->finals(v, res, a))
					found = true;
			}
		}
		else {
			res.push_back(s);
			return true;
		}
	}
	return found;
}
//----------------------------------------------------------------------------------------
bool AtanorFst::up(wstring& w, int pos, string& s, vector<string>& res, AtanorAutomaton& a, int threshold, short flags) {
	string v;

	if (w.size() == pos) {
		if (status&xfarcend) {
			if (status&xfepsilonlower) {
				string v;
				for (int i = 0; i < arcs.nb; i++) {
					//It should not be a transducer arc....
					if (arcs.indexes[i] & 0xFFFF)
						continue;
					v = s + a.ialphabet[arcs.code(i)];
					arcs.found(i)->up(w, pos, v, res, a, threshold, flags);
				}
			}
			else {
				int ps = s.find("\t", 0);
				if (ps != string::npos) {
					v = s;
					v = s_replacestrings(v, "\t", "");
					v.insert(ps, "\t");
				}
				res.push_back(v);
			}
			return true;
		}
		return false;
	}

	int current;
	unsigned long cr = w[pos];
	unsigned long cw = a.code(cr);

	unsigned long u;

	bool found = false;
	int i = -1;
	while (arcs.checkup(cw, i)) {
		if (!(arcs.indexes[i] & 0xFFFF))
			current = pos;
		else
			current = pos + 1;

		u = arcs.code(i);
		if (u) {
			v = s + a.ialphabet[u];
			if (arcs.found(i)->up(w, current, v, res, a, threshold, flags))
				found = true;
		}
		else {
			if (arcs.found(i)->up(w, current, s, res, a, threshold, flags))
				found = true;
		}
	}


	if (flags && threshold) {
		//if we have both flags and threshold
		//we have four cases: action_change, action_delete, action_insert, action_switch
		if (pos == 0 && isnotaction(action_first))
			return found;

		if (isaction(action_delete)) {
			if (up(w, pos + 1, s, res, a, threshold - 1, flags))
				found = true;
		}

		if (isaction(action_switch) && (pos + 1) < w.size()) {
			//we switch the two characters...
			wchar_t c = w[pos];
			w[pos] = w[pos + 1];
			w[pos + 1] = c;

			if (up(w, pos, s, res, a, threshold - 1, flags))
				found = true;

			w[pos + 1] = w[pos];
			w[pos] = c;
		}

		if (isaction(action_nocase) && c_is_upper(cr)) {
			w[pos] = c_to_lower(cr);
			if (up(w, pos, s, res, a, threshold - 1, flags))
				found = true;
			w[pos] = cr;
		}

		if (flags & action_suball) {
			//in that case, we do not try our current character, we accept any others
			for (i = 0; i < arcs.nb; i++) {
				u = arcs.indexes[i] & 0xFFFF;
				if (!u)
					continue;

				if (isaction(action_insert)) {
					u = arcs.code(i);
					if (u) {
						v = s + a.ialphabet[u];
						if (arcs.found(i)->up(w, pos, v, res, a, threshold - 1, flags))
							found = true;
					}
				}

				if (isaction(action_change)) {
					if (u == cw)
						continue;

					u = arcs.code(i);
					if (u) {
						v = s + a.ialphabet[u];
						if (arcs.found(i)->up(w, pos + 1, v, res, a, threshold - 1, flags))
							found = true;
					}
				}
			}
		}
	}

	return found;
}

bool AtanorFst::down(vector<unsigned short>& w, int pos, string& s, vector<string>& res, AtanorAutomaton& a) {
	if (pos == w.size()) {
		if (status & xfarcend) {
			res.push_back(s);
			return true;
		}
		return false;
	}

	bool found = false;
	unsigned long car, code;
	int nxt;
	string v;
	for (int i = 0; i < arcs.nb; i++) {
		code = arcs.code(i);
		car = arcs.character(i);
		if (code) {
			if (code == w[pos]) {
				nxt = pos + 1;
				if ((status & xfarcend) && (status&xfepsilonlower) && !car) {
					if (arcs.found(i)->down(w, nxt, s, res, a)) {
						found = true;
						continue;
					}
				}
			}
			else
				continue;
		}
		else
			nxt = pos;

		if (car) {
			v = s + a.ialphabet[car];
			if (arcs.found(i)->down(w, nxt, v, res, a))
				found = true;
		}
		else {
			if (arcs.found(i)->down(w, nxt, s, res, a))
				found = true;
		}
	}

	return found;
}

bool AtanorAutomaton::down(wstring& w, vector<string>& res) {
	if (!finalize)
		fillencoding(true);

	//first we detect the features...
	int ps = w.find(L"\t", 0);
	if (ps == wstring::npos)
		return false;
	//we need a tab to cut at the feature level...
	vector<unsigned short> v;
	int i;
	for (i = 0; i < ps; i++)
		v.push_back(encoding[w[i]]);

	int fpos;
	for (i = 0; i < sortedmultis.size(); i++) {
		fpos = w.find(sortedmultis[i], ps);
		if (fpos != wstring::npos) {
			w = s_replacestring(w, sortedmultis[i], L"&");
			w[fpos] = multis[sortedmultis[i]];
		}
	}

	for (i = ps + 1; i < w.size(); i++)
		v.push_back((unsigned short)w[i]);

	res.clear();
	string s;
	return start.down(v, 0, s, res, *this);
}


bool CompareStrings(wstring s1, wstring s2) {
	if (s1.size() > s2.size())
		return(true);
	return(false);
}

void AtanorAutomaton::fillencoding(bool add) {
	finalize = true;
	
	for (auto& it : alphabet) {
		agnostring s(it.first);
		if (s.sizec() == 1) {
			ushort p = s.nextcode();
			encoding[p] = it.second;
			decoding[it.second] = p;
		}
		else {
			if (add) {
				wstring u = s.latintounicode(encoding_table);
				if (it.first.find("\t") == string::npos) {
					multis[u] = it.second;
					sortedmultis.push_back(u);
					ialphabet[it.second] = "\t" + it.first;
				}
			}
		}
	}
	sort(sortedmultis.begin(), sortedmultis.end(), CompareStrings);
}





