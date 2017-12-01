#include "TCPSocket.h"
#include <ws2tcpip.h>
#include <string>

TCPSocket::TCPSocket(unsigned int port) {
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	std::string DEFAULT_PORT = std::to_string(port);
	int iResult = getaddrinfo(nullptr, DEFAULT_PORT.c_str(), &hints, &result);
	if(iResult != 0)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Error opening socket");

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if(ListenSocket == INVALID_SOCKET)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Error opening socket");
}


TCPSocket::~TCPSocket() {
	closesocket(ListenSocket);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void TCPSocket::Bind() {
	// Setup the TCP listening socket
	int iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if(iResult == SOCKET_ERROR)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Error opening socket");


	freeaddrinfo(result);
}

void TCPSocket::Listen() {
	int iResult = listen(ListenSocket, SOMAXCONN);
	if(iResult == SOCKET_ERROR)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Error opening socket");

}

ClientSocket TCPSocket::Accept() {
	SOCKET client = accept(ListenSocket, NULL, NULL);
	if(client == INVALID_SOCKET)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Error opening socket");
	return ClientSocket(client);
}
