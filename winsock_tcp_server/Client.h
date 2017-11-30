#pragma once
#include "TCPSocket.h"

class Client {
	const int bufforLenght = 42;
	TCPSocket& socket;
	SOCKET mySocket;

	std::string id;
	std::string numberL;
public:
	Client(TCPSocket& socket);

	void run(const int id);
	std::string getId() const;

	~Client();
};

