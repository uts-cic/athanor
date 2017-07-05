
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
filename   : atanorxmldoc.cxx
Date       : 2017/04/01
Purpose    : xml document management (based on libxml2)
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorxmldoc.h"
#include "atanorvector.h"
#include "atanorxml.h"
#include "instructions.h"

ThreadLock _xmlDocLock(NULL, false);

static bool atanxmlStop = false;

void startElementNs(void *ctx,
	const xmlChar *localname,
	const xmlChar *prefix,
	const xmlChar *URI,
	int nb_namespaces,
	const xmlChar **namespaces,
	int nb_attributes,
	int nb_defaulted,
	const xmlChar **attributes) {

	if (atanxmlStop)
		return;

	Atanorxml* knode;
	Atanor* func;
	Atanorxmldoc* xmldoc;
	{
		Locking _lock(_xmlDocLock);
		xmlSAX2StartElementNs(ctx, localname, prefix, URI, nb_namespaces, namespaces, nb_attributes, nb_defaulted, attributes);
		xmlParserCtxtPtr x = (xmlParserCtxtPtr)ctx;
		xmldoc = (Atanorxmldoc*)x->_private;

		if (xmldoc->stop || globalAtanor->Error(0)) {
			xmldoc->stop = true;
			return;
		}
		func = xmldoc->function;
		knode = new Atanorxml(globalAtanor, x->node);
		if (x->node != NULL && x->node->_private == NULL)
			x->node->_private = (void*)xmldoc->idnode++;
	}

	if (func != NULL) {
		AtanorCallFunction kcf(func);
		knode->Setreference();
		kcf.arguments.push_back(knode);
		kcf.arguments.push_back(xmldoc->object);
		kcf.Get(aNULL, aNULL, 0);
		knode->Resetreference();
	}
}

void endElementNs(void *ctx,
	const xmlChar * name,
	const xmlChar * prefix,
	const xmlChar * URI) {


	if (atanxmlStop)
		return;

	Atanorxml* knode;
	Atanor* func;
	Atanorxmldoc* xmldoc;

	{
		Locking _lock(_xmlDocLock);
		xmlParserCtxtPtr x = (xmlParserCtxtPtr)ctx;
		xmlSAX2EndElementNs(ctx, name, prefix, URI);
		xmldoc = (Atanorxmldoc*)x->_private;

		if (xmldoc->stop || globalAtanor->Error(0)) {
			xmldoc->stop = true;
			return;
		}

		func = xmldoc->functionend;
		knode = new Atanorxml(globalAtanor, x->node);
		if (x->node != NULL && x->node->_private == NULL)
			x->node->_private = (void*)xmldoc->idnode++;
	}

	AtanorCallFunction kcf(func);
	knode->Setreference();
	kcf.arguments.push_back(knode);
	kcf.arguments.push_back(xmldoc->objectend);
	kcf.Get(aNULL, aNULL, 0);
	knode->Resetreference();
}

extern "C" {
	int xmlXPathRegisterNs(xmlXPathContextPtr ctxt, const xmlChar *prefix, const xmlChar *ns_uri);
}

static void XmlCleaningDoc(xmlDocPtr dx) {
	Locking _lock(globalAtanor);

	Atanor* a;
	for (int i = 0; i < globalAtanor->Trackedsize(); i++) {
		a = globalAtanor->Gettracked(i);
		if (a != NULL && a->Type() == Atanorxml::idtype) {
			Atanorxml* kxml = (Atanorxml*)a;
			if (kxml->node == NULL)
				continue;

			if (kxml->node->doc == dx)
				kxml->node = NULL;
		}
	}
}

//We need to declare once again our local definitions.
hmap<short, xmldocMethod>  Atanorxmldoc::methods;
hmap<string, string> Atanorxmldoc::infomethods;
bin_hash<unsigned long> Atanorxmldoc::exported;

short Atanorxmldoc::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorxmldoc::AddMethod(AtanorGlobal* global, string name, xmldocMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorxmldoc::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorxmldoc::idtype = global->Getid("xmldoc");

	Atanorxmldoc::AddMethod(global, "_initial", &Atanorxmldoc::MethodInitial, P_ONE, "_initial(variable): Interception of a callback object");

	Atanorxmldoc::AddMethod(global, "load", &Atanorxmldoc::MethodLoadXML, P_ONE, "load(string filename): load an xml file");
	Atanorxmldoc::AddMethod(global, "onclosing", &Atanorxmldoc::MethodOnClosing, P_TWO, "onclosing(functionobject): function to call on closing markup tag");
	Atanorxmldoc::AddMethod(global, "parse", &Atanorxmldoc::MethodLoadXMLBuffer, P_ONE, "parse(string buffer): parse an xml buffer");
	Atanorxmldoc::AddMethod(global, "create", &Atanorxmldoc::MethodCreate, P_ONE, "create(string topnode): create an XML file with topnode as the top node or with topnode if topdenode is a full XML structure");
	Atanorxmldoc::AddMethod(global, "xpath", &Atanorxmldoc::MethodXPath, P_ONE, "xpath(string myxpath): Return a vector of xml nodes matching myxpath");
	Atanorxmldoc::AddMethod(global, "save", &Atanorxmldoc::MethodSave, P_ONE | P_TWO, "save(string filename, string encoding): save an XML file. Encoding default is 'utf8'");
	Atanorxmldoc::AddMethod(global, "close", &Atanorxmldoc::MethodClose, P_NONE, "close(): close an xml file");
	Atanorxmldoc::AddMethod(global, "node", &Atanorxmldoc::MethodNode, P_NONE, "node(): return the top node of the file");
	Atanorxmldoc::AddMethod(global, "xmlstring", &Atanorxmldoc::MethodxmlEncoding, P_NONE, "xmlstring(): Return an XML document as a string");
	Atanorxmldoc::AddMethod(global, "serialize", &Atanorxmldoc::MethodSerialize, P_ONE, "serialize(object): Serialize as an XML document any KiF object");
	Atanorxmldoc::AddMethod(global, "serializestring", &Atanorxmldoc::MethodSerializeString, P_ONE, "serialize, string(object): Serialize as an XML document any KiF object and return the corresponding string");


	global->newInstance[Atanorxmldoc::idtype] = new Atanorxmldoc(global, NULL);
	global->RecordMethods(Atanorxmldoc::idtype, Atanorxmldoc::exported);

	return true;
}



Atanor* Atanorxmldoc::Put(Atanor* index, Atanor* kval, short idthread) {
	if (doc != NULL) {
		Locking _lock(_xmlDocLock);
		XmlCleaningDoc(doc);
		xmlFreeDoc(doc);
		doc = NULL;
	}
	if (sax != NULL)
		xmlFree(sax);
	sax = NULL;
	filename = "";

	string topnode = kval->String();
	stop = false;

	if (topnode.size() > 4 && topnode[0] == '<' && topnode[topnode.size() - 1] == '>')
		doc = xmlSAXParseMemory(NULL, STR(topnode), topnode.size(), 0);

	return aTRUE;
}

Atanor* Atanorxmldoc::Get(Atanor* context, Atanor* index, short idthread) {
	return this;
}


Atanor* Atanorxmldoc::MethodOnClosing(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	functionend = callfunc->Evaluate(0, aNULL, idthread);
	if (!functionend->isFunction())
		return globalAtanor->Returnerror("XML(211): Expecting a function", idthread);
	objectend = callfunc->arguments[1];
	if (!objectend->isCallVariable())
		objectend = objectend->Get(aNULL, aNULL, idthread);
	return aTRUE;
}

Atanor* Atanorxmldoc::MethodCreate(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (doc != NULL)
		return globalAtanor->Returnerror("XML(234): Already created", idthread);
	string topnode = callfunc->Evaluate(0, aNULL, idthread)->String();
	stop = false;
	if (topnode.size() > 4 && topnode[0] == '<' && topnode[topnode.size() - 1] == '>')
		doc = xmlSAXParseMemory(NULL, STR(topnode), topnode.size(), 0);
	else {
		string buf = "<";
		buf += topnode;
		buf += "/>";
		doc = xmlSAXParseMemory(NULL, STR(buf), buf.size(), 0);
	}
	return aTRUE;
}

Atanor* Atanorxmldoc::MethodXPath(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (doc == NULL)
		return globalAtanor->Returnerror("XML(235): Empty doc", idthread);
	xmlNodePtr nn;
	if (xpathcontext == NULL) {
		xpathcontext = xmlXPathNewContext(doc);
		nn = doc->children;
		xmlNsPtr cur = NULL;
		while (nn != NULL && nn->type != XML_ELEMENT_NODE)
			nn = nn->next;
		if (nn != NULL)
			cur = nn->nsDef;
		//pour le contexte xpath a partir du namespace du document
		while (cur != NULL) {
			xmlXPathRegisterNs(xpathcontext, cur->prefix, cur->href);
			cur = cur->next;
		}
	}
	xmlXPathObjectPtr nodeList;
	string theXPath = callfunc->Evaluate(0, aNULL, idthread)->String();
	nodeList = xmlXPathEval((xmlChar*)theXPath.c_str(), xpathcontext);
	Atanor* kvect = Selectavector(contextualpattern);
	if (nodeList != NULL) {
		int nbelements = 0;
		if (nodeList->nodesetval != NULL)
			nbelements = nodeList->nodesetval->nodeNr;
		Atanorxml* xml;
		//On jette un coup d'oeil sur la structure construite en memoire
		for (int j = 0; j < nbelements; j++) {
			nn = nodeList->nodesetval->nodeTab[j];
			xml = new Atanorxml(globalAtanor, nn);
			kvect->Push(xml);
		}
		xmlXPathFreeObject(nodeList);
	}
	return kvect;
}

Atanor* Atanorxmldoc::MethodSave(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (doc == NULL)
		return globalAtanor->Returnerror("XML(235): Empty doc", idthread);
	string encoding("UTF-8");
	if (callfunc->Size() == 2)
		encoding = callfunc->Evaluate(1, aNULL, idthread)->String();
	string filename = callfunc->Evaluate(0, aNULL, idthread)->String();
	xmlKeepBlanksDefault(0);
	xmlSaveFormatFileEnc(STR(filename), doc, STR(encoding), 1);
	return aTRUE;
}

Atanor* Atanorxmldoc::MethodClose(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (doc != NULL) {
		Locking _lock(_xmlDocLock);
		XmlCleaningDoc(doc);
		xmlFreeDoc(doc);
		doc = NULL;
	}
	if (sax != NULL)
		xmlFree(sax);
	object->Resetreference();
	sax = NULL;
	filename = "";
	return aTRUE;

}

Atanor* Atanorxmldoc::MethodNode(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (doc != NULL)
		return new Atanorxml(globalAtanor, doc->children);

	return aNULL;
}

Atanor* Atanorxmldoc::MethodxmlEncoding(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (doc == NULL)
		return globalAtanor->Returnerror("XML(235): Empty doc", idthread);
	xmlChar* buff = NULL;
	int longueur;
	xmlDocDumpMemoryEnc(doc, &buff, &longueur, "UTF-8");
	Atanor* kstr = globalAtanor->Providestring((char*)buff);
	xmlFree(buff);
	return kstr;
}




Atanor* Atanorxmldoc::MethodLoadXML(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (doc != NULL)
		return globalAtanor->Returnerror("XML(234): Already created", idthread);
	stop = false;
	atanxmlStop = false;
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	Atanor* kname = callfunc->Evaluate(0, aNULL, idthread);
	filename = kname->String();
	strcpy_s(fname, 1024, STR(filename));
	if (function != NULL || functionend != NULL) {
		sax = (xmlSAXHandler *)xmlMalloc(sizeof(xmlSAXHandler));
		sax->initialized = 0;
		//initxmlDefaultSAXHandler(sax, xmlGetWarningsDefaultValue);       
		xmlSAX2InitDefaultSAXHandler(sax, xmlGetWarningsDefaultValue);
		sax->initialized = XML_SAX2_MAGIC;
		//you may return any value of course...
		if (function != NULL)
			sax->startElementNs = startElementNs;
		
		if (functionend != NULL)
			sax->endElementNs = endElementNs;
		if (atanxmlStop)
			return aNULL;
		//On conserve un pointeur sur l'objet GlobalParseur
		//Puis on analyse le document
		doc = xmlSAXParseFileWithData(sax, fname, 1, this);
	}
	else {
		sax = NULL;
		if (atanxmlStop)
			return aNULL;
		doc = xmlParseFile(fname);
	}
	if (doc == NULL)
		return globalAtanor->Returnerror("XML(102): Unknown file", idthread);
	return aTRUE;
}

xmlNodePtr Atanorxmldoc::buildnode(xmlNodePtr r, Atanor* e) {
	xmlNodePtr sub;
	xmlChar* name;
	if (e->isVectorContainer()) {
		name = xmlCharStrdup("vector");
		sub = xmlNewNodeEatName(NULL, name);
		xmlAddChild(r, sub);
		AtanorIteration* itr = e->Newiteration(false);
		for (itr->Begin(); itr->End() == aFALSE; itr->Next())
			buildnode(sub, itr->IteratorValue());
		itr->Release();
		return sub;
	}
	if (e->isMapContainer()) {
		xmlNodePtr n;
		name = xmlCharStrdup("map");
		sub = xmlNewNodeEatName(NULL, name);
		xmlAddChild(r, sub);
		AtanorIteration* itr = e->Newiteration(false);
		string k;
		xmlChar ky[] = { 'k', 'e', 'y', 0 };
		for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
			k = itr->Keystring();
			n = buildnode(sub, itr->IteratorValue());
			xmlSetProp(n, ky, (xmlChar*)k.c_str());
		}
		itr->Release();
		return sub;
	}

	string stype = globalAtanor->Getsymbol(e->Type());
	string v = e->String();
	name = xmlCharStrdup(stype.c_str());
	sub = xmlNewNodeEatName(NULL, name);
	xmlAddChild(r, sub);
	xmlNodePtr txt = xmlNewDocText(doc, (xmlChar*)v.c_str());
	xmlAddChild(sub, txt);
	return sub;
}


Atanor* Atanorxmldoc::MethodSerialize(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//first we clean everything, to replace it with the new structure...
	if (doc != NULL) {
		Locking _lock(_xmlDocLock);

		XmlCleaningDoc(doc);
		xmlFreeDoc(doc);
		doc = NULL;
	}
	if (sax != NULL)
		xmlFree(sax);
	sax = NULL;
	filename = "";
	string buf;
	Atanor* atanobject = callfunc->arguments[0]; //would match i as in the example above: mynewtype toto(i,j);
	if (atanobject->isCallVariable()) {
		string name = globalAtanor->Getsymbol(atanobject->Name());
		buf = "<atan name=\"" + name + "\"/>";
	}
	else {
		atanobject = atanobject->Get(aNULL, aNULL, idthread);
		buf = "<atan/>";
	}
	stop = false;
	doc = xmlSAXParseMemory(NULL, STR(buf), buf.size(), 0);
	buildnode(doc->children, atanobject->Get(aNULL, aNULL, idthread));
	return aTRUE;
}

Atanor* Atanorxmldoc::MethodSerializeString(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//first we clean everything, to replace it with the new structure...
	if (doc != NULL) {
		Locking _lock(_xmlDocLock);
		XmlCleaningDoc(doc);
		xmlFreeDoc(doc);
		doc = NULL;
	}
	if (sax != NULL)
		xmlFree(sax);
	sax = NULL;
	filename = "";

	Atanor* atanobject = callfunc->arguments[0]; //would match i as in the example above: mynewtype toto(i,j);
	if (!atanobject->isCallVariable())
		return globalAtanor->Returnerror("Expecting a variable", idthread);
	string name = globalAtanor->Getsymbol(atanobject->Name());
	string buf = "<atan name=\"" + name + "\"/>";

	stop = false;
	doc = xmlSAXParseMemory(NULL, STR(buf), buf.size(), 0);
	buildnode(doc->children, atanobject->Get(aNULL, aNULL, idthread));
	xmlChar* buff = NULL;
	int longueur;
	xmlDocDumpMemoryEnc(doc, &buff, &longueur, "UTF-8");
	atanobject = globalAtanor->Providestring((char*)buff);
	xmlFree(buff);
	if (doc != NULL) {
		Locking _lock(_xmlDocLock);
		XmlCleaningDoc(doc);
		xmlFreeDoc(doc);
		doc = NULL;
	}
	if (sax != NULL)
		xmlFree(sax);
	sax = NULL;
	return atanobject;
}

Atanor* Atanorxmldoc::MethodLoadXMLBuffer(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (doc != NULL)
		return globalAtanor->Returnerror("XML(234): Already created", idthread);
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	stop = false;
	atanxmlStop = false;
	Atanor* kbuffer = callfunc->Evaluate(0, aNULL, idthread);
	string buff = kbuffer->String();
	if (function != NULL || functionend != NULL) {
		sax = (xmlSAXHandler *)xmlMalloc(sizeof(xmlSAXHandler));
		sax->initialized = 0;
		//initxmlDefaultSAXHandler(sax, xmlGetWarningsDefaultValue);       
		xmlSAX2InitDefaultSAXHandler(sax, xmlGetWarningsDefaultValue);
		//you may return any value of course...
		if (function != NULL) {
			sax->startElementNs = startElementNs;
		}
		if (functionend != NULL) {
			sax->endElementNs = endElementNs;
		}
		if (atanxmlStop)
			return aNULL;
		//On conserve un pointeur sur l'objet GlobalParseur
		//Puis on analyse le document
		doc = xmlSAXParseMemoryWithData(sax, STR(buff), buff.size(), 1, this);
	}
	else {
		sax = NULL;
		if (atanxmlStop)
			return aNULL;
		doc = xmlSAXParseMemory(NULL, STR(buff), buff.size(), 1);
	}
	if (doc == NULL)
		return globalAtanor->Returnerror("XML(106): Not an XML buffer", idthread);
	return aTRUE;
}





