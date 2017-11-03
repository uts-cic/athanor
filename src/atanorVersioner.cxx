#include "versionerAtanorVersion.h"
#include <vector>
#include <iostream>
#include <fstream> 
#include <sstream>

//
// Update the version_number.py file used in creating versioned jars
// with the version value of the cpp atanor library. 
//
// This is a more reliable way of producing this version number 
// than for the python utility to directly read the cpp code. 
//
// *** NOTE *** 
//
// If you modify this program to change its version_number.py 
// output, you need to also invoke and test the version_athanor.py 
// utility. 
// 
	
int main() 
{
    const std::string python_version_file = "version_number.py"; 
 
    std::ifstream fin(python_version_file);
    
    if (!fin) 
    {
        std::cout << "atanorVersioner: failed to open:" << python_version_file << "for read\n"; 
        return 1;
    }
 

    std::string atanor_cpp_version_definition = "atanor_cpp_version = "; 

    std::vector<std::string> lines;    
    std::string line; 
    while (std::getline(fin, line)) 
    {
        // Do not read in the string we are trying to generate
        if (line.find(atanor_cpp_version_definition) == std::string::npos)
            lines.push_back(line);
    }
    fin.close();   

    std::stringstream buff;
    buff << atanor_cpp_version; 
    std::string atanor_cpp_version_string=buff.str();  

    line = atanor_cpp_version_definition  + atanor_cpp_version_string; 
    lines.push_back(line); 

    std::ofstream fout(python_version_file);
    if (!fout) 
    {
        std::cout << "atanorVersioner: failed to open:" << python_version_file << "for write\n";
        return 1;
    }

    for (auto line:lines) 
        fout << line << "\n"; 
    fout.close(); 

    std::cout << python_version_file << " has been updated\n";
    return 0;
} 
