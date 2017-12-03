#include <vector>
#include <thread>
#include <iostream>
#include <string>
#include "WSASession.h"
#include "ListenSocket.h"
#include "Client.h"
#include "CurrentTime.h"

#define DEFAULT_PORT 19975


void run(Client* client, const int id) {
	client->run(id);
}

int main(int argc, char** argv_str) {
	std::mutex mutexClog;
	unsigned port;
	if(argc < 2) {
		std::cout << "Port number: ";
		std::cin >> port;
	}
	else port = atoi(argv_str[1]);
	port = ((port <= 0) || (port > 65535)) ? DEFAULT_PORT : port;

	int id = 1;
	{
		try {
			WSASession session;

			ListenSocket socket(port);
			socket.Bind();
			socket.Listen();

			while(true) {
				std::mutex attempt1;
				std::mutex attempt2;
				attempt1.lock();
				attempt2.lock();

				Client client1(socket, attempt1, mutexClog);
				Client client2(socket, attempt2, mutexClog);

				std::vector<std::thread> clients;
				clients.push_back(std::thread(&run, &client1, id++));
				clients.push_back(std::thread(&run, &client2, id++));

				unsigned numL1 = stoi(client1.getNumberL()), numL2 = stoi(client2.getNumberL());
				unsigned attempts = (numL1 + numL2) / 2;

				client1.setAttempts(attempts);
				client2.setAttempts(attempts);

				attempt1.unlock();
				attempt2.unlock();

				for(auto& client : clients) {
					client.join();
				}
				mutexClog.lock();
				std::clog << currentTime() << " - Server: next round." << std::endl;
				mutexClog.unlock();
			}
		}
		catch(const std::exception& ex) {
			std::clog << currentTime() << " - Server error: " << ex.what() << std::endl;
		}
	}

	std::clog << currentTime() << " - Server stopped working." << std::endl;
	std::cin.clear();
	std::cin.ignore(2);

	return 0;
}
