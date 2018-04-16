#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "linear.h"

#include "atanor.h"
#include "atanorversion.h"
#include "liblinear.h"


static map<string, int> solvers;

//We need to declare once again our local definitions.
hmap<unsigned short, liblinearMethod>  Atanorliblinear::methods;
hmap<string, string> Atanorliblinear::infomethods;
bin_hash<unsigned long> Atanorliblinear::exported;

short Atanorliblinear::idtype = 0;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorliblinear;
//This typedef defines a type "liblinearMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorliblinear::*liblinearMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

void print_null(const char *s) {}


Atanorliblinear::Atanorliblinear(AtanorGlobal* g, Atanor* parent) : AtanorObject(g, parent) {
	//Do not forget your variable initialisation

	solvers["L2R_LR"] = 0;
	solvers["L2R_L2LOSS_SVC_DUAL"] = 1;
	solvers["L2R_L2LOSS_SVC"] = 2;
	solvers["L2R_L1LOSS_SVC_DUAL"] = 3;
	solvers["MCSVM_CS"] = 4;
	solvers["L1R_L2LOSS_SVC"] = 5;
	solvers["L1R_LR"] = 6;
	solvers["L2R_LR_DUAL"] = 7;
	solvers["L2R_L2LOSS_SVR = 11"] = 8;
	solvers["L2R_L2LOSS_SVR_DUAL"] = 9;
	solvers["L2R_L1LOSS_SVR_DUAL"] = 10;

	//Do not forget your variable initialisation
	RAZ();
}



//------------------------------------------------------------------------------------------------------------------
//If you want to implement an external library... Otherwise remove it...
extern "C" {
	Exporting bool liblinear_InitialisationModule(AtanorGlobal* global, string version) {
		if (version != AtanorVersion())
			return false;

		global->Update();

		return Atanorliblinear::InitialisationModule(global, version);
	}
}
//------------------------------------------------------------------------------------------------------------------

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorliblinear::AddMethod(AtanorGlobal* global, string name, liblinearMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorliblinear::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorliblinear::idtype = global->Getid("liblinear");

	Atanorliblinear::AddMethod(global, "_initial", &Atanorliblinear::MethodInitial, P_TWO | P_THREE, "load(string inputdata,smap options,string outputfilename): load your training data with option. outputfilename is optional.");

	Atanorliblinear::AddMethod(global, "load", &Atanorliblinear::MethodLoad, P_TWO | P_THREE, "load(string inputdata,smap options,string outputfilename): load your training data with option. outputfilename is optional.");
	Atanorliblinear::AddMethod(global, "train", &Atanorliblinear::MethodLoad, P_TWO | P_THREE, "train(string inputdata,smap options,string outputfilename): train your training data with option. outputfilename is optional.");
	Atanorliblinear::AddMethod(global, "savemodel", &Atanorliblinear::MethodSave, P_ONE, "savemodel(string outputfilename): save your model in a file");
	Atanorliblinear::AddMethod(global, "options", &Atanorliblinear::MethodOptions, P_ONE, "options(smap actions): Set the training options");
	Atanorliblinear::AddMethod(global, "crossvalidation", &Atanorliblinear::MethodCrossvalidation, P_NONE, "crossvalidation(): Relaunch the cross validation with new parameters");
	Atanorliblinear::AddMethod(global, "cleandata", &Atanorliblinear::MethodClean, P_NONE, "cleandata(): clean internal data");
	Atanorliblinear::AddMethod(global, "trainingset", &Atanorliblinear::MethodTrainingSet, P_TWO, "trainingset(fvector labels,vector data): create your training set out of a iftreemap vector");
	Atanorliblinear::AddMethod(global, "loadmodel", &Atanorliblinear::MethodLoadModel, P_ONE, "loadmodel(string filename): Load your model");
	Atanorliblinear::AddMethod(global, "predictfromfile", &Atanorliblinear::MethodDoPredictFromFile, P_THREE, "predictfromfile(string input,bool flag_predict_probability,int infos): Predict from a file");
	Atanorliblinear::AddMethod(global, "predict", &Atanorliblinear::MethodDoPredict, P_THREE | P_FOUR, "predict(fvector labels,vector data,bool flag_predict_probability,int infos): Predict from a vector of iftreemap. labels is optional.");

	Atanorliblinear::AddMethod(global, "stloadproblems", &Atanorliblinear::MethodLoadproblems, P_ONE, "stloadproblems(svector files): Load and merge a list of training files (libshorttext).");
	Atanorliblinear::AddMethod(global, "staddproblems", &Atanorliblinear::MethodAddproblemvectors, P_TWO, "staddproblems(fvector labels,vector data): create your training set out of a iftreemap vector (libshorttext), which is merged with previous training sets.");
	Atanorliblinear::AddMethod(global, "stcomputeidf", &Atanorliblinear::MethodComputeidf, P_ONE, "stcomputeidf(fvector idfval): Compute the idv_val vector (libshorttext).");
	Atanorliblinear::AddMethod(global, "stnormalize", &Atanorliblinear::MethodNormalize, P_FIVE, "stnormalize(fvector idfval,bool binary, bool norm, bool tf, bool idf): Normalize the idv_val vector (libshorttext).");
	Atanorliblinear::AddMethod(global, "stanalyzeproblems", &Atanorliblinear::MethodAnalysisproblems, P_ONE, "stanalyzeproblems(bool training): Analyze the training files (libshorttext).");
	Atanorliblinear::AddMethod(global, "stclean", &Atanorliblinear::MethodCleaning, P_NONE, "stclean(): Cleaning internal structures (libshorttext).");

	global->newInstance[Atanorliblinear::idtype] = new Atanorliblinear(global);
	global->RecordMethods(Atanorliblinear::idtype,Atanorliblinear::exported);
	
	return true;
}


Atanor* Atanorliblinear::Put(Atanor* idx, Atanor* kval, short idthread) {
	return aTRUE;
}

Atanor* Atanorliblinear::Get(Atanor* context, Atanor* idx, short idthread) {
	return this;
}


Atanor* Atanorliblinear::parse_options(Atanor* kmap) {
	void(*print_func)(const char*) = &print_null;	// default printing to stdout

	// default values
	if (!loaded) {
		param.solver_type = L2R_L2LOSS_SVC_DUAL;
		param.C = 1;
		param.eps = INF; // see setting below
		param.p = 0.1;
		param.nr_weight = 0;
		param.weight_label = NULL;
		param.weight = NULL;
		//param.ovo = 0;
		flag_cross_validation = 0;
		bias = -1;
	}

	string key;
	string val;
	string msg;

	AtanorIteration* itr = kmap->Newiteration(false);
	for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
		key = itr->Keystring();
		val = itr->Valuestring();
		switch (key[0])
		{
		case 's':
			if (solvers.find(val) == solvers.end())
				param.solver_type = atoi(STR(val));
			else
				param.solver_type = solvers[val];
			break;
		case 'c':
			param.C = atof(STR(val));
			break;

		case 'p':
			param.p = atof(STR(val));
			break;

		case 'e':
			param.eps = atof(STR(val));
			break;

		case 'B':
			bias = atof(STR(val));
			break;

		case 'w':
			++param.nr_weight;
			param.weight_label = (INT64 *)realloc(param.weight_label, sizeof(int)*param.nr_weight);
			param.weight = (double *)realloc(param.weight, sizeof(double)*param.nr_weight);
			msg = key.substr(1, key.size() - 1);
			param.weight_label[param.nr_weight - 1] = atoi(STR(msg));
			param.weight[param.nr_weight - 1] = atoi(STR(val));
			break;

		case 'M':
			//param.ovo = atoi(STR(val));
			break;
		case 'v':
			flag_cross_validation = 1;
			nr_fold = atoi(STR(val));
			if (nr_fold < 2)
			{
				msg = "LLN(019): n-fold cross validation: n must >= 2";
				return exit_with_help(msg);
			}
			break;
		case 'q':
			print_func = &print_null;
			break;

		default:
			return exit_with_help(key);
		}
	}

	itr->Release();

	set_print_string_function(print_func);

	if (param.eps == INF)
	{
		switch (param.solver_type)
		{
		case L2R_LR:
		case L2R_L2LOSS_SVC:
			param.eps = 0.01;
			break;
		case L2R_L2LOSS_SVR:
			param.eps = 0.001;
			break;
		case L2R_L2LOSS_SVC_DUAL:
		case L2R_L1LOSS_SVC_DUAL:
		case MCSVM_CS:
		case L2R_LR_DUAL:
			param.eps = 0.1;
			break;
		case L1R_L2LOSS_SVC:
		case L1R_LR:
			break;
		case L2R_L1LOSS_SVR_DUAL:
		case L2R_L2LOSS_SVR_DUAL:
			param.eps = 0.1;
			break;
		}
	}
	return aTRUE;
}


Atanor* Atanorliblinear::MethodComputeidf(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (svmproblems.size() == 0 || num_srcs == 0)
		return globalAtanor->Returnerror("LLN(044): No data", idthread);

	Atanor* kvf = callfunc->Evaluate(0, contextualpattern, idthread);
	if (!kvf->isVectorContainer())
		return globalAtanor->Returnerror("LLN(043): Expecting a vector container", idthread);

	if (kvf->Type() == a_fvector) {
		for (int i = 0; i < svmproblems.size(); i++)
			compute_idf(&svmproblems[i].prob, ((Atanorfvector*)kvf)->values);
	}
	else {
		kvf->Clear();
		vector<double> v;
		for (int i = 0; i < svmproblems.size(); i++)
			compute_idf(&svmproblems[i].prob, v);
		for (int i = 0; i < v.size(); i++)
			kvf->Push(globalAtanor->Providefloat(v[i]));
	}
	return aTRUE;
}

Atanor* Atanorliblinear::MethodNormalize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (svmproblems.size() || num_srcs == 0)
		return globalAtanor->Returnerror("LLN(044): No data", idthread);

	Atanor* kvf = callfunc->Evaluate(0, contextualpattern, idthread);
	if (!kvf->isVectorContainer())
		return globalAtanor->Returnerror("LLN(043): Expecting a vector container", idthread);

	vector<double> v;
	if (kvf->Type() == a_fvector)
		v = ((Atanorfvector*)kvf)->values;
	else {
		for (int i = 0; i < v.size(); i++)
			v.push_back(kvf->getvalue(i)->Float());
	}

	int binary = callfunc->Evaluate(1, contextualpattern, idthread)->Boolean();
	int norm = callfunc->Evaluate(2, contextualpattern, idthread)->Boolean();
	int tf = callfunc->Evaluate(3, contextualpattern, idthread)->Boolean();
	int idf = callfunc->Evaluate(4, contextualpattern, idthread)->Boolean();

	for (int i = 0; i < svmproblems.size(); i++)
		normalize(&svmproblems[i].prob, binary, norm, tf, idf, v);

	if (kvf->Type() == a_fvector)
		((Atanorfvector*)kvf)->values = v;
	else {
		kvf->Clear();
		for (int i = 0; i < v.size(); i++)
			kvf->Push(globalAtanor->Providefloat(v[i]));
	}

	return aTRUE;
}

Atanor* Atanorliblinear::MethodAddproblemvectors(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* klabels = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* kdata = callfunc->Evaluate(1, contextualpattern, idthread);
	if (klabels->Type() != a_fvector || kdata->Type() != a_vector)
		return globalAtanor->Returnerror("LLN(007): Wrong type for labels or data", idthread);

	if (klabels->Size() != kdata->Size())
		return globalAtanor->Returnerror("LLN(016): The two containers should have the same size", idthread);
	int err = load_vector_problems((Atanorfvector*)klabels, (Atanorvector*)kdata);
	if (err) {
		string message(errormessage);
		return globalAtanor->Returnerror(message, idthread);
	}
	return aTRUE;
}

Atanor* Atanorliblinear::MethodLoadproblems(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* kvect = callfunc->Evaluate(0, contextualpattern, idthread);
	vector<string> sources;
	if (!kvect->isVectorContainer())
		return globalAtanor->Returnerror("LLN(047): Wrong type: expecting a vector", idthread);
	if (kvect->Type() == a_svector)
		sources = ((Atanorsvector*)kvect)->values;
	else {
		for (int i = 0; i < kvect->Size(); i++)
			sources.push_back(kvect->getvalue(i)->String());
	}
	int err = load_merge_problems(sources);
	if (err) {
		string msg = "LLN(049): ";
		msg += errormessage;
		return globalAtanor->Returnerror(msg, idthread);
	}
	return aTRUE;
}

Atanor* Atanorliblinear::MethodAnalysisproblems(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (svmproblems.size() || num_srcs == 0)
		return globalAtanor->Returnerror("LLN(044): No data", idthread);

	bool training = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
	INT64 offsets[1000];
	Atanorvector* kvect = (Atanorvector*)Selectavector(contextualpattern);
	analysis_problems(offsets, kvect, training);
	return aTRUE;
}

Atanor* Atanorliblinear::MethodCleaning(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (svmproblems.size() || num_srcs == 0)
		return globalAtanor->Returnerror("LLN(044): No data", idthread);

	for (int i = 0; i < num_srcs; i++)
		freeSVMProblem(svmproblems[i]);
	svmproblems.clear();
	num_srcs = 0;
	return aTRUE;
}

Atanor* Atanorliblinear::MethodCrossvalidation(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (!loaded)
		return globalAtanor->Returnerror("LLN(008): Load your training data first", idthread);
	return do_cross_validation();
}

Atanor* Atanorliblinear::MethodLoadModel(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (loaded)
		return globalAtanor->Returnerror("LLN(011): Clean your data first", idthread);

	flag_predict_probability = 0;
	model_file_name = callfunc->Evaluate(0, contextualpattern, idthread)->String();

	if ((model_ = load_model(STR(model_file_name))) == 0)
		return globalAtanor->Returnerror("LLN(012): can't open model file", idthread);
	loaded = true;
	return aTRUE;
}

Atanor* Atanorliblinear::MethodLoad(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	infilename = callfunc->Evaluate(0, contextualpattern, idthread)->String();

	Atanor* koptions = callfunc->Evaluate(1, contextualpattern, idthread);
	if (koptions->isContainer() == false)
		return globalAtanor->Returnerror("LLN(003): Options should be a map", idthread);

	model_file_name = "";
	if (callfunc->Size() == 3)
		model_file_name = callfunc->Evaluate(2, contextualpattern, idthread)->String();

	if (parse_options(koptions) == aRAISEERROR)
		return aRAISEERROR;

	//Loading the data
	if (read_problem(STR(infilename)) == aRAISEERROR)
		return aRAISEERROR;

	const char *error_msg = check_parameter(&prob, &param);

	string msgerr;
	if (error_msg)
	{
		msgerr = "LLN(005): ERROR: ";
		msgerr += error_msg;
		return globalAtanor->Returnerror(msgerr, idthread);
	}

	Atanor* retour = aTRUE;
	model_ = train(&prob, &param);
	if (model_file_name != "") {
		if (save_model(STR(model_file_name), model_)) {
			msgerr = "LLN(004): can't save model to file ";
			msgerr += model_file_name;
			return globalAtanor->Returnerror(msgerr, idthread);
		}
	}

	loaded = true;
	return aTRUE;
}

Atanor* Atanorliblinear::MethodSave(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (model_ == NULL || !loaded)
		return globalAtanor->Returnerror("LLN(021): No model to save", idthread);

	model_file_name = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	if (save_model(STR(model_file_name), model_)) {
		string msgerr = "LLN(004): can't save model to file ";
		msgerr += model_file_name;
		return globalAtanor->Returnerror(msgerr, idthread);
	}

	return aTRUE;
}

Atanor* Atanorliblinear::MethodClean(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (!loaded)
		return aFALSE;
	CLEAN();
	RAZ();
	return aTRUE;
}

Atanor* Atanorliblinear::MethodOptions(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* koptions = callfunc->Evaluate(0, contextualpattern, idthread);
	if (koptions->isContainer() == false)
		return globalAtanor->Returnerror("LLN(003): Options should be a map", idthread);
	if (parse_options(koptions) == aRAISEERROR)
		return aRAISEERROR;
	return aTRUE;
}

Atanor* Atanorliblinear::MethodTrainingSet(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (loaded)
		return globalAtanor->Returnerror("LLN(011): Clean your data first", idthread);
	Atanor* klabels = callfunc->Evaluate(0, contextualpattern, idthread);
	Atanor* kdata = callfunc->Evaluate(1, contextualpattern, idthread);

	if (klabels->Type() != a_fvector || kdata->Type() != a_vector)
		return globalAtanor->Returnerror("LLN(007): Wrong type for labels or data", idthread);


	if (klabels->Size() != kdata->Size())
		return globalAtanor->Returnerror("LLN(016): The two containers should have the same size", idthread);

	return load_problem((Atanorfvector*)klabels, (Atanorvector*)kdata);
}

Atanor* Atanorliblinear::MethodDoPredictFromFile(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (!loaded)
		return globalAtanor->Returnerror("LLN(018): Load your model first", idthread);

	string inputfile = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	{
		ifstream fl(STR(inputfile), openMode);
		if (fl.fail())
			return globalAtanor->Returnerror("LLN(014): Can't open input file", idthread);
	}

	FILE* input;
#ifdef WIN32
	fopen_s(&input, STR(inputfile), "r");
#else
	input = fopen(STR(inputfile), "r");
#endif

	if (input == NULL)
		return globalAtanor->Returnerror("LLN(014): Can't open input file", idthread);

	flag_predict_probability = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();

	x = (struct feature_node *) malloc(max_nr_attr*sizeof(struct feature_node));
	infos = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();

	Atanorvector* output = (Atanorvector*)Selectavector(contextualpattern);
	do_predict_with_file(input, output);
	fclose(input);
	free(x);
	x = NULL;
	return output;
}

Atanor* Atanorliblinear::MethodDoPredict(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (!loaded)
		return globalAtanor->Returnerror("LLN(018): Load your model first", idthread);

	Atanor* klabels = NULL;
	Atanor* kdata = NULL;

	if (callfunc->Size() == 4) {
		klabels = callfunc->Evaluate(0, contextualpattern, idthread);
		kdata = callfunc->Evaluate(1, contextualpattern, idthread);
		flag_predict_probability = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
		infos = callfunc->Evaluate(3, contextualpattern, idthread)->Integer();
		if (klabels->Type() != a_fvector || kdata->Type() != a_vector)
			return globalAtanor->Returnerror("LLN(007): Wrong type for labels or data", idthread);
		if (klabels->Size() != kdata->Size())
			return globalAtanor->Returnerror("LLN(016): The two containers should have the same size", idthread);
	}
	else {
		kdata = callfunc->Evaluate(0, contextualpattern, idthread);
		flag_predict_probability = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
		infos = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
		if (kdata->Type() != a_vector)
			return globalAtanor->Returnerror("LLN(007): Wrong type for labels or data", idthread);
	}

	Atanorvector* output = (Atanorvector*)Selectavector(contextualpattern);

	x = (struct feature_node *) malloc(max_nr_attr*sizeof(struct feature_node));
	do_predict((Atanorfvector*)klabels, (Atanorvector*)kdata, output);
	free(x);
	x = NULL;
	return output;
}

