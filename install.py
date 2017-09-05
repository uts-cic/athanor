# The directories to check are: /usr/lib64, /usr/lib or /usr/lib/x86_64-linux-gnu/

import sys
import os
from os import walk
import subprocess

print "Test"

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
### ATANOR requires fltk. If it is not installed on your system, please install the necessary package
### In the case a library is only available with a mangled name such as libboost_regex.so.1.53,
### the installer will create a "systems" directory in which symbolic links to these libraries will be created...
###################################################################################################################

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
libpath="/usr"
if len(sys.argv)==2:
    libpath=sys.argv[1]
else: #we look for libgcc
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
if len(found)!=0:    
    for (dirpath, dirnames, filenames) in walk("."):
        if "systems" in dirnames:
            os.system("rm systems/*")
            os.system("rmdir systems")
            break
    
    os.system("mkdir systems")

for u in found:
    if ".a" not in found[u] and ".dylib" not in found[u]:
        command="ln -s "+libpath+found[u]+" systems/"+u+".so"
        os.system(command)
    

############################
f=open("Makefile.in","w")
f.write("BINPATH = bin/linux\n")
f.write("OBJPATH = objs/linux\n")
f.write("LIBOBJPATH = libobjs/linux\n")
f.write("SYSTEMSPATH = -Lsystems -Llibs/linux\n")
############################
if len(v)!=0:
    print "--------------------------------"
    print
    print "Missing libraries:", v
    if "libfltk" in v:
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
regexflag="""
#REGEX=-DATANOR_REGEX
#LIBREGEX=-lboost_regex
"""

if "libboost_regex" not in v:
    regexflag=regexflag.replace("#","") 
else:
    print "Regex will not be available in atanor"

f.write("# boost regex support")
f.write(regexflag)  
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

command=["gcc","-std=c++11",pathtest]
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

f.close()

print
print
print "All is ok... You can launch 'make all' now"
print



