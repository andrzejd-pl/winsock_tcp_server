#pragma once
#include <winsock2.h>
#include "ClientSocket.h"

#pragma comment (lib, "Ws2_32.lib")

class ListenSocket {
private:
	SOCKET sock;
	addrinfo *result = nullptr;
	addrinfo hints;
public:
	// ReSharper disable once CppNonExplicitConvertingConstructor
	ListenSocket(unsigned int port);
	~ListenSocket();

	void Bind();
	void Listen();

	ClientSocket Accept();
};

