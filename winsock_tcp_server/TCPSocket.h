#pragma once

#include <winsock2.h>
#include <string>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

class TCPSocket {
private:
	SOCKET sock;
	std::vector<SOCKET> clients;
public:
	TCPSocket();
	~TCPSocket();

	void Bind(unsigned int port);
	void Listen();
	void Accept();
	std::string Recieve(const int id, const int& bufforLenght);
	void Send(const int id, const std::string& data);
	void Shutdown(const int id);
};

class ClosingConnectionException : public std::exception {
public:
	virtual const char* what() const throw() {
		return "Connection closing...";
	}
};