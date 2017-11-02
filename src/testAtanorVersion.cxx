#include "atanor.h"
#include "atanorversion.h"
#include "globalatanor.h"
#include "versionerAtanorVersion.h"
#include <sstream> 
#include <string> 

int main()
{
 
    //
    // This is a simple test 
    // just to test the version output has not changed 
    // by the jar versioning work done when writing the 
    // version_athanor python utility
    //
    std::stringstream expectedVersionStream;   
    expectedVersionStream << "ATANOR " << atanor_cpp_version << " build " << atanor_build_number;   
    std::string expectedVersion = expectedVersionStream.str(); 
    std::string receivedVersion = std::string(AtanorVersion()); 
    if (expectedVersion != receivedVersion) 
    {
        cout << "failed expected :'" <<  expectedVersion << "'\n"; 
        cout << "       received :'" <<  receivedVersion << "'\n"; 
    }
    else 
    {
        cout << "Testcase successful" << endl;
    }
} 
