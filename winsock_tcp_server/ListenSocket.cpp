#include "ListenSocket.h"
#include <ws2tcpip.h>
#include <string>

ListenSocket::ListenSocket(unsigned int port) {
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
	sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if(sock == INVALID_SOCKET)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Error opening socket");
}


ListenSocket::~ListenSocket() {
	closesocket(sock);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ListenSocket::Bind() {
	// Setup the TCP listening socket
	int iResult = bind(sock, result->ai_addr, (int)result->ai_addrlen);
	if(iResult == SOCKET_ERROR)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Error opening socket");


	freeaddrinfo(result);
}

void ListenSocket::Listen() {
	int iResult = listen(sock, SOMAXCONN);
	if(iResult == SOCKET_ERROR)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Error opening socket");

}

ClientSocket ListenSocket::Accept() {
	SOCKET client = accept(sock, NULL, NULL);
	if(client == INVALID_SOCKET)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Error opening socket");
	return ClientSocket(client);
}
