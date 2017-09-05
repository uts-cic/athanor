
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
filename   : codecompile.cxx
Date       : 2017/04/01
Purpose    :
Programmer : Claude ROUX
Reviewer   :
*/

//This section is to parse the syntactic tree of your program...

#include "codeparse.h"
#include "atanor.h"
#include "atanorhaskell.h"
#include "compilecode.h"
#include "instructions.h"
#include "constobjects.h"
#include "atanorframeinstance.h"
#include "atanorvector.h"
#include "atanormap.h"
#include "atanoratanor.h"
#include "predicate.h"
#include "atanorsynode.h"
#include "atanormapss.h"
#include "automate.h"
#include "atanorivector.h"
#include "atanorfvector.h"
#include "atanorlvector.h"
#include "atanorsvector.h"
#include "atanoruvector.h"
#include "atanorhvector.h"
#include "atanordvector.h"

//--------------------------------------------------------------------
Atanor* ProcCreateFrame(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
//--------------------------------------------------------------------

static x_node* creationxnode(string t, string v, x_node* parent = NULL) {
	x_node* n = new x_node;
	n->token = t;
	n->value = v;
	if (parent != NULL) {
		n->start = parent->start;
		n->end = parent->end;
		parent->nodes.push_back(n);
	}
	return n;
}

//--------------------------------------------------------------------
uchar Returnequ(short ty) {
	switch (ty) {
	case a_boolean:
	case a_byte:
	case a_short:
	case a_bloop:
		return b_short;
	case a_int:
	case a_iloop:
	case a_intthrough:
		return b_int;
	case a_long:
	case a_lloop:
	case a_longthrough:
		return b_long;
	case a_decimal:
	case a_dloop:
	case a_decimalthrough:
		return b_decimal;
	case a_fraction:
	case a_float:
	case a_floop:
	case a_floatthrough:
		return b_float;
	case a_string:
	case a_sloop:
	case a_stringthrough:
		return b_string;
	case a_ustring:
	case a_uloop:
	case a_ustringthrough:
		return b_ustring;
	}

	return 255;
}

Atanor* Evaluatetype(uchar thetype, uchar ref, Atanor* a) {
	uchar t = Returnequ(a->Type());
	Atanor* ret = NULL;
	if ((thetype & b_allnumbers)) {
		if (t & b_allstrings) {
			//return a value according to ref
			switch (ref) {
			case b_short:
				ret = new Atanorshort(a->Short());
				break;
			case b_int:
				ret = globalAtanor->Provideint(a->Integer());
				break;
			case b_long:
				ret = new Atanorshort(a->Long());
				break;
			case b_decimal:
				ret = new Atanorshort(a->Decimal());
				break;
			case b_float:
				ret = globalAtanor->Providefloat(a->Float());
				break;
			}
			if (ret != NULL) {
				a->Release();
				return ret;
			}
			return a;
		}
		if (t & b_allnumbers) {
			if (ref <= t)
				return a;

			switch (ref) {
			case b_int:
				ret = globalAtanor->Provideint(a->Integer());
				break;
			case b_long:
				ret = new Atanorshort(a->Long());
				break;
			case b_decimal:
				ret = new Atanorshort(a->Decimal());
				break;
			case b_float:
				ret = globalAtanor->Providefloat(a->Float());
				break;
			}
			if (ret != NULL) {
				a->Release();
				return ret;
			}
		}
		return a;
	}

	if ((thetype & b_allstrings) && (t & b_allnumbers)) {
		if (thetype == a_ustring) {
			ret = globalAtanor->Provideustring(a->UString());
			a->Release();
			return ret;
		}
		ret = globalAtanor->Providestring(a->String());
		a->Release();
		return ret;
	}
	return a;
}
//--------------------------------------------------------------------
//The main function, which is used to traverse the parse tree and creates each instruction and declaration...
Atanor* AtanorCode::Traverse(x_node* xn, Atanor* parent) {
	if (xn == NULL)
		return NULL;

	currentline = Computecurrentline(0, xn);
	if (globalAtanor->parseFunctions.find(xn->token) != globalAtanor->parseFunctions.end())
		return (this->*globalAtanor->parseFunctions[xn->token])(xn, parent);

	Atanor* a;
	Atanor* res = NULL;
	for (size_t i = 0; i < xn->nodes.size(); i++) {
		a = Traverse(xn->nodes[i], parent);
		if (res == NULL)
			res = a;
	}

	return res;
}

void AtanorCode::BrowseVariable(x_node* xn, Atanor* kf) {
	if (xn == NULL)
		return;

	if (xn->token == "variable") {
		short id = globalAtanor->Getid(xn->nodes[0]->value);
		if (!kf->isDeclared(id) || kf->Declaration(id) == aNOELEMENT) {
			Atanor* var = new AtanorSelfDeclarationVariable(global, id, a_self, kf);
			kf->Declare(id, var);
		}
		return;
	}

	for (size_t i = 0; i < xn->nodes.size(); i++)
		BrowseVariable(xn->nodes[i], kf);
}

void AtanorCode::BrowseVariableVector(x_node* xn, Atanor* kf) {
	if (xn == NULL)
		return;

	if (xn->token == "variable") {
		short id = globalAtanor->Getid(xn->nodes[0]->value);
		if (!kf->isDeclared(id) || kf->Declaration(id) == aNOELEMENT) {
			Atanor* var = new AtanorSelfDeclarationVariable(global, id, a_self, kf);
			kf->Declare(id, var);
		}
		return;
	}

	for (size_t i = 0; i < xn->nodes.size(); i++)
		BrowseVariableVector(xn->nodes[i], kf);
}

void AtanorCode::BrowseVariableMap(x_node* xn, Atanor* kf) {
	if (xn == NULL)
		return;

	if (xn->token == "variable") {
		short id = globalAtanor->Getid(xn->nodes[0]->value);
		if (!kf->isDeclared(id) || kf->Declaration(id) == aNOELEMENT) {
			Atanor* var = new AtanorSelfDeclarationVariable(global, id, a_self, kf);
			kf->Declare(id, var);
		}
		return;
	}

	for (size_t i = 0; i < xn->nodes.size(); i++)
		BrowseVariableMap(xn->nodes[i], kf);
}

void AtanorCode::DeclareVariable(x_node* xn, Atanor* kf) {
	if (xn == NULL)
		return;

	if (xn->token == "variable") {
		short id = globalAtanor->Getid(xn->nodes[0]->value);
		if (!kf->isDeclared(id))
			kf->Declare(id, aNOELEMENT);
		return;
	}

	for (size_t i = 0; i < xn->nodes.size(); i++)
		DeclareVariable(xn->nodes[i], kf);
}

//--------------------------------------------------------------------
//declared in automate.cxx
void EvaluateMetaString(string& thestr, string sub);
//--------------------------------------------------------------------
long AtanorCode::Computecurrentline(int i, x_node* xn) {
	current_start = globalAtanor->currentbnf->x_linenumber(xn->start + i);
	current_end = globalAtanor->currentbnf->x_linenumber(xn->end + i);
	return (long)current_start;
}

//--------------------------------------------------------------------
void AtanorGlobal::RecordCompileFunctions() {
	parseFunctions["declaration"] = &AtanorCode::C_parameterdeclaration;
	parseFunctions["multideclaration"] = &AtanorCode::C_multideclaration;
	parseFunctions["subfunc"] = &AtanorCode::C_subfunc;
	parseFunctions["subfuncbis"] = &AtanorCode::C_subfunc;
	parseFunctions["regularcall"] = &AtanorCode::C_regularcall;
	parseFunctions["purecall"] = &AtanorCode::C_regularcall;

	parseFunctions["variable"] = &AtanorCode::C_variable;
	parseFunctions["purevariable"] = &AtanorCode::C_variable;

	parseFunctions["indexes"] = &AtanorCode::C_indexes;
	parseFunctions["interval"] = &AtanorCode::C_interval;

	parseFunctions["parameters"] = &AtanorCode::C_parameters;
	parseFunctions["dependencyparameters"] = &AtanorCode::C_parameters;

	parseFunctions["astring"] = &AtanorCode::C_astring;
	parseFunctions["afullstring"] = &AtanorCode::C_astring;
	parseFunctions["astringdouble"] = &AtanorCode::C_astring;
	parseFunctions["astringsimple"] = &AtanorCode::C_astring;

	parseFunctions["ufullstring"] = &AtanorCode::C_ustring;
	parseFunctions["ustringdouble"] = &AtanorCode::C_ustring;
	parseFunctions["ustringsimple"] = &AtanorCode::C_ustring;

	parseFunctions["punct"] = &AtanorCode::C_astring;

	parseFunctions["anumber"] = &AtanorCode::C_anumber;
	parseFunctions["xnumber"] = &AtanorCode::C_axnumber;
	parseFunctions["frame"] = &AtanorCode::C_frame;
	parseFunctions["extension"] = &AtanorCode::C_extension;

	parseFunctions["valvector"] = &AtanorCode::C_valvector;
	parseFunctions["valvectortail"] = &AtanorCode::C_valvector;
	parseFunctions["onelisp"] = &AtanorCode::C_valvector;
	parseFunctions["onetag"] = &AtanorCode::C_valvector;
	parseFunctions["valpredicatevector"] = &AtanorCode::C_valvector;

	parseFunctions["jvector"] = &AtanorCode::C_jsonvector;

	parseFunctions["valmap"] = &AtanorCode::C_valmap;
	parseFunctions["maptail"] = &AtanorCode::C_valmap;
	parseFunctions["valmaptail"] = &AtanorCode::C_valmap;
	parseFunctions["hvalmaptail"] = &AtanorCode::C_valmap;
	parseFunctions["valmappredicate"] = &AtanorCode::C_valmap;
	parseFunctions["mapmerging"] = &AtanorCode::C_valmap;
	parseFunctions["jmap"] = &AtanorCode::C_jsonmap;


	parseFunctions["features"] = &AtanorCode::C_features;
	parseFunctions["dico"] = &AtanorCode::C_dico;
	parseFunctions["dicoval"] = &AtanorCode::C_dico;
	parseFunctions["predicatedico"] = &AtanorCode::C_dico;

	parseFunctions["jdico"] = &AtanorCode::C_jsondico;

	parseFunctions["intentionvector"] = &AtanorCode::C_intentionvector;
	parseFunctions["intentionwithdouble"] = &AtanorCode::C_intentionwithdouble;

	parseFunctions["breakcall"] = &AtanorCode::C_uniquecall;
	parseFunctions["returncall"] = &AtanorCode::C_uniquecall;
	parseFunctions["breakpointcall"] = &AtanorCode::C_uniquecall;
	parseFunctions["continuecall"] = &AtanorCode::C_uniquecall;

	parseFunctions["function"] = &AtanorCode::C_createfunction;
	parseFunctions["blocs"] = &AtanorCode::C_blocs;

	parseFunctions["operatoraffectation"] = &AtanorCode::C_operator;
	parseFunctions["operator"] = &AtanorCode::C_operator;
	parseFunctions["hoper"] = &AtanorCode::C_operator;
	parseFunctions["comparator"] = &AtanorCode::C_operator;
	parseFunctions["hcomparator"] = &AtanorCode::C_operator;
	parseFunctions["orand"] = &AtanorCode::C_operator;
	parseFunctions["increment"] = &AtanorCode::C_increment;
	parseFunctions["affectation"] = &AtanorCode::C_affectation;
	parseFunctions["affectationpredicate"] = &AtanorCode::C_affectation;
	parseFunctions["predplusplus"] = &AtanorCode::C_increment;
	parseFunctions["comparepredicate"] = &AtanorCode::C_comparepredicate;

	parseFunctions["multiply"] = &AtanorCode::C_multiply;	
	parseFunctions["hmultiply"] = &AtanorCode::C_multiply;
	parseFunctions["operation"] = &AtanorCode::C_operation;
	parseFunctions["fulloperation"] = &AtanorCode::C_operation;
	parseFunctions["hoperation"] = &AtanorCode::C_operation;
	parseFunctions["plusplus"] = &AtanorCode::C_plusplus;
	parseFunctions["power"] = &AtanorCode::C_plusplus;
	parseFunctions["operationin"] = &AtanorCode::C_operationin;
	parseFunctions["comparison"] = &AtanorCode::C_comparison;
	parseFunctions["hcomparison"] = &AtanorCode::C_comparison;
	parseFunctions["abool"] = &AtanorCode::C_constant;
	parseFunctions["aconstant"] = &AtanorCode::C_constant;
	parseFunctions["negated"] = &AtanorCode::C_negated;

	parseFunctions["parenthetic"] = &AtanorCode::C_parenthetic;
	parseFunctions["optionalboolean"] = &AtanorCode::C_parenthetic;
	parseFunctions["hoptionalboolean"] = &AtanorCode::C_parenthetic;

	parseFunctions["iftest"] = &AtanorCode::C_test;
	parseFunctions["localif"] = &AtanorCode::C_test;
	parseFunctions["testelif"] = &AtanorCode::C_test;

	parseFunctions["negation"] = &AtanorCode::C_negation;

	parseFunctions["booleanexpression"] = &AtanorCode::C_booleanexpression;
	parseFunctions["hbooleanexpression"] = &AtanorCode::C_booleanexpression;

	parseFunctions["switch"] = &AtanorCode::C_switch;
	parseFunctions["testswitch"] = &AtanorCode::C_testswitch;

	parseFunctions["loop"] = &AtanorCode::C_loop;
	parseFunctions["doloop"] = &AtanorCode::C_doloop;
	parseFunctions["for"] = &AtanorCode::C_for;
	parseFunctions["blocfor"] = &AtanorCode::C_blocfor;
	parseFunctions["forin"] = &AtanorCode::C_forin;

	parseFunctions["trycatch"] = &AtanorCode::C_trycatch;

	parseFunctions["alist"] = &AtanorCode::C_alist;
	parseFunctions["valtail"] = &AtanorCode::C_alist;
	parseFunctions["apredicatelist"] = &AtanorCode::C_alist;
	parseFunctions["merging"] = &AtanorCode::C_alist;
	parseFunctions["alistnomerge"] = &AtanorCode::C_alist;

	//Predicates
	parseFunctions["cut"] = &AtanorCode::C_cut;
	parseFunctions["fail"] = &AtanorCode::C_cut;
	parseFunctions["stop"] = &AtanorCode::C_cut;
	parseFunctions["dcg"] = &AtanorCode::C_dcg;
	parseFunctions["predicatefact"] = &AtanorCode::C_predicatefact;
	parseFunctions["predicatedefinition"] = &AtanorCode::C_predicatefact;
	parseFunctions["predicate"] = &AtanorCode::C_predicate;
	parseFunctions["predicateexpression"] = &AtanorCode::C_predicateexpression;
	parseFunctions["predicatevariable"] = &AtanorCode::C_predicatevariable;
	parseFunctions["assertpredicate"] = &AtanorCode::C_assertpredicate;
	parseFunctions["term"] = &AtanorCode::C_term;
	parseFunctions["tuple"] = &AtanorCode::C_term;
	parseFunctions["predicatecall"] = &AtanorCode::C_regularcall;

	parseFunctions["dependencyrule"] = &AtanorCode::C_dependencyrule;
	parseFunctions["dependency"] = &AtanorCode::C_dependency;
	parseFunctions["dependencyfact"] = &AtanorCode::C_dependency;
	parseFunctions["dependencyresult"] = &AtanorCode::C_dependencyresult;

	//Haskell vector handling...

	parseFunctions["declarationhaskell"] = &AtanorCode::C_declarationhaskell;
	parseFunctions["haskelldico"] = &AtanorCode::C_dico;
	parseFunctions["haskellcall"] = &AtanorCode::C_haskellcall;
	parseFunctions["haskellcase"] = &AtanorCode::C_haskellcase;
	parseFunctions["guard"] = &AtanorCode::C_guard;
	parseFunctions["letmin"] = &AtanorCode::C_letmin;
	parseFunctions["hinexpression"] = &AtanorCode::C_hinexpression;
	parseFunctions["whereexpression"] = &AtanorCode::C_whereexpression;
	parseFunctions["haskellvector"] = &AtanorCode::C_haskellvector;
	parseFunctions["haskellmap"] = &AtanorCode::C_haskellmap;

	parseFunctions["haskellexpression"] = &AtanorCode::C_haskellexpression;
	parseFunctions["haskellkeymap"] = &AtanorCode::C_haskellexpression;

	parseFunctions["repeating"] = &AtanorCode::C_cycling;
	parseFunctions["cycling"] = &AtanorCode::C_cycling;

	parseFunctions["flipping"] = &AtanorCode::C_flipping;
	parseFunctions["mapping"] = &AtanorCode::C_mapping;
	parseFunctions["filtering"] = &AtanorCode::C_filtering;
	parseFunctions["taking"] = &AtanorCode::C_filtering;

	parseFunctions["zipping"] = &AtanorCode::C_zipping;
	parseFunctions["pairing"] = &AtanorCode::C_zipping;

	parseFunctions["folding1"] = &AtanorCode::C_folding;
	parseFunctions["folding"] = &AtanorCode::C_folding;
	parseFunctions["haskellbooling"] = &AtanorCode::C_haskellbooling;
	parseFunctions["hfunctioncall"] = &AtanorCode::C_hfunctioncall;
	parseFunctions["hcompose"] = &AtanorCode::C_hcompose;
	parseFunctions["let"] = &AtanorCode::C_multideclaration;
	parseFunctions["hlambda"] = &AtanorCode::C_hlambda;
	parseFunctions["telque"] = &AtanorCode::C_telque;
	parseFunctions["subtelque"] = &AtanorCode::C_telque;
	parseFunctions["hbloc"] = &AtanorCode::C_hbloc;
	parseFunctions["hdata"] = &AtanorCode::C_hdata;
	parseFunctions["hdeclaration"] = &AtanorCode::C_hdeclaration;
}


//------------------------------------------------------------------------
AtanorInstruction* AtanorCode::AtanorCreateInstruction(Atanor* parent, short op) {
	AtanorInstruction* res;
	switch (op) {
	case a_plusequ:
	case a_minusequ:
	case a_multiplyequ:
	case a_divideequ:
	case a_modequ:
	case a_powerequ:
	case a_shiftleftequ:
	case a_shiftrightequ:
	case a_orequ:
	case a_xorequ:
	case a_andequ:
	case a_mergeequ:
	case a_addequ:
		res = new AtanorInstructionAPPLYOPERATIONEQU(global, parent);
		break;
	case a_stream:
		res = new AtanorInstructionSTREAM(global, parent);
		break;
	case a_affectation:
		res = new AtanorInstructionAFFECTATION(global, parent);
		break;		
	case a_less:
	case a_more:
	case a_same:
	case a_different:
	case a_lessequal:
	case a_moreequal:
		res = new AtanorInstructionCOMPARE(global, parent);
		break;
	case a_plus:
	case a_minus:
	case a_multiply:
	case a_divide:
	case a_mod:
	case a_power:
	case a_shiftleft:
	case a_shiftright:
	case a_or:
	case a_xor:
	case a_and:
	case a_merge:
	case a_add:
		res = new AtanorInstructionAPPLYOPERATION(global, parent);
		break;
	case a_disjunction:
		res = new AtanorInstructionOR(global, parent);
		break;
	case a_conjunction:
		res = new AtanorInstructionAND(global, parent);
		break;
	case a_notin:
	case a_in:
		res = new AtanorInstructionIN(global, parent);
		break;
	case a_match:
		res = new AtanorInstructionHASKELLCASE(global, parent);
		break;
	default:
		res = new AtanorInstruction(a_instructions, global, parent);
	}

	res->Setaction(op);
	return res;
}

AtanorInstruction* AtanorCode::CloningInstruction(AtanorInstruction* ki) {
	if (ki->action == a_bloc || ki->action == a_blocboolean || !ki->isInstruction())
		return ki;

	AtanorInstruction* k = AtanorCreateInstruction(NULL, ki->Action());
	ki->Clone(k);
	k->instructions.clear();
	variables.clear();
	for (short i = 0; i < ki->instructions.last; i++)
		k->instructions.push_back(EvaluateVariable(ki->instructions.vecteur[i]));

	ki->Remove();
	if (k->Action() >= a_less && k->Action() <= a_moreequal) {
		//Let's try analysing this stuff...
		AtanorInstruction* kcomp = (AtanorInstruction*)k->Compile(NULL);
		if (k == kcomp)
			return k;

		k->Remove();
		return kcomp;
	}

	return k;
}

Atanor* AtanorCode::EvaluateVariable(Atanor* var) {
	if (var->Action() == a_variable) {
		short n = var->Name();
		if (variables.check(n)) {
			var->Remove();
			return variables[n];
		}

		AtanorActionVariable* act;
		if (var->Typeinfered() == a_self || var->Typeinfered() == a_let)
			act = new AtanorActionLetVariable(n, var->Typeinfered());
		else
			act = new AtanorActionVariable(n, var->Typeinfered());
		var->Remove();
		variables[n] = act;
		return act;
	}

	return var;
}

//-------------------------------------------------------------------------------------

static uchar Evaluateatomtype(Atanor* ins) {
	short ty = ins->Typeinfered();
	if (ty == a_none)
		return 255;

	if (ty < a_short || ty > a_ustring) {
		if (ins->Function() != NULL) {
			if (!globalAtanor->newInstance.check(ty))
				return 255;

			Atanor* a = globalAtanor->newInstance[ty];
			if (a->isValueContainer() && ins->Function()->isIndex()) {
				if (a->isDecimal())
					ty = a_decimal;
				else
				if (a->isFloat())
					ty = a_float;
				else
				if (a->isString()) {
					if (a->Type() == a_ustring)
						ty = a_ustring;
					else
						ty = a_string;
				}
				else
				if (a->isLong())
					ty = a_long;
				else
				if (a->isShort())
					ty = a_short;
				else
				if (a->isNumber())
					ty = a_int;
			}
			else
				return 255;
			return Returnequ(ty);
		}

		if (ins->isDecimal())
			ty = a_decimal;
		else
		if (ins->isFloat())
			ty = a_float;
		else
		if (ins->isString()) {
			if (ins->Type() == a_ustring)
				ty = a_ustring;
			else
				ty = a_string;
		}
		else
		if (ins->isLong())
			ty = a_long;
		else
		if (ins->isShort())
			ty = a_short;
		if (ins->isNumber())
			ty = a_int;
		else
			return 255;
	}
	return Returnequ(ty);
}

bool AtanorInstructionAPPLYOPERATIONROOT::Stacking(Atanor* ins, char top) {	
	if (top && !head) {
		//we might need to detect the type of the all instruction set, which is based on
		//the deepest element in the structure on the left...
		Atanor* loop = ins;
		while (loop != NULL && loop->isInstruction()) loop = loop->Instruction(0);
		if (loop != NULL)
			head = Evaluateatomtype(loop);
		else
			head = 255;
	}

	if (ins->isInstruction()) {
		bool simple = true;

		if (!ins->isEQU() && !ins->isOperation()) {
			instructions.push_back(ins);
			return simple;
		}

		char t = top;

		if (ins->Action() == a_divide) {
			alls |= b_float;
			if (t == 2)
				t = 0;
		}
		else
		if (ins->Action() >= a_shiftleft) {
			if (t == 2)
				t = 0;
			alls |= b_forcedlong;			
		}

		if (ins->Subcontext() && instructions.size() == 0) {
			instructions.push_back(aPIPE);
			sub = true;
			t = 2;
			simple = false;
		}

		AtanorInstruction* ai = (AtanorInstruction*)ins;
		for (short i = ai->instructions.size() - 1; i >= 0; i--) {
			if (ai->instructions[i]->isOperation()) {
				instructions.push_back(aPIPE);
				sub = true;
				t = 2;
				simple = false;
			}
			if (!Stacking(ai->instructions[i], t))
				simple = false;
			if (t == 2)
				t = 0;
		}

		if (!ins->isEQU()) {
			instructions.push_back(globalAtanor->actions[ins->Action()]);			
			if (top != 1)
				ins->Remove();
		}

		return simple;
	}

	Atanor* haskell = ins->Composition();
	if (haskell != aNOELEMENT) {
		if (haskell->isROOTOPERATION()) {
			//in that case, we need to merge the instructions from haskell into our current structure
			AtanorInstructionAPPLYOPERATIONROOT* local = (AtanorInstructionAPPLYOPERATIONROOT*)haskell;
			instructions.push_back(aPIPE);
			sub = true;
			for (short i = 0; i < local->instructions.last; i++)
				instructions.push_back(local->instructions[i]);

			haskell->Remove();
			ins->Remove();
			return true;
		}
		ins->Remove();
		ins = haskell;
	}

	bool remove = false;
	if (ins->Action() == a_variable && ins->Type() != a_actionvariable) {
		short n = ins->Name();
		if (variables.check(n))
			instructions.push_back(variables[n]);
		else {
			AtanorActionVariable* act;
			if (ins->Typeinfered() == a_self || ins->Typeinfered() == a_let)
				act = new AtanorActionLetVariable(n, ins->Typeinfered());
			else
				act = new AtanorActionVariable(n, ins->Typeinfered());
			instructions.push_back(act);
			variables[n] = act;
		}
		remove = true;
	}
	else
		instructions.push_back(ins);

	if (top) {
		uchar ty = Evaluateatomtype(ins);
		alls |= ty;

		if (ins->Typeinfered() == a_fraction)
			fraction = true;
	}

	if (remove)
		ins->Remove();

	return true;
}

AtanorInstruction* AtanorInstructionAPPLYOPERATIONROOT::Returnlocal(AtanorGlobal* g, Atanor* parent) {

	if (!thetype) {
		if (alls == 255)
			thetype = 255;
		else
			thetype = head;
	}

	if (sub && thetype != 255) {

		//Otherwise, we will go through ccompute
		if (alls != 255) {
			//If we have only numbers or only strings, we can apply one of the regular strategies...
			if ((alls & b_allnumbers) == alls || (alls & b_allstrings) == alls)
				sub = false;
			else {
				head = 0;
				if ((alls & b_forcedlong))
					head = b_long;
				else
				if ((alls & b_float) || (alls & b_longdecimal) == b_longdecimal)
					head = b_float;
				else
				if ((alls & b_decimal))
					head = b_decimal;
				else
				if ((alls & b_long))
					head = b_long;
				else
				if ((alls & b_int) || (alls & b_short))
					head = b_int;
			}
		}
	}


	if (!sub) {
		switch (thetype) {
		case b_string:
			return new AtanorInstructionSTRING(this, g, parent);
		case b_ustring:
			return new AtanorInstructionUSTRING(this, g, parent);
		case b_short:
		case b_int:
		case b_long:
		case b_forcedlong:
		case b_decimal:
		case b_float:
			//First we try to get the top numerical value
			alls |= thetype;
			if (alls != 255) {
				if ((alls & b_forcedlong))
					return new AtanorInstructionLONG(this, g, parent);

				if ((alls & b_float) || (alls & b_longdecimal) == b_longdecimal) {
					if (fraction)
						return new AtanorInstructionFRACTION(this, g, parent);
					return new AtanorInstructionFLOAT(this, g, parent);
				}

				if ((alls & b_decimal))
					return new AtanorInstructionDECIMAL(this, g, parent);

				if ((alls & b_long))
					return new AtanorInstructionLONG(this, g, parent);

				if ((alls & b_int))
					return new AtanorInstructionINTEGER(this, g, parent);

				if ((alls & b_short))
					return new AtanorInstructionSHORT(this, g, parent);
			}

			switch (thetype) {
			case b_short:
				return new AtanorInstructionSHORT(this, g, parent);
			case b_int:
				return new AtanorInstructionINTEGER(this, g, parent);
			case b_long:
				return new AtanorInstructionLONG(this, g, parent);
			case b_decimal:
				return new AtanorInstructionDECIMAL(this, g, parent);
			case b_float:
				if (fraction)
					return new AtanorInstructionFRACTION(this, g, parent);
				return new AtanorInstructionFLOAT(this, g, parent);
			}
		}
	}

	if (parent != NULL)
		parent->AddInstruction(this);
	return this;
}

//Composition returns a potential ROOT instruction that could be merged within a ROOT...
Atanor* AtanorCallFunctionHaskell::Composition() {
	if (body->lambdadomain.instructions.size() == 0 && body->instructions.size() == 1)
		return body->instructions.back()->Argument(0);

	return aNOELEMENT;
}

Atanor* AtanorInstructionAPPLYOPERATION::Compile(Atanor* parent) {
	AtanorInstructionAPPLYOPERATIONROOT* kroot = new AtanorInstructionAPPLYOPERATIONROOT(globalAtanor);
	if (parent != NULL)
		kroot->thetype = Evaluateatomtype(parent);

	kroot->Stacking(this, true);
	kroot->Setsize();
	Atanor* kvroot = kroot->Returnlocal(globalAtanor);
	if (kvroot != kroot) {
		kroot->Remove();
		return kvroot;
	}

	return kroot;
}

Atanor* AtanorInstructionCOMPARE::Compile(Atanor* parent) {
	uchar left;
	uchar right;
	if (parent == NULL) {
		left = Evaluateatomtype(instructions.vecteur[0]);
		right = Evaluateatomtype(instructions.vecteur[1]);
	}
	else {
		left = Evaluateatomtype(instructions.vecteur[0]->Get(aNULL, aNULL, 0));
		right = Evaluateatomtype(instructions.vecteur[1]->Get(aNULL, aNULL, 0));
	}
	if (left != 255 && right != 255 && (left&b_allnumbers) && (right&b_allnumbers)) {
		left |= right;
		
		AtanorInstruction* res;
		if ((left & b_float) || (left & b_longdecimal) == b_longdecimal) {
			res = new AtanorInstructionCOMPAREFLOAT(globalAtanor);
			res->instructions = instructions;
			res->action = action;
			return res;
		}

		if ((left & b_decimal)) {
			res = new AtanorInstructionCOMPAREDECIMAL(globalAtanor);
			res->instructions = instructions;
			res->action = action;
			return res;
		}

		if ((left & b_long)) {
			res = new AtanorInstructionCOMPARELONG(globalAtanor);
			res->instructions = instructions;
			res->action = action;
			return res;
		}

		if ((left & b_int)) {
			res = new AtanorInstructionCOMPAREINTEGER(globalAtanor);
			res->instructions = instructions;
			res->action = action;
			return res;
		}

		if ((left & b_short)) {
			res = new AtanorInstructionCOMPARESHORT(globalAtanor);
			res->instructions = instructions;
			res->action = action;
			return res;
		}
	}
	return this;
}


//-------------------------------------------------------------------------------------


static bool TestFunction(string& token, bool func) {
	if (token == "subfunc" || token == "subfuncbis")
		return true;
	if (func == false) {
		if (token == "indexes" || token == "interval")
			return true;
	}
	return false;
}

//------------------------------------------------------------------------
long AtanorGlobal::Getcurrentline() {
	if (spaceid >= spaces.size())
		return -1;
	return spaces[spaceid]->Getcurrentline();
}

string AtanorGlobal::Getcurrentfile() {
	if (spaceid >= spaces.size())
		return "";
	short idfile = spaces[spaceid]->currentfileid;
	return Getfilename(idfile);
}

short AtanorGlobal::Getfileid(short& idc, string f) {
	short idf = -1;
	for (idc = 0; idc < spaces.size(); idc++) {
		idf = spaces[idc]->Getfileid(f);
		if (idf != -1)
			return idf;
	}
	return -1;
}

Exporting string AtanorGlobal::Getfilename(short fileid) {
	if (fileid != -1)
		return filenames[fileid];
	return "";
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
// There is only three possible spot in which an object can have been declared
// On the top of the stack
// On the current domain (which a frame that is being declared)
// In the global space...
Atanor* AtanorCode::Declaror(short id) {
	Atanor* a;
	for (BLONG i = globalAtanor->Stacksize() - 1; i >= 0; i--) {
		a = globalAtanor->Stack(i);
		if (a->isDeclared(id))
			return a;
	}
	return NULL;
}

Atanor* AtanorCode::Declaror(short id, Atanor* parent) {
	if (parent->isDeclared(id))
		return parent;

	Atanor* a;
	for (BLONG i = globalAtanor->Stacksize() - 1; i >= 0; i--) {
		a = globalAtanor->Stack(i);
		if (a->isDeclared(id))
			return a;
	}
	return NULL;
}

Atanor* AtanorCode::Frame() {
	Atanor* a;
	for (BLONG i = globalAtanor->Stacksize() - 1; i >= 1; i--) {
		a = globalAtanor->Stack(i);
		if (a->isFrame())
			return a;
	}
	return NULL;
}


Atanor* AtanorCode::Declaration(short id) {
	Atanor* a;
	for (BLONG i = globalAtanor->Stacksize() - 1; i >= 0; i--) {
		a = globalAtanor->Stack(i);
		if (a->isDeclared(id))
			return a->Declaration(id);
	}
	return NULL;
}

Atanor* AtanorCode::Declaration(short id, Atanor* parent) {
	if (parent->isDeclared(id))
		return parent->Declaration(id);

	Atanor* a;
	for (BLONG i = globalAtanor->Stacksize() - 1; i >= 0; i--) {
		a = globalAtanor->Stack(i);
		if (a->isDeclared(id))
			return a->Declaration(id);
	}
	return NULL;
}

bool AtanorCode::isDeclared(short id) {
	Atanor* a;
	for (BLONG i = globalAtanor->Stacksize() - 1; i >= 0; i--) {
		a = globalAtanor->Stack(i);
		if (a->isDeclared(id))
			return true;
	}
	return false;
}

Atanor* AtanorMainFrame::Declaration(short id) {
	Locking _lock(_locker);
	if (declarations.check(id) == false)
		return NULL;
	return declarations[id];
}

void AtanorMainFrame::Declare(short id, Atanor* a) {
	Locking _lock(_locker);
	declarations[id] = a;
	if (a->isFunction())
		exported[id] = true;
}

bool AtanorMainFrame::isDeclared(short id) {
	Locking _lock(_locker);
	if (declarations.check(id))
		return true;
	return false;
}

inline Atanor* ThreadStruct::GetTopFrame() {
	for (int i = stack.last - 1; i >= 0; i--) {
		if (stack[i]->isFrame())
			return stack[i];
	}
	return NULL;
}

Atanor* AtanorGlobal::GetTopFrame(short idthread) {
	return threads[idthread].GetTopFrame();
}

inline Atanor* ThreadStruct::Declarator(short id) {
	for (int i = stack.last - 1; i >= 0; i--) {
		if (stack.vecteur[i]->isDeclared(id))
			return stack.vecteur[i];
	}
	return aNULL;
}


Atanor* AtanorGlobal::Declarator(short id, short idthread) {
	return threads[idthread].Declarator(id);
}

inline Atanor* ThreadStruct::Getdefinition(short id) {
	Atanor* value;
	for (int i = stack.last - 1; i >= 0; i--) {
		value = stack.vecteur[i]->Declaration(id);
		if (value != NULL)
			return value;
	}
	return aNULL;
}

Atanor* AtanorGlobal::Getdefinition(short id, short idthread, Atanor* current) {
	if (current != aNULL) {
		current = current->Declaration(id);
		if (current != NULL)
			return current;
	}

	return threads[idthread].Getdefinition(id);
}

Atanor* AtanorGlobal::Getdeclaration(short id, short idthread) {
	return threads[idthread].variables.get(id).back();
}

inline bool ThreadStruct::isDeclared(short id) {
	for (int i = stack.last - 1; i >= 0; i--) {
		if (stack.vecteur[i]->isDeclared(id))
			return true;
	}
	return false;
}

bool AtanorGlobal::isDeclared(short id, short idthread) {
	if (threads[idthread].variables.check(id) && threads[idthread].variables[id].last)
		return true;
	return false;
}
//---------------------------------------------------------------------

Atanor* AtanorCode::C_parameterdeclaration(x_node* xn, Atanor* parent) {
	Atanor* top = globalAtanor->Topstack();

	string& type = xn->nodes[0]->value;
	bool isprivate = false;
	if (xn->nodes[0]->nodes[0]->token == "private") {
		isprivate = true;
		type = xn->nodes[0]->nodes[1]->value;
	}

	if (globalAtanor->symbolIds.find(type) == globalAtanor->symbolIds.end()) {
		stringstream message;
		message << "Unknown type: " << type;
		throw new AtanorRaiseError(message, filename, current_start, current_end);
	}

	short tid = globalAtanor->symbolIds[type];
	Atanor* element = NULL;
	if (global->newInstance.find(tid) == global->newInstance.end()) {
		stringstream message;
		message << "Unknown type: " << type;
		throw new AtanorRaiseError(message, filename, current_start, current_end);
	}

	short name = globalAtanor->Getid(xn->nodes[1]->value);

	if (top->isDeclared(name)) {
		stringstream message;
		message << "This variable has already been declared: " << xn->nodes[1]->value;
		throw new AtanorRaiseError(message, filename, current_start, current_end);
	}

	//We declare a variable, which we store in our declaration domain...
	AtanorVariableDeclaration* a;
	if (tid == a_self || tid == a_let)
		a = new AtanorSelfDeclarationVariable(global, name, tid, parent);
	else {
		if (parent == &mainframe) {
			a = new AtanorGlobalVariableDeclaration(global, name, tid, isprivate, false, parent);
			global->Storevariable(0, name, aNOELEMENT); //a dummy version to avoid certain bugs in the console
			//Basically, if a program fails before allocating this variable, and the variable is still requested in the console, it might crash...
		}
		else {
			if (parent->isFrame())
				a = new AtanorFrameVariableDeclaration(global, name, tid, isprivate, iscommon, parent);
			else
				a = new AtanorVariableDeclaration(global, name, tid, isprivate, false, parent);
		}
	}

	if (xn->nodes.size() == 3) {
		AtanorInstruction bloc;
		//this is a temporary affectation, in order to push the type of the variable
		//into its affectation value...
		bloc.action = a_affectation;
		bloc.instructions.push_back(a);
		a->choice = 0;
		Traverse(xn->nodes[2], &bloc);
		a->AddInstruction(bloc.instructions[1]);
	}

	top->Declare(name, a);

	return element;
}

Atanor* AtanorCode::C_multideclaration(x_node* xn, Atanor* parent) {
	Atanor* top = globalAtanor->Topstack();

	string& type = xn->nodes[0]->value;
	bool oldprive = isprivate;
	bool oldcommon = iscommon;
	bool oldconstant = isconstant;
	if (xn->nodes[0]->nodes.size() && xn->nodes[0]->nodes[0]->token == "feature") {
		string& s = xn->nodes[0]->nodes[0]->nodes[0]->value;
		if (s == "private") {
			isprivate = true;
			if (xn->nodes[0]->nodes[0]->nodes.size() != 1)
				iscommon = true;
		}
		else
		if (s == "common")
			iscommon = true;
		else
		if (s == "const")
			isconstant = true;
		type = xn->nodes[0]->nodes[1]->value;
	}

	Atanor* element = NULL;
	size_t last = xn->nodes.size() - 1;
	bool recall = false;

	x_node* xnew;
	if (xn->nodes[last]->token == "declarationlist") {
		xnew = new x_node(xn->nodes[0]->token, type, xn);
		xn->nodes[last]->nodes.insert(xn->nodes[last]->nodes.begin(), xnew);
		recall = true;
	}

	if (globalAtanor->symbolIds.find(type) == globalAtanor->symbolIds.end()) {
		stringstream message;
		message << "Unknown type: " << type;
		throw new AtanorRaiseError(message, filename, current_start, current_end);
	}

	short tid = globalAtanor->symbolIds[type];

	if (global->newInstance.find(tid) == global->newInstance.end()) {
		stringstream message;
		message << "Unknown type: " << type;
		throw new AtanorRaiseError(message, filename, current_start, current_end);
	}

	string name = xn->nodes[1]->value;
	short idname = global->Getid(name);

	if (tid == a_atanor) {
		if (xn->nodes.size() != 3) {
			stringstream message;
			message << "Missing parameter: '" << name << "'";
			throw new AtanorRaiseError(message, filename, current_start, current_end);
		}

		Atanor* ret = Callingprocedure(xn->nodes[2], tid);
		((Atanoratanor*)ret)->InitialisationModule(idname, global);
		top->Declare(idname, ret);
		return parent;
	}

	if (top->isDeclared(idname) && top->Declaration(idname) != aNOELEMENT) {
		stringstream message;
		message << "This variable has already been declared: " << name;
		throw new AtanorRaiseError(message, filename, current_start, current_end);
	}

	if (top->isFunction()) {
		//this a specific case, where we cannot accept a variable which declared both in a function and in its frame...
		for (long i = 1; i < global->threads[0].stack.last - 1; i++) {
			if (global->threads[0].stack[i]->isFrame() && global->threads[0].stack[i]->isDeclared(idname)) {
				stringstream message;
				message << "This variable has already been declared as a frame variable: " << name;
				throw new AtanorRaiseError(message, filename, current_start, current_end);
			}
		}
	}

	if (recall) {
		C_multideclaration(xn->nodes[last], parent);
		xnew = xn->nodes[last];
		xn->nodes.erase(xn->nodes.begin() + last);
		delete xnew;
	}


	//We declare a variable, which we store in our declaration domain...
	if (tid == a_predicate)
		globalAtanor->predicates[idname] = new AtanorPredicateFunction(global, NULL, idname);

	AtanorVariableDeclaration* a;
	if (tid >= a_intthrough && tid <= a_floatthrough)
		a = new AtanorThroughVariableDeclaration(idname, tid, parent);
	else {
		if (tid == a_self || tid == a_let)
			a = new AtanorSelfDeclarationVariable(global, idname, tid, parent);
		else {
			if (parent == &mainframe) {
				a = new AtanorGlobalVariableDeclaration(global, idname, tid, isprivate, isconstant, parent);
				global->Storevariable(0, idname, aNOELEMENT); //a dummy version to avoid certain bugs in the console
			}
			else {
				if (parent->isFrame())
					a = new AtanorFrameVariableDeclaration(global, idname, tid, isprivate, iscommon, parent);
				else
					a = new AtanorVariableDeclaration(global, idname, tid, isprivate, isconstant, parent);
			}
		}
	}

	top->Declare(idname, a);

	if (xn->nodes.size() >= 3) {
		for (size_t pos = 2; pos < xn->nodes.size(); pos++) {
			if (xn->nodes[pos]->token == "depend") {
				a->choice = 1;
				string funcname = xn->nodes[pos]->nodes[0]->value;
				short idfuncname = globalAtanor->Getid(funcname);
				Atanor* func = Declaration(idfuncname);
				if (func == NULL || !func->isFunction()) {
					stringstream message;
					message << "We can only associate a function through 'with': " << name;
					throw new AtanorRaiseError(message, filename, current_start, current_end);
				}

				if (func->isVariable()) {
					x_node nvar("variable", "", xn);
					creationxnode("word", globalAtanor->Getsymbol(func->Name()), &nvar);
					func = Traverse(&nvar, parent);
				}

				a->function = func;
				continue;
			}

			if (xn->nodes[pos]->token == "wnexpressions") {
				//This is a temporary assignment, in order to push the type of the variable
				//into its assignment value...
				AtanorInstruction bloc;
				bloc.action = a_affectation;
				bloc.instructions.push_back(a);
				a->choice = 0;
				Traverse(xn->nodes[pos], &bloc);
				a->AddInstruction(bloc.instructions[1]);
				continue;
			}

			if (xn->nodes[pos]->token == "parameters") {
				//Then we need to call a _initial method...
				if (!global->newInstance[tid]->isDeclared(a_initial)) {
					stringstream message;
					message << "Missing '_initial' function for this object: '" << globalAtanor->idSymbols[tid] << " " << name << "'";
					throw new AtanorRaiseError(message, filename, current_start, current_end);
				}
				AtanorCall* call;
				if (global->newInstance[tid]->isFrame()) {
					call = new AtanorCallFrameFunction((AtanorFrame*)global->newInstance[tid]->Frame(), a_initial, global, a);
					Traverse(xn->nodes[pos], call);
					if (!call->Checkarity()) {
						stringstream message;
						message << "Wrong number of arguments or incompatible argument in '_initial' function for this object: '" << globalAtanor->idSymbols[tid] << " " << name << "'";
						throw new AtanorRaiseError(message, filename, current_start, current_end);
					}
				}
				else {
					call = new AtanorCallMethod(a_initial, global, a);
					Traverse(xn->nodes[pos], call);
				}
				continue;
			}

			AtanorInstruction inst;
			Traverse(xn->nodes[pos], &inst);
			a->AddInstruction(inst.instructions[0]);
		}
	}

	isprivate = oldprive;
	iscommon = oldcommon;
	isconstant = oldconstant;

	return element;
}

Atanor* AtanorCode::C_subfunc(x_node* xn, Atanor* parent) {
	string name = xn->nodes[0]->nodes[0]->value;
	if (xn->nodes[0]->nodes[0]->nodes.size() != 1)
		name = xn->nodes[0]->nodes[0]->nodes[1]->value;

	parent->Addfunctionmode();
	short id = globalAtanor->Getid(name);
	Atanor* function = NULL;
	//We have then two cases, either parent is a frame instance or it is a variable...
	if (parent->isFrame()) {
		Atanor* frame = parent->Frame();
		function = frame->Declaration(id);
		if (function == NULL || function->isPrivate() || !function->isFunction()) {
			stringstream message;
			message << "Unknown function: '" << name << "'";
			throw new AtanorRaiseError(message, filename, current_start, current_end);
		}
		function = new AtanorCallFrameFunction((AtanorFrame*)frame, id, global, parent);
	}
	else {
		short tyvar = parent->Typevariable();
		if (globalAtanor->commons.check(id) && !globalAtanor->extensionmethods.check(id))
			function = new AtanorCallCommonMethod(id, global, parent);
		else {
			if (globalAtanor->extensions.check(tyvar) && globalAtanor->extensions[tyvar]->isDeclared(id))
				function = new AtanorCallCommonMethod(id, global, parent);
			else {
				if (globalAtanor->methods[tyvar].check(id))
					function = new AtanorCallMethod(id, global, parent);
				else {
					if (globalAtanor->allmethods.check(id)) {
						if (tyvar == a_self || tyvar == a_let || xn->token == "subfunc") {
							if (globalAtanor->extensionmethods.check(id))
								function = new AtanorCallCommonMethod(id, global, parent);
							else
								function = new AtanorCallMethod(id, global, parent);
						}
					}
					else {
						if (globalAtanor->framemethods.check(id))
							function = new AtanorCallFrameFunction(NULL, id, global, parent);
					}
				}
			}
		}
	}

	if (function == NULL) {
		stringstream message;
		message << "Unknown method: '" << name << "'";
		throw new AtanorRaiseError(message, filename, current_start, current_end);
	}

	//we then parse the arguments of the function call
	if (xn->nodes[0]->nodes.size() >= 2) {
		x_node* sub = xn->nodes[0];
		if (sub->nodes[1]->token != "parameters")
			function->Addfunctionmode();
		Atanor* a = Traverse(sub->nodes[1], function);
		if (sub->nodes.size() == 3) {
			function->Addfunctionmode();
			a = Traverse(sub->nodes[2], function);
		}
		return a;
	}

	if (!function->Checkarity()) {
		stringstream message;
		message << "Wrong number of arguments or incompatible argument: '" << name << "'";
		throw new AtanorRaiseError(message, filename, current_start, current_end);
	}

	return parent;
}

Atanor* AtanorCode::Callingprocedure(x_node* xn, short id) {
	AtanorCallProcedure proc(id);
	Traverse(xn, &proc);
	//First we execute our code to evaluate the current global variables...
	Atanor* call = globalAtanor->EvaluateMainVariable();
	if (!call->isError())
		call = proc.Get(&mainframe, aNULL, 0);

	if (call->isError()) {
		stringstream message;
		message << call->String();
		delete call;
		throw new AtanorRaiseError(message, filename, current_start, current_end);
	}
	return call;
}

bool AtanorCode::isaFunction(string& name, short id) {
	//Is it a procedure?
	if (globalAtanor->procedures.check(id))
		return true;

	//This HAS to be a function declaration...
	if (isDeclared(id)) {
		Atanor* call = Declaration(id);
		if (call->isHaskellFunction())
			return true;
		if (call->isFunction())
			return true;
	}
	return false;
}

Atanor* AtanorCode::C_regularcall(x_node* xn, Atanor* parent) {
	string name = xn->nodes[0]->value;
	short id = globalAtanor->Getid(name);
	string params;

	if (xn->token == "predicatecall")
		params = "predicateparameters";
	else
		params = "parameters";

	//It could be a predicate, then two cases as part of an expression or as a call to evaluation
	if (globalAtanor->predicates.check(id)) {
		//then it is a PredicateInstance
		if (parent->Type() != a_predicateruleelement && parent->Type() != a_parameterpredicate) {
			globalAtanor->predicatevariables.clear();
			AtanorInstructionLaunch* kbloc = new AtanorInstructionLaunch(global, parent);
			parent = kbloc;
		}

		AtanorPredicate* kx = globalAtanor->predicates[id];
		if (kx->isPredicateFunction()) {
			kx = (AtanorPredicate*)kx->Newinstance(0, parent);
			parent->AddInstruction(kx);
		}
		else
			kx = new AtanorPredicate(id, global, a_predicate, parent);

		if (xn->nodes.back()->token == params)
			ComputePredicateParameters(xn->nodes.back(), kx);

		if (!kx->Checkarity()) {
			stringstream message;
			message << "Wrong number of arguments or incompatible argument: '" << name << "'";
			throw new AtanorRaiseError(message, filename, current_start, current_end);
		}
		return kx;
	}

	//It could be a term, but we favor any frame or type interpretation first...
	if (!global->newInstance.check(id)) {
		if (globalAtanor->terms.check(id) || parent->Type() == a_parameterpredicate) {
			AtanorPredicateTerm* kx = new AtanorPredicateTerm(global, id, parent);
			if (globalAtanor->terms.find(id) == globalAtanor->terms.end())
				globalAtanor->terms[id] = name;
			if (xn->nodes.back()->token == params)
				ComputePredicateParameters(xn->nodes.back(), kx);
			return kx;
		}
	}

	Atanor* call = NULL;
	if (globalAtanor->systemfunctions.find(name) != globalAtanor->systemfunctions.end()) {
		Callingprocedure(xn->nodes[1], id);
		return parent;
	}

	if (id == a_return)
		call = new AtanorCallReturn(global, parent);
	else {
		if (xn->nodes[0]->nodes.size() == 2) {
			//A frameup call
			short framename = globalAtanor->Getid(xn->nodes[0]->nodes[0]->nodes[0]->value);
			AtanorFrame* top = globalAtanor->frames[framename];
			if (top == NULL) {
				stringstream message;
				message << "Unknown frame: '" << xn->nodes[0]->nodes[0]->nodes[0]->value << "'";
				throw new AtanorRaiseError(message, filename, current_start, current_end);
			}
			call = Frame();
			if (call == NULL || !call->isParent(top)) {
				stringstream message;
				message << "Frame function unreachable: '" << name << "'";
				throw new AtanorRaiseError(message, filename, current_start, current_end);
			}
			name = xn->nodes[0]->nodes[1]->value;
			id = globalAtanor->Getid(name);
			call = new AtanorCallTopFrameFunction(top, id, global, parent);
		}
		else {
			//Is it a procedure?
			//Is it a method? It should have been cleared when the variable was detected...
			if (globalAtanor->allmethods.check(id) && parent->isCall()) {
				//We create a call method...
				parent->Addfunctionmode();
				if (globalAtanor->commons.check(id))
					call = new AtanorCallCommonMethod(id, global, parent);
				else
					call = new AtanorCallMethod(id, global, parent);
			}
			else {
				if (globalAtanor->procedures.check(id)) {
					//We create a procedure call
					call = new AtanorCallProcedure(id, global, parent);
				}
			}

			//This HAS to be a function declaration...
			if (call == NULL && isDeclared(id)) {
				call = Declaration(id);
				if (call->isHaskellFunction())
					call = new AtanorCallFunctionArgsHaskell((AtanorFunctionLambda*)call, global, parent);
				else {
					if (call->isFunction()) {
						if (call->isThread())
							call = new AtanorCallThread(call, global, parent);
						else {
							if (call->isVariable())
								call = new AtanorFunctionDeclarationCall(call->Name(), global, parent);
							else
								call = new AtanorCallFunction(call, global, parent);
						}
					}
					else
						call = NULL;
				}
			}
		}
	}


	//We then parse the arguments of that function...
	if (call == NULL) {
		stringstream message;
		message << "Unknown function or procedure: '" << name << "'";
		throw new AtanorRaiseError(message, filename, current_start, current_end);
	}

	parent->Addargmode();
	//If we have parameters...
	if (xn->nodes.size() == 2)
		Traverse(xn->nodes[1], call);

	if (!call->Checkarity()) {
		stringstream message;
		message << "Wrong number of arguments or incompatible argument: '" << name << "'";
		throw new AtanorRaiseError(message, filename, current_start, current_end);
	}

	return call;
}

Atanor* AtanorCode::C_haskellcall(x_node* xn, Atanor* parent) {
	string name = xn->nodes[0]->value;

	if (xn->nodes[0]->token == "power") {
		short nm = globalAtanor->Getid(xn->nodes[1]->nodes[0]->value);
		short op = globalAtanor->string_operators[name];
		if (op == a_square)
			return new AtanorCallSQUARE(global, nm, parent);
		if (op == a_cube)
			return new AtanorCallCUBE(global, nm, parent);
	}

	short id = globalAtanor->Getid(name);

	//Then, we simply need a method...
	//We create a call method...
	//the parameter in this case is the object we need...

	if (globalAtanor->procedures.check(id)) {
		AtanorCallProcedure* call = new AtanorCallProcedure(id, global, parent);
		if (xn->nodes.size() == 2)
			Traverse(xn->nodes[1], call);
		call->CheckHaskellComposition();
		return call;
	}

	if (globalAtanor->commons.check(id)) {
		AtanorGetCommon* call = new AtanorGetCommon(id, global, parent);
		if (xn->nodes.size() != 2) {
			stringstream message;
			message << "Missing argument in: '" << name << "'";
			throw new AtanorRaiseError(message, filename, current_start, current_end);
		}
		Traverse(xn->nodes[1], call);
		call->CheckHaskellComposition();
		return call;
	}

	if (globalAtanor->allmethods.check(id)) {
		AtanorGetMethod* call = new AtanorGetMethod(id, global, parent);
		if (xn->nodes.size() != 2) {
			stringstream message;
			message << "Missing argument in: '" << name << "'";
			throw new AtanorRaiseError(message, filename, current_start, current_end);
		}
		Traverse(xn->nodes[1], call);
		call->CheckHaskellComposition();
		return call;
	}

	Atanor* call = Declaration(id);
	if (call != NULL) {
		short tyvar = call->Typevariable();
		if (tyvar == a_lambda) {			
			if (xn->nodes.size() == 2) {
				call = new AtanorCallFunctionArgsHaskell((AtanorFunctionLambda*)call, global, parent);
				Traverse(xn->nodes[1], call);
			}
			else
				call = new AtanorCallFunctionHaskell((AtanorFunctionLambda*)call, global, parent);

			if (!call->Checkarity()) {
				stringstream message;
				message << "Wrong number of arguments or incompatible argument: '" << name << "'";
				throw new AtanorRaiseError(message, filename, current_start, current_end);
			}
			call->CheckHaskellComposition();
			return call;
		}

		if (tyvar == a_function) {
			call = new AtanorCallFunction(call, global, parent);
			if (xn->nodes.size() == 2)
				Traverse(xn->nodes[1], call);

			if (!call->Checkarity()) {
				stringstream message;
				message << "Wrong number of arguments or incompatible argument: '" << name << "'";
				throw new AtanorRaiseError(message, filename, current_start, current_end);
			}
			call->CheckHaskellComposition();
			return call;
		}

		//second possibility, this is a simple variable...
		if (tyvar == a_self || tyvar == a_let || tyvar == a_call) {
			call = new AtanorGetFunction(id, global, parent);
			//We create a call method...
			//the parameter in this case is the object we need...
			if (xn->nodes.size() == 2)
				Traverse(xn->nodes[1], call);

			call->CheckHaskellComposition();
			return call;
		}

	}

	stringstream message;
	message << "Unknown function or procedure: '" << name << "'";
	throw new AtanorRaiseError(message, filename, current_start, current_end);
	return parent;
}

Atanor* AtanorCode::C_variable(x_node* xn, Atanor* parent) {
	string name = xn->nodes[0]->value;
	short idname = global->Getid(name);

	Atanor* av;
	short tyvar = 0;
	Atanor* a = aNULL;

	if (globalAtanor->systems.check(idname)) {
		switch (idname) {
		case a_null:
			av = aNULL;
			parent->AddInstruction(aNULL);
			break;
		case a_empty:
			av = aNOELEMENT;
			parent->AddInstruction(aNOELEMENT);
			break;
		case a_universal:
			av = aUNIVERSAL;
			parent->AddInstruction(aUNIVERSAL);
			break;
		default:
			av = new AtanorCallSystemVariable(idname, globalAtanor->systems[idname]->Typevariable(), global, parent);
		}
	}
	else {
		Atanor* dom = Declaror(idname);

		if (dom == NULL) {
			//It could be a procedure or a method
			if (globalAtanor->procedures.check(idname))
				av = new AtanorProcedureParameter(idname, global, parent);
			else
			if (globalAtanor->commons.check(idname))
				av = new AtanorCommonParameter(idname, global, parent);
			else
			if (globalAtanor->allmethods.check(idname))
				av = new AtanorMethodParameter(idname, global, parent);
			else
			if (globalAtanor->framemethods.check(idname))
				av = new AtanorFrameMethodParameter(idname, global, parent);
			else {
				stringstream message;
				message << "Unknown variable: '" << name << "'";
				throw new AtanorRaiseError(message, filename, current_start, current_end);
			}
		}
		else {
			if (idname == a_this)
				av = new AtanorCallThis(parent->Name(), global, parent);
			else {
				a = dom->Declaration(idname);
				//This is a variable call
				if (a->Type() == a_atanor)
					av = new AtanorCallAtanorVariable(idname, (Atanoratanor*)a, global, parent);
				else {
					tyvar = a->Typevariable();
					switch (tyvar) {
					case a_lambda:
						av = new AtanorFunctionHaskellParameter(idname, global, parent);
						break;
					case a_function:
						av = new AtanorFunctionParameter(idname, global, parent);
						break;
					case a_self:
					case a_let:
						av = new AtanorCallSelfVariable(idname, tyvar, global, parent);
						break;
					case a_intthrough:
					case a_longthrough:
					case a_decimalthrough:
					case a_floatthrough:
					case a_stringthrough:
					case a_ustringthrough:
					case a_vectorthrough:
					case a_mapthrough:
						av = new AtanorCallThroughVariable(idname, tyvar, parent);
						break;
					default:
						if (dom->isFrame() && dom != &mainframe) {
							if (parent->isCallVariable())
								av = new AtanorCallFromFrameVariable(idname, tyvar, global, parent);
							else
								av = new AtanorCallFrameVariable(idname, (AtanorFrame*)dom, tyvar, global, parent);
						}
						else
						if (dom->isFunction())
							av = new AtanorCallFunctionVariable(idname, tyvar, global, parent);
						else {
							if (a->isConstant())
								av = new AtanorCallConstantVariable(idname, tyvar, global, parent);
							else
								av = new AtanorCallVariable(idname, tyvar, global, parent);
						}
					}
				}
			}
		}
	}

	bool protect = false;
	if (global->newInstance.check(tyvar) && global->newInstance[tyvar]->isFrame()) {
		protect = true;
		globalAtanor->Pushstack(global->newInstance[tyvar]->Frame());
	}

	//we might have four cases then: interval, indexes, method or variable (in a frame)
	if (xn->nodes.size() != 1)
		Traverse(xn->nodes[1], av);

	if (protect)
		globalAtanor->Popstack();

	return av;
}

Atanor* AtanorCode::C_indexes(x_node* xn, Atanor* parent) {
	AtanorIndex* idx = new AtanorIndex(false, global, parent);
	AtanorInstruction ai;

	if (xn->nodes[0]->token == "minus") {
		idx->signleft = true;
		Traverse(xn->nodes[0]->nodes[0], &ai);
	}
	else
		Traverse(xn->nodes[0], &ai);

	idx->AddInstruction(ai.instructions[0]);

	if (xn->nodes.size() == 2)
		Traverse(xn->nodes[1], idx);

	return idx;
}

Atanor* AtanorCode::C_interval(x_node* xn, Atanor* parent) {
	size_t xsz = xn->nodes.size();

	AtanorIndex* idx = new AtanorIndex(true, global, parent);

	//If we have another function at the end of the structure, we do not want to take it into account
	if (TestFunction(xn->nodes[xsz - 1]->token, false))
		xsz--;

	if (xsz == 1) {
		idx->interval = true;
		idx->AddInstruction(aZERO);
		idx->AddInstruction(aNULL);
	}
	else {
		bool beforesep = true;
		for (int i = 0; i < xsz; i++) {
			if (xn->nodes[i]->token == "sep") {
				beforesep = false;
				if (idx->left == aNOELEMENT)
					idx->left = aNULL;
				continue;
			}

			AtanorInstruction ai;

			if (xn->nodes[i]->token == "minus") {
				if (beforesep)
					idx->signleft = true;
				else
					idx->signright = true;
				Traverse(xn->nodes[i]->nodes[0], &ai);
			}
			else
				Traverse(xn->nodes[i], &ai);
			idx->AddInstruction(ai.instructions[0]);
		}
		if (!beforesep && idx->right == NULL)
			idx->right = aNULL;
	}

	//If we have a call
	if (xsz != xn->nodes.size()) {
		idx->interval = false;
		Traverse(xn->nodes[xsz], idx);
		idx->interval = true;
	}

	return idx;
}

Atanor* AtanorCode::C_astring(x_node* xn, Atanor* parent) {
	string thestr;

	if (xn->token == "astring" || xn->token == "punct")
		thestr = xn->value;
	else
	if (xn->token == "astringdouble")
		EvaluateMetaString(thestr, xn->value.substr(1, xn->value.size() - 2));
	else
		thestr = xn->value.substr(1, xn->value.size() - 2);

	//compilemode indicates whether the code is compiled from a file or from a string while executing some code...
	if (compilemode)
		return new AtanorConstString(thestr, global, parent);

	return new Atanorstring(thestr, NULL, parent);
}

Atanor* AtanorCode::C_ustring(x_node* xn, Atanor* parent) {
	string thestr;

	if (xn->token == "ustringdouble")
		EvaluateMetaString(thestr, xn->value.substr(1, xn->value.size() - 2));
	else
		thestr = xn->value.substr(1, xn->value.size() - 2);

	wstring res;
	s_utf8_to_unicode(res, STR(thestr));

	//compilemode indicates whether the code is compiled from a file or from a string while executing some code...
	if (compilemode)
		return new AtanorConstUString(res, global, parent);

	return new Atanorustring(res, NULL, parent);
}

Atanor* AtanorCode::C_anumber(x_node* xn, Atanor* parent) {
	string& name = xn->value;
	Atanor* kv = NULL;
	BLONG v;
	if (name.find(".") == -1) {
		v = conversionintegerhexa(STR(name));

		switch (v) {
		case -10: kv = aMINUSTEN; break;
		case -9: kv = aMINUSNINE; break;
		case -8: kv = aMINUSEIGHT; break;
		case -7: kv = aMINUSSEVEN; break;
		case -6: kv = aMINUSSIX; break;
		case -5: kv = aMINUSFIVE; break;
		case -4: kv = aMINUSFOUR; break;
		case -3: kv = aMINUSTHREE; break;
		case -2: kv = aMINUSTWO; break;
		case -1: kv = aMINUSONE; break;
		case 0: kv = aZERO; break;
		case 1: kv = aONE; break;
		case 2: kv = aTWO; break;
		case 3: kv = aTHREE; break;
		case 4: kv = aFOUR; break;
		case 5: kv = aFIVE; break;
		case 6: kv = aSIX; break;
		case 7: kv = aSEVEN; break;
		case 8: kv = aEIGHT; break;
		case 9: kv = aNINE; break;
		case 10: kv = aTEN; break;
		default:
			if (globalAtanor->numbers.find(v) != globalAtanor->numbers.end())
				kv = globalAtanor->numbers[v];
		}

		if (kv != NULL) {
			parent->AddInstruction(kv);
			return kv;
		}
		if (compilemode) {
			if (IsLong(v))
				kv = new AtanorConstLong(v, global, parent);
			else
			if (IsShort(v))
				kv = new AtanorConstShort((short)v, global, parent);
			else
				kv = new AtanorConstInt((long)v, global, parent);

			
			globalAtanor->numbers[v] = kv;
			return kv;
		}

		if (IsLong(v))
			return new Atanorlong(v, NULL, parent);

		if (IsShort(v))
			kv = new Atanorshort((short)v, global, parent);

		return new Atanorint((long)v, NULL, parent);
	}


	if (compilemode)
		return new AtanorConstFloat(convertfloat(STR(name)), global, parent);
	return new Atanorfloat(convertfloat(STR(name)), NULL, parent);
}

Atanor* AtanorCode::C_axnumber(x_node* xn, Atanor* parent) {
	string& name = xn->value;
	BLONG v = conversionintegerhexa(STR(name));
	Atanor* kv = NULL;
	switch (v) {
	case -10: kv = aMINUSTEN; break;
	case -9: kv = aMINUSNINE; break;
	case -8: kv = aMINUSEIGHT; break;
	case -7: kv = aMINUSSEVEN; break;
	case -6: kv = aMINUSSIX; break;
	case -5: kv = aMINUSFIVE; break;
	case -4: kv = aMINUSFOUR; break;
	case -3: kv = aMINUSTHREE; break;
	case -2: kv = aMINUSTWO; break;
	case -1: kv = aMINUSONE; break;
	case 0: kv = aZERO; break;
	case 1: kv = aONE; break;
	case 2: kv = aTWO; break;
	case 3: kv = aTHREE; break;
	case 4: kv = aFOUR; break;
	case 5: kv = aFIVE; break;
	case 6: kv = aSIX; break;
	case 7: kv = aSEVEN; break;
	case 8: kv = aEIGHT; break;
	case 9: kv = aNINE; break;
	case 10: kv = aTEN; break;
	default:
		if (globalAtanor->numbers.find(v) != globalAtanor->numbers.end())
			kv = globalAtanor->numbers[v];
	}

	if (kv != NULL) {
		parent->AddInstruction(kv);
		return kv;
	}

	if (compilemode) {
		if (IsLong(v))
			kv = new AtanorConstLong(v, global, parent);
		else
		if (IsShort(v))
			kv = new AtanorConstShort((short)v, global, parent);
		else
			kv = new AtanorConstInt((long)v, global, parent);

		if (!parent->isContainer())
			globalAtanor->numbers[v] = kv;

		return kv;
	}

	if (IsLong(v))
		return new Atanorlong(v, NULL, parent);
	
	if (IsShort(v))
		return new Atanorshort((short)v, NULL, parent);

	return new Atanorint((long)v, NULL, parent);
}


Atanor* AtanorCode::C_constant(x_node* xn, Atanor* kf) {
	if (xn->value == "true") {
		kf->AddInstruction(aTRUE);
		return aTRUE;
	}
	if (xn->value == "false") {
		kf->AddInstruction(aFALSE);
		return aFALSE;
	}
	if (xn->value == "null") {
		kf->AddInstruction(aNULL);
		return aNULL;
	}
	return kf;
}


Atanor* AtanorCode::C_intentionvector(x_node* xn, Atanor* kf) {
	//We will rewrite this instruction into a call to "range"...
	x_node* nstep = NULL;
	if (xn->nodes.back()->token == "step") {
		nstep = xn->nodes.back();
		xn->nodes.pop_back();
	}

	if (xn->nodes.size() == 3) {
		x_node* nop = new x_node("regularcall", "", xn);		
		x_node* nfunc = creationxnode("functioncall", "range", nop);
		creationxnode("word", "range", nfunc);
		x_node* param = creationxnode("parameters", "", nop);

		param->nodes.push_back(xn->nodes[0]);
		param->nodes.push_back(xn->nodes[2]);
		

		if (nstep != NULL)
			//we add the step
			param->nodes.push_back(nstep);

		AtanorInstruction ai;
		Traverse(nop, &ai);
		bool getvect = true;
		AtanorCall* args = (AtanorCall*)ai.instructions[0];
		for (short i = 0; i < args->arguments.size(); i++) {
			if (!args->arguments[i]->isConst()) {
				getvect = false;
				break;
			}
		}
		if (getvect) {
			//If the intention vector is only composed of const values, we can evaluate it now...
			Atanor* kvect = ai.instructions[0]->Get(aNULL, aNULL, 0);
			if (kvect == aNOELEMENT)
				kf->AddInstruction(ai.instructions[0]);
			else {
				//We set it to const to prevent any modification of it...
				kvect->Setreference();
				kvect->SetConst();
				kf->AddInstruction(kvect);
				ai.instructions[0]->Remove();
			}
		}
		else
			kf->AddInstruction(ai.instructions[0]);
		param->nodes.clear();
		delete nop;
		return kf;
	}

	Atanor* kinfvect;
	if (xn->nodes[0]->token == "intentionsep") {
		kinfvect = new AtanorInfinitevector(-1, global, kf);
		Traverse(xn->nodes[1], kinfvect);
	}
	else {
		kinfvect = new AtanorInfinitevector(1, global, kf);
		Traverse(xn->nodes[0], kinfvect);
	}

	if (nstep != NULL) {
		Traverse(nstep, kinfvect);
		delete nstep;
	}

	return kinfvect;
}


Atanor* AtanorCode::C_intentionwithdouble(x_node* xn, Atanor* kf) {
	//Different types of expressions: [..x,y]
	AtanorInfinitevector* kinfvect;
	if (xn->nodes[0]->token == "intentionsep") {
		kinfvect = new AtanorInfinitevector(-1, global, kf);
		kinfvect->compute = true;
		//The initial value
		Traverse(xn->nodes[2], kinfvect);
		//The next value from which the step will be computed...
		Traverse(xn->nodes[1], kinfvect);
		return kinfvect;
	}

	//[x,y..]
	if (xn->nodes.back()->token == "intentionsep") {
		kinfvect = new AtanorInfinitevector(1, global, kf);
		kinfvect->compute = true;
		//The initial value
		Traverse(xn->nodes[0], kinfvect);
		//The next value from which the step will be computed...
		Traverse(xn->nodes[1], kinfvect);
		return kinfvect;
	}

	//[x,y..z]

	x_node* nop = new x_node("regularcall", "", xn);
	
	x_node* nfunc = creationxnode("functioncall", "range", nop);
	creationxnode("word", "range", nfunc);

	x_node* param = creationxnode("parameters", "", nop);
	param->nodes.push_back(xn->nodes[0]);
	param->nodes.push_back(xn->nodes[3]);

	AtanorCallProcedure* kcf;
	Atanor* kret = Traverse(nop, kf);
	kret->Addargmode();
	kcf = (AtanorCallProcedure*)kret;
	bool getvect = true;
	for (short i = 0; i < kcf->arguments.size(); i++) {
		if (!kcf->arguments[i]->isConst()) {
			getvect = false;
			break;
		}
	}
	

	AtanorInstructionAPPLYOPERATIONROOT* kroot = new AtanorInstructionAPPLYOPERATIONROOT(global, kret);
	AtanorInstruction ki;
	
	short idord = 0;

	Traverse(xn->nodes[0], &ki);
	if (ki.instructions[0]->isString()) {
		idord = globalAtanor->Getid("ord");
		kcf = new AtanorCallProcedure(idord, global);
		kcf->arguments.push_back(ki.instructions[0]);
		kroot->instructions.push_back(kcf);
	}
	else
		kroot->Stacking(ki.instructions[0], false);
	
	ki.instructions.clear();
	Traverse(xn->nodes[1], &ki);
	if (!idord)
		kroot->Stacking(ki.instructions[0], false);
	else {
		kcf = new AtanorCallProcedure(idord, global);
		kcf->arguments.push_back(ki.instructions[0]);
		kroot->instructions.push_back(kcf);
	}

	kroot->instructions.push_back(globalAtanor->actions[a_minus]);
	kroot->Setsize();
	if (getvect && ki.instructions[0]->isConst()) {
		//If the intention vector is only composed of const values, we can evaluate it now...
		Atanor* kvect = kret->Get(aNULL, aNULL, 0);
		if (kvect != aNOELEMENT) {
			kvect->Setreference();
			kvect->SetConst(); //we set to const to prevent any modification...
			kf->InstructionRemoveLast();
			kf->AddInstruction(kvect);
			kret->Remove();
			kroot->Remove();
		}

	}

	param->nodes.clear();
	delete nop;
	return kf;

}



Atanor* AtanorCode::C_valvector(x_node* xn, Atanor* kf) {
	int i;

	if (!compilemode) {
		Atanorvector* kvect = new Atanorvector(NULL, kf);
		for (i = 0; i < xn->nodes.size(); i++) {
			if (xn->nodes[i]->nodes.size() == 1)
				Traverse(xn->nodes[i], kvect);
			else {
				AtanorInstruction ki;
				Traverse(xn->nodes[i], &ki);
				kvect->AddInstruction(ki.instructions[0]);
			}
		}
		return kvect;
	}


	//First, we check if we are in an assignement...
	short vartype = 0;
	if (kf->Action() == a_affectation) {
		if (kf->InstructionSize()) {
			Atanor* ins = kf->Instruction(0);
			if (globalAtanor->newInstance[ins->Typevariable()]->isValueContainer()) {
				vartype = ins->Typevariable();
			}
		}
	}

	AtanorConstvector* kvect = new AtanorConstvector(global);

	if (xn->token == "valpredicatevector" || xn->token == "valvectortail") {
		for (i = 0; i < xn->nodes.size(); i++) {
			if (xn->nodes[i]->token == "valtail") {
				AtanorConstvectormerge* kbv = new AtanorConstvectormerge(global, NULL);
				kvect->values.push_back(kbv);
				Traverse(xn->nodes[i], kbv);
			}
			else
				Traverse(xn->nodes[i], kvect);
		}
	}
	else {
		for (i = 0; i < xn->nodes.size(); i++) {
			if (xn->nodes[i]->nodes.size() == 1)
				Traverse(xn->nodes[i], kvect);
			else {
				AtanorInstruction ki;
				Traverse(xn->nodes[i], &ki);
				kvect->AddInstruction(ki.instructions[0]);
			}
		}
	}

	bool duplicate = true;
	kvect->evaluate = false;
	Atanor* a;
	for (i = 0; i < kvect->values.size(); i++) {
		a = kvect->values[i];
		if (!a->baseValue()) {
			kvect->evaluate = true;
			duplicate = false;
			break;
		}
		if (!a->isConst()) {
			duplicate = false;
			continue;
		}
	}

	
	if (vartype && duplicate) {
		Atanor* v = globalAtanor->newInstance[vartype]->Newinstance(0);
		v->SetConst();
		v->Setreference();
		for (long i = 0; i < kvect->values.size(); i++)
			v->Push(kvect->values[i]);
		kvect->Remove();
		kf->AddInstruction(v);
		return v;
	}
	
	kf->AddInstruction(kvect);
	return kvect;
}

Atanor* AtanorCode::C_valmap(x_node* xn, Atanor* kf) {
	AtanorConstmap* kmap;
	long i;

	if (xn->token == "maptail" || xn->token == "mapmerging") {
		kmap = (AtanorConstmap*)kf;
		kmap->keys.push_back(aPIPE);
		AtanorInstruction kbloc;
		Traverse(xn->nodes[0], &kbloc);
		kmap->values.push_back(kbloc.instructions[0]);
		kmap->Setevaluate(true);
		return kf;
	}

	//First, we check if we are in an assignement...
	short vartype = 0;
	if (kf->Action() == a_affectation) {
		if (kf->InstructionSize()) {
			Atanor* ins = kf->Instruction(0);
			if (globalAtanor->newInstance[ins->Typevariable()]->isValueContainer()) {
				vartype = ins->Typevariable();
			}
		}
	}

	kmap = new AtanorConstmap(global);
	for (i = 0; i < xn->nodes.size(); i++)
		Traverse(xn->nodes[i], kmap);

	Atanor* val;
	Atanor* key;
	bool duplicate = true;
	uchar types = 0;
	if (!kmap->isEvaluate()) {
		for (i = 0; i < kmap->values.size(); i++) {
			val = kmap->values[i];
			key = kmap->keys[i];
			if (!key->baseValue() || !val->baseValue()) {
				kmap->Setevaluate(true);
				duplicate = false;
				types = 0;
				break;
			}

			if (!key->isConst() || !val->isConst()) {
				duplicate = false;
				types = 0;
				continue;
			}
			types |= Returnequ(key->Type());
		}
	}
	
	if (types && !vartype) {
		//in that case, if all keys are numbers, well we need to take this into account...
		if ((types&b_allnumbers) == types) {
			if (types & b_floats)
				types = b_float;
			else {
				if (types&b_long)
					types = b_long;
				else
					types = b_int;
			}
			Atanor* m = globalAtanor->mapnewInstances[types][b_none]->Newinstance(0);
			m->SetConst();
			m->Setreference();
			for (long i = 0; i < kmap->keys.size(); i++)
				m->Push(kmap->keys[i], kmap->values[i]);
			kmap->Remove();
			kf->AddInstruction(m);
			return m;
		}
	}

	//If we have a map, which can be directly evaluated at this moment (no variable in the map for instance) and we know the type
	//of the variable to store it in, we do it now...	
	if (duplicate && vartype) {
		Atanor* m = globalAtanor->newInstance[vartype]->Newinstance(0);
		m->SetConst();
		m->Setreference();
		for (long i = 0; i < kmap->keys.size(); i++)
			m->Push(kmap->keys[i], kmap->values[i]);
		kmap->Remove();
		kf->AddInstruction(m);
		return m;
	}

	kf->AddInstruction(kmap);

	return kmap;
}


Atanor* AtanorCode::C_jsondico(x_node* xn, Atanor* kf) {
	AtanorInstruction kbloc;

	//First the key
	Traverse(xn->nodes[0], &kbloc);
	string key = kbloc.instructions[0]->String();

	kbloc.instructions[0]->Remove();
	kbloc.instructions.clear();

	//then the value
	Traverse(xn->nodes[1], &kbloc);
	Atanor* val = kbloc.instructions[0];

	kf->push(key, val);
	return kf;
}

Atanor* AtanorCode::C_features(x_node* xn, Atanor* kf) {
	AtanorInstruction kbloc;

	Atanormapss* kmap = new Atanormapss(global, kf);
	for (int i = 0; i < xn->nodes.size(); i++) {
		x_node* sub = xn->nodes[i];
		string key;
		string val;
		if (sub->nodes[0]->token == "word")
			key = sub->nodes[0]->value;
		else {
			key = sub->nodes[0]->value;
			if (sub->nodes[0]->token == "afullstring")
				key = key.substr(2, key.size() - 3);
			else
				key = key.substr(1, key.size() - 2);
		}

		if (Atanorsynode::Checkattribute(key) == false) {
			stringstream message;
			message << "Unknown attribute: '" << key << "'";
			throw new AtanorRaiseError(message, filename, current_start, current_end);
		}

		if (sub->nodes.size() == 1)
			kmap->values[key] = "+";
		else {
			bool rgx = false;
			if (sub->nodes[1]->value == "=") {

				if (featureassignment == 2) {
					stringstream message;
					message << "Cannot assign a feature to a dependency node";
					throw new AtanorRaiseError(message, filename, current_start, current_end);
				}

				key = "=" + key;
			}
			else
			if (sub->nodes[1]->value == "~:")
				key = "~" + key;
			else
			if (sub->nodes[1]->value == "-:") {
				key = ":" + key;
				rgx = true;
			}
			else
			if (sub->nodes[1]->value == "~-:") {
				key = ":~" + key;
				rgx = true;
			}


			if (sub->nodes[2]->value == "~")
				kmap->values[key] = sub->nodes[2]->value;
			else {
				if (sub->nodes[2]->token == "word" || sub->nodes[2]->token == "valplus")
					val = sub->nodes[2]->value;
				else {
					Traverse(sub->nodes[2], &kbloc);
					val = kbloc.instructions[0]->String();
				}
				if (Atanorsynode::Checkfeature(key, val) == false) {
					stringstream message;
					message << "Unknown attribute/value: '" << key << "'/'" << val << "'";
					throw new AtanorRaiseError(message, filename, current_start, current_end);
				}

				if (rgx) {
					if (val == "") {
						stringstream message;
						message << "Empty string cannot be used as a regular expression: '" << key << "'";
						throw new AtanorRaiseError(message, filename, current_start, current_end);
					}

					global->rules[val] = new automate(USTR(val), (void*)1);
				}

				kmap->values[key] = val;
				kbloc.instructions.clear();
			}
		}
	}
	return kmap;
}

Atanor* AtanorCode::C_declarationhaskell(x_node* xn, Atanor* kf) {
	//The first element is a list of types, the second is the type output...

	string arg;
	short ty;
	AtanorFunctionLambda* klambda = (AtanorFunctionLambda*)kf;

	long sz = xn->nodes.size() - 1;
	for (long i = 0; i <= sz; i++) {
		if (xn->nodes[i]->token == "word") {
			arg = xn->nodes[i]->value;
			ty = global->Getid(arg);
			if (ty != a_universal && global->newInstance.check(ty) == false) {
				stringstream message;
				message << "Unknown type: '" << arg << "'";
				throw new AtanorRaiseError(message, filename, current_start, current_end);
			}
			if (i == sz) {
				klambda->returntype = ty;
				if (ty == a_universal)
					klambda->returntype = a_null;
			}
			else
				klambda->haskelldeclarations.push_back(new Haskelldeclaration(ty));
			continue;
		}

		arg = xn->nodes[i]->nodes[0]->nodes[0]->value;
		ty = global->Getid(arg);
		if (ty != a_universal && global->newInstance.check(ty) == false) {
			stringstream message;
			message << "Unknown type: '" << arg << "'";
			throw new AtanorRaiseError(message, filename, current_start, current_end);
		}
		if (i == sz) {
			klambda->returntype = ty;
			if (ty == a_universal)
				klambda->returntype = a_null;
		}
		else {
			SubHaskelldeclaration* sub = new SubHaskelldeclaration(ty);
			for (long j = 1; j < xn->nodes[i]->nodes[0]->nodes.size(); j++) {
				arg = xn->nodes[i]->nodes[0]->nodes[j]->value;
				ty = global->Getid(arg);
				if (ty != a_universal && global->newInstance.check(ty) == false) {
					stringstream message;
					message << "Unknown type: '" << arg << "'";
					throw new AtanorRaiseError(message, filename, current_start, current_end);
				}
				sub->Push(ty);
			}
			klambda->haskelldeclarations.push_back(sub);
		}
	}

	klambda->hdeclared = true;

	return kf;
}

Atanor* AtanorCode::C_dico(x_node* xn, Atanor* kf) {
	AtanorInstruction kbloc;

	//First the key
	Traverse(xn->nodes[0], &kbloc);
	Atanor* key = kbloc.instructions[0];
	kbloc.instructions.clear();

	//then the value
	Traverse(xn->nodes[1], &kbloc);
	Atanor* val = kbloc.instructions[0];

	AtanorConstmap* kmap = (AtanorConstmap*)kf;
	kmap->keys.push_back(key);
	kmap->values.push_back(val);
	if (xn->nodes.size() == 3)
		Traverse(xn->nodes[2], kf);
	return kf;
}


Atanor* AtanorCode::C_jsonmap(x_node* xn, Atanor* kf) {
	Atanormap* kmap = new Atanormap(NULL, kf);
	for (int i = 0; i < xn->nodes.size(); i++)
		Traverse(xn->nodes[i], kmap);
	return kmap;
}

Atanor* AtanorCode::C_jsonvector(x_node* xn, Atanor* kf) {
	Atanorvector* kvect = new Atanorvector(NULL, kf);
	for (int i = 0; i < xn->nodes.size(); i++)
		Traverse(xn->nodes[i], kvect);
	return kvect;
}

Atanor* AtanorCode::C_affectation(x_node* xn, Atanor* kf) {
	AtanorInstruction* ki = AtanorCreateInstruction(NULL, a_affectation);

	for (size_t i = 0; i < xn->nodes.size(); i++)
		Traverse(xn->nodes[i], ki);

	
	if (ki->InstructionSize()) {
		if (ki->Instruction(0)->isConstant()) {
			stringstream message;
			message << "You cannot modify this constant variable";
			throw new AtanorRaiseError(message, filename, current_start, current_end);
		}
	}

	short act = ki->Action();
	Atanor* kfirst;
	Atanor* func;
	if (act != a_affectation && act != a_forcedaffectation) {
		AtanorInstruction* k = AtanorCreateInstruction(kf, act);
		ki->Clone(k);
		ki->Remove();
		ki = k;
		kfirst = ki->Instruction(0);
		func = kfirst->Function();

		if (act == a_stream)
			ki->Instruction(0)->Setaffectation(true);
		else
		if (ki->isEQU()) {
			ki->Instruction(0)->Setaffectation(true);
			AtanorInstructionAPPLYOPERATIONEQU* kequ = (AtanorInstructionAPPLYOPERATIONEQU*)ki;
			kequ->action = globalAtanor->equto[kequ->action];
			AtanorInstructionAPPLYOPERATIONEQU ktmp(NULL);
			ktmp.action = kequ->action;
			for (int i = 1; i < kequ->instructions.size(); i++)
				ktmp.instructions.push_back(kequ->instructions[i]);
			kequ->recipient = kequ->instructions[0];
			kequ->thetype = Evaluateatomtype(kequ->recipient);
			kequ->instructions.clear();
			kequ->Stacking(&ktmp, true);
			kequ->Setsize();
			kequ->instruction = kequ->Returnlocal(global);
			if (kequ->instruction == kequ) {
				AtanorInstructionAPPLYOPERATIONROOT* kroot = new AtanorInstructionAPPLYOPERATIONROOT(global);
				kequ->instruction = kroot;
				kroot->thetype = kequ->thetype;
				kroot->head = kroot->head;
				kroot->instructions = kequ->instructions;
				((AtanorInstructionAPPLYOPERATIONROOT*)kequ->instruction)->Setsize();
			}
		}
	}
	else {
		if (act == a_affectation || act == a_stream) {
			kfirst = ki->Instruction(1);
			//If the first operation has not been converted into a ROOT, we do it here...
			//For instance, i=-i, where -i is converted into -1*i, does not always go through the conversion process...
			if (kfirst->isOperation()) {
				//we need then to recast it into an actual operation root
				Atanor* kroot = kfirst->Compile(ki->Instruction(0));
				kfirst->Remove();
				ki->Putinstruction(1, kroot);
			}
		}

		kfirst = ki->Instruction(0);
		func = kfirst->Function();
		if (func == NULL && act == a_affectation) {
			func = new AtanorInstructionSIMPLEAFFECTATION(global, kf);
			((AtanorInstructionSIMPLEAFFECTATION*)func)->instructions = ki->instructions;
			ki->Remove();
			ki = (AtanorInstruction*)func;
		}
		else
			kf->AddInstruction(ki);

		ki->Instruction(0)->Setaffectation(true);
		if (act == a_forcedaffectation)
			ki->Instruction(0)->Setforced(true);
	}

	if (!kfirst->isAssignable()) {
		stringstream message;
		message << "Cannot assign a value in this configuration";
		if (func != NULL)
			message << ": '" << globalAtanor->Getsymbol(func->Name()) << "'";
		else {
			if (kfirst->Name() > 1)
				message << ", : '" << globalAtanor->Getsymbol(kfirst->Name()) << "'";
		}
		throw new AtanorRaiseError(message, filename, current_start, current_end);
	}
	kfirst->Setstopindex();
	return ki;
}

Atanor* AtanorCode::C_operator(x_node* xn, Atanor* parent) {
	short op = globalAtanor->string_operators[xn->value];
	parent->Setaction(op);
	return parent;
}


Atanor* AtanorCode::C_plusplus(x_node* xn, Atanor* kf) {
	Atanor* ki;
	long sz;
	short op = globalAtanor->string_operators[xn->value];
	switch (op) {
	case a_plusplus:
		return new AtanorPLUSPLUS(global, kf);
	case a_minusminus:
		return new AtanorMINUSMINUS(global, kf);
	case a_square:
		if (kf->isInstruction()) {
			sz = kf->InstructionSize() - 1;
			ki = new AtanorInstructionAPPLYOPERATION(global, NULL);
			ki->Setaction(a_power);
			ki->AddInstruction(kf->Instruction(sz));
			ki->AddInstruction(aTWO);
			kf->Putinstruction(sz, ki);
			return kf;
		}
		return new AtanorSQUARE(global, kf);
	case a_cube:
		if (kf->isInstruction()) {
			sz = kf->InstructionSize() - 1;
			ki = new AtanorInstructionAPPLYOPERATION(global, NULL);
			ki->Setaction(a_power);
			ki->AddInstruction(kf->Instruction(sz));
			ki->AddInstruction(aTHREE);
			kf->Putinstruction(sz, ki);
			return kf;
		}
		return new AtanorCUBE(global, kf);
	}
	return kf;
}

Atanor* AtanorCode::C_increment(x_node* xn, Atanor* kf) {
	AtanorInstruction ki;
	Traverse(xn->nodes[0], &ki);
	Traverse(xn->nodes[1], ki.instructions[0]);
	kf->AddInstruction(ki.instructions[0]);
	return kf;
}


Atanor* AtanorCode::C_multiply(x_node* xn, Atanor* kf) {
	short op = a_multiply;
	//The second parameter is the rest of the operation
	//kf is the TOP instruction
	AtanorInstruction* ki;
	if (kf->Action() == a_bloc) {
		//We are in a new bloc, which is our current element
		//In this case, we create a new level
		ki = AtanorCreateInstruction(NULL, op);
		//It becomes the new element
		ki->AddInstruction(kf->Instruction(0));
		kf->Putinstruction(0, ki);
		ki->Addparent(kf);
		Traverse(xn->nodes[0], ki);
		if (xn->nodes.size() == 2)
			Traverse(xn->nodes[1], ki);

		ki->Subcontext(true);
		return ki;
	}

	if (kf->Action() == op) {
		Traverse(xn->nodes[0], kf);
		if (xn->nodes.size() == 2)
			Traverse(xn->nodes[1], kf);
		return kf;
	}


	//we create a new level
	ki = AtanorCreateInstruction(NULL, op);
	ki->AddInstruction(kf->Lastinstruction());
	kf->Putinstruction(kf->InstructionSize() - 1, ki);
	ki->Addparent(kf);
	Traverse(xn->nodes[0], ki);
	if (xn->nodes.size() == 2)
		Traverse(xn->nodes[1], ki);


	//we can merge in this case the content of ki into kf
	if (ki->Action() == kf->Action()) {
		size_t sz = kf->InstructionSize() - 1;
		kf->InstructionRemove(sz);
		for (size_t i = 0; i < ki->InstructionSize(); i++)
			kf->Putinstruction(i + sz, ki->Instruction(i));
		ki->Remove();
	}

	short act = kf->Action();
	if ((act == a_none || act == a_stream || act == a_affectation) && kf->InstructionSize() >= 1) {
		Atanor* kroot;
		if (act != a_none)
			kroot = ki->Compile(kf->Instruction(0));
		else
			kroot = ki->Compile(NULL);

		kf->InstructionRemoveLast();
		kf->AddInstruction(kroot);
		ki->Remove();
	}

	return kf;
}

Atanor* AtanorCode::C_operation(x_node* xn, Atanor* kf) {
	//The first parameter is the operator	
	short op = globalAtanor->string_operators[xn->nodes[0]->value];
	//The second parameter is the rest of the operation
	//kf is the TOP instruction
	AtanorInstruction* ki;
	if (kf->Action() == a_bloc) {
		//We are in a new bloc, which is our current element
		//In this case, we create a new level
		ki = AtanorCreateInstruction(NULL, op);
		//It becomes the new element
		ki->AddInstruction(kf->Instruction(0));
		Traverse(xn->nodes[1], ki);
		ki->Subcontext(true);
		kf->Putinstruction(0, ki);
		ki->Addparent(kf);
		return ki;
	}

	if (kf->Action() == op) {
		Traverse(xn->nodes[1], kf);
		return kf;
	}

	//In this case, the operator is not the same
	//we still have two cases: if it is
	//if it is a PLUS or a MINUS, we reset the top node with the new information
	//It has to be in the middle of an operation
	Atanor* kloop;

	if (globalAtanor->atanOperatorMath.check(kf->Action())) {
		if (op == a_plus || op == a_minus || op == a_merge) {
			kloop = kf;
			short kact = kloop->Action();
			while (kloop != NULL && globalAtanor->atanOperatorMath.check(kact) && kact != a_plus && kact != a_minus && kact != a_merge) {
				kloop = kloop->Parent();
				if (kloop != NULL)
					kact = kloop->Action();
			}

			if (kloop == NULL || globalAtanor->atanOperatorMath.check(kloop->Action()) == false)
				kloop = kf;
			else {
				if (kloop->Action() == op) {
					Traverse(xn->nodes[1], kloop);
					return kloop;
				}
			}

			ki = AtanorCreateInstruction(NULL, kloop->Action());
			for (size_t i = 0; i < kloop->InstructionSize(); i++)
				ki->instructions.push_back(kloop->Instruction(i));
			kloop->Setaction(op);
			kloop->InstructionClear();
			kloop->Putinstruction(0, ki);
			ki->Addparent(kf);
			Traverse(xn->nodes[1], kloop);
			return kloop;
		}
	}

	//we create a new level
	ki = AtanorCreateInstruction(NULL, op);
	ki->AddInstruction(kf->Lastinstruction());
	kf->Putinstruction(kf->InstructionSize() - 1, ki);
	ki->Addparent(kf);
	Traverse(xn->nodes[1], ki);

	//we can merge in this case the content of ki into kf
	if (ki->Action() == kf->Action()) {
		size_t sz = kf->InstructionSize() - 1;
		kf->InstructionRemove(sz);
		for (size_t i = 0; i < ki->InstructionSize(); i++)
			kf->Putinstruction(i + sz, ki->Instruction(i));
		ki->Remove();
	}

	short act = kf->Action();
	if ((act == a_none || act == a_stream || act == a_affectation) && kf->InstructionSize() >= 1) {
		Atanor* kroot;
		if (act != a_none)
			kroot = ki->Compile(kf->Instruction(0));
		else
			kroot = ki->Compile(NULL);

		kf->InstructionRemoveLast();
		kf->AddInstruction(kroot);
		ki->Remove();
	}
	return kf;
}


Atanor* AtanorCode::C_comparison(x_node* xn, Atanor* kf) {
	//The first parameter is the operator
	Atanor* ki = kf;
	short op = globalAtanor->string_operators[xn->nodes[0]->value];
	if (kf->Action() == a_blocboolean || kf->Action() == a_bloc)
		ki->Setaction(op);
	else
	if (kf->Action() >= a_plus && kf->Action() <= a_and) {
		Atanor* kparent = kf->Parent();
		while (kparent != NULL) {
			if (kparent->Action() == a_blocboolean || kparent->Action() == a_bloc) {
				kparent->Setaction(op);
				ki = kparent;
				break;
			}
			kparent = kparent->Parent();
		}
		if (kparent == NULL) {
			kparent = kf->Parent();
			ki = AtanorCreateInstruction(kparent, op);
		}
	}
	else
	if (kf->Action() == a_predicateelement) {
		ki = AtanorCreateInstruction(NULL, op);
		//A modifier pour PROLOG
		((AtanorInstruction*)ki)->instructions.push_back(((AtanorPredicateRuleElement*)kf)->instructions[0]);
		((AtanorPredicateRuleElement*)kf)->instructions[0] = ki;
	}
	else
		ki = AtanorCreateInstruction(kf, op);

	for (size_t i = 0; i < xn->nodes.size(); i++)
		Traverse(xn->nodes[i], ki);

	if (ki->Type() == a_sequence && ki->InstructionSize() == 2 && (ki->Instruction(1)->baseValue() || ki->Instruction(1)->isConst())) {
		//Then we invert the comparison to force with the const to compare with itself
		//We do not want 0==empty for instance to be true...
		AtanorInstruction* kinst = (AtanorInstruction*)ki;
		kf = kinst->instructions[1];
		if (kf->Name() == a_empty) {
			kinst->instructions.pop_back();
			kinst->instructions.insert(0, kf);
		}
	}

	return ki;
}

Atanor* AtanorCode::C_negated(x_node* xn, Atanor* kf) {

	Atanor* ki = AtanorCreateInstruction(NULL, a_multiply);
	ki->AddInstruction(aMINUSONE);
	Traverse(xn->nodes[0], ki);
	Atanor* kroot = ki->Compile(NULL);
	ki->Remove();
	kf->AddInstruction(kroot);
	return kroot;
}


Atanor* AtanorCode::C_uniquecall(x_node* xn, Atanor* kf) {
	string& name = xn->value;
	//Looking if it is known as function
	Atanor* kcf = NULL;

	if (name == "break")
		kcf = new AtanorBreak(global, kf);
	else
	if (name == "continue")
		kcf = new AtanorContinue(global, kf);
	else
	if (name == "return" || name == "_return")
		kcf = new AtanorCallReturn(global, kf);

	return kcf;
}



Atanor* AtanorCode::C_operationin(x_node* xn, Atanor* kf) {
	//The first parameter is the operator
	AtanorInstruction* kinst;
	short kcurrentop = globalAtanor->string_operators[xn->nodes[0]->token];
	if (kf->Action() == a_blocboolean) {
		kinst = (AtanorInstruction*)kf;
		kinst->action = kcurrentop;
	}
	else {
		Atanor* last;
		//In this case, we replace the previous last element in kf with this one
		short kact = kf->Action();
		if (kact == a_affectation || (kact >= a_plusequ && kact <= a_andequ)) {
			last = kf->InstructionRemoveLast();
			kinst = AtanorCreateInstruction(kf, kcurrentop);
			kinst->instructions.push_back(last);
		}
		else {
			if (kf->Parent() != NULL) {
				//In this case, we need to keep the full instruction			
				last = kf->Parent()->InstructionRemoveLast();
				kinst = AtanorCreateInstruction(kf->Parent(), kcurrentop);
				kinst->instructions.push_back(kf);
				kf->Addparent(kinst);
			}
			else {
				last = kf->InstructionRemoveLast();
				kinst = AtanorCreateInstruction(kf, kcurrentop);
				if (last != NULL)
					kinst->instructions.push_back(last);
			}
		}
	}

	Traverse(xn->nodes[1], kinst);

	return kinst;
}


Atanor* AtanorCode::C_createfunction(x_node* xn, Atanor* kf) {
	short idname;
	string name = xn->nodes[1]->value;
	if (kf->isFunction()) {
		stringstream message;
		message << "Error: You cannot declare a function within a function: '" << name << "'";
		throw new AtanorRaiseError(message, filename, current_start, current_end);
	}

	idname = globalAtanor->Getid(name);
	if (kf->Type() != a_extension) {
		if (globalAtanor->procedures.check(idname) || globalAtanor->allmethods.check(idname)) {
			if (!kf->isFrame() || kf == &mainframe || (idname != a_initial && !globalAtanor->methods.check(idname))) {
				stringstream message;
				message << "Error: Predefined function, consider choosing another name: '" << name << "'";
				throw new AtanorRaiseError(message, filename, current_start, current_end);
			}
		}
	}

	Atanor* kprevious = Declaration(idname);
	if (kf != &mainframe)
		globalAtanor->functions[idname] = true;

	size_t last = xn->nodes.size() - 1;


	AtanorFunction* kfunc = NULL;
	bool autorun = false;
	bool privatefunction = false;
	bool strictfunction = false;
	bool joinfunction = false;

	AtanorFunction* predeclared = NULL;

	if (kprevious != NULL) {
		if (kprevious->isFunction() == false) {
			stringstream message;
			message << "Variable: '" << name << "' already declared";
			throw new AtanorRaiseError(message, filename, current_start, current_end);
		}

		//When value==".", it is an actual pre-declaration in a frame otherwise it is a type declaration scan... See AtanorParsePredeclareFunctions
		if (xn->nodes[last]->token == "declarationending" && xn->nodes[last]->value == ";") {
			kfunc = (AtanorFunction*)kprevious;
			while (kfunc != NULL) {
				if (kfunc->choice == 3) {
					predeclared = kfunc;
					break;
				}
				kfunc = kfunc->next;
			}
			kfunc = NULL;
		}

		if (predeclared == NULL) {
			//To cope with predeclarations, we need to use these predeclarations in the order in which they were found
			//When a predeclaration has been used, it is marked as declared (choice==1), we then jump to the next one
			while (kprevious != NULL && kprevious->Alreadydeclared())
				kprevious = ((AtanorFunction*)kprevious)->next;
		}
	}


	string typefunction;
	int protection = 0;
	vector<x_node*>& xsub = xn->nodes[0]->nodes;
	int si = 0;
	if (xsub[si]->value == "joined") {
		joinfunction = true;
		si++;
	}

	if (xsub[si]->value == "protected") {
		protection = 1;
		si++;
	}

	if (xsub[si]->value == "exclusive") {
		protection = 2;
		si++;
	}

	if (xsub[si]->value == "private") {
		privatefunction = true;
		si++;
	}
	if (xsub[si]->value == "strict") {
		strictfunction = true;
		si++;
	}
	if (xsub[si]->token == "functionlabel")
		typefunction = xsub[si]->value;

	//Two cases:
	//If this is the first implementation of that function OR NO predeclaration had been issued OR it is not a function, then we create a new function
	if (kprevious == NULL || xn->nodes[last]->token == "declarationending") {

		if (typefunction == "thread")
			kfunc = new AtanorThread(idname, global, joinfunction, protection, kf);
		else {
			if (protection != 0)
				kfunc = new AtanorProtectedFunction(idname, global, protection, kf);
			else
				kfunc = new AtanorFunction(idname, global, kf);
			if (typefunction == "autorun")
				autorun = true;
		}
	}

	//If we already have an implementation for that function, either it is a predeclaration, then we simply use it
	//Or this predeclaration is NOT a function
	if (kprevious != NULL && xn->nodes[last]->token != "declarationending") {
		if (kprevious->isFunction()) {
			if (kprevious->Predeclared()) {
				kfunc = (AtanorFunction*)kprevious; //we use the predeclaration as our new function
				kprevious = NULL;
			}//else, we will add this new function to a previous declaration...
			else
			if (kprevious->isUsed() == true)
				kprevious = NULL;
			else //if it has been implemented in the mother frame already, then it should not be attached to that set of functions
			if (kf != kprevious->Frame())
				kprevious = NULL;
		}
		else
			kprevious = NULL;
	}

	if (kfunc == NULL) {
		stringstream message;
		message << "Error: This function has already been used in a call: '" << name << "'";
		throw new AtanorRaiseError(message, filename, current_start, current_end);
	}

	kfunc->privatefunction = privatefunction;
	kfunc->strict = strictfunction;
	globalAtanor->Pushstack(kfunc);

	if (xn->nodes[last]->token == "declarationending") {
		kfunc->choice = 0;
		//we process the arguments, if they are available
		if (last != 2)
			Traverse(xn->nodes[2], kfunc);
		kfunc->choice = 2;
	}
	else {
		string typeret;
		if (xn->nodes[2]->token == "returntype") {
			typeret = xn->nodes[2]->nodes[0]->value;
			if (globalAtanor->symbolIds.find(typeret) == globalAtanor->symbolIds.end()) {
				stringstream message;
				message << "Unknown type: " << typeret;
				throw new AtanorRaiseError(message, filename, current_start, current_end);
			}
			kfunc->returntype = global->Getid(typeret);

			x_node* sub = xn->nodes[2];
			xn->nodes.erase(xn->nodes.begin() + 2);
			delete sub;
		}
		else {
			if (xn->nodes.size() > 3 && xn->nodes[3]->token == "returntype") {
				typeret = xn->nodes[3]->nodes[0]->value;
				if (globalAtanor->symbolIds.find(typeret) == globalAtanor->symbolIds.end()) {
					stringstream message;
					message << "Unknown type: " << typeret;
					throw new AtanorRaiseError(message, filename, current_start, current_end);
				}
				kfunc->returntype = global->Getid(typeret);

				x_node* sub = xn->nodes[3];
				xn->nodes.erase(xn->nodes.begin() + 3);
				delete sub;
			}
		}

		if (xn->nodes[2]->token == "instruction") {
			kfunc->choice = 1;
			Traverse(xn->nodes[2], kfunc);
		}
		else {
			//If we have a predeclaration, then the arguments have already been parsed...
			if (kfunc->Predeclared() == false) {
				kfunc->choice = 0;
				Traverse(xn->nodes[2], kfunc);
			}
			kfunc->choice = 1;
			Traverse(xn->nodes[3], kfunc);
		}

		if (kf->isFrame() && kf->Name() != a_mainframe) {
			//We declare our function within a frame...
			unsigned int a = 1 << kfunc->parameters.size();
			if (globalAtanor->framemethods.check(idname))
				globalAtanor->framemethods[idname] |= a;
			else
				globalAtanor->framemethods[idname] = a;
		}
	}

	if (kprevious != NULL) {
		bool found = false;
		//We need to consume it, an actual pre-declared function in a frame...
		if (predeclared != NULL) {
			if (predeclared->parameters.size() == kfunc->parameters.size()) {
				found = true;
				for (size_t arg = 0; arg < predeclared->parameters.size(); arg++) {
					if (predeclared->parameters[arg]->Type() != kfunc->parameters[arg]->Type()) {
						found = false;
						break;
					}
				}

				if (found) {
					if (kf->Declaration(idname) == NULL)
						kf->Declare(idname, predeclared);
					predeclared->choice = 2;
					predeclared->parameters = kfunc->parameters;
					kfunc = predeclared;
				}
				else {
					stringstream message;
					message << "Error: Cannot find a matching function to a pre-declared function (check the declaration order): '" << name << "'";
					throw new AtanorRaiseError(message, filename, current_start, current_end);
				}
			}
		}

		if (found == false)
			kprevious->Addfunction(kfunc);
	}
	else {
		if (kf->Declaration(idname) == NULL)
			kf->Declare(idname, kfunc);
	}

	globalAtanor->Popstack();

	//Autorun section
	if (kfunc->autorun)
		new AtanorCallFunction(kfunc, global, kf);

	if (autorun && loader == NULL) {
		if (kfunc->parameters.size() != 0) {
			stringstream message;
			message << "Error: An AUTORUN cannot have parameters: '" << name << "'";
			throw new AtanorRaiseError(message, filename, current_start, current_end);
		}

		if (kf != &mainframe) {
			stringstream message;
			message << "Error: An AUTORUN must be declared as a global function: '" << name << "'";
			throw new AtanorRaiseError(message, filename, current_start, current_end);
		}

		kfunc->autorun = true;
	}

	return kfunc;
}

//We preload our libraries...
bool AtanorCode::CheckUse(x_node* xn, Atanor* kf) {
	while (xn->nodes.size()) {
		if (xn->token == "regularcall") {
			if (xn->nodes[0]->token == "functioncall"	&& xn->nodes[0]->value == "use") {
				Traverse(xn, kf);
				xn->token[0] = '_';
				return true;
			}
		}
		xn = xn->nodes[0];
	}

	return false;
}



Atanor* AtanorCode::C_blocs(x_node* xn, Atanor* kf) {
	hmap<size_t, bool> skip;
	size_t i;
	Atanor* s;

	if (kf->isFrame()) {
		x_node* xend;
		x_node declaration_ending("declarationending", ";", xn);
		size_t last;
		//First we check all the functions present in the blocs and we predeclare them
		for (i = 0; i < xn->nodes.size(); i++) {
			if (CheckUse(xn->nodes[i], kf))
				continue;

			if (xn->nodes[i]->token == "bloc") {
				x_node* nsub = xn->nodes[i]->nodes[0];
				if (nsub->token == "predicatedefinition" || nsub->token == "predicatefact") {
					if (nsub->nodes[0]->token == "predicate" || nsub->nodes[0]->token == "dependencyfact") {
						short id = globalAtanor->Getid(nsub->nodes[0]->nodes[0]->value);
						//A modifier pour PROLOG
						if (!globalAtanor->predicates.check(id))
							globalAtanor->predicates[id] = new AtanorPredicateFunction(global, NULL, id);
					}
				}
				continue;
			}

			if (xn->nodes[i]->token == "function" || xn->nodes[i]->token == "frame") {
				last = xn->nodes[i]->nodes.size() - 1;
				xend = xn->nodes[i]->nodes[last];
				if (xend->token == "declarationending") {
					//in this case, we do not need to take these predeclarations into account anymore...
					skip[i] = true;
					if ((!kf->isFrame() || kf == &mainframe) && xn->nodes[i]->token == "function")
						continue;

					//we modify the value as a hint of a predeclared function for the actual building of that function
					xn->nodes[i]->nodes[last]->value = ".";
				}
				else
					xn->nodes[i]->nodes[last] = &declaration_ending;

				try {
					s = Traverse(xn->nodes[i], kf);
				}
				catch (AtanorRaiseError* m) {
					xn->nodes[i]->nodes[last] = xend;
					throw m;
				}
				xn->nodes[i]->nodes[last] = xend;
				//In this case, this an actual predeclaration from within a frame... We use a specific encoding
				//to isolate it later on... This can only happen in a frame...
				if (skip.find(i) != skip.end()) {
					((AtanorFunction*)s)->choice = 3;
					xn->nodes[i]->nodes[last]->value = ";";
				}
			}
		}
	}

	Atanor* ke = NULL;
	for (i = 0; i < xn->nodes.size(); i++) {
		if (skip.find(i) != skip.end())
			continue;
		s = Traverse(xn->nodes[i], kf);
		if (s != NULL)
			ke = s;
	}
	return ke;

}

Atanor* AtanorCode::C_extension(x_node* xn, Atanor* kf) {
	string nametype = xn->nodes[0]->value;

	short idtypename = globalAtanor->Getid(nametype);

	if (!global->newInstance.check(idtypename)) {
		stringstream message;
		message << "Error: cannot extend this type:" << nametype;
		throw new AtanorRaiseError(message, filename, current_start, current_end);
	}

	string name;
	name = "_" + nametype;

	short idname = global->Getid(name);

	//A temporary place to store our stuff
	AtanorFrame* extension;

	if (globalAtanor->extensions.check(idtypename))
		extension = globalAtanor->extensions[idtypename];
	else {
		globalAtanor->extensions[idtypename] = extension = new AtanorFrame(idname, false, global); //the name of the frame is also the name of the inner variable...
		extension->idtype = a_extension;
	}

	//A variable, which will be our contact here to our object...
	AtanorVariableDeclaration* var = new AtanorVariableDeclaration(global, idname, idtypename);
	extension->Declare(idname, var);
	globalAtanor->Pushstack(extension);

	Traverse(xn->nodes[1], extension);
	globalAtanor->Popstack();
	//Now we extract every single function from this extension
	basebin_hash<Atanor*>::iterator it;
	Atanor* f;
	for (it = extension->declarations.begin(); it != extension->declarations.end(); it++) {
		f = it->second;
		if (f->isFunction()) {
			unsigned long arity = 1 << f->Size();
			global->RecordMethods(idtypename, f->Name(), arity);
		}
	}

	return kf;
}

Atanor* AtanorCode::C_frame(x_node* xn, Atanor* kf) {
	//We create a frame
	//The name is the next parameter

	bool privated = false;
	int pos = 0;
	if (xn->nodes[0]->token == "private") {
		privated = true;
		pos = 1;
	}
	string name = xn->nodes[pos]->value;
	short idname = global->Getid(name);
	AtanorFrame* kframe = NULL;
	Atanor* ke = NULL;

	if (globalAtanor->frames.find(idname) != globalAtanor->frames.end()) {
		ke = globalAtanor->frames[idname];
		if (privated) {
			stringstream message;
			message << "Error: attempt to use private frame:" << name;
			throw new AtanorRaiseError(message, filename, current_start, current_end);
		}
	}
	else
		ke = kf->Declaration(idname);

	if (ke == NULL) {
		kframe = new AtanorFrame(idname, privated, global, kf);
		Atanorframeinstance::RecordFrame(idname, kframe, global);
		//We consider each frame as a potential procedure that will create a frame of the same type.
		globalAtanor->RecordOneProcedure(name, ProcCreateFrame, P_FULL);
		globalAtanor->returntypes[idname] = idname;
	}
	else {
		if (ke->Type() == a_frame)
			kframe = (AtanorFrame*)ke;
	}

	if (kframe == NULL) {
		stringstream message;
		message << "Error: This frame has already been declared:" << name;
		throw new AtanorRaiseError(message, filename, current_start, current_end);
	}

	//We record the compatibilities, which might come as handy to check function argument
	globalAtanor->SetCompatibilities(idname);
	if (kf->Type() == a_frame && &mainframe != kf) {
		globalAtanor->compatibilities[idname][kf->Name()] = true;
		globalAtanor->strictcompatibilities[idname][kf->Name()] = true;
	}

	if (xn->nodes[pos + 1]->token == "declarationending") {
		kf->Declare(kframe->name, kframe);
		return kframe;
	}

	globalAtanor->Pushstack(kframe);

	bool replace = false;
	//If it is a sub-frame definition
	if (kf->Type() == a_frame && &mainframe != kf) {
		//We copy all our declarations in it
		//These declarations, will be replaced by local ones if necessary
		kf->Sharedeclaration(kframe, false);
		replace = true;
	}

	//We then record this new Frame in our instructions list
	//We also store it at the TOP level, so that others can have access to it...
	mainframe.Declare(kframe->name, kframe);
	if (xn->nodes[pos + 1]->token == "depend") {
		string funcname = xn->nodes[pos + 1]->nodes[0]->value;
		short idf = globalAtanor->Getid(funcname);
		Atanor* kfunc = NULL;
		kfunc = Declaration(idf, kf);
		//We have a WITH description
		if (kfunc == NULL) {
			stringstream message;
			message << "Unknown function: '" << funcname << "'";
			throw new AtanorRaiseError(message, filename, current_start, current_end);
		}
		if (kfunc->isFunction()) {
			if (kfunc->isVariable()) {
				x_node nvar("variable", "", xn);
				creationxnode("word", globalAtanor->Getsymbol(kfunc->Name()), &nvar);
				kfunc = Traverse(&nvar, kf);
			}

			kframe->Addfunction(kfunc);
		}
		else {
			stringstream message;
			message << "Unknown function: '" << globalAtanor->idSymbols[kfunc->Name()] << "'";
			throw new AtanorRaiseError(message, filename, current_start, current_end);
		}

		Traverse(xn->nodes[pos + 2], kframe);
	}
	else
		Traverse(xn->nodes[pos + 1], kframe);

	globalAtanor->Popstack();

	if (replace)
		kf->Sharedeclaration(kframe, true);

	//The initial function returns the frame itself in the case of a frame...
	Atanor* callinitial = kframe->Declaration(a_initial);
	while (callinitial != NULL) {
		((AtanorFunction*)callinitial)->returntype = idname;
		callinitial = callinitial->Nextfunction();
	}

	return kframe;
}

Atanor* AtanorCode::C_parenthetic(x_node* xn, Atanor* kf) {
	if (kf->Action() != a_blocboolean) {
		AtanorInstruction* ki = new AtanorSequence(global);

		for (size_t i = 0; i < xn->nodes.size(); i++)
			Traverse(xn->nodes[i], ki);

		if (ki->action == a_bloc && ki->instructions.size() == 1) {
			Atanor* ke = ki->instructions[0];
			//Either optional is called from within an arithmetic operation, and then we do nothing
			//or it is called from within a bloc of instructions, then we need to promote it as a ROOT
			if (ke->isFunction() || ke->isInstruction()) {
				if (!kf->isOperation() && ke->isOperation()) {
					Atanor* kroot = ke->Compile(NULL);
					ke->Remove();
					ke = kroot;
				}

				kf->AddInstruction(ke);
				ki->Remove();
				return ke;
			}
		}
		else
			ki = CloningInstruction(ki);

		kf->AddInstruction(ki);
		return ki;
	}

	for (size_t i = 0; i < xn->nodes.size(); i++)
		Traverse(xn->nodes[i], kf);
	return kf;
}

Atanor* AtanorCode::C_test(x_node* xn, Atanor* kf) {
	AtanorInstructionIF* ktest = new AtanorInstructionIF(global, kf);
	Traverse(xn->nodes[0], ktest);
	Atanor* nxt = ktest->instructions[0];

	//Small improvement, when we have only one element of test
	//It is BOOLEANBLOC, with only one element...
	//We can get rid of it and push it to the top...
	if (nxt->Action() == a_blocboolean && nxt->InstructionSize() == 1) {
		//we push the negation up then... otherwise it would be lost
		ktest->negation = nxt->isNegation();
		nxt = nxt->Instruction(0);
		ktest->instructions[0]->Remove();
		ktest->instructions.vecteur[0] = nxt;
	}

	Atanor* ktrue = new AtanorSequence(global, ktest);
	globalAtanor->Pushstack(ktrue);
	Traverse(xn->nodes[1], ktrue);
	globalAtanor->Popstack();
	if (ktrue->InstructionSize() == 1 && !ktrue->Instruction(0)->isVariable()) {
		nxt = ktrue->Instruction(0);
		ktest->Putinstruction(1, nxt);
		ktrue->InstructionClear();
		ktrue->Remove();
	}

	if (xn->nodes.size() == 3) {
		Atanor* kfalse = new AtanorSequence(global, ktest);
		globalAtanor->Pushstack(kfalse);
		Traverse(xn->nodes[2], kfalse);
		globalAtanor->Popstack();
		if (kfalse->InstructionSize() == 1 && !kfalse->Instruction(0)->isVariable()) {
			nxt = kfalse->Instruction(0);
			ktest->Putinstruction(2, nxt);
			kfalse->InstructionClear();
			kfalse->Remove();
		}
	}

	return ktest;
}

Atanor* AtanorCode::C_booleanexpression(x_node* xn, Atanor* kf) {
	Atanor* kbloc;
	Atanor* ke;

	if (xn->nodes.size() == 1) {
		//It is a test on a function or on a single variable...
		kbloc = AtanorCreateInstruction(NULL, a_blocboolean);

		Traverse(xn->nodes[0], kbloc);
		//The test on negation stems from the fact, that negation will be tested through a_blocboolean in AtanorInstruction::Exec
		//otherwise, we do not need it...
		if (kbloc->Action() == a_blocboolean && kbloc->InstructionSize() == 1 && !kbloc->isNegation()) {
			//In this case, we can get rid of it...
			ke = kbloc->Instruction(0);
			kbloc->Remove();
			kf->AddInstruction(ke);
			return ke;
		}

		kbloc = CloningInstruction((AtanorInstruction*)kbloc);
		kf->AddInstruction(kbloc);
		return kbloc;
	}
	//Else, we have two expressions and an operator
	//If our operator is new to our test
	ke = kf;
	short op = globalAtanor->string_operators[xn->nodes[1]->value];
	if (op != kf->Action()) {
		if (kf->Action() == a_blocboolean)
			ke->Setaction(op);
		else {
			ke = AtanorCreateInstruction(kf, op);
		}
	}

	kbloc = AtanorCreateInstruction(NULL, a_blocboolean);
	Traverse(xn->nodes[0], kbloc);
	if (kbloc->Action() == a_blocboolean && kbloc->InstructionSize() == 1 && !kbloc->isNegation()) {
		//In this case, we can get rid of it...
		Atanor* k = kbloc->Instruction(0);
		kbloc->Remove();
		kbloc = k;
	}
	else
		kbloc = CloningInstruction((AtanorInstruction*)kbloc);

	ke->AddInstruction(kbloc);

	if (xn->nodes.size() == 3)
		Traverse(xn->nodes[2], ke);
	else
		Traverse(xn->nodes[1], kbloc);
	return ke;
}

Atanor* AtanorCode::C_negation(x_node* xn, Atanor* kf) {
	kf->Setnegation(true);
	return kf;
}

Atanor* AtanorCode::C_switch(x_node* xn, Atanor* kf) {
	//We create a IF section
	Atanor* kswitch = new AtanorInstructionSWITCH(global, kf);

	Traverse(xn->nodes[0], kswitch);
	int i = 1;

	for (; i < xn->nodes.size(); i++)
		Traverse(xn->nodes[i], kswitch);
	AtanorInstructionSWITCH* ks = (AtanorInstructionSWITCH*)kswitch;
	bool onlybasevalue = true;
	for (i = 1; i < ks->instructions.size(); i += 2) {
		if (ks->instructions[i] == aDEFAULT)
			break;

		if (ks->instructions[i]->baseValue() == false) {
			onlybasevalue = false;
			break;
		}
		ks->keys[ks->instructions[i]->String()] = i;
	}
	ks->usekeys = onlybasevalue;
	return kswitch;
}

Atanor* AtanorCode::C_testswitch(x_node* xn, Atanor* kf) {
	if (xn->nodes[0]->token == "default")
		kf->AddInstruction(aDEFAULT);
	else
		Traverse(xn->nodes[0], kf);

	Atanor* ktrue = new AtanorSequence(global, kf);
	globalAtanor->Pushstack(ktrue);
	Traverse(xn->nodes[1], ktrue);
	globalAtanor->Popstack();
	if (ktrue->InstructionSize() == 1 && !ktrue->Instruction(0)->isVariable()) {
		kf->Putinstruction(kf->InstructionSize() - 1, ktrue->Instruction(0));
		ktrue->InstructionClear();
		ktrue->Remove();
	}

	return ktrue;
}


Atanor* AtanorCode::C_loop(x_node* xn, Atanor* kf) {
	AtanorInstructionWHILE* kwhile = new AtanorInstructionWHILE(global, kf);

	Traverse(xn->nodes[0], kwhile);

	Atanor* kseq = new AtanorSequence(global, kwhile);
	globalAtanor->Pushstack(kseq);
	Traverse(xn->nodes[1], kseq);
	globalAtanor->Popstack();
	if (kseq->InstructionSize() == 1 && !kseq->Instruction(0)->isVariable()) {
		kwhile->Putinstruction(1, kseq->Instruction(0));
		kseq->InstructionClear();
		kseq->Remove();
	}


	Atanor* kinst = kwhile->instructions[1];
	if (kinst->Type() == a_sequence && kinst->InstructionSize() == 1)
		kwhile->instructions.vecteur[1] = kinst->Instruction(0);

	return kwhile;
}



Atanor* AtanorCode::C_doloop(x_node* xn, Atanor* kf) {
	AtanorInstructionUNTIL* kuntil = new AtanorInstructionUNTIL(global, kf);
	Traverse(xn->nodes[1], kuntil);

	Atanor* kseq = new AtanorSequence(global, kuntil);

	globalAtanor->Pushstack(kseq);
	Traverse(xn->nodes[0], kseq);
	globalAtanor->Popstack();
	if (kseq->InstructionSize() == 1 && !kseq->Instruction(0)->isVariable()) {
		kuntil->Putinstruction(1, kseq->Instruction(0));
		kseq->InstructionClear();
		kseq->Remove();
	}

	Atanor* kinst = kuntil->instructions[1];
	if (kinst->Type() == a_sequence && kinst->InstructionSize() == 1)
		kuntil->instructions.vecteur[1] = kinst->Instruction(0);

	return kuntil;
}



Atanor* AtanorCode::C_for(x_node* xn, Atanor* kf) {
	Atanor* kbase = NULL;
	AtanorInstructionFOR* kfor;
	bool protecting = false;
	if (xn->nodes[0]->token == "multideclaration") {
		kbase = new AtanorSequence(global, kf);
		globalAtanor->Pushstack(kbase);
		Traverse(xn->nodes[0], kbase);
		kfor = new AtanorInstructionFOR(global, kbase);
		protecting = true;
	}
	else
		kfor = new AtanorInstructionFOR(global, kf);

	//Initialisation
	if (!protecting)
		Traverse(xn->nodes[0], kfor);
	else
		kfor->AddInstruction(aTRUE);

	//Test
	Traverse(xn->nodes[1], kfor);
	//Increment
	Traverse(xn->nodes[2], kfor);

	Atanor* kbloc = new AtanorSequence(global, kfor);

	//Instruction
	globalAtanor->Pushstack(kbloc);
	Traverse(xn->nodes[3], kbloc);
	globalAtanor->Popstack();
	if (protecting)
		globalAtanor->Popstack();

	if (kbloc->InstructionSize() == 1 && !kbloc->Instruction(0)->isVariable()) {
		kfor->Putinstruction(3, kbloc->Instruction(0));
		kbloc->InstructionClear();
		kbloc->Remove();
	}

	return kfor;
}


Atanor* AtanorCode::C_blocfor(x_node* xn, Atanor* kf) {

	Atanor* kbloc = new AtanorSequence(global, kf);
	for (int i = 0; i < xn->nodes.size(); i++)
		Traverse(xn->nodes[i], kbloc);
	return kbloc;
}

Atanor* AtanorCode::C_forin(x_node* xn, Atanor* kf) {

	AtanorInstruction* kforin;
	AtanorInstruction ktemp;
	Traverse(xn->nodes[1], &ktemp);
	Atanor* kcontainer = ktemp.instructions[0];
	bool protecting = false;
	Atanor* kref = kf;
	Atanor* kbase = NULL;
	if (xn->nodes[0]->nodes.size() == 2 && xn->nodes[0]->token == "declarationfor") {
		kbase = new AtanorSequence(global, kf);
		xn->nodes[0]->token = "declaration";
		globalAtanor->Pushstack(kbase);
		Traverse(xn->nodes[0], kbase);
		protecting = true;
		kref = kbase;
	}

	bool checkrange = false;
	bool checkforinself = false;
	short idvar = -1;
	if (kcontainer->isCallVariable() && globalAtanor->isFile(kcontainer))
		kforin = new AtanorInstructionFILEIN(global, kref);
	else {
		if (xn->nodes[1]->token == "arange") {
			kforin = new AtanorInstructionFORINRANGE(global);
			checkrange = true;
		}
		else {
			if (xn->nodes[0]->token == "valvectortail")
				kforin = new AtanorInstructionFORVECTORIN(global, kref);
			else {
				if (xn->nodes[0]->token == "valmaptail")
					kforin = new AtanorInstructionFORMAPIN(global, kref);
				else {
					idvar = kcontainer->Typevariable();
					if (globalAtanor->newInstance.check(idvar) && kcontainer->Function() == NULL &&
						(globalAtanor->newInstance[idvar]->isValueContainer() || 
						globalAtanor->newInstance[idvar]->isMapContainer()))
						kforin = new AtanorInstructionFORINVALUECONTAINER(global, kref);
					else {
						kforin = new AtanorInstructionFORIN(global);										
						checkforinself = true;
					}
				}
			}
		}
	}

	Atanor* kin = AtanorCreateInstruction(kforin, a_blocloopin);
	if (kbase == NULL)
		Traverse(xn->nodes[0], kin);
	else
		Traverse(xn->nodes[0]->nodes[1], kin);

	if (!kin->Instruction(0)->Checkvariable()) {
		stringstream message;
		message << "Expecting variables in FOR to loop in";
		throw new AtanorRaiseError(message, filename, current_start, current_end);
	}

	if (checkforinself) {
		
		if (kcontainer->isVectorContainer() && kcontainer->Function() == NULL) {
			if (kin->Instruction(0)->Function() == NULL) {
				kforin->Remove();
				kforin = new AtanorInstructionFORINVECTOR(global, kref);
				kforin->AddInstruction(kin);
				checkforinself = false;
			}
		}

		if (checkforinself)
			kref->AddInstruction(kforin);
	}

	kin->Instruction(0)->Setevaluate(true);
	kin->Instruction(0)->Setaffectation(true);

	if (kforin->Type() == a_forinrange) {
		if (!kin->Instruction(0)->isCallVariable()) {
			stringstream message;
			message << "Expecting a variable in FOR";
			throw new AtanorRaiseError(message, filename, current_start, current_end);
		}

		if (!globalAtanor->isNumber(kin->Instruction(0))) {
			stringstream message;
			message << "Only numerical variable can be used here";
			throw new AtanorRaiseError(message, filename, current_start, current_end);
		}

		kin->AddInstruction(ktemp.instructions[0]);
		kin->AddInstruction(ktemp.instructions[1]);
		kin->AddInstruction(ktemp.instructions[2]);
	}
	else
		kin->AddInstruction(kcontainer);


	//We then compile the instruction bloc
	Atanor* ktrue = new AtanorSequence(global, kforin);
	globalAtanor->Pushstack(ktrue);
	Traverse(xn->nodes[2], ktrue);
	globalAtanor->Popstack();
	if (protecting)
		globalAtanor->Popstack();
	if (ktrue->InstructionSize() == 1 && !ktrue->Instruction(0)->isVariable()) {
		kforin->Putinstruction(1, ktrue->Instruction(0));
		ktrue->InstructionClear();
		ktrue->Remove();
	}
	if (checkrange) {
		Atanor* value = kforin->instructions.vecteur[0]->Instruction(0);
		if (value->Function() == NULL) {
			switch (value->Typevariable()) {
			case a_int:
				value = new AtanorInstructionFORINRANGEINTEGER(value, kforin, global, kref);
				kforin->Remove();
				return value;
			case a_float:
				value = new AtanorInstructionFORINRANGEFLOAT(value, kforin, global, kref);
				kforin->Remove();
				return value;
			case a_decimal:
				value = new AtanorInstructionFORINRANGEDECIMAL(value, kforin, global, kref);
				kforin->Remove();
				return value;
			case a_long:
				value = new AtanorInstructionFORINRANGELONG(value, kforin, global, kref);
				kforin->Remove();
				return value;
			case a_short:
				value = new AtanorInstructionFORINRANGESHORT(value, kforin, global, kref);
				kforin->Remove();
				return value;
			}
		}
		kref->AddInstruction(kforin);
	}

	return kforin;
}



Atanor* AtanorCode::C_trycatch(x_node* xn, Atanor* kf) {
	Atanor* ktry = new AtanorInstructionTRY(global, kf);

	string name;
	Atanor* declaration = NULL;
	AtanorCallVariable* ki;

	short id;
	Atanor* dom = NULL;
	if (xn->nodes.size() != 1 && xn->nodes[1]->token == "word") {
		name = xn->nodes[1]->value;
		id = globalAtanor->Getid(name);
		declaration = Declaration(id, kf);

		if (!declaration->isVariable()) {
			stringstream message;
			message << "Unknown variable: '" << name << "'";
			throw new AtanorRaiseError(message, filename, current_start, current_end);
		}

		AtanorInstruction* kaff = AtanorCreateInstruction(ktry, a_affectation);

		dom = Declaror(id, kf);

		if (dom == &mainframe)
			ki = new AtanorCallVariable(id, declaration->Type(), global, kaff);
		else
		if (dom->isFunction())
			ki = new AtanorCallFunctionVariable(id, declaration->Type(), global, kaff);
		else
			ki = new AtanorCallFrameVariable(id, (AtanorFrame*)dom, declaration->Typevariable(), global, kaff);

		kaff->AddInstruction(aNULL);
		kaff->Instruction(0)->Setaffectation(true);

	}

	Traverse(xn->nodes[0], ktry);


	AtanorCallProcedure* kcatchproc = new AtanorCallProcedure(globalAtanor->Getid("catch"), global, ktry);

	if (xn->nodes.size() != 1) {
		if (xn->nodes[1]->token == "word") {
			if (dom == &mainframe)
				ki = new AtanorCallVariable(id, declaration->Type(), global, kcatchproc);
			else
			if (dom->isFunction())
				ki = new AtanorCallFunctionVariable(id, declaration->Type(), global, kcatchproc);
			else
				ki = new AtanorCallFrameVariable(id, (AtanorFrame*)dom, declaration->Typevariable(), global, kcatchproc);

			if (xn->nodes.size() == 3) {
				if (xn->nodes[2]->token == "blocs")  {
					Atanor* kbloc = new AtanorInstructionCATCH(global, ktry);
					//Instruction
					Traverse(xn->nodes[2], kbloc);
				}
			}
		}
		else {
			if (xn->nodes[1]->token == "blocs")  {
				Atanor* kbloc = new AtanorInstructionCATCH(global, ktry);
				//Instruction
				Traverse(xn->nodes[1], kbloc);
			}
		}
	}

	return ktry;
}


Atanor* AtanorCode::C_parameters(x_node* xn, Atanor* kcf) {
	AtanorInstruction kbloc;
	short id = kbloc.idtype;
	if (kcf->Name() >= a_asserta && kcf->Name() <= a_retract)
		id = a_parameterpredicate;

	Atanor* ki;
	AtanorInstruction* k;
	for (int i = 0; i < xn->nodes.size(); i++) {
		kbloc.idtype = id;

		Traverse(xn->nodes[i], &kbloc);

		if (kbloc.instructions.last == 0) {
			stringstream message;
			message << "Error: Wrong parameter definition" << endl;
			throw new AtanorRaiseError(message, filename, current_start, current_end);
		}

		ki = kbloc.instructions[0];

		if (ki->InstructionSize() == 1) {
			k = AtanorCreateInstruction(kcf, ki->Action());
			ki->Clone(k);
			ki->Remove();
		}
		else
			kcf->AddInstruction(ki);

		kbloc.instructions.clear();
	}
	return kcf;
}
//----------------------------------------------------------------------------------------------
//-------------------- HASKELL


bool AtanorCode::CheckVariable(x_node* xn, Atanor* kf) {
	if (xn == NULL)
		return false;

	if (xn->token == "variable") {
		short id = globalAtanor->Getid(xn->nodes[0]->value);
		if (kf->isDeclared(id) != NULL)
			return true;
		return false;
	}

	for (size_t i = 0; i < xn->nodes.size(); i++)
	if (CheckVariable(xn->nodes[i], kf) == true)
		return true;

	return false;
}


bool AtanorCode::CheckingVariableName(x_node* xn, Atanor* kf) {
	if (xn == NULL)
		return false;

	if (xn->token == "variable") {
		short id = globalAtanor->Getid(xn->nodes[0]->value);
		if (kf->Name() == id)
			return true;
		return false;
	}

	for (size_t i = 0; i < xn->nodes.size(); i++)
	if (CheckingVariableName(xn->nodes[i], kf) == true)
		return true;

	return false;
}

Atanor* AtanorCode::C_hbloc(x_node* xn, Atanor* kf) {
	Atanor* kseq = new AtanorHBloc(global, kf);
	globalAtanor->Pushstack(kseq);
	Traverse(xn->nodes[0], kseq);
	globalAtanor->Popstack();
	return kseq;
}

Atanor* AtanorCode::C_hdata(x_node* xn, Atanor* kf) {
	//we create a frame, whose name is the value of that data structure...
	string name = xn->nodes[0]->value;
	short idname = global->Getid(name);

	globalAtanor->SetCompatibilities(idname);

	AtanorFrame* kframe = new AtanorFrame(idname, false, global, kf);
	Atanorframeinstance::RecordFrame(idname, kframe, global);
	//We consider each frame as a potential procedure that will create a frame of the same type.
	globalAtanor->RecordOneProcedure(name, ProcCreateFrame, P_FULL);
	globalAtanor->returntypes[idname] = idname;
	//We then record this new Frame in our instructions list
	//We also store it at the TOP level, so that others can have access to it...
	globalAtanor->Pushstack(kframe);
	mainframe.Declare(kframe->name, kframe);

	for (int i = 1; i < xn->nodes.size(); i++)
		Traverse(xn->nodes[i], kframe);

	globalAtanor->Popstack();
	return kframe;
}

Atanor* AtanorCode::C_hdeclaration(x_node* xn, Atanor* kf) {
	//we create a frame, whose name is the value of a subframe to the data structure...
	string name = xn->nodes[0]->value;
	short idname = global->Getid(name);

	if (kf->Type() == a_lambda) {
		if (!global->frames.check(idname)) {
			stringstream message;
			message << "Unknown data structure (or frame): '" << name << "'";
			throw new AtanorRaiseError(message, filename, current_start, current_end);
		}
		AtanorFrame* kframe = global->frames[idname];
		AtanorFrameParameter* fparam = new AtanorFrameParameter(kframe->Name(), global, kf);
		Atanor* var;
		if (kframe->variables.size() != xn->nodes.size() - 1) {
			stringstream message;
			message << "Data structure mismatch (or frame): '" << name << "'";
			throw new AtanorRaiseError(message, filename, current_start, current_end);
		}
		short argtype;
		short id;
		for (long i = 0; i < kframe->variables.size(); i++) {
			argtype = kframe->variables[i]->Typevariable();			
			if (xn->nodes[i + 1]->token == "word") {
				id = globalAtanor->Getid(xn->nodes[i + 1]->value);
				if (id == a_universal)
					continue;
				if (argtype == a_universal || argtype == a_self || argtype == a_let)
					var = new AtanorSelfDeclarationVariable(global, id, a_self);
				else
					var = new AtanorVariableDeclaration(global, id, argtype);
				kf->Declare(id, var);
				fparam->Declare(kframe->variables[i]->Name(), var);
			}
		}
		return fparam;
	}

	//We map a Haskell data structure into a frame...
	stringstream framecode;

	framecode << "frame " << name << "{";

	char nm[] = {'d', '0', 0 };
	for (int i = 1; i < xn->nodes.size(); i++) {
		nm[1] = 48 + i;
		framecode << xn->nodes[i]->value << " " << nm << ";";
	}
	//Now we need an initial function
	framecode << "function _initial(";
	nm[0] = 'p';
	for (int i = 1; i < xn->nodes.size(); i++) {
		if (i>1)
			framecode << ",";
		nm[1] = 48 + i;
		framecode << xn->nodes[i]->value << " " << nm;
	}

	framecode << ") {";
	for (int i = 1; i < xn->nodes.size(); i++) {
		nm[0] = 'd';
		nm[1] = 48 + i;
		framecode << nm << "=";
		nm[0] = 'p';
		framecode << nm << ";";
	}
	framecode << "}";
	//The string function...
	framecode << "function string() {return(" <<"\"<"<< name<<'"';
	nm[0] = 'd';
	for (int i = 1; i < xn->nodes.size(); i++) {
		nm[1] = 48 + i;
		framecode << "+' '+" << nm;
	}
	framecode << "+'>');}}";

	bnf_atanor bnf;
	bnf.baseline = globalAtanor->linereference;

	x_readstring xr(framecode.str());
	xr.loadtoken();
	global->lineerror = -1;

	x_node* xstring = bnf.x_parsing(&xr, FULL);
	xstring->start = xn->start;
	xstring->end = xn->end;

	//Specific case, when the declaration is: <data TOTO = TOTO...>
	//In this case, we do not want this element to be a subframe of itself...
	//So we pop up the element in the stack
	if (idname == kf->Name())
		global->Popstack();

	Traverse(xstring, &mainframe);
	
	//And we put it back...
	if (idname == kf->Name())
		global->Pushstack(kf);
	else {//otherwise, we add a strictcompatibilities between the mother and the daughter frames...
		global->compatibilities[kf->Name()][idname] = true;
		global->strictcompatibilities[kf->Name()][idname] = true;
	}

	delete xstring;

	return kf;
}

Atanor* AtanorCode::C_telque(x_node* xn, Atanor* kf) {
	//We deactivate temporarily the instance recording...

	AtanorCallFunctionHaskell* kint = NULL;

	if (kf->Type() == a_callhaskell)
		kint = (AtanorCallFunctionHaskell*)kf;


	//hmetafunctions: filter, map, zipWith, takeWhile, scan, fold
	if (xn->nodes.size() && xn->nodes[0]->token == "hmetafunctions") {
		if (kint == NULL)
			kint = new AtanorCallFunctionHaskell(global, kf);
		kint->Init(NULL);
		return Traverse(xn->nodes[0]->nodes[0], kint);
	}

	short idname;
	int i;
	AtanorFunctionLambda* kfuncbase = NULL;

	int first = 0;
	short return_type = -1;
	bool onepushtoomany = false;
	
	Atanor* kprevious = NULL;

	vector<Haskelldeclaration*> localhaskelldeclarations;
	
	bool clearlocalhaskelldeclarations = false;
	bool haskelldeclarationfound = false;
	if (xn->nodes[0]->token == "haskelldeclaration")
		haskelldeclarationfound = true;
	else
	if (xn->nodes[0]->token == "returnhaskelldeclaration") {
		x_node* hdecl = xn->nodes[0];
		xn->nodes.erase(xn->nodes.begin());
		AtanorFunctionLambda localfunc(0);
		Traverse(hdecl, &localfunc);
		return_type = localfunc.returntype;
		localhaskelldeclarations = localfunc.haskelldeclarations;
		localfunc.haskelldeclarations.clear();
		delete hdecl;
		clearlocalhaskelldeclarations = true;
	}
		

	if (xn->nodes[0]->token == "haskell" || haskelldeclarationfound) {
		string name = xn->nodes[0]->nodes[first]->value;
		idname = globalAtanor->Getid(name);

		if (globalAtanor->procedures.check(idname) || globalAtanor->allmethods.check(idname)) {
			stringstream message;
			message << "Error: Predefined function, consider choosing another name: '" << name << "'";
			throw new AtanorRaiseError(message, filename, current_start, current_end);
		}

		kprevious = kf->Declaration(idname);
		if (kprevious == NULL && !kf->isFrame()) {
			//we check for a hdeclarede declaration function...
			kprevious = mainframe.Declaration(idname);
			//If we have a function declaration with the same name, but without instructions and a hdeclared, we keep it...
			if (kprevious != NULL && !kprevious->Purehaskelldeclaration())
				kprevious = NULL;
		}

		if (kprevious != NULL) {
			if (haskelldeclarationfound || kprevious->Type() != a_lambda) {
				stringstream message;
				message << "Error: A function with this name already exists: '" << name << "'";
				throw new AtanorRaiseError(message, filename, current_start, current_end);
			}

			kfuncbase = (AtanorFunctionLambda*)kprevious;
			if (kfuncbase->hdeclared) {
				if (return_type == -1) {
					return_type = kfuncbase->returntype;
					localhaskelldeclarations = kfuncbase->haskelldeclarations;
				}
			}

			kfuncbase = new AtanorFunctionLambda(idname, global);
			
			if (kprevious->Purehaskelldeclaration()) {
				if (kf->hasDeclaration())
					kf->Declare(idname, kfuncbase);
				else //in that case, it means that we are returning a function as result...
					kf->AddInstruction(new AtanorGetFunctionLambda(kfuncbase, globalAtanor));
			}
			//we keep the top function as the reference in the declarations list (which will be available throughout the stack)
			kfuncbase->declarations[idname] = kprevious;
			kprevious->Addfunction(kfuncbase);			
		}
		else {
			kfuncbase = new AtanorFunctionLambda(idname, global);
			if (kf->hasDeclaration())
				kf->Declare(idname, kfuncbase);
			else //in that case, it means that we are returning a function as result...
				kf->AddInstruction(new AtanorGetFunctionLambda(kfuncbase, globalAtanor));							
		}

		if (haskelldeclarationfound) {
			Traverse(xn->nodes[0]->nodes[1], kfuncbase);
			return kfuncbase;
		}

		kfuncbase->choice = 0;
		short id;
		Atanor* var;
		
		first++;
		//If no hdeclared has been declared, we declare each variable as a Self variable
		if (return_type == -1) {
			for (i = first; i < xn->nodes[0]->nodes.size(); i++) {
				if (xn->nodes[0]->nodes[i]->token == "word") {
					id = globalAtanor->Getid(xn->nodes[0]->nodes[i]->value);
					var = new AtanorSelfDeclarationVariable(global, id, a_self, kfuncbase);
					kfuncbase->Declare(id, var);
				}
				else {//it is a haskellvector...
					Traverse(xn->nodes[0]->nodes[i], kfuncbase);
				}
			}
		}
		else {
			//In that case, we know the intput, we need to do something about it...
			kfuncbase->returntype = return_type;
			kfuncbase->hdeclared = true;
			if (clearlocalhaskelldeclarations)
				kfuncbase->haskelldeclarations = localhaskelldeclarations;
			else
				kfuncbase->sethaskelldeclarations(localhaskelldeclarations);

			int sz = xn->nodes[0]->nodes.size() - first;
			if (sz != localhaskelldeclarations.size()) {
				stringstream message;
				message << "The declaration does not match the argument list of the function";
				throw new AtanorRaiseError(message, filename, current_start, current_end);
			}
			short argtype;
			Atanor* local;
			for (i = first; i < xn->nodes[0]->nodes.size(); i++) {
				argtype = localhaskelldeclarations[i - first]->Type();
				if (xn->nodes[0]->nodes[i]->token == "word") {
					id = globalAtanor->Getid(xn->nodes[0]->nodes[i]->value);
					if (argtype == a_universal || argtype == a_self || argtype == a_let)
						var = new AtanorSelfDeclarationVariable(global, id, a_self, kfuncbase);
					else
						var = new AtanorVariableDeclaration(global, id, argtype, kfuncbase);
					kfuncbase->Declare(id, var);
				}
				else {//it is a haskellvector...					
					Traverse(xn->nodes[0]->nodes[i], kfuncbase);
					local = kfuncbase->parameters.back();
					if (argtype == a_vector) {
						if (!local->isVectorContainer()) {
							stringstream message;
							message << "The argument: " << (i - first) + 1 << " does not match the hdeclared description";
							throw new AtanorRaiseError(message, filename, current_start, current_end);
						}
					}
					else if (argtype == a_map) {
						if (!local->isMapContainer()) {
							stringstream message;
							message << "The argument: " << (i - first) + 1 << " does not match the hdeclared description";
							throw new AtanorRaiseError(message, filename, current_start, current_end);
						}
					}					
				}
			}
			return_type = -1;
			localhaskelldeclarations.clear();
		}

		onepushtoomany = true;
		globalAtanor->Pushstack(kfuncbase);
		kfuncbase->choice = 1;
		if (kint == NULL) //it is a function declaration, we create it out of the main loop...
			kint = new AtanorCallFunctionHaskell(global);
		kint->Init(kfuncbase);
		if (xn->nodes.size() == 2) {//we do not have a range or a Boolean expression			
			if (xn->nodes[1]->token == "hmetafunctions") {
				Traverse(xn->nodes[1], kint);
				globalAtanor->Popstack();
				return kf;
			}

			if (xn->nodes[1]->token == "hbloc") {
				Traverse(xn->nodes[1], kint->body);
				globalAtanor->Popstack();
				return kf;
			}

			Atanor* ki = C_ParseReturnValue(xn, kfuncbase);
			AtanorInstruction kbloc;

			Traverse(xn->nodes[1], &kbloc); //compiling a return value section
			ki->AddInstruction(kbloc.instructions[0]);
			return_type = kbloc.instructions[0]->Returntype();
			if (return_type != a_none) {
				if (kfuncbase->Returntype() != a_none) {
					if (global->Compatiblestrict(kfuncbase->Returntype(), return_type) == false) {
						stringstream message;
						message << "Type mismatch... Expected: '" << global->Getsymbol(kfuncbase->Returntype()) << "' Proposed: '" << global->Getsymbol(return_type) << "'";
						throw new AtanorRaiseError(message, filename, current_start, current_end);
					}
				}
				else
					kfuncbase->returntype = return_type;
			}
			//In that case, we do not need anything else...
			globalAtanor->Popstack();
			return kint;
		}
	}

	//It is a direct call then
	if (kint == NULL)
		kint = new AtanorCallFunctionHaskell(global, kf);

	kint->Init(NULL);
	AtanorFunctionLambda* kfunc = kint->body;
	AtanorLambdaDomain* lambdadom = &kfunc->lambdadomain;

	//If we have a hdeclared, then we have a return_type...
	if (return_type != -1 && kfunc->returntype == a_null) {		
		if (localhaskelldeclarations.size() != 0) {
			stringstream message;
			message << "Only a return type can declared in a lambda expression";
			throw new AtanorRaiseError(message, filename, current_start, current_end);
		}
		kfunc->returntype = return_type;
		kfunc->hdeclared = true;
	}

	kint->body->choice = 1;

	bool addreturn = false;
	Atanor* kret = NULL;
	globalAtanor->Pushstack(kint);
	globalAtanor->Pushstack(lambdadom);
	globalAtanor->Pushstack(kfunc);

	//Mapping implementation equivalent to:   < operation with &mapping; | &mapping; <- expressions >;

	string xname;
	bool conjunction = false;
	int inc = 1;
	string tok;
	//We implement a pre-declaration of all variables...
	for (i = first; i < xn->nodes.size(); i++) {

		if (xn->nodes[i]->token == "letmin") {
			addreturn = true;
			inc = 0;
			continue;
		}

		if (xn->nodes[i]->token == "range") {
			if (xn->nodes[i]->nodes[0]->token == "let")
				DeclareVariable(xn->nodes[i]->nodes[0]->nodes[1], kfunc);
			else
				DeclareVariable(xn->nodes[i]->nodes[0], lambdadom);

			continue;
		}

		if (xn->nodes[i]->token == "whereexpression")
			Traverse(xn->nodes[i], kint);
	}

	for (i = first + inc; i < xn->nodes.size(); i++) {
		tok = xn->nodes[i]->token;
		if (tok == "whereexpression")
			continue;

		if (tok == "hoperator") {
			conjunction = false;
			if (xn->nodes[i]->value == ";")
				conjunction = true;
			continue;
		}

		if (tok == "letmin") {
			Traverse(xn->nodes[i], kfunc);
			continue;
		}

		if (tok == "hinexpression") {
			kret = C_ParseReturnValue(xn, kfunc, 1);
			Traverse(xn->nodes[i], kret);
			continue;
		}

		if (tok == "hbooleanexpression") {
			kfunc->choice = 1;
			//We add our return statement with our variable...
			Atanor* ktest = new AtanorInstructionHaskellIF(global, kfunc);
			Traverse(xn->nodes[i], ktest);
			Atanor* kobj = ktest;
			bool neg = ktest->Instruction(0)->isNegation();
			Atanor* nxt;
			while (kobj != NULL && kobj->InstructionSize() == 1) {
				if (kobj->isNegation())
					neg = true;
				nxt = kobj;
				kobj = (AtanorObject*)kobj->Instruction(0);
				if (nxt != ktest) {
					nxt->InstructionClear();
					nxt->Remove();
				}
			}
			if (kobj != ktest->Instruction(0))
				((AtanorInstruction*)ktest)->instructions.vecteur[0] = kobj;
			ktest->Setnegation(neg);

			//In order to add it to the test return on true
			kret = new AtanorCallReturn(global, ktest);
			continue;
		}

		if (tok == "hbloc") {
			Traverse(xn->nodes[i], kfunc);
			continue;
		}

		if (tok == "range") {
			xname = xn->nodes[i]->nodes[0]->token;
			if (xname == "let") {
				xname = xn->nodes[i]->nodes[0]->nodes[1]->token;
				if (xname == "valvectortail") {
					BrowseVariableVector(xn->nodes[i]->nodes[0]->nodes[1], kfunc);
					delete xn->nodes[i]->nodes[0]->nodes[0];
					xn->nodes[i]->nodes[0]->nodes.erase(xn->nodes[i]->nodes[0]->nodes.begin());
					xn->nodes[i]->nodes[0]->token = "affectation";
					Traverse(xn->nodes[i]->nodes[0], kfunc);
				}
				else {
					if (xname == "valmaptail") {
						BrowseVariableMap(xn->nodes[i]->nodes[0]->nodes[1], kfunc);
						delete xn->nodes[i]->nodes[0]->nodes[0];
						xn->nodes[i]->nodes[0]->nodes.erase(xn->nodes[i]->nodes[0]->nodes.begin());
						xn->nodes[i]->nodes[0]->token = "affectation";
						Traverse(xn->nodes[i]->nodes[0], kfunc);
					}
					else
						Traverse(xn->nodes[i]->nodes[0], kfunc); //a let, a simple variable affectation...
				}
				continue;
			}

			if (xname == "valvectortail") {
				lambdadom->adding = false;
				lambdadom->local = true;
				BrowseVariableVector(xn->nodes[i]->nodes[0], lambdadom);
				lambdadom->adding = true;
				Traverse(xn->nodes[i]->nodes[0], lambdadom);
			}
			else {
				if (xname == "hvalmaptail") {
					lambdadom->adding = false;
					lambdadom->local = true;
					BrowseVariableMap(xn->nodes[i]->nodes[0], lambdadom);
					lambdadom->adding = true;
					Traverse(xn->nodes[i]->nodes[0], lambdadom);
				}
				else
					BrowseVariable(xn->nodes[i]->nodes[0], lambdadom);
			}

			if (xn->nodes[i]->nodes[1]->token == "hmetafunctions") {
				AtanorCallFunctionHaskell* klocal = new AtanorCallFunctionHaskell(global, lambdadom);
				klocal->Init(NULL);
				Traverse(xn->nodes[i]->nodes[1], klocal);
			}
			else
				Traverse(xn->nodes[i]->nodes[1], lambdadom);

			if (conjunction)
				lambdadom->AddInstruction(aZERO);
			else
				lambdadom->AddInstruction(aFALSE);
		}
	}

	if (!addreturn) {
		if (kret == NULL)
			//we create one, whose value is returned by our return(value) that we add to the end of the function
			kret = C_ParseReturnValue(xn, kfunc);

		if (kret != NULL) {
			//when a variable is requested in the "return" structure, then it has to be reactivated, if it had been deactivated
			//as in <[a,b] | let a=[1..10], let b= filter (<4) a>... a is deactivated (eval is set to 200)
			//see AtanorVariableDeclaration::Create to see why we set eval to 200. It corresponds to a simple storage in a frame
			//without a call to initialisation. When reset to 0, then the initialisation can be activated again...
			//With this mechanism, we do not need to get rid of the specific code associated to such a variable...
			AtanorInstruction kbloc;
			Traverse(xn->nodes[first], &kbloc);
			kret->AddInstruction(kbloc.instructions[0]);
			return_type = kbloc.instructions[0]->Returntype();
			if (return_type != a_none) {
				if (kfunc->Returntype() != a_none) {
					if (global->Compatiblestrict(kfunc->Returntype(), return_type) == false) {
						stringstream message;
						message << "Type mismatch... Expected: '" << global->Getsymbol(kfunc->Returntype()) << "' Proposed: '" << global->Getsymbol(return_type) << "'";
						throw new AtanorRaiseError(message, filename, current_start, current_end);
					}
				}
				else
					kfunc->returntype = return_type;
			}
		}
	}

	globalAtanor->Popstack();
	globalAtanor->Popstack();
	globalAtanor->Popstack();
	if (onepushtoomany)
		//In that case, we need to copy onto the current function (see KIFAPPLY) all the variables that were declared
		//in funcbase, for them to be accessible from the code...			
		globalAtanor->Popstack();

	return kint;
}

Atanor* AtanorCode::C_ParseReturnValue(x_node* xn, AtanorFunctionLambda* kf, char adding) {
	kf->choice = 1;
	if (adding == 1 && kf->instructions.size() != 0 && kf->instructions.back()->Type() == a_if)
		return NULL;

	if (kf->instructions.size() == 0 || (kf->instructions.back()->Type() != a_if && kf->instructions.back()->Type() != a_return))
		return new AtanorCallReturn(global, kf);


	x_node nvar("variable", "&return;", xn);
	x_node* nname = creationxnode("word", nvar.value, &nvar);
	kf->choice = 1;

	Atanor* returnstatement = kf->instructions.back();
	kf->instructions.pop_back();
	AtanorCallReturn* kcallret;
	if (returnstatement->Type() == a_if)
		kcallret = (AtanorCallReturn*)returnstatement->Instruction(1);
	else
		kcallret = (AtanorCallReturn*)returnstatement;

	if (adding == 2) {
		AtanorCallReturn* k = new AtanorCallReturn(global, NULL);
		k->argument = kcallret->argument;
		kcallret = k;
	}


	AtanorInstruction* ki = NULL;
	//We create our variable if necessary...
	if (!kf->declarations.check(a_idreturnvariable)) {
		Atanor* kret = kcallret->argument;
		kcallret->argument = aNOELEMENT;

		Atanor* va = new AtanorSelfDeclarationVariable(global, a_idreturnvariable, a_self, kf);
		kf->Declare(a_idreturnvariable, va);

		ki = AtanorCreateInstruction(NULL, a_affectation);
		Traverse(&nvar, ki);
		//We create an accumulator value in the function instructions...
		ki->AddInstruction(kret);
		ki->Instruction(0)->Setaffectation(true);
		//Its value will be set by the next instructions after the call to C_ParseReturnValue
		kf->AddInstruction(ki);
		Traverse(&nvar, kcallret);
	}

	if (adding == 1) {
		ki = AtanorCreateInstruction(NULL, a_affectation);
		Traverse(&nvar, ki);
		ki->Instruction(0)->Setaffectation(true);

		kf->AddInstruction(ki);
	}
	kf->AddInstruction(returnstatement);
	return ki;
}


Atanor* AtanorCode::C_hlambda(x_node* xn, Atanor* kf) {
	int idname = globalAtanor->Getid("&lambdahaskell;");
	AtanorFunction* kfunc = new AtanorFunction(idname, global);
	Atanor* var;
	for (int i = 0; i < xn->nodes.size() - 1; i++) {
		if (xn->nodes[i]->token == "word") {
			idname = globalAtanor->Getid(xn->nodes[i]->value);
			var = new AtanorSelfDeclarationVariable(global, idname, a_self, kfunc);
			kfunc->Declare(idname, var);
		}
		else
			Traverse(xn->nodes[i], kfunc);
	}
	kfunc->choice = 1;
	var = new AtanorCallReturn(global, kfunc);
	globalAtanor->Pushstack(kfunc);
	AtanorInstruction* kbloc = AtanorCreateInstruction(NULL, a_bloc);
	Traverse(xn->nodes.back(), kbloc);

	if (kbloc->action == a_bloc && kbloc->instructions.size() == 1) {
		Atanor* kroot = kbloc->instructions[0]->Compile(NULL);
		var->AddInstruction(kroot);
		if (kroot != kbloc->instructions[0])
			kbloc->instructions[0]->Remove();
		kbloc->Remove();
	}
	else {
		kbloc = CloningInstruction(kbloc);
		var->AddInstruction(kbloc);
	}

	globalAtanor->Popstack();
	return kfunc;
}


Atanor* AtanorCode::C_hcompose(x_node* xn, Atanor* kbase) {
	if (kbase->Type() != a_callhaskell) {
		AtanorCallFunctionHaskell* kf = new AtanorCallFunctionHaskell(global, kbase);
		kf->Init(NULL);
		return Traverse(xn->nodes[0], kf);
	}
	return Traverse(xn->nodes[0], kbase);
}

Atanor* AtanorCode::C_hfunctioncall(x_node* xn, Atanor* kf) {
	//We rebuild a complete tree, in order to benefit from the regular parsing of a function call		
	if (xn->nodes[0]->token == "telque") {
		AtanorInstruction ai;
		Traverse(xn->nodes[0], &ai);		
		Atanor* calllocal = ai.instructions[0];
		
		AtanorFunctionLambda* kfunc = ((AtanorCallFunctionHaskell*)calllocal)->body;
		AtanorLambdaDomain* lambdadom = &kfunc->lambdadomain;

		//We are dealing with a simple return function
		//The value is stored in a variable, which is one step before the return statement
		//We store the calculus in a intermediary variable, whose name is &return; and which is created
		//with C_ParseReturnValue function...
		if (lambdadom->instructions.size() == 0 && kfunc->instructions.size() == 1) {
			calllocal = kfunc->instructions.back()->Argument(0);
			ai.instructions[0]->Remove();
			kfunc->Remove();
		}

		//This is a specific case, where we have a call without any parameters that has been falsely identify as a variable
		//We need to rebuilt it...
		if (calllocal->isFunctionParameter()) {
			x_node* nop = new x_node("haskellcall", "", xn);

			//this is a call without any variable, which was misinterpreted... It should be another call...
			x_node* nfunc = creationxnode("functioncall", global->Getsymbol(calllocal->Name()), nop);
			nfunc->nodes.push_back(xn->nodes[0]);

			ai.instructions.clear();
			Atanor* kcall = Traverse(nop, &ai);
			nfunc->nodes.clear();
			calllocal->Remove();
			calllocal = kcall;
			delete nop;
		}

		AtanorGetFunctionThrough* ag = new AtanorGetFunctionThrough(calllocal, global, kf);
		for (int i = 1; i < xn->nodes.size(); i++)
			Traverse(xn->nodes[i], ag);
		return ag;
	}

	x_node* nop = new x_node("", "", xn);

	if (xn->nodes[0]->token == "operator") {
		vector<x_node*> nodes;
		nodes.push_back(nop);
		nop->token = "expressions";
		nop->nodes.push_back(xn->nodes[1]);
		x_node* noper;
		x_node* prev = nop;
		int i;
		for (i = 2; i < xn->nodes.size(); i++) {
			noper = new x_node("", "", xn);
			nodes.push_back(noper);
			if (prev == nop) {
				noper->token = "operation";
				prev->nodes.push_back(noper);
			}
			else {
				noper->token = "expressions";
				noper->nodes.push_back(prev->nodes[1]);
				prev->nodes[1] = noper;
				noper->nodes.push_back(new x_node("", "", xn));
				noper = noper->nodes.back();
				nodes.push_back(noper);
				noper->token = "operation";
			}
			noper->nodes.push_back(xn->nodes[0]);
			noper->nodes.push_back(xn->nodes[i]);
			prev = noper;
		}
		Atanor* kcall = Traverse(nop, kf);
		for (i = 0; i < nodes.size(); i++) {
			nodes[i]->nodes.clear();
			delete nodes[i];
		}
		return kcall;

	}
	
	nop->token = "haskellcall";
	x_node* nfunc = creationxnode("functioncall", xn->nodes[0]->value, nop);
	nfunc->nodes.push_back(xn->nodes[0]);

	x_node* param = NULL;

	//The parameters is a recursive structure...
	for (int i = 1; i < xn->nodes.size(); i++) {
		if (param == NULL)
			param = creationxnode("parameters", "", nop);
		param->nodes.push_back(xn->nodes[i]);
	}

	Atanor* kcall = Traverse(nop, kf);

	//We check if it is a data structure, for which we have a variable description...
	// <data TOTO :: TOTO Truc Float> for example...
	short idname = global->Getid(xn->nodes[0]->value);
	if (global->frames.check(idname)) {
		AtanorFrame* frame = global->frames[idname];
		if (frame->variables.size() != param->nodes.size()) {
			stringstream message;
			message << "The number of parameters does not match the data structure definition";
			throw new AtanorRaiseError(message, filename, current_start, current_end);
		}
		//Now for each field in the fname, we need to check if it compatible with the function parameters...
		short ftype, ptype;
		for (int i = 0; i < frame->variables.size(); i++) {
			ftype = frame->variables[i]->Typevariable();
			ptype = kcall->Argument(i)->Typeinfered();
			if (ptype != a_none && !global->Compatiblestrict(ptype, ftype)) {
				stringstream message;
				message << "Type mismatch... Expected: '" << global->Getsymbol(ftype) << "' Proposed: '" << global->Getsymbol(ptype) << "'";
				throw new AtanorRaiseError(message, filename, current_start, current_end);
			}
		}
	}
	nfunc->nodes.clear();
	if (param != NULL)
		param->nodes.clear();
	delete nop;
	return kcall;
}

x_node* Composecalls(x_node* nop, x_node* xn) {
	x_node* nfunc = NULL;
	for (long i = 0; i < xn->nodes.size(); i++) {
		if (nfunc != NULL) {			
			x_node* param = creationxnode("parameters", "", nop);
			nop = creationxnode("haskellcall", "", param);
		}		
		if (xn->nodes[i]->token == "power")
			nfunc = creationxnode("power", xn->nodes[i]->value, nop);
		else
			nfunc = creationxnode("functioncall", xn->nodes[i]->value, nop);
		nfunc->nodes.push_back(xn->nodes[i]);
	}

	xn->nodes.clear();
	return nop;
}

Atanor* AtanorCode::C_mapping(x_node* xn, Atanor* kbase) {
	AtanorCallFunctionHaskell* kf;
	if (kbase->Type() == a_callhaskell)
		kf = (AtanorCallFunctionHaskell*)kbase;
	else {
		kf = new AtanorCallFunctionHaskell(global, kbase);
		kf->Init(NULL);
	}
	AtanorFunctionLambda* kfunc = kf->body;
	AtanorLambdaDomain* lambdadom = &kfunc->lambdadomain;

	x_node nvar("variable", "&common;", xn);
	x_node* nname = creationxnode("word", nvar.value, &nvar);

	globalAtanor->Pushstack(kfunc);
	globalAtanor->Pushstack(lambdadom);

	Atanor* kret;
	//We parse our "list" value
	kf->haskellchoice = 3;
	Traverse(xn->nodes[1], kf);
	//We are dealing with a simple return function
	//The value is stored in a variable, which is one step before the return statement
	//We store the calculus in a intermediary variable, whose name is &return; and which is created
	//with C_ParseReturnValue function...
	if (lambdadom->instructions.size() == 0 && kfunc->instructions.size() == 1) {
		kret = kfunc->instructions.back()->Argument(0);
		if (kret->Type() == a_callhaskell) {
			globalAtanor->Popstack();
			globalAtanor->Popstack();
			kf = (AtanorCallFunctionHaskell*)kret;
			kfunc = kf->body;
			kfunc->choice = 1;
			lambdadom = &kfunc->lambdadomain;
			globalAtanor->Pushstack(kfunc);
			globalAtanor->Pushstack(lambdadom);
		}
	}

	//No composition
	if (lambdadom->instructions.size() == 1) {
		kret = lambdadom->instructions[0];
		AtanorCallFunctionHaskell* kinit = NULL;
		if (kret->Initialisation()->Instruction(0)->Type() == a_callhaskell) {
			kinit = (AtanorCallFunctionHaskell*)kret->Initialisation()->Instruction(0);
			//we look for the most embedded call...
			if (kinit->body->lambdadomain.instructions.size() == 0 && kinit->body->Instruction(0)->Argument(0)->Type() == a_callhaskell)
				kinit = (AtanorCallFunctionHaskell*)kinit->body->Instruction(0)->Argument(0);
		}

		if (kinit != NULL && kinit->body->lambdadomain.instructions.size() != 0) {
			//First we copy all our substructures into our main structure...
			kf->body->lambdadomain.instructions = kinit->body->lambdadomain.instructions;
			kf->body->lambdadomain.declarations = kinit->body->lambdadomain.declarations;
			kf->body->lambdadomain.local = kinit->body->lambdadomain.local;
			kf->body->parameters = kinit->body->parameters;
			kf->body->declarations = kinit->body->declarations;
			kf->body->instructions = kinit->body->instructions;
			//kf->declarations = kinit->declarations;
			char adding = 0;
			if (kf != kbase) {
				adding = 2;
				kbase->AddInstruction(kf);
			}
			Atanor* kvar = kret;
			//if adding is two, then the return section in kinit will be duplicated and left intact...
			kret = C_ParseReturnValue(xn, kfunc, adding);
			if (kret != NULL) {
				nvar.value = "&return;";
				nname->value = "&return;";
			}
		}
		else {
			lambdadom->instructions.clear();
			//we need first to create a variable...
			BrowseVariable(&nvar, lambdadom);
			lambdadom->AddInstruction(kret);
			lambdadom->AddInstruction(aFALSE);

			kret = C_ParseReturnValue(xn, kfunc, 1);

			//This variable becomes our current variable...
			if (kf != kbase)
				kbase->AddInstruction(kf);
		}
	}
	else {
		kret = C_ParseReturnValue(xn, kfunc, 1);
		if (kret == NULL) {
			//Then this is a filter..., A if is detected as a last element in the function instructions set...
			//We create a new AtanorCallFunctionHaskell, in which we copy all that was computed with xn->nodes[1]
			AtanorCallFunctionHaskell* kcf = new AtanorCallFunctionHaskell(global);
			kcf->Init(NULL);
			kcf->body->lambdadomain.instructions = lambdadom->instructions;
			kcf->body->lambdadomain.declarations = lambdadom->declarations;
			kcf->body->lambdadomain.local = lambdadom->local;
			kcf->body->declarations = kfunc->declarations;
			kcf->body->instructions = kfunc->instructions;

			//We clear our current structure, in which we create a CALL to that filter...
			//kcf->declarations = kf->declarations;
			lambdadom->instructions.clear();
			lambdadom->declarations.clear();
			lambdadom->local = false;
			kfunc->instructions.clear();
			kfunc->declarations.clear();

			//The second element, which is the list on which we compute, is then stored
			//in our structure...
			BrowseVariable(&nvar, lambdadom);
			lambdadom->AddInstruction(kcf);
			lambdadom->AddInstruction(aFALSE);

			kret = C_ParseReturnValue(xn, kfunc, 1);
		}
		else {
			nvar.value = "&return;";
			nname->value = "&return;";
		}
	}

	//The next
	//The return value, with either an operation
	if (xn->nodes[0]->token == "inverted") {
		short op = globalAtanor->string_operators[xn->nodes[0]->nodes[1]->value];
		if (op >= a_plus && op <= a_add) {
			AtanorInstructionAPPLYOPERATION kins(NULL);
			kins.action = op;
			Traverse(xn->nodes[0]->nodes[0], &kins);
			Traverse(&nvar, &kins);
			Atanor* kroot = kins.Compile(NULL);
			kret->AddInstruction(kroot);
		}
		else {
			AtanorInstruction* ki = AtanorCreateInstruction(kret, op);
			Traverse(xn->nodes[0]->nodes[0], ki);
			Traverse(&nvar, ki);
		}
	}
	else {
		if (xn->nodes[0]->token == "operator") {
			short op = globalAtanor->string_operators[xn->nodes[0]->value];
			if (op >= a_plus && op <= a_add) {
				AtanorInstructionAPPLYOPERATION kins(NULL);
				kins.action = op;
				Traverse(&nvar, &kins);
				Traverse(&nvar, &kins);
				Atanor* kroot = kins.Compile(NULL);
				kret->AddInstruction(kroot);
			}
			else {
				AtanorInstruction* ki = AtanorCreateInstruction(kret, op);
				Traverse(&nvar, ki);
				Traverse(&nvar, ki);
			}
		}
		else {
			if (xn->nodes[0]->token == "operation") {
				AtanorInstruction* ki = AtanorCreateInstruction(NULL, a_bloc);
				Traverse(&nvar, ki);
				Traverse(xn->nodes[0], ki);

				if (ki->action == a_bloc && ki->instructions.size() == 1) {
					Atanor* kroot = ki->instructions[0]->Compile(NULL);
					kret->AddInstruction(kroot);
					if (kroot != ki->instructions[0])
						ki->instructions[0]->Remove();
					ki->Remove();
				}
				else {
					ki = CloningInstruction(ki);
					kret->AddInstruction(ki);
				}
			}
			else {
				if (xn->nodes[0]->token == "hlambda") {
					Atanor* kprevious = C_hlambda(xn->nodes[0], kfunc);
					AtanorCallFunction* kcall = new AtanorCallFunction((AtanorFunction*)kprevious, global, kret);
					Traverse(&nvar, kcall);
				}
				else { // a function call
					x_node* nop = new x_node("haskellcall", "", xn);					
					x_node* nfunc = Composecalls(nop, xn->nodes[0]->nodes[0]);

					x_node* param = creationxnode("parameters", "", nfunc);				

					for (int i = 1; i < xn->nodes[0]->nodes.size(); i++)
						param->nodes.push_back(xn->nodes[0]->nodes[i]);

					param->nodes.push_back(&nvar);					

					Traverse(nop, kret);
					
					param->nodes.clear();
					delete nop;
				}
			}
		}
	}

	globalAtanor->Popstack();
	globalAtanor->Popstack();
	return kfunc;
}


Atanor* AtanorCode::C_haskellbooling(x_node* xn, Atanor* kbase) {
	AtanorCallFunctionHaskell* kf;
	if (kbase->Type() == a_callhaskell)
		kf = (AtanorCallFunctionHaskell*)kbase;
	else {
		kf = new AtanorCallFunctionHaskell(global, kbase);
		kf->Init(NULL);
	}
	AtanorFunctionLambda* kfunc = kf->body;
	AtanorLambdaDomain* lambdadom = &kfunc->lambdadomain;
	globalAtanor->Pushstack(kfunc);
	globalAtanor->Pushstack(lambdadom);

	char buff[50];
	sprintf_s(buff, 50, "&%s;", xn->nodes[0]->value.c_str());
	//we need first to create a variable...
	x_node nvar("variable", buff, xn);
	creationxnode("word", nvar.value, &nvar);

	BrowseVariable(&nvar, lambdadom);
	Traverse(xn->nodes[2], lambdadom);

	kfunc->choice = 1;
	AtanorInstructionHaskellIF* ktest = new AtanorInstructionHaskellIF(global, kfunc);

	if (xn->nodes[1]->token == "hcomparison" || xn->nodes[1]->token == "comparison" || xn->nodes[1]->token == "operationin") {
		AtanorInstruction* ki = AtanorCreateInstruction(NULL, a_bloc);
		Traverse(&nvar, ki); // we add our variable to compare with
		Traverse(xn->nodes[1], ki); // we add our comparison operator with its value...

		if (ki->action == a_bloc && ki->instructions.size() == 1) {
			Atanor* kroot = ki->instructions[0]->Compile(NULL);
			ktest->AddInstruction(kroot);
			if (kroot != ki->instructions[0])
				ki->instructions[0]->Remove();
			ki->Remove();
		}
		else {
			ki = CloningInstruction(ki);
			ktest->AddInstruction(ki);
		}
	}
	else {
		if (xn->nodes[1]->token == "hboollambda") {
			Atanor* kprevious = C_hlambda(xn->nodes[1], kfunc);
			AtanorCallFunction* kcall = new AtanorCallFunction(kprevious, global, ktest);
			Traverse(&nvar, kcall);
		}
		else {
			x_node* nop = new x_node("haskellcall", "", xn);
			x_node* nfunc = Composecalls(nop, xn->nodes[1]->nodes[0]);

			x_node* param = creationxnode("parameters", "", nfunc);

			for (int i = 1; i < xn->nodes[1]->nodes.size(); i++)
				param->nodes.push_back(xn->nodes[1]->nodes[i]);

			param->nodes.push_back(&nvar);
			
			Traverse(nop, ktest);

			param->nodes.clear();

			delete nop;
		}
	}

	if (xn->nodes[0]->value == "and") {
		//Whenever a value is not true, we break
		ktest->AddInstruction(aNULL);
		ktest->AddInstruction(aBREAKFALSE);
	}
	else {
		ktest->AddInstruction(aBREAKTRUE);
		lambdadom->name = 2; //the name 2 corresponds to a OR
	}

	globalAtanor->Popstack();
	globalAtanor->Popstack();
	return kfunc;

}


Atanor* AtanorCode::C_folding(x_node* xn, Atanor* kbase) {
	AtanorCallFunctionHaskell* kf;
	if (kbase->Type() == a_callhaskell)
		kf = (AtanorCallFunctionHaskell*)kbase;
	else {
		kf = new AtanorCallFunctionHaskell(global, kbase);
		kf->Init(NULL);
	}
	AtanorFunctionLambda* kfunc = kf->body;
	AtanorLambdaDomain* lambdadom = &kfunc->lambdadomain;
	globalAtanor->Pushstack(kfunc);
	globalAtanor->Pushstack(lambdadom);

	char buff[50];
	sprintf_s(buff, 50, "&%s;", xn->nodes[0]->value.c_str());
	//we need first to create a variable...
	x_node nvar("variable", buff, xn);
	creationxnode("word", nvar.value, &nvar);

	//The &folding; <- expressions part...
	BrowseVariable(&nvar, lambdadom);
	if (xn->token == "folding")
		Traverse(xn->nodes[3], lambdadom);
	else
		Traverse(xn->nodes[2], lambdadom);

	//The initial value for the lambda function
	x_node accuvar("variable", "&iaccu;", xn);
	creationxnode("word", accuvar.value, &accuvar);

	BrowseVariable(&accuvar, lambdadom);
	if (xn->token == "folding")
		Traverse(xn->nodes[2], lambdadom);
	else
		lambdadom->AddInstruction(aNOELEMENT);

	//The iterator direction
	Atanor* choice;
	string act = xn->nodes[0]->nodes[0]->token;
	if (act[0] == 'l') {
		if (act[1] == 's')
			choice = aZERO; //scanning from left to right
		else
			choice = aFALSE; //folding from left to right
	}
	else {
		if (act[1] == 's')
			choice = aONE; //scanning right to left
		else
			choice = aTRUE; //folding right to left
	}

	lambdadom->AddInstruction(choice);

	kfunc->choice = 1;
	Atanor* kret = C_ParseReturnValue(xn, kfunc);

	if (xn->nodes[1]->token == "operator") {
		short op = globalAtanor->string_operators[xn->nodes[1]->value];
		if (op >= a_plus && op <= a_add) {
			AtanorInstructionAPPLYOPERATION kins(NULL);
			kins.action = op;
			Traverse(&accuvar, &kins);
			Traverse(&nvar, &kins);
			Atanor* kroot = kins.Compile(NULL);
			kret->AddInstruction(kroot);
		}
		else {
			AtanorInstruction* ki = AtanorCreateInstruction(kret, op);
			Traverse(&accuvar, ki);
			Traverse(&nvar, ki);
		}
	}
	else {
		AtanorCallFunction* kcall;
		if (xn->nodes[1]->token == "hlambda") {
			Atanor* kprevious = C_hlambda(xn->nodes[1], kfunc);
			kcall = new AtanorCallFunction((AtanorFunction*)kprevious, global, kret);
			if (choice->Boolean() == false) {
				Traverse(&accuvar, kcall);
				Traverse(&nvar, kcall);
			}
			else {
				Traverse(&nvar, kcall);
				Traverse(&accuvar, kcall);
			}
		}
		else {
			x_node* nop = new x_node("haskellcall", "", xn);
			x_node* nfunc = Composecalls(nop, xn->nodes[1]->nodes[0]);

			x_node* param = creationxnode("parameters", "", nfunc);
			
			for (int i = 1; i < xn->nodes[1]->nodes.size(); i++)
				param->nodes.push_back(xn->nodes[1]->nodes[i]);

			if (choice->Boolean() == true) {
				param->nodes.push_back(&nvar);
				param->nodes.push_back(&accuvar);
			}
			else {
				param->nodes.push_back(&accuvar);
				param->nodes.push_back(&nvar);
			}
			

			Traverse(nop, kret);

			param->nodes.clear();
			delete nop;
		}
	}

	globalAtanor->Popstack();
	globalAtanor->Popstack();
	return kfunc;

}



Atanor* AtanorCode::C_zipping(x_node* xn, Atanor* kbase) {
	AtanorCallFunctionHaskell* kf;
	if (kbase->Type() == a_callhaskell) {
		kf = (AtanorCallFunctionHaskell*)kbase;
	}
	else {
		kf = new AtanorCallFunctionHaskell(global);
		kf->Init(NULL);
	}
	AtanorFunctionLambda* kfunc = kf->body;
	AtanorLambdaDomain* lambdadom = &kfunc->lambdadomain;
	globalAtanor->Pushstack(kfunc);
	globalAtanor->Pushstack(lambdadom);

	vector<x_node*> nvars;
	char buff[50];
	int first = 1;
	int i;
	if (xn->token == "pairing")
		first = 0;

	for (i = first; i < xn->nodes.size(); i++) {
		//we need first to create a variable...
		sprintf_s(buff, 50, "&zipping%d", i);
		x_node* nvar = new x_node("variable", buff, xn);
		creationxnode("word", nvar->value, nvar);

		BrowseVariable(nvar, lambdadom);
		Traverse(xn->nodes[i], lambdadom);
		if (i == first)
			lambdadom->AddInstruction(aFALSE);
		else
			lambdadom->AddInstruction(aZERO);
		nvars.push_back(nvar);
	}

	kfunc->choice = 1;
	Atanor* kret = C_ParseReturnValue(xn, kfunc);

	if (!first) {
		AtanorConstvector* kvect = new AtanorConstvector(global, kret);
		kvect->evaluate = true;
		for (i = 0; i < nvars.size(); i++) {
			Traverse(nvars[i], kvect);
			delete nvars[i];
		}
	}
	else {
		if (xn->nodes[0]->token == "operator") {
			short op = globalAtanor->string_operators[xn->nodes[0]->value];
			if (op >= a_plus && op <= a_add) {
				AtanorInstructionAPPLYOPERATION kins(NULL);				
				kins.action = op;
				for (i = 0; i < nvars.size(); i++) {
					Traverse(nvars[i], &kins);
					delete nvars[i];
				}
				Atanor* kroot = kins.Compile(NULL);
				kret->AddInstruction(kroot);
			}
			else {
				AtanorInstruction* ki = AtanorCreateInstruction(NULL, op);
				for (i = 0; i < nvars.size(); i++) {
					Traverse(nvars[i], ki);
					delete nvars[i];
				}
				kret->AddInstruction(ki);
			}
		}
		else {
			if (xn->nodes[0]->token == "hlambda") {
				Atanor* kprevious = C_hlambda(xn->nodes[0], kfunc);
				AtanorCallFunction* kcall = new AtanorCallFunction((AtanorFunctionLambda*)kprevious, global, kret);
				for (i = 0; i < nvars.size(); i++) {
					Traverse(nvars[i], kcall);
					delete nvars[i];
				}
			}
			else {
				x_node* nop = new x_node("haskellcall", "", xn);
				x_node* nfunc = Composecalls(nop, xn->nodes[0]->nodes[0]);

				x_node* param = creationxnode("parameters", "", nfunc);
				
				for (i = 1; i < xn->nodes[0]->nodes.size(); i++)
					param->nodes.push_back(xn->nodes[0]->nodes[i]);

				for (i = 0; i < nvars.size(); i++)
					param->nodes.push_back(nvars[i]);				

				Traverse(nop, kret);

				param->nodes.clear();

				delete nop;
			}
		}
	}
	globalAtanor->Popstack();
	globalAtanor->Popstack();
	return kfunc;
}



Atanor* AtanorCode::C_filtering(x_node* xn, Atanor* kbase) {
	AtanorCallFunctionHaskell* kf;
	AtanorFunctionLambda* kfunc;
	AtanorLambdaDomain* lambdadom;
	if (kbase->Type() == a_callhaskell) {
		kf = (AtanorCallFunctionHaskell*)kbase;
	}
	else {
		kf = new AtanorCallFunctionHaskell(global);
		kf->Init(NULL);
	}

	Atanor* kret = NULL;
	kfunc = kf->body;
	kfunc->choice = 1;
	lambdadom = &kfunc->lambdadomain;

	x_node nvar("variable", "&common;", xn);
	x_node* nname = creationxnode("word", nvar.value, &nvar);

	globalAtanor->Pushstack(kfunc);
	globalAtanor->Pushstack(lambdadom);

	kf->haskellchoice = 3;
	Traverse(xn->nodes[2], kf);
	if (lambdadom->instructions.size() == 0 && kfunc->instructions.size() == 1) {
		kret = kfunc->instructions.back()->Argument(0);
		if (kret->Type() == a_callhaskell) {
			globalAtanor->Popstack(); //A call to a Haskell function
			globalAtanor->Popstack();
			kf = (AtanorCallFunctionHaskell*)kret;
			kfunc = kf->body;
			kfunc->choice = 1;
			lambdadom = &kfunc->lambdadomain;
			globalAtanor->Pushstack(kfunc);
			globalAtanor->Pushstack(lambdadom);
		}
	}

	if (lambdadom->instructions.size() == 1) {
		kret = lambdadom->instructions[0];
		AtanorCallFunctionHaskell* kinit = NULL;
		if (kret->Initialisation()->Instruction(0)->Type() == a_callhaskell) {
			kinit = (AtanorCallFunctionHaskell*)kret->Initialisation()->Instruction(0);
			//we look for the most embedded call...
			if (kinit->body->lambdadomain.instructions.size() == 0 && kinit->body->Instruction(0)->Argument(0)->Type() == a_callhaskell)
				kinit = (AtanorCallFunctionHaskell*)kinit->body->Instruction(0)->Argument(0);
		}

		if (kinit != NULL && kinit->body->lambdadomain.instructions.size() != 0) {
			//First we copy all our substructures into our main structure...
			kf->body->lambdadomain.instructions = kinit->body->lambdadomain.instructions;
			kf->body->lambdadomain.declarations = kinit->body->lambdadomain.declarations;
			kf->body->lambdadomain.local = kinit->body->lambdadomain.local;
			kf->body->parameters = kinit->body->parameters;
			kf->body->declarations = kinit->body->declarations;
			kf->body->instructions = kinit->body->instructions;
			//kf->declarations = kinit->declarations;
			char adding = 0;
			if (kf != kbase) {
				adding = 2;
				kbase->AddInstruction(kf);
			}
			Atanor* kvar = kret;
			//if adding is two, then the return section in kinit will be duplicated and left intact...
			kret = C_ParseReturnValue(xn, kfunc, adding);
			if (kret != NULL) {
				nvar.value = "&return;";
				nname->value = "&return;";
			}
		}
		else {
			lambdadom->instructions.clear();
			//we need first to create a variable...
			BrowseVariable(&nvar, lambdadom);
			lambdadom->AddInstruction(kret);
			lambdadom->AddInstruction(aFALSE);
			//This variable becomes our current variable...

			kret = C_ParseReturnValue(xn, kfunc);
			Traverse(&nvar, kret);
			if (kf != kbase)
				kbase->AddInstruction(kf);
		}
	}
	else {
		C_ParseReturnValue(xn, kfunc);
		nvar.value = "&return;";
		nname->value = "&return;";
	}

	x_node nvardrop("variable", "&drop;", xn);

	if (xn->nodes[0]->value == "dropWhile") {
		creationxnode("word", nvardrop.value, &nvardrop);

		int iddrop = globalAtanor->Getid(nvardrop.value);
		Atanor* var = new AtanorSelfDeclarationVariable(global, iddrop, a_self, kf);
		var = new Atanorbool(true, global, var);
		lambdadom->Declare(iddrop, var);
		lambdadom->local = true;
	}

	//We remove the last instruction, to insert it into our test
	kret = kfunc->instructions.back();
	kfunc->instructions.pop_back();
	AtanorInstructionHaskellIF* ktest;

	//The return statement should be removed and replaced
	if (xn->nodes[0]->value == "take" || xn->nodes[0]->value == "drop") {//In that case, we need to count the number of elements that were used so far...
		//First we need to declare a variable which will be used as a counter...
		nvar.value = "&counter;"; //Our counter
		nname->value = "&counter;";
		Atanor* var = new AtanorSelfDeclarationVariable(global, a_counter);
		lambdadom->Declare(a_counter, var);
		var->AddInstruction(aZERO);
		lambdadom->local = true;
		//We add a PLUSPLUS to increment our value...
		var = new AtanorCallSelfVariable(a_counter, a_self, global, kfunc);
		var = new AtanorPLUSPLUS(global, var);
		//Then we need to add our test
		ktest = new AtanorInstructionHaskellIF(global, kfunc);
		AtanorInstruction* ki;
		if (xn->nodes[0]->value == "drop")
			ki = AtanorCreateInstruction(ktest, a_more);
		else
			ki = AtanorCreateInstruction(ktest, a_lessequal);
		Traverse(&nvar, ki);
		Traverse(xn->nodes[1], ki);
	}
	else {
		ktest = new AtanorInstructionHaskellIF(global, kfunc);
		//The only difference is that we process a Boolean expression in a filter
		if (xn->nodes[1]->token == "hcomparison" || xn->nodes[1]->token == "comparison" || xn->nodes[1]->token == "operationin") {
			AtanorInstruction* ki = AtanorCreateInstruction(NULL, a_bloc);
			Traverse(&nvar, ki); // we add our variable to compare with
			Traverse(xn->nodes[1], ki); // we add our comparison operator with its value...

			if (ki->action == a_bloc && ki->instructions.size() == 1) {
				Atanor* kroot = ki->instructions[0]->Compile(NULL);
				ktest->AddInstruction(kroot);
				if (kroot != ki->instructions[0])
					ki->instructions[0]->Remove();
				ki->Remove();
			}
			else {
				ki = CloningInstruction(ki);
				ktest->AddInstruction(ki);
			}
		}
		else {
			if (xn->nodes[1]->token == "hboollambda") {
				Atanor* kprevious = C_hlambda(xn->nodes[1], kfunc);
				AtanorCallFunction* kcall = new AtanorCallFunction((AtanorFunction*)kprevious, global, ktest);
				Traverse(&nvar, kcall);
			}
			else {
				x_node* nop = new x_node("haskellcall", "", xn);
				x_node* nfunc = Composecalls(nop, xn->nodes[1]->nodes[0]);

				x_node* param = creationxnode("parameters", "", nfunc);
				
				for (int i = 1; i < xn->nodes[1]->nodes.size(); i++)
					param->nodes.push_back(xn->nodes[1]->nodes[i]);

				param->nodes.push_back(&nvar);
				
				Traverse(nop, ktest);

				param->nodes.clear();
				delete nop;
			}
		}
	}

	if (xn->nodes[0]->value == "dropWhile") {
		//Then, in that case, when the test is positive, we return aNULL else the value
		//First we modify the test in ktest...
		//We need to use a Boolean (&drop;) which will be set to false, when the test will be true...
		AtanorInstruction* ki = AtanorCreateInstruction(NULL, a_conjunction);
		Traverse(&nvardrop, ki);
		ki->AddInstruction(ktest->instructions[0]);
		ktest->instructions.vecteur[0] = ki;
		AtanorCallReturn* kretdrop = new AtanorCallReturn(global, ktest);
		kretdrop->AddInstruction(aNULL);
		//We need now a sequence of instructions
		AtanorSequence* kseq = new AtanorSequence(global, ktest);
		//the variable &drop; is set to false
		ki = AtanorCreateInstruction(kseq, a_affectation);
		Traverse(&nvardrop, ki);
		ki->Instruction(0)->Setaffectation(true);
		ki->AddInstruction(aFALSE);
		//we add our return value...
		kseq->AddInstruction(kret);
	}
	else {
		ktest->AddInstruction(kret); //We add the value to return if test is positive
		if (xn->nodes[0]->value != "filter" && xn->nodes[0]->value != "drop")
			kret = new AtanorBreak(global, ktest);
	}
	globalAtanor->Popstack();
	globalAtanor->Popstack();
	return kfunc;
}


Atanor* AtanorCode::C_flipping(x_node* xn, Atanor* kbase) {
	AtanorCallFunctionHaskell* kf;
	if (kbase->Type() == a_callhaskell) {
		kf = (AtanorCallFunctionHaskell*)kbase;
	}
	else {
		kf = new AtanorCallFunctionHaskell(global);
		kf->Init(NULL);
	}
	AtanorFunctionLambda* kfunc = kf->body;
	AtanorLambdaDomain* lambdadom = &kfunc->lambdadomain;
	globalAtanor->Pushstack(kfunc);
	globalAtanor->Pushstack(lambdadom);

	int i;
	AtanorInstruction kvar;


	kfunc->choice = 1;
	Atanor* kret = C_ParseReturnValue(xn, kfunc);

	if (xn->nodes[0]->token == "operator") {
		short op = globalAtanor->string_operators[xn->nodes[0]->value];
		if (op >= a_plus && op <= a_add) {
			AtanorInstructionAPPLYOPERATION kins(NULL);
			kins.action = op;
			Traverse(xn->nodes[2], &kins);
			Traverse(xn->nodes[1], &kins);
			Atanor* kroot = kins.Compile(NULL);
			kret->AddInstruction(kroot);
		}
		else {
			AtanorInstruction* ki = AtanorCreateInstruction(NULL, op);
			Traverse(xn->nodes[2], ki);
			Traverse(xn->nodes[1], ki);
			kret->AddInstruction(ki);
		}
	}
	else {
		if (xn->nodes[0]->token == "hlambda") {
			Atanor* kprevious = C_hlambda(xn->nodes[0], kfunc);
			AtanorCallFunction* kcall = new AtanorCallFunction((AtanorFunctionLambda*)kprevious, global, kret);
			Traverse(xn->nodes[2], kcall);
			Traverse(xn->nodes[1], kcall);
		}
		else {
			x_node* nop = new x_node("haskellcall", "", xn);
			x_node* nfunc = Composecalls(nop, xn->nodes[0]->nodes[0]);

			x_node* param = NULL;

			for (i = 2; i >= 1; i--) {
				if (param == NULL)
					x_node* param = creationxnode("parameters", "", nfunc);
				param->nodes.push_back(xn->nodes[i]);
			}

			Traverse(nop, kret);

			if (param != NULL)
				param->nodes.clear();
			
			delete nop;
		}
	}
	globalAtanor->Popstack();
	globalAtanor->Popstack();
	return kfunc;
}

Atanor* AtanorCode::C_cycling(x_node* xn, Atanor* kbase) {//Cycling in a list...
	AtanorCallFunctionHaskell* kf;
	if (kbase->Type() == a_callhaskell)
		kf = (AtanorCallFunctionHaskell*)kbase;
	else {
		kf = new AtanorCallFunctionHaskell(global, kbase);
		kf->Init(NULL);
	}
	AtanorFunctionLambda* kfunc = kf->body;
	AtanorLambdaDomain* lambdadom = &kfunc->lambdadomain;

	x_node nvar("variable", "&common;", xn);
	x_node* nname = creationxnode("word", nvar.value, &nvar);

	globalAtanor->Pushstack(kfunc);
	globalAtanor->Pushstack(lambdadom);

	Atanor* kret;
	//We parse our "list" value
	kf->haskellchoice = 3;
	Traverse(xn->nodes[1], kf);
	//We are dealing with a simple return function
	//The value is stored in a variable, which is one step before the return statement
	//We store the calculus in a intermediary variable, whose name is &return; and which is created
	//with C_ParseReturnValue function...
	if (lambdadom->instructions.size() == 0 && kfunc->instructions.size() == 1) {
		kret = kfunc->instructions.back()->Argument(0);
		if (kret->Type() == a_callhaskell) {
			globalAtanor->Popstack();
			globalAtanor->Popstack();
			kf = (AtanorCallFunctionHaskell*)kret;
			kfunc = kf->body;
			kfunc->choice = 1;
			lambdadom = &kfunc->lambdadomain;
			globalAtanor->Pushstack(kfunc);
			globalAtanor->Pushstack(lambdadom);
		}
	}

	//No composition
	if (lambdadom->instructions.size() == 1) {
		kret = lambdadom->instructions[0];
		AtanorCallFunctionHaskell* kinit = NULL;
		if (kret->Initialisation()->Instruction(0)->Type() == a_callhaskell) {
			kinit = (AtanorCallFunctionHaskell*)kret->Initialisation()->Instruction(0);
			//we look for the most embedded call...
			if (kinit->body->lambdadomain.instructions.size() == 0 && kinit->body->Instruction(0)->Argument(0)->Type() == a_callhaskell)
				kinit = (AtanorCallFunctionHaskell*)kinit->body->Instruction(0)->Argument(0);
		}

		if (kinit != NULL && kinit->body->lambdadomain.instructions.size() != 0) {
			//First we copy all our substructures into our main structure...
			kf->body->lambdadomain.instructions = kinit->body->lambdadomain.instructions;
			kf->body->lambdadomain.declarations = kinit->body->lambdadomain.declarations;
			kf->body->lambdadomain.local = kinit->body->lambdadomain.local;
			kf->body->parameters = kinit->body->parameters;
			kf->body->declarations = kinit->body->declarations;
			kf->body->instructions = kinit->body->instructions;
			//kf->declarations = kinit->declarations;
			char adding = 0;
			if (kf != kbase) {
				adding = 2;
				kbase->AddInstruction(kf);
			}
			Atanor* kvar = kret;
			//if adding is two, then the return section in kinit will be duplicated and left intact...
			kret = C_ParseReturnValue(xn, kfunc, adding);
			if (kret != NULL) {
				nvar.value = "&return;";
				nname->value = "&return;";
			}
		}
		else {
			lambdadom->instructions.clear();
			//we need first to create a variable...
			BrowseVariable(&nvar, lambdadom);
			lambdadom->AddInstruction(kret);
			lambdadom->AddInstruction(aFALSE);

			kret = C_ParseReturnValue(xn, kfunc, 1);

			//This variable becomes our current variable...
			if (kf != kbase)
				kbase->AddInstruction(kf);
		}
	}
	else {
		kret = C_ParseReturnValue(xn, kfunc, 1);
		if (kret == NULL) {
			//Then this is a filter..., A if is detected as a last element in the function instructions set...
			//We create a new AtanorCallFunctionHaskell, in which we copy all that was computed with xn->nodes[1]
			AtanorCallFunctionHaskell* kcf = new AtanorCallFunctionHaskell(global);
			kcf->Init(NULL);
			kcf->body->lambdadomain.instructions = lambdadom->instructions;
			kcf->body->lambdadomain.declarations = lambdadom->declarations;
			kcf->body->lambdadomain.local = lambdadom->local;
			kcf->body->declarations = kfunc->declarations;
			kcf->body->instructions = kfunc->instructions;

			//We clear our current structure, in which we create a CALL to that filter...
			//kcf->declarations = kf->declarations;
			lambdadom->instructions.clear();
			lambdadom->declarations.clear();
			lambdadom->local = false;
			kfunc->instructions.clear();
			kfunc->declarations.clear();

			//The second element, which is the list on which we compute, is then stored
			//in our structure...
			BrowseVariable(&nvar, lambdadom);
			lambdadom->AddInstruction(kcf);
			lambdadom->AddInstruction(aFALSE);

			kret = C_ParseReturnValue(xn, kfunc, 1);
		}
		else {
			nvar.value = "&return;";
			nname->value = "&return;";
		}
	}

	//Now we modify lambdadom a little bit...
	if (xn->nodes[0]->value == "repeat")
		lambdadom->instructions.vecteur[1] = new AtanorCycleVector(lambdadom->instructions[1], true, global);
	else
	if (xn->nodes[0]->value == "cycle")
		lambdadom->instructions.vecteur[1] = new AtanorCycleVector(lambdadom->instructions[1], false, global);
	else {
		lambdadom->instructions.vecteur[1] = new AtanorReplicateVector(lambdadom->instructions[1], global);
		Traverse(xn->nodes[0], lambdadom->instructions[1]);
	}

	//we simply return our value...
	Traverse(&nvar, kret);
	globalAtanor->Popstack();
	globalAtanor->Popstack();
	return kfunc;
}



Atanor* AtanorCode::C_haskellexpression(x_node* xn, Atanor* kf) {
	int id;
	Atanor* var;
	if (xn->nodes[0]->token == "word") {
		id = globalAtanor->Getid(xn->nodes[0]->value);
		if (kf->isDeclared(id)) {
			//In this case, we do not need to return an error
			//It is a case construction within a function...
			stringstream message;
			message << "Variable: '" << globalAtanor->Getsymbol(id) << "' has already been declared";
			throw new AtanorRaiseError(message, filename, current_start, current_end);
		}

		var = new AtanorSelfDeclarationVariable(global, id);
		kf->Declare(id, var);
		return var;
	}
	else
		var = Traverse(xn->nodes[0], kf);
	return var;
}


Atanor* AtanorCode::C_haskellmap(x_node* xn, Atanor* kf) {

	AtanorConstmap* kmap = new AtanorConstmap(global, kf);
	char ch = kf->Choice(); //Only the top Haskell Vector can be an argument
	kf->Setchoice(1);
	Atanor* var;
	for (int i = 0; i < xn->nodes.size(); i++) {
		if (xn->nodes[i]->token == "haskellmaptail") {
			kmap->keys.push_back(aPIPE);
			var = Traverse(xn->nodes[i], kf);
			kmap->values.push_back(var);
			break;
		}
		var = Traverse(xn->nodes[i]->nodes[0], kf);
		kmap->keys.push_back(var);
		var = Traverse(xn->nodes[i]->nodes[1], kf);
		kmap->values.push_back(var);
	}
	kf->Setchoice(ch);
	return kmap;
}



Atanor* AtanorCode::C_haskellvector(x_node* xn, Atanor* kf) {

	AtanorConstvector* kvect = new AtanorConstvector(global, kf);
	char ch = kf->Choice(); //Only the top Haskell Vector can be an argument
	kf->Setchoice(1);
	Atanor* var;
	for (int i = 0; i < xn->nodes.size(); i++) {
		if (xn->nodes[i]->token == "haskelltail") {
			AtanorConstvectormerge* kbv = new AtanorConstvectormerge(global, NULL);
			kvect->values.push_back(kbv);
			var = Traverse(xn->nodes[i]->nodes[0], kf);
			kbv->values.push_back(var);
			break;
		}
		var = Traverse(xn->nodes[i], kf);
		var->Setreference();
		kvect->values.push_back(var);
	}
	kf->Setchoice(ch);
	return kvect;
}



Atanor* AtanorCode::C_whereexpression(x_node* xn, Atanor* kf) {
	AtanorVariableDeclaration* var;
	int idname;
	AtanorCallFunctionHaskell* kint = (AtanorCallFunctionHaskell*)kf;
	kint->body->lambdadomain.local = true;
	for (int i = 0; i < xn->nodes.size(); i++) {
		if (xn->nodes[i]->token == "word") {
			idname = globalAtanor->Getid(xn->nodes[i]->value);
			if (kint->body->lambdadomain.declarations.check(idname)) {
				stringstream message;
				message << "Variable: '" << xn->nodes[i]->value << "' already declared";
				throw new AtanorRaiseError(message, filename, current_start, current_end);
			}

			var = new AtanorSelfDeclarationVariable(global, idname);
			kint->body->lambdadomain.Declare(idname, var);
			kint->body->instructions.push_back(var);
			AtanorInstruction kres;
			Traverse(xn->nodes[i + 1], &kres);
			var->initialization = kres.instructions[0];
			var->Setaffectation(true);
			i++;
		}
		else
		if (xn->nodes[i]->token == "hbloc")
			Traverse(xn->nodes[i], kint->body);
		else
			Traverse(xn->nodes[i], kint);
	}
	return kf;
}



Atanor* AtanorCode::C_hinexpression(x_node* xn, Atanor* kf) {
	AtanorInstruction kbloc;

	//we must feed our return section with this value...
	Traverse(xn->nodes[0], &kbloc);
	kf->AddInstruction(kbloc.instructions[0]);
	if (xn->value == "notin")
		kf->Setnegation(true);
	return kf;
}


Atanor* AtanorCode::C_letmin(x_node* xn, Atanor* kf) {
	//We do some makeup in order to transform a letmin into a let
	x_node* n = new x_node("letkeyword", "let", xn);
	xn->token = "let";
	xn->nodes.insert(xn->nodes.begin(), n);
	Traverse(xn, kf);//then we can compile it as if it was a known let structure...
	return kf;
}

Atanor* AtanorCode::C_haskellcase(x_node* xn, Atanor* kf) {
	Atanor* kres = NULL;
	AtanorInstructionHaskellMainCASE* ktest = NULL;
	AtanorInstruction* thetest;
	Atanor* kfirst = NULL;
	AtanorFunctionLambda* kfunc = NULL;
	AtanorCallReturn* kret;

	string xname;
	for (int i = 1; i < xn->nodes.size(); i++) {
		if (xn->nodes[i]->token == "haskellcaseotherwise")
			break;
		
		xname = xn->nodes[i]->nodes[0]->token;
		if (xname == "valvectortail") {
			//In this case, we need to create a function, in which everything is going to get executed
			//We might need to declare variables, hence this function declaration...
			if (kfunc == NULL) {
				kfunc = new AtanorFunctionLambda(ATANORCASEFUNCTION, global);
				kf->AddInstruction(kfunc);
				kfunc->adding = true;
				globalAtanor->Pushstack(kfunc);
			}
			BrowseVariableVector(xn->nodes[i]->nodes[0], kfunc);
		}

		if (xname == "valmaptail") {
			//In this case, we need to create a function, in which everything is going to get executed
			//We might need to declare variables, hence this function declaration...
			if (kfunc == NULL) {
				kfunc = new AtanorFunctionLambda(ATANORCASEFUNCTION, global);
				kf->AddInstruction(kfunc);
				kfunc->adding = true;
				globalAtanor->Pushstack(kfunc);
			}
			BrowseVariableMap(xn->nodes[i]->nodes[0], kfunc);
		}
	}

	ktest = new AtanorInstructionHaskellMainCASE(global);

	for (int i = 1; i < xn->nodes.size(); i++) {
		if (xn->nodes[i]->token == "haskellcaseotherwise") {
			ktest->other = 1;
			if (kfunc == NULL)
				Traverse(xn->nodes[i]->nodes[0], ktest); //then the value
			else {//In the case of a function we need to officially return the value...
				kret = new AtanorCallReturn(global, ktest);
				thetest = AtanorCreateInstruction(NULL, a_bloc);
				Traverse(xn->nodes[i]->nodes[0], thetest);

				if (thetest->action == a_bloc && thetest->instructions.size() == 1) {
					Atanor* kroot = thetest->instructions[0]->Compile(NULL);
					kret->AddInstruction(kroot);
					if (kroot != thetest->instructions[0])
						thetest->instructions[0]->Remove();
					thetest->Remove();
				}
				else {
					thetest = CloningInstruction(thetest);
					kret->AddInstruction(thetest);
				}
			}
			break;
		}				
		//It should a be match between two values, it will be a match
		thetest = AtanorCreateInstruction(ktest, a_match);
		Traverse(xn->nodes[0], thetest);
		Traverse(xn->nodes[i]->nodes[0], thetest); //the comparison
		if (kfunc == NULL)
			Traverse(xn->nodes[i]->nodes[1], ktest); //then the value
		else {//In the case of a function we need to officially return the value...
			kret = new AtanorCallReturn(global, ktest);
			thetest = AtanorCreateInstruction(NULL, a_bloc);
			Traverse(xn->nodes[i]->nodes[1], thetest);
			if (thetest->action == a_bloc && thetest->instructions.size() == 1) {
				Atanor* kroot = thetest->instructions[0]->Compile(NULL);
				kret->AddInstruction(kroot);
				if (kroot != thetest->instructions[0])
					thetest->instructions[0]->Remove();
				thetest->Remove();
			}
			else {
				thetest = CloningInstruction(thetest);
				kret->AddInstruction(thetest);
			}
		}
	}

	if (kfunc == NULL) //No function, we can add our structure directly...
		kf->AddInstruction(ktest);
	else {
		//We then transform the declarations into instructions
		bin_hash<Atanor*>::iterator it;
		for (it = kfunc->declarations.begin(); it != kfunc->declarations.end(); it++) {
			if (it->first != ATANORCASEFUNCTION)
				kfunc->instructions.push_back(it->second);
		}
		kfunc->instructions.push_back(ktest);
		globalAtanor->Popstack();
	}

	return kf;
}


//Similar to haskellcase, however we analyse blocs of three elements...
Atanor* AtanorCode::C_guard(x_node* xn, Atanor* kf) {

	AtanorInstructionHaskellMainCASE* ktest = new AtanorInstructionHaskellMainCASE(global);

	while (xn->nodes.size() == 3) {		
		Traverse(xn->nodes[0], ktest);//the comparison
		Traverse(xn->nodes[1], ktest); //then the value
		xn = xn->nodes.back();
	}

	if (xn->token == "otherwise") {
		ktest->other = 1;
		Traverse(xn->nodes[0], ktest); //then the value
	}

	kf->AddInstruction(ktest);
	return kf;
}


Atanor* AtanorCode::C_alist(x_node* xn, Atanor* kf) {
	//AtanorInstruction* kbloc=new AtanorInstruction(global,kf,a_bloc);

	if (xn->token == "merging") {
		//we create a sub-vector
		AtanorConstvector* kbv = new AtanorConstvectormerge(global, NULL);
		Traverse(xn->nodes[0], kbv);
		kf->push(kbv);
		return kbv;
	}

	AtanorInstruction kbloc;
	Traverse(xn->nodes[0], &kbloc);
	Atanor* ke = kbloc.instructions[0];
	kf->push(ke);
	if (xn->nodes.size() == 2)
		Traverse(xn->nodes[1], kf);
	return ke;
}

Atanor* AtanorCode::C_cut(x_node* xn, Atanor* kf) {
	if (xn->token == "cut")
		kf->AddInstruction(globalAtanor->gCUT);
	else {
		if (xn->token == "fail")
			kf->AddInstruction(globalAtanor->gFAIL);
		else
			kf->AddInstruction(globalAtanor->gSTOP);
	}
	return kf;
}

//A predicate container contains the rules that have been declared either at the top level or in a frame...
AtanorPredicateContainer* AtanorGlobal::Predicatecontainer() {
	if (predicateContainer == NULL)
		predicateContainer = new AtanorPredicateContainer(this);
	return predicateContainer;
}

Atanor* AtanorCode::AtanorParsePredicateDCGVariable(string& name, Atanor* kcf, bool param) {
	//if (param)
	//	kcf = new AtanorInstructionPARAMETER(global, kcf);

	AtanorPredicateVariable* var;
	if (globalAtanor->predicatevariables.find(name) == globalAtanor->predicatevariables.end()) {
		var = new AtanorPredicateVariable(global, globalAtanor->Getid(name), kcf);
		globalAtanor->predicatevariables[name] = var;
	}
	else {
		var = globalAtanor->predicatevariables[name];
		kcf->AddInstruction(var);
	}
	return var;
}

void AtanorCode::ComputePredicateParameters(x_node* xn, Atanor* kcf) {
	short id = a_parameterpredicate;
	AtanorInstruction kbloc(id);

	for (int i = 0; i < xn->nodes.size(); i++) {
		kbloc.idtype = id;
		Traverse(xn->nodes[i], &kbloc);
		kcf->AddInstruction(kbloc.instructions[0]);
		kbloc.instructions.clear();
	}
}



// A call to a predicate expression
//We have three possibilities, which must be taken into account:
//1)  predicate(x,y):-true; in this case, this predicate is appended to the knowledge base
//2)  predicate(x,y):-false; in this case, it is removed from the knwoledge base
//3) predicate(x,y) :-  expression, then we must evaluate the expression to add this predicate
Atanor* AtanorCode::C_predicatefact(x_node* xn, Atanor* kf) {
	//We create an expression
	//If it is a boolean value (either true or false)
	AtanorPredicateContainer* kpcont = globalAtanor->Predicatecontainer();

	string sname = xn->nodes[0]->nodes[0]->value;
	short name = globalAtanor->Getid(sname);
	if (!globalAtanor->predicates.check(name))
		globalAtanor->predicates[name] = new AtanorPredicateFunction(global, NULL, name);

	globalAtanor->predicatevariables.clear();
	if (xn->token == "predicatefact" || xn->nodes[1]->token == "abool") {
		Atanor* kbloc = new AtanorInstructionPredicate(name, global);
		AtanorPredicateKnowledgeBase* kcf;
		if (xn->nodes[0]->token == "dependencyfact")
			kcf = new AtanorDependencyKnowledgeBase(global, name, kbloc);
		else
			kcf = new AtanorPredicateKnowledgeBase(global, name, kbloc);

		if (xn->nodes[0]->nodes.back()->token == "predicateparameters")
			ComputePredicateParameters(xn->nodes[0]->nodes.back(), kcf);
		//We check if there are some PredicateVariable in the parameters
		if (xn->token == "predicatefact" || xn->token == "dependencyfact" || xn->nodes[1]->value == "true") {
			bool keepasknowledge = true;
			if (kpcont->rules.find(name) != kpcont->rules.end() || kcf->isUnified(NULL) == false)
				keepasknowledge = false;
			//if there is a predicate in the formula, then it cannot be an instance, it is a goal

			if (keepasknowledge == false) {
				kbloc->Remove();
				kbloc = new AtanorPredicateRule(name, global, kf);
				((AtanorPredicateRule*)kbloc)->addfinal(kpcont);
				AtanorPredicate* kfx = new AtanorPredicate(name, global);
				for (int j = 0; j < kcf->parameters.size(); j++)
					kfx->parameters.push_back(kcf->parameters[j]);
				kcf->parameters.clear();
				kcf->Release();
				((AtanorPredicateRule*)kbloc)->head = kfx;
				return kbloc;
			}
			//We want to add our value to the knowlegde base		
			kcf->add = true;
			if (xn->nodes[0]->token == "dependencyfact")
				Traverse(xn->nodes[0]->nodes[1], kcf);
		}
		kf->AddInstruction(kbloc);
		return kbloc;
	}

	AtanorPredicateRule* kbloc = new AtanorPredicateRule(name, global, kf);
	AtanorPredicate* kcf = new AtanorPredicate(name, global);

	if (xn->nodes[0]->nodes.back()->token == "predicateparameters")
		ComputePredicateParameters(xn->nodes[0]->nodes.back(), kcf);
	kbloc->head = kcf;

	AtanorPredicateRuleElement* kblocelement = new AtanorPredicateRuleElement(global, NULL);
	//Else we need to browse our structure...
	Traverse(xn->nodes[1], kblocelement);
	kbloc->Addtail(kpcont, kblocelement);
	return kbloc;
}


//This is a DCG rule. The first element is the head of the rule. The other elements might be:
//a dcgfinal : [.]
//a predicate or a word...

Atanor* AtanorCode::C_dcg(x_node* xn, Atanor* kf) {

	//the container is where the rules are stored... 
	AtanorPredicateContainer* kpcont = globalAtanor->Predicatecontainer();
	//We extract our predicate head name
	string sname = xn->nodes[0]->nodes[0]->value;
	short name = globalAtanor->Getid(sname);
	if (!globalAtanor->predicates.check(name))
		globalAtanor->predicates[name] = new AtanorPredicateFunction(global, NULL, name);
	//We compute the inner variables: from S0 to Sn
	short endidx = (short)xn->nodes.size() - 1;
	//The last element, if it is a final block should not be counted in...
	if (xn->nodes.back()->token == "finaldcg")
		endidx--;

	//We create our rule:
	globalAtanor->predicatevariables.clear();
	AtanorPredicateRule* krule = new AtanorPredicateRule(name, global, kf);
	AtanorPredicate* kcf = new AtanorPredicate(name, global);
	krule->head = kcf;
	AtanorPredicateRuleElement* kblocelement = NULL;
	AtanorInstruction kcount;
	x_node* nsub;
	AtanorPredicateRuleElement* kpredelement = NULL;
	char buff[] = { '?', '?', 'A', 0 };

	//Two cases: First it is a rule of the sort: det --> [a]...
	if (xn->nodes[1]->token == "finaltoken") {
		//Terminal rule		
		AtanorConstvector* kvect = new AtanorConstvector(global, kcf);
		//We add our value...
		Traverse(xn->nodes[1], kvect);
		sname = "";
		int i;
		int nb = 0;
		//we try to compute the number of dcgword in the middle... to generate a sequence of variable from ??X0 to ??Xnb
		for (i = 2; i < xn->nodes.size(); i++) {
			if (xn->nodes[2]->token == "dcgword")
				nb++;
			else
				break;
		}

		if (kvect->values.size()) {
			AtanorConstvectormerge* kmerge = new AtanorConstvectormerge(global, kvect);
			//Basically, we have: det --> [a]. which is transformed into det([a|?X],?X,...)
			sname = buff;
			AtanorParsePredicateDCGVariable(sname, kmerge, false); //this part is [a|?X]
			if (nb) {
				buff[2] += nb;
				sname = buff;
			}
			AtanorParsePredicateDCGVariable(sname, kcf, true); //this one is simply ?X...
		}
		else
			kcf->AddInstruction(kvect); //In that case, we do not want a variable... det([],[],...).

		//If we have some parameters, it is finally time to handle them, at the level of the root predicate...
		if (xn->nodes[0]->nodes.size() >= 2)
			ComputePredicateParameters(xn->nodes[0]->nodes.back(), kcf);

		for (int i = 2; i < xn->nodes.size(); i++) {
			//If we have a "finaldcg" object, then we create a specific predicate suite
			//which is added to kblocelement...
			if (xn->nodes[i]->token == "finaldcg") {
				kpredelement = new AtanorPredicateRuleElement(global, kpredelement);
				Traverse(xn->nodes.back(), kpredelement);
				if (kblocelement == NULL)
					krule->Addtail(kpcont, kpredelement);
				else
					krule->Addtail(kpcont, kblocelement);
				return krule;
			}

			//We have a rule: det --> [a], tst...
			nsub = xn->nodes[i];
			//Then we analyze each element, which should have the same form as the head...
			name = globalAtanor->Getid(nsub->nodes[0]->value);
			kpredelement = new AtanorPredicateRuleElement(global, kpredelement);
			if (kblocelement == NULL)
				kblocelement = kpredelement;
			kcf = new AtanorPredicate(name, global, a_predicate, kpredelement);
			if (sname == "")
				kcf->AddInstruction(kvect); //in that case, we automatically transmit [], we do not have a ?X variable to deal with
			else {
				buff[2] = 'A' + i - 2;
				sname = buff;//previous variable
				AtanorParsePredicateDCGVariable(sname, kcf, true); //we add the ?X variable... It is the rest of the list...
			}

			buff[2] = 'A' + i - 1;
			sname = buff;
			AtanorParsePredicateDCGVariable(sname, kcf, true); //then we control variable...

			if (nsub->nodes.size() >= 2) //we add the other variables, which might part of the description of the predicate....
				ComputePredicateParameters(nsub->nodes.back(), kcf);
		}

		if (kblocelement == NULL)
			((AtanorPredicateRule*)krule)->addfinal(kpcont);
		else
			krule->Addtail(kpcont, kblocelement);
		return krule;
	}

	kblocelement = new AtanorPredicateRuleElement(global, NULL);

	//the head...
	//We will implement the head as: predicate(??S0,??Sn...) where n is the number of elements in our expressions
	// p --> sn,vp,pp. here n is 3... p(??S0,??S3...)	
	sname = buff;
	//We add our first variable
	AtanorParsePredicateDCGVariable(sname, kcf, true);
	buff[2] = 'A' + endidx;
	sname = buff;
	//and our following one...
	AtanorParsePredicateDCGVariable(sname, kcf, true);

	//If we have some parameters, it is finally time to handle them, at the level of the root predicate...
	if (xn->nodes[0]->nodes.size() >= 2)
		ComputePredicateParameters(xn->nodes[0]->nodes.back(), kcf);

	for (int i = 1; i <= endidx; i++) {
		nsub = xn->nodes[i];
		//Then we analyze each element, which should have the same form as the head...
		name = globalAtanor->Getid(nsub->nodes[0]->value);
		if (kpredelement == NULL)
			kpredelement = kblocelement;
		else
			kpredelement = new AtanorPredicateRuleElement(global, kpredelement);

		kcf = new AtanorPredicate(name, global, a_predicate, kpredelement);
		buff[2] = 'A' + i - 1;
		sname = buff;
		AtanorParsePredicateDCGVariable(sname, kcf, true);
		buff[2] = 'A' + i;
		sname = buff;
		AtanorParsePredicateDCGVariable(sname, kcf, true);
		if (nsub->nodes.size() >= 2)
			ComputePredicateParameters(nsub->nodes.back(), kcf);
	}

	if (xn->nodes.back()->token == "finaldcg") {
		if (kpredelement == NULL)
			Traverse(xn->nodes.back(), kblocelement);
		else {
			kpredelement = new AtanorPredicateRuleElement(global, kpredelement);
			Traverse(xn->nodes.back(), kpredelement);
		}
	}

	krule->Addtail(kpcont, kblocelement);
	return krule;
}


Atanor* AtanorCode::C_dependency(x_node* xn, Atanor* kf) {
	int i = 0;
	bool modifcall = false;
	bool negation = false;
	if (xn->nodes[0]->token == "modifcall") {
		if (global->modifieddependency != NULL) {
			stringstream message;
			message << "Error: You can only modify one dependency at a time.";
			throw new AtanorRaiseError(message, filename, current_start, current_end);
		}

		modifcall = true;
		i++;
	}
	else
	if (xn->nodes[0]->token == "negation") {
		negation = true;
		i++;
	}

	string name = xn->nodes[i++]->value;
	long lst = name.size() - 1;
	short idname = -1;
	short idvar = 0;
	if (name[lst] >= '0' && name[lst] <= '9' && lst >= 1) {
		//we then check, if we have a '_', which is indicative of a dependency variable...
		lst--;
		while (name[lst] >= '0' && name[lst] <= '9')
			lst--;
		if (name[lst] == '_') {
			if (lst != 0) {
				string nm = name.substr(0, lst);
				idname = globalAtanor->Getid(nm);
				name = name.substr(lst, name.size() - lst);
			}
			else
				idname = a_universal;

			idvar = globalAtanor->Getid(name);

			//We then add a variable, (which will be stored in main memory)
			if (mainframe.isDeclared(idname) == false) {
				global->dependenciesvariable[idvar] = idvar;
				AtanorDependency* a = new AtanorDependency(global, aNULL, idvar, idvar);
				mainframe.Declare(idvar, a);
			}
		}
	}

	if (idname == -1)
		idname = globalAtanor->Getid(name);

	if (!modifcall) {
		if (isaFunction(name, idname))
			return C_regularcall(xn, kf);

		if (globalAtanor->predicates.check(idname)) {
			AtanorPredicate* kx = globalAtanor->predicates[idname];
			if (kx->isPredicateFunction()) {
				kx = (AtanorPredicate*)kx->Newinstance(0, kf);
				kf->AddInstruction(kx);

				for (i = 1; i < xn->nodes.size(); i++)
					Traverse(xn->nodes[i], kx);

				if (!kx->Checkarity()) {
					stringstream message;
					message << "Wrong number of arguments or incompatible argument: '" << name << "'";
					throw new AtanorRaiseError(message, filename, current_start, current_end);
				}
				kx->Setnegation(negation);
				return kx;
			}
		}
	}

	AtanorDependency* a = new AtanorDependency(global, aNULL, idname, idvar);
	a->negation = negation;

	if (modifcall) {
		global->modifieddependency = a;
		a->idvar = a_modifydependency;
		AtanorDependency* adep = new AtanorDependency(globalAtanor, aNULL, a_modifydependency, a_modifydependency);
		mainframe.Declare(a_modifydependency, adep);
	}


	if (xn->nodes[i]->token == "features") {
		a->chosen = 2;
		Traverse(xn->nodes[i], a);
		i++;
	}

	a->chosen = 0;
	Traverse(xn->nodes[i], a);

	a->chosen = modifcall;
	kf->AddInstruction(a);

	return kf;
}

Atanor* AtanorCode::C_dependencyresult(x_node* xn, Atanor* kpredelement) {
	AtanorDependencyKnowledgeBaseFunction* kbloc;
	long idrule = globalAtanor->Predicatecontainer()->rules[a_dependency].size();

	for (int i = 0; i < xn->nodes.size(); i++) {
		kpredelement = new AtanorPredicateRuleElement(global, kpredelement);
		if (xn->nodes[i]->token == "negation") {
			if (globalAtanor->modifieddependency != NULL) {
				kbloc = new AtanorDependencyKnowledgeBaseFunction(global, a_remove, idrule, kpredelement);
				kbloc->AddInstruction(globalAtanor->modifieddependency);
				//We change the idvar to 0, to distinguish between a dependency deletion from a simple modification
				kbloc->idvar = 0;
			}
		}
		else {
			if (xn->nodes[i]->token == "dependency") {
				short nm = 0;
				if (globalAtanor->modifieddependency != NULL) {
					kbloc = new AtanorDependencyKnowledgeBaseFunction(global, a_remove, idrule, kpredelement);
					kbloc->AddInstruction(globalAtanor->modifieddependency);
					globalAtanor->modifieddependency = NULL;

					kpredelement = new AtanorPredicateRuleElement(global, kpredelement);
					nm = a_modifydependency;
				}

				kbloc = new AtanorDependencyKnowledgeBaseFunction(global, a_assertz, idrule, kpredelement);

				// We prevent feature assignement within a dependency as result...
				featureassignment = 1;
				Traverse(xn->nodes[i], kbloc);
				featureassignment = 0;
				kbloc->idvar = nm;
			}
			else
				Traverse(xn->nodes[i], kpredelement);
		}
	}
	
	return kpredelement;
}


static void StorePredicateVariables(x_node* xn, Atanorsynode** a) {
	for (int i = 0; i < xn->nodes.size(); i++) {
		if (xn->nodes[i]->token == "predicatevariable") {
			if (xn->nodes[i]->nodes.size() == 0 || xn->nodes[i]->nodes[0]->token != "anumber") //#_ variable
				continue;
			string sid = xn->nodes[i]->nodes[0]->value;
			if (globalAtanor->dependencyvariables.find(sid) == globalAtanor->dependencyvariables.end()) {
				Atanorsynode* as = new Atanorsynode(atoi(STR(sid)), globalAtanor);
				globalAtanor->dependencyvariables[sid] = as;
				as->reference = 1;
				if (*a == NULL)
					*a = as;
			}
		}
	}
}

//This is a dependency syntactic rule of the form:
// IF (dep(#1,#2,#3)...) dep(#1,#2,#3).
Atanor* AtanorCode::C_dependencyrule(x_node* xn, Atanor* kf) {
	//We extract our predicate head name
	if (!globalAtanor->predicates.check(a_dependency))
		globalAtanor->predicates[a_dependency] = new AtanorPredicateFunction(global, NULL, a_dependency);
	//We compute the inner variables: from S0 to Sn
	short endidx = (short)xn->nodes.size() - 1;
	//The last element, if it is a final block should not be counted in...
	if (xn->nodes.back()->token == "dependencyresult")
		endidx--;

	//We create our rule:
	globalAtanor->modifieddependency = NULL;
	globalAtanor->dependencyvariables.clear();
	AtanorPredicateRule* krule = new AtanorPredicateRule(a_dependency, global, kf);
	AtanorPredicate* kcf = new AtanorPredicate(a_dependency, global);
	krule->head = kcf;
	x_node* nsub;

	//the head...
	//We will implement the head as: &head&(#1,#n), where #1..#n are extracted from the dependencies...
	int i;
	string sid;
	Atanorsynode* as = NULL;
	for (i = 0; i < xn->nodes.size() - 1; i++) {
		nsub = xn->nodes[i]; //we do not take into account the negated dependencies... Hence the test nsub->nodes[0]->token
		if (nsub->token == "dependance" && nsub->nodes[0]->token == "dependency") {
			nsub = nsub->nodes[0];
			if (nsub->nodes[0]->token == "negation")
				continue;
			StorePredicateVariables(nsub->nodes.back(), &as);
		}
	}

	switch (globalAtanor->dependencyvariables.size()) {
	case 0: //no variable detected, we add two empty variables
		kcf->AddInstruction(aUNIVERSAL);
		kcf->AddInstruction(aUNIVERSAL);
		break;
	case 1:
		//we add it twice... if there is only one that was detected
		kcf->AddInstruction(as);
		kcf->AddInstruction(as);
		break;
	default: //else the two first...
	{
				 map<string, Atanor*>::iterator it = globalAtanor->dependencyvariables.begin();
				 kcf->AddInstruction(it->second);
				 it++;
				 kcf->AddInstruction(it->second);
	}
	}

	AtanorPredicateRuleElement* kblocelement = new AtanorPredicateRuleElement(global, NULL);
	AtanorPredicateRuleElement* kpredelement = NULL;

	featureassignment = 0;

	for (i = 0; i < xn->nodes.size() - 1; i += 2) {
		nsub = xn->nodes[i];
		if (kpredelement == NULL)
			kpredelement = kblocelement;
		else
			kpredelement = new AtanorPredicateRuleElement(global, kpredelement);
		Traverse(nsub, kpredelement);
		kpredelement->Setnegation(kpredelement->instructions[0]->isNegation());
		if (xn->nodes[i + 1]->value == "||" || xn->nodes[i + 1]->value == "or") {
			kpredelement->instructions.back()->Setdisjunction(true);
			kpredelement->Setdisjunction(true);
		}
	}

	Traverse(xn->nodes.back(), kpredelement);

	//the container retrieved by Predicatecontainer is where the rules are stored... 
	krule->Addtail(globalAtanor->Predicatecontainer(), kblocelement);
	return krule;
}

Atanor* AtanorCode::C_predicate(x_node* xn, Atanor* kf) {
	short name = globalAtanor->Getid(xn->nodes[0]->value);
	AtanorPredicate* kcf = new AtanorPredicate(name, global);

	if (xn->nodes.back()->token == "predicateparameters")
		ComputePredicateParameters(xn->nodes.back(), kcf);
	kf->AddInstruction(kcf);
	return kcf;
}


Atanor* AtanorCode::C_predicateexpression(x_node* xn, Atanor* kf) {
	//This is where we analyse our structure...
	AtanorPredicateRuleElement* kbloc = new AtanorPredicateRuleElement(global, kf);
	if (xn->nodes[0]->token == "negation") {
		kbloc->negation = true;
		if (xn->nodes[1]->token == "expressions") {
			AtanorInstruction klocbloc;
			Traverse(xn->nodes[1], &klocbloc);
			AtanorInstruction* kint = CloningInstruction((AtanorInstruction*)klocbloc.instructions[0]);
			kbloc->AddInstruction(kint);
		}
		else
			Traverse(xn->nodes[1], kbloc);
		if (xn->nodes.size() == 4) {
			if (xn->nodes[2]->value == ";")
				kbloc->disjunction = true;
			Traverse(xn->nodes[3], kbloc);
		}
	}
	else {
		if (xn->nodes[0]->token == "expressions") {
			AtanorInstruction klocbloc;
			Traverse(xn->nodes[0], &klocbloc);
			AtanorInstruction* kint = CloningInstruction((AtanorInstruction*)klocbloc.instructions[0]);
			kbloc->AddInstruction(kint);
		}
		else
			Traverse(xn->nodes[0], kbloc);
		if (xn->nodes.size() == 3) {
			if (xn->nodes[1]->value == ";")
				kbloc->disjunction = true;
			Traverse(xn->nodes[2], kbloc);
		}
	}
	return kbloc;
}

bool Mergingfeatures(Atanormapss* nodes, Atanormapss* novel) {
	hmap<string, string>::iterator it;
	for (it = novel->values.begin(); it != novel->values.end(); it++) {
		if (nodes->values.find(it->first) == nodes->values.end()) 
			nodes->values[it->first] = it->second;
		else {
			if (nodes->values[it->first] != it->second)
				return false;
		}
	}

	return true;
}

Atanor* AtanorCode::C_predicatevariable(x_node* xn, Atanor* kf) {

	string& name = xn->value;

	if (name[0] == '#') {
		//if it is a "#_" variable, then it is a UNIVERSAL variable...
		if (xn->nodes.size() == 0) {
			kf->AddInstruction(aUNIVERSAL);
			return aUNIVERSAL;
		}

		Atanorsynode* as;
		int sz = 2;
		if (xn->nodes[0]->token != "anumber") { //again a '#_' variable but with constraints...
			as = new Atanorsynode(a_universal, global);
			sz = 1;
		}
		else {
			string sid = xn->nodes[0]->value;
			as = (Atanorsynode*)globalAtanor->dependencyvariables[sid];
			if (as == NULL) {
				stringstream message;
				message << "Non instanciated variable in a dependency rule: '" << name << "' ";
				throw new AtanorRaiseError(message, filename, current_start, current_end);
			}						
		}

		if (xn->nodes.size() == sz) {
			if (kf->Action() == a_affectation || kf->Type() == a_predicateruleelement) {
				AtanorCallsynode* acs = new AtanorCallsynode(as, global, kf);
				Traverse(xn->nodes[sz - 1], acs);
				return acs;
			}
			

			if (featureassignment == 1) {
				stringstream message;
				message << "Cannot assign or test a feature to this dependency node";
				throw new AtanorRaiseError(message, filename, current_start, current_end);
			}

			featureassignment = 2;

			if (as->features == aNULL)
				Traverse(xn->nodes[sz - 1], as);
			else {
				AtanorInstruction kbloc;
				Traverse(xn->nodes[sz - 1], &kbloc);

				if (kbloc.instructions[0]->Type() != Atanormapss::idtype) {
					stringstream message;
					message << "Wrong feature structure";
					throw new AtanorRaiseError(message, filename, current_start, current_end);
				}

				if (!Mergingfeatures((Atanormapss*)as->features, (Atanormapss*)kbloc.instructions[0])) {
					stringstream message;
					message << "Incoherent feature testing";
					throw new AtanorRaiseError(message, filename, current_start, current_end);
				}
			}			

			featureassignment = 0;
		}

		kf->AddInstruction(as);
		return as;
	}

	AtanorPredicateVariable* var;
	short idname;

	if (xn->nodes[0]->token == "goal")
		return Traverse(xn->nodes[0], kf);

	if (globalAtanor->predicatevariables.find(name) == globalAtanor->predicatevariables.end()) {
		idname = globalAtanor->Getid(name);
		var = new AtanorPredicateVariable(global, idname, kf);
		globalAtanor->predicatevariables[name] = var;
	}
	else {
		var = globalAtanor->predicatevariables[name];
		kf->AddInstruction(var);
	}
	return var;
}


Atanor* AtanorCode::C_assertpredicate(x_node* xn, Atanor* kf) {
	short id = a_assertz;
	if (xn->nodes[0]->token == "assertcommandbefore")
		id = a_asserta;
	else {
		if (xn->nodes[0]->token == "retractcommand")
			id = a_retract;
	}

	Atanor* kbloc = new AtanorPredicateKnowledgeBaseFunction(global, id, kf);
	if (xn->nodes.size() != 2)  {
		stringstream message;
		message << "Error: Wrong assert or retract definition";
		throw new AtanorRaiseError(message, filename, current_start, current_end);
	}
	Traverse(xn->nodes.back(), kbloc);
	return kbloc;
}


Atanor* AtanorCode::C_term(x_node* xn, Atanor* kf) {
	string name("_");
	short idname = a_universal;
	if (xn->token == "term") {
		name = xn->value;
		name = name.substr(0, name.size() - 2);
		idname = globalAtanor->Getid(name);
	}

	AtanorPredicateTerm* var = new AtanorPredicateTerm(global, idname, kf);
	if (xn->nodes.back()->token == "predicateparameters")
		ComputePredicateParameters(xn->nodes.back(), var);
	return var;
}


Atanor* AtanorCode::C_comparepredicate(x_node* xn, Atanor* kf) {
	//The first parameter is the operator
	short op = globalAtanor->string_operators[xn->nodes[1]->value];
	Atanor* ki = AtanorCreateInstruction(kf, op);
	for (size_t i = 0; i < xn->nodes.size(); i++)
		Traverse(xn->nodes[i], ki);
	return ki;
}


//------------------------------------------------------------------------

bool AtanorCode::Load(x_reading& xr) {
	short currentspaceid = globalAtanor->spaceid;
	bnf_atanor* previous = globalAtanor->currentbnf;
	Atanoratanor* atan = new Atanoratanor(filename, this, globalAtanor);

	globalAtanor->spaceid = idcode;

	bnf_atanor bnf;
	bnf.baseline = globalAtanor->linereference;
	global->lineerror = -1;

	x_node* xn = bnf.x_parsing(&xr, FULL);
	if (xn == NULL) {
		cerr << " in " << filename << endl;
		stringstream message;
		global->lineerror = bnf.lineerror;
		currentline = global->lineerror;
		message << "Error while parsing program file: ";
		if (xr.error()) {
			message << "Unknown file: ";
			message << xr.name;
		}
		else
		if (bnf.errornumber != -1)
			message << bnf.x_errormsg(bnf.errornumber);
		else
			message << bnf.labelerror;
		throw new AtanorRaiseError(message, filename, global->lineerror, bnf.lineerror);
	}


	globalAtanor->currentbnf = &bnf;

	globalAtanor->Pushstack(&mainframe);
	Traverse(xn, &mainframe);
	globalAtanor->Popstack();

	globalAtanor->currentbnf = previous;
	globalAtanor->spaceid = currentspaceid;

	delete xn;
	return true;
}

//------------------------------------------------------------------------

bool AtanorCode::Compile(string& body) {
	//we store our AtanorCode also as an Atanoratanor...
	filename = NormalizeFileName(filename);
	Atanoratanor* atan = new Atanoratanor(filename, this, globalAtanor);

	globalAtanor->spaceid = idcode;

	bnf_atanor bnf;
	bnf.baseline = globalAtanor->linereference;

	x_readstring xr(body);
	xr.loadtoken();
	global->lineerror = -1;

	x_node* xn = bnf.x_parsing(&xr, FULL);
	if (xn == NULL) {
		cerr << " in " << filename << endl;
		stringstream& message = globalAtanor->threads[0].message;
		global->lineerror = bnf.lineerror;
		currentline = global->lineerror;
		message << "Error while parsing program file: ";
		if (xr.error()) {
			message << "Unknown file: ";
			message << xr.name;
		}
		else
		if (bnf.errornumber != -1)
			message << bnf.x_errormsg(bnf.errornumber);
		else
			message << bnf.labelerror;

		globalAtanor->Returnerror(message.str(), globalAtanor->GetThreadid());
		return false;
	}

	globalAtanor->currentbnf = &bnf;
	firstinstruction = mainframe.instructions.size();

	globalAtanor->Pushstack(&mainframe);
	try {
		Traverse(xn, &mainframe);
	}
	catch (AtanorRaiseError* a) {
		globalAtanor->threads[0].currentinstruction = NULL;
		globalAtanor->lineerror = a->left;
		globalAtanor->threads[0].message.str("");
		globalAtanor->threads[0].message << a->message;
		if (a->message.find(a->filename) == string::npos)
			globalAtanor->threads[0].message << " in " << a->filename;

		if (globalAtanor->threads[0].errorraised == NULL)
			globalAtanor->threads[0].errorraised = new AtanorError(globalAtanor->threads[0].message.str());
		else
			globalAtanor->threads[0].errorraised->error = globalAtanor->threads[0].message.str();

		globalAtanor->threads[0].error = true;
		AtanorCode* c = globalAtanor->Getcurrentcode();
		if (c->filename != a->filename)
			c->filename = a->filename;
		delete a;
		delete xn;
		globalAtanor->Popstack();
		return false;
	}

	globalAtanor->Popstack();

	delete xn;
	return true;
}


Atanor* AtanorCode::Compilefunction(string& body) {
	//we store our AtanorCode also as an Atanoratanor...

	bnf_atanor bnf;
	bnf.baseline = globalAtanor->linereference;

	x_readstring xr(body);
	xr.loadtoken();
	global->lineerror = -1;

	x_node* xn = bnf.x_parsing(&xr, FULL);
	if (xn == NULL) {
		cerr << " in " << filename << endl;
		stringstream& message = globalAtanor->threads[0].message;
		global->lineerror = bnf.lineerror;
		currentline = global->lineerror;
		message << "Error while parsing program file: ";
		if (xr.error()) {
			message << "Unknown file: ";
			message << xr.name;
		}
		else
		if (bnf.errornumber != -1)
			message << bnf.x_errormsg(bnf.errornumber);
		else
			message << bnf.labelerror;

		return globalAtanor->Returnerror(message.str(), globalAtanor->GetThreadid());
	}

	globalAtanor->currentbnf = &bnf;
	firstinstruction = mainframe.instructions.size();

	globalAtanor->Pushstack(&mainframe);
	Atanor* compiled = NULL;
	try {
		compiled = Traverse(xn, &mainframe);
	}
	catch (AtanorRaiseError* a) {
		globalAtanor->threads[0].message.str("");
		globalAtanor->threads[0].message << a->message;
		if (a->message.find(a->filename) == string::npos)
			globalAtanor->threads[0].message << " in " << a->filename;

		if (globalAtanor->threads[0].errorraised == NULL)
			globalAtanor->threads[0].errorraised = new AtanorError(globalAtanor->threads[0].message.str());
		else
			globalAtanor->threads[0].errorraised->error = globalAtanor->threads[0].message.str();

		globalAtanor->threads[0].error = true;
		AtanorCode* c = globalAtanor->Getcurrentcode();
		if (c->filename != a->filename)
			c->filename = a->filename;
		delete a;
		delete xn;
		globalAtanor->Popstack();
		return NULL;
	}

	globalAtanor->Popstack();

	delete xn;
	return compiled;
}



