# Athanor README

A rhetorical parsing library

## Quick install 

1. Install/Tailor the environment (you only need to do this once) 

In the athanor home directory type: 

```
python install.py
```

You may need to install Python 2.7 if it is not already on your 
system. 

Ignore warning messages about missing packages, as long as the 
following step works. 
We don't need these packages just to produce the athanor 
library. You do however need the g++ compiler, make and 
ant utilities that are used in building the product. 

2. Build Athanor and the Athanor library

Again, position in the Athanor repository home directory: type 

```
make
```
 
The athanor library will be in 
    java/dist/jatanor.jar

This jar can then be linked to or copied to Athanor-Server where 
it can be invoked. 

The versioned distributions of Athanor in java/versioned_dist/ 
can also be used for this purpose. 

For more details on tailoring Athanor-Server to work with a given
Athanor jar, refer to the Athanor-Server repository Developer
documentation (Interfacing with Athanor Section).

For more information on using Athanor, building Athanor components, 
or building versioned athanor library distributions see 
[The Athanor repository](athanor_repository.md)


