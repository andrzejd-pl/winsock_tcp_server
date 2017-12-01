#include "Client.h"
#include "Packet.h"
#include "CurrentTime.h"
#include <mutex>
#include <iostream>
#include <string>



Packet Client::createPacket(Packet::Builder& builder) {
	return builder.setId(id).setTime(currentTime()).builder();
}

void Client::log(const std::string& message) {
	mutexClog.lock();
	std::clog << message << std::endl;
	mutexClog.unlock();
}

std::string Client::createLogMessage(const std::string& message, bool sendOrReceive) {
	return currentTime() + " - Client " + id + ((sendOrReceive)?(" send: "):(" receive: ")) + message;
}

Client::Client(TCPSocket& socket, std::mutex& mux, std::mutex& log) : mySocket(socket.Accept()), mutexAttempt(mux), mutexClog(log) {}

void Client::run(const int id) {
	mutexNumberL.lock();
	try {
		Packet p = mySocket.receivePacket();
		log(createLogMessage(std::string(p.convertToString().data()), true));

		this->id = std::to_string(id);

		// ReSharper disable once CppMsExtBindingRValueToLvalueReference
		Packet s = createPacket(Packet::Builder().setOperation("GEN"));
		log(createLogMessage(std::string(s.convertToString().data()), false));
		mySocket.sendPacket(s);
		p = mySocket.receivePacket();
		log(createLogMessage(std::string(p.convertToString().data()), true));

		numberL = p.getResponse();
		mutexNumberL.unlock();

		mutexAttempt.lock();


		// ReSharper disable once CppMsExtBindingRValueToLvalueReference
		s = createPacket(Packet::Builder().setOperation("ATT").setResponse(attempts));
		log(createLogMessage(std::string(s.convertToString().data()), false));

		mySocket.sendPacket(s);
		unsigned numL = stoi(attempts);
		bool end = false;
		while(!end) {
			p = mySocket.receivePacket();
			log(createLogMessage(std::string(p.convertToString().data()), true));

			std::string answer = p.getResponse();
			numL--;
			int check = 0;
			if(answer == "5") {
				check = 1;
				end = true;
			}
			else if(numL == 0) {
				end = true;
				check = -1;
			}

			// ReSharper disable once CppMsExtBindingRValueToLvalueReference
			s = createPacket(Packet::Builder().setOperation("ANS").setResponse(check == 1 ? "win" : check == 0 ? "bad" : "game_over"));
			log(createLogMessage(std::string(s.convertToString().data()), false));

			mySocket.sendPacket(s);
		}

		while(true) {
			p = mySocket.receivePacket();
			log(createLogMessage(std::string(p.convertToString().data()), true));
		}
	}
	catch(const ConnectionClosing& cc) {
		log(currentTime() + " - Client " + this->id + ": " + cc.what());
	}
	catch(const std::exception& ex) {
		log(currentTime() + " - Client " + this->id + " in method run throw exception: " + ex.what());
	}
	mutexAttempt.unlock();
}

std::string Client::getNumberL() {
	mutexNumberL.lock();
	std::string rt = numberL;
	mutexNumberL.unlock();
	return rt;
}

void Client::setAttempts(unsigned at) {
	attempts = std::to_string(at);
}

Client::~Client() {}
