
/*
*  Athanor: Le langage des Alpages mède à Grenoble
*
*  Copyright (C) 2017: ATHANOR Language
* This file can only be used with the ATHANOR library or the executable
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : ATHANOR
Version    : ATANOR 0.26
filename   : atanorsocket.cxx
Date       : 2017/04/01
Purpose    : 
Programmer : Claude ROUX
Reviewer   :
*/

#include "atanor.h"
#include "atanormap.h"
#include "atanorbyte.h"
#include "atanorsocket.h"

const int MAXSIZEINPUT = 256;
const int POSSIGNATURE = 5;
const char* FORMATSIGNATURE = "%4x";

static vector<Atanorsocket*> sockets;
static vector<long> isockets;

#ifdef WIN32
#define BIND ::bind
#else
#define BIND bind
#endif

string Msgerror() {
	char* s = strerror(errno);
	if (strstr(s, "No error") != NULL)
		return "";
	return s;
}


static short XConvert(char* number, int nb) {
	unsigned short v = 0;
	for (int i = 0; i<nb; i++) {
		switch (number[i]) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			v <<= 4;
			v |= number[i] - 48;
			break;
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
			v <<= 4;
			v |= number[i] - 55;
			break;
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
			v <<= 4;
			v |= number[i] - 87;
		}
	}
	return v;
}

static bool checkipaddres(string serveraddr) {
	int countpoint = 0;
	for (int i = 0; i<serveraddr.size(); i++) {
		if (serveraddr[i] == '.') {
			countpoint++;
			if (countpoint>3)
				return false;
		}
		else
		if (serveraddr[i]<48 || serveraddr[i]>57)
			return false;
	}
	return true;
}

static bool validstream(int nb) {
	if (nb == SOCKET_ERROR || nb == 0)
		return false;
	return true;
}


//----------------------------------------------------------------------------------------------
//We need to declare once again our local definitions.
Exporting hmap<unsigned short, socketMethod>  Atanorsocket::methods;
Exporting hmap<string, string> Atanorsocket::infomethods;
Exporting bin_hash<unsigned long> Atanorsocket::exported;

Exporting short Atanorsocket::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Atanorsocket::AddMethod(AtanorGlobal* global, string name, socketMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

#ifdef WIN32
static WSADATA WSAData;
static bool wsainit = true;
#endif

bool Atanorsocket::InitialisationModule(AtanorGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();
	sockets.clear();
	isockets.clear();

	Atanorsocket::idtype = global->Getid("socket");

	Atanorsocket::AddMethod(global, "createserver", &Atanorsocket::MethodCreateServer, P_THREE | P_TWO, "createserver(string hostnameint portint nblients: create a server if hostname is omitted then the local hostname is used");
	Atanorsocket::AddMethod(global, "connect", &Atanorsocket::MethodCreateClient, P_TWO, "connect(string hostnameint port): connect to the server");
	Atanorsocket::AddMethod(global, "wait", &Atanorsocket::MethodWait, P_NONE, "wait(): wait for a client to connect and returns its socket id");
	Atanorsocket::AddMethod(global, "read", &Atanorsocket::MethodRead, P_ONE | P_NONE, "read(int num): read a string on a socket. On the server side 'num' is the value returned by 'wait()'. Use 'read()' on the client side");
	Atanorsocket::AddMethod(global, "write", &Atanorsocket::MethodWrite, P_ATLEASTONE, "write(int numstring s): write the string s on the socket. On the server side num is the value returned by wait()'. Use 'write(string s)' on the client side");
	Atanorsocket::AddMethod(global, "receive", &Atanorsocket::MethodReadRaw, P_TWO | P_ONE | P_NONE, "receive(int numint nb): read a string on a socket in a non KiF environment. On the server side 'num' is the value returned by 'wait()'. Use 'receive()' on the client side");
	Atanorsocket::AddMethod(global, "get", &Atanorsocket::MethodGet, P_ONE | P_NONE, "get(int num): get one character from a socket in a non KiF environment. On the server side 'num' is the value returned by 'wait()'. Use 'get()' on the client side");
	Atanorsocket::AddMethod(global, "send", &Atanorsocket::MethodWriteRaw, P_ATLEASTONE, "send(int numstring s): write the string s on the socket in a non KiF environment. On the server side num is the value returned by wait()'. Use 'send(string s)' on the client side");
	Atanorsocket::AddMethod(global, "close", &Atanorsocket::MethodClose, P_NONE | P_ONE, "close(int num): Close a socket. On the server side if 'num' is provided (it is the value returned by wait()) it closes the client socket otherwise it closes the current socket.");
	Atanorsocket::AddMethod(global, "blocking", &Atanorsocket::MethodBlock, P_ONE, "blocking(bool flag): if 'flag' is true the socket works in 'blocking' mode otherwise in 'non blocking' mode");
	Atanorsocket::AddMethod(global, "settimeout", &Atanorsocket::MethodTimeout, P_ONE, "settimeout(int t): Set a time out of 't' seconds on the socket");
	Atanorsocket::AddMethod(global, "gethostname", &Atanorsocket::MethodGethostname, P_NONE, "gethostname(): return the current host name");
	Atanorsocket::AddMethod(global, "port", &Atanorsocket::MethodPort, P_NONE, "port(): return the current port number");
	Atanorsocket::AddMethod(global, "getpeername", &Atanorsocket::MethodGetpeername, P_ONE, "getpeername(int num): return the current peer name on the socket 'num'");

	Atanorsocket* s = new Atanorsocket(global);
	s->root = true;
	global->newInstance[Atanorsocket::idtype] = s;
	global->RecordMethods(Atanorsocket::idtype,Atanorsocket::exported);

#ifdef WIN32
	if (wsainit) {
		WSAStartup(MAKEWORD(2, 0), &WSAData);
		wsainit = false;
	}
#endif

	return true;
}

Atanorsocket::Atanorsocket(AtanorGlobal* g, Atanor* parent) : AtanorObject(g, parent) {
	
	root = false;
	//Do not forget your variable initialisation
	hp = NULL;
	sock = -1;
	idclient = 0;
	blocking = true;
	timeout = false;
	g->RecordInTrackerProtected(this);
	sockets.push_back(this);
	isockets.push_back(idtracker);
}


Atanorsocket::~Atanorsocket() {
	if (root) {
#ifdef WIN32
		WSACleanup();
		wsainit = true;
#endif
		for (size_t i = 0; i < isockets.size(); i++) {
			if (globalAtanor->Gettracked(isockets[i]) == sockets[i])
				sockets[i]->Close();
		}

		sockets.clear();
		isockets.clear();

	}
}

Atanor* Atanorsocket::Put(Atanor* index, Atanor* value, short idthread) {
	return this;
}

Atanor* Atanorsocket::Get(Atanor* context, Atanor* index, short idthread) {
	return this;
}


Atanor* Atanorsocket::MethodCreateServer(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//AtanorThreadLock _lock(general);
	//In our example, we have only two parameters
	if (hp != NULL)
		return globalAtanor->Returnerror("SOCKET(831): Server already launched on this socket", idthread);
	//0 is the first parameter and so on...
	Atanor* kport;
	Atanor* knbclients;
	if (callfunc->Size() == 2) {
		kport = callfunc->Evaluate(0, aNULL, idthread);
		gethostname(servername, MAXHOSTNAME);
		knbclients = callfunc->Evaluate(1, aNULL, idthread);
	}
	else {
		Atanor* kserver = callfunc->Evaluate(0, aNULL, idthread);
		string s = kserver->String();
		if (s.size() >= MAXHOSTNAME)
			return globalAtanor->Returnerror("SOCKET(850): Wrong server name", idthread);
		strcpy(servername, (char*)s.c_str());
		kport = callfunc->Evaluate(1, aNULL, idthread);
		knbclients = callfunc->Evaluate(2, aNULL, idthread);
	}
	int nbclients = knbclients->Integer();
	port = kport->Integer();
	Atanor* ret = CreateSocket(idthread);
	if (ret != aTRUE)
		return ret;
#ifndef WIN32
	int yes = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		string errmessage = "SOCKET(872): Error on reuse addess";
		errmessage += Msgerror();
		return globalAtanor->Returnerror(errmessage, idthread);
	}
#endif
	servAddr.sin_addr.s_addr = INADDR_ANY;
	if (BIND(sock, (struct sockaddr*)&servAddr, len) < 0) {
		string errmessage = "SOCKET(853): Error on bind ";
		errmessage += Msgerror();
		return globalAtanor->Returnerror(errmessage, idthread);
	}
	if (listen(sock, nbclients) < 0) {
		string errmessage = "SOCKET(854): Error on listen ";
		errmessage += Msgerror();
		return globalAtanor->Returnerror(errmessage, idthread);
	}
	server = true;
	return aTRUE;
}

Atanor* Atanorsocket::MethodCreateClient(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//AtanorThreadLock _lock(general);
	//In our example, we have only two parameters
	if (hp != NULL)
		return globalAtanor->Returnerror("SOCKET(831): Server already launched on this socket", idthread);
	//0 is the first parameter and so on...
	Atanor* kserver = callfunc->Evaluate(0, aNULL, idthread);
	Atanor* kport = callfunc->Evaluate(1, aNULL, idthread);
	port = kport->Integer();
	strcpy(servername, (char*)kserver->String().c_str());
	Atanor* ret = CreateSocket(idthread);
	if (ret != aTRUE)
		return ret;
	server = false;
	if (connect(sock, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
		string errmessage = "SOCKET(857): Error on connection ";
		errmessage += Msgerror();
		return globalAtanor->Returnerror(errmessage, idthread);
	}
	return aTRUE;
}

Atanor* Atanorsocket::MethodWait(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//AtanorThreadLock _lock(waiting);
	struct sockaddr_in cliAddr;
	SOCKET socketclient = -1;
	if (server == true) {
		if ((socketclient = accept(sock, (struct sockaddr*)&cliAddr, &len)) < 0) {
			string errmessage = "SOCKET(855): Error on read ";
			errmessage += Msgerror();
			return globalAtanor->Returnerror(errmessage, idthread);
		}

		if (globalAtanor->Error(idthread) == true)
			return globalAtanor->Errorobject(idthread);

		socketclients[socketclient] = true;
		return globalAtanor->Provideint(socketclient);
	}
	return aMINUSONE;
}

Atanor* Atanorsocket::MethodRead(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	short ssz;
	string res;
	SOCKET currentsock;
	int socketclient;
	if (server == true) {
		if (callfunc == NULL || callfunc->Size() == 0)
			return globalAtanor->Returnerror("SOCKET(820): Missing parameter in read", idthread);
		socketclient = callfunc->Evaluate(0, aNULL, idthread)->Integer();
		if (socketclient == SOCKET_ERROR)
			return globalAtanor->Returnerror("SOCKET(824): Non connected socket", idthread);
		currentsock = socketclient;
	}
	else
		currentsock = sock;
	if (currentsock == SOCKET_ERROR)
		return globalAtanor->Returnerror("SOCKET(858): No client connected", idthread);
	char inputstr[MAXSIZEINPUT + 1];
	int nbcharread = 0;
	int nbloc;
	while (nbcharread < POSSIGNATURE) {
		if (TestTimeOutRead(idthread, currentsock) == false)
			return aNULL;
		nbloc = readsock(currentsock, inputstr + nbcharread, POSSIGNATURE - nbcharread);
		if (validstream(nbloc) == false) {
			string errmessage = "SOCKET(861): Error on read";
			errmessage += Msgerror();
			return globalAtanor->Returnerror(errmessage, idthread);
		}
		nbcharread += nbloc;
	}
	inputstr[POSSIGNATURE] = 0;
	ssz = XConvert(inputstr + 1, POSSIGNATURE - 1);
	int maxtoread;
	//cout<<"Reading:"<<ssz<<":"<<inputstr<<endl;
	while (ssz > 0) {
		inputstr[0] = 0;
		if (TestTimeOutRead(idthread, currentsock) == false)
			return aNULL;
		maxtoread = ssz;
		if (maxtoread > MAXSIZEINPUT)
			maxtoread = MAXSIZEINPUT;
		nbcharread = readsock(currentsock, inputstr, maxtoread);
		if (validstream(nbcharread) == false) {
			string errmessage = "SOCKET(861): Error on read";
			errmessage += Msgerror();
			return globalAtanor->Returnerror(errmessage, idthread);
		}
		inputstr[nbcharread] = 0;
		ssz -= nbcharread;
		res += inputstr;
	}
	return globalAtanor->Providestring(res);
}

Atanor* Atanorsocket::MethodWrite(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	return Write("N", contextualpattern, idthread, callfunc);
}

Atanor* Atanorsocket::MethodReadRaw(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	string res;
	int nb = -1;
	SOCKET currentsock;
	//AtanorThreadLock _lock(reading);
	int nbbytes = MAXSIZEINPUT;
	bool decrement = false;
	if (server == true) {
		if (callfunc->Size() == 0)
			return globalAtanor->Returnerror("SOCKET(821): Missing parameter in receive", idthread);
		currentsock = callfunc->Evaluate(0, aNULL, idthread)->Integer();
		if (callfunc->Size() == 2) {
			nbbytes = callfunc->Evaluate(1, aNULL, idthread)->Integer();
			decrement = true;
		}
		if (currentsock == SOCKET_ERROR)
			return globalAtanor->Returnerror("SOCKET(824): Non connected socket", idthread);
	}
	else {
		if (callfunc->Size() == 2)
			return globalAtanor->Returnerror("SOCKET(823): Too many parameter in receive", idthread);
		currentsock = sock;
		if (callfunc->Size() == 1) {
			nbbytes = callfunc->Evaluate(0, aNULL, idthread)->Integer();
			decrement = true;
		}
	}
	if (currentsock == SOCKET_ERROR)
		return globalAtanor->Returnerror("SOCKET(858): No client connected", idthread);
	if (TestTimeOutRead(idthread, currentsock) == false)
		return aNULL;
	char inputstr[MAXSIZEINPUT + 1];
	if (nbbytes >= MAXSIZEINPUT)
		nb = readsock(currentsock, inputstr, MAXSIZEINPUT);
	else
		nb = readsock(currentsock, inputstr, nbbytes);
	if (validstream(nb) == false) {						//In the case of non blocking socket, we simply return the empty string
		string errmessage = "SOCKET(860): Error on RECEIVE";
		errmessage += Msgerror();
		return globalAtanor->Returnerror(errmessage, idthread);
	}
	if (nb != 0) {
		inputstr[nb] = 0;
		res += inputstr;
	}
	if (decrement)
		nbbytes -= nb;
	while (nb != 0 && nbbytes > 0) {
		if (TestTimeOutRead(idthread, currentsock) == false)
			return aNULL;
		if (nbbytes >= MAXSIZEINPUT)
			nb = readsock(currentsock, inputstr, MAXSIZEINPUT);
		else
			nb = readsock(currentsock, inputstr, nbbytes);
		if (decrement)
			nbbytes -= nb;
		if (nb != 0) {
			inputstr[nb] = 0;
			res += inputstr;
		}
		if (validstream(nb) == false) {
			string errmessage = "SOCKET(860): Error on RECEIVE";
			errmessage += Msgerror();
			return globalAtanor->Returnerror(errmessage, idthread);
		}
	}
	return globalAtanor->Providestring(res);
}

Atanor* Atanorsocket::MethodGet(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	int nb = -1;
	SOCKET currentsock;
	if (server == true) {
		if (callfunc->Size() == 0)
			return globalAtanor->Returnerror("SOCKET(821): Missing parameter in receive", idthread);
		currentsock = callfunc->Evaluate(0, aNULL, idthread)->Integer();
		if (currentsock == SOCKET_ERROR)
			return globalAtanor->Returnerror("SOCKET(824): Non connected socket", idthread);
	}
	else
		currentsock = sock;
	if (currentsock == SOCKET_ERROR)
		return globalAtanor->Returnerror("SOCKET(858): No client connected", idthread);
	if (TestTimeOutRead(idthread, currentsock) == false)
		return aNULL;
	char rd[] = { 0, 0, 0 };
	nb = readsock(currentsock, rd, 1);
	if (validstream(nb) == false) {						//In the case of non blocking socket, we simply return the empty string
		string errmessage = "SOCKET(860): Error on RECEIVE ";
		errmessage += Msgerror();
		return globalAtanor->Returnerror(errmessage, idthread);
	}
	if (!nb)
		return aNULL;
	if (contextualpattern->Type() == a_byte) {
		((Atanorbyte*)contextualpattern)->value = rd[0];
		return contextualpattern;
	}
	return new Atanorbyte(rd[0]);
}

Atanor* Atanorsocket::MethodWriteRaw(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	SOCKET currentsock = sock;
	int first = 0;
	if (server == true) {
		first = 1;
		if (callfunc->Size() <= 1)
			return globalAtanor->Returnerror("SOCKET(827): Missing parameter in send", idthread);
		currentsock = callfunc->Evaluate(0, aNULL, idthread)->Integer();
		if (currentsock == SOCKET_ERROR)
			return globalAtanor->Returnerror("SOCKET(858): No client connected", idthread);
	}
	if (TestTimeOutWrite(idthread, currentsock) == false)
		return aNULL;
	Atanor* kvalue;
	string strc;
	char* buff;
	int sz;
	for (int i = first; i<callfunc->Size(); i++) {
		kvalue = callfunc->Evaluate(i, aNULL, idthread);
		strc = kvalue->String();
		buff = STR(strc);
		sz = strc.size();
		while (sz>0) {
			int nbsz = sz;
			if (nbsz > MAXSIZEINPUT)
				nbsz = MAXSIZEINPUT;
			if (writesock(currentsock, buff, nbsz) < 0) {
				string errmessage = "SOCKET(859): Error sending";
				errmessage += Msgerror();
				return globalAtanor->Returnerror(errmessage, idthread);
			}
			buff += MAXSIZEINPUT;
			sz -= MAXSIZEINPUT;
		}
	}
	return aTRUE;
}

Atanor* Atanorsocket::MethodClose(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//AtanorThreadLock _lock(general);
	if (server == true) {
		if (hp == NULL || sock == SOCKET_ERROR)
			return aFALSE;
		//We clean a client connection
		if (callfunc->Size() == 1) {
			int socketclient = callfunc->Evaluate(0, aNULL, idthread)->Integer();
			if (socketclient == SOCKET_ERROR)
				return globalAtanor->Returnerror("SOCKET(824): Non connected socket", idthread);
			closesock(socketclient);
			if (socketclients.find(socketclient) != socketclients.end())
				socketclients.erase(socketclient);
			return aTRUE;
		}
	}
	//otherwise we clean the current socket
	if (hp != NULL && sock != SOCKET_ERROR)
		closesock(sock);
	hp = NULL;
	return aTRUE;
}

Atanor* Atanorsocket::MethodBlock(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//AtanorThreadLock _lock(general);
	blocking = callfunc->Evaluate(0, aNULL, idthread)->Boolean();
#ifdef WIN32
	u_long iMode = 0;
	if (blocking == false)
		iMode = 1;
	ioctlsocket(sock, FIONBIO, &iMode);
#else
	int flags;
	flags = fcntl(sock, F_GETFL, 0);
	if (blocking == false)
		flags |= O_NONBLOCK;
	else
		flags &= ~O_NONBLOCK;
	fcntl(sock, F_SETFL, flags);
#endif
	return aTRUE;
}

Atanor* Atanorsocket::MethodTimeout(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	//AtanorThreadLock _lock(general);
	//In our example, we have only two parameters
	Atanor* ktimeout = callfunc->Evaluate(0, aNULL, idthread);
	v_timeout = ktimeout->Integer();
	if (v_timeout == -1) {
		timeout = false;
		return aTRUE;
	}
	//We create our timeout		
	tv.tv_sec = v_timeout;
	tv.tv_usec = 0;
	timeout = true;
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	return aTRUE;
}

Atanor* Atanorsocket::MethodGethostname(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	gethostname(servername, MAXHOSTNAME);           /* who are we? */
	return globalAtanor->Providestring(servername);
}

Atanor* Atanorsocket::MethodPort(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	return globalAtanor->Provideint(port);
}

Atanor* Atanorsocket::MethodGetpeername(Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {
	if (server == true) {
		int socketclient = callfunc->Evaluate(0, aNULL, idthread)->Integer();
		if (socketclient == SOCKET_ERROR)
			return globalAtanor->Returnerror("SOCKET(824): Non connected socket", idthread);
		struct sockaddr cliAddr;
#ifdef WIN32
		int len = sizeof(struct sockaddr);
#else
		socklen_t len = sizeof(struct sockaddr);
#endif
		getpeername(socketclient, &cliAddr, &len);
		struct sockaddr_in* client = (struct sockaddr_in*)&cliAddr;
		char* nm = inet_ntoa(client->sin_addr);
		Atanormap* kmap = globalAtanor->Providemap();
		string key1 = "port";
		kmap->Push(key1, globalAtanor->Provideint(client->sin_port));
		string key2 = "address";
		string nms = nm;
		kmap->Push(key2, globalAtanor->Providestring(nms));
		return kmap;
	}
	return globalAtanor->Returnerror("SOCKET(852): You cannot use GETPEERNAME on the client side", idthread);
}


Atanor* Atanorsocket::CreateSocket(short idthread) {

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == SOCKET_ERROR)
		return globalAtanor->Returnerror("SOCKET(856): Socket error", idthread);
#ifdef WIN32        
	memset((char *)&servAddr, 0, sizeof(servAddr));
#else
	bzero((char *)&servAddr, sizeof(servAddr));
#endif

	if (checkipaddres(servername) == false) {
		hp = gethostbyname(servername);                  /* get our address info */
		if (hp == NULL)                             /* we don't exist !? */
			return globalAtanor->Returnerror("SOCKET(851): Cannot find host by name on this machine", idthread);
		memcpy((char *)&servAddr.sin_addr, hp->h_addr, hp->h_length);     /* set address */
		servAddr.sin_family = hp->h_addrtype;              /* this is our host address */
		/* this is our port number */
	}
	else {
		servAddr.sin_addr.s_addr = inet_addr(servername);
		servAddr.sin_family = AF_INET;
	}

	servAddr.sin_port = htons(port);

	len = sizeof(servAddr);


	return aTRUE;
}

bool Atanorsocket::TestTimeOutRead(short idthread, SOCKET currentsock) {
	if (timeout == true) {
		tv.tv_sec = v_timeout;
		tv.tv_usec = 0;
		FD_ZERO(&readfds);
		//Then we insert our socket in it
		FD_CLR(currentsock, &readfds);
		FD_SET(currentsock, &readfds);
		//int sl=select(currentsock+1,&readfds,NULL,NULL, &tv);
		int sl = select(FD_SETSIZE, &readfds, NULL, NULL, &tv);
		if (sl < 0) {
			string errmessage = "SOCKET(888): Reading connection refused ";
			errmessage += Msgerror();
			globalAtanor->Returnerror(errmessage, idthread);
			return false;
		}
		sl = FD_ISSET(currentsock, &readfds);
		if (sl == 0) {
			string errmessage = "SOCKET(830): Time out ";
			//errmessage+=Msgerror();
			globalAtanor->Returnerror(errmessage, idthread);
			return false;
		}
	}
	return true;
}

bool Atanorsocket::TestTimeOutWrite(short idthread, SOCKET currentsock) {
	if (timeout == true) {
		tv.tv_sec = v_timeout;
		tv.tv_usec = 0;
		FD_ZERO(&writefds);
		//Then we insert our socket in it
		FD_CLR(currentsock, &writefds);
		FD_SET(currentsock, &writefds);
		//int sl=select(currentsock+1,NULL,&writefds,NULL,&tv);
		int sl = select(FD_SETSIZE, NULL, &writefds, NULL, &tv);
		if (sl < 0) {
			string errmessage = "SOCKET(889): Writing connection refused ";
			errmessage += Msgerror();
			globalAtanor->Returnerror(errmessage, idthread);
			return false;
		}
		sl = FD_ISSET(currentsock, &writefds);
		if (sl == 0) {
			string errmessage = "SOCKET(830): Time out ";
			//errmessage+=Msgerror();
			globalAtanor->Returnerror(errmessage, idthread);
			return false;
		}
	}
	return true;
}

Atanor* Atanorsocket::Write(string act, Atanor* contextualpattern, short idthread, AtanorCall* callfunc) {

	Atanor* kvalue;
	short sz;
	string strc;
	int first = 0;
	char padding[POSSIGNATURE + 1];

	memset(padding, 0, POSSIGNATURE + 1);

	SOCKET currentsock = sock;

	if (server == true) {
		first = 1;
		if (callfunc->Size() <= 1)
			return globalAtanor->Returnerror("SOCKET(822): Missing parameter in write", idthread);

		currentsock = callfunc->Evaluate(0, aNULL, idthread)->Integer();
		if (currentsock == SOCKET_ERROR)
			return globalAtanor->Returnerror("SOCKET(858): No client connected", idthread);
	}

	char* buff;
	bool written = false;
	for (int i = first; i < callfunc->Size(); i++) {
		kvalue = callfunc->Evaluate(i, aNULL, idthread);
		if (kvalue->isError()) {
			strc = "&&ERROR&&:";
			strc += kvalue->String();
		}
		else
			strc = kvalue->String();

		sz = strc.size();
		if (sz>0) {
			written = true;
			sprintf_s(padding + 1, POSSIGNATURE, FORMATSIGNATURE, sz);
			padding[0] = act[0];
			//cout<<"Writing:"<<sz<<":"<<padding<<endl;

			if (TestTimeOutWrite(idthread, currentsock) == false)
				return aNULL;

			if (writesock(currentsock, padding, POSSIGNATURE)<0) {
				string errmessage = "SOCKET(859): Error sending";
				errmessage += Msgerror();
				return globalAtanor->Returnerror(errmessage, idthread);
			}
			buff = STR(strc);
			while (sz>0) {
				int nbsz = sz;
				if (nbsz > MAXSIZEINPUT)
					nbsz = MAXSIZEINPUT;

				if (TestTimeOutWrite(idthread, currentsock) == false)
					return aNULL;

				if (writesock(currentsock, buff, nbsz) < 0) {
					string errmessage = "SOCKET(859): Error sending";
					errmessage += Msgerror();
					return globalAtanor->Returnerror(errmessage, idthread);
				}
				buff += MAXSIZEINPUT;
				sz -= MAXSIZEINPUT;
			}
		}
	}

	if (written == false) {
		//Empty strings... We still write it...
		sprintf_s(padding + 1, POSSIGNATURE, FORMATSIGNATURE, 0);
		padding[0] = act[0];
		if (TestTimeOutWrite(idthread, currentsock) == false)
			return aNULL;

		if (writesock(currentsock, padding, POSSIGNATURE) < 0) {
			string errmessage = "SOCKET(859): Error sending";
			errmessage += Msgerror();
			return globalAtanor->Returnerror(errmessage, idthread);
		}
	}
	return aTRUE;
}




