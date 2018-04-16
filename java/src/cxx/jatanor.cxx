/*
*  NAVER LABS Europe
*  Copyright NAVER LABS
*  All Rights Reserved
*
*/

/**
* Native implementation for Java API to access the ATANOR C++ API.
*/

#define MULTIGLOBALATANOR

#include "jni.h"
#include "com_xerox_jatanor_JAtanor.h"

#include "atanor.h"
#include "math.h"
#include "globalatanor.h"
#include "compilecode.h"
#include "atanorstring.h"

static ThreadLock jLock(NULL, false);

class JatanorLocking {
private:
	std::lock_guard<std::recursive_mutex>* g;

public:
	JatanorLocking() {
		g = new std::lock_guard<std::recursive_mutex>(*jLock.lock);
	}

	~JatanorLocking() {
		if (g != NULL)
			delete g;
	}
};


/**
* Converts the built-in Unicode representation of a java string into a UTF-8 c++ string
* Take care if the java input string contains non ASCII characters,
    * utf8 represents them with more than one byte.
* if the input string contains only ASCII characters, the output UTF-8 string
*  is the same than the input string.
* @param str the java string
* @return the utf8 c++ string
*/
string jstringToString(JNIEnv* env, jstring str) {
    /*
    * The GetStringUTFChars function retrieves 8-bit characters from a 16-bit jstring
    * using the Unicode Transformation Format (UTF). UTF represents Unicode as a string
    * of 8 or 16 bit characters without losing any information.
    * The third parameter GetStringUTFChars results the result JNI_TRUE if it made
    * a local copy of the jstring or JNI_FALSE otherwise.
    */
    jboolean iscopy;
    const char* strChars = env->GetStringUTFChars(str, &iscopy);
    string result = string(strChars);
    //cout <<"jstsringToString result = " <<result <<endl;
    // now the the JVM can free the memory used by jstring str
    env->ReleaseStringUTFChars(str, strChars);
    return result;
}

/**
* Constructs a new java.lang.String object from an array of UTF-8 characters.
* IMPORTANT NOTE: to avoid memory leak use deleteLocalJStringRef() to ask JVM to free memeory
*
* @param the pointer to a utf8 c++ string
* @return the java string
*
*/
//VECTA<jstring> listjstring;

jstring jstringFromChars(JNIEnv* env, const char *chars) {
    jstring jstr = env->NewStringUTF(chars);
    //    listjstring.ajoute(jstr);
    return jstr;
}

/**
* converts a c++ string to a java string
* IMPORTANT NOTE: to avoid memory leak use deleteLocalJStringRef() to ask JVM to free memeory
* @param the utf8 c++ string
* @return the java string
*
*/
jstring jstringFromString(JNIEnv* env, string& str) {
    return jstringFromChars(env, str.c_str());
}

/**
* Execute and load an ATANOR program
* @param filename is the ATANOR program pathname
* @param args is a string in which each argument is seperated from the others with a space
*/
JNIEXPORT jint JNICALL Java_com_xerox_jatanor_JAtanor_LoadProgramImplementation(JNIEnv *env, jobject obj, jstring filename, jstring args) {

    string nameOfFile = jstringToString(env, filename);
    string theargs = jstringToString(env, args);
	int idglobal = AtanorCreateGlobal(1000);
    AtanorSetArguments(theargs);
    short idcode = -1;
    try {
        idcode = AtanorLoad(nameOfFile);
        if (idcode != -1) {
            if (!AtanorLoading(idcode)) {
                cerr << AtanorErrorMessage() << endl;
                return -1;
            }
        }
    }
    catch (AtanorRaiseError* m) {
        cerr << m->message << " in " << m->filename << " at line: " << m->left << endl;
        return -1;
    }

    return idglobal;
}

/**
* Execute and load an ATANOR function
* @param funcname is the ATANOR function name
* @param parameters is a vector of string, each corresponding to a parameter of the function...
*/
JNIEXPORT jstring JNICALL Java_com_xerox_jatanor_JAtanor_ExecuteFunctionImplementation(JNIEnv *env, jobject obj, jint handler, jstring funcname, jobjectArray parameters) {

    string nameOfFunction = jstringToString(env, funcname);
    vector<Atanor*> params;
    int stringCount = env->GetArrayLength(parameters);
    string value;
    jstring element;
    bool theerror = false;

    for (int i = 0; i < stringCount; i++) {
        element = (jstring)env->GetObjectArrayElement(parameters, i);
        value = jstringToString(env, element);
        params.push_back(new Atanorstring(value));
    }

	//JatanorLocking _lock;
	if (!AtanorSelectglobal(handler)) {
		value = "This handler does not match an existing ATANOR space";
		element = jstringFromString(env, value);
		return element;
	}

    AtanorCode* atancode = AtanorCodeSpace(0);
    if (atancode == NULL) {
        value = "This handler does not match an existing ATANOR space";
        element = jstringFromString(env, value);
        return element;
    }

    Atanor* resultat = AtanorExecute(atancode, nameOfFunction, params);
	
    if (globalAtanor->Error(0)) {
        value = globalAtanor->Errorstring(0);
        element = jstringFromString(env, value);
		AtanorReleaseglobal(handler);
        return element;
    }

	AtanorReleaseglobal(handler);
	
    value = resultat->String();
    resultat->Resetreference();
    element = jstringFromString(env, value);
    return element;
}

/**
* Execute and load an ATANOR function
* @param funcname is the ATANOR function name
* @param parameters is a vector of string, each corresponding to a parameter of the function...
*/
JNIEXPORT jobjectArray JNICALL Java_com_xerox_jatanor_JAtanor_ExecuteFunctionArrayImplementation(JNIEnv *env, jobject obj, jint handler, jstring funcname, jobjectArray parameters) {

    string nameOfFunction = jstringToString(env, funcname);
    vector<Atanor*> params;
    int stringCount = env->GetArrayLength(parameters);
    string value;
    jstring element;
    bool theerror = false;

    for (int i = 0; i < stringCount; i++) {
        element = (jstring)env->GetObjectArrayElement(parameters, i);
        value = jstringToString(env, element);
        params.push_back(new Atanorstring(value));
    }

    jobjectArray ret;
	//JatanorLocking _lock;

	if (!AtanorSelectglobal(handler)) {
		value = "This handler does not match an existing ATANOR space";
		element = jstringFromString(env, value);
		ret = (jobjectArray)env->NewObjectArray(1, env->FindClass("java/lang/String"), env->NewStringUTF(""));
		env->SetObjectArrayElement(ret, 0, element);
		env->DeleteLocalRef(element);
		return ret;
	}

	AtanorCode* atancode = AtanorCodeSpace(0);
    if (atancode == NULL) {
        value = "This handler does not match an existing ATANOR space";
        element = jstringFromString(env, value);
        ret = (jobjectArray)env->NewObjectArray(1, env->FindClass("java/lang/String"), env->NewStringUTF(""));
        env->SetObjectArrayElement(ret, 0, element);
        env->DeleteLocalRef(element);
        return ret;
    }

    Atanor* resultat = AtanorExecute(atancode, nameOfFunction, params);
	
    if (globalAtanor->Error(0)) {
        value = globalAtanor->Errorstring(0);
        element = jstringFromString(env, value);
        ret = (jobjectArray)env->NewObjectArray(1, env->FindClass("java/lang/String"), env->NewStringUTF(""));
        env->SetObjectArrayElement(ret, 0, element);
        env->DeleteLocalRef(element);
		AtanorReleaseglobal(handler);
        return ret;
    }

	AtanorReleaseglobal(handler);
	
    if (resultat->isVectorContainer()) {
        ret = (jobjectArray)env->NewObjectArray(resultat->Size(), env->FindClass("java/lang/String"), env->NewStringUTF(""));
        for (long i = 0; i < resultat->Size(); i++) {
            value = resultat->getstring(i);
            element = jstringFromString(env, value);
            env->SetObjectArrayElement(ret, i, element);
            env->DeleteLocalRef(element);
        }
    }

    resultat->Resetreference();
    return ret;
}

/**
* Clean memory for all AtanorGlobal Objects
*/
JNIEXPORT jint JNICALL Java_com_xerox_jatanor_JAtanor_CleanAllImplementation(JNIEnv *env, jobject obj) {
	AtanorCleanAllGlobals();
	return 1;
}


/**
* Clean memory for a Global
*/
JNIEXPORT jint JNICALL Java_com_xerox_jatanor_JAtanor_CleanImplementation(JNIEnv *env, jobject obj, jint handler) {
	if (AtanorDeleteGlobal(handler))
		return 1;
	return 0;
}
