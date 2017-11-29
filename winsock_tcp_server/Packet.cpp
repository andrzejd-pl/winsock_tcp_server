#include "Packet.h"
#include <regex>


Packet::Packet(std::string id,
			   std::string operation,
			   std::string time,
			   std::string response) : id(id), operation(operation), time(time), response(response) {}

Packet::Packet() {}

Packet::Packet(const std::string& rawData) {
	std::regex expresion("\\|"), sExpresion("#");
	std::smatch match, sMatch;
	std::string buffer = rawData;

	if(!std::regex_search(buffer, match, expresion))
		throw BadPacketException();
	{
		std::string prefix = match.prefix().str();
		if(!std::regex_search(prefix, sMatch, sExpresion))
			throw BadPacketException();

		if(sMatch.prefix().str() != "id")
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
		
		if(sMatch.prefix().str() != "op")
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
		
		if(sMatch.prefix().str() != "time")
			throw BadPacketException();

		std::string sufix = sMatch.suffix().str();
		time = sufix;
	}
	response = match.suffix().str();
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
	buffor += "id#" + id + "|";
	buffor += "op#" + operation + "|";
	buffor += "time#" + time + "|";
	buffor += response;

	buffor.resize(bufforSize);

	std::vector<char>  rt;
	std::copy(buffor.begin(), buffor.end(), std::back_inserter(rt));

	if(*(rt.end() - 1) == '\0') rt.erase(rt.end() - 1);

	return rt;
}


Packet::~Packet() {}

const char* BadPacketException::what() const throw() {
	return "Bad or corrupted packet!!!";
}
