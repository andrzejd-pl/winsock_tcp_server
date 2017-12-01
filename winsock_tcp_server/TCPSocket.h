#pragma once
#include <winsock2.h>
#include "ClientSocket.h"

#pragma comment (lib, "Ws2_32.lib")

class TCPSocket {
private:
	SOCKET ListenSocket;
	addrinfo *result = nullptr;
	addrinfo hints;
public:
	// ReSharper disable once CppNonExplicitConvertingConstructor
	TCPSocket(unsigned int port);
	~TCPSocket();

	void Bind();
	void Listen();

	ClientSocket Accept();
};

