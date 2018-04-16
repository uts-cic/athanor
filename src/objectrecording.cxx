
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
filename   : objectrecording.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorversion.h"

//The objects we need to record
#include "atanorstring.h"
#include "atanorfloat.h"
#include "atanordecimal.h"
#include "atanorint.h"
#include "atanorshort.h"
#include "atanorustring.h"
#include "atanorrawstring.h"
#include "atanorlong.h"
#include "atanorframeinstance.h"
#include "atanorfile.h"
#include "atanorufile.h"
#include "atanorbyte.h"
#include "atanoriterator.h"
#include "constobjects.h"
#include "atanorbool.h"
#include "atanordate.h"
#include "atanortime.h"
#include "atanorfraction.h"
#include "atanorsocket.h"
#include "atanortransducer.h"
#include "atanorsys.h"
#include "atanorgrammar.h"
#include "atanorimatrix.h"
#include "atanorfmatrix.h"
#include "atanoratanor.h"
#include "predicate.h"
#include "atanorstdin.h"
#include "atanorsynode.h"
//-------------------------------------------------------

void AtanorGlobal::RecordObjects() {

	newInstance[a_self] = new AtanorSelf(this);
	newInstance[a_let] = new AtanorLet(this);

	newInstance[a_call] = new AtanorFunctionCall(this);
	
	newInstance[a_term] = new AtanorPredicateTerm(this, a_term);

	//These objects do not automatically record into the tracker for late removal...
	Atanor* a = new AtanorLoopString(this);
	newInstance[a_sloop] = a;
	RecordInTracker(a);
	
	a = new AtanorLoopUString(this);
	newInstance[a_uloop] = a;
	RecordInTracker(a);
	
	a = new AtanorLoopInteger(this);
	newInstance[a_iloop] = a;
	RecordInTracker(a);

	a = new AtanorLoopLong(this);
	newInstance[a_lloop] = a;
	RecordInTracker(a);

	a = new AtanorLoopDecimal(this);
	newInstance[a_dloop] = a;
	RecordInTracker(a);

	a = new AtanorLoopFloat(this);
	newInstance[a_floop] = a;
	RecordInTracker(a);

	a = new AtanorLoopBoolean;
	newInstance[a_bloop] = a;
	RecordInTracker(a);

	Atanoratanor::InitialisationModule(this, AtanorVersion());

	Atanorstring::InitialisationModule(this, AtanorVersion());
	Atanorrawstring::InitialisationModule(this, AtanorVersion());
	Atanorint::InitialisationModule(this, AtanorVersion());
	Atanorshort::InitialisationModule(this, AtanorVersion());
	Atanorlong::InitialisationModule(this, AtanorVersion());
	Atanordecimal::InitialisationModule(this, AtanorVersion());
	Atanorfloat::InitialisationModule(this, AtanorVersion());
	Atanorbyte::InitialisationModule(this, AtanorVersion());
	Atanorustring::InitialisationModule(this, AtanorVersion());
	Atanorbool::InitialisationModule(this, AtanorVersion());

	Atanorframeinstance::InitialisationModule(this, AtanorVersion());
	
	Atanoriterator::InitialisationModule(this, AtanorVersion());

	Atanorfile::InitialisationModule(this, AtanorVersion());
	Atanorufile::InitialisationModule(this, AtanorVersion());

	Atanordate::InitialisationModule(this, AtanorVersion());
	Atanortime::InitialisationModule(this, AtanorVersion());

	Atanorfraction::InitialisationModule(this, AtanorVersion());
	Atanorsocket::InitialisationModule(this, AtanorVersion());

	Atanortransducer::InitialisationModule(this, AtanorVersion());
	Atanorgrammar::InitialisationModule(this, AtanorVersion());

	Atanorsys::InitialisationModule(this, AtanorVersion());

	Atanorimatrix::InitialisationModule(this, AtanorVersion());
	Atanorfmatrix::InitialisationModule(this, AtanorVersion());
	Atanorstdin::InitialisationModule(this, AtanorVersion());

	//------------------ Predicates ------------------
	AtanorPredicate::InitialisationModule(this, AtanorVersion());
	Atanorsynode::InitialisationModule(this, AtanorVersion());
	
}




