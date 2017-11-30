#pragma once

#include <winsock2.h>
#include <string>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

class TCPSocket {
private:
	SOCKET sock;
public:
	TCPSocket();
	~TCPSocket();

	void Bind(unsigned int port);
	void Listen();
	SOCKET Accept();
	std::vector<char> Recieve(SOCKET client, const int& bufforLenght);
	void Send(SOCKET client, const std::vector<char>& data);
	void Shutdown(SOCKET* client);
};

class ClosingConnectionException : public std::exception {
public:
	virtual const char* what() const throw() {
		return "Connection closing...";
	}
};