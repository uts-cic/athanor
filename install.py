# The directories to check are: /usr/lib64, /usr/lib or /usr/lib/x86_64-linux-gnu/

import sys
import os
from os import walk
import subprocess

sourcegui="""
#FLTK support
FLTKFLAG = -DWITHFLTK

SOURCEGUI= atanorbitmap.cxx atanorbox.cxx atanorbrowser.cxx atanorbutton.cxx atanorchoice.cxx atanorconsole.cxx \
atanorcounter.cxx atanoreditor.cxx atanorfilebrowser.cxx atanorfltk.cxx atanorgroup.cxx atanorimage.cxx \
atanorprogress.cxx atanorslider.cxx atanortabs.cxx atanorwidget.cxx atanorwindow.cxx atanorwinput.cxx atanorwoutput.cxx atanorwtable.cxx

"""

############################
ostype = subprocess.Popen("uname", stdout=subprocess.PIPE).stdout.read()
ostype=ostype.strip()
############################ MAC OS case...
if ostype=="Darwin":
    f=open("Makefile.in","w")
    print "MAC OS"
    f.write("BINPATH = bin/mac\n")
    f.write("OBJPATH = objs/mac\n")
    f.write("LIBOBJPATH = libobjs/mac\n")
    f.write("INCLUDEPATH= -DMAVERICK -DAPPLE -Iinclude/macos/fltk -Iinclude/macos/ao\n")
    MACLIBS= "MACLIBS= -framework Cocoa -framework AudioToolbox -framework AudioUnit -framework CoreAudio\n"
    f.write("# MAC OS support\n")
    f.write(MACLIBS)
    f.write(sourcegui)
    f.write("FLTKLIBS=-Llibs/macos -lfltk -lfltk_images\n")
    f.write("JPEGLIB = -lfltk_jpeg\n\n")    
    f.write("SOURCEMM = macsound.mm\n")
    f.write("SYSTEMSPATH = -Llibs/macos\n")
    f.write("OBJECTLIBMM = $(SOURCEMM:%.mm=$(LIBOBJPATH)/%.o)\n")
    f.write("\n")
    f.write("#Python\n")
    f.write("INCLUDEPYTHON = -I/System/Library/Frameworks/Python.framework/Versions/Current/include/python2.7\n")
    f.write("PYTHONLIB = /System/Library/Frameworks/Python.framework/Versions/Current/Python\n\n")
    f.write("LIBSOUND=-lao -lsndfile\n")
    f.write("SOUNDFILE=atanorsound.cxx atanormp3.cxx\n")
    f.write("SOUNDFLAG= -DATANORSOUND -DMACSOUND -Iinclude/macos -Iinclude/macos/ao\n")
    f.write("FLAGMPG123=-DUSEMPG123\n")
    f.write("LIBMPG123=-lmpg123\n")
    f.write("C++11Flag = -std=c++11\n")
    f.close();
    print "You can launch 'make all' now"
    sys.exit(0)
    
############################ Linux case ###########################################################################
### In the case a library is only available with a mangled name such as libboost_regex.so.1.53,
### the installer will create a "systems" directory in which symbolic links to these libraries will be created...
###################################################################################################################

versiongcc = subprocess.Popen(["gcc","-dumpversion"], stdout=subprocess.PIPE).stdout.read()
nbversion=float(versiongcc[:3])
print "GCC Version=",nbversion

#### We check some parameters ###

def displayhelp(s):
    if s!="":
        print
        print "Unknown command:", s
    print
    print "Options:"
    print " -nosound: Do not compile with sound support"
    print " -noregex: Do not compile with regular expression support"
    print " -pathregex: Path to regex include files"
    print " -nogui: Do not compile with GUI support"
    print " -nofastint: Do not compile with fast int"
    print " -crfsuite: Compile the crfsuite libs"
    print " -java: Prepare compiling to java"
    print " -gccversion: Directory names for intermediate and final files depend on GCCVERSION environment variable (setenv GCCVERSION `gcc -dumpversion`)"
    print " -pathlib path: provides a system path to check for system libraries"
    print " -help: display this help"
    print
    exit(-1)

nosound=False
gccversion=False
noregex=False
nogui=False
nofastint=False
regexpath=""
crfsuite=False
compilejava=False
   
libpath="/usr"
i=1

while i < len(sys.argv):
    if sys.argv[i]=="-java":
        compilejava=True
    elif sys.argv[i]=="-nosound":
        nosound=True
    elif sys.argv[i]=="-gccversion":
        gccversion=True
    elif sys.argv[i]=="-noregex":
        noregex=True
    elif sys.argv[i]=="-crfsuite":
        crfsuite=True
    elif sys.argv[i]=="-nogui":
        nogui=True        
    elif sys.argv[i]=="-nofastint":
        nofastint=True
    elif sys.argv[i]=="-help":
        displayhelp("")
    elif sys.argv[i]=="-pathregex":
        if i >= len(sys.argv):
            print "Missing regex path on command line"
            exit(-1)
        regexpath="-I"+sys.argv[i+1]
        i+=1
    elif sys.argv[i]=="-pathlib":
        if i >= len(sys.argv):
            print "Missing library path on command line"
            exit(-1)
        libpath=sys.argv[i+1]
        i+=1
    else:
        displayhelp(sys.argv[i])
    i+=1

sourcegui += """
FLTKLIBS=-lfltk -lfltk_images
"""
    
v=['libfltk', 'libfltk_images', 'libfltk_jpeg', 'libcurl', 'libboost_regex', 'libxml2', 'libssl', 
'libsqlite3', 'libmpg123', 'libao', 'libsndfile', 'libldap','libcrypto','libldap', 'libgmp', 'python2.7']

def traverse(libpath):
    for (dirpath, dirnames, filenames) in walk(libpath):
        for s in filenames:
            if "libgcc" in s:
                return libpath
        for d in dirnames:
            s=traverse(libpath+d+"/")
            if s!=None:
                return s
    return None


#### We look for the library path that contains all our libraries
if libpath=="/usr":#we look for libgcc
    libpath=traverse("/usr/lib64/")
    if libpath==None:
        libpath=traverse("/usr/lib/")
    if libpath==None:
        sys.exit(0)

############################
### We check the system path lib directories for the requested libraries
if libpath[-1] != '/':
    libpath+='/'

print "Investigating:",libpath
f = []
for (dirpath, dirnames, filenames) in walk(libpath):
    f.extend(filenames)
    break
    
for s in f:
    if s in v:
        v.remove(s)

found={}
ret=[]
for s in v:
    for x in f:
        if s in x:
            if s in found:
                if len(x) < len(found[s]):
                    found[s]=x  
            else:
                found[s]=x
                ret.append(s)

for s in ret:
    while s in v:
        v.remove(s)      

### If we have some mangled names, we need to create a "systems" directory
### to store symbolic links to them, which will be used to link our final application

os.system("mkdir -p systems")

if compilejava:
    os.system("rm -r java/regexboost.txt");
    os.system("rm -r java/regexcpp.txt");
    os.system("rm -r java/objspath.txt");

if gccversion:
    if compilejava:
        fs = open("java/objspath.txt","w")
        thepath='objs/linux'+versiongcc.strip()
	#thepath=os.path.abspath(thepath)
        fs.write(thepath)
        fs.close()

    if len(found)!=0:    
        for (dirpath, dirnames, filenames) in walk("."):
            if "systems" in dirnames:
                os.system("rm -r systems/linux$GCCVERSION")
                break
        
        os.system("mkdir systems/linux$GCCVERSION")

    for u in found:
        if ".a" not in found[u] and ".dylib" not in found[u]:
            command="ln -s "+libpath+found[u]+" systems/linux$GCCVERSION/"+u+".so"
            os.system(command)
else:
    if len(found)!=0:    
        for (dirpath, dirnames, filenames) in walk("."):
            if "systems" in dirnames:
                os.system("rm systems/*")
                break        
    for u in found:
        if ".a" not in found[u] and ".dylib" not in found[u]:
            command="ln -s "+libpath+found[u]+" systems/"+u+".so"
            os.system(command)
    

############################
f=open("Makefile.in","w")

if gccversion:
    if crfsuite:
        os.system("cd crfsuite; sh installgccversion.sh")
    f.write("CRFSUITEPATH = -Lcrfsuite/libs${GCCVERSION}\n")
    f.write("BINPATH = bin/linux${GCCVERSION}\n")
    f.write("OBJPATH = objs/linux${GCCVERSION}\n")
    f.write("LIBOBJPATH = libobjs/linux${GCCVERSION}\n")
    f.write("SYSTEMSPATH = -Lsystems/linux${GCCVERSION} -Llibs/linux${GCCVERSION} ${CRFSUITEPATH}\n")
else:
    if crfsuite:
        os.system("cd crfsuite; sh install.sh")
    f.write("CRFSUITEPATH = -Lcrfsuite/libs\n")
    f.write("BINPATH = bin/linux\n")
    f.write("OBJPATH = objs/linux\n")
    f.write("LIBOBJPATH = libobjs/linux\n")
    f.write("SYSTEMSPATH = -Lsystems -Llibs/linux ${CRFSUITEPATH}\n")
    
############################
specflags="SPECFLAGS =";
if nofastint:
    specflags+=" -DNOFASTTYPE"

#We cannot use the CODECVT include to read/write UTF16 files...
if nbversion<= 5.1:
    specflags+=" -DNOCODECVT"

if specflags!="SPECFLAGS =":
    specflags+="\n"    
    f.write(specflags)
    
############################
if len(v)!=0:
    print "--------------------------------"
    print
    print "Missing libraries:", v
    if "libfltk" in v or nogui==True:
        print
        print "Sorry atanor will have no GUI functionalities. Install FLTK package for GUI features..."
    else:
        f.write(sourcegui)
############################
if "python2.7" not in v:
    f.write("\n\n#Python support to compile atanor python library: 'pyatan'\n")
    for (dirpath, dirnames, filenames) in walk("/usr/include"):
        if "python2.7" in dirnames:
            f.write("INCLUDEPYTHON = -I/usr/include/python2.7\n")
            f.write("PYTHONLIB = \n")
            print
            print "You can compile the pyatan library (atanor python library)"
            print
        else:
            f.write("INCLUDEPYTHON = \n")
            f.write("PYTHONLIB = \n")
            print
            print "Could not find python 2.7 include and library paths. "
            print "Modify 'INCLUDEPYTHON' and 'PYTHONLIB' in Makefile.in if you want to compile pyatan (the atanor python library)"
            print            
        break
############################
includepath="INCLUDEPATH = -Iinclude/linux"
############################
jpegflag="#JPEGFLAG = -DFLTKNOJPEG"
jpeglib="#JPEGLIB = -lfltk_jpeg"

if "libfltk_jpeg" not in v:
    jpeglib=jpeglib.replace("#","")
else:
    jpegflag=jpegflag.replace("#","")
    print "No fltk_jpeg library available"

f.write("\n\n# JPEG support\n")
f.write(jpegflag+"\n")
f.write(jpeglib+"\n")
f.write("\n")


############################
    
flagmpg123="""
#FLAGMPG123=-DUSEMPG123
#LIBMPG123=-lmpg123
"""

if nosound==False:
    if "libmpg123" not in v:
        flagmpg123=flagmpg123.replace("#","")
    else:
        print "MPG123 will not be available in atanor"
    f.write("# MPG3 support")
    f.write(flagmpg123)
    f.write("\n")

############################
soundao="""
#LIBAO=-lao
#LIBSOUNDFILE=-lsndfile
"""
soundflag="""
#LIBSOUND=$(LIBAO) $(LIBSOUNDFILE)
#SOUNDFILE=atanorsound.cxx
#SOUNDFLAG= -DATANORSOUND
"""

if nosound==False:
    sndbool=False
    if "libao" not in v:
        soundao=soundao.replace("#LIBAO","LIBAO")
        soundflag=soundflag.replace("#","")
        includepath+=" -Iinclude/linux/ao"
        sndbool=True

    if "libsndfile" not in v:
        soundao=soundao.replace("#LIBSOUNDFILE","LIBSOUNDFILE")
        soundflag=soundflag.replace("#","")
        includepath+=" -Iinclude/linux/ao"

    if not sndbool:
        print "No sound available"
    else:    
        f.write("# SOUND (ao and sndfile) support")
        f.write(soundao)
        f.write(soundflag)
        f.write("\n")


############################
sslflag="""
#SSLLIB= -lssl
"""

if "libssl" not in v:
    sslflag=sslflag.replace("#","")
    
f.write("# SSL support")
f.write(sslflag)
f.write("\n")

############################
cryptoflag="""
#CRYPTOLIB= -lcrypto
"""

if "libcrypto" not in v:
    cryptoflag=cryptoflag.replace("#","")
    
f.write("# CRYPTO support")
f.write(cryptoflag)
f.write("\n")

############################
gmpflag="""
#GMPLIB= -lgmp
"""

if "libgmp" not in v:
    gmpflag=gmpflag.replace("#","")
    
f.write("# GMP support")
f.write(gmpflag)
f.write("\n")

############################
f.write(includepath+"\n")
############################
# We test now if the compiler onboard is compatible with C++11 norm
#C++11Flag = -std=c++0x
#C++11Flag = -std=gnu++0x
#C++11Flag = -std=c++11

pathtest= os.getcwd()
if pathtest[-1]!='/':
    pathtest+='/src/testcompile.cxx'
else:
    pathtest+='src/testcompile.cxx'        

command=["g++","-std=c++11",pathtest]
ostype = subprocess.call(command)
if ostype:
    command[1]="-std=c++0x"
    ostype = subprocess.call(command)
    if ostype:
        command[1]="-std=gnu++0x"
        ostype = subprocess.call(command)
        if ostype:
            print
            print
            print "Error: You need a compiler compatible with C++11"
            print
            sys.exit(-1)

f.write("\n# Compiler C++11 option\n")
f.write("C++11Flag = "+command[1]+"\n")


############################
regexflag="""
#REGEXFLAG = %%%
#REGEX=-DATANOR_REGEX $(REGEXFLAG)

"""

if gccversion:
    regexflag+="#LIBREGEX= -Lsystems/linux${GCCVERSION} -lboost_regex\n"
else:
    regexflag+="#LIBREGEX= -Lsystems -lboost_regex\n"

if "libboost_regex" not in v and noregex==False:    
    #we check now how to include regex... either #include "boost/regex.hpp" or <regex>
    pathtest= os.getcwd()
    if pathtest[-1]!='/':
        pathtest+='/src/testregex.cxx'
    else:
        pathtest+='src/testregex.cxx'

    systempath="-Lsystems"
    if gccversion:
        systempath+="/linux"+os.getenv("GCCVERSION")
        
    cmd=["g++","-o","testregex",command[1],"-DREGEXCPP",pathtest,systempath,"-lboost_regex"]
    
    ostype = subprocess.call(cmd)
    if ostype:
        if regexpath!="":
            cmd=["g++","-o","testregex",command[1],regexpath,pathtest,systempath,"-lboost_regex"]
        else:
            cmd=["g++","-o", "testregex",command[1],pathtest,systempath,"-lboost_regex"]
        ostype = subprocess.call(cmd)
        if ostype:
            noregex=True
        else:
            print
            print "Using boost::regex"
            if compilejava:
                fs = open("java/regexboost.txt","w")
                fs.write("REGEXBOOST")
                fs.close()
            if regexpath!="":
                regexflag=regexflag.replace("%%%",regexpath)            
    else:
        print
        print "Using std::regex"
        regexflag=regexflag.replace("%%%","-DREGEXCPP")
        if compilejava:
            fs = open("java/regexcpp.txt","w")
            fs.write("REGEXCPP")
            fs.close()
            fs = open("java/regexboost.txt","w")
            fs.write("REGEXBOOST")
            fs.close()

if noregex==False:
    os.system("rm testregex")
    regexflag=regexflag.replace("%%%","")
    regexflag=regexflag.replace("#","") 
else:
    print "Regex will not be available in atanor"
    noregex==False

f.write("# boost regex support")
f.write(regexflag)  
f.write("\n")

f.close()

print
print
print "All is ok... You can launch 'make all' now"
print






