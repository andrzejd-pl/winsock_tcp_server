#pragma once
#include "TCPSocket.h"

class Client {
	TCPSocket& socket;
	SOCKET mySocket;
public:
	Client(TCPSocket& socket);



	~Client();
};

