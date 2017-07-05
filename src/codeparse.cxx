#include "codeparse.h"


static const char* errorlabels[] = { "", "Wrong index format",
"Wrong expression after an operator",
"Wrong list of parameters",
"Wrong expressions in parentheses",
"Wrong FRAME declaration",
"Wrong argument declaration",
"Wrong FUNCTION declaration",
"Wrong BOOLEAN EXPRESSION",
"Wrong BOOLEAN expression",
"Wrong IF declaration",
"Wrong WHILE declaration",
"Wrong FOR declaration",
"Wrong FUNCTION CALL",
"Wrong Expression",
"Wrong Instruction",
"Wrong SWITCH",
"Wrong test in a SWITCH",
"Wrong IN or ON expression",
"Wrong WITH expression",
"Wrong PREDICATE declaration",
"Wrong PATTERN in A RULE",
"Wrong TRY-CATCH expression",
"Wrong INTERVAL",
"Wrong ELSE section",
"Wrong VECTOR definition",
"Wrong MAP definition",
"Wrong Function call in an index reference",
"Wrong comments",
"Wrong dotted expression",
"Wrong expression or Missing ';'",
"Missing '}'",
"Wrong declaration",
"Wrong FOR IN declaration",
"Wrong PREDICATE definition",
"Wrong PREDICATE expression",
"Wrong PREDICATE parameter list",
"Wrong PREDICATE term",
"Wrong tuple",
"Wrong ASSERT definition",
"Wrong PREDICATE VARIABLE",
"Wrong RANGE definition",
"Wrong LISP expression",
"Wrong TAG expression",
"Wrong DCG declaration",
"Wrong DCG or Missing '.'",
"Wrong Predicate Rule or Missing '.'",
"Wrong LAMBDA use",
"Wrong FUNCTIONAL description",
"Wrong FILTER",
"Wrong FOLD or SCAN",
"Wrong MAP",
"Wrong CYCLE or REPEAT",
"Wrong REPLICATE",
"Wrong Haskell LAMBDA",
"Wrong Haskell CASE",
"Wrong Haskell Boolean Use",
"Wrong FLIP",
"Wrong EXTENSION implementation",
"Expecting a meta-character",
"Expecting a '.'",
"Wrong tuple definition",
"Wrong 'IF' expression" };

static char tab0[] = { '_', '#', 0 };
static char tab1[] = { '&', '|', '+', '-', '*', '/', '%', '^', 0 };
static char tab2[] = { '<', '>', 0 };
static char tab3[] = { ':', '=', 0 };
static char tab4[] = { ',', ';', 0 };
static char tab5[] = { '|', ':', 0 };
static char tab6[] = { ':', '=', 0 };
static char tab7[] = { ',', ';', 0 };
static char tab8[] = { '&', '+', '-', '*', '/', '%', '^', 0 };
static char tab9[] = { ',', ';', 0 };
static char tab10[] = { 'd', 'p', 'c', 'C', 'a', 'r', 's', 'S', '?', '%', 0 };
static char tab11[] = { '*', '+', 0 };
static const char* varstring5 = "and";
static const char* varstring16 = "exclusive";
static const char* varstring27 = "elif";
static const char* varstring47 = "otherwise";
static const char* varstring72 = "false";
static const char* varstring56 = "takeWhile";
static const char* varstring61 = "map";
static const char* varstring45 = "assert";
static const char* varstring2 = "is";
static const char* varstring6 = "private";
static const char* varstring67 = "flip";
static const char* varstring40 = "fail";
static const char* varstring64 = "repeat";
static const char* varstring48 = "foldl";
static const char* varstring31 = "in";
static const char* varstring17 = "polynomial";
static const char* varstring39 = "null";
static const char* varstring0 = "×";
static const char* varstring73 = "do";
static const char* varstring21 = "if";
static const char* varstring50 = "scanl";
static const char* varstring74 = "try";
static const char* varstring58 = "dropWhile";
static const char* varstring8 = "const";
static const char* varstring25 = "for";
static const char* varstring71 = "true";
static const char* varstring26 = "switch";
static const char* varstring13 = "strict";
static const char* varstring30 = "continue";
static const char* varstring66 = "replicate";
static const char* varstring59 = "take";
static const char* varstring62 = "zipWith";
static const char* varstring19 = "autorun";
static const char* varstring42 = "retract";
static const char* varstring49 = "foldr";
static const char* varstring65 = "cycle";
static const char* varstring1 = "÷";
static const char* varstring10 = "frame";
static const char* varstring51 = "scanr";
static const char* varstring18 = "function";
static const char* varstring12 = "...";
static const char* varstring35 = "notin";
static const char* varstring28 = "return";
static const char* varstring55 = "scanr1";
static const char* varstring53 = "foldr1";
static const char* varstring41 = "stop";
static const char* varstring38 = "..";
static const char* varstring52 = "foldl1";
static const char* varstring32 = "else";
static const char* varstring29 = "break";
static const char* varstring46 = "let";
static const char* varstring3 = "not";
static const char* varstring37 = "from";
static const char* varstring75 = "catch";
static const char* varstring9 = "with";
static const char* varstring22 = "IF";
static const char* varstring68 = "case";
static const char* varstring36 = "absent";
static const char* varstring43 = "asserta";
static const char* varstring11 = "extension";
static const char* varstring20 = "thread";
static const char* varstring76 = "default";
static const char* varstring69 = "of";
static const char* varstring60 = "drop";
static const char* varstring14 = "joined";
static const char* varstring57 = "filter";
static const char* varstring24 = "while";
static const char* varstring15 = "protected";
static const char* varstring7 = "common";
static const char* varstring33 = "²";
static const char* varstring34 = "³";
static const char* varstring54 = "scanl1";
static const char* varstring44 = "assertz";
static const char* varstring70 = "where";
static const char* varstring4 = "or";
static const char* varstring63 = "zip";
static const char* varstring23 = "If";


char bnf_atanor::m_char(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "char";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (x_test_ord(lret, 'A', 'Z') || x_test_ord(lret, 'a', 'z') || x_test_in(lret, tab0))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_path_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_path(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_path(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "path";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '/'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_path_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_shell_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_path(lret, &subtree) || m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_shell(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '#'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '!'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_shell_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_pop_node(tree, addsubtree);
	return(1);
}


char bnf_atanor::m_signature(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, 239))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, 187))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, 191))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_pop_node(tree, addsubtree);
	return(1);
}


char bnf_atanor::m_label(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "label";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '#'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_anydouble_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '\\'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_any(lret))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_anydouble_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYNEGATION
	subtree = new x_node;
	if (x_test_char(lret, 34) == 0) {
		x_test_any(lret);
		x_init_tree(tree, subtree, addsubtree);
	}
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_anydouble(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "anydouble";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_anydouble_0_1(lret, &subtree) || m_anydouble_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_anysimple(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "anysimple";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYNEGATION
	subtree = new x_node;
	if (x_test_char(lret, 39) == 0) {
		x_test_any(lret);
		x_init_tree(tree, subtree, addsubtree);
	}
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_anycolon_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '%'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_any(lret))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_anycolon_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYNEGATION
	subtree = new x_node;
	if (x_test_char(lret, ';') == 0) {
		x_test_any(lret);
		x_init_tree(tree, subtree, addsubtree);
	}
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_anycolon(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "anycolon";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_anycolon_0_1(lret, &subtree) || m_anycolon_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_operator_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '&') || !x_test_char(lret, '&') || !x_test_char(lret, '&')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_operator_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, ':') || !x_test_char(lret, ':')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_operator_0_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '<') || !x_test_char(lret, '<')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_operator_0_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '>') || !x_test_char(lret, '>')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_operator_0_5(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '^') || !x_test_char(lret, '^')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_operator_0_6(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '*') || !x_test_char(lret, '*')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_operator(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "operator";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;

	if (currentpos >= fx->stack.size())
		return 0;
	unsigned char x = fx->stack[currentpos][intoken];
	bool err = true;
	switch (x) {
	case '&':
		if (m_operator_0_1(lret, &subtree) || x_test_in(lret, tab1))
			err = false;
		break;
	case ':':
		if (m_operator_0_2(lret, &subtree))
			err = false;
		break;
	case '<':
		if (m_operator_0_3(lret, &subtree))
			err = false;
		break;
	case '>':
		if (m_operator_0_4(lret, &subtree))
			err = false;
		break;
	case '^':
		if (m_operator_0_5(lret, &subtree) || x_test_in(lret, tab1))
			err = false;
		break;
	case '*':
		if (m_operator_0_6(lret, &subtree) || x_test_in(lret, tab1))
			err = false;
		break;
	case '|':
		if (x_test_in(lret, tab1))
			err = false;
		break;
	case '+':
		if (x_test_in(lret, tab1))
			err = false;
		break;
	case '-':
		if (x_test_in(lret, tab1))
			err = false;
		break;
	case '/':
		if (x_test_in(lret, tab1))
			err = false;
		break;
	case '%':
		if (x_test_in(lret, tab1))
			err = false;
		break;
	default:
		if (x_test_string(lret, varstring0) || x_test_string(lret, varstring1))
			err = false;
	}
	if (err == false)
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}

char bnf_atanor::m_operatoraffectation_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring0))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '='))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_operatoraffectation_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring1))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '='))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_operatoraffectation_0_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '&') || !x_test_char(lret, '&') || !x_test_char(lret, '&') || !x_test_char(lret, '=')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_operatoraffectation_0_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, ':') || !x_test_char(lret, ':') || !x_test_char(lret, '=')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_operatoraffectation_0_5(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, ':') || !x_test_char(lret, '=')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_operatoraffectation_0_6_7(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (x_test_string(lret, varstring3))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_operatoraffectation_0_6(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring2))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_operatoraffectation_0_6_7(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_operatoraffectation_0_8(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '<') || !x_test_char(lret, '<') || !x_test_char(lret, '=')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_operatoraffectation_0_9(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '>') || !x_test_char(lret, '>') || !x_test_char(lret, '=')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_operatoraffectation_0_10(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '+') || !x_test_char(lret, '=')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_operatoraffectation_0_11(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '-') || !x_test_char(lret, '=')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_operatoraffectation_0_12(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '*') || !x_test_char(lret, '*') || !x_test_char(lret, '=')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_operatoraffectation_0_13(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '/') || !x_test_char(lret, '=')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_operatoraffectation_0_14(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '%') || !x_test_char(lret, '=')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_operatoraffectation_0_15(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '&') || !x_test_char(lret, '=')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_operatoraffectation_0_16(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '|') || !x_test_char(lret, '=')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_operatoraffectation_0_17(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '^') || !x_test_char(lret, '^') || !x_test_char(lret, '=')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_operatoraffectation_0_18(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '*') || !x_test_char(lret, '=')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_operatoraffectation_0_19(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '^') || !x_test_char(lret, '=')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_operatoraffectation_0_20(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '<') || !x_test_char(lret, '<') || !x_test_char(lret, '<')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_operatoraffectation(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "operatoraffectation";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;

	if (currentpos >= fx->stack.size())
		return 0;
	unsigned char x = fx->stack[currentpos][intoken];
	bool err = true;
	switch (x) {
	case '&':
		if (m_operatoraffectation_0_3(lret, &subtree) || m_operatoraffectation_0_15(lret, &subtree))
			err = false;
		break;
	case ':':
		if (m_operatoraffectation_0_4(lret, &subtree) || m_operatoraffectation_0_5(lret, &subtree))
			err = false;
		break;
	case '<':
		if (m_operatoraffectation_0_8(lret, &subtree) || m_operatoraffectation_0_20(lret, &subtree))
			err = false;
		break;
	case '>':
		if (m_operatoraffectation_0_9(lret, &subtree))
			err = false;
		break;
	case '+':
		if (m_operatoraffectation_0_10(lret, &subtree))
			err = false;
		break;
	case '-':
		if (m_operatoraffectation_0_11(lret, &subtree))
			err = false;
		break;
	case '*':
		if (m_operatoraffectation_0_12(lret, &subtree) || m_operatoraffectation_0_18(lret, &subtree))
			err = false;
		break;
	case '/':
		if (m_operatoraffectation_0_13(lret, &subtree))
			err = false;
		break;
	case '%':
		if (m_operatoraffectation_0_14(lret, &subtree))
			err = false;
		break;
	case '|':
		if (m_operatoraffectation_0_16(lret, &subtree))
			err = false;
		break;
	case '^':
		if (m_operatoraffectation_0_17(lret, &subtree) || m_operatoraffectation_0_19(lret, &subtree))
			err = false;
		break;
	case '=':
		if (x_test_char(lret, '='))
			err = false;
		break;
	default:
		if (m_operatoraffectation_0_1(lret, &subtree) || m_operatoraffectation_0_2(lret, &subtree) || m_operatoraffectation_0_6(lret, &subtree))
			err = false;
	}
	if (err == false)
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}

char bnf_atanor::m_orand_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '|') || !x_test_char(lret, '|')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_orand_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '&') || !x_test_char(lret, '&')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_orand(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "orand";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_orand_0_1(lret, &subtree) || m_orand_0_2(lret, &subtree) || x_test_string(lret, varstring4) || x_test_string(lret, varstring5))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_comparator_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '~') || !x_test_char(lret, '=')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_comparator_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '!') || !x_test_char(lret, '=')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_comparator_0_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '=') || !x_test_char(lret, '=')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_comparator_0_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '<') || !x_test_char(lret, '=')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_comparator_0_5(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '>') || !x_test_char(lret, '=')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_comparator_0_6(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '$') || !x_test_char(lret, '>')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_comparator_0_7(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '$') || !x_test_char(lret, '<')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_comparator(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "comparator";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;

	if (currentpos >= fx->stack.size())
		return 0;
	unsigned char x = fx->stack[currentpos][intoken];
	bool err = true;
	switch (x) {
	case '~':
		if (m_comparator_0_1(lret, &subtree))
			err = false;
		break;
	case 33:
		if (m_comparator_0_2(lret, &subtree))
			err = false;
		break;
	case '=':
		if (m_comparator_0_3(lret, &subtree))
			err = false;
		break;
	case '<':
		if (m_comparator_0_4(lret, &subtree) || x_test_in(lret, tab2))
			err = false;
		break;
	case '>':
		if (m_comparator_0_5(lret, &subtree) || x_test_in(lret, tab2))
			err = false;
		break;
	case '$':
		if (m_comparator_0_6(lret, &subtree) || m_comparator_0_7(lret, &subtree))
			err = false;
		break;
	}
	if (err == false)
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}

char bnf_atanor::m_operation(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "operation";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_operator(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_multiply_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_power(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_multiply_0_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_operation(lret, &subtree) || m_operationin(lret, &subtree) || m_multiply(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_multiply_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_multiply_0_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_multiply(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "multiply";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_multiply_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_multiply_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_comparison(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "comparison";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_comparator(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_nocomparisonexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_private(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "private";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_string(lret, varstring6))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_common(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "common";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_string(lret, varstring7))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_const(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "const";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_string(lret, varstring8))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_feature_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_common(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_feature_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_private(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_feature_0_1_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_feature_0_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_common(lret, &subtree) || m_const(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_feature(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "feature";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_feature_0_1(lret, &subtree) || m_feature_0_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_type_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_feature(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_type(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "type";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_type_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_typename(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_depend(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "depend";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring9))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 19);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_declaration_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_parameters(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_declaration_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_declaration_0_1_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_declaration_0_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_depend(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_declaration_0_4_5(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '='))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_wnexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_declaration_0_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_declaration_0_4_5(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_declaration(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "declaration";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_type(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_declaration_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_declaration_0_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_declaration_0_4(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_declarationlist_0_4_5(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "declarationlist";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '='))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 32);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_wnexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_declarationlist_0_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_declarationlist_0_4_5(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_declarationlist_0_6_7(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "declarationlist";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 32);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_declarationlist(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_declarationlist_0_6(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_declarationlist_0_6_7(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_declarationlist(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "declarationlist";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_declaration_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_declaration_0_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_declarationlist_0_4(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_declarationlist_0_6(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_multideclaration(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "multideclaration";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_type(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_declaration_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_declaration_0_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_declarationlist_0_4(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_declarationlist_0_6(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_declarationending(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "declarationending";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_char(lret, ';'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_frame_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_private(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_frame_0_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_declarationending(lret, &subtree) || m_instruction(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_frame(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "frame";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_frame_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring10))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 5);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_typename(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_declaration_0_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_frame_0_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_extension(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "extension";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring11))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 58);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_typename(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_instruction(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_instruction_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_blocs(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_instruction(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "instruction";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '{'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 15);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_instruction_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '}'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_nonlimited(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "nonlimited";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_string(lret, varstring12))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_arguments_0_1_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_nonlimited(lret, &subtree) || m_arguments(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_arguments_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "arguments";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 6);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_arguments_0_1_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_arguments_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_arguments_0_1_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_arguments(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "arguments";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_declaration(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_arguments_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_strict(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "strict";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_string(lret, varstring13))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_join(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "join";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_string(lret, varstring14))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_protecclusive(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "protecclusive";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (x_test_string(lret, varstring15) || x_test_string(lret, varstring16))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_functionsort(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "functionsort";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (x_test_string(lret, varstring17) || x_test_string(lret, varstring18) || x_test_string(lret, varstring19) || x_test_string(lret, varstring20))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_typefunction_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_join(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_typefunction_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_protecclusive(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_typefunction_0_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_private(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_typefunction_0_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_strict(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_typefunction(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "typefunction";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_typefunction_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_typefunction_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_typefunction_0_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_typefunction_0_4(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_functionsort(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_indexname(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "indexname";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '[') || !x_test_char(lret, ']')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_intervalname(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "intervalname";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '[') || !x_test_char(lret, ':') || !x_test_char(lret, ']')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_returntype(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "returntype";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '='))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_function_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_word(lret, &subtree) || m_plusplus(lret, &subtree) || m_operator(lret, &subtree) || m_comparator(lret, &subtree) || m_indexname(lret, &subtree) || m_intervalname(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_function_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_arguments(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_function_0_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_returntype(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_function(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "function";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_typefunction(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 7);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_function_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_function_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_function_0_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_frame_0_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_subfunc(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "subfunc";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_dot(lret))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 27);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_regularcall(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_subfuncbis(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "subfuncbis";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_dot(lret))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_regularcall(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_subvar(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "subvar";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_dot(lret))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 29);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_variable(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_notafunction(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "notafunction";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (x_test_string(lret, varstring21) || x_test_string(lret, varstring22) || x_test_string(lret, varstring23) || x_test_string(lret, varstring24) || x_test_string(lret, varstring25) || x_test_string(lret, varstring26) || x_test_string(lret, varstring27))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_framesep(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "framesep";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (m_operator_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_frameup(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "frameup";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_framesep(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_functioncall_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_frameup(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_functioncall(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "functioncall";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_functioncall_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYFAIL
	subtree = NULL;
	if (m_notafunction(lret, &subtree)) {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_regularcall_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_parameters(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_regularcall_0_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_interval(lret, &subtree) || m_indexes(lret, &subtree) || m_subfunc(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_regularcall_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_regularcall_0_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_regularcall(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "regularcall";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_functioncall(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 13);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_regularcall_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_regularcall_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_purecall_0_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_interval(lret, &subtree) || m_indexes(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_purecall_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_purecall_0_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_purecall(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "purecall";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_functioncall(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_regularcall_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_purecall_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_returncall(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "returncall";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_string(lret, varstring28))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_breakcall(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "breakcall";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_string(lret, varstring29))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_continuecall(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "continuecall";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_string(lret, varstring30))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_call(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "call";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_breakcall(lret, &subtree) || m_continuecall(lret, &subtree) || m_regularcall(lret, &subtree) || m_returncall(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_sep(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "sep";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_char(lret, ':'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_parameters_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "parameters";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 3);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_nocomparisonexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_parameters_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYWHILE
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_parameters_0_1_2(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	return(1);
}


char bnf_atanor::m_parameters(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "parameters";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_nocomparisonexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_parameters_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_sousbloc(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "sousbloc";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_multideclaration(lret, &subtree) || m_increment(lret, &subtree) || m_affectation(lret, &subtree) || m_expressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_bloc_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "bloc";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_dcg(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 45);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_dot(lret))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_bloc_0_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_dependencyrule(lret, &subtree) || m_predicatedefinition(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_bloc_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "bloc";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_bloc_0_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 46);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_dot(lret))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_bloc_0_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "bloc";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_sousbloc(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 30);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ';'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_bloc(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "bloc";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_predicatefact(lret, &subtree) || m_bloc_0_1(lret, &subtree) || m_bloc_0_2(lret, &subtree) || m_bloc_0_4(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_complexbloc(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "complexbloc";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_switch(lret, &subtree) || m_trycatch(lret, &subtree) || m_loop(lret, &subtree) || m_doloop(lret, &subtree) || m_test(lret, &subtree) || m_forin(lret, &subtree) || m_for(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_blocs_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "blocs";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_label(lret, &subtree) || m_extension(lret, &subtree) || m_frame(lret, &subtree) || m_complexbloc(lret, &subtree) || m_function(lret, &subtree) || m_bloc(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_blocs(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "blocs";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYWHILE
	subtree = NULL;
	if (m_blocs_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_blocs_0_1(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_analyse_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_signature(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_analyse_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_shell(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_analyse(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "analyse";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_analyse_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_analyse_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_blocs(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_wrong(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "wrong";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (x_test_string(lret, varstring4) || x_test_string(lret, varstring5) || x_test_string(lret, varstring31) || x_test_string(lret, varstring3) || x_test_string(lret, varstring32) || x_test_string(lret, varstring30) || x_test_string(lret, varstring29) || x_test_string(lret, varstring28))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_variable_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_interval(lret, &subtree) || m_indexes(lret, &subtree) || m_subfuncbis(lret, &subtree) || m_subvar(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_variable_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_variable_0_1_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_variable(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "variable";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYFAIL
	subtree = NULL;
	if (m_wrong(lret, &subtree)) {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_variable_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_allvariable(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "allvariable";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_predicatevariable(lret, &subtree) || m_variable(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_purevariable_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_interval(lret, &subtree) || m_indexes(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_purevariable_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_purevariable_0_1_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_purevariable(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "purevariable";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_purevariable_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_predvariable(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "predvariable";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_predicatevariable(lret, &subtree) || m_purevariable(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_plusplus_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '+') || !x_test_char(lret, '+')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_plusplus_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '-') || !x_test_char(lret, '-')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_plusplus(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "plusplus";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_plusplus_0_1(lret, &subtree) || m_plusplus_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_power(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "power";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (x_test_string(lret, varstring33) || x_test_string(lret, varstring34))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_affectation_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_valmaptail(lret, &subtree) || m_valvectortail(lret, &subtree) || m_variable(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_affectation_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "affectation";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_operatoraffectation(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 14);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_wnexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_affectation(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "affectation";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_affectation_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_affectation_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_increment_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_plusplus(lret, &subtree) || m_power(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_increment(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "increment";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_variable(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_increment_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_notin_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring3))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring31))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_notin_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring36))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring37))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_notin(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "notin";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (x_test_string(lret, varstring35) || m_notin_0_1(lret, &subtree) || m_notin_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_in(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "in";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_string(lret, varstring31))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_operatorin(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "operatorin";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_notin(lret, &subtree) || m_in(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_operationin(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "operationin";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_operatorin(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 18);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_negation(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "negation";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (x_test_char(lret, '~') || x_test_string(lret, varstring3))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_minus_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_astringdouble(lret, &subtree) || m_afullstring(lret, &subtree) || m_astringsimple(lret, &subtree) || m_variable(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_minus(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "minus";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '-'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_minus_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_indexes_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_minus(lret, &subtree) || m_expressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_indexes_0_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_interval(lret, &subtree) || m_indexes(lret, &subtree) || m_subfunc(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_indexes_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_indexes_0_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_indexes(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "indexes";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '['))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 1);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_indexes_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ']'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_indexes_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_interval_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_indexes_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_interval(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "interval";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '['))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_interval_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_sep(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 23);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_interval_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ']'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_indexes_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_valtuple_0_1_2_3_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_wnexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_valtuple_0_1_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYWHILE
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_valtuple_0_1_2_3_4(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	return(1);
}


char bnf_atanor::m_valtuple_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_wnexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_valtuple_0_1_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_valtuple_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_valtuple_0_1_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_valtuple(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "valtuple";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_wnexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 61);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_valtuple_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_valvector(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "valvector";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '['))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 25);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_valtuple_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ']'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_valtail(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "valtail";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '|'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_valvectortail_0_1_2_3_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_valvectortail_0_1_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYWHILE
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_valvectortail_0_1_2_3_4(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	return(1);
}


char bnf_atanor::m_valvectortail_0_1_2_5(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_valtail(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_valvectortail_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_valvectortail_0_1_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_valvectortail_0_1_2_5(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_valvectortail_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_valvectortail_0_1_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_valvectortail(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "valvectortail";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '['))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 25);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_valvectortail_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ']'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_maptail(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "maptail";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '|'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_variable(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_dicoval(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "dicoval";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ':'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_valmaptail_0_1_2_3_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_dicoval(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_valmaptail_0_1_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYWHILE
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_valmaptail_0_1_2_3_4(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	return(1);
}


char bnf_atanor::m_valmaptail_0_1_2_5(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_maptail(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_valmaptail_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_dicoval(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_valmaptail_0_1_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_valmaptail_0_1_2_5(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_valmaptail_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_valmaptail_0_1_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_valmaptail(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "valmaptail";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '{'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 26);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_valmaptail_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '}'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_intentionsep(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "intentionsep";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_string(lret, varstring38))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_step(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "step";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ':'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_wnexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_intentionvector_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "intentionvector";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_intentionsep(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 25);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_wnexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_intentionvector_0_1_3_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_wnexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_intentionvector_0_1_3_5(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_step(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_intentionvector_0_1_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "intentionvector";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_wnexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_intentionsep(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 25);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_intentionvector_0_1_3_4(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_intentionvector_0_1_3_5(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_intentionvector_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_intentionvector_0_1_2(lret, &subtree) || m_intentionvector_0_1_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_intentionvector(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "intentionvector";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '['))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_intentionvector_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ']'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_intentionwithdouble_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "intentionwithdouble";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_intentionsep(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 25);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_wnexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_wnexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_intentionwithdouble_0_1_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "intentionwithdouble";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_wnexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_wnexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_intentionsep(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 25);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_intentionvector_0_1_3_4(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_intentionwithdouble_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_intentionwithdouble_0_1_2(lret, &subtree) || m_intentionwithdouble_0_1_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_intentionwithdouble(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "intentionwithdouble";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '['))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_intentionwithdouble_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ']'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_intentionvect(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "intentionvect";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	bool fnd = false;
	bool nbcomma = false;
	unsigned char c;
	for (int ix = currentpos + 1; ix < fx->stack.size(); ix++) {
		c = fx->stack[ix][0];
		if (c == '[' || c == ']')
			return 0;
		if (c == ',')
		{
			if (nbcomma)
				return 0;
			nbcomma = true;
		}
		if (fx->stack[ix] == "..")
		{
			fnd = true;
			break;
		}
	}
	if (!fnd)
		return 0;

	//BODYOR
	subtree = NULL;
	if (m_intentionvector(lret, &subtree) || m_intentionwithdouble(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}

char bnf_atanor::m_dico(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "dico";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_wnexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ':'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_wnexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_valmap_0_1_2_3_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_dico(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_valmap_0_1_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYWHILE
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_valmap_0_1_2_3_4(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	return(1);
}


char bnf_atanor::m_valmap_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_dico(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_valmap_0_1_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_valmap_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_valmap_0_1_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_valmap(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "valmap";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '{'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 26);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_valmap_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '}'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_jvector_0_1_2_3_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_jexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_jvector_0_1_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYWHILE
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_jvector_0_1_2_3_4(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	return(1);
}


char bnf_atanor::m_jvector_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_jexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_jvector_0_1_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_jvector_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_jvector_0_1_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_jvector(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "jvector";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '['))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 25);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_jvector_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ']'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_jdico_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;

	if (currentpos >= fx->stack.size())
		return 0;
	unsigned char x = fx->stack[currentpos][intoken];
	bool err = true;
	switch (x) {
	case 34:
		if (m_astringdouble(lret, &subtree) || m_afullstring(lret, &subtree))
			err = false;
		break;
	case 39:
		if (m_astringsimple(lret, &subtree))
			err = false;
		break;
	case '0':
		if (m_xnumber(lret, &subtree) || m_anumber(lret, &subtree))
			err = false;
		break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '-':
		if (m_anumber(lret, &subtree))
			err = false;
		break;
	case 't':
	case 'f':
		if (m_abool(lret, &subtree))
		{
			err = false;
			break;
		}
	}
	if (err == false)
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}

char bnf_atanor::m_jdico(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "jdico";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_jdico_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ':'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_jexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_jmap_0_1_2_3_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_jdico(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_jmap_0_1_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYWHILE
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_jmap_0_1_2_3_4(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	return(1);
}


char bnf_atanor::m_jmap_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_jdico(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_jmap_0_1_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_jmap_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_jmap_0_1_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_jmap(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "jmap";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '{'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 26);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_jmap_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '}'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_jexpression(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "jexpression";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;

	if (currentpos >= fx->stack.size())
		return 0;
	unsigned char x = fx->stack[currentpos][intoken];
	bool err = true;
	switch (x) {
	case 34:
		if (m_astringdouble(lret, &subtree) || m_afullstring(lret, &subtree))
			err = false;
		break;
	case 39:
		if (m_astringsimple(lret, &subtree))
			err = false;
		break;
	case '[':
		if (m_jvector(lret, &subtree))
			err = false;
		break;
	case '{':
		if (m_jmap(lret, &subtree))
			err = false;
		break;
	case '0':
		if (m_xnumber(lret, &subtree) || m_anumber(lret, &subtree))
			err = false;
		break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '-':
		if (m_anumber(lret, &subtree))
			err = false;
		break;
	case 't':
	case 'f':
		if (m_abool(lret, &subtree))
		{
			err = false;
			break;
		}
	default:
		if (m_aconstant(lret, &subtree))
			err = false;
	}
	if (err == false)
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}

char bnf_atanor::m_aconstant(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "aconstant";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_string(lret, varstring39))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_cut(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "cut";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_char(lret, '!'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_fail(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "fail";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_string(lret, varstring40))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_stop(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "stop";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_string(lret, varstring41))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_predicate_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_predicateparameters(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_predicate(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "predicate";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYFAIL
	subtree = NULL;
	if (m_notafunction(lret, &subtree)) {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_predicate_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_predicatecall(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "predicatecall";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_functioncall(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_predicate_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_retractcommand(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "retractcommand";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_string(lret, varstring42))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_assertcommandbefore(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "assertcommandbefore";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_string(lret, varstring43))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_assertcommandafter(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "assertcommandafter";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (x_test_string(lret, varstring44) || x_test_string(lret, varstring45))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_assertpredicate_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_retractcommand(lret, &subtree) || m_assertcommandbefore(lret, &subtree) || m_assertcommandafter(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_assertpredicate_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_dependencyfact(lret, &subtree) || m_predicate(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_assertpredicate_0_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_valvectortail_0_1_2_3_4(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_assertpredicate(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "assertpredicate";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_assertpredicate_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 39);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_assertpredicate_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_assertpredicate_0_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_compfeat_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '~') || !x_test_char(lret, '-') || !x_test_char(lret, ':')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_compfeat_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '-') || !x_test_char(lret, ':')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_compfeat_0_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSYMBOL
	if (!x_test_char(lret, '~') || !x_test_char(lret, ':')) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_compfeat(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "compfeat";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_compfeat_0_1(lret, &subtree) || m_compfeat_0_2(lret, &subtree) || m_compfeat_0_3(lret, &subtree) || x_test_in(lret, tab3))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_valplus(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "valplus";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_char(lret, '+'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_feat_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_astringdouble(lret, &subtree) || m_afullstring(lret, &subtree) || m_astringsimple(lret, &subtree) || m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_feat_0_2_3_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_negation(lret, &subtree) || m_valplus(lret, &subtree) || m_xnumber(lret, &subtree) || m_anumber(lret, &subtree) || m_astringdouble(lret, &subtree) || m_afullstring(lret, &subtree) || m_astringsimple(lret, &subtree) || m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_feat_0_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_compfeat(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_feat_0_2_3_4(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_feat_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_feat_0_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_feat(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "feat";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_feat_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_feat_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_features_0_1_2_3_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_feat(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_features_0_1_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYWHILE
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_features_0_1_2_3_4(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_features_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_feat(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_features_0_1_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_features_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_features_0_1_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_features(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "features";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '['))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_features_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ']'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_modifcall(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "modifcall";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_char(lret, '^'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_dependencyparameters_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_predicatevariable(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_dependencyparameters_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYWHILE
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_dependencyparameters_0_1_2(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	return(1);
}


char bnf_atanor::m_dependencyparameters(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "dependencyparameters";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_predicatevariable(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_dependencyparameters_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_dependencyfact(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "dependencyfact";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_functioncall(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_features(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_predicate_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_dependency_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_modifcall(lret, &subtree) || m_negation(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_dependency_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_dependency_0_1_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_dependency_0_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_features(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_dependency_0_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//CONSTRAINT
	int foundsolution = 0;
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (x_test_char(lret, ')')) {
			foundsolution = 1;
			x_init_tree(tree, subtree, addsubtree);
			break;
		}
		subtree = NULL;
		if (m_dependencyparameters(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	if (foundsolution == 0) {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_dependency(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "dependency";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_dependency_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_functioncall(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_dependency_0_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_dependency_0_4(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_dependance(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "dependance";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_fail(lret, &subtree) || m_cut(lret, &subtree) || m_stop(lret, &subtree) || m_dependency(lret, &subtree) || m_predplusplus(lret, &subtree) || m_affectationpredicate(lret, &subtree) || m_comparepredicate(lret, &subtree) || m_valpredicatevector(lret, &subtree) || m_valmappredicate(lret, &subtree) || m_purecall(lret, &subtree) || m_wnexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_dependencyresult_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_predicatevariable(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_dependencyparameters_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_dependencyresult_0_4_5_6(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_dependency(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_dependencyresult_0_4_5(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYWHILE
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_dependencyresult_0_4_5_6(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	return(1);
}


char bnf_atanor::m_dependencyresult_0_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_dependency(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_dependencyresult_0_4_5(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_dependencyresult(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "dependencyresult";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_negation(lret, &subtree) || m_dependencyresult_0_1(lret, &subtree) || m_dependencyresult_0_4(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_dependencyrule_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (x_test_string(lret, varstring22) || x_test_string(lret, varstring23))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_dependencyrule_0_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_orand(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_dependance(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_dependencyrule_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//CONSTRAINT
	int foundsolution = 0;
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (x_test_char(lret, ')')) {
			foundsolution = 1;
			x_init_tree(tree, subtree, addsubtree);
			break;
		}
		subtree = NULL;
		if (m_dependencyrule_0_2_3(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	if (foundsolution == 0) {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_dependencyrule(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "dependencyrule";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_dependencyrule_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 62);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_dependance(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_dependencyrule_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_dependencyresult(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_predicatevariable_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "predicatevariable";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '?'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 40);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_predicatevariable_0_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (x_test_char(lret, '_') || m_anumber(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_predicatevariable_0_2_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_features(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_predicatevariable_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "predicatevariable";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '#'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 40);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_predicatevariable_0_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_predicatevariable_0_2_4(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_predicatevariable(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "predicatevariable";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_predicatevariable_0_1(lret, &subtree) || m_predicatevariable_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_term_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_predicateparameters(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_term(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "term";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '?'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 37);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_term_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_tuple(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "tuple";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_predicateparameters(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_affectexpression_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_xnumber(lret, &subtree) || m_anumber(lret, &subtree) || m_pexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_affectexpression_0_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_operation(lret, &subtree) || m_comparison(lret, &subtree) || m_operationin(lret, &subtree) || m_multiply(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_affectexpression_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_affectexpression_0_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_affectexpression(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "affectexpression";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_affectexpression_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_affectexpression_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_comparepredicate_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "comparepredicate";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_comparator(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 14);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_affectexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_comparepredicate(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "comparepredicate";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_predvariable(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_comparepredicate_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_affectationpredicate_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "affectationpredicate";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_operatoraffectation(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 14);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_affectexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_affectationpredicate(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "affectationpredicate";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_predvariable(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_affectationpredicate_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_predplusplus(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "predplusplus";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_predvariable(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_plusplus(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_pintentionvector_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "pintentionvector";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_intentionsep(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 25);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_pexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_pintentionvector_0_1_3_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_pexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_pintentionvector_0_1_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "pintentionvector";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_pexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_intentionsep(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 25);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_pintentionvector_0_1_3_4(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_intentionvector_0_1_3_5(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_pintentionvector_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_pintentionvector_0_1_2(lret, &subtree) || m_pintentionvector_0_1_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_pintentionvector(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "pintentionvector";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '['))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_pintentionvector_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ']'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_pintentionwithdouble_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "pintentionwithdouble";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_intentionsep(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 25);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_pexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_pexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_pintentionwithdouble_0_1_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "pintentionwithdouble";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_pexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_pexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_intentionsep(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 25);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_pintentionvector_0_1_3_4(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_pintentionwithdouble_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_pintentionwithdouble_0_1_2(lret, &subtree) || m_pintentionwithdouble_0_1_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_pintentionwithdouble(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "pintentionwithdouble";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '['))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_pintentionwithdouble_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ']'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_pintentionvect(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "pintentionvect";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	bool fnd = false;
	bool nbcomma = false;
	unsigned char c;
	for (int ix = currentpos + 1; ix < fx->stack.size(); ix++) {
		c = fx->stack[ix][0];
		if (c == '[' || c == ']')
			return 0;
		if (c == ',')
		{
			if (nbcomma)
				return 0;
			nbcomma = true;
		}
		if (fx->stack[ix] == "..")
		{
			fnd = true;
			break;
		}
	}
	if (!fnd)
		return 0;

	//BODYOR
	subtree = NULL;
	if (m_pintentionvector(lret, &subtree) || m_pintentionwithdouble(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}

char bnf_atanor::m_merging(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "merging";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (m_pexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_apredicatelist_0_1_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "apredicatelist";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 25);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_apredicatelist(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_apredicatelist_0_1_2_4_5(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYCONTEXTUAL
	long poscontext = currentpos;
	int itokcontext = intoken;
	string lretbis;
	if (x_test_char(lretbis, ']')) {
		currentpos = poscontext;
		intoken = itokcontext;
	}
	else {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_apredicatelist_0_1_2_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "apredicatelist";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '|'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 25);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_merging(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_apredicatelist_0_1_2_4_5(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_apredicatelist_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_apredicatelist_0_1_2_3(lret, &subtree) || m_apredicatelist_0_1_2_4(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_apredicatelist_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_apredicatelist_0_1_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_apredicatelist(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "apredicatelist";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_pexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_apredicatelist_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_valpredicatevector_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_apredicatelist(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_valpredicatevector(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "valpredicatevector";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '['))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_valpredicatevector_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ']'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_pexpression(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "pexpression";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;

	if (currentpos >= fx->stack.size())
		return 0;
	unsigned char x = fx->stack[currentpos][intoken];
	bool err = true;
	switch (x) {
	case '?':
		if (m_term(lret, &subtree) || m_predicatevariable(lret, &subtree))
			err = false;
		break;
	case '(':
		if (m_tuple(lret, &subtree))
			err = false;
		break;
	case '#':
		if (m_predicatevariable(lret, &subtree))
			err = false;
		break;
	case '[':
		if (m_pintentionvect(lret, &subtree) || m_valpredicatevector(lret, &subtree))
			err = false;
		break;
	case '{':
		if (m_valmappredicate(lret, &subtree))
			err = false;
		break;
	case 34:
		if (m_astringdouble(lret, &subtree) || m_afullstring(lret, &subtree))
			err = false;
		break;
	case 39:
		if (m_astringsimple(lret, &subtree))
			err = false;
		break;
	case '0':
		if (m_xnumber(lret, &subtree) || m_anumber(lret, &subtree))
			err = false;
		break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '-':
		if (m_anumber(lret, &subtree))
			err = false;
		break;
	case 't':
	case 'f':
		if (m_abool(lret, &subtree))
		{
			err = false;
			break;
		}
	default:
		if (m_purecall(lret, &subtree) || m_purevariable(lret, &subtree))
			err = false;
	}
	if (err == false)
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}

char bnf_atanor::m_predicateparameters_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_pexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_predicateparameters_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYWHILE
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_predicateparameters_0_1_2(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	return(1);
}


char bnf_atanor::m_predicateparameters(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "predicateparameters";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_pexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_predicateparameters_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_mapmerging(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "mapmerging";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (m_pexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_mappredicatelist_0_1_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_mappredicatelist(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_mappredicatelist_0_1_2_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '|'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_mapmerging(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_mappredicatelist_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_mappredicatelist_0_1_2_3(lret, &subtree) || m_mappredicatelist_0_1_2_4(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_mappredicatelist_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_mappredicatelist_0_1_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_mappredicatelist(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "mappredicatelist";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_predicatedico(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_mappredicatelist_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_valmappredicate_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_mappredicatelist(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_valmappredicate(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "valmappredicate";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '{'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 26);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_valmappredicate_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '}'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_predicatedico(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "predicatedico";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_pexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ':'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_pexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_predicateoperator(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "predicateoperator";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (x_test_in(lret, tab4))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_predicateexpression_0_1_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_negation(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_predicateexpression_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_predicateexpression_0_1_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_predicatecall(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_predicateexpression_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_fail(lret, &subtree) || m_cut(lret, &subtree) || m_stop(lret, &subtree) || m_assertpredicate(lret, &subtree) || m_predicateexpression_0_1_2(lret, &subtree) || m_predplusplus(lret, &subtree) || m_affectationpredicate(lret, &subtree) || m_comparepredicate(lret, &subtree) || m_valpredicatevector(lret, &subtree) || m_valmappredicate(lret, &subtree) || m_expressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_predicateexpression_0_4_5(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "predicateexpression";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_predicateoperator(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 35);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_predicateexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_predicateexpression_0_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_predicateexpression_0_4_5(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_predicateexpression(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "predicateexpression";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_predicateexpression_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_predicateexpression_0_4(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_predicatedefinition_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_abool(lret, &subtree) || m_predicateexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_predicatedefinition(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "predicatedefinition";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_predicate(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ':'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '-'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 20);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_predicatedefinition_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_predicatefact(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "predicatefact";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_assertpredicate_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_dot(lret))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_dcgword(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "dcgword";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_predicate(lret, &subtree) || m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_finaltoken_0_1_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;

	if (currentpos >= fx->stack.size())
		return 0;
	unsigned char x = fx->stack[currentpos][intoken];
	bool err = true;
	switch (x) {
	case '#':
		if (m_predicatevariable(lret, &subtree))
			err = false;
		break;
	case '?':
		if (m_predicatevariable(lret, &subtree))
			err = false;
		break;
	case 34:
		if (m_astringdouble(lret, &subtree))
			err = false;
		break;
	case 39:
		if (m_astringsimple(lret, &subtree))
			err = false;
		break;
	case '0':
		if (m_xnumber(lret, &subtree) || m_anumber(lret, &subtree))
			err = false;
		break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '-':
		if (m_anumber(lret, &subtree))
			err = false;
		break;
	default:
		if (m_astring(lret, &subtree))
			err = false;
	}
	if (err == false)
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	lreturn += lret;
	return(1);
}

char bnf_atanor::m_finaltoken_0_1_2_4_5(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_finaltoken_0_1_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_finaltoken_0_1_2_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYWHILE
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_finaltoken_0_1_2_4_5(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_finaltoken_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_finaltoken_0_1_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_finaltoken_0_1_2_4(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_finaltoken_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_finaltoken_0_1_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_finaltoken(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "finaltoken";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '['))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_finaltoken_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ']'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_finaldcg(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "finaldcg";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '{'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_predicateexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '}'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_dcg_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_finaltoken(lret, &subtree) || m_dcgword(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_dcg_0_1_3_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_dcgword(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_dcg_0_1_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYWHILE
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_dcg_0_1_3_4(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	return(1);
}


char bnf_atanor::m_dcg_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_dcg_0_1_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_dcg_0_1_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_dcg_0_5_6(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_finaldcg(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_dcg_0_5(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_dcg_0_5_6(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_dcg(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "dcg";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_dcgword(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '-'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '-'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '>'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 44);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_dcg_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_dcg_0_5(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_haskelltail_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (x_test_in(lret, tab5))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_haskelltail(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "haskelltail";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_haskelltail_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_haskellexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_haskellexpression(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "haskellexpression";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;

	if (currentpos >= fx->stack.size())
		return 0;
	unsigned char x = fx->stack[currentpos][intoken];
	bool err = true;
	switch (x) {
	case '{':
		if (m_haskellmap(lret, &subtree))
			err = false;
		break;
	case '[':
		if (m_haskellvector(lret, &subtree))
			err = false;
		break;
	case 34:
		if (m_astringdouble(lret, &subtree) || m_afullstring(lret, &subtree))
			err = false;
		break;
	case 39:
		if (m_astringsimple(lret, &subtree))
			err = false;
		break;
	case '0':
		if (m_xnumber(lret, &subtree) || m_anumber(lret, &subtree))
			err = false;
		break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '-':
		if (m_anumber(lret, &subtree))
			err = false;
		break;
	case 't':
	case 'f':
		if (m_abool(lret, &subtree))
		{
			err = false;
			break;
		}
	default:
		if (m_word(lret, &subtree))
			err = false;
	}
	if (err == false)
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}

char bnf_atanor::m_haskellvector_0_1_2_3_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_haskellexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_haskellvector_0_1_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYWHILE
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_haskellvector_0_1_2_3_4(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	return(1);
}


char bnf_atanor::m_haskellvector_0_1_2_5(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_haskelltail(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_haskellvector_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_haskellexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_haskellvector_0_1_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_haskellvector_0_1_2_5(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_haskellvector_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_haskellvector_0_1_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_haskellvector(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "haskellvector";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '['))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_haskellvector_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ']'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_haskellmaptail(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "haskellmaptail";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '|'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_variable(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_haskellkeymap(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "haskellkeymap";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;

	if (currentpos >= fx->stack.size())
		return 0;
	unsigned char x = fx->stack[currentpos][intoken];
	bool err = true;
	switch (x) {
	case 34:
		if (m_astringdouble(lret, &subtree) || m_afullstring(lret, &subtree))
			err = false;
		break;
	case 39:
		if (m_astringsimple(lret, &subtree))
			err = false;
		break;
	case '0':
		if (m_xnumber(lret, &subtree) || m_anumber(lret, &subtree))
			err = false;
		break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '-':
		if (m_anumber(lret, &subtree))
			err = false;
		break;
	case 't':
	case 'f':
		if (m_abool(lret, &subtree))
		{
			err = false;
			break;
		}
	default:
		if (m_word(lret, &subtree))
			err = false;
	}
	if (err == false)
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}

char bnf_atanor::m_haskelldico(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "haskelldico";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_haskellkeymap(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ':'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_haskellexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_haskellmap_0_1_2_3_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_haskelldico(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_haskellmap_0_1_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYWHILE
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_haskellmap_0_1_2_3_4(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	return(1);
}


char bnf_atanor::m_haskellmap_0_1_2_5(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_haskellmaptail(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_haskellmap_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_haskelldico(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_haskellmap_0_1_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_haskellmap_0_1_2_5(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_haskellmap_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_haskellmap_0_1_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_haskellmap(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "haskellmap";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '{'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_haskellmap_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '}'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_letkeyword(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "letkeyword";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_string(lret, varstring46))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_let_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_hmetafunctions(lret, &subtree) || m_nocomparisonexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_let(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "let";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_letkeyword(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_affectation_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '='))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_let_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_letmin_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_hmetafunctions(lret, &subtree) || m_nocomparisonnorinexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_letmin(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "letmin";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_variable(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '='))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_letmin_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_range_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_affectation_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '<'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '-'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_let_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_range(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "range";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_let(lret, &subtree) || m_range_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_haskell_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;

	if (currentpos >= fx->stack.size())
		return 0;
	unsigned char x = fx->stack[currentpos][intoken];
	bool err = true;
	switch (x) {
	case '{':
		if (m_haskellmap(lret, &subtree))
			err = false;
		break;
	case '[':
		if (m_haskellvector(lret, &subtree))
			err = false;
		break;
	case 34:
		if (m_astringdouble(lret, &subtree) || m_afullstring(lret, &subtree))
			err = false;
		break;
	case 39:
		if (m_astringsimple(lret, &subtree))
			err = false;
		break;
	case '0':
		if (m_xnumber(lret, &subtree) || m_anumber(lret, &subtree))
			err = false;
		break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '-':
		if (m_anumber(lret, &subtree))
			err = false;
		break;
	case 't':
	case 'f':
		if (m_abool(lret, &subtree))
		{
			err = false;
			break;
		}
	default:
		if (m_word(lret, &subtree))
			err = false;
	}
	if (err == false)
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}

char bnf_atanor::m_haskell_0_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_haskell_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_haskell_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//CONSTRAINT
	int foundsolution = 0;
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (x_test_char(lret, ')')) {
			foundsolution = 1;
			x_init_tree(tree, subtree, addsubtree);
			break;
		}
		subtree = NULL;
		if (m_haskell_0_2_3(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	if (foundsolution == 0) {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_haskell_0_5(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (x_test_in(lret, tab6))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_haskell(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "haskell";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_haskell_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_haskell_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_haskell_0_5(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_otherwise(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "otherwise";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '|'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring47))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '='))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_guard_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_otherwise(lret, &subtree) || m_guard(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_guard(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "guard";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '|'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_booleanexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '='))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 48);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_guard_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_hlambda_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_haskellmap(lret, &subtree) || m_haskellvector(lret, &subtree) || m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_hlambda_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//CONSTRAINT
	int foundsolution = 0;
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (x_test_char(lret, '-')) {
			foundsolution = 1;
			x_init_tree(tree, subtree, addsubtree);
			break;
		}
		subtree = NULL;
		if (m_hlambda_0_1(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	if (foundsolution == 0) {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_hlambda(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "hlambda";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '\\'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 54);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_hlambda_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_hlambda_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '>'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_wnexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_hboollambda(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "hboollambda";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '\\'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 54);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_hlambda_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_hlambda_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '>'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_booleanexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_hfunction_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYWHILE
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_expression(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	return(1);
}


char bnf_atanor::m_hfunction(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "hfunction";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_hfunction_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_lfold(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "lfold";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_string(lret, varstring48))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_rfold(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "rfold";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_string(lret, varstring49))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_lscan(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "lscan";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_string(lret, varstring50))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_rscan(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "rscan";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_string(lret, varstring51))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_fold(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "fold";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_lfold(lret, &subtree) || m_rfold(lret, &subtree) || m_rscan(lret, &subtree) || m_lscan(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_folding_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_hlambda(lret, &subtree) || m_hfunction(lret, &subtree) || m_operator(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_folding_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_hcompose(lret, &subtree) || m_hcomposecall(lret, &subtree) || m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_folding(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "folding";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_fold(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 50);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_folding_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_folding_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_haskellbooloperator(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "haskellbooloperator";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (x_test_string(lret, varstring5) || x_test_string(lret, varstring4))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_lfold1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "lfold1";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_string(lret, varstring52))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_rfold1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "rfold1";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_string(lret, varstring53))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_lscan1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "lscan1";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_string(lret, varstring54))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_rscan1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "rscan1";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_string(lret, varstring55))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_fold1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "fold1";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_lfold1(lret, &subtree) || m_rfold1(lret, &subtree) || m_rscan1(lret, &subtree) || m_lscan1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_folding1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "folding1";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_fold1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 50);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_folding_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_folding_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_filterkeyword(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "filterkeyword";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (x_test_string(lret, varstring56) || x_test_string(lret, varstring57) || x_test_string(lret, varstring58))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_takekeyword(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "takekeyword";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (x_test_string(lret, varstring59) || x_test_string(lret, varstring60))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_inverted(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "inverted";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_operator(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_mapping_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_hlambda(lret, &subtree) || m_operation(lret, &subtree) || m_inverted(lret, &subtree) || m_hfunction(lret, &subtree) || m_operator(lret, &subtree) || m_multiply(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_mapping(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "mapping";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring61))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 51);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_mapping_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_folding_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_filtering_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_hboollambda(lret, &subtree) || m_comparison(lret, &subtree) || m_operationin(lret, &subtree) || m_hfunction(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_filtering(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "filtering";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_filterkeyword(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 49);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_filtering_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_folding_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_haskellbooling(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "haskellbooling";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_haskellbooloperator(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 56);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_filtering_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_folding_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_taking(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "taking";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_takekeyword(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 49);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_folding_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_zipping_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_hlambda(lret, &subtree) || m_operator(lret, &subtree) || m_hfunction(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_zipping_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYWHILE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_expression(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	return(1);
}


char bnf_atanor::m_zipping(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "zipping";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring62))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_zipping_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_zipping_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_pairing(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "pairing";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring63))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_zipping_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_cycle(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "cycle";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (x_test_string(lret, varstring64) || x_test_string(lret, varstring65))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_cycling(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "cycling";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_cycle(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 52);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_repeating(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "repeating";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring66))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 53);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_flipping(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "flipping";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring67))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 57);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_mapping_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_haskellcaseotherwise(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "haskellcaseotherwise";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring47))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_haskellresult_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_valmaptail(lret, &subtree) || m_valvectortail(lret, &subtree) || m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_haskellresult(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "haskellresult";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_haskellresult_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '-'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '>'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_haskellcase_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_haskellresult(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_haskellcase_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYWHILE
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_haskellcase_0_1_2(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	return(1);
}


char bnf_atanor::m_haskellcase_0_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_haskellcaseotherwise(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_haskellcase(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "haskellcase";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring68))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 55);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_variable(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring69))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_haskellresult(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_haskellcase_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_haskellcase_0_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_hfunctioncall_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_operator(lret, &subtree) || m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_hfunctioncall_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYWHILE
	subtree = NULL;
	if (m_folding_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_folding_0_2(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	return(1);
}


char bnf_atanor::m_hfunctioncall(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "hfunctioncall";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_hfunctioncall_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_hfunctioncall_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_hoperator(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "hoperator";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (x_test_in(lret, tab7))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_hmetafunctions(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static map<string, int> keys;
	static bool init = false;
	if (!init) {
		keys["foldl"] = 1;
		keys["foldl1"] = 0;
		keys["scanr1"] = 0;
		keys["scanr"] = 1;
		keys["foldr1"] = 0;
		keys["foldr"] = 1;
		keys["scanl1"] = 0;
		keys["filter"] = 2;
		keys["and"] = 3;
		keys["scanl"] = 1;
		keys["takeWhile"] = 2;
		keys["flip"] = 4;
		keys["dropWhile"] = 2;
		keys["take"] = 5;
		keys["drop"] = 5;
		keys["map"] = 6;
		keys["zipWith"] = 7;
		keys["zip"] = 8;
		keys["repeat"] = 9;
		keys["cycle"] = 9;
		keys["replicate"] = 10;
		keys["or"] = 3;
		init = true;
	}
	static const char* label = "hmetafunctions";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;

	if (currentpos >= fx->stack.size())
		return 0;
	string x = fx->stack[currentpos];
	if (keys.find(x) == keys.end())
		return 0;
	int i = keys[x];
	bool err = true;
	switch (i) {
	case 1:
		if (m_folding(lret, &subtree))
			err = false;
		break;
	case 0:
		if (m_folding1(lret, &subtree))
			err = false;
		break;
	case 6:
		if (m_mapping(lret, &subtree))
			err = false;
		break;
	case 2:
		if (m_filtering(lret, &subtree))
			err = false;
		break;
	case 3:
		if (m_haskellbooling(lret, &subtree))
			err = false;
		break;
	case 5:
		if (m_taking(lret, &subtree))
			err = false;
		break;
	case 7:
		if (m_zipping(lret, &subtree))
			err = false;
		break;
	case 8:
		if (m_pairing(lret, &subtree))
			err = false;
		break;
	case 9:
		if (m_cycling(lret, &subtree))
			err = false;
		break;
	case 10:
		if (m_repeating(lret, &subtree))
			err = false;
		break;
	case 4:
		if (m_flipping(lret, &subtree))
			err = false;
		break;
	}
	if (err == false)
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}

char bnf_atanor::m_hcompose_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static map<string, int> keys;
	static bool init = false;
	if (!init) {
		keys["foldl"] = 1;
		keys["foldl1"] = 0;
		keys["scanr1"] = 0;
		keys["scanr"] = 1;
		keys["foldr1"] = 0;
		keys["foldr"] = 1;
		keys["scanl1"] = 0;
		keys["filter"] = 2;
		keys["and"] = 3;
		keys["scanl"] = 1;
		keys["takeWhile"] = 2;
		keys["flip"] = 4;
		keys["dropWhile"] = 2;
		keys["take"] = 5;
		keys["drop"] = 5;
		keys["map"] = 6;
		keys["zipWith"] = 7;
		keys["zip"] = 8;
		keys["repeat"] = 9;
		keys["cycle"] = 9;
		keys["replicate"] = 10;
		keys["or"] = 3;
		init = true;
	}
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;

	if (currentpos >= fx->stack.size())
		return 0;
	string x = fx->stack[currentpos];
	if (keys.find(x) == keys.end())
		return 0;
	int i = keys[x];
	bool err = true;
	switch (i) {
	case 6:
		if (m_mapping(lret, &subtree))
			err = false;
		break;
	case 2:
		if (m_filtering(lret, &subtree))
			err = false;
		break;
	case 3:
		if (m_haskellbooling(lret, &subtree))
			err = false;
		break;
	case 5:
		if (m_taking(lret, &subtree))
			err = false;
		break;
	case 7:
		if (m_zipping(lret, &subtree))
			err = false;
		break;
	case 8:
		if (m_pairing(lret, &subtree))
			err = false;
		break;
	case 1:
		if (m_folding(lret, &subtree))
			err = false;
		break;
	case 0:
		if (m_folding1(lret, &subtree))
			err = false;
		break;
	case 9:
		if (m_cycling(lret, &subtree))
			err = false;
		break;
	case 10:
		if (m_repeating(lret, &subtree))
			err = false;
		break;
	case 4:
		if (m_flipping(lret, &subtree))
			err = false;
		break;
	}
	if (err == false)
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}

char bnf_atanor::m_hcompose(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "hcompose";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_dot(lret))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_hcompose_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_hcomposecall(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "hcomposecall";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_dot(lret))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_hfunctioncall(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_hoper(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "hoper";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;

	if (currentpos >= fx->stack.size())
		return 0;
	unsigned char x = fx->stack[currentpos][intoken];
	bool err = true;
	switch (x) {
	case '&':
		if (m_operator_0_1(lret, &subtree) || x_test_in(lret, tab8))
			err = false;
		break;
	case ':':
		if (m_operator_0_2(lret, &subtree))
			err = false;
		break;
	case '<':
		if (m_operator_0_3(lret, &subtree))
			err = false;
		break;
	case '>':
		if (m_operator_0_4(lret, &subtree))
			err = false;
		break;
	case '^':
		if (m_operator_0_5(lret, &subtree) || x_test_in(lret, tab8))
			err = false;
		break;
	case '*':
		if (m_operator_0_6(lret, &subtree) || x_test_in(lret, tab8))
			err = false;
		break;
	case '+':
		if (x_test_in(lret, tab8))
			err = false;
		break;
	case '-':
		if (x_test_in(lret, tab8))
			err = false;
		break;
	case '/':
		if (x_test_in(lret, tab8))
			err = false;
		break;
	case '%':
		if (x_test_in(lret, tab8))
			err = false;
		break;
	}
	if (err == false)
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}

char bnf_atanor::m_hoperation(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "hoperation";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_hoper(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_hexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_hexpression(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "hexpression";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;

	if (currentpos >= fx->stack.size())
		return 0;
	unsigned char x = fx->stack[currentpos][intoken];
	bool err = true;
	switch (x) {
	case 34:
		if (m_astringdouble(lret, &subtree) || m_afullstring(lret, &subtree))
			err = false;
		break;
	case 39:
		if (m_astringsimple(lret, &subtree))
			err = false;
		break;
	case '(':
		if (m_valtuple(lret, &subtree) || m_optional(lret, &subtree))
			err = false;
		break;
	case '<':
		if (m_telque(lret, &subtree))
			err = false;
		break;
	case '[':
		if (m_intentionvect(lret, &subtree) || m_valvector(lret, &subtree))
			err = false;
		break;
	case '{':
		if (m_valmap(lret, &subtree))
			err = false;
		break;
	case '0':
		if (m_xnumber(lret, &subtree) || m_anumber(lret, &subtree))
			err = false;
		break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '-':
		if (m_anumber(lret, &subtree))
			err = false;
		break;
	case 'c':
		if (m_haskellcase(lret, &subtree) || m_call(lret, &subtree) || m_increment(lret, &subtree) || m_allvariable(lret, &subtree))
			err = false;
		break;
	case 'i':
		if (m_localif(lret, &subtree) || m_call(lret, &subtree) || m_increment(lret, &subtree) || m_allvariable(lret, &subtree))
			err = false;
		break;
	case 't':
	case 'f':
		if (m_abool(lret, &subtree) || m_call(lret, &subtree) || m_increment(lret, &subtree) || m_allvariable(lret, &subtree))
			err = false;
		break;
	default:
		if (m_call(lret, &subtree) || m_increment(lret, &subtree) || m_allvariable(lret, &subtree))
			err = false;
	}
	if (err == false)
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}

char bnf_atanor::m_hexpressions_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_hoperation(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_hexpressions(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "hexpressions";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_hexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_multiply_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_hexpressions_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_whereexpression_0_1_2_3_4_5(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (x_test_string(lret, varstring46))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_whereexpression_0_1_2_3_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_whereexpression_0_1_2_3_4_5(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '='))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_whereexpression_0_1_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_whereexpression_0_1_2_3_4(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_whereexpression_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_whereexpression_0_1_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_nocomparisonexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ';'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_whereexpression_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYWHILE
	subtree = NULL;
	if (m_whereexpression_0_1_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_whereexpression_0_1_2(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	return(1);
}


char bnf_atanor::m_whereexpression(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "whereexpression";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring70))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_whereexpression_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_hinexpression(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "hinexpression";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring31))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_nocomparisonexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_telque_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_haskell(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_telque_0_1_3_4_5_6_7(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (x_test_in(lret, tab9))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_telque_0_1_3_4_5_6(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_telque_0_1_3_4_5_6_7(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_letmin(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_telque_0_1_3_4_5(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//CONSTRAINT
	int foundsolution = 0;
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_hinexpression(lret, &subtree)) {
			foundsolution = 1;
			x_init_tree(tree, subtree, addsubtree);
			break;
		}
		subtree = NULL;
		if (m_telque_0_1_3_4_5_6(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	if (foundsolution == 0) {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_telque_0_1_3_4_8_9(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_whereexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_telque_0_1_3_4_8(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_telque_0_1_3_4_8_9(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_telque_0_1_3_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring46))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_letmin(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_telque_0_1_3_4_5(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_telque_0_1_3_4_8(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_telque_0_1_3_10_11(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	long max = 0;
	vector<x_couple*> results;
	lret = "";
	subtree = NULL;
	currentpos = pos;
	intoken = itok;
	if (m_hexpressions(lret, &subtree)) {
		results.push_back(new x_couple(currentpos, intoken, subtree, lret));
		if (max < currentpos) {
			max = currentpos;
		}
	}
	gFail = 0;
	lret = "";
	subtree = NULL;
	currentpos = pos;
	intoken = itok;
	if (m_hfunctioncall(lret, &subtree)) {
		results.push_back(new x_couple(currentpos, intoken, subtree, lret));
		if (max < currentpos) {
			max = currentpos;
		}
	}
	gFail = 0;
	if (results.size() == 0) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	char add = 0;
	for (int i = 0; i < results.size(); i++) {
		if (add == 0 && max == results[i]->pos) {
			add = 1;
			currentpos = results[i]->pos;
			intoken = results[i]->intoken;
			results[i]->set(lret);
			x_init_tree(tree, results[i]->tree, addsubtree);
		}
		else
			delete results[i]->tree;
		delete results[i];
	}
	return(1);
}


char bnf_atanor::m_telque_0_1_3_10_12_13_14(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (x_test_char(lret, '|') || x_test_string(lret, varstring70))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_telque_0_1_3_10_12_13_15_16_17_18(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_hoperator(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_range(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_telque_0_1_3_10_12_13_15_16_17(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYWHILE
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_telque_0_1_3_10_12_13_15_16_17_18(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	return(1);
}


char bnf_atanor::m_telque_0_1_3_10_12_13_15_16_19_20_21_22(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_booleanexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_telque_0_1_3_4_8(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_telque_0_1_3_10_12_13_15_16_19_20_21(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_whereexpression(lret, &subtree) || m_telque_0_1_3_10_12_13_15_16_19_20_21_22(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_telque_0_1_3_10_12_13_15_16_19_20(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_telque_0_1_3_10_12_13_15_16_19_20_21(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_telque_0_1_3_10_12_13_15_16_19(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_telque_0_1_3_10_12_13_15_16_19_20(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_telque_0_1_3_10_12_13_15_16(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_range(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_telque_0_1_3_10_12_13_15_16_17(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_telque_0_1_3_10_12_13_15_16_19(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_telque_0_1_3_10_12_13_15(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_telque_0_1_3_10_12_13_15_16(lret, &subtree) || m_whereexpression(lret, &subtree) || m_booleanexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_telque_0_1_3_10_12_13(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_telque_0_1_3_10_12_13_14(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_telque_0_1_3_10_12_13_15(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_telque_0_1_3_10_12(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_telque_0_1_3_10_12_13(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_telque_0_1_3_10(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_telque_0_1_3_10_11(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_telque_0_1_3_10_12(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_telque_0_1_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_hmetafunctions(lret, &subtree) || m_guard(lret, &subtree) || m_telque_0_1_3_4(lret, &subtree) || m_telque_0_1_3_10(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_telque_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_telque_0_1_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_telque_0_1_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_telque(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "telque";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '<'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 48);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_telque_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '>'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_optional(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "optional";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 4);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_wnexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_expression_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_negation(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_expression_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;

	if (currentpos >= fx->stack.size())
		return 0;
	unsigned char x = fx->stack[currentpos][intoken];
	bool err = true;
	switch (x) {
	case 34:
		if (m_astringdouble(lret, &subtree) || m_afullstring(lret, &subtree))
			err = false;
		break;
	case 39:
		if (m_astringsimple(lret, &subtree))
			err = false;
		break;
	case '(':
		if (m_valtuple(lret, &subtree) || m_optional(lret, &subtree))
			err = false;
		break;
	case '<':
		if (m_telque(lret, &subtree))
			err = false;
		break;
	case '[':
		if (m_intentionvect(lret, &subtree) || m_valvector(lret, &subtree))
			err = false;
		break;
	case '{':
		if (m_valmap(lret, &subtree))
			err = false;
		break;
	case '0':
		if (m_xnumber(lret, &subtree) || m_anumber(lret, &subtree))
			err = false;
		break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '-':
		if (m_anumber(lret, &subtree))
			err = false;
		break;
	case 'c':
		if (m_haskellcase(lret, &subtree) || m_call(lret, &subtree) || m_increment(lret, &subtree) || m_allvariable(lret, &subtree))
			err = false;
		break;
	case 'i':
		if (m_localif(lret, &subtree) || m_call(lret, &subtree) || m_increment(lret, &subtree) || m_allvariable(lret, &subtree))
			err = false;
		break;
	case 't':
	case 'f':
		if (m_abool(lret, &subtree) || m_call(lret, &subtree) || m_increment(lret, &subtree) || m_allvariable(lret, &subtree))
			err = false;
		break;
	default:
		if (m_call(lret, &subtree) || m_increment(lret, &subtree) || m_allvariable(lret, &subtree))
			err = false;
	}
	if (err == false)
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	lreturn += lret;
	return(1);
}

char bnf_atanor::m_expression(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "expression";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_wnexpressions_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_xnumber(lret, &subtree) || m_anumber(lret, &subtree) || m_negated(lret, &subtree) || m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_wnexpressions(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "wnexpressions";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_wnexpressions_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_multiply_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_affectexpression_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_nocomparisonexpressions(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "nocomparisonexpressions";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_wnexpressions_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_multiply_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_multiply_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_nocomparisonnorinexpressions_0_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_operation(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_nocomparisonnorinexpressions(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "nocomparisonnorinexpressions";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_wnexpressions_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_multiply_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_nocomparisonnorinexpressions_0_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_expressions(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "expressions";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_multiply_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_affectexpression_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_numbers_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_operation(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_numbers(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "numbers";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_numbers_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_negated(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "negated";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '-'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_xnumber_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (x_test_ord(lret, '0', '9') || x_test_ord(lret, 'A', 'F') || x_test_ord(lret, 'a', 'f'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_xnumber_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYWHILE
	subtree = NULL;
	if (m_xnumber_0_1_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_xnumber_0_1_2(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_xnumber(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "xnumber";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '0'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, 'x'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_xnumber_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_abool(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "abool";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (x_test_string(lret, varstring71) || x_test_string(lret, varstring72))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_blocfor_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_affectation(lret, &subtree) || m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_blocfor_0_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_blocfor(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_blocfor_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_blocfor_0_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_blocfor(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "blocfor";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_blocfor_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_blocfor_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_localif_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring32))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_localif_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_localif_0_1_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_localif(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "localif";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring21))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 10);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_booleanexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_localif_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_declarationfor_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_variable(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring31))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_declarationfor_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_type(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_variable(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring31))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_declarationfor(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "declarationfor";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_declarationfor_0_1(lret, &subtree) || m_declarationfor_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_optionalboolean(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "optionalboolean";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_booleanexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_booleanexpression_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	long max = 0;
	vector<x_couple*> results;
	lret = "";
	subtree = NULL;
	currentpos = pos;
	intoken = itok;
	if (m_optionalboolean(lret, &subtree)) {
		results.push_back(new x_couple(currentpos, intoken, subtree, lret));
		if (max < currentpos) {
			max = currentpos;
		}
	}
	gFail = 0;
	lret = "";
	subtree = NULL;
	currentpos = pos;
	intoken = itok;
	if (m_expressions(lret, &subtree)) {
		results.push_back(new x_couple(currentpos, intoken, subtree, lret));
		if (max < currentpos) {
			max = currentpos;
		}
	}
	gFail = 0;
	if (results.size() == 0) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	char add = 0;
	for (int i = 0; i < results.size(); i++) {
		if (add == 0 && max == results[i]->pos) {
			add = 1;
			currentpos = results[i]->pos;
			intoken = results[i]->intoken;
			results[i]->set(lret);
			x_init_tree(tree, results[i]->tree, addsubtree);
		}
		else
			delete results[i]->tree;
		delete results[i];
	}
	return(1);
}


char bnf_atanor::m_booleanexpression_0_2_3_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	long max = 0;
	vector<x_couple*> results;
	lret = "";
	subtree = NULL;
	currentpos = pos;
	intoken = itok;
	if (m_optionalboolean(lret, &subtree)) {
		results.push_back(new x_couple(currentpos, intoken, subtree, lret));
		if (max < currentpos) {
			max = currentpos;
		}
	}
	gFail = 0;
	lret = "";
	subtree = NULL;
	currentpos = pos;
	intoken = itok;
	if (m_booleanexpression(lret, &subtree)) {
		results.push_back(new x_couple(currentpos, intoken, subtree, lret));
		if (max < currentpos) {
			max = currentpos;
		}
	}
	gFail = 0;
	if (results.size() == 0) {
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	char add = 0;
	for (int i = 0; i < results.size(); i++) {
		if (add == 0 && max == results[i]->pos) {
			add = 1;
			currentpos = results[i]->pos;
			intoken = results[i]->intoken;
			results[i]->set(lret);
			x_init_tree(tree, results[i]->tree, addsubtree);
		}
		else
			delete results[i]->tree;
		delete results[i];
	}
	return(1);
}


char bnf_atanor::m_booleanexpression_0_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "booleanexpression";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_orand(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 8);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_booleanexpression_0_2_3_4(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_booleanexpression_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_booleanexpression_0_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_booleanexpression(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "booleanexpression";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_booleanexpression_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_booleanexpression_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_arange(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "arange";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '<'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 41);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_wnexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_wnexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_wnexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '>'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_test_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_instruction(lret, &subtree) || m_complexbloc(lret, &subtree) || m_bloc(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_test_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_testelif(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_test_0_3_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "test";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring32))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 24);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_test_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_test_0_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_test_0_3_4(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_test(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "test";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring21))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 10);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_booleanexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_test_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_test_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_test_0_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_testelif(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "testelif";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring27))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 10);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_booleanexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_test_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_test_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_test_0_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_loop(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "loop";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring24))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 11);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_booleanexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_test_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_doloop(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "doloop";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring73))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_test_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring24))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 11);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_booleanexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_declarationending(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_for_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_multideclaration(lret, &subtree) || m_blocfor(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_for(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "for";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring25))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 12);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_for_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ';'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_booleanexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ';'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_blocfor(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_test_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_forin_0_1_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;

	if (currentpos >= fx->stack.size())
		return 0;
	unsigned char x = fx->stack[currentpos][intoken];
	bool err = true;
	switch (x) {
	case '{':
		if (m_valmaptail(lret, &subtree))
			err = false;
		break;
	case '[':
		if (m_valvectortail(lret, &subtree))
			err = false;
		break;
	}
	if (err == false)
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}

char bnf_atanor::m_forin_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_forin_0_1_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring31))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_forin_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_forin_0_1_2(lret, &subtree) || m_declarationfor(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_forin_0_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_arange(lret, &subtree) || m_expressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_forin(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "forin";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring25))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_forin_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 33);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_forin_0_4(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_test_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_switch_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//CONSTRAINT
	subtree = NULL;
	if (x_test_char(lret, '}')) {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	subtree = NULL;
	if (m_testswitch(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	int foundsolution = 0;
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (x_test_char(lret, '}')) {
			foundsolution = 1;
			x_init_tree(tree, subtree, addsubtree);
			break;
		}
		subtree = NULL;
		if (m_testswitch(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	if (foundsolution == 0) {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_switch(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "switch";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring26))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 16);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_expression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_declaration_0_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '{'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 31);
	bnf_atanor_pop pop1(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_switch_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_trycatch_0_1_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_trycatch_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_trycatch_0_1_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_trycatch_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_trycatch_0_1_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_trycatch_0_4_5(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "trycatch";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '{'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 22);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_blocs(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '}'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_trycatch_0_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (x_test_char(lret, ';') || m_trycatch_0_4_5(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_trycatch(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "trycatch";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring74))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '{'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 22);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_blocs(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '}'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, varstring75))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 22);
	bnf_atanor_pop pop1(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_trycatch_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_trycatch_0_4(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_testswitch_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;

	if (currentpos >= fx->stack.size())
		return 0;
	unsigned char x = fx->stack[currentpos][intoken];
	bool err = true;
	switch (x) {
	case 34:
		if (m_astringdouble(lret, &subtree) || m_afullstring(lret, &subtree))
			err = false;
		break;
	case 39:
		if (m_astringsimple(lret, &subtree))
			err = false;
		break;
	case '0':
		if (m_xnumber(lret, &subtree) || m_anumber(lret, &subtree))
			err = false;
		break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '-':
		if (m_anumber(lret, &subtree))
			err = false;
		break;
	case 't':
	case 'f':
		if (m_abool(lret, &subtree))
		{
			err = false;
			break;
		}
	default:
		if (m_default(lret, &subtree))
			err = false;
	}
	if (err == false)
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}

char bnf_atanor::m_testswitch(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "testswitch";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_testswitch_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ':'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 17);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_test_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_default(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "default";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_string(lret, varstring76))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_lispexpression(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "lispexpression";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_xnumber(lret, &subtree) || m_astringdouble(lret, &subtree) || m_afullstring(lret, &subtree) || m_astringsimple(lret, &subtree) || m_anumber(lret, &subtree) || m_astring(lret, &subtree) || m_punct(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_lispexpressions(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "lispexpressions";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_onelisp(lret, &subtree) || m_lispexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_onelisp_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//CONSTRAINT
	int foundsolution = 0;
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (x_test_char(lret, Y_var_1)) {
			foundsolution = 1;
			x_init_tree(tree, subtree, addsubtree);
			break;
		}
		subtree = NULL;
		if (m_lispexpressions(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	if (foundsolution == 0) {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_onelisp(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "onelisp";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, Y_var_0))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_lispexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 42);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_onelisp_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_lisp_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYWHILE
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_onelisp(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_lisp(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "lisp";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_onelisp(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_lisp_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_tagexpression(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "tagexpression";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_xnumber(lret, &subtree) || m_astringdouble(lret, &subtree) || m_afullstring(lret, &subtree) || m_astringsimple(lret, &subtree) || m_anumber(lret, &subtree) || m_astring(lret, &subtree) || m_punct(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_tagexpressions(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "tagexpressions";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_onetag(lret, &subtree) || m_tagexpression(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_onetag_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//CONSTRAINT
	int foundsolution = 0;
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (x_test_string(lret, VS_var_3)) {
			foundsolution = 1;
			x_init_tree(tree, subtree, addsubtree);
			break;
		}
		subtree = NULL;
		if (m_tagexpressions(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	if (foundsolution == 0) {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_onetag(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "onetag";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_string(lret, VS_var_2))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_tagexpressions(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 43);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_onetag_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_tag_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYWHILE
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_onetag(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_tag(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "tag";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_onetag(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_tag_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_ameta_0_1_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (x_test_in(lret, tab10))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_ameta_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "ameta";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '%'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	exitonfail = true;
	initfail(label, currentpos, 59);
	bnf_atanor_pop pop0(this);
	//BODYSEQUENCE
	subtree = NULL;
	if (m_ameta_0_1_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_ameta_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (x_test_char(lret, '?') || m_ameta_0_1_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_ameta_0_4(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (x_test_char(lret, '_'))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	lreturn += lret;
	return(1);
}


char bnf_atanor::m_ameta(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "ameta";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_ameta_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_ameta_0_4(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_amulti(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "amulti";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '$'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_asimple(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "asimple";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_atoken_0_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_subgram(lret, &subtree) || m_ameta(lret, &subtree) || m_xnumber(lret, &subtree) || m_amulti(lret, &subtree) || m_astringdouble(lret, &subtree) || m_astringsimple(lret, &subtree) || m_anumber(lret, &subtree) || m_abool(lret, &subtree) || m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_atoken(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "atoken";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_predicateexpression_0_1_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_atoken_0_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_anitem(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "anitem";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_ameta(lret, &subtree) || m_xnumber(lret, &subtree) || m_amulti(lret, &subtree) || m_astringdouble(lret, &subtree) || m_astringsimple(lret, &subtree) || m_anumber(lret, &subtree) || m_abool(lret, &subtree) || m_asimple(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_dj(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "dj";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	if (x_test_char(lret, ';'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_aoptional(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "aoptional";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_atoken(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_aoptitem(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "aoptitem";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '('))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_anitem(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ')'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_akleene(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "akleene";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (x_test_in(lret, tab11))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, true);
	return(1);
}


char bnf_atanor::m_anelement_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_akleene(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_anelement_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_atoken(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_anelement_0_1_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_anelement(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "anelement";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_aoptional(lret, &subtree) || m_anelement_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_anitemelement_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_anitem(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_anelement_0_1_2(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_anitemelement(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "anitemelement";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_aoptitem(lret, &subtree) || m_anitemelement_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_body_0_1_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOR
	subtree = NULL;
	if (m_dj(lret, &subtree) || x_test_char(lret, ','))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return 0;
	}
	return(1);
}


char bnf_atanor::m_body_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_body_0_1_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_anitemelement(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_body_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYWHILE
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_body_0_1_2(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	return(1);
}


char bnf_atanor::m_body(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "body";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_anitemelement(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_body_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_rule_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_body_0_1_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_anelement(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_rule_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//CONSTRAINT
	int foundsolution = 0;
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (x_test_dot(lret)) {
			foundsolution = 1;
			x_init_tree(tree, subtree, addsubtree);
			break;
		}
		subtree = NULL;
		if (m_rule_0_1_2(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	if (foundsolution == 0) {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_rule(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "rule";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (m_word(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, ':'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '='))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_anelement(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_rule_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_subgram_0_1_2_3(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '|'))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_body(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_subgram_0_1_2(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYOPTIONAL
	subtree = NULL;
	if (m_subgram_0_1_2_3(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else
		lret = "";
	return(1);
}


char bnf_atanor::m_subgram_0_1(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//CONSTRAINT
	int foundsolution = 0;
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (x_test_char(lret, ']')) {
			foundsolution = 1;
			x_init_tree(tree, subtree, addsubtree);
			break;
		}
		subtree = NULL;
		if (m_subgram_0_1_2(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	if (foundsolution == 0) {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	return(1);
}


char bnf_atanor::m_subgram(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "subgram";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYSEQUENCE
	subtree = NULL;
	if (x_test_char(lret, '['))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_body(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	//BODYSEQUENCE
	subtree = NULL;
	if (m_subgram_0_1(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}


char bnf_atanor::m_rules(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	static const char* label = "rules";
	string lret;
	long pos = currentpos;
	int itok = intoken;
	x_node* subtree = NULL;
	int addsubtree = 0;
	bool exitonfail = false;
	//BODYWHILE
	subtree = NULL;
	if (m_rule(lret, &subtree))
		x_init_tree(tree, subtree, addsubtree);
	else {
		x_pop_node(tree, addsubtree);
		currentpos = pos;
		intoken = itok;
		setfail(exitonfail);
		return(0);
	}
	while (currentpos < fx->stack.size()) {
		subtree = NULL;
		if (m_rule(lret, &subtree))
			x_init_tree(tree, subtree, addsubtree);
		else
			break;
	}
	x_init_node(lreturn, lret, tree, label, pos, false);
	return(1);
}




string bnf_atanor::x_errormsg(int i) {
	return errorlabels[i];
}


x_node* bnf_atanor::x_parsing(x_reading* xr, x_parsing_mode mode, bool display) {
	fx = xr;
	string lret;
	x_node* tree = new x_node;
	gFail = 0;
	lineerror = -1;
	labelerror = "";
	errornumber = -1;
	currentpos = 0;
	intoken = 0;
	labelerrors.clear();
	lineerrors.clear();
	errornumbers.clear();

	char res = m_analyse(lret, &tree);

	if (currentpos != fx->stack.size() && mode == FULL) {
		if (display) {
			if (errornumber != -1)
				cerr << endl << "Error line:" << lineerror << " => " << errorlabels[errornumber];
			else
				cerr << endl << "Error line:" << lineerror;
		}
		if (tree != NULL)
			delete tree;
		return NULL;
	}

	if (res == 1)
		return tree;

	delete tree;
	return NULL;
}



char bnf_atanor::x_test_dot(string& lret) {
	if (currentpos >= fx->stack.size())
		return(0);
	if (fx->stack[currentpos] == ".") {
		incrementpos();
		lret += ".";
		return 1;
	}
	return 0;
}


char bnf_atanor::x_test_string(string& lret, string sub) {
	if (currentpos >= fx->stack.size())
		return(0);
	if (fx->stack[currentpos] == sub) {
		incrementpos();
		lret += sub;
		return 1;
	}
	return 0;
}


char bnf_atanor::m_punct(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	if (currentpos >= fx->stack.size())
		return(0);
	static const char* label = "punct";
	if (fx->stacktype[currentpos] == 0) {
		x_init_node(lreturn, fx->stack[currentpos], tree, label, currentpos, true);
		incrementpos();
		return(1);
	}
	return(0);
}

char bnf_atanor::m_word(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	if (currentpos >= fx->stack.size())
		return(0);
	static const char* label = "word";
	if (fx->stacktype[currentpos] == 4) {
		x_init_node(lreturn, fx->stack[currentpos], tree, label, currentpos, true);
		incrementpos();
		return(1);
	}
	return(0);
}

char bnf_atanor::m_typename(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	if (currentpos >= fx->stack.size())
		return(0);
	static const char* label = "typename";
	if (fx->stacktype[currentpos] == 4) {
		x_init_node(lreturn, fx->stack[currentpos], tree, label, currentpos, true);
		incrementpos();
		return(1);
	}
	return(0);
}

char bnf_atanor::m_astring(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	if (currentpos >= fx->stack.size())
		return(0);
	static const char* label = "astring";
	if (fx->stacktype[currentpos] == 4) {
		x_init_node(lreturn, fx->stack[currentpos], tree, label, currentpos, true);
		incrementpos();
		return(1);
	}
	return(0);
}

char bnf_atanor::m_anumber(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	if (currentpos >= fx->stack.size())
		return(0);
	static const char* label = "anumber";
	if (fx->stacktype[currentpos] == 3) {
		x_init_node(lreturn, fx->stack[currentpos], tree, label, currentpos, true);
		incrementpos();
		return(1);
	}
	if ((fx->stack[currentpos] == "-" || fx->stack[currentpos] == "+") && fx->stacktype[currentpos + 1] == 3) {
		string s = fx->stack[currentpos];
		s += fx->stack[currentpos + 1];
		x_init_node(lreturn, s, tree, label, currentpos, true);
		currentpos += 2;
		return(1);
	}
	return(0);
}

char bnf_atanor::m_astringsimple(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	if (currentpos >= fx->stack.size())
		return(0);
	if (fx->stacktype[currentpos] == 2) {
		static const char* label = "astringsimple";
		x_init_node(lreturn, fx->stack[currentpos], tree, label, currentpos, true);
		incrementpos();
		return(1);
	}
	if (fx->stacktype[currentpos] == 7) {
		static const char* label = "ustringsimple";
		x_init_node(lreturn, fx->stack[currentpos], tree, label, currentpos, true);
		incrementpos();
		return(1);
	}
	return(0);
}

char bnf_atanor::m_astringdouble(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	if (currentpos >= fx->stack.size())
		return(0);
	if (fx->stacktype[currentpos] == 1) {
		static const char* label = "astringdouble";
		x_init_node(lreturn, fx->stack[currentpos], tree, label, currentpos, true);
		incrementpos();
		return(1);
	}
	if (fx->stacktype[currentpos] == 6) {
		static const char* label = "ustringdouble";
		x_init_node(lreturn, fx->stack[currentpos], tree, label, currentpos, true);
		incrementpos();
		return(1);
	}
	return(0);
}

char bnf_atanor::m_afullstring(string& lreturn, x_node** tree) {
	if (gFail == 1) return(0);
	if (currentpos >= fx->stack.size())
		return(0);
	if (fx->stacktype[currentpos] == 5) {
		static const char* label = "afullstring";
		x_init_node(lreturn, fx->stack[currentpos], tree, label, currentpos, true);
		incrementpos();
		return(1);
	}
	if (fx->stacktype[currentpos] == 8) {
		static const char* label = "ufullstring";
		x_init_node(lreturn, fx->stack[currentpos], tree, label, currentpos, true);
		incrementpos();
		return(1);
	}
	return(0);
}
