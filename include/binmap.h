/*
* Xerox Research Centre Europe - Grenoble Laboratory
*
* Copyright (C) 2003 - 2017 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
* This file can only be used with the ATHANOR library,
    * it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : ATHANOR
Version    : 0.26
filename   : binmap.h
Date       : 2017/04/01
Purpose    : Implementation of a map sans collision
Programmer : Claude ROUX
Reviewer   :
*/


#ifndef i_binmap
#define i_binmap

#define ushort unsigned short
#define binuint64 unsigned long long

const ushort binbits = 6;
const ushort binsize = 1 << binbits;
const binuint64 binONE = 1;

const binuint64 binval64[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768,
    binONE << 16, binONE << 17, binONE << 18, binONE << 19, binONE << 20, binONE << 21, binONE << 22, binONE << 23, binONE << 24,
    binONE << 25, binONE << 26, binONE << 27, binONE << 28,
    binONE << 29, binONE << 30, binONE << 31, binONE << 32, binONE << 33, binONE << 34, binONE << 35, binONE << 36, binONE << 37,
    binONE << 38, binONE << 39, binONE << 40, binONE << 41,
    binONE << 42, binONE << 43, binONE << 44, binONE << 45, binONE << 46, binONE << 47, binONE << 48, binONE << 49, binONE << 50,
    binONE << 51, binONE << 52, binONE << 53, binONE << 54,
binONE << 55, binONE << 56, binONE << 57, binONE << 58, binONE << 59, binONE << 60, binONE << 61, binONE << 62, binONE << 63 };

template <class Z> class bin_hash;

template<class S, class Z> class bin_iter : public std::iterator<std::forward_iterator_tag, Z> {
    public:
    Z** table;
    Z second;
    S first;
    short base;
    ushort i, j;

    private:
    binuint64* indexes;
    binuint64 filter;
    long tsize;

    public:
    bin_iter<S, Z>() {
        table = NULL;
    }

    bin_iter<S, Z>(Z** n, binuint64* idx, long sz, short b = 0) {
        first = 0;
        base = b;
        tsize = sz;
        table = n;
        indexes = idx;
        i = 0;
        j = 0;
        filter = 0;
        if (n != NULL)
            next();
    }

    bin_iter<S, Z>(Z** n, binuint64* idx, long sz, ushort ix, ushort jx, short b = 0) {
        base = b;
        indexes = idx;
        table = n;
        tsize = sz;
        i = ix;
        j = jx;
        first = (((i + base) << binbits) + j);
        second = table[i][j];
        filter = indexes[i] >> (j + 1);
    }


    bin_iter<S, Z>& operator=(const bin_iter<S, Z>& raw) {
        base = raw.base;
        filter = raw.filter;
        table = raw.table;
        indexes = raw.indexes;
        tsize = raw.tsize;
        i = raw.i;
        j = raw.j;
        first = raw.first;
        second = raw.second;
        return (*this);
    }


    bin_iter& operator++() {
        next();
    }

    bin_iter<S, Z>* operator->()  {
        return this;
    }

    inline void next() {
        while (i < tsize) {
            if (!j)
                filter = indexes[i];

            if (filter) {
                if (!(filter & 1)) {
                    while (!(filter & 15)) {
                        filter >>= 4;
                        j = j + 4;
                    }
                    while (!(filter & 1)) {
                        filter >>= 1;
                        j++;
                    }
                }
                first = (((i + base) << binbits) + j);
                second = table[i][j];
                filter >>= 1;
                if (!filter) {
                    j = 0;
                    i++;
                }
                else
                    j++;
                return;
            }
            j = 0;
            i++;
        }

        first = 0;
        table = NULL;
        tsize = -1;
    }

    bin_iter<S, Z>& operator++(int) {
        next();
        return *this;
    }

    friend bool operator!=(bin_iter<S, Z> a, bin_iter<S, Z> b) {
        if (a.table == b.table)
            return false;
        return true;
    }

    friend bool operator==(bin_iter<S, Z> a, bin_iter<S, Z> b) {
        if (a.table == b.table)
            return true;
        return false;
    }
};

template <class Z> class bin_hash {
    public:
    long tsize;
    Z** table;
    binuint64* indexes;
    bool initialization;

    typedef bin_iter<short, Z> iterator;

    iterator begin(){ return iterator(table, indexes, tsize); }
    iterator end() {
        return iterator();
    }

    bin_hash(bool init = true)  {
        initialization = init;
        tsize = 4;
        table = new Z*[tsize];
        indexes = new binuint64[tsize];

        table[0] = NULL;
        table[1] = NULL;
        table[2] = NULL;
        table[3] = NULL;
        indexes[0] = 0;
        indexes[1] = 0;
        indexes[2] = 0;
        indexes[3] = 0;
    }

    bin_hash(bin_hash<Z>& t) {
        tsize = t.tsize;
        table = new Z*[tsize];
        indexes = new binuint64[tsize];
        initialization = t.initialization;

        for (long i = 0; i < t.tsize; i++) {
            if (t.indexes[i]) {
                table[i] = new Z[binsize];
                indexes[i] = t.indexes[i];
                for (long j = 0; j < binsize; j++)
                    table[j] = t.table[j];
            }
            else {
                table[i] = NULL;
                indexes[i] = 0;
            }
        }
    }

    ~bin_hash() {
        for (long i = 0; i < tsize; i++) {
            if (table[i] != NULL)
                delete[] table[i];
        }
        delete[] table;
        delete[] indexes;
    }

    //pour trouver une valeur...
    iterator find(ushort r) {
        ushort i = r >> binbits;
        if (i >= tsize || !indexes[i])
            return iterator();

        r = r - (i << binbits);
        if (!(indexes[i] & binval64[r]))
            return iterator();

        return iterator(table, indexes, tsize, i, r);
    }

    bool get(ushort p, ushort& i, ushort& r) {
        i = p >> binbits;
        if (i >= tsize || !indexes[i])
            return false;
        r = p - (i << binbits);
        if (!(indexes[i] & binval64[r]))
            return false;

        return true;
    }

    bool check(ushort r) {
        ushort i = r >> binbits;
        if (i >= tsize || !indexes[i])
            return false;
        r = r - (i << binbits);
        if (!(indexes[i] & binval64[r]))
            return false;

        return true;
    }

    Z search(ushort r) {
        ushort i = r >> binbits;
        if (i >= tsize || !indexes[i])
            return NULL;

        r = r - (i << binbits);
        return table[i][r];
    }

    //nettoyage
    void clear() {
        for (long i = 0; i < tsize; i++) {
            if (table[i] != NULL) {
                delete[] table[i];
                table[i] = NULL;
                indexes[i] = 0;
            }
        }
    }

    bool empty() {
        for (long i = 0; i < tsize; i++) {
            if (indexes[i])
                return false;
        }
        return true;
    }

    void erase(ushort r) {
        ushort i = r >> binbits;
        if (table[i] == NULL)
            return;

        r = r - (i << binbits);
        indexes[i] &= ~binval64[r];
        table[i][r] = NULL;
    }

    size_t size() {
        long nb = 0;
        binuint64 filter;

        for (long i = 0; i < tsize; i++) {
            if (table[i] != NULL) {
                filter = indexes[i];
                while (filter) {
                    if (!(filter & 1)) {
                        while (!(filter & 65535))
                            filter >>= 16;
                        while (!(filter & 255))
                            filter >>= 8;
                        while (!(filter & 15))
                            filter >>= 4;
                        while (!(filter & 1))
                            filter >>= 1;
                    }
                    nb++;
                    filter >>= 1;
                }
            }
        }

        return nb;
    }

    void resize(long sz) {
        Z** ntable = new Z*[sz];
        binuint64* nindexes = new binuint64[sz];

        long i;
        for (i = 0; i < tsize; i++) {
            ntable[i] = table[i];
            nindexes[i] = indexes[i];
        }

        tsize = sz;
        for (; i < tsize; i++)  {
            ntable[i] = NULL;
            nindexes[i] = 0;
        }
        delete[] table;
        delete[] indexes;
        table = ntable;
        indexes = nindexes;
    }

    Z& operator [](ushort r) {
        ushort i = r >> binbits;
        r = r - (i << binbits);
        if (i >= tsize)
            resize(i + 2);
        if (table[i] == NULL) {
            table[i] = new Z[binsize];
            if (initialization)
                memset(table[i], NULL, sizeof(Z) << binbits);
        }
        indexes[i] |= binval64[r];
        //indexes[i] |= (1 << r);
        return table[i][r];
    }

    void put(ushort r, Z v) {
        ushort i = r >> binbits;
        r = r - (i << binbits);
        if (i >= tsize)
            resize(i + 2);
        if (table[i] == NULL) {
            table[i] = new Z[binsize];
            if (initialization)
                memset(table[i], NULL, sizeof(Z) << binbits);
        }
        indexes[i] |= binval64[r];
        //indexes[i] |= (1 << r);
        table[i][r] = v;
    }

    //affectation
    void operator =(bin_hash<Z>& t) {
        long i;

        for (i = 0; i < tsize; i++) {
			if (indexes[i])
				delete[] table[i];
        }

		if (tsize != t.tsize) {
			delete[] table;
			delete[] indexes;

			tsize = t.tsize;
			table = new Z*[tsize];
			indexes = new binuint64[tsize];
		}

		initialization = t.initialization;

		for (i = 0; i < t.tsize; i++) {
			if (t.indexes[i]) {
				table[i] = new Z[binsize];
				indexes[i] = t.indexes[i];
				for (long j = 0; j < binsize; j++)
					table[j] = t.table[j];
			}
			else {
				table[i] = NULL;
				indexes[i] = 0;
			}
		}
	}
};

template <class L, class Z> class hash_bin {
    public:
    L tsize;
    Z** table;
    binuint64* indexes;
    bool initmem;

    typedef bin_iter<L, Z> iterator;

    iterator begin(){ return iterator(table, indexes, tsize); }
    iterator end() {
        return iterator();
    }

    hash_bin(bool m = true)  {
        initmem = m;
        tsize = 4;
        table = new Z*[tsize];
        indexes = new binuint64[tsize];

        table[0] = NULL;
        table[1] = NULL;
        table[2] = NULL;
        table[3] = NULL;
        indexes[0] = 0;
        indexes[1] = 0;
        indexes[2] = 0;
        indexes[3] = 0;
    }

    hash_bin(L tz, bool m)  {
        initmem = m;
        tsize = tz;
        if (tsize < 2)
            tsize = 2;

        table = new Z*[tsize];
        indexes = new binuint64[tsize];

        for (short i = 0; i < tz; i++) {
            table[i] = NULL;
            indexes[i] = 0;
        }
    }

    hash_bin(hash_bin<L, Z>& t) {
        initmem = t.initmem;

        tsize = t.tsize;
        table = new Z*[tsize];
        indexes = new binuint64[tsize];

        for (long i = 0; i < t.tsize; i++) {
			if (t.indexes[i]) {
                table[i] = new Z[binsize];
                indexes[i] = t.indexes[i];
                for (short j = 0; j < binsize; j++)
                    table[i][j] = t.table[i][j];
            }
            else {
                table[i] = NULL;
                indexes[i] = 0;
            }
        }
    }

    ~hash_bin() {
        for (L i = 0; i < tsize; i++) {
            if (table[i] != NULL)
                delete[] table[i];
        }
        delete[] table;
        delete[] indexes;
    }

    long countnull() {
        long nb = 0;
        for (L j = 0; j < tsize; j++) {
            if (table[j] == NULL)
                nb++;
        }
        return nb;
    }

    //pour trouver une valeur...
    iterator find(L r) {
        L i = r >> binbits;
        if (i >= tsize || !indexes[i])
            return iterator();

        r = r - (i << binbits);
        if (!(indexes[i] & binval64[r]))
            return iterator();

        return iterator(table, indexes, tsize, i, r);
    }

    bool get(L p, L& i, L& r) {
        i = p >> binbits;
        if (i >= tsize || !indexes[i])
            return false;
        r = p - (i << binbits);
        if (!(indexes[i] & binval64[r]))
            return false;

        return true;
    }

    bool check(L r) {
        L i = r >> binbits;
        if (i >= tsize || !indexes[i])
            return false;
        r = r - (i << binbits);
        if (!(indexes[i] & binval64[r]))
            return false;

        return true;
    }

    Z search(L r) {
        L i = r >> binbits;
        if (i >= tsize || !indexes[i])
            return NULL;

        r = r - (i << binbits);
        return table[i][r];
    }

    //nettoyage
    void clear() {
        for (L i = 0; i < tsize; i++) {
            if (table[i] != NULL) {
                delete[] table[i];
                table[i] = NULL;
                indexes[i] = 0;
            }
        }
    }

    bool empty() {
        for (L i = 0; i < tsize; i++) {
            if (indexes[i])
                return false;
        }
        return true;
    }

    void erase(L r) {
        L i = r >> binbits;
        if (table[i] == NULL)
            return;

        r = r - (i << binbits);
        indexes[i] &= ~binval64[r];
    }

    void erase(iterator& it) {
        if (table[it.i] == NULL)
            return;

        indexes[it.i] &= ~binval64[it.j];
    }

    size_t size() {
        size_t nb = 0;
        binuint64 filter;

        for (L i = 0; i < tsize; i++) {
            if (table[i] != NULL) {
                filter = indexes[i];
                while (filter) {
                    if (!(filter & 1)) {
                        while (!(filter & 65535))
                            filter >>= 16;
                        while (!(filter & 255))
                            filter >>= 8;
                        while (!(filter & 15))
                            filter >>= 4;
                        while (!(filter & 1))
                            filter >>= 1;
                    }
                    nb++;
                    filter >>= 1;
                }
            }
        }

        return nb;
    }

    void resize(L sz) {
        Z** ntable = new Z*[sz];
        binuint64* nindexes = new binuint64[sz];

        L i;
        for (i = 0; i < tsize; i++) {
            ntable[i] = table[i];
            nindexes[i] = indexes[i];
        }

        tsize = sz;
        for (; i < tsize; i++)  {
            ntable[i] = NULL;
            nindexes[i] = 0;
        }
        delete[] table;
        delete[] indexes;
        table = ntable;
        indexes = nindexes;
    }

    Z& operator [](L r) {
        L i = r >> binbits;
        r = r - (i << binbits);
        if (i >= tsize)
            resize(i + 2);
        if (table[i] == NULL) {
            table[i] = new Z[binsize];
            if (initmem)
                memset(table[i], NULL, sizeof(Z) << binbits);
        }
        indexes[i] |= binval64[r];
        //indexes[i] |= (1 << r);
        return table[i][r];
    }

    //affectation
    void operator =(hash_bin<L, Z>& t) {
		L i;
		for (i = 0; i < tsize; i++) {
			if (indexes[i])
				delete[] table[i];
		}

		if (tsize != t.tsize) {
			delete[] table;
			delete[] indexes;


			tsize = t.tsize;
			table = new Z*[tsize];
			indexes = new binuint64[tsize];
		}

		initmem = t.initmem;

		for (i = 0; i < t.tsize; i++) {
			if (t.indexes[i]) {
				table[i] = new Z[binsize];
				indexes[i] = t.indexes[i];
				for (short j = 0; j < binsize; j++)
					table[i][j] = t.table[i][j];
			}
			else {
				table[i] = NULL;
				indexes[i] = 0;
			}
		}
	}	
};

template <class Z> class hash_short {
    private:
    short hashshortsz;
    short hashshortkey; // 65536/hashshortsz == 2^^hashshortkey

    public:
    hash_bin<unsigned short, Z>** table;

    hash_short(short h, short k)  {
        hashshortsz = h;
        hashshortkey = k;
        table = new hash_bin<unsigned short, Z>*[hashshortsz];
        for (long i = 0; i < hashshortsz; i++)
            table[i] = NULL;
    }

    ~hash_short() {
        for (long i = 0; i < hashshortsz; i++) {
            if (table[i] != NULL)
                delete table[i];
        }
        delete[] table;
    }

    bool check(ushort r) {
        ushort i = r >> hashshortkey;
        if (table[i] == NULL)
            return false;
        //65536/hashshortsz = hashshortsz192 = 2^^13
        r = r - (i << hashshortkey);
        return table[i]->check(r);
    }

    Z& operator [](ushort r) {
        ushort i = r >> hashshortkey;
        if (table[i] == NULL)
            table[i] = new hash_bin<ushort, Z>(2, true);

        r = r - (i << hashshortkey);
        return (*table[i])[r];
    }

    long countnull() {
        long nb = 0;
        for (long i = 0; i < hashshortsz; i++) {
            if (table[i] == NULL)
                nb++;
            else {
                for (long j = 0; j < table[i]->tsize; j++) {
                    if (table[i]->table[j] == NULL)
                        nb++;
                }
            }
        }
        return nb;
    }
};


template <class Z> class basebin_hash {
    public:
    Z** table;
    binuint64* indexes;
    short base;
    long tsize;
    bool initmem;

    typedef bin_iter<short, Z> iterator;

    iterator begin(){ return iterator(table, indexes, tsize, base); }
    iterator end() {
        return iterator();
    }

    basebin_hash(bool m = true)  {
        base = -1;
        tsize = 1;
        table = new Z*[tsize];
        indexes = new binuint64[tsize];

        table[0] = NULL;
        indexes[0] = 0;
        initmem = m;
    }

    basebin_hash(basebin_hash<Z>& t) {
        static long sz = sizeof(Z) << binbits;
        tsize = t.tsize;
        base = t.base;
        table = new Z*[tsize];
        indexes = new binuint64[tsize];
        initmem = t.initmem;

        for (long i = 0; i < t.tsize; i++) {
			if (t.indexes[i]) {
                table[i] = new Z[binsize];
                indexes[i] = t.indexes[i];
                if (initmem)
                    memcpy(table[i], t.table[i], sz);
                else {
                    for (short j = 0; j < binsize; j++) {
                        table[i][j] = t.table[i][j];
                    }
                }
            }
            else {
                table[i] = NULL;
                indexes[i] = 0;
            }
        }
    }

    ~basebin_hash() {
        for (long i = 0; i < tsize; i++) {
            if (table[i] != NULL)
                delete[] table[i];
        }
        delete[] table;
        delete[] indexes;
    }

    bool check(ushort r) {
        short i = r >> binbits;
        r = r - (i << binbits);
        i -= base;
        if (i < 0 || i >= tsize || !indexes[i])
            return false;
        if (!(indexes[i] & binval64[r]))
            return false;

        return true;
    }

	bool check(short i, short r) {
		i -= base;
		if (i < 0 || i >= tsize || !indexes[i])
			return false;
		if (!(indexes[i] & binval64[r]))
			return false;

		return true;
	}


    Z search(ushort r) {
        short i = r >> binbits;
        r = r - (i << binbits);
        i -= base;
        if (i < 0 || i >= tsize || !indexes[i])
            return NULL;

        return table[i][r];
    }

    iterator find(ushort r) {
        if (base == -1)
            return iterator();

        short i = r >> binbits;
        r = r - (i << binbits);
        i -= base;
        if (i < 0 || i >= tsize || !indexes[i])
            return iterator();

        if (!(indexes[i] & binval64[r]))
            return iterator();

        return iterator(table, indexes, tsize, i, r);
    }

    bool get(ushort p, short& i, ushort& r) {
        i = p >> binbits;
        r = p - (i << binbits);
        i -= base;
        if (i < 0 || i >= tsize || !indexes[i])
            return false;

        if (!(indexes[i] & binval64[r]))
            return false;

        return true;
    }

    void erase(ushort r) {
        short i = r >> binbits;
        r = r - (i << binbits);
        i -= base;
        if (i < 0 || i >= tsize || !indexes[i])
            return;

        indexes[i] &= ~binval64[r];
    }

    size_t size() {
        long nb = 0;
        binuint64 filter;

        for (long i = 0; i < tsize; i++) {
            if (table[i] != NULL) {
                filter = indexes[i];
                while (filter) {
                    if (!(filter & 1)) {
                        while (!(filter & 65535))
                            filter >>= 16;
                        while (!(filter & 255))
                            filter >>= 8;
                        while (!(filter & 15))
                            filter >>= 4;
                        while (!(filter & 1))
                            filter >>= 1;
                    }
                    nb++;
                    filter >>= 1;
                }
            }
        }

        return nb;
    }

    //nettoyage
    void clear() {
        for (long i = 0; i < tsize; i++) {
            if (table[i] != NULL) {
                delete[] table[i];
                table[i] = NULL;
                indexes[i] = 0;
            }
        }
        base = -1;
    }

    //We insert some new boxes before the position 0
    void insert(short p) {
        short inc = base - p;
        long sz = inc + tsize;
        Z** ntable = new Z*[sz];
        binuint64* nindexes = new binuint64[sz];

        tsize = sz;
        for (short i = 0; i < tsize; i++)  {
            if (i >= inc) {
                ntable[i] = table[i - inc];
                nindexes[i] = indexes[i - inc];
            }
            else {
                ntable[i] = NULL;
                nindexes[i] = 0;
            }
        }

        delete[] table;
        delete[] indexes;
        table = ntable;
        indexes = nindexes;

        base = p; //this the new zero position
    }

    void resize(long sz) {
        Z** ntable = new Z*[sz];
        binuint64* nindexes = new binuint64[sz];

        long i;
        for (i = 0; i < tsize; i++) {
            ntable[i] = table[i];
            nindexes[i] = indexes[i];
        }

        tsize = sz;
        for (; i < tsize; i++)  {
            ntable[i] = NULL;
            nindexes[i] = 0;
        }
        delete[] table;
        delete[] indexes;
        table = ntable;
        indexes = nindexes;
    }

	//Only if you are sure that the element exists...
	Z&  get(ushort r) {
		short i = r >> binbits;
		r = r - (i << binbits);
		i -= base;
		return table[i][r];
	}

    Z& operator [](ushort r) {
        short i = r >> binbits;
        r = r - (i << binbits);
        if (base == -1) {
            base = i;
            i = 0;
        }
        else {
            if (i < base){
                insert(i);
                i = 0;
            }
            else
                i -= base;
        }
        if (i >= tsize)
            resize(i + 2);
        if (table[i] == NULL) {
            table[i] = new Z[binsize];
            if (initmem)
                memset(table[i], NULL, sizeof(Z) << binbits);
        }
        indexes[i] |= binval64[r];
        return table[i][r];
    }

    //affectation
    void operator =(basebin_hash<Z>& t) {
		static long sz = sizeof(Z) << binbits;

        long i;
		for (i = 0; i < tsize; i++) {
			if (indexes[i])
				delete[] table[i];
		}

		if (tsize != t.tsize) {
			delete[] table;
			delete[] indexes;

			tsize = t.tsize;
			table = new Z*[tsize];
			indexes = new binuint64[tsize];
		}

		initmem = t.initmem;
		base = t.base;

		for (i = 0; i < t.tsize; i++) {
			if (t.indexes[i]) {
				table[i] = new Z[binsize];
				indexes[i] = t.indexes[i];
				if (initmem)
					memcpy(table[i], t.table[i], sz);
				else {
					for (short j = 0; j < binsize; j++) {
						table[i][j] = t.table[i][j];
					}
				}
			}
			else {
				table[i] = NULL;
				indexes[i] = 0;
			}
		}
	}
};

#endif


