#include <iostream>
#include <string>
#include <ctime>
#include <cctype>
#include <WS2tcpip.h>
#include <regex>
#pragma comment (lib,"ws2_32.lib")

class BadPacketException : public std::exception
{
	void what()
	{
		std::cout << "Bad packet!" << std::endl;
	}

};

class WSASession {
private:
	WSAData data;
public:
	WSASession() {
		int ret = WSAStartup(MAKEWORD(2, 2), &data);
		if (ret != 0)
			throw std::system_error(WSAGetLastError(), std::system_category(), "WSAStartup Failed");
	}

	~WSASession() {
		WSACleanup();
	}

};
class TCPSocket
{
protected:
	SOCKET sock;

public:
	// TWORZENIE SOCKETA
	TCPSocket()
	{
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == INVALID_SOCKET)
			throw std::system_error(WSAGetLastError(), std::system_category(), "Error opening socket");
	}

	// DESTRUKTOR SOCKETU USUWANIE SOCKETA
	~TCPSocket() {
		closesocket(sock);
	}

	//	POLACZENIE Z SERWEREM
	void connection(const std::string &IPAddress, const unsigned int port) const
	{
		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(port);
		inet_pton(AF_INET, IPAddress.c_str(), &hint.sin_addr);

		const int  connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
		if (connResult == INVALID_SOCKET)
		{
			throw std::system_error(WSAGetLastError(), std::system_category(), "Error connection socket");
		}
	}

	//WYSYLANIE PAKIETU
	void Send(const std::string &message)
	{
		/// NIE JESTEM PEWIEN CZY +1
		const auto ret = send(sock, message.c_str(), message.size(), 0);
		if (ret < 0)
			throw std::system_error(WSAGetLastError(), std::system_category(), "sendto failed");
	}

	//OTRZYMYWANIE PAKIETU
	void Recv(char buf[], unsigned bufLeng) const
	{
		int bytesReceived = recv(sock, buf, bufLeng, 0);
	}
};
struct packet {
private:
	std::string id;
	std::string operation;
	std::string answer;
	std::string time;
public:
	packet(const std::string &id_, const std::string &op_, const std::string &ans_, const std::string &tim_)
	{
		this->id = id_;
		this->operation = op_;
		this->answer = ans_;
		this->time = tim_;
	}
	packet(const std::string& rawData) {

		std::string message = rawData;
		std::regex reg("[0-9a-zA-Z:\.]+(?=\\|)");
		std::smatch match;

		std::regex_search(message, match, reg);
		id = match[0];
		message = match.suffix();

		std::regex_search(message, match, reg);
		operation = match[0];
		message = match.suffix();

		std::regex_search(message, match, reg);
		answer = match[0];
		message = match.suffix();

		std::regex_search(message, match, reg);
		time = match[0];
		message = match.suffix();
	}



	std::string getID() { return id; }
	std::string getOperation() { return operation; }
	std::string getAnswer() { return answer; }
	std::string getTime() { return time; }
	std::string convertToString()
	{
		return ("Id#" + id + "|Op#" + operation + "|Odp#" + answer + "|Czas#" + time + "|");
	}
};

//generowanie id,			->GEN odpowiedź w polu IDENTYFIKATOR
//przesłanie liczby L		->NUM, w polu ODPOWIEDZI
//przesłanie liczby prób	->ATT  w polu ODPOWIEDZI
//przesylanie wyniku		->TRY  w polu ODPOWIEDZI
//odpowiedz na wynik		->ANS  w polu ODPOWIEDZI
//zakonczenie polaczenia	->EXI  

// Example:
// id#01|op#GEN|time#12:00:00.000|odp#123123|
const std::string ipAddress = "25.50.98.211";
const unsigned int port = 19975;

bool is_number(const std::string& s)
{
	return !s.empty() && std::find_if(s.begin(),
		s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}
void requestID(TCPSocket &Socket)
{
	packet messagePakiet("0", "GEN", "00", "12:00:00.000");
	std::string messageString = messagePakiet.convertToString();
	Socket.Send(messageString);

}
void sendL(TCPSocket &Socket, std::string &id)
{
	std::string numberL = "a"; unsigned int division;
	do
	{
		std::cout << "Podaj dodatnia parzysta liczbe L: ";
		std::getline(std::cin, numberL);
		division = std::stoi(numberL);

	} while (!is_number(numberL) && division % 2 == 0);
	packet messagePakiet(id, "NUM", numberL, "12:00:00.000");
	std::string messageString = messagePakiet.convertToString();
	Socket.Send(messageString);

}
void trial(TCPSocket &Socket, std::string &id)
{
	std::string numberToSend = "a"; unsigned int division;
	do {
		std::cout << "Sprobuj zgadnac liczbe: ";
		getline(std::cin, numberToSend);
		division = std::stoi(numberToSend);

	} while (!is_number(numberToSend) && division % 2 == 0);

	packet messagePakiet(id, "ATT", numberToSend, "12:00:00.000");
	std::string messageString = messagePakiet.convertToString();
	Socket.Send(messageString);
}
void disconnection(TCPSocket &Socket, std::string &id)
{
	packet messagePakiet(id, "EXIT", "0", "12:00:00.000");
	std::string messageString = messagePakiet.convertToString();
	Socket.Send(messageString);

}

int main()
{
	WSASession Session;
	TCPSocket Socket;

	Socket.connection(ipAddress, port);

	requestID(Socket);
	char buf[64]; const unsigned int bufLeng = 64;
	Socket.Recv(buf, bufLeng);
	std::string ID;
	{
		packet receivedPacket(buf);
		ID = receivedPacket.getID();
	}

	sendL(Socket, ID);

	Socket.Recv(buf, bufLeng);
	packet receivedPacket(buf);
	std::string numOfTriels = receivedPacket.getAnswer();
	unsigned int numOfTrielsInt = std::stoi(numOfTriels);

	while (numOfTrielsInt)
	{
		std::cout << "Twoja liczba prob to: " << numOfTrielsInt << std::endl;
		trial(Socket, ID);

		Socket.Recv(buf, bufLeng);
		packet receivedPacket(buf);
		if (receivedPacket.getAnswer() == "game over")
		{
			std::cout << "Przegrales!" << std::endl; break;
		}
		if (receivedPacket.getAnswer() == "win")
		{
			std::cout << "Wygrales!" << std::endl; break;
		}
		if (receivedPacket.getAnswer() == "bad number" &&numOfTrielsInt > 0)
		{
			std::cout << "Sprobuj jeszcze raz!" << std::endl; numOfTrielsInt--;
		}
	}

	//disconnection(Socket, ID);

	std::cin.ignore(2);

	return 0;
}
