#pragma once
#include "TCPSocket.h"
#include <mutex>

#pragma comment (lib, "Ws2_32.lib")

class Client {
	ClientSocket mySocket;

	std::string id;
	std::string numberL;
	std::string attempts;

	std::mutex mutexNumberL;
	std::mutex& mutexAttempt;

	Packet createPacket(Packet::Builder& builder);
public:
	// ReSharper disable once CppNonExplicitConvertingConstructor
	Client(TCPSocket& socket, std::mutex& mux);

	void run(const int id);
	std::string getNumberL();

	void setAttempts(unsigned at);

	~Client();
};