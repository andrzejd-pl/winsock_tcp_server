#include "ClientSocket.h"

ClientSocket::ClientSocket(SOCKET& sock) : socket_(sock) {}

ClientSocket::~ClientSocket() {
	closesocket(socket_);
}

ConnectionClosing::~ConnectionClosing() throw() {}

char const* ConnectionClosing::what() const {
	return "Connection closed";
}

ClientSocket::ClientSocket(const ClientSocket& other)
	: socket_(other.socket_) {}

ClientSocket::ClientSocket(ClientSocket&& other) noexcept : socket_(other.socket_) {}

ClientSocket& ClientSocket::operator=(const ClientSocket& other) {
	if(this == &other)
		return *this;
	socket_ = other.socket_;
	return *this;
}

ClientSocket& ClientSocket::operator=(ClientSocket&& other) noexcept {
	if(this == &other)
		return *this;
	socket_ = other.socket_;
	return *this;
}

// ReSharper disable once CppMemberFunctionMayBeConst
Packet ClientSocket::receivePacket() {
	char buffor[DEFAULT_BUFFLEN];
	int ret = recv(socket_, buffor, DEFAULT_BUFFLEN, 0);

	if(ret == 0)
		throw ConnectionClosing();
	if(ret < 0)
		throw BadPacketException();

	//std::clog << "Client " << id << "  recvbuf: " << recvbuf;
	return Packet(std::vector<char>(buffor, buffor + DEFAULT_BUFFLEN));
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ClientSocket::sendPacket(const Packet& packet) {
	std::vector<char> str = packet.convertToString();
	//std::clog << "Client " << id << "  send: " << str.data();
	send(socket_, str.data(), str.size(), 0);
}
