
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
filename   : matrix.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX (claude.roux@xrce.xerox.com)
Reviewer   :
*/

#ifndef matrix_h
#define matrix_h


class KIFLINEINTEGER {
public:
	long* line;
	int size;

	KIFLINEINTEGER() {
		size = 0;
		line = NULL;
	}

	~KIFLINEINTEGER() {
		if (line != NULL)
			delete[] line;
	}

	void clear(int nb = 0) {
		if (line != NULL) {
			delete[] line;
			size = 0;
			line = NULL;
		}
	}

	void release() {
		line = NULL;
		size = 0;
	}

	void update(int r) {
		if (r >= size) {
			long* ln = new long[r + 1];
			memset(ln, 0, (r + 1)*sizeof(long));
			if (line != NULL) {
				memcpy(ln, line, size*sizeof(long));
				delete[] line;
			}
			size = r + 1;
			line = ln;
		}
	}

	inline long& operator [](int r) {
		if (r >= size) {
			long* ln = new long[r + 1];
			memset(ln, 0, (r + 1)*sizeof(long));
			if (line != NULL) {
				memcpy(ln, line, size*sizeof(long));
				delete[] line;
			}
			size = r + 1;
			line = ln;
		}
		return line[r];
	}

	void init(int r) {
		if (line == NULL) {
			size = r;
			line = new long[r];
			memset(line, 0, size*sizeof(long));
		}
	}

	bool check(int r) {
		if (r < 0 || r >= size || line == NULL)
			return false;
		return true;
	}
};

class KIFLINEFLOAT {
public:
	double* line;
	int size;

	KIFLINEFLOAT() {
		size = 0;
		line = NULL;
	}

	~KIFLINEFLOAT() {
		if (line != NULL)
			delete[] line;
	}

	void clear(int nb = 0) {
		if (line != NULL) {
			delete[] line;
			size = 0;
			line = NULL;
		}
	}

	void release() {
		line = NULL;
		size = 0;
	}

	void update(int r) {
		if (r >= size) {
			double* ln = new double[r + 1];
			memset(ln, 0, (r + 1)*sizeof(double));
			if (line != NULL) {
				memcpy(ln, line, size*sizeof(double));
				delete[] line;
			}
			size = r + 1;
			line = ln;
		}
	}

	inline double& operator [](int r) {
		if (r >= size) {
			double* ln = new double[r + 1];
			memset(ln, 0, (r + 1)*sizeof(double));
			if (line != NULL) {
				memcpy(ln, line, size*sizeof(double));
				delete[] line;
			}
			size = r + 1;
			line = ln;
		}
		return line[r];
	}

	void init(int r) {
		if (line == NULL) {
			size = r;
			line = new double[r];
			memset(line, 0, size*sizeof(double));
		}
	}

	bool check(int r) {
		if (r < 0 || r >= size || line == NULL)
			return false;
		return true;
	}
};

template<class Z> class KIFMATRIX {
public:
	Z* matrix;
	int size;

	KIFMATRIX() {
		matrix = NULL;
		size = 0;
	}

	KIFMATRIX(int r) {
		matrix = NULL;
		size = 0;
		init(r);
	}

	void clear(int nb = 0) {
		if (matrix != NULL) {
			delete[] matrix;
			matrix = NULL;
			size = 0;
		}
	}

	~KIFMATRIX() {
		clear();
	}

	void update(int r, int c) {
		if (r >= size) {
			Z* mat = new Z[r + 1];
			memset(mat, NULL, (r + 1)*sizeof(Z));
			if (matrix != NULL) {
				for (int i = 0; i < size; i++) {
					mat[i].line = matrix[i].line;
					mat[i].size = matrix[i].size;
					matrix[i].release();
				}
				delete[] matrix;
			}
			matrix = mat;
			size = r + 1;
		}

		for (int i = 0; i<size; i++)
			matrix[i].update(c);
	}

	void init(int r) {
		if (matrix == NULL && r > 0) {
			size = r;
			matrix = new Z[size];
		}
	}

	inline Z& operator[](int r) {
		if (r >= size) {
			Z* mat = new Z[r + 1];
			memset(mat, NULL, (r + 1)*sizeof(Z));
			if (matrix != NULL) {
				for (int i = 0; i < size; i++) {
					mat[i].line = matrix[i].line;
					mat[i].size = matrix[i].size;
					matrix[i].release();
				}
				delete[] matrix;
			}
			matrix = mat;
			size = r + 1;
		}
		return matrix[r];
	}

	bool check(int r) {
		if (r < 0 || r >= size || matrix == NULL || matrix[r].line == NULL)
			return false;
		return true;
	}
};


#endif


