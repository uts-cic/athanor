
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
filename   : atanorxml.cxx
Date       : 2017/04/01
Purpose    : xml nodes management (based on libxml2)
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorxml.h"
#include "atoms.h"
#include "atanorprimemapss.h"
#include "atanorconstants.h"
#include "atanorxmldoc.h"

void XmlCleaningNode(xmlNodePtr x) {
	Locking _lockg(globalAtanor);

	Atanor* a;
	for (int i = 0; i < globalAtanor->Trackedsize(); i++) {
		a = globalAtanor->Gettracked(i);
		if (a != NULL && a->Type() == Atanorxml::idtype) {
			Atanorxml* kxml = (Atanorxml*)a;
			if (kxml->node == NULL)
				continue;

			if (kxml->node == x)
				kxml->node = NULL;
		}
	}
}

void xmlnodeclear(xmlNodePtr n) {
	n->doc = NULL;
	n->ns = NULL;
	if (n->children != NULL)
		xmlnodeclear(n->children);
	if (n->next != NULL)
		xmlnodeclear(n->next);
}


//We need to declare once again our local definitions.
hmap<short, xmlMethod>  Atanorxml::methods;
hmap<string, string> Atanorxml::infomethods;
bin_hash<unsigned long> Atanorxml::exported;

short Atanorxml::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorxml::AddMethod(AtanorGlobal* global, string name, xmlMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Atanorxml::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Atanorxml::idtype = global->Getid("xml");

	Atanorxml::AddMethod(global, "_initial", &Atanorxml::MethodNew, P_ONE, "_initial(string name): Create a new XML node");
	Atanorxml::AddMethod(global, "root", &Atanorxml::MethodRoot, P_NONE, "root(): return the root node");
	Atanorxml::AddMethod(global, "document", &Atanorxml::MethodDocument, P_NONE, "document(): return the xmldoc associated with this node");
	Atanorxml::AddMethod(global, "next", &Atanorxml::MethodNext, P_NONE | P_ONE, "next(): next xml node");
	Atanorxml::AddMethod(global, "unlink", &Atanorxml::MethodUnlink, P_NONE, "unlink(): remove a node from a tree structure");
	Atanorxml::AddMethod(global, "delete", &Atanorxml::MethodDelete, P_NONE, "delete(): delete the internal representation of the XML node");
	Atanorxml::AddMethod(global, "previous", &Atanorxml::MethodPrevious, P_NONE | P_ONE, "previous(): previous xml node");
	Atanorxml::AddMethod(global, "parent", &Atanorxml::MethodParent, P_NONE, "parent(): parent xml node");
	Atanorxml::AddMethod(global, "child", &Atanorxml::MethodChild, P_NONE | P_ONE, "child(): child xml node");
	Atanorxml::AddMethod(global, "name", &Atanorxml::MethodName, P_NONE | P_ONE, "name(): name node");
	Atanorxml::AddMethod(global, "line", &Atanorxml::MethodLine, P_NONE, "line(): return the line position of the node");
	Atanorxml::AddMethod(global, "id", &Atanorxml::MethodId, P_NONE, "id(): return the node id (only available with callback functions)");
	Atanorxml::AddMethod(global, "xmltype", &Atanorxml::MethodxmlType, P_NONE, "xmltype(): return the XML node type");
	Atanorxml::AddMethod(global, "properties", &Atanorxml::MethodProperties, P_NONE | P_ONE, "properties(): return the xml node properties");
	Atanorxml::AddMethod(global, "content", &Atanorxml::MethodContent, P_NONE | P_ONE, "content(): return the text content of an xml node");
	Atanorxml::AddMethod(global, "namespace", &Atanorxml::MethodNamespace, P_NONE, "namespace(): return the namespace vector of an xml node");
	Atanorxml::AddMethod(global, "new", &Atanorxml::MethodNew, P_ONE, "new(string name): Create a new XML node");
	Atanorxml::AddMethod(global, "xmlstring", &Atanorxml::Methodxmlstring, P_NONE, "xmlstring(): Return the whole tree from the current XML node as a string");



	global->newInstance[Atanorxml::idtype] = new Atanorxml(global, NULL);
	global->RecordMethods(Atanorxml::idtype,Atanorxml::exported);

	return true;
}



Atanor* Atanorxml::Put(Atanor* idx, Atanor* kval, short idthread) {
	if (kval == aNULL || kval == aNOELEMENT) {
		node = NULL;
		return aTRUE;
	}

	if (kval->Type() != idtype) {
		if (node != NULL && idx != NULL && !idx->isConst()) {
			string att = idx->String();
			string val = kval->String();
			xmlSetProp(node, USTR(att), USTR(val));
		}
		return aTRUE;
	}

	Atanorxml* xml = (Atanorxml*)kval;
	node = xml->node;
	return aTRUE;
}

Atanor* Atanorxml::Get(Atanor* context, Atanor* idx, short idthread) {
	if (node == NULL)
		return aNOELEMENT;
	if (idx != NULL && !idx->isConst()) {
		string att = idx->String();
		xmlAttrPtr propriete;
		if (node->properties == NULL)
			return aNOELEMENT;
		propriete = node->properties;
		string p;
		while (propriete != NULL) {
			p = (char*)propriete->name;
			if (att == p)
				return globalAtanor->Providestring((char*)propriete->children->content);
			propriete = propriete->next;
		}
		return aNOELEMENT;
	}
	return this;

}



Atanor* Atanorxml::MethodRoot(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (node == NULL)
		return globalAtanor->Returnerror("XML(097): Empty node", idthread);
	return new Atanorxml(globalAtanor, node->doc->children);
}

Atanor* Atanorxml::MethodNext(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (node == NULL)
		return globalAtanor->Returnerror("XML(097): Empty node", idthread);
	if (callfunc->Size() == 0) {
		if (node->next == NULL)
			return aNULL;
		return new Atanorxml(globalAtanor, node->next);
	}
	Atanor* knode = callfunc->Evaluate(0, aNULL, idthread);
	if (knode->Type() != idtype)
		return globalAtanor->Returnerror("XML(098): Expecting an XML node", idthread);
	Atanorxml* xml = (Atanorxml*)knode;
	if (xml->node == NULL)
		return globalAtanor->Returnerror("XML(097): Empty node", idthread);
	xmlUnlinkNode(xml->node);
	xmlnodeclear(xml->node);
	xmlAddNextSibling(node, xml->node);
	return aTRUE;
}

Atanor* Atanorxml::MethodUnlink(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (node == NULL)
		return globalAtanor->Returnerror("XML(097): Empty node", idthread);
	xmlUnlinkNode(node);
	xmlnodeclear(node);
	return aTRUE;
}

Atanor* Atanorxml::MethodDelete(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (node == NULL)
		return globalAtanor->Returnerror("XML(097): Empty node", idthread);
	xmlUnlinkNode(node);
	xmlFreeNode(node);
	node = NULL;
	return aTRUE;
}

Atanor* Atanorxml::MethodPrevious(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (node == NULL)
		return globalAtanor->Returnerror("XML(097): Empty node", idthread);
	if (callfunc->Size() == 0) {
		if (node->prev == NULL)
			return aNULL;
		return new Atanorxml(globalAtanor, node->prev);
	}
	Atanor* knode = callfunc->Evaluate(0, aNULL, idthread);
	if (knode->Type() != idtype)
		return globalAtanor->Returnerror("XML(098): Expecting an XML node", idthread);
	Atanorxml* xml = (Atanorxml*)knode;
	if (xml->node == NULL)
		return globalAtanor->Returnerror("XML(097): Empty node", idthread);
	xmlUnlinkNode(xml->node);
	xmlnodeclear(xml->node);
	xmlAddPrevSibling(node, xml->node);
	return aTRUE;
}

Atanor* Atanorxml::MethodParent(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (node == NULL)
		return globalAtanor->Returnerror("XML(097): Empty node", idthread);
	if (node->parent == NULL || node->parent->type == XML_DOCUMENT_NODE || node->parent->name == NULL)
		return aNULL;
	return new Atanorxml(globalAtanor, node->parent);
}

Atanor* Atanorxml::MethodChild(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (node == NULL)
		return globalAtanor->Returnerror("XML(097): Empty node", idthread);
	if (callfunc->Size() == 0) {
		if (node->children == NULL)
			return aNULL;
		return new Atanorxml(globalAtanor, node->children);
	}
	Atanor* knode = callfunc->Evaluate(0, aNULL, idthread);
	if (knode->Type() != idtype)
		return globalAtanor->Returnerror("XML(098): Expecting an XML node", idthread);
	Atanorxml* xml = (Atanorxml*)knode;
	if (xml->node == NULL)
		return globalAtanor->Returnerror("XML(097): Empty node", idthread);
	xmlUnlinkNode(xml->node);
	xmlnodeclear(xml->node);
	xmlAddChild(node, xml->node);
	return aTRUE;
}

Atanor* Atanorxml::MethodName(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (node == NULL)
		return globalAtanor->Returnerror("XML(097): Empty node", idthread);
	if (callfunc->Size() == 0) {
		if (node->name == NULL)
			return aNULL;
		return globalAtanor->Providestring((char*)node->name);
	}
	string name = callfunc->Evaluate(0, aNULL, idthread)->String();
	xmlFree((xmlChar*)node->name);
	node->name = xmlCharStrdup((const char*)USTR(name));
	return aTRUE;
}

Atanor* Atanorxml::MethodLine(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (node == NULL)
		return globalAtanor->Returnerror("XML(097): Empty node", idthread);
	return globalAtanor->Provideint(node->line);
}

Atanor* Atanorxml::MethodId(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (node == NULL)
		return globalAtanor->Returnerror("XML(097): Empty node", idthread);
	return globalAtanor->Provideint((long)node->_private);
}

Atanor* Atanorxml::MethodxmlType(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (node == NULL)
		return globalAtanor->Returnerror("XML(097): Empty node", idthread);
	return globalAtanor->Provideint(node->type);
}

Atanor* Atanorxml::MethodProperties(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (node == NULL)
		return globalAtanor->Returnerror("XML(097): Empty node", idthread);
	if (callfunc->Size() == 0) {
		Atanorprimemapss* kmap = (Atanorprimemapss*)Selectaprimemapss(contextualpattern);
		xmlAttrPtr propriete;
		if (node->properties != NULL) {
			propriete = node->properties;
			while (propriete != NULL) {
				kmap->values[(char*)propriete->name] = (char*)propriete->children->content;
				propriete = propriete->next;
			}
		}
		return kmap;
	}
	Atanor* km = callfunc->Evaluate(0, aNULL, idthread);
	if (!km->isMapContainer())
		return globalAtanor->Returnerror("XML(123): Expecting a map", idthread);
	xmlAttrPtr propriete;
	xmlAttrPtr base;
	//Cleaning existing properties
	if (node->properties != NULL) {
		propriete = node->properties;
		while (propriete != NULL) {
			base = propriete;
			propriete = propriete->next;
			xmlFreeProp(base);
		}
		node->properties = NULL;
	}
	string v;
	string k;
	AtanorIteration* itr = km->Newiteration(false);
	for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
		k = itr->Keystring();
		v = itr->Valuestring();
		xmlSetProp(node, USTR(k), USTR(v));
	}
	itr->Release();
	return aTRUE;
}

Atanor* Atanorxml::MethodContent(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (node == NULL)
		return globalAtanor->Returnerror("XML(097): Empty node", idthread);
	string content;
	if (callfunc->Size() == 0) {
		TraverseXML(node, content, true);
		return globalAtanor->Providestring(content);
	}
	content = callfunc->Evaluate(0, aNULL, idthread)->String();
	if (node->content == NULL) {
		xmlNodePtr nouveau = xmlNewDocText(node->doc, (xmlChar*)content.c_str());
		xmlAddChild(node, nouveau);
		return aTRUE;
	}
	xmlFree((xmlChar*)node->content);
	node->content = xmlStrdup((xmlChar*)content.c_str());
	return aTRUE;
}

Atanor* Atanorxml::MethodNamespace(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	Atanor* kvect = Selectasvector(contextualpattern);
	if (node != NULL && node->nsDef != NULL) {
		xmlNsPtr cur = node->nsDef;
		while (cur != NULL) {
			if (cur->href != NULL)
				kvect->storevalue((char*)cur->href);
			else
				kvect->storevalue("");
			kvect->storevalue((char*)cur->prefix);
			cur = cur->next;
		}
	}
	return kvect;
}

Atanor* Atanorxml::MethodNew(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	string name = callfunc->Evaluate(0, aNULL, idthread)->String();
	xmlChar* n = xmlCharStrdup((const char*)name.c_str());
	node = xmlNewNodeEatName(NULL, n);
	return this;
}

Atanor* Atanorxml::Methodxmlstring(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (node == NULL)
		return aNULL;
	xmlDocPtr doc = node->doc;
	xmlNodePtr topnode = doc->children;
	doc->children = node;
	xmlNodePtr thenext = node->next;
	node->next = NULL;
	xmlChar* buff = NULL;
	int longueur;
	xmlDocDumpMemoryEnc(doc, &buff, &longueur, "UTF-8");
	Atanor* kstr = globalAtanor->Providestring((char*)buff);
	xmlFree(buff);
	doc->children = topnode;
	node->next = thenext;
	return kstr;
}


Atanor* Atanorxml::MethodDocument(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (node == NULL)
		return globalAtanor->Returnerror("XML(097): Empty node", idthread);
	if (node->doc == NULL)
		return globalAtanor->Returnerror("XML(099): No document", idthread);
	Atanorxmldoc* xdoc = new Atanorxmldoc(node->doc);
	xdoc->filename = xdoc->doc->name;
	return xdoc;
}




