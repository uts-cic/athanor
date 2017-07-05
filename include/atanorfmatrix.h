
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
filename   : atanorfmatrix.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX (claude.roux@xrce.xerox.com)
Reviewer   :
*/

#ifndef atanorfmatrix_h
#define atanorfmatrix_h

#include "atanor.h"
#include "vecte.h"
#include "atanorstring.h"
#include "matrix.h"

#ifdef WIN32
#include <math.h>
#endif

#define NMAX 100
#define TINY 1.5e-16
#define REAL double
#define ZERO (double)0.0
#define ONE (double)1.0
#define TWO (double)2.0

#ifdef UNIX
#include <math.h>
#include <float.h>
#define ABS(x) fabs((double)x)
#define TRUE true
#define FALSE false
#else
#define ABS(x) abs((double)x)
#endif

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorfmatrix;
//This typedef defines a type "fmatrixMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorfmatrix::*fmatrixMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
class Atanorfmatrix : public AtanorObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging hmap<unsigned short, fmatrixMethod> methods;
	static Exchanging hmap<string, string> infomethods;
	static Exchanging bin_hash<unsigned long> exported;

	static Exchanging short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	KIFMATRIX<KIFLINEFLOAT> values;

	long rowsize;
	long columnsize;

	//---------------------------------------------------------------------------------------------------------------------
	Atanorfmatrix(AtanorGlobal* g, Atanor* parent = NULL) : AtanorObject(g, parent) {
		//Do not forget your variable initialisation
	}

	Atanorfmatrix() {
		//Do not forget your variable initialisation
	}

	void Methods(Atanor* v) {

		for (auto& it : infomethods)
			v->storevalue(it.first);
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
		return Atanorfmatrix::idtype;
	}
	string Typename() {
		return "fmatrix";
	}

	bool isString() {
		return false;
	}

	bool isNumber() {
		return true;
	}

	bool isBoolean() {
		return false;
	}

	bool isFrame() {
		return false;
	}
	Atanor* Atom(bool forced = false) {
		return this;
	}
	//If it is too complex to duplicate an element (for instance when passed to a function)
	//then you can use return false...  Usually in that case, Atom should always be just: return this;
	bool duplicateForCall() {
		return false;
	}

	bool isFloat() {
		return true;
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
		return new Atanorfmatrix;
	}
	AtanorIteration* Newiteration(bool direction);
	static void AddMethod(AtanorGlobal* g, string name, fmatrixMethod func, unsigned long arity, string infos);
	static bool InitialisationModule(AtanorGlobal* g, string version);
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	Atanor* MethodInitial(Atanor* context, short idthread, AtanorCall* callfunc) {
		rowsize = callfunc->Evaluate(0, context, idthread)->Integer(); //would match i as in the example above: mynewtype toto(i,j);
		columnsize = 1;
		if (callfunc->Size() == 2)
			columnsize = callfunc->Evaluate(1, context, idthread)->Integer(); //would match j as in the example above: mynewtype toto(i,j);	
		values.init(rowsize);
		return aTRUE;
	}
	Atanor* MethodTransposed(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodMatrixSize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodDeterminant(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodSum(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodProduct(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodInversion(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	//---------------------------------------------------------------------------------------------------------------------
	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
	//is returned by the Declaration method.
	Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		Locking _lock(this);
		return (this->*methods[idname])(contextualpattern, idthread, callfunc);
	}
	void cleaning() {
		values.clear();
		rowsize = 0;
		columnsize = 0;
	}
	void init(int rs, int cs) {
		rowsize = rs;
		columnsize = cs;
		values.init(rs);
	}
	void populate(long r, long c, double val) {
		values[r][c] = val;
	}
	void populate(Atanor* krow, Atanor* kcol, Atanor* value) {
		long r = krow->Integer();
		long c = kcol->Integer();
		values[r][c] = value->Float();
	}
	void populate(Atanorfmatrix* ke) {
		ke->cleaning();
		ke->init(rowsize, columnsize);
		for (long r = 0; r < rowsize; r++) {
			if (values.check(r)) {
				for (long c = 0; c < columnsize; c++) {
					if (values[r].check(c))
						ke->values[r][c] = values[r][c];
				}
			}
		}
	}

	double val(long r, long c) {
		if (values.check(r) && values[r].check(c))
			return values[r][c];
		return 0;
	}

	int LUDCMP(int n, VECTE<int>& INDX, int& d, Atanorfmatrix& m) {

		d = 1;
		double AMAX, DUM, SUM;
		int  I, IMAX, J, K;
		vector<double> VV;

		for (I = 0; I < n; I++)  {
			AMAX = 0.0;
			for (J = 0; J<n; J++)  {
				SUM = m.val(I, J);
				if (ABS(SUM) > AMAX)
					AMAX = ABS(SUM);
			}

			if (AMAX < TINY)
				return 1;
			VV.push_back(1.0 / AMAX);
		} // i loop 

		for (J = 0; J < n; J++)  {

			for (I = 0; I < J; I++)  {
				SUM = m.val(I, J);
				for (K = 0; K < I; K++)
					SUM = SUM - m.val(I, K)*m.val(K, J);
				m.values[I][J] = SUM;
			} // i loop 
			AMAX = 0.0;

			for (I = J; I < n; I++)  {
				SUM = m.val(I, J);
				for (K = 0; K < J; K++)
					SUM = SUM - m.val(I, K)*m.val(K, J);
				m.values[I][J] = SUM;
				DUM = VV[I] * ABS(SUM);
				if (DUM >= AMAX) {
					IMAX = I;
					AMAX = DUM;
				}
			} // i loop

			if (J != IMAX)  {
				for (K = 0; K < n; K++)  {
					DUM = m.val(IMAX, K);
					m.values[IMAX][K] = m.val(J, K);
					m.values[J][K] = DUM;
				} // k loop 
				d = -d;
				VV[IMAX] = VV[J];
			}

			INDX.at(J, IMAX);

			if (ABS(m.val(J, J)) < TINY)
				m.values[J][J] = TINY;

			if (J != n - 1)  {
				DUM = 1.0 / m.val(J, J);
				for (I = J + 1; I < n; I++)
					m.values[I][J] *= DUM;
			}
		} // j loop 

		return 0;

	} // subroutine LUDCMP 

	void LUBKSB(int n, VECTE<int>& INDX, VECTE<double>& B, Atanorfmatrix& m)  {
		double SUM;
		int  I, II, J, LL;

		II = -1;

		for (I = 0; I < n; I++)  {
			LL = INDX[I];
			SUM = B[LL];
			B.at(LL, B[I]);
			if (II != -1)
			for (J = II; J < I; J++)
				SUM = SUM - m.val(I, J)*B[J];
			else
			if (SUM != 0.0)
				II = I;
			B.at(I, SUM);
		} // i loop

		for (I = n - 1; I >= 0; I--)  {
			SUM = B[I];
			if (I < n - 1)  {
				for (J = I + 1; J < n; J++)
					SUM = SUM - m.val(I, J)*B[J];
			}
			B.at(I, SUM / m.val(I, I));
		} // i loop 

	} // LUBKSB    

	double determinant(long rsize, long csize, KIFMATRIX<KIFLINEFLOAT>& m) {
		if (rsize == 2 && csize == 2) {
			//then in that case
			if (m.check(0) && m.check(1))
				return (m[0][0] * m[1][1] - m[1][0] * m[0][1]);

			return 0;
		}


		int i;
		for (i = 0; i < rsize; i++) {
			if (m.check(i) == false)
				return 0;
		}

		i = 0;
		double det = 0;
		for (int j = 0; j < rsize; j++) {
			if (!m[i].check(j) || m[i][j] == 0)
				continue;

			KIFMATRIX<KIFLINEFLOAT> sub(rsize - 1);

			int pc = 0;
			int pr = 0;
			for (int r = 0; r < rsize; r++) {
				if (r == i)
					continue;
				pc = 0;
				for (int c = 0; c < csize; c++) {
					if (c == j)
						continue;
					sub[pr][pc] = m[r][c];
					pc++;
				}
				pr++;
			}
			double sg = pow(-1, (i + j + 2));
			det += m[i][j] * sg*determinant(rsize - 1, csize - 1, sub);
		}
		return det;
	}

	bool inversion(Atanorfmatrix& Y) {
		//else Local decomposition

		Y.init(rowsize, columnsize);

		Atanorfmatrix m(NULL, NULL);
		populate(&m);

		VECTE<int> INDX(rowsize);
		int id;
		//call LU decomposition, on the matrixbyrow values
		int rc = LUDCMP(rowsize, INDX, id, m);
		if (rc == 1)
			return false;


		VECTE<double> temp(rowsize);
		int i;
		//We create an identity matrix, which will contain the final result... 
		for (i = 0; i < rowsize; i++)
			Y.values[i][i] = 1;

		for (int j = 0; j < rowsize; j++) {
			for (i = 0; i < rowsize; i++)
				temp.at(i, Y.val(i, j));
			LUBKSB(rowsize, INDX, temp, m);
			for (i = 0; i < rowsize; i++) {
				Y.values[i][j] = temp[i];
			}
		}

		return true;
	}

	Atanorfmatrix* duplicate() {
		Atanorfmatrix* ke = new Atanorfmatrix;

		for (long r = 0; r < rowsize; r++) {
			if (values.check(r)) {
				for (long c = 0; c < columnsize; c++) {
					if (values[r].check(c))
						ke->values[r][c] = values[r][c];
				}
			}
		}

		ke->rowsize = rowsize;
		ke->columnsize = columnsize;
		return ke;
	}

	string String() {
		Locking _lock(this);
		stringstream buff;

		for (int r = 0; r < rowsize; r++) {
			for (int c = 0; c < columnsize; c++) {
				if (c)
					buff << ",";
				if (values.check(r) && values[r].check(c))
					buff << values[r][c];
				else
					buff << "0";
			}
			buff << endl;
		}

		return buff.str();
	}

	long Size() {
		return (rowsize*columnsize);
	}


	long Integer() { return Size(); }
	double Float() { return Size(); }
	bool Boolean() { return true; }

	Atanor* same(Atanor* a) {
		if (a->Type() != idtype)
			return aFALSE;

		Locking _lock(this);
		Atanorfmatrix* ma = this;
		if (ma->rowsize != rowsize && ma->columnsize != columnsize)
			return aFALSE;

		for (long r = 0; r < rowsize; r++) {
			if (ma->values.check(r)) {
				if (!values.check(r))
					return aFALSE;
				for (long c = 0; c < columnsize; c++) {
					if (ma->values[r].check(c)) {
						if (!values[r].check(c))
							return aFALSE;

						if (ma->values[r][c] != values[r][c])
							return aFALSE;
					}
					else {
						if (values[r].check(c))
							return aFALSE;
					}
				}
			}
			else
			if (values.check(r))
				return aFALSE;
		}
		return aTRUE;
	}

	Atanor* orset(Atanor* b, bool autoself) {
		if (b->Type() != idtype)
			return this;

		Locking _lock(this);
		Atanorfmatrix* ma = this;
		Atanorfmatrix* mb = (Atanorfmatrix*)b;
		Atanorfmatrix* res = new Atanorfmatrix;

		long v;
		for (long r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r) && mb->values.check(r)) {
				for (long c = 0; c < ma->columnsize; c++) {
					if (ma->values[r].check(c) && mb->values[r].check(c)) {
						v = (long)ma->values[r][c] | (long)mb->values[r][c];
						res->populate(r, c, (double)v);
					}
				}
			}
		}

		return res;
	}

	//intersection of two sets
	Atanor* andset(Atanor* b, bool autoself) {
		if (b->Type() != idtype)
			return this;

		Locking _lock(this);
		Atanorfmatrix* ma = this;
		Atanorfmatrix* mb = (Atanorfmatrix*)b;
		Atanorfmatrix* res = new Atanorfmatrix;

		long v;
		for (long r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r) && mb->values.check(r)) {
				for (long c = 0; c < ma->columnsize; c++) {
					if (ma->values[r].check(c) && mb->values[r].check(c)) {
						v = (long)ma->values[r][c] & (long)mb->values[r][c];
						res->populate(r, c, (double)v);
					}
				}
			}
		}
		return res;
	}

	Atanor* xorset(Atanor* b, bool autoself) {
		if (b->Type() != idtype)
			return this;

		Locking _lock(this);
		Atanorfmatrix* ma = this;
		Atanorfmatrix* mb = (Atanorfmatrix*)b;
		Atanorfmatrix* res = new Atanorfmatrix;

		long v;
		for (long r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r) && mb->values.check(r)) {
				for (long c = 0; c < ma->columnsize; c++) {
					if (ma->values[r].check(c) && mb->values[r].check(c)) {
						v = (long)ma->values[r][c] ^ (long)mb->values[r][c];
						res->populate(r, c, (double)v);
					}
				}
			}
		}

		return res;
	}

	//Beware that a might be the current element
	//plus: a+b
	Atanor* plus(Atanor* b, bool autoself) {

		if (b->Type() != idtype)
			return this;

		Locking _lock(this);
		Atanorfmatrix* ma = this;
		Atanorfmatrix* mb = (Atanorfmatrix*)b;
		Atanorfmatrix* res = new Atanorfmatrix;
		double v;
		if (b->Type() != idtype) {
			double vb = b->Float();
			for (long r = 0; r < ma->rowsize; r++) {
				if (ma->values.check(r)) {
					for (long c = 0; c < ma->columnsize; c++) {
						if (ma->values[r].check(c)) {
							v = ma->values[r][c] + vb;
							res->populate(r, c, v);
						}
					}
				}
			}
			return res;
		}

		for (long r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r) && mb->values.check(r)) {
				for (long c = 0; c < ma->columnsize; c++) {
					if (ma->values[r].check(c) && mb->values[r].check(c)) {
						v = ma->values[r][c] + mb->values[r][c];
						res->populate(r, c, v);
					}
				}
			}
		}

		return res;
	}

	//minus: a-b
	Atanor* minus(Atanor* b, bool autoself) {
		if (b->Type() != idtype)
			return this;

		Locking _lock(this);
		Atanorfmatrix* ma = this;
		Atanorfmatrix* mb = (Atanorfmatrix*)b;
		Atanorfmatrix* res = new Atanorfmatrix;

		double v;
		if (b->Type() != idtype) {
			double vb = b->Float();
			for (long r = 0; r < ma->rowsize; r++) {
				if (ma->values.check(r)) {
					for (long c = 0; c < ma->columnsize; c++) {
						if (ma->values[r].check(c)) {
							v = ma->values[r][c] - vb;
							res->populate(r, c, v);
						}
					}
				}
			}
			return res;
		}


		for (long r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r) && mb->values.check(r)) {
				for (long c = 0; c < ma->columnsize; c++) {
					if (ma->values[r].check(c) && mb->values[r].check(c)) {
						v = ma->values[r][c] - mb->values[r][c];
						res->populate(r, c, v);
					}
				}
			}
		}
		return res;
	}

	Atanor* multiply(Atanor* b, bool autoself) {
		if (b->Type() != idtype)
			return this;

		Locking _lock(this);
		Atanorfmatrix* ma = this;
		Atanorfmatrix* res = new Atanorfmatrix;

		double v;
		long r, c, k;

		if (b->Type() != idtype) {
			double vb = b->Float();
			for (r = 0; r < ma->rowsize; r++) {
				if (ma->values.check(r)) {
					for (c = 0; c < ma->columnsize; c++) {
						if (ma->values[r].check(c)) {
							v = ma->values[r][c] * vb;
							res->populate(r, c, v);
						}
					}
				}
			}
			return res;
		}

		Atanorfmatrix* mb = (Atanorfmatrix*)b;
		if (ma->columnsize != mb->rowsize)
			return new AtanorError("MAT(209): Matrix size mismatch");

		res->init(ma->rowsize, mb->columnsize);
		for (r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r)) {
				for (c = 0; c < mb->columnsize; c++) {
					v = 0;
					for (k = 0; k < ma->columnsize; k++) {
						if (ma->values[r].check(k) && mb->values.check(k) && mb->values[k].check(c))
							v += ma->values[r][k] * mb->values[k][c];
					}
					res->populate(r, c, v);
				}
			}
		}

		return res;
	}


	//Division: a/b
	Atanor* divide(Atanor* b, bool autoself) {
		if (b->Type() != idtype)
			return this;

		Locking _lock(this);
		Atanorfmatrix* ma = this;
		Atanorfmatrix* res = new Atanorfmatrix;

		double v;
		long r;
		long c;

		if (b->Type() != idtype) {
			double vb = b->Float();
			if (vb == 0)
				return new AtanorError("MAT(203): Cannot divide by 0");

			for (r = 0; r < ma->rowsize; r++) {
				if (ma->values.check(r)) {
					for (c = 0; c < ma->columnsize; c++) {
						if (ma->values[r].check(c)) {
							v = ma->values[r][c] / vb;
							res->populate(r, c, v);
						}
					}
				}
			}
			return res;
		}

		Atanorfmatrix* mb = (Atanorfmatrix*)b;

		Atanorfmatrix inverted(NULL, NULL);

		if (!mb->inversion(inverted))
			return new AtanorError("MAT(202): Cannot divide with this matrix");

		if (ma->columnsize != inverted.rowsize)
			return new AtanorError("MAT(202): Cannot divide with this matrix");

		long k;
		for (r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r)) {
				for (c = 0; c < inverted.columnsize; c++) {
					v = 0;
					for (k = 0; k < ma->columnsize; k++) {
						if (ma->values[r].check(k) && inverted.values.check(k) && inverted.values[k].check(c))
							v += ma->values[r][k] * inverted.values[k][c];
					}
					res->populate(r, c, v);
				}
			}
		}

		res->rowsize = ma->rowsize;
		res->columnsize = inverted.columnsize;
		return res;

	}

};

class AtanorIterationfmatrix : public AtanorIteration {
public:
	Atanorfmatrix* value;
	long r;
	long c;
	long mr, mc;
	Locking _lock;

	AtanorIterationfmatrix(Atanorfmatrix* v, bool d, AtanorGlobal* g = NULL) : _lock(v), AtanorIteration(d, g) {
		value = v;
	}

	Atanor* Key() {
		char buff[100];
		sprintf_s(buff, 100, "%d:%d", r, c);
		return new Atanorstring(buff);
	}

	Atanor* Value() {
		double v = 0;
		if (value->values.check(r) && value->values[r].check(c))
			v = value->values[r][c];

		return new Atanorfloat(v);
	}

	string Valuestring() {
		double v = 0;
		if (value->values.check(r) && value->values[r].check(c))
			v = value->values[r][c];
		stringstream s;
		s << v;
		return s.str();
	}

	long Valueinteger() {
		double v = 0;
		if (value->values.check(r) && value->values[r].check(c))
			v = value->values[r][c];
		return (long)v;
	}

	double Valuefloat() {
		double v = 0;
		if (value->values.check(r) && value->values[r].check(c))
			v = value->values[r][c];
		return v;
	}

	void Next() {
		r++;
		if (r == mr) {
			mc++;
			r = 0;
		}
	}

	Atanor* End() {
		if (r == mr && c == mc)
			return aTRUE;
		return aFALSE;
	}

	Atanor* Begin() {
		r = 0;
		c = 0;
		mr = value->rowsize;
		mc = value->columnsize;

		if (!mr && !mc)
			return aFALSE;

		return aTRUE;
	}

};

#endif




