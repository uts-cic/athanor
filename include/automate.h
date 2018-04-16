/*
* Xerox Research Centre Europe - Grenoble Laboratory
*
* Copyright (C) 2003 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
* This file can only be used with the ATHANOR library,
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : ATHANOR
Version    : 4.10
filename   : automate.h
Date       : 2017/04/01
Purpose    : Automaton implementation to handle lexicons
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef automate_h
#define automate_h

#include "vecta.h"

class arc;
class etat;
class automate;



#define AUTOMATONSTART 1
#define AUTOMATONEND   2
#define AUTOMATONNODE 4
#define AUTOMATEPILE 2

#define NON 0
#define OUI 1
#define TailleChaine 512

typedef enum {
	ARCINITIAL,
	ARCSIMPLE,
	ARCCOMPLEXE,
	ARCBOUCLE,
	ARCCROCHET,
	ARCVIRGULE,
	ARCFINCROPAR,
	ARCPARENTHESE,
	ARCCODE,
	ARCANY
} typeautoparse;

typedef enum { AUTOMATONARC, AUTOMATONARCARACTERE, AUTOMATONARCCHAINE } typearc;


typedef enum {
	AUTOMATONIF, AUTOMATONELSE, AUTOMATONEPSILON, AUTOMATONANY, AUTOMATONDIGIT, AUTOMATONPUNCT,
	AUTOMATONLETTER,
	AUTOMATONLETTERLOWER,
	AUTOMATONLETTERUPPER,
	AUTOMATONHANGUL, 
	AUTOMATONHEXA,
	AUTOMATONHEXAUPPER,
	AUTOMATONLABEL,
	AUTOMATONVARIABLE,
	AUTOMATONSTAR = '*',
	AUTOMATONPLUS = '+'
} arcautomate;

typedef enum {
	AUTOMATONMARKEDFORDELETION,
	AUTOMATONDELETED,
	AUTOMATONNONE
} arcmanip;

arcautomate TestId(char* m);
char chainecomplexe(unsigned char* chaine, int position, typeautoparse state);
char typeautomate(unsigned char* chaine, int position);

typedef enum {
	AERR_NO_ERROR,
	AERR_MISSING_PARENTHESIS,
	AERR_MISSING_BRACKET,
	AERR_WRONG_NEGATION_POSITION,
	AERR_WRONG_USE_OF_COMMA,
	AERR_WRONG_KLEENE,
	AERR_MISMATCHED_PARENTHESIS,
	AERR_MISMATCHED_BRACKET,
	AERR_WRONG_VARIABLE_ORDER,
	AERR_WRONG_CODE
} autoerreur;


class etatarc {
public:
	etat* e;
	arc* ar;

	etatarc(etat* ee, arc* aar) {
		e = ee;
		ar = aar;
	}
};

class etat {
public:
	char status;
	char marque;
	int nombre;
	void* valeur;
	size_t id;
	VECTA<arc*> arcs;


	char start() {
		if ((status& AUTOMATONSTART) == AUTOMATONSTART)
			return 1;
		return 0;
	}

	char end() {
		if ((status & AUTOMATONEND) == AUTOMATONEND)
			return 1;
		return 0;
	}

	char node() {
		if ((status & AUTOMATONNODE) == AUTOMATONNODE)
			return 1;
		return 0;
	}


	etat(automate* a, char s = AUTOMATONSTART);
	etat();

	void aupluspres();
	etat* parse(automate*,
		unsigned char* chaine,
		int position,
		int& finchaine,
		unsigned char fin,
		void* value, etat*
		debut, arc*,
		etat*,
		typeautoparse state,
		char negation);

	etat* parsebrute(automate* a,
		unsigned char* chaine,
		int position,
		void* value,
		typeautoparse state);
	etat* parsebrutelin(automate* a, unsigned char* chaine, void* value);
	arc* trouvearc(unsigned char l, arcautomate, char, char);
	arc* trouvearc(arc* a);
	void ajoutearc(arc* a);
	void insertionarc(arc* a, char aupluslong);
	void compose(etat* a, VECTA<void*>& res);
	void recherchetous(unsigned char* mot, VECTA<void*>& res, int i, int max);
	etat* recherche(unsigned char* mot, automate*, int i, int max);
	etat* recherchelin(unsigned char* mot, automate* ref, int i);
	etat* recherchepartielle(unsigned char* mot, automate*, int i);
	etat* recherchemax(unsigned char* mot, automate*, int i);
	int rechercherecursive(unsigned char* mot, automate* ref, int i, etat**);
	void print(int p, ostream* os);
	void chaine(char*);
	void ArcEpsilon(VECTA<etatarc*>& eas);
	int CalculChemin(int max);
	void fusion(VECTA<arc*>&);
	void dedoublonne(VECTA<arc*>&, VECTA<int>&);
	void marqueduplicat();
	void recherchetouslin(automate*, unsigned char* mot, VECTA<void*>& res, int i);
	char retirebrute(unsigned char* mot, automate* ref, int i);

};


class automate {
public:
	size_t dernier;
	VECTA<etat*> lesetats;
	VECTA<arc*> lesarcs;

	autoerreur erreur;
	int lieuerreur;
	int maxpos;

	VECTA<int> pilearcs;
	VECTA<int> pileposition;
	VECTA<etat*> pileetats;
	char* automate_buffer;
	int tailledebuf;
	char aupluslong;

	etat e;

	Exporting automate();

	Exporting automate(unsigned char* m, void* ind);

	Exporting ~automate();

	void retirefordeleted(VECTA<arc*>& adetruire);
	virtual void affecte(string chaines[10]) {}
	virtual void extraitvariables(unsigned char* ch, string chaines[10]) {}
	virtual void extraitvariables(unsigned char* ch, automate*) {}
	virtual void extraction(unsigned char* ch) {}
	virtual void mot(char*, int);
	virtual void mot(string&);
	void aupluspres();
	virtual void vectorisation() {}
	void chaine(char*);
	int compare(automate* a);
	void duplique(automate* a);
	etat* parse(unsigned char* mot, void* valeur, char ordre = 1);
	etat* parsebrute(unsigned char* mot, void* valeur);
	etat* parsebrutelin(unsigned char* mot, void* valeur);
	virtual void recherchetouslabels(unsigned char* mot, VECTA<void*>& res);
	etat* recherche(unsigned char* mot);
	etat* recherche(string& mot);
	virtual void recherchetouslabels(char* mot, VECTA<void*>& res);
	etat* recherchepartielle(unsigned char*);
	etat* recherchemax(unsigned char*);
	etat* recherche(char* mot);
	void compose(automate& a, VECTA<void*>& res);
	void demarque();
	void print(ostream*);
	char retireEpsilon(VECTA<arc*>&);
	void initErreur(int, autoerreur);
	virtual char Type() { return NON; }
	virtual void initvar() {}
	virtual void majvar(arc*, int) {}
	virtual char construitvar(unsigned char lab, int) { return 1; }
	virtual void testordre(char ordre) {}
	void fusion(VECTA<arc*>&);
	void nettoie();
	int rechercheavecextraction(unsigned char* m, string&);
	int rechercheavecextractionparlafin(unsigned char* m, string& res);
	Exporting int rechercheavecextractiondansvecteur(unsigned char* m, VECTA<string*>& vect);
	Exporting int rechercheavecextractiondansvecteur(unsigned char* m, vector<string>& vect);
	int rechercheindexes(unsigned char* m, vector<int>& vect);
	int rechercheindexes(unsigned char* m, vector<long>& vect);
	int rechercheindexes(unsigned char* m, vector<double>& vect);
	int rechercheposition(unsigned char* m, int&);
	char retirebrute(unsigned char* mot);
	int recherchejusqualafin(unsigned char* m, string& res);
};


class automatevecteur : public automate {
public:
	arc* vectorise[260];

	void vectorisation();
	void recherchetouslabels(unsigned char* mot, VECTA<void*>& res);
	void recherchetouslabels(char* mot, VECTA<void*>& res);
};


class automatepile : public automate {
public:

	int variables[10];
	int maxvar;

	automatepile() : automate() { raz(); };

	automatepile(unsigned char* m, void* ind) : automate(m, ind) { raz(); };

	string chaines[10];
	void affecte(string chaines[10]);
	void extraitvariables(unsigned char* ch, string chaines[10]);
	void extraitvariables(unsigned char* ch, automate*);
	void extraction(unsigned char* ch);
	void mot(char*, int mx);
	void mot(string&);
	void initvar();
	void majvar(arc*, int);
	char construitvar(unsigned char lab, int);
	void testordre(char ordre);
	void raz();
	char Type() { return AUTOMATEPILE; }
};


class arc {
public:

	unsigned char label;
	int id;
	char negation;
	char marque;

	arcautomate action;
	arcmanip manipulation;
	etat* e;


	arc(automate* a, arcautomate ac, char n, unsigned char l = 0);
	arc(automate* a, arc* c);

	char compare(unsigned char c) {
		return (c == label);
	}
	char test(unsigned char c);
	char atest(unsigned char* m, int& i);
	void chaine(char*);

	void aupluspres(int id);
	void recherchetous(unsigned char* mot, VECTA<void*>& res, int i, int max);
	etat* recherche(unsigned char* mot, automate*, int i, int max);
	void print(int p, int id, ostream*);
	void compose(arc* a, VECTA<void*>& res);
	void ArcEpsilon(VECTA<etatarc*>& eas);
	int CalculChemin(int max);
	void fusion(VECTA<arc*>&);
	void dedoublonne();
	char retirebrute(unsigned char* mot, automate* ref, int i);
	etat* recherchemax(unsigned char* mot, automate* ref, int i);
	int rechercherecursive(unsigned char* mot, automate* ref, int i, etat**);
};

#endif







