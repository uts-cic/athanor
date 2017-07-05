
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
filename   : atanorimatrix.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorimatrix.h"

//We need to declare once again our local definitions.
Exporting hmap<unsigned short, imatrixMethod>  Atanorimatrix::methods;
Exporting hmap<string, string> Atanorimatrix::infomethods;
Exporting bin_hash<unsigned long> Atanorimatrix::exported;

Exporting short Atanorimatrix::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorimatrix::AddMethod(AtanorGlobal* global, string name, imatrixMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorimatrix::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorimatrix::idtype = global->Getid("imatrix");

	Atanorimatrix::AddMethod(global, "_initial", &Atanorimatrix::MethodInitial, P_ONE | P_TWO, "_initial(int rowsiwe; int columnsize): Initialize the size of the matrix.");
	Atanorimatrix::AddMethod(global, "transpose", &Atanorimatrix::MethodTransposed, P_NONE, "transpose(): return the transposed matrix");
	Atanorimatrix::AddMethod(global, "dimension", &Atanorimatrix::MethodMatrixSize, P_NONE | P_TWO, "dimension(): return the matrix size.\ndimension(int rowsizeint columnsize): set the matrix size.");
	Atanorimatrix::AddMethod(global, "determinant", &Atanorimatrix::MethodDeterminant, P_NONE, "determinant(): return the matrix determinant");
	Atanorimatrix::AddMethod(global, "sum", &Atanorimatrix::MethodSum, P_NONE, "sum(): return the sum of all elements");
	Atanorimatrix::AddMethod(global, "product", &Atanorimatrix::MethodProduct, P_NONE, "product(): return the product of all elements");
	Atanorimatrix::AddMethod(global, "invert", &Atanorimatrix::MethodInversion, P_NONE, "invert(): Return the inverted matrix.");



	global->newInstance[Atanorimatrix::idtype] = new Atanorimatrix(global);
	global->RecordMethods(Atanorimatrix::idtype,Atanorimatrix::exported);

	return true;
}



Atanor* Atanorimatrix::Put(Atanor* idx, Atanor* ke, short idthread) {
	Locking _lock(this);
	
	if (ke == this)
		return aTRUE;

	if (!idx->isIndex()) {
		Clear();
		if (ke->Type() != idtype)
			return aTRUE;
		((Atanorimatrix*)ke)->populate(this);
		return aTRUE;
	}

	Atanor* keyleft;
	Atanor* keyright;

	//Two possibilities:
	//both indexes
	bool interval = idx->isInterval();
	AtanorIndex* kind = (AtanorIndex*)idx;
	if (interval == true && kind->left != aNULL && kind->right != aNULL) {
		keyleft = kind->left->Get(aNULL, aNULL, idthread);
		keyright = kind->right->Get(aNULL, aNULL, idthread);
		populate(keyleft, keyright, ke);
		return aTRUE;
	}
	long r, c;
	AtanorIteration* itr;
	//it should be a map as value or a matriceline
	if (ke->isVectorContainer()) {
		long itx = 0;
		//then it means that only the column key is present
		if (kind->right != NULL && kind->right != aNULL) {
			keyleft = kind->right->Get(aNULL, aNULL, idthread);
			c = keyleft->Integer();			

			itr = ke->Newiteration(false);
			for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
				populate(itx, c, itr->Valueinteger());
				itx++;
			}
			itr->Release();
			return aTRUE;
		}

		//We have the line, not the column
		if (kind->left != NULL && kind->left != aNULL) {
			keyleft = kind->left->Get(aNULL, aNULL, idthread);
			r = keyleft->Integer();			
			itr = ke->Newiteration(false);
			for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
				populate(r, itx, itr->Valueinteger());
				itx++;
			}
			itr->Release();
			return aTRUE;
		}
	}
	else
	if (ke->isMapContainer()) {
		//then it means that only the column key is present
		if (kind->right != NULL && kind->right != aNULL) {
			keyleft = kind->right->Get(aNULL, aNULL, idthread);
			c = keyleft->Integer();			
			itr = ke->Newiteration(false);
			for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
				r = itr->Keyinteger();
				populate(r, c, itr->Valueinteger());
			}
			itr->Release();
			return aTRUE;
		}

		//We have the line, not the column
		if (kind->left != NULL && kind->left != aNULL) {
			keyleft = kind->left->Get(aNULL, aNULL, idthread);
			r = keyleft->Integer();			
			itr = ke->Newiteration(false);
			for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
				c = itr->Keyinteger();
				populate(r, c, itr->Valueinteger());
			}
			itr->Release();
			return aTRUE;
		}
	}
	return globalAtanor->Returnerror("MAT(131): Wrong index or Wrong value (recipient should be a map or a mvector)", idthread);
}

Atanor* Atanorimatrix::Get(Atanor* context, Atanor* idx, short idthread) {
	Locking _lock(this);

	if (!idx->isIndex())
		return this;

	//Two possibilities:
	//both indexes
	bool interval = idx->isInterval();
	AtanorIndex* kind = (AtanorIndex*)idx;
	long r, c;
	if (interval == true && kind->left != aNULL && kind->right != aNULL) {
		r = kind->left->Get(aNULL, aNULL, idthread)->Integer();
		c = kind->right->Get(aNULL, aNULL, idthread)->Integer();
		if (values.check(r) && values[r].check(c))
			return globalAtanor->Provideint(values[r][c]);
		return aZERO;
	}

	//then it means that only the column key is present
	if (kind->right != NULL && kind->right != aNULL) {
		c = kind->right->Get(aNULL, aNULL, idthread)->Integer();
		if (c < 0 || c >= columnsize)
			return aNULL;

		Atanor* kvect = Selectaivector(context);

		for (unsigned short r = 0; r < rowsize; r++) {
			if (values.check(r) && values[r].check(c))
				kvect->storevalue(values[r][c]);
			else
				kvect->storevalue((long)0);
		}
		return kvect;
	}

	if (kind->left != NULL && kind->left != aNULL) {
		//We have the line, not the column
		r = kind->left->Get(aNULL, aNULL, idthread)->Integer();
		if (r < 0 || r >= rowsize)
			return aNULL;

		Atanor* kvect = Selectaivector(context);
		if (values.check(r) == false)
			return kvect;

		KIFLINEINTEGER& line = values[r];
		for (unsigned short i = 0; i < columnsize; i++) {
			if (line.check(i))
				kvect->storevalue(line[i]);
			else
				kvect->storevalue((long)0);
		}
		return kvect;
	}
	return this;
}


Atanor* Atanorimatrix::MethodTransposed(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanorimatrix* res = new Atanorimatrix;
	for (unsigned short r = 0; r < rowsize; r++) {
		if (values.check(r)) {
			for (unsigned short c = 0; c < columnsize; c++) {
				if (values[r].check(c))
					res->populate(c, r, values[r][c]);
			}
		}
	}
	res->rowsize = columnsize;
	res->columnsize = rowsize;
	return res;
}

Atanor* Atanorimatrix::MethodMatrixSize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (callfunc->Size() == 0) {
		Atanor* kvect = Selectaivector(contextualpattern);
		kvect->storevalue((long)rowsize);
		kvect->storevalue((long)columnsize);
		return kvect;
	}
	//we use the values to set the new matrix size...
	int szr = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	int szc = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
	rowsize = szr;
	columnsize = szc;
	values.update(szr, szc);
	return aTRUE;
}

Atanor* Atanorimatrix::MethodDeterminant(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (columnsize < 2 || columnsize != rowsize)
		return 0;
	int det = determinant(rowsize, columnsize, values);
	return globalAtanor->Provideint(det);
}

Atanor* Atanorimatrix::MethodSum(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	long v = 0;
	for (unsigned short r = 0; r < rowsize; r++) {
		if (values.check(r)) {
			for (unsigned short c = 0; c < columnsize; c++) {
				if (values[r].check(c))
					v += values[r][c];
			}
		}
	}
	return globalAtanor->Provideint(v);
}

Atanor* Atanorimatrix::MethodProduct(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	long v = 1;
	for (unsigned short r = 0; r < rowsize; r++) {
		if (values.check(r)) {
			for (unsigned short c = 0; c < columnsize; c++) {
				if (values[r].check(c))
					v *= values[r][c];
			}
		}
	}
	return globalAtanor->Provideint(v);
}

Atanor* Atanorimatrix::MethodInversion(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (rowsize != columnsize)
		return aNULL;
	Atanorimatrix* Y = new Atanorimatrix;
	if (!inversion(*Y)) {
		Y->Release();
		return aNULL;
	}
	return Y;
}





