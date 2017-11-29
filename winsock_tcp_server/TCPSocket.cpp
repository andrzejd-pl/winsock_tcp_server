#include "TCPSocket.h"
#include <system_error>
#include <vector>


TCPSocket::TCPSocket() {
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock == INVALID_SOCKET)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Error opening socket");
}


TCPSocket::~TCPSocket() {
	for(auto& client : clients) {
		closesocket(client);
	}
}

void TCPSocket::Bind(unsigned int port) {
	sockaddr_in add;
	add.sin_family = AF_INET;
	add.sin_addr.s_addr = htonl(INADDR_ANY);
	add.sin_port = htons(port);
	int ret = bind(sock, reinterpret_cast<SOCKADDR *>(&add), sizeof(add));
	if(ret == SOCKET_ERROR)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Bind failed");
}

void TCPSocket::Listen() {
	int ret = listen(sock, SOMAXCONN);
	if(ret == SOCKET_ERROR)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Listen failed");

}

void TCPSocket::Accept() {
	SOCKET client = accept(sock, NULL, NULL);
	if(client == INVALID_SOCKET)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Accept failed");

	clients.push_back(client);
}

std::string TCPSocket::Recieve(const int id, const int& bufforLenght) {
	std::string buff2;
	std::vector<char> buff;
	buff.resize(bufforLenght);

	int ret = recv(clients[id], buff.data(), buff.size(), 0);
	if(ret < 0)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Receive failed");
	if(ret == 0)
		throw ClosingConnectionException();


	return std::string(buff.cbegin(), buff.cend());
}

void TCPSocket::Send(const int id, const std::string& data) {
	int ret = send(clients[id], data.c_str(), data.size(), 0);
	if(ret == SOCKET_ERROR)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Send failed");
}

void TCPSocket::Shutdown(const int id) {
	int ret = shutdown(clients[id], SD_SEND);
	if(ret == SOCKET_ERROR)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Shutdown failed");

	closesocket(clients[id]);
}
