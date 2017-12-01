#include "Client.h"
#include "Packet.h"
#include "CurrentTime.h"
#include <mutex>
#include <iostream>
#include <string>



Packet Client::createPacket(Packet::Builder& builder) {
	return builder.setId(id).setTime(currentTime()).builder();
}

Client::Client(TCPSocket& socket, std::mutex& mux) : mySocket(socket.Accept()), mutexAttempt(mux) {}

void Client::run(const int id) {
	mutexNumberL.lock();
	try {
		Packet p = mySocket.receivePacket();
		std::clog << currentTime() << " - Client " << id << " send: " << p.convertToString().data() << std::endl;

		this->id = std::to_string(id);

		// ReSharper disable once CppMsExtBindingRValueToLvalueReference
		Packet s = createPacket(Packet::Builder().setOperation("GEN"));
		std::clog << currentTime() << " - Client " << id << " receive: " << s.convertToString().data() << std::endl;
		mySocket.sendPacket(s);
		p = mySocket.receivePacket();
		std::clog << currentTime() << " - Client " << id << " send: " << p.convertToString().data() << std::endl;

		numberL = p.getResponse();
		mutexNumberL.unlock();

		mutexAttempt.lock();


		// ReSharper disable once CppMsExtBindingRValueToLvalueReference
		s = createPacket(Packet::Builder().setOperation("ATT").setResponse(attempts));
		std::clog << currentTime() << " - Client " << id << " receive: " << s.convertToString().data() << std::endl;

		mySocket.sendPacket(s);
		unsigned numL = stoi(attempts);
		bool end = false;
		while(!end) {
			p = mySocket.receivePacket();
			std::clog << currentTime() << " - Client " << id << " send: " << p.convertToString().data() << std::endl;

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
			std::clog << currentTime() << " - Client " << id << " receive: " << s.convertToString().data() << std::endl;

			mySocket.sendPacket(s);
		}

		while(true) {
			p = mySocket.receivePacket();
			std::clog << currentTime() << " - Client " << id << " send: " << p.convertToString().data() << std::endl;
		}
	}
	catch(const ConnectionClosing& cc) {
		std::clog << currentTime() << " - Client " << id << ": " << cc.what() << std::endl;
	}
	catch(const std::exception& ex) {
		std::cerr << currentTime() << " - Client " << id << " in mathod run throw exception: " << ex.what() << std::endl;
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
