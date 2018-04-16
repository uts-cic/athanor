/*
 * Xerox Research Centre Europe - Grenoble Laboratory (XRCE)
 *
 * Copyright (C) 2003 - 2017 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
 * This file can only be used with the ATHANOR library,
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : ATANOR
   Version    : 9.60
   filename   : vecte.h
   Date       : 2017/04/01
   Purpose    : Template to handle vector of pointers.
   Programmer : Claude ROUX (claude.roux@xrce.xerox.com)
   Reviewer   :
   */

#ifndef vecte_h
#define vecte_h


template <class Z> class VECTE {
public:

	//Un vecteur de Fonction
	Z* vecteur;
	//sz est la sz actuelle de la liste
	long sz;
	//last element entre... Pour gerer les ajouts en fin de liste...
	long last;

	VECTE(long t = 3) {
		vecteur = NULL;
		if (t > 0)
			vecteur = new Z[t];
		sz = t;
		last = 0;
	}

	~VECTE() {
		delete[] vecteur;
	}

	long size() {
		return last;
	}

	inline void clean() {
		if (vecteur == NULL)
			return;
		for (last = 0; last < sz; last++) {
			if (vecteur[last] != NULL)
				delete vecteur[last];
			vecteur[last] = NULL;
		}
		last = 0;
	}

	inline void clear() {
		last = 0;
	}

	void reserve(long t) {
		if (t <= sz)
			return;

		Z* tfs;

		//on realloue par bloc de t
		tfs = new Z[t];

		if (last)
			memcpy(tfs, vecteur, sizeof(Z)*last);

		delete[] vecteur;
		vecteur = tfs;
		sz = t;
	}

	void taillor(long t) {
		Z* tfs;

		if (t <= sz)
			return;
		//on realloue par bloc de t
		tfs = new Z[t];

		if (last)
			memcpy(tfs, vecteur, sizeof(Z)*last);

		delete[] vecteur;
		vecteur = tfs;
		sz = t;
	}

	inline Z remove(long pos = -1) {
		Z v;
		if (pos < 0) {
			if (last == 0)
				return NULL;
			last--;
			v = vecteur[last];
			vecteur[last] = NULL;
			return v;
		}

		if (pos >= last)
			return NULL;

		v = vecteur[pos];
		//On deplace toutes les cases de 1...
		last--;
		for (; pos < last; pos++)
			vecteur[pos] = vecteur[pos + 1];
		vecteur[last] = NULL;
		return v;
	}


	inline void pop_back() {
		last--;
	}

	inline void insert(long pos, Z val) {
		if (last >= sz)
			taillor(sz + 3);

		//Dans ce cas, c'est un simple push
		if (pos >= last) {
			vecteur[pos] = val;
			last++;
			return;
		}

		//on ajoute alors l'element a sa place
		//si la case est vide on le place a cet endroit
		//sinon on effectue un deplacement de tous les elements vers la droite
		if (vecteur[pos] != NULL) {
			//sinon, on deplace tous les elements d'une case vers la droite
			for (long i = last - 1; i >= pos; i--)
				vecteur[i + 1] = vecteur[i];
			vecteur[pos] = val;
			last++;
		}
		else
			vecteur[pos] = val;
	}

	inline Z back() {
		return vecteur[last - 1];
	}

	inline long push_back(Z val, long inc) {

		if (last >= sz)
			taillor(sz + inc);

		//sinon on ajoute l'element en queue...
		vecteur[last] = val;
		last++;
		return last - 1;
	}

	inline void push_back(VECTE<Z>& z) {
		long ta = z.last + last;
		if (ta > sz)
			taillor(ta + 3);

		for (long i = 0; i<z.last; i++)
			push_back(z.vecteur[i]);
	}

	inline void push_back(Z val) {

		if (last >= sz)
			taillor(sz + 3);

		//sinon on ajoute l'element en queue...
		vecteur[last++] = val;
	}


	inline Z operator [](long pos) {
		return vecteur[pos];
	}

	void erase(long i) {
		if (i < 0 || i >= last)
			return;
		
		if (last == sz) {
			last--;
			while (i < last) {
				vecteur[i] = vecteur[++i];
			}
			vecteur[last] = NULL;
			return;
		}

		while (i < last) {
			vecteur[i] = vecteur[++i];
		}
		last--;
	}

	inline Z removeElement(long i = -1) {
		if (last == 0)
			return NULL;

		long pos = i;

		if (i == -1)
			pos = last - 1;

		Z v = vecteur[pos];
		vecteur[pos] = NULL;

		//On deplace toutes les cases de 1...

		if (i != -1) {
			for (long k = i; k<last - 1; k++)
				vecteur[k] = vecteur[k + 1];
			if (last>0)
				vecteur[last - 1] = NULL;
		}

		last--;
		return v;
	}

	void shaveoff() {
		if (last == sz)
			return;
		Z* tfs;

		//on realloue par bloc de t
		tfs = new Z[last];

		memcpy(tfs, vecteur, sizeof(Z)*last);

		delete[] vecteur;
		vecteur = tfs;
		sz = last;
	}

	void at(long pos, Z val) {
		if (pos >= sz)
			taillor(pos + 3);
		vecteur[pos] = val;
	}

	void operator =(VECTE<Z>& z) {
		last = z.last;
		if (last >= sz) {
			delete[] vecteur;
			sz = last + 3;
			vecteur = new Z[sz];
		}

		if (last)
			memcpy(vecteur, z.vecteur, sizeof(Z)*last);
	}

	inline long search(Z v) {
		for (long i = 0; i< last; i++)
		if (vecteur[i] == v)
			return i;
		return -1;
	}

};

#endif




