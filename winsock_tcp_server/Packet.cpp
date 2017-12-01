#include "Packet.h"
#include <regex>
#include <iostream>


Packet::Packet(std::string id,
			   std::string operation,
			   std::string time,
			   std::string response) : id(id), operation(operation), time(time), response(response) {}

Packet::Packet() {}

Packet::Packet(const Packet& other) : id(other.id), operation(other.operation), time(other.time), response(other.response) {}

Packet::Packet(Packet&& other) noexcept : id(std::move(other.id)), operation(std::move(other.operation)), time(std::move(other.time)), response(std::move(other.response)) {}

Packet& Packet::operator=(const Packet& other) {
	if(this == &other)
		return *this;
	id = other.id;
	operation = other.operation;
	time = other.time;
	response = other.response;
	return *this;
}

Packet& Packet::operator=(Packet&& other) noexcept {
	if(this == &other)
		return *this;
	id = std::move(other.id);
	operation = std::move(other.operation);
	time = std::move(other.time);
	response = std::move(other.response);
	return *this;
}

Packet::Packet(const std::vector<char>& rawData) {
	try {
		std::regex expresion("\\/"), sExpresion("#");
		std::smatch match, sMatch;
		std::string buffer(rawData.begin(), rawData.end());

		if(!std::regex_search(buffer, match, expresion))
			throw BadPacketException();
		{
			std::string prefix = match.prefix().str();
			if(!std::regex_search(prefix, sMatch, sExpresion))
				throw BadPacketException();

			if(sMatch.prefix().str() != "Id")
				throw BadPacketException();

			std::string sufix = sMatch.suffix().str();
			id = sufix;
		}
		buffer = match.suffix().str();

		if(!std::regex_search(buffer, match, expresion))
			throw BadPacketException();
		{
			std::string prefix = match.prefix().str();
			if(!std::regex_search(prefix, sMatch, sExpresion))
				throw BadPacketException();

			if(sMatch.prefix().str() != "Op")
				throw BadPacketException();

			std::string sufix = sMatch.suffix().str();
			operation = sufix;
		}
		buffer = match.suffix().str();

		if(!std::regex_search(buffer, match, expresion))
			throw BadPacketException();
		{
			std::string prefix = match.prefix().str();
			if(!std::regex_search(prefix, sMatch, sExpresion))
				throw BadPacketException();

			if(sMatch.prefix().str() != "Time")
				throw BadPacketException();

			std::string sufix = sMatch.suffix().str();
			time = sufix;
		}

		buffer = match.suffix().str();

		if(!std::regex_search(buffer, match, expresion))
			throw BadPacketException();
		{
			std::string prefix = match.prefix().str();
			if(!std::regex_search(prefix, sMatch, sExpresion))
				throw BadPacketException();

			if(sMatch.prefix().str() != "Odp")
				throw BadPacketException();

			response = sMatch.suffix().str();
		}
	}
	catch(const std::exception& ex) {
		std::cerr << "Exception in Ctor Packet(const std::vector<char>& rawData): " << ex.what() << std::endl;
	}
}

Packet::Builder Packet::Builder::setId(const std::string& id) {
	this->id = id;
	return *this;
}

Packet::Builder Packet::Builder::setOperation(const std::string& operation) {
	this->operation = operation;
	return *this;
}

Packet::Builder Packet::Builder::setTime(const std::string& time) {
	this->time = time;
	return *this;
}

Packet::Builder Packet::Builder::setResponse(const std::string& response) {
	this->response = response;
	return *this;
}

Packet Packet::Builder::builder() const {
	return Packet(id, operation, time, response);
}

std::string Packet::getId() const {
	return id;
}

std::string Packet::getOperation() const {
	return operation;
}

std::string Packet::getTime() const {
	return time;
}

std::string Packet::getResponse() const {
	return response;
}

std::vector<char> Packet::convertToString() const {
	std::string buffor;
	buffor += "Id#" + id + "/";
	buffor += "Op#" + operation + "/";
	buffor += "Time#" + time + "/";
	buffor += "Odp#" + response + "/";

	std::vector<char>  rt;
	std::copy(buffor.begin(), buffor.end(), std::back_inserter(rt));

	if(*(rt.end() - 1) == '\0') rt.erase(rt.end() - 1);

	return rt;
}


Packet::~Packet() {}

const char* BadPacketException::what() const throw() {
	return "Bad or corrupted packet!!!";
}
