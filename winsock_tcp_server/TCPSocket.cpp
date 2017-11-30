#include "TCPSocket.h"
#include <system_error>
#include <vector>


TCPSocket::TCPSocket() {
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock == INVALID_SOCKET)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Error opening socket");
}


TCPSocket::~TCPSocket() {
	closesocket(sock);
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

SOCKET TCPSocket::Accept() {
	SOCKET client = accept(sock, NULL, NULL);
	if(client == INVALID_SOCKET)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Accept failed");

	return client;
}

std::vector<char> TCPSocket::Recieve(SOCKET client, const int& bufforLenght) {
	std::vector<char> buff;
	buff.resize(bufforLenght);

	int ret = recv(client, buff.data(), buff.size(), 0);
	if(ret < 0)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Receive failed");
	if(ret == 0)
		throw ClosingConnectionException();

	
	return buff;
}

void TCPSocket::Send(SOCKET client, const std::vector<char>& data) {
	int ret = send(client, data.data(), data.size(), 0);
	if(ret == SOCKET_ERROR)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Send failed");
}

void TCPSocket::Shutdown(SOCKET* client) {
	int ret = shutdown(*client, SD_SEND);
	if(ret == SOCKET_ERROR)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Shutdown failed");

	closesocket(*client);
}
