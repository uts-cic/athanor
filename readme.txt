------------------------------
Athanor archive
------------------------------

This archive contains all the makefiles to compile versions for the following platforms: windows, MacOs, Linux.

For Windows, we provide: atanor.sln, which is a Visual Studio 2013 Makefile.

The archive contains:

include/					the main include files
include/libs/
include/libxml/
include/linux/
include/linux/FL/
include/linux/ao/
include/linux/curl/
include/macos/
include/macos/ao/
include/macos/fltk/
include/macos/fltk/FL/
include/macos/fltk/src/
include/macos/fltk/src/xutf8/
include/macos/fltk/src/xutf8/MAPPINGS/
include/macos/fltk/src/xutf8/headers/
include/macos/fltk/src/xutf8/lcUniConv/
include/macos/fltk/src/xutf8/utils/
include/windows/
include/windows/FL/
include/windows/ao/
include/windows/curl/

src/
src/java/
src/java/cxx/
src/java/java/
src/java/java/com/
src/java/java/com/xerox/
src/java/java/com/xerox/jatanor/
src/libs/


scripts/dependencies/				This is where the grammar is. See Description.txt for more information.

scripts/
scripts/GenerationProlog/
scripts/Predicates/
scripts/haskell/
scripts/music/
scripts/python/

java/
java/include/
java/src/
java/src/cxx/
java/src/java/
java/src/java/com/
java/src/java/com/xerox/
java/src/java/com/xerox/jatanor/
java/test/
java/test/build/


------------------------------
Installing Athanor
------------------------------

To compile the Athanor library, you need first to launch:

python install.py

This will generate a Makefile.in

Then do: make libatanor atanor

------------------------------
Compiling Java
------------------------------

You need to have "ant" installed.

First mgo to the java directory.

Launch: ant compile

(The source code is in: src/java)

Once the library is compiled, you can test it, in the test directory within the java directory.

You can launch: ant (no arguments), to test it.

The test file is called: Test.java.
It shows how to load an athanor program, with LoadProgram and how to execute a function from within the athanor file with ExecuteFunction.

------------------------------
Executing the grammars:
------------------------------

First, we use: stanford-corenlp-full-2015-12-09 as input to our system...

To launch the grammars, go to scripts/dependencies.

Launch: atan analyse.kif context.xml

You can provide the program with an argument (the name of the xml file to process). However, if you do not provide any filenames, then the analyzer will automatically process: context.xml.

- context.xml is the result of the Stanford parser on context.txt.
- context.txt contains sentences that the grammar tags as context (MOVE).

Description.txt presents a description of all grammar files.

-------

You can also launch the athanor console to examine the file content:

atan -console analyse.kif

From within which, you can execute the code.

Note that the console is only available if the FLTK library is installed on your system.

For Windows and MacOS, these libraries are provided with this archive.

