###################################################################################################
#
# Produces a versioned athanor library for use by other applications 
# such as athanor-server
#
# 
# Versioning must happen when major feature changes are added 
# to athnor which may not be backward compatible. When that is done, the version_number.py 
# file which is maninpulated by this program, and the produced  
# versioned atanor_version.release.level.jar must be both checked into the repository
# where they can be later downloaded, and/or linked to by other programs
# such as athanor-server.
#
# Updated to an existing distribution jar can only be done using the verion_type 'fix' 
# option of this program. In this case, only the distribution jar needs to be checked 
# in if no new versioned target is produced. 
#
#
# The version_type 'fix' option is also useful to update a target that you already 
# produced but have not checked into the repository, but you need to reproduce it 
# because you discovered for example errors during testing.    
# 
# Using this scheme, we can keep the last two or three versioned copies 
# in the repository, and switch between them when necessary.   
#
# *** NOTE *** This script has only been tested on Linux so far, so it 
# assumes this versioning work will be performed from a Linux machine. 
#
# jh 1 Nov/2017
# 
###################################################################################################
import os
import shutil
import argparse
import glob

 
from version_number import version 
from version_number import release
from version_number import level

################################# Function Definitions ############################################

#
# Pase command line agruments 
# Invocation: python version_athanor.py version_type [--noclean]
#    where version_type = version || release || level || fix 
#
#    version and release and level options are for major and significant 
#    athanor library changes are per conventional use of the terms. 
#    
#    fix changes update the existing versioned library jar and do not create a new version.  
# 
#    noclean is specified to skip the time consuming clean build. 
#    Should normally only be used to test or get familiar with this program.
#     
#
def parse_arguments(): 
    parser = argparse.ArgumentParser(description='Validate Input arguments.')

    parser.add_argument("version_type",
                        choices=("version","release","level","fix"),
                        help="specify the versioning type:version, release, level, or fix")
 
    parser.add_argument("-n", "--noclean",
                         action="store_false",
                         dest="clean", default=True,
                         help="don't perform time consuming make clean build")

    args = parser.parse_args()
    return args 

#
# Calculate the next version number depending on the contents of the version file
# and the versioning type request 
#
def calculate_next_level(version_type, version, release, level):

    if (version_type == 'version'): 
        next_version = version + 1 
        next_release = 0 
        next_level = 0 
    elif (version_type == 'release'): 
        next_version = version 
        next_release = release + 1 
        next_level = 0 
    elif (version_type == 'level'): 
        next_version = version 
        next_release = release 
        next_level = level + 1
    else:  
        next_version = version 
        next_release = release 
        next_level = level
   
    return (next_version, next_release, next_level)  

#
# Knows where to find the the source jar  
# 
def get_source_jar_name():
    return "java/dist/jatanor.jar" 

#
# Knows how to form and get the versioned jar name 
# 
def get_versioned_jar_name(version, release, level):
    
    target_dir = "java/versioned_dist/"
    versioned_jar = target_dir + "jatanor_" + str(version) + "." + str(release) + "." + str(level) + ".jar"
    return versioned_jar  

#
# for fix maintenance we must have a matching distribution jar 
# otherwise for development releases, we are creating a new distribution jar and 
# we shouldn't have a distribution that matches what we need 
# to create.
# This check should only fail if something was not done right 
# previously, such as: version_number.py was not checked in 
# to the repository when a versioned jar was checked in,
# or the latest versioned jar was deleted, or an error in the 
# logic or previous functioning of this program.  
#
def check_version_file_against_jars(version_type, version, release, level): 
    target_dir = "java/versioned_dist/"
    versioned_jar = get_versioned_jar_name(version, release, level)
    if (version_type == 'fix'):   
        if (not os.path.exists(versioned_jar)): 
            print "Error: versioned jar:",versioned_jar," does not exist. Version number mismatch." 
            print "You may need to do one of the following actions:"
            print "  1) find and download the missing version"
            print "  2) Manually update the number in version_number.py to match an existing version"
            print "  3) specify a different versioning action"
            return False
    else: 
        if (os.path.exists(versioned_jar)): 
            print "Error: versioned jar:",versioned_jar," already exists. We cannot create this version." 
            print "You may need to do one of the following :" 
            print " 1) delete the existing version" 
            print " 2) manually update the numbers in version_number.py" 
            print " 3) specify a different versioning action"
            return False
    return True 

def do_build(clean): 

#
# This is needed on the first invocation to configure the build. 
#
    os.system("python install.py") 

#
# This is needed on the first invocation in case object directories
# are not yet set up. 
# 
    os.system("make install") 
#
# This is needed in case we have no distributed versions yet
#
    os.system("mkdir -p java/versioned_dist/") 

    if (clean):
        
       retval = os.system("make clean")
       if (retval != 0): 
           print "Failed to make clean build"
           return retval

       jar_files = glob.glob("java/dist/*.jar") 
       for file in jar_files: 
           os.remove(file)

       class_files = glob.glob("java/classes/com/xerox/jatanor/*.class") 
       for file in class_files: 
           os.remove(file) 

       cpp_jni_files = glob.glob("java/objs/linux/*.o") 
       for file in cpp_jni_files: 
           os.remove(file) 

    
    retval = os.system("make libatanor")
    if (retval != 0):
        print "Failed to build libatanor" 
        return retval 

    os.chdir("java")
    retval = os.system("ant compile")
    if (retval != 0): 
        print "Failed to compile java classes"
        return retval  
 
    os.chdir("..")
   
    return retval 

#
# This produces the versioned jar file that we can test and check into the 
# git repository
#
def update_target_jar(version, release, level):
    source_jar = get_source_jar_name()
    versioned_jar = get_versioned_jar_name(version, release, level)
    shutil.copyfile(source_jar, versioned_jar)
    print versioned_jar," has been created/updated"  

#
# Update version file
# to be in sync with the versioned jar that we just made.  
#    

def update_version_file(version_type, version, release, level): 
    
    # No need to update for fix, as no new jar has been produced.  
    if (version_type == 'fix'): 
        return 

    with open('./version_number.py', 'r') as content_file:
        content = content_file.readlines()
 
    outlines = ""
    for line in content:  
        if (line.startswith('#')): 
            outlines = outlines + line 

    with open('/tmp/version_number.py', 'w') as content_file: 
        for line in outlines:
            content_file.write(line)
        line1 = "version = " + str(next_version) + "\n"
        content_file.write(line1)
        line2 = "release = " + str(release) + "\n"
        content_file.write(line2)
        line3 = "level = " + str(level) 
        content_file.write(line3) 
    
    shutil.copyfile("/tmp/version_number.py", "./version_number.py") 
    print("version_number.py has been updated") 

######################### Main Program ############################################################

args = parse_arguments()  

next_version, next_release, next_level = calculate_next_level(args.version_type, version, release, level)

if (not check_version_file_against_jars(args.version_type, next_version, next_release, next_level)): 
    exit() 

if (do_build(args.clean) != 0): 
    exit() 

update_target_jar(next_version, next_release, next_level) 

update_version_file(args.version_type, next_version, next_release, next_level) 

