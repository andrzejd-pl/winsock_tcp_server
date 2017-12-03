#pragma once
#include "ListenSocket.h"
#include <mutex>

#pragma comment (lib, "Ws2_32.lib")

class Client {
	ClientSocket mySocket;

	std::string id;
	std::string numberL;
	std::string attempts;

	std::mutex mutexNumberL;
	std::mutex& mutexAttempt;
	std::mutex& mutexClog;

	Packet createPacket(Packet::Builder& builder);

	void log(const std::string& message);
	std::string createLogMessage(const std::string& message, bool sendOrReceive);
public:
	// ReSharper disable once CppNonExplicitConvertingConstructor
	Client(ListenSocket& socket, std::mutex& muxNumL, std::mutex& log);

	void run(const int id);
	std::string getNumberL();

	void setAttempts(unsigned at);

	~Client();
};