#pragma once
#include <winsock2.h>
#include "Packet.h"

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFFLEN 512

class ClientSocket {
private:
	SOCKET socket_;
public:
	ClientSocket(SOCKET& sock);

	ClientSocket(const ClientSocket& other);
	ClientSocket(ClientSocket&& other) noexcept;

	ClientSocket& operator=(const ClientSocket& other);
	ClientSocket& operator=(ClientSocket&& other) noexcept;

	Packet receivePacket();
	void sendPacket(const Packet& packet);

	~ClientSocket();
};

class ConnectionClosing : public std::exception {
public:
	virtual ~ConnectionClosing() throw();
	// ReSharper disable once CppOverridingFunctionWithoutOverrideSpecifier
	virtual char const* what() const;
};
