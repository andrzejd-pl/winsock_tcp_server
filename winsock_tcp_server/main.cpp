#include <iostream>
#include "WSASession.h"
#include "TCPSocket.h"
#include <thread>
#include "Packet.h"

void run(TCPSocket* socket, const short id, const int bufforLenght);

void main(int argc, char **argv_string) {

	try {
		unsigned short port = 0;
		if(argc < 2) {
			std::cout << "Podaj numer protu do nasluchiwania: ";
			std::cin >> port;
		}
		else port = atoi(argv_string[1]);
		int bufforLenght = (argc < 3) ? 6 : atoi(argv_string[2]);

		WSASession session;
		TCPSocket socket;

		socket.Bind(port);

		socket.Listen();

		socket.Accept();
		//socket.Accept();

		std::vector<std::thread> clients;
		clients.push_back(std::thread(&run, &socket, 0, bufforLenght));
		//clients.push_back(std::thread(&run, &socket, 1, bufforLenght));

		for(auto& client : clients) {
			client.join();
		}

		socket.Shutdown(0);
		//socket.Shutdown(1);
	}
	catch(const std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}

	std::cin.clear();
	std::cin.ignore(2);
}


void run(TCPSocket* socket, const short id, const int bufforLenght) {
	std::string buffor_1 = socket->Recieve(id, bufforLenght);
	std::cout << buffor_1 << std::endl;
	std::reverse(buffor_1.begin(), buffor_1.end());
	socket->Send(id, buffor_1);
}