###################################################################
include Makefile.in
################ To compile ATANOR#################################
SOURCEATANOR = atanor.cxx atanoratanor.cxx atanorbool.cxx atanorbvector.cxx atanorbyte.cxx atanordate.cxx atanordecimal.cxx\
atanorfile.cxx atanorfloat.cxx atanorfmatrix.cxx atanorfraction.cxx atanorframeinstance.cxx atanorfvector.cxx atanordvector.cxx\
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
atanorufile.cxx atanorustring.cxx atanoruvector.cxx atanorvector.cxx automate.cxx automaton.cxx codecompile.cxx codeexecute.cxx codeparse.cxx codeoperations.cxx\
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
SOURCECRFSUITEC = themain.c dump.c iwa.c learn.c c_option.c c_reader.c tag.c
SOURCECRFSUITECXX = atanorcrfsuite.cxx
#------------------------------------------------------------
SOURCEWAPITIC = bcd.c decoder.c gradient.c lbfgs.c model.c woptions.c pattern.c progress.c quark.c reader.c rprop.c sgdl1.c thread.c wtools.c vmath.c wapiti.c
SOURCEWAPITICPP = puretagger.cpp
SOURCEWAPITICXX = atanorwapiti.cxx
#------------------------------------------------------------
SOURCEATANORMAIN = atanormain.cxx 
#------------------------------------------------------------
OBJECTATANOR = $(SOURCEATANOR:%.cxx=objs/%.o)
OBJECTLIB = $(SOURCELIB:%.cxx=libobjs/%.o)
#------------------------------------------------------------    
OBJECTATANORMAIN = $(SOURCEATANORMAIN:%.cxx=libobjs/%.o)
#------------------------------------------------------------
OBJECTPYTHON = $(SOURCEPYTHON:%.cxx=libobjs/%.o)
#------------------------------------------------------------
OBJECTLINEARC = $(SOURCELINEARC:%.c=libobjs/%.o)
OBJECTLINEARCXX = $(SOURCELINEARCXX:%.cxx=libobjs/%.o)
OBJECTLINEARCPP = $(SOURCELINEARCPP:%.cpp=libobjs/%.o)
#------------------------------------------------------------
OBJECTWAPITIC = $(SOURCEWAPITIC:%.c=libobjs/%.o)
OBJECTWAPITICXX = $(SOURCEWAPITICXX:%.cxx=libobjs/%.o)
OBJECTWAPITICPP = $(SOURCEWAPITICPP:%.cpp=libobjs/%.o)
#------------------------------------------------------------
OBJECTCRFSUITEC = $(SOURCECRFSUITEC:%.c=libobjs/%.o)
OBJECTCRFSUITECXX = $(SOURCECRFSUITECXX:%.cxx=libobjs/%.o)
#------------------------------------------------------------

ATANORCFLAGS = -w -c -fPIC -O3 -DUNIX $(FLTKFLAG) $(JPEGFLAG) -DCURL_STATICLIB -DFL_INTERNALS -DSQLITE_ENABLE_COLUMN_METADATA -DSQLITE_THREADSAFE $(FLAGMPG123) $(REGEX) $(SOUNDFLAG) 
ATANORFLAGS = $(C++11Flag) $(ATANORCFLAGS)

ATANORincludes= -Iinclude -Iinclude/libs $(INCLUDEPATH) $(INCLUDEPYTHON)

#if you compile your own version of fltk, you will need those:
#FLTKX11LIBS = -lXext -lXft -lXinerama -lX11 -lfontconfig -lXfixes

ATANORSYSTEMLIBS = $(SYSTEMSPATH) -lpthread $(MACLIBS) -lcurl -lxml2 -lsqlite3 -ldl $(SSLLIB) $(CRYPTOLIB) -lldap $(GMPLIB) $(LIBSOUND) $(LIBMPG123) $(LIBREGEX) $(FLTKLIBS) $(FLTKX11LIBS) $(JPEGLIB) 

#------------------------------------------------------------
objs/%.o: src/%.cxx
	g++ $(ATANORFLAGS) $(ATANORincludes) $< -o $@  

libobjs/%.o: src/%.cxx
	g++ $(ATANORFLAGS) $(ATANORincludes) $< -o $@  

libobjs/%.o: src/libs/%.cxx
	g++ $(ATANORFLAGS) $(ATANORincludes) $< -o $@  

libobjs/%.o: src/libs/%.mm
	g++ $(ATANORFLAGS) $(ATANORincludes) $< -o $@  

#------------------------------------------------------------
libatanor: $(OBJECTATANOR)
	g++ -shared -o libatanor.so $(OBJECTATANOR)
	
# For those who prefer a small executable linked with a dynamic library
#atanor: $(OBJECTATANORMAIN) $(OBJECTLIB) $(OBJECTLIBMM)
#	g++ -o atan $(OBJECTATANORMAIN) -L. -latanor $(OBJECTLIB) $(OBJECTLIBMM) $(ATANORSYSTEMLIBS)

# For those who do not want to be bothered with a dynamic library link...
atanor: $(OBJECTATANORMAIN) $(OBJECTATANOR) $(OBJECTLIB) $(OBJECTLIBMM)
	g++ -o atan $(OBJECTATANORMAIN) $(OBJECTATANOR) $(OBJECTLIB) $(OBJECTLIBMM) $(ATANORSYSTEMLIBS)

#------------------------------------------------------------
libobjs/%.o: linuxcrfsuite/src/%.c
	gcc -Ilinuxcrfsuite/include $(ATANORCFLAGS) $(ATANORincludes) $< -o $@  

libobjs/%.o: linuxcrfsuite/src/%.cxx
	gcc -Ilinuxcrfsuite/include $(ATANORFLAGS) $(ATANORincludes) $< -o $@  

libobjs/%.o: linuxliblinear/src/%.c
	gcc -Ilinuxliblinear/include $(ATANORCFLAGS) $(ATANORincludes) $< -o $@  

libobjs/%.o: linuxliblinear/src/%.cxx
	gcc -Ilinuxliblinear/include $(ATANORFLAGS) $(ATANORincludes) $< -o $@  

libobjs/%.o: linuxliblinear/src/%.cpp
	gcc -Ilinuxliblinear/include $(ATANORFLAGS) $(ATANORincludes) $< -o $@  
	
libobjs/%.o: linuxwapiti/src/%.c
	gcc -Ilinuxwapiti/include $(ATANORCFLAGS) -DWAPITIPARSE$(ATANORincludes) $< -o $@  

libobjs/%.o: linuxwapiti/src/%.cxx
	gcc -Ilinuxwapiti/include $(ATANORFLAGS) -DWAPITIPARSE $(ATANORincludes) $< -o $@  

libobjs/%.o: linuxwapiti/src/%.cpp
	gcc -Ilinuxwapiti/include $(ATANORFLAGS) -DWAPITIPARSE $(ATANORincludes) $< -o $@  
#------------------------------------------------------------

pyatan: $(OBJECTPYTHON) $(OBJECTATANOR)
	g++ -shared -o pyatan.so $(OBJECTATANOR) $(OBJECTPYTHON) -L. $(PYTHONLIB) $(LIBREGEX)

linear: $(OBJECTLINEARC) $(OBJECTLINEARCXX) $(OBJECTLINEARCPP)
	g++ -shared -o liblinear.so $(OBJECTLINEARC) $(OBJECTLINEARCPP) $(OBJECTLINEARCXX) -L. -latanor $(LIBREGEX) 

crfsuite: $(OBJECTCRFSUITEC) $(OBJECTCRFSUITECXX)
	g++ -shared -o libcrfsuite.so $(OBJECTCRFSUITEC) $(OBJECTCRFSUITECXX) -L. $(SYSTEMSPATH) -lcqdb -lccrfsuite -llbfgs -latanor $(LIBREGEX) 

wapiti: $(OBJECTWAPITIC) $(OBJECTWAPITICXX) $(OBJECTWAPITICPP)
	g++ -shared -o libwapiti.so $(OBJECTWAPITIC) $(OBJECTWAPITICPP) $(OBJECTWAPITICXX) -L. -latanor $(LIBREGEX) 

all: libatanor atanor
	$(libatanor)
	$(atanor)

install:
	mkdir objs
	mkdir libobjs

clean:
	rm objs/*.o
	rm libobjs/*.o
	rm libatanor.so
	rm atan
	rm pyatan.so
	rm libcrfsuite.so
	rm liblinear.so
	rm libwapiti.so







