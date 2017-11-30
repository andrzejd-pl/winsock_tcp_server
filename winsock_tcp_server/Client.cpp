#include "Client.h"
#include "Packet.h"


class Packet;

Client::Client(TCPSocket& socket) : socket(socket) {
	mySocket = socket.Accept();
}

void Client::run(const int id) {
	/*generowanie id*/
	{
		Packet p(socket.Recieve(mySocket, 42));

		if(!(p.getId() == "0" && p.getOperation() == "GEN"))
			throw BadPacketException();

		this->id = std::to_string(id);

		Packet s = Packet::Builder().
			setId(this->id).
			setOperation("GEN").
			setTime("12:00:00.001").
			builder();

		auto str = s.convertToString();
		str.resize(42, 0);
		socket.Send(mySocket, str);
	}
	/*----------------------*/

	/*otrzymanie liczby L*/
	{
		Packet p(socket.Recieve(mySocket, 42));

		if(!(p.getId() == this->id && p.getOperation() == "NUM"))
			throw BadPacketException();

		numberL = p.getResponse();
	}
	/*----------------------*/
	int numL = (std::stoi(numberL) / 2);
	/*przes³anie liczby prób TODO: narazie to jest tylko liczbaL/2*/
	{

		Packet s = Packet::Builder().
			setId(this->id).
			setOperation("ATT").
			setResponse(std::to_string(numL)).
			setTime("12:00:00.002").
			builder();

		auto str = s.convertToString();
		str.resize(42, 0);
		socket.Send(mySocket, str);
	}
	/*----------------------*/

	bool end = false;
	while(!end) {
		std::string answer;
		/*odebranie odpowiedzi*/
		{
			Packet p(socket.Recieve(mySocket, 42));

			if(!(p.getId() == this->id && p.getOperation() == "TRY"))
				throw BadPacketException();

			answer = p.getResponse();
			numL--;
		}
		/*----------------------*/
		int check = 0;
		if(answer == "5") {
			check = 1;
			end = true;
		}
		else if(numL == 0) {
			end = true;
			check = -1;
		}

		/*odes³anie informacji czy pakiet jest poprawn¹ odpowiedzi¹ czy nie*/
		{
			Packet s = Packet::Builder().
				setId(this->id).
				setOperation("ANS").
				setResponse(check == 1 ? "win" : check == 0 ? "bad number" : "game over").
				setTime("12:00:00.003").
				builder();

			auto str = s.convertToString();
			str.resize(42, 0);
			socket.Send(mySocket, str);
		}
		/*----------------------*/
	}

	/*czekanie na koniec po³aczenia*/
	Packet p(socket.Recieve(mySocket, 42));
	/*----------------------*/
}


Client::~Client() {
	closesocket(mySocket);
}
