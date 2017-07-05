
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
filename   : automaton.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX (claude.roux@xrce.xerox.com)
Reviewer   :
*/

#ifndef automaton_h
#define automaton_h

//----------------------------------------------------------------------------------------------------------------------------
const char xfarcend = 1;
const char xflast = 2;
const char xfepsilonlower = 4;
const char xfepsilonupper = 8;
const char xfmark = 16;

#define hashincrement 1

union octet2 {
	unsigned short v;
	char b[2];

	octet2() {
		v = 0;
	}
};

union octet4 {
	unsigned long v;
	char b[4];
	octet4() {
		v = 0;
	}
};

union octet8 {
	unsigned long long v;
	char b[8];
	octet8() {
		v = 0;
	}
};


template <class Z> class arc_map {
public:

	Z* table;
	unsigned long* indexes;
	unsigned char sz, nb, last;

	arc_map<Z>() {
		sz = hashincrement;
		table = new Z[sz];
		memset(table, NULL, sz*sizeof(Z));
		indexes = new unsigned long[sz];
		memset(indexes, 0, sz*sizeof(unsigned long));
		nb = 0;
		last = 255;
	}

	~arc_map<Z>() {
		delete[] table;
		delete[] indexes;
	}

	void redimension(unsigned char sze) {
		if (sz >= sze)
			return;
		delete[] table;
		delete[] indexes;
		sz = sze;
		table = new Z[sz];
		memset(table, NULL, sz*sizeof(Z));
		indexes = new unsigned long[sz];
		memset(indexes, 0, sz*sizeof(unsigned long));
	}

	void resize(unsigned char sze) {
		if (sz >= sze)
			return;

		Z* ntable = new Z[sze];
		memset(ntable, NULL, sze*sizeof(Z));
		unsigned long* nindexes = new unsigned long[sze];
		memset(nindexes, 0, sze*sizeof(unsigned long));

		memcpy(ntable, table, sz*sizeof(Z));
		memcpy(nindexes, indexes, sz*sizeof(unsigned long));
		delete[] table;
		delete[] indexes;
		table = ntable;
		indexes = nindexes;
		sz = sze;
	}

	bool remove(unsigned long r) {
		for (int i = 0; i < nb; i++) {
			if (indexes[i] == r) {
				for (; i < nb - 1; i++) {
					indexes[i] = indexes[i + 1];
					table[i] = table[i + 1];
				}
				nb--;
				indexes[nb] = 0;
				table[nb] = NULL;
				return true;
			}
		}
		return false;
	}

	bool find(unsigned long r) {
		for (int i = 0; i < nb; i++) {
			if (indexes[i] == r) {
				last = i;
				return true;
			}
		}
		last = 255;
		return false;
	}

	Z& found(int i) {
		return table[i];
	}

	unsigned long code(int i) {
		return (indexes[i] >> 16);
	}

	unsigned long character(int i) {
		return (indexes[i] & 0xFFFF);
	}

	bool checkup(unsigned long r, int& l) {
		unsigned long v;
		int i = l + 1;
		for (; i < nb; i++) {
			v = indexes[i] & 0xFFFF;
			if (!v || v == r) {
				l = i;
				return true;
			}
		}
		return false;
	}

	bool checkdown(unsigned long r, int& l) {
		unsigned long v;
		int i = l + 1;
		for (; i < nb; i++) {
			v = indexes[i] >> 16;
			if (!v || v == r) {
				l = i;
				return true;
			}
		}
		return false;
	}

	void add(unsigned long r, Z v) {
		if (v == NULL)
			return;
		if (nb < sz) {
			indexes[nb] = r;
			table[nb] = v;
			nb++;
			return;
		}

		Z* ntable = new Z[sz + hashincrement];
		unsigned long* nindexes = new unsigned long[sz + hashincrement];
		memset(ntable, NULL, (sz + hashincrement)*sizeof(Z));
		memset(nindexes, 0, (sz + hashincrement)*sizeof(unsigned long));
		memcpy(ntable, table, sz*sizeof(Z));
		memcpy(nindexes, indexes, sz*sizeof(unsigned long));
		sz += hashincrement;
		delete[] table;
		delete[] indexes;
		table = ntable;
		indexes = nindexes;
		indexes[nb] = r;
		table[nb] = v;
		nb++;
		return;
	}

	Z& operator [](unsigned long r) {
		if (last != 255 && indexes[last] == r)
			return table[last];

		for (int i = 0; i < nb; i++) {
			if (indexes[i] == r)
				return table[i];
		}

		if (nb < sz) {
			indexes[nb] = r;
			return table[nb++];
		}

		Z* ntable = new Z[sz + hashincrement];
		unsigned long* nindexes = new unsigned long[sz + hashincrement];
		memset(ntable, NULL, (sz + hashincrement)*sizeof(Z));
		memset(nindexes, 0, (sz + hashincrement)*sizeof(unsigned long));
		memcpy(ntable, table, sz*sizeof(Z));
		memcpy(nindexes, indexes, sz*sizeof(unsigned long));
		sz += hashincrement;
		delete[] table;
		delete[] indexes;
		table = ntable;
		indexes = nindexes;
		indexes[nb] = r;
		return table[nb++];
	}

	int size() {
		return nb;
	}
};

class charRead {
public:

	agnostring w;
	int encoding_table;

	bool addoffsets;
	int bbegin;
	int cbegin;
	int bend;
	int cend;

	vector<int> cends;
	vector<int> bends;

	char buff[100];

	charRead() {
		addoffsets = false;
		bbegin = bend = 0;
		cbegin = cend = 0;
	}


	void init(agnostring& w)  {
		cends.clear();
		bends.clear();
		bbegin = bend = w.bytepos;
		cbegin = cend = w.charpos;
	}

	void eset(agnostring& w) {
		cends.push_back(w.charpos);
		bends.push_back(w.bytepos);

		if (cend < w.charpos) {
			cend = w.charpos;
			bend = w.bytepos;
		}
	}

	void eset(int b, int c) {
		bend = b;
		cend = c;
	}

	string extract(agnostring& w) {
		return w.substr(bbegin, bend - bbegin);
	}

	virtual string offsets() {
		sprintf_s(buff, 100, "+%d+%d+%d+%d", bbegin, bend, cbegin, cend);
		return buff;
	}

	virtual BULONG nextcode() {
		return 0;
	}

	virtual bool begin() {
		return false;
	}

	virtual bool end() {
		return false;
	}

	virtual void reset() {}
	void setpos(int b, int c) {
		w.setpos(b, c);
	}

	string extract() {
		return w.substr(bbegin, bend - bbegin);
	}

	int bytepos() {
		return w.bytepos;
	}

	int charpos() {
		return w.charpos;
	}

	virtual int beginbyte() {
		return w.bytepos;
	}

	virtual int beginchar() {
		return w.charpos;
	}


	virtual void offset(long& bb, long& be, long& cb, long& ce) {
		bb = bbegin;
		be = bend;
		cb = cbegin;
		ce = cend;
	}

};

class charReadString : public charRead {
public:

	charReadString(string& s) {
		w = s;
	}

	charReadString(unsigned char* s) {
		w = s;
	}

	bool begin() {
		w.begin();
		init(w);
		return true;
	}

	virtual BULONG nextcode() {
		if (encoding_table == 1)
			return w.nextcode();

		return c_latin_table_to_unicode(encoding_table, w.nextcode());
	}

	virtual bool end() {
		return w.end();
	}

};


class charReadFile : public charRead {
public:

	FILE* afile;
	int bbase, cbase;

	charReadFile(FILE* f) {
		afile = f;
		bbase = 0;
		cbase = 0;
	}

	void readline() {
		w = "";
		unsigned char c;
		bool non_spaces = false;
		while (!feof(afile)) {
			c = fgetc(afile);
			w += c;
			if (c <= 32) {
				//we do not want a line containing only spaces
				if (non_spaces && (c == 10 || c == 13))
					break;
			}
			else
				non_spaces = true;
		}
	}

	bool begin() {
		readline();
		w.begin();
		init(w);
		return true;
	}

	BULONG nextcode() {
		if (w.end()) {
			char c[10];
			c[0] = fgetc(afile);
			int nb = c_detect_utf8((unsigned char*)c);
			if (nb) {
				nb = fread(c + 1, 1, nb, afile);
				c[nb + 1] = 0;
			}
			else
				c[1] = 0;

			w += c;
		}

		if (encoding_table == 1)
			return w.nextcode();

		return c_latin_table_to_unicode(encoding_table, w.nextcode());
	}

	void reset() {
		if (w.end()) {
			bbase += w.size();
			cbase += w.sizec();
			readline();
			w.begin();
			init(w);
		}
	}

	bool end() {
		if (w.end())
			return feof(afile);
		return false;
	}

	void offset(long& bb, long& be, long& cb, long& ce) {
		bb = bbase + bbegin;
		be = bbase + bend;
		cb = cbase + cbegin;
		ce = cbase + cend;
	}

	int beginbyte() {
		return (bbase + w.bytepos);
	}

	int beginchar() {
		return (cbase + w.charpos);
	}

	string offsets() {
		sprintf_s(buff, 100, "+%d+%d+%d+%d", bbase + bbegin, bbase + bend, cbase + cbegin, cbase + cend);
		return buff;
	}

};

//----------------------------------------------------------------------------------------------------------------------------
class AtanorAutomaton;
bool compileAutomaton(AtanorAutomaton& a, string intrans, string outtrans, int latintable, bool norm);

class AtanorFst {
public:
	unsigned char status;
	arc_map<AtanorFst*> arcs;
	int id;

	AtanorFst() {
		id = 0;
		status = 0;
	}

	AtanorFst(AtanorAutomaton& a);

	AtanorFst(wstring& w, wstring& lf, int posw, int posl, AtanorAutomaton& a);

	void add(wstring& w, wstring& lf, int posw, int posl, AtanorAutomaton& a);
	void regulars(AtanorAutomaton& a);
	bool parse(AtanorAutomaton& a, vector<wstring>& vs, int& i, vector<unsigned long>& indexes);
	bool parse(AtanorAutomaton& a, agnostring& e, vector<unsigned long>& indexes);
	bool loadtext(string name, AtanorAutomaton& a);
	bool factorize(AtanorAutomaton& a);
	bool addmap(hmap<string, string>& lexicon, AtanorAutomaton& a);
	bool load(string name, AtanorAutomaton& a);
	void loadarcs(ifstream& dump, hmap<int, long>&, AtanorAutomaton& a);
	bool compile(string name, AtanorAutomaton& a);
	void savearc(ofstream& dump, hmap<long, int>&);
	bool up(wstring& w, int, string& s, vector<string>& res, AtanorAutomaton& a, int threshold, short flags);
	bool down(vector<unsigned short>& w, int, string& s, vector<string>& res, AtanorAutomaton& a);
	bool finals(string& s, vector<string>& res, AtanorAutomaton& a);

	bool process(charRead& w, string& s, vector<string>& res, bool punct, AtanorAutomaton& a);
};

class AtanorFst_Vectorized : public AtanorFst {
public:

	hash_bin<unsigned long, vector<int> > arcsv;

	AtanorFst_Vectorized(AtanorAutomaton& a) : AtanorFst(a) {}

	void shuffle() {
		unsigned long u;
		arcsv.clear();
		for (int i = 0; i < arcs.nb; i++) {
			u = arcs.indexes[i] & 0xFFFF;
			arcsv[u].push_back(i);
		}
	}
	bool vprocess(charRead& w, string& s, vector<string>& res, AtanorAutomaton& a);
};

class AtanorAutomaton {
public:
	vector<AtanorFst*> garbage;
	hmap<string, unsigned short> alphabet;
	hmap<unsigned long, AtanorFst*> features;
	hmap<wstring, unsigned long> multis;
	vector<wstring> sortedmultis;

	hash_bin<unsigned short, string> ialphabet;
	hash_bin<unsigned short, unsigned short> encoding;
	hash_bin<unsigned short, unsigned short> decoding;

	AtanorFst_Vectorized start;
	int encoding_table;
	bool finalize;
	bool normalize;

	AtanorAutomaton() : start(*this), ialphabet(false) {
		normalize = false;
		finalize = false;
		encoding_table = 1;
		ialphabet[0] = "";
		index(9);
		index(10);
		index(13);
		index(32);
		index(160);
		index(0x202F);
	}

	~AtanorAutomaton() {
		//we cannot delete the first element, which is defined here...
		for (int i = 1; i < garbage.size(); i++) {
			if (garbage[i] != NULL)
				delete garbage[i];
		}
	}

	void Clear() {
		for (int i = 1; i < garbage.size(); i++) {
			if (garbage[i] != NULL)
				delete garbage[i];
		}
		alphabet.clear();
		ialphabet.clear();
		features.clear();
		encoding.clear();
		decoding.clear();
		garbage.clear();
	}

	void fillencoding(bool add);

	AtanorFst* addfeature(unsigned long p, AtanorFst* a = NULL) {
		if (features.find(p) == features.end()) {
			if (a == NULL)
				features[p] = new AtanorFst(*this);
			else
				features[p] = a;

			features[p]->status |= xfarcend;
		}
		return features[p];
	}

	void regulars() {
		start.regulars(*this);
	}

	bool up(wstring& w, vector<string>& res, int threshold, short flags) {
		if (!finalize)
			fillencoding(true);

		res.clear();
		string s;
		return start.up(w, 0, s, res, *this, threshold, flags);
	}

	bool down(wstring& w, vector<string>& res);

	int index(string s) {
		if (alphabet.find(s) == alphabet.end()) {
			//the 300 is a horrible hack, but it works with European languages...
			unsigned long fpos = 1 + alphabet.size();
			alphabet[s] = fpos;
			ialphabet[fpos] = s;
			return fpos;
		}

		return alphabet[s];
	}

	int index(unsigned long c) {
		return index(s_unicode_to_utf8(c));
	}

	int code(unsigned long c) {
		return encoding.search(c);
	}

	bool load(string name) {
		return start.load(name, *this);
	}

	bool loadtext(string name) {
		return start.loadtext(name, *this);
	}

	bool addmap(hmap<string, string>& lexicon) {
		return start.addmap(lexicon, *this);
	}

	bool compile(string name, string res) {
		if (loadtext(name) == false)
			return false;
		return start.compile(res, *this);
	}

	bool store(string name) {
		return start.compile(name, *this);
	}

	bool process(charRead& w, vector<string>& res, bool parseur);
};

#endif

