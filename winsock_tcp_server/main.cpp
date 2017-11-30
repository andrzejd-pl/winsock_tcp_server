#include <iostream>
#include "WSASession.h"
#include "TCPSocket.h"
#include <thread>
#include "Client.h"

void run(Client* client, const int id);

void main(int argc, char **argv_string) {

	try {
		unsigned short port = 2000;
		/*if(argc < 2) {
			std::cout << "Podaj numer protu do nasluchiwania: ";
			std::cin >> port;
		}
		else port = atoi(argv_string[1]);*/

		WSASession session;
		TCPSocket socket;

		socket.Bind(port);

		socket.Listen();

		auto tmp = socket.Accept();

		Client client1(socket);
		Client client2(socket);

		std::vector<std::thread> clients;
		clients.push_back(std::thread(&run, &client1, 1));
		clients.push_back(std::thread(&run, &client2, 2));

		for(auto& client : clients) {
			client.join();
		}
	}
	catch(const std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}

	std::cin.clear();
	std::cin.ignore(2);
}

void run(Client* client, const int id) {
	client->run(id);
}