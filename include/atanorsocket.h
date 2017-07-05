
/*
* Xerox Research Centre Europe - Grenoble Laboratory (XRCE)
*
* ATHANOR Language
* This file can only be used with the ATANOR library or the executable
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : ATHANOR
Version    : ATANOR 0.26
filename   : atanorsocket.h
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX (claude.roux@xrce.xerox.com)
Reviewer   :
*/

#ifndef atanorsocket_h
#define atanorsocket_h

#ifdef UNIX
#include <unistd.h>
#endif

#ifdef WIN32
#include <WinSock.h>
#define closesock closesocket
#define readsock(x,y,z) recv(x,y,z,0)
#define writesock(x,y,z) send(x,y,z,0)
#else
#include <sys/utsname.h>        /* for uname() */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#define SOCKET_ERROR -1
#define closesock close
#define readsock read
#define writesock write
#define SOCKET int
#endif

const int MAXHOSTNAME = 1000;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Atanorsocket;
//This typedef defines a type "socketMethod", which expose the typical parameters of a new KiF method implementation
typedef Atanor* (Atanorsocket::*socketMethod)(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Atanorsocket : public AtanorObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging hmap<unsigned short, socketMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
	

	struct sockaddr_in servAddr;
	int idclient;
	bool blocking;
	struct hostent *hp;
	char servername[MAXHOSTNAME + 10];
	int port;
	SOCKET sock;
	bool server;
	//For inserting a time out
	fd_set readfds;
	fd_set writefds;
	struct timeval tv;
	bool timeout;
	int v_timeout;

	map<SOCKET, bool> socketclients;
#ifdef WIN32
	int len;
#else
	socklen_t len;
#endif

	bool root;
    //---------------------------------------------------------------------------------------------------------------------
	Atanorsocket(AtanorGlobal* g, Atanor* parent = NULL);

	~Atanorsocket();

	void Methods(Atanor* v) {

		for (auto& it : infomethods)
			v->storevalue(it.first);
	}


	string Info(string n) {

		if (infomethods.find(n) != infomethods.end())
			return infomethods[n];
		return "Unknown method";
	}


    //----------------------------------------------------------------------------------------------------------------------
	Atanor* Put(Atanor* v, Atanor* i, short idthread);
    
	Atanor* Get(Atanor* context, Atanor* v, short idthread);


    short Type() {
        return Atanorsocket::idtype;
    }

    string Typename() {
        return "socket";
    }

    bool isString() {
        return false;
    }

    bool isNumber() {
        return false;
    }

    bool isBoolean() {
        return false;
    }

    bool isFrame() {
        return false;
    }

    Atanor* Atom(bool forced=false) {
        return this;
    }

	//If it is too complex to duplicate an element (for instance when passed to a function)
	//then you can use return false....
	bool duplicateForCall() {
		return false;
	}


    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in kifexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        if (exported.find(n) != exported.end())
            return true;
        return false;
    }

    Atanor* Newinstance(short, Atanor* f = NULL) {
        return new Atanorsocket(globalAtanor);
    }

    AtanorIteration* Newiteration(bool direction) {
        return aITERNULL;
    }

    static void AddMethod(AtanorGlobal* g, string name, socketMethod func, unsigned long arity, string infos);
    static bool InitialisationModule(AtanorGlobal* g, string version);

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
    //Atanor* MethodSize(AtanorGlobal* global,Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {return aZERO;}

	Atanor* MethodCreateServer(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodCreateClient(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodWait(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodRead(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodWrite(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodReadRaw(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodGet(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodWriteRaw(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodClose(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodBlock(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodTimeout(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodGethostname(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodPort(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* MethodGetpeername(Atanor* contextualpattern, short idthread, AtanorCall* callfunc);

    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a AtanorCallMethodMethod object
    //is returned by the Declaration method.
    Atanor* CallMethod(short idname, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods[idname])(contextualpattern, idthread, callfunc);
    }

	//---------------------------------------------------------------------------------------------------------------------

	void Close() {
		if (hp != NULL && sock != -1) {
			closesock(sock);

			for (auto& it : socketclients)
				closesock(it.first);
			socketclients.clear();
		}

		hp = NULL;
		sock = -1;
	}

	Atanor* Write(string act, Atanor* contextualpattern, short idthread, AtanorCall* callfunc);
	Atanor* CreateSocket(short);
	bool TestTimeOutRead(short idthread, SOCKET currentsock);
	bool TestTimeOutWrite(short idthread, SOCKET currentsock);


    void Clear() {
        //To set a variable back to empty
    }

    void Clean() {
        //Your cleaning code
    }

    string String() {
        Locking _lock(this);
        return "";
    }

    //wstring UString() {
    //   Locking _lock(this);
    //  return L"";
    //	}

    long Integer() {
        Locking _lock(this);
        return 0;
    }
    double Float() {
        Locking _lock(this);
        return 0;
    }
    BLONG Long() {
        Locking _lock(this);
        return 0;
    }

    bool Boolean() {
        Locking _lock(this);
        return false;
    }

    
    //Basic operations
    long Size() {
        Locking _lock(this);
        return 0;
    }
    
    Atanor* andset(Atanor* a, bool itself) {
        Locking _lock(this);
        return this;
    }

    Atanor* orset(Atanor* a, bool itself) {
        Locking _lock(this);
        return this;
    }

    Atanor* xorset(Atanor* a, bool itself) {
        Locking _lock(this);
        return this;
    }

    
    Atanor* plus(Atanor* a, bool itself) {
        Locking _lock(this);
        return this;
    }

    Atanor* minus(Atanor* a, bool itself) {
        Locking _lock(this);
        return this;
    }

    Atanor* multiply(Atanor* a, bool itself) {
        Locking _lock(this);
        return this;
    }

    Atanor* divide(Atanor* a, bool itself) {
        Locking _lock(this);
        return this;
    }
    Atanor* power(Atanor* a, bool itself) {
        Locking _lock(this);
        return this;
    }
    Atanor* shiftleft(Atanor* a, bool itself) {
        Locking _lock(this);
        return this;
    }
    Atanor* shiftright(Atanor* a, bool itself) {
        Locking _lock(this);
        return this;
    }
    Atanor* mod(Atanor* a, bool itself) {
        Locking _lock(this);
        return this;
    }
    
    Atanor* less(Atanor* a) {
        Locking _lock(this);
        return aFALSE;
    }
    
    Atanor* more(Atanor* a) {
        Locking _lock(this);
        return aFALSE;
    }

    Atanor* same(Atanor* a) {
        Locking _lock(this);
        if (a == this)
            return aTRUE;
        return aFALSE;
    }

    Atanor* different(Atanor* a) {
        Locking _lock(this);
        if (same(a) == aFALSE)
            return aTRUE;
        return aFALSE;
    }

    Atanor* lessequal(Atanor* a) {
        Locking _lock(this);
        return aFALSE;
    }
    
    Atanor* moreequal(Atanor* a) {
        Locking _lock(this);
        return aFALSE;
    }

};

#endif




