
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
filename   : libsrecording.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorversion.h"
#include "globalatanor.h"

//The objects we need to record
#ifdef WITHFLTK
#include "atanorbitmap.h"
#include "atanorbox.h"
#include "atanorbrowser.h"
#include "atanorbutton.h"
#include "atanorchoice.h"
#include "atanorcounter.h"
#include "atanoreditor.h"
#include "atanorfilebrowser.h"
#include "atanorgroup.h"
#include "atanorimage.h"
#include "atanorprogress.h"
#include "atanorslider.h"
#include "atanortabs.h"
#include "atanorwindow.h"
#include "atanorwinput.h"
#include "atanorwoutput.h"
#include "atanorwtable.h"
#endif

#include "atanorcurl.h"

#ifdef ATANORSOUND
#include "atanorsound.h"
#endif

#if defined(MACSOUND) || defined(WIN32)
#include "atanormp3.h"
#endif

#include "atanorxml.h"
#include "atanorxmldoc.h"
#include "atanorsqlite.h"

//-------------------------------------------------------

void RecordLibs(AtanorGlobal* global) {
	//Self instanciations... the "let" in Hashkell is considered as a self
#ifdef ATANORSOUND
	Atanorsound::InitialisationModule(global, AtanorVersion());
#endif

#if defined(MACSOUND) || defined(WIN32)
	Atanormp3::InitialisationModule(global, AtanorVersion());
#endif

#ifdef WITHFLTK
	Atanorbitmap::InitialisationModule(global, AtanorVersion());
	Atanorbox::InitialisationModule(global, AtanorVersion());
	Atanorbrowser::InitialisationModule(global, AtanorVersion());
	Atanorbutton::InitialisationModule(global, AtanorVersion());
	Atanorchoice::InitialisationModule(global, AtanorVersion());
	Atanorcounter::InitialisationModule(global, AtanorVersion());
	Atanoreditor::InitialisationModule(global, AtanorVersion());
	Atanorfilebrowser::InitialisationModule(global, AtanorVersion());
	Atanorgroup::InitialisationModule(global, AtanorVersion());
	Atanorimage::InitialisationModule(global, AtanorVersion());
	Atanorprogress::InitialisationModule(global, AtanorVersion());
	Atanorslider::InitialisationModule(global, AtanorVersion());
	Atanortabs::InitialisationModule(global, AtanorVersion());
	Atanorwindow::InitialisationModule(global, AtanorVersion());
	Atanorwinput::InitialisationModule(global, AtanorVersion());
	Atanorwoutput::InitialisationModule(global, AtanorVersion());
	Atanorwtable::InitialisationModule(global, AtanorVersion());
#endif

	Atanorcurl::InitialisationModule(global, AtanorVersion());

	Atanorxml::InitialisationModule(global, AtanorVersion());
	Atanorxmldoc::InitialisationModule(global, AtanorVersion());

	Atanorsqlite::InitialisationModule(global, AtanorVersion());
}


Exporting bool AtanorIgnition(long nbthreads) {
	AtanorGlobal* global = AtanorCreate(nbthreads);
	if (global == NULL)
		return false;
	RecordLibs(global);
	return true;
}




