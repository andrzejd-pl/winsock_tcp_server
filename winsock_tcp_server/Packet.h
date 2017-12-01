#pragma once
#include <vector>

class Packet {
	const size_t bufforSize = 65;

	std::string id;
	std::string operation;
	std::string time;
	std::string response;

	Packet(std::string id,
		   std::string operation,
		   std::string time,
		   std::string response);

public:

	Packet();
	explicit Packet(const std::vector<char>& rawData);
	Packet(const Packet& other);
	Packet(Packet&& other) noexcept;

	~Packet();

	Packet& operator=(const Packet& other);
	Packet& operator=(Packet&& other) noexcept;

	class Builder {
		std::string id;
		std::string operation;
		std::string time;
		std::string response;

	public:
		Builder setId(const std::string& id);
		Builder setOperation(const std::string& operation);
		Builder setTime(const std::string& time);
		Builder setResponse(const std::string& response);

		Packet builder() const;
	};

	std::string getId() const;
	std::string getOperation() const;
	std::string getTime() const;
	std::string getResponse() const;

	std::vector<char> convertToString() const;
};

class BadPacketException : public std::exception {
	virtual const char* what() const throw();
};