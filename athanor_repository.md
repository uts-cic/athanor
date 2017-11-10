# Athanor Repository 

rhetorical parsing library

See also the [README file](README.md) for information 
on how to build the Athanor library.

## Athanor archive


This archive contains all the makefiles to compile versions for the following platforms: windows, MacOs, Linux.

For Windows, we provide: atanor.sln, which is a Visual Studio 2013 Makefile.

The archive contains:
```bash
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
```

## Installing Athanor


To compile the Athanor library, you need first to launch:

```
python install.py
```
This will generate a `Makefile.in`

Then do: 

```
make libatanor atanor
```

## Compiling Java

Note: You only need to do this step if you want to test the 
java build separately as typing make to compile the athanor
library in the previous step should automatically invoke this step. 
 
You need to have "ant" installed.

First mgo to the java directory.

Launch: `ant compile`

(The source code is in: src/java)

Once the library is compiled, you can test it, in the test directory within the java directory.

You can launch: `ant` (no arguments), to test it.

The test file is called: Test.java.
It shows how to load an athanor program, with LoadProgram and how to execute a function from within the athanor file with ExecuteFunction.


## Executing the grammars:


First, we use: `stanford-corenlp-full-2015-12-09` as input to our system...

To launch the grammars, go to `scripts/dependencies`.

Launch: `atan analyse.kif context.xml`

You can provide the program with an argument (the name of the xml file to process). However, if you do not provide any filenames, then the analyzer will automatically process: `context.xml`.

- `context.xml` is the result of the Stanford parser on `context.txt`.
- `context.txt` contains sentences that the grammar tags as context (MOVE).

`Description.txt` presents a description of all grammar files.

-------

You can also launch the athanor console to examine the file content:

```
atan -console analyse.kif
```

From within which, you can execute the code.

Note that the console is only available if the `FLTK` library is installed on your system.

For Windows and MacOS, these libraries are provided with this archive.

## Building versioned Athanor distributions 

This should be done regularly whenever there are important maintenance or development 
fixes to the Athanor code.
These versioned distributions can then be used by Athanor-server. 

For more information, see the documentation in version_athanor.py and/or type: 

```
python version_athanor.py -h
```

In the lib directory we can link to the athanor version jar that we want athanor server 
to default to.  
Ensure that you create a relative link as the names of the folders will change when 
sbt downloads athanor as a dependency of athanor-server. For example, to select the 
athanor.087b-1.0.0, position in the athanor lib directory and type:
 
ln -s ../java/versioned_dist/athanor-0.87b-1.0.0.jar athanor.jar
