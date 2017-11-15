###################################################################
include Makefile.in
################ To compile ATANOR#################################
SOURCEATANOR = atanor.cxx atanoratanor.cxx atanorbool.cxx atanorbvector.cxx atanorbyte.cxx atanordate.cxx atanordecimal.cxx\
atanorfile.cxx atanorufile.cxx atanorfloat.cxx atanorfmatrix.cxx atanorfraction.cxx atanorframeinstance.cxx atanorfvector.cxx atanordvector.cxx\
atanorbinmap.cxx atanorbinmapi.cxx atanorbinmapf.cxx atanorbinmaps.cxx atanorbinmapu.cxx atanorsynode.cxx atanortable.cxx\
atanorgrammar.cxx atanorhaskell.cxx atanorimatrix.cxx atanorint.cxx atanoriterator.cxx atanorivector.cxx atanorlist.cxx atanorlvector.cxx\
atanorlong.cxx atanormap.cxx atanormapf.cxx atanormapff.cxx atanormapfi.cxx atanormapfs.cxx atanormapfu.cxx atanormapi.cxx\
atanormapif.cxx atanormapii.cxx atanormapis.cxx atanormapiu.cxx atanormapsf.cxx atanormapsi.cxx atanormapss.cxx atanormapu.cxx\
atanormapuf.cxx atanormapui.cxx atanormapuu.cxx atanorprimemap.cxx atanorprimemapf.cxx atanorprimemapff.cxx atanorprimemapfi.cxx\
atanorprimemapfs.cxx atanorprimemapfu.cxx atanorprimemapi.cxx atanorprimemapif.cxx atanorprimemapii.cxx atanorprimemapis.cxx\
atanorprimemapiu.cxx atanorprimemapsf.cxx atanorprimemapsi.cxx atanorprimemapss.cxx atanorprimemapu.cxx atanorprimemapuf.cxx\
atanorprimemapui.cxx atanorprimemapuu.cxx atanorshort.cxx atanorsocket.cxx atanorstdin.cxx atanorstring.cxx atanorsvector.cxx\
atanorsys.cxx atanortime.cxx atanortransducer.cxx atanortreemap.cxx atanortreemapf.cxx atanortreemapff.cxx atanortreemapfi.cxx\
atanortreemapfs.cxx atanortreemapfu.cxx atanortreemapi.cxx atanortreemapif.cxx atanortreemapii.cxx atanortreemapis.cxx atanortreemapiu.cxx\
atanortreemapsf.cxx atanortreemapsi.cxx atanortreemapss.cxx atanortreemapu.cxx atanortreemapuf.cxx atanortreemapui.cxx atanortreemapuu.cxx\
atanorustring.cxx atanoruvector.cxx atanorvector.cxx automate.cxx automaton.cxx codecompile.cxx codeexecute.cxx codeparse.cxx codeoperations.cxx\
atanorrawstring.cxx containerrecording.cxx conversion.cxx globalatanor.cxx objectrecording.cxx predicate.cxx predicatefunctions.cxx procedures.cxx tools.cxx\
atanorhvector.cxx atanorbinmapl.cxx atanormapfl.cxx atanormapl.cxx atanormaplf.cxx atanormapll.cxx atanormapls.cxx atanormaplu.cxx atanormapsl.cxx atanormapul.cxx\
atanorprimemapfl.cxx atanorprimemapl.cxx atanorprimemaplf.cxx atanorprimemapll.cxx atanorprimemapls.cxx atanorprimemaplu.cxx atanorprimemapsl.cxx atanorprimemapul.cxx\
atanortreemapfl.cxx atanortreemapl.cxx atanortreemaplf.cxx atanortreemapll.cxx atanortreemapls.cxx atanortreemaplu.cxx atanortreemapsl.cxx atanortreemapul.cxx
#------------------------------------------------------------              
SOURCELIB= atanorcurl.cxx atanorsqlite.cxx atanorxml.cxx atanorxmldoc.cxx libsrecording.cxx $(SOUNDFILE) $(SOURCEGUI)
#------------------------------------------------------------
SOURCEPYTHON = atanorpython.cxx
#------------------------------------------------------------
SOURCELINEARC = daxpy.c  ddot.c  dnrm2.c  dscal.c
SOURCELINEARCXX = liblinear.cxx
SOURCELINEARCPP = linear.cpp  tron.cpp
#------------------------------------------------------------
SOURCECRFSUITEC = themain.c dump.c iwa.c learn.c option.c reader.c tag.c
SOURCECRFSUITECXX = atanorcrfsuite.cxx
#------------------------------------------------------------
SOURCEWAPITIC = bcd.c decoder.c gradient.c lbfgs.c model.c options.c pattern.c progress.c quark.c reader.c rprop.c sgdl1.c thread.c tools.c vmath.c wapiti.c
SOURCEWAPITICPP = puretagger.cpp
SOURCEWAPITICXX = atanorwapiti.cxx
#------------------------------------------------------------
SOURCEWORD2VECCXX = atanorword2vec.cxx
#------------------------------------------------------------
SOURCEATANORMAIN = atanormain.cxx 
#------------------------------------------------------------
OBJECTATANOR = $(SOURCEATANOR:%.cxx=$(OBJPATH)/%.o)
OBJECTLIB = $(SOURCELIB:%.cxx=$(LIBOBJPATH)/%.o)
#------------------------------------------------------------    
OBJECTATANORMAIN = $(SOURCEATANORMAIN:%.cxx=$(LIBOBJPATH)/%.o)
#------------------------------------------------------------
OBJECTPYTHON = $(SOURCEPYTHON:%.cxx=$(LIBOBJPATH)/%.o)
#------------------------------------------------------------
OBJECTLINEARC = $(SOURCELINEARC:%.c=$(LIBOBJPATH)/linear/%.o)
OBJECTLINEARCXX = $(SOURCELINEARCXX:%.cxx=$(LIBOBJPATH)/linear/%.o)
OBJECTLINEARCPP = $(SOURCELINEARCPP:%.cpp=$(LIBOBJPATH)/linear/%.o)
#------------------------------------------------------------
OBJECTWAPITIC = $(SOURCEWAPITIC:%.c=$(LIBOBJPATH)/wapiti/%.o)
OBJECTWAPITICXX = $(SOURCEWAPITICXX:%.cxx=$(LIBOBJPATH)/wapiti/%.o)
OBJECTWAPITICPP = $(SOURCEWAPITICPP:%.cpp=$(LIBOBJPATH)/wapiti/%.o)
#------------------------------------------------------------
OBJECTWORD2VECCXX = $(SOURCEWORD2VECCXX:%.cxx=$(LIBOBJPATH)/%.o)
#------------------------------------------------------------
OBJECTCRFSUITEC = $(SOURCECRFSUITEC:%.c=$(LIBOBJPATH)/suite/%.o)
OBJECTCRFSUITECXX = $(SOURCECRFSUITECXX:%.cxx=$(LIBOBJPATH)/suite/%.o)
#------------------------------------------------------------

ATANORBASICFLAGS = -w -c -fPIC -O3 -DUNIX $(FLTKFLAG) $(JPEGFLAG) -DCURL_STATICLIB -DFL_INTERNALS -DSQLITE_ENABLE_COLUMN_METADATA -DSQLITE_THREADSAFE $(FLAGMPG123) $(REGEX) $(SOUNDFLAG) $(SPECFLAGS) 
ATANORCFLAGS = -std=c99 $(ATANORBASICFLAGS)
ATANORFLAGS = $(C++11Flag) $(ATANORBASICFLAGS)

ATANORincludes= -Iinclude -Iinclude/libs $(INCLUDEPATH) $(INCLUDEPYTHON)

#if you compile your own version of fltk, you will need those:
#FLTKX11LIBS = -lXext -lXft -lXinerama -lX11 -lfontconfig -lXfixes

ATANORSYSTEMLIBS = $(SYSTEMSPATH) -lpthread $(MACLIBS) $(LIBSOUND) $(LIBMPG123) $(LIBREGEX) $(FLTKLIBS) $(FLTKX11LIBS) $(JPEGLIB) $(GMPLIB) -lcurl -lxml2 -lsqlite3 -ldl $(SSLLIB) $(CRYPTOLIB) -lldap  

#------------------------------------------------------------
$(OBJPATH)/%.o: src/%.cxx
	g++ $(ATANORFLAGS) $(ATANORincludes) $< -o $@  

$(LIBOBJPATH)/%.o: src/%.cxx
	g++ $(ATANORFLAGS) $(ATANORincludes) $< -o $@  

$(LIBOBJPATH)/%.o: src/libs/%.cxx
	g++ $(ATANORFLAGS) $(ATANORincludes) $< -o $@  

$(LIBOBJPATH)/%.o: src/libs/%.mm
	g++ $(ATANORFLAGS) $(ATANORincludes) $< -o $@  


#------------------------------------------------------------
$(LIBOBJPATH)/suite/%.o: crfsuite/src/%.c
	gcc -Icrfsuite/include $(ATANORCFLAGS) $(ATANORincludes) $< -o $@  

$(LIBOBJPATH)/suite/%.o: crfsuite/src/%.cxx
	gcc -Icrfsuite/include $(ATANORFLAGS) $(ATANORincludes) $< -o $@  

$(LIBOBJPATH)/linear/%.o: liblinear/src/%.c
	gcc -Iliblinear/include $(ATANORCFLAGS) $(ATANORincludes) $< -o $@  

$(LIBOBJPATH)/linear/%.o: liblinear/src/%.cxx
	gcc -Iliblinear/include $(ATANORFLAGS) $(ATANORincludes) $< -o $@  

$(LIBOBJPATH)/linear/%.o: liblinear/src/%.cpp
	gcc -Iliblinear/include $(ATANORFLAGS) $(ATANORincludes) $< -o $@  
	
$(LIBOBJPATH)/wapiti/%.o: libwapiti/src/%.c
	gcc -Ilibwapiti/include $(ATANORCFLAGS) -DWAPITIPARSE$(ATANORincludes) $< -o $@  

$(LIBOBJPATH)/wapiti/%.o: libwapiti/src/%.cxx
	gcc -Ilibwapiti/include $(ATANORFLAGS) -DWAPITIPARSE $(ATANORincludes) $< -o $@  

$(LIBOBJPATH)/wapiti/%.o: libwapiti/src/%.cpp
	gcc -Ilibwapiti/include $(ATANORFLAGS) -DWAPITIPARSE $(ATANORincludes) $< -o $@  

$(LIBOBJPATH)/%.o: libword2vec/src/%.cxx
	gcc -Ilibword2vec/include $(ATANORFLAGS) $(ATANORincludes) $< -o $@  

#------------------------------------------------------------
libatanor: $(OBJECTATANOR)
	g++ -shared -o $(BINPATH)/libatanor.so $(OBJECTATANOR)
	
# For those who prefer a small executable linked with a dynamic library
#atanor: $(OBJECTATANORMAIN) $(OBJECTLIB) $(OBJECTLIBMM)
#	g++ -o $(BINPATH)/atan $(OBJECTATANORMAIN) -L$(BINPATH) -latanor $(OBJECTLIB) $(OBJECTLIBMM) $(ATANORSYSTEMLIBS)

# For those who do not want to be bothered with a dynamic library link...
atanor: $(OBJECTATANORMAIN) $(OBJECTATANOR) $(OBJECTLIB) $(OBJECTLIBMM)
	g++ -o $(BINPATH)/atan $(OBJECTATANORMAIN) $(OBJECTATANOR) $(OBJECTLIB) $(OBJECTLIBMM) $(ATANORSYSTEMLIBS)

pyatan: $(OBJECTPYTHON) $(OBJECTATANOR)
	g++ -shared -o $(BINPATH)/pyatan.so $(OBJECTATANOR) $(OBJECTPYTHON) $(PYTHONLIB) $(LIBREGEX)

linear: $(OBJECTLINEARC) $(OBJECTLINEARCXX) $(OBJECTLINEARCPP)
	g++ -shared -o $(BINPATH)/liblinear.so $(OBJECTLINEARC) $(OBJECTLINEARCPP) $(OBJECTLINEARCXX) -L$(BINPATH) -latanor $(LIBREGEX) 

crfsuite: $(OBJECTCRFSUITEC) $(OBJECTCRFSUITECXX)	
	g++ -shared -o $(BINPATH)/libcrfsuite.so $(OBJECTCRFSUITEC) $(OBJECTCRFSUITECXX) $(SYSTEMSPATH) -lcqdb -lccrfsuite -llbfgs -L$(BINPATH) -latanor $(LIBREGEX) 

wapiti: $(OBJECTWAPITIC) $(OBJECTWAPITICXX) $(OBJECTWAPITICPP)
	g++ -shared -o $(BINPATH)/libwapiti.so $(OBJECTWAPITIC) $(OBJECTWAPITICPP) $(OBJECTWAPITICXX) -L$(BINPATH) -latanor $(LIBREGEX) 

word2vec: $(OBJECTWORD2VECCXX)
	g++ -shared -o $(BINPATH)/libword2vec.so $(OBJECTWORD2VECCXX) -L$(BINPATH) -latanor $(LIBREGEX)

all: install libatanor atanor
	$(libatanor)
	$(atanor)

full: install libatanor atanor pyatan wapiti crfsuite linear word2vec
	$(libatanor)
	$(atanor)
	$(pyatan)
	$(wapiti)
	$(crfsuite)
	$(linear)
	$(word2vec)
            

install:
	mkdir -p bin
	mkdir -p $(BINPATH)
	mkdir -p objs
	mkdir -p $(OBJPATH)
	mkdir -p libobjs
	mkdir -p $(LIBOBJPATH)
	mkdir -p $(LIBOBJPATH)/linear
	mkdir -p $(LIBOBJPATH)/wapiti
	mkdir -p $(LIBOBJPATH)/suite

clean:
	rm -f $(OBJPATH)/*.o
	rm -f $(LIBOBJPATH)/*.o
	rm -f $(LIBOBJPATH)/linear/*.o
	rm -f $(LIBOBJPATH)/wapiti/*.o
	rm -f $(LIBOBJPATH)/suite/*.o	
	rm -f $(BINPATH)/libatanor.so
	rm -f $(BINPATH)/atan
	rm -f $(BINPATH)/pyatan.so
	rm -f $(BINPATH)/libcrfsuite.so
	rm -f $(BINPATH)/liblinear.so
	rm -f $(BINPATH)/libwapiti.so
	rm -f $(BINPATH)/libword2vec.so

cleanlibs:
	rm -f $(LIBOBJPATH)/linear/*.o
	rm -f $(LIBOBJPATH)/wapiti/*.o
	rm -f $(LIBOBJPATH)/suite/*.o	
	rm -f $(BINPATH)/pyatan.so
	rm -f $(BINPATH)/libcrfsuite.so
	rm -f $(BINPATH)/liblinear.so
	rm -f $(BINPATH)/libwapiti.so
	rm -f $(BINPATH)/libword2vec.so




