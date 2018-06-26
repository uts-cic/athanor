
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
filename   : compilecode.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#ifndef compile_h
#define compile_h

class AtanorActionVariable;

class AtanorFunctionLambda;
//AtanorCode is a class in which programs are loaded...
class AtanorCode {
private:


	//this is where we record functions, variables and instructions
	AtanorMainFrame mainframe;
	AtanorCode* loader;

	size_t currentline;
	size_t current_start, current_end;


public:
	//This is a pointer to the global object with which this code space is linked...
	AtanorGlobal* global;
	basebin_hash<AtanorActionVariable*> variables;
	uchar featureassignment;

	string filename;

	short currentfileid;
	size_t firstinstruction;
	short idcode;

	bool isprivate;
	bool iscommon;
	bool isconstant;

	bool compilemode;

	Atanor* EvaluateVariable(Atanor* var);

	AtanorCode(short i, string& f, AtanorGlobal* g) : idcode(i), filename(f), currentline(0), mainframe(a_mainframe, false, NULL) {
		compilemode = true;
		g->filenames.push_back(f);
		g->frames[a_mainframe] = &mainframe;
		currentfileid = 0;
		loader = NULL;
		isprivate = false;
		iscommon = false;
		isconstant = false;
		firstinstruction = 0;
		global = g;
		current_start = 0;
		current_end = 0;
		featureassignment = 0;
	}

	~AtanorCode() {
		mainframe.cleaning();
	}

	short SetFilename(string f) {
		f = NormalizeFileName(f);

		currentfileid = (short)global->filenames.size();
		global->filenames.push_back(f);
		filename = f;
		return currentfileid;
	}

	Atanor* Mainframe() {
		return &mainframe;
	}

	long InstructionSize() {
		return mainframe.instructions.size();
	}

	long Computecurrentline(int i, x_node* xn);

	long Getcurrentline() {
		return (long)currentline;
	}

	short Getfileid(string f) {
		for (short i = 0; i < global->filenames.size(); i++) {
			if (global->filenames[i] == f)
				return i;
		}
		return -1;
	}


	bool Load(x_reading& xr);
	bool Compile(string& code);
	Atanor* Compilefunction(string& code);

	bool Loadlibrary(string n, string& library_name);
	//------------------------------
	Atanor* Run();
	Atanor* Execute(long i, short idthread);
	Atanor* Loading();
	//---------------------------------------------
	Atanor* Declaror(short id);
	Atanor* Declaror(short id, Atanor* parent);

	Atanor* Declaration(short id);
	Atanor* Declaration(short id, Atanor* parent);
    Atanor* Frame();
	bool isDeclared(short id);
	//---------------------------------------------
	//This method traverses a x_node tree to create the adequate elements...
	Atanor* Traverse(x_node* xn, Atanor* parent);
	void BrowseVariable(x_node* xn, Atanor* kf);
	void BrowseVariableVector(x_node* xn, Atanor* kf);
	void BrowseVariableMap(x_node* xn, Atanor* kf);
	void DeclareVariable(x_node* xn, Atanor* kf);

	//---------------------------------------
	AtanorInstruction* AtanorCreateInstruction(Atanor* kf, short op);
	AtanorInstruction* CloningInstruction(AtanorInstruction* ki);

	Atanor* Callingprocedure(x_node*, short id);
	//----------------------------------------
	//The methods which need to be implemented to compile a code
	Atanor* C_parameterdeclaration(x_node*, Atanor*);
	Atanor* C_multideclaration(x_node*, Atanor*);
	Atanor* C_subfunc(x_node*, Atanor*);
	Atanor* C_variable(x_node*, Atanor*);
	bool isaFunction(string& name, short id);
	Atanor* C_regularcall(x_node*, Atanor*);
	Atanor* C_haskellcall(x_node*, Atanor*);
	Atanor* C_indexes(x_node*, Atanor*);
	Atanor* C_interval(x_node*, Atanor*);
	Atanor* C_astring(x_node*, Atanor*);
	Atanor* C_ustring(x_node*, Atanor*);
	Atanor* C_anumber(x_node*, Atanor*);
	Atanor* C_axnumber(x_node*, Atanor*);
	Atanor* C_frame(x_node* xn, Atanor* kf);
	Atanor* C_extension(x_node* xn, Atanor* kf);
	Atanor* C_affectation(x_node* xn, Atanor* kf);
	Atanor* C_operator(x_node* xn, Atanor* kf);
	Atanor* C_increment(x_node* xn, Atanor* kf);
	Atanor* C_valvector(x_node* xn, Atanor* kf);
	Atanor* C_jsonvector(x_node* xn, Atanor* kf);

	Atanor* C_dependencyrule(x_node* xn, Atanor* kf);
	Atanor* C_dependency(x_node* xn, Atanor* kf);
	Atanor* C_dependencyresult(x_node* xn, Atanor* kf);

	Atanor* C_intentionvector(x_node* xn, Atanor* kf);
	Atanor* C_intentionwithdouble(x_node* xn, Atanor* kf);

	Atanor* C_valmap(x_node* xn, Atanor* kf);

	Atanor* C_conceptfunction(x_node* xn, Atanor* kf);

	Atanor* C_jsonmap(x_node* xn, Atanor* kf);
	Atanor* C_dico(x_node* xn, Atanor* kf);
	Atanor* C_jsondico(x_node* xn, Atanor* kf);
	Atanor* C_features(x_node* xn, Atanor* kf);

	Atanor* C_parenthetic(x_node* xn, Atanor* kf);
	Atanor* C_operation(x_node* xn, Atanor* kf);
	Atanor* C_multiply(x_node* xn, Atanor* kf);
	Atanor* C_plusplus(x_node* xn, Atanor* kf);
	Atanor* C_operationin(x_node* xn, Atanor* kf);
	Atanor* C_comparison(x_node* xn, Atanor* kf);
	Atanor* C_constant(x_node* xn, Atanor* kf);
	Atanor* C_negated(x_node* xn, Atanor* kf);
	Atanor* C_uniquecall(x_node* xn, Atanor* kf);
	Atanor* C_createfunction(x_node* xn, Atanor* kf);
	Atanor* C_blocs(x_node* xn, Atanor* kf);
	bool CheckUse(x_node* xn, Atanor* kf);

	Atanor* C_ifcondition(x_node* xn, Atanor* kf);

	Atanor* C_negation(x_node* xn, Atanor* kf);

	Atanor* C_booleanexpression(x_node* xn, Atanor* kf);
	
	Atanor* C_switch(x_node* xn, Atanor* kf);
	Atanor* C_testswitch(x_node* xn, Atanor* kf);

	Atanor* C_trycatch(x_node* xn, Atanor* kf);

	Atanor* C_loop(x_node* xn, Atanor* kf);
	Atanor* C_doloop(x_node* xn, Atanor* kf);

	Atanor* C_for(x_node* xn, Atanor* kf);
	Atanor* C_blocfor(x_node* xn, Atanor* kf);
	Atanor* C_forin(x_node* xn, Atanor* kf);
	Atanor* C_parameters(x_node* xn, Atanor* kf);

	//Haskell

	Atanor* C_hdata(x_node* xn, Atanor* kf);
	Atanor* C_hdeclaration(x_node* xn, Atanor* kf);
	Atanor* C_telque(x_node* xn, Atanor* kf);
	Atanor* C_ontology(x_node* xn, Atanor* kf);

	Atanor* C_hbloc(x_node* xn, Atanor* kf);
	Atanor* C_ParseReturnValue(x_node* xn, AtanorFunctionLambda* kf, char adding = false);
	bool CheckVariable(x_node* xn, Atanor* kf);
	bool CheckingVariableName(x_node* xn, Atanor* kf);

	Atanor* C_declarationhaskell(x_node* xn, Atanor* kf);
	Atanor* C_hlambda(x_node* xn, Atanor* kf);
	Atanor* C_hcompose(x_node* xn, Atanor* kf);
	Atanor* C_hfunctioncall(x_node* xn, Atanor* kf);
	Atanor* C_haskellbooling(x_node* xn, Atanor* kf);
	Atanor* C_folding(x_node* xn, Atanor* kf);
	Atanor* C_zipping(x_node* xn, Atanor* kf);
	Atanor* C_filtering(x_node* xn, Atanor* kf);
	Atanor* C_mapping(x_node* xn, Atanor* kf);
	Atanor* C_flipping(x_node* xn, Atanor* kf);
	Atanor* C_cycling(x_node* xn, Atanor* kf);
	Atanor* C_haskellexpression(x_node* xn, Atanor* kf);
	Atanor* C_haskellmap(x_node* xn, Atanor* kf);
	Atanor* C_haskellvector(x_node* xn, Atanor* kf);
	Atanor* C_whereexpression(x_node* xn, Atanor* kf);
	Atanor* C_hinexpression(x_node* xn, Atanor* kf);
	Atanor* C_letmin(x_node* xn, Atanor* kf);
	Atanor* C_haskellcase(x_node* xn, Atanor* kf);
	Atanor* C_guard(x_node* xn, Atanor* kf);
	Atanor* C_dataassignment(x_node* xn, Atanor* kf);

	Atanor* C_alist(x_node* xn, Atanor* kf);
	Atanor* C_cut(x_node* xn, Atanor* kf);
	Atanor* C_dcg(x_node* xn, Atanor* kf);
	Atanor* C_predicatefact(x_node* xn, Atanor* kf);
	Atanor* C_predicate(x_node* xn, Atanor* kf);
	Atanor* C_predicateexpression(x_node* xn, Atanor* kf);
	Atanor* C_predicatevariable(x_node* xn, Atanor* kf);
	Atanor* C_assertpredicate(x_node* xn, Atanor* kf);
	Atanor* C_term(x_node* xn, Atanor* kf);
	Atanor* C_comparepredicate(x_node* xn, Atanor* kf);

	Atanor* AtanorParsePredicateDCGVariable(string& name, Atanor* kcf, bool param);
	void ComputePredicateParameters(x_node* xn, Atanor* kcf);

};

#endif






