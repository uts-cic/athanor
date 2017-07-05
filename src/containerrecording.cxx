
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
filename   : containerrecording.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanorversion.h"

#include "atanorvector.h"
#include "atanorlist.h"
#include "atanortable.h"

#include "atanorivector.h"
#include "atanorbvector.h"
#include "atanoruvector.h"
#include "atanorsvector.h"
#include "atanorfvector.h"
#include "atanordvector.h"
#include "atanorhvector.h"
#include "atanorlvector.h"

#include "atanormap.h"
#include "atanortreemap.h"
#include "atanorprimemap.h"

#include "atanormapi.h"
#include "atanormapf.h"
#include "atanortreemapi.h"
#include "atanortreemapf.h"

#include "atanormapu.h"
#include "atanortreemapu.h"

#include "atanormapff.h"
#include "atanormapfi.h"
#include "atanormapfs.h"
#include "atanormapfu.h"
#include "atanormapif.h"
#include "atanormapii.h"
#include "atanormapis.h"
#include "atanormapiu.h"
#include "atanormapsf.h"
#include "atanormapsi.h"
#include "atanormapss.h"
#include "atanormapuf.h"
#include "atanormapui.h"
#include "atanormapuu.h"
#include "atanormapfl.h"
#include "atanormapl.h"
#include "atanormaplf.h"
#include "atanormapll.h"
#include "atanormapls.h"
#include "atanormaplu.h"
#include "atanormapsl.h"
#include "atanormapul.h"

#include "atanortreemapff.h"
#include "atanortreemapfi.h"
#include "atanortreemapfs.h"
#include "atanortreemapfu.h"
#include "atanortreemapif.h"
#include "atanortreemapii.h"
#include "atanortreemapis.h"
#include "atanortreemapiu.h"
#include "atanortreemapsf.h"
#include "atanortreemapsi.h"
#include "atanortreemapss.h"
#include "atanortreemapuf.h"
#include "atanortreemapui.h"
#include "atanortreemapuu.h"
#include "atanortreemapfl.h"
#include "atanortreemapl.h"
#include "atanortreemaplf.h"
#include "atanortreemapll.h"
#include "atanortreemapls.h"
#include "atanortreemaplu.h"
#include "atanortreemapsl.h"
#include "atanortreemapul.h"

#include "atanorprimemapf.h"
#include "atanorprimemapff.h"
#include "atanorprimemapfi.h"
#include "atanorprimemapfs.h"
#include "atanorprimemapfu.h"
#include "atanorprimemapi.h"
#include "atanorprimemapif.h"
#include "atanorprimemapii.h"
#include "atanorprimemapis.h"
#include "atanorprimemapiu.h"
#include "atanorprimemapsf.h"
#include "atanorprimemapsi.h"
#include "atanorprimemapss.h"
#include "atanorprimemapu.h"
#include "atanorprimemapuf.h"
#include "atanorprimemapui.h"
#include "atanorprimemapuu.h"
#include "atanorprimemapfl.h"
#include "atanorprimemapl.h"
#include "atanorprimemaplf.h"
#include "atanorprimemapll.h"
#include "atanorprimemapls.h"
#include "atanorprimemaplu.h"
#include "atanorprimemapsl.h"
#include "atanorprimemapul.h"

#include "atanorbinmap.h"
#include "atanorbinmapf.h"
#include "atanorbinmapi.h"
#include "atanorbinmaps.h"
#include "atanorbinmapu.h"
#include "atanorbinmapl.h"


void AtanorGlobal::RecordContainers() {

	Atanorvector::InitialisationModule(this, AtanorVersion());
	Atanorivector::InitialisationModule(this, AtanorVersion());
	Atanorbvector::InitialisationModule(this, AtanorVersion());
	Atanorfvector::InitialisationModule(this, AtanorVersion());
	Atanordvector::InitialisationModule(this, AtanorVersion());
	Atanorhvector::InitialisationModule(this, AtanorVersion());
	Atanorlvector::InitialisationModule(this, AtanorVersion());
	Atanoruvector::InitialisationModule(this, AtanorVersion());
	Atanorsvector::InitialisationModule(this, AtanorVersion());

	Atanorlist::InitialisationModule(this, AtanorVersion());
	Atanortable::InitialisationModule(this, AtanorVersion());

	Atanormap::InitialisationModule(this, AtanorVersion());
	Atanortreemap::InitialisationModule(this, AtanorVersion());
	Atanorprimemap::InitialisationModule(this, AtanorVersion());

	Atanormapi::InitialisationModule(this, AtanorVersion());
	Atanormapf::InitialisationModule(this, AtanorVersion());
	Atanortreemapi::InitialisationModule(this, AtanorVersion());
	Atanortreemapf::InitialisationModule(this, AtanorVersion());

	Atanormapu::InitialisationModule(this, AtanorVersion());
	Atanortreemapu::InitialisationModule(this, AtanorVersion());

	Atanormapff::InitialisationModule(this, AtanorVersion());
	Atanormapfi::InitialisationModule(this, AtanorVersion());
	Atanormapfs::InitialisationModule(this, AtanorVersion());
	Atanormapfu::InitialisationModule(this, AtanorVersion());
	Atanormapif::InitialisationModule(this, AtanorVersion());
	Atanormapii::InitialisationModule(this, AtanorVersion());
	Atanormapis::InitialisationModule(this, AtanorVersion());
	Atanormapiu::InitialisationModule(this, AtanorVersion());
	Atanormapsf::InitialisationModule(this, AtanorVersion());
	Atanormapsi::InitialisationModule(this, AtanorVersion());
	Atanormapss::InitialisationModule(this, AtanorVersion());
	Atanormapuf::InitialisationModule(this, AtanorVersion());
	Atanormapui::InitialisationModule(this, AtanorVersion());
	Atanormapuu::InitialisationModule(this, AtanorVersion());
	Atanormapfl::InitialisationModule(this, AtanorVersion());
	Atanormapl::InitialisationModule(this, AtanorVersion());
	Atanormaplf::InitialisationModule(this, AtanorVersion());
	Atanormapll::InitialisationModule(this, AtanorVersion());
	Atanormapls::InitialisationModule(this, AtanorVersion());
	Atanormaplu::InitialisationModule(this, AtanorVersion());
	Atanormapsl::InitialisationModule(this, AtanorVersion());
	Atanormapul::InitialisationModule(this, AtanorVersion());

	Atanortreemapff::InitialisationModule(this, AtanorVersion());
	Atanortreemapfi::InitialisationModule(this, AtanorVersion());
	Atanortreemapfs::InitialisationModule(this, AtanorVersion());
	Atanortreemapfu::InitialisationModule(this, AtanorVersion());
	Atanortreemapif::InitialisationModule(this, AtanorVersion());
	Atanortreemapii::InitialisationModule(this, AtanorVersion());
	Atanortreemapis::InitialisationModule(this, AtanorVersion());
	Atanortreemapiu::InitialisationModule(this, AtanorVersion());
	Atanortreemapsf::InitialisationModule(this, AtanorVersion());
	Atanortreemapsi::InitialisationModule(this, AtanorVersion());
	Atanortreemapss::InitialisationModule(this, AtanorVersion());
	Atanortreemapuf::InitialisationModule(this, AtanorVersion());
	Atanortreemapui::InitialisationModule(this, AtanorVersion());
	Atanortreemapuu::InitialisationModule(this, AtanorVersion());
	Atanortreemapfl::InitialisationModule(this, AtanorVersion());
	Atanortreemapl::InitialisationModule(this, AtanorVersion());
	Atanortreemaplf::InitialisationModule(this, AtanorVersion());
	Atanortreemapll::InitialisationModule(this, AtanorVersion());
	Atanortreemapls::InitialisationModule(this, AtanorVersion());
	Atanortreemaplu::InitialisationModule(this, AtanorVersion());
	Atanortreemapsl::InitialisationModule(this, AtanorVersion());
	Atanortreemapul::InitialisationModule(this, AtanorVersion());

	Atanorprimemapf::InitialisationModule(this, AtanorVersion());
	Atanorprimemapff::InitialisationModule(this, AtanorVersion());
	Atanorprimemapfi::InitialisationModule(this, AtanorVersion());
	Atanorprimemapfs::InitialisationModule(this, AtanorVersion());
	Atanorprimemapfu::InitialisationModule(this, AtanorVersion());
	Atanorprimemapi::InitialisationModule(this, AtanorVersion());
	Atanorprimemapif::InitialisationModule(this, AtanorVersion());
	Atanorprimemapii::InitialisationModule(this, AtanorVersion());
	Atanorprimemapis::InitialisationModule(this, AtanorVersion());
	Atanorprimemapiu::InitialisationModule(this, AtanorVersion());
	Atanorprimemapsf::InitialisationModule(this, AtanorVersion());
	Atanorprimemapsi::InitialisationModule(this, AtanorVersion());
	Atanorprimemapss::InitialisationModule(this, AtanorVersion());
	Atanorprimemapu::InitialisationModule(this, AtanorVersion());
	Atanorprimemapuf::InitialisationModule(this, AtanorVersion());
	Atanorprimemapui::InitialisationModule(this, AtanorVersion());
	Atanorprimemapuu::InitialisationModule(this, AtanorVersion());
	Atanorprimemapfl::InitialisationModule(this, AtanorVersion());
	Atanorprimemapl::InitialisationModule(this, AtanorVersion());
	Atanorprimemaplf::InitialisationModule(this, AtanorVersion());
	Atanorprimemapll::InitialisationModule(this, AtanorVersion());
	Atanorprimemapls::InitialisationModule(this, AtanorVersion());
	Atanorprimemaplu::InitialisationModule(this, AtanorVersion());
	Atanorprimemapsl::InitialisationModule(this, AtanorVersion());
	Atanorprimemapul::InitialisationModule(this, AtanorVersion());

	Atanorbinmap::InitialisationModule(this, AtanorVersion());
	Atanorbinmapf::InitialisationModule(this, AtanorVersion());
	Atanorbinmapi::InitialisationModule(this, AtanorVersion());
	Atanorbinmaps::InitialisationModule(this, AtanorVersion());
	Atanorbinmapu::InitialisationModule(this, AtanorVersion());
	Atanorbinmapl::InitialisationModule(this, AtanorVersion());

	mapnewInstances[b_int][b_int] = newInstance[Atanormapii::idtype];
	mapnewInstances[b_int][b_long] = newInstance[Atanormapll::idtype];
	mapnewInstances[b_int][b_ustring] = newInstance[Atanormapiu::idtype];
	mapnewInstances[b_int][b_float] = newInstance[Atanormapif::idtype];
	mapnewInstances[b_int][b_string] = newInstance[Atanormapis::idtype];
	mapnewInstances[b_int][b_none] = newInstance[Atanormapi::idtype];
	mapnewInstances[b_long][b_int] = newInstance[Atanormapll::idtype];
	mapnewInstances[b_long][b_long] = newInstance[Atanormapll::idtype];
	mapnewInstances[b_long][b_ustring] = newInstance[Atanormaplu::idtype];
	mapnewInstances[b_long][b_float] = newInstance[Atanormaplf::idtype];
	mapnewInstances[b_long][b_string] = newInstance[Atanormapls::idtype];
	mapnewInstances[b_long][b_none] = newInstance[Atanormapl::idtype];
	mapnewInstances[b_ustring][b_int] = newInstance[Atanormapui::idtype];
	mapnewInstances[b_ustring][b_long] = newInstance[Atanormapul::idtype];
	mapnewInstances[b_ustring][b_ustring] = newInstance[Atanormapuu::idtype];
	mapnewInstances[b_ustring][b_float] = newInstance[Atanormapuf::idtype];
	mapnewInstances[b_ustring][b_string] = newInstance[Atanormapuu::idtype];
	mapnewInstances[b_ustring][b_none] = newInstance[Atanormapu::idtype];
	mapnewInstances[b_float][b_int] = newInstance[Atanormapfi::idtype];
	mapnewInstances[b_float][b_long] = newInstance[Atanormapfl::idtype];
	mapnewInstances[b_float][b_ustring] = newInstance[Atanormapfu::idtype];
	mapnewInstances[b_float][b_float] = newInstance[Atanormapff::idtype];
	mapnewInstances[b_float][b_string] = newInstance[Atanormapfs::idtype];
	mapnewInstances[b_float][b_none] = newInstance[Atanormapf::idtype];
	mapnewInstances[b_string][b_int] = newInstance[Atanormapsi::idtype];
	mapnewInstances[b_string][b_long] = newInstance[Atanormapsl::idtype];
	mapnewInstances[b_string][b_ustring] = newInstance[Atanormapuu::idtype];
	mapnewInstances[b_string][b_float] = newInstance[Atanormapsf::idtype];
	mapnewInstances[b_string][b_string] = newInstance[Atanormapss::idtype];
	mapnewInstances[b_string][b_none] = newInstance[Atanormap::idtype];

}




