
#include "auth_server.h"
#include <mutex>
#include <thread>
#include <algorithm>
#include <protobuf/AuthenticationProtocol.pb.h>
#include "database.h"

std::mutex mtx;

#define IP "127.0.0.1:3306"
#define USR "root"
#define PAS "root"
#define SCH	"authenticationschema"

void HandleAccept(connection* conn)
{
	int iResult;

	// #5 recv & send	(Blocking calls)
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	do
	{
		printf("Waiting to receive data from the client...\n");
		iResult = recv(conn->acceptSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			// We have received data successfully!
			// iResult is the number of bytes received
			printf("Bytes received: %d\n", iResult);

			conn->server.ProcessMessage(recvbuf, recvbuflen, conn);
			/*network_message nm;
			NetworkBuffer buf(DEFAULT_BUFLEN, recvbuf);
			nm.message_length = buf.readInt32LE();
			nm.message = buf.readStringBE(nm.message_length);

			conn->Server.SendMessageToClients(nm.message, conn);*/

		}
		else if (iResult < 0)
		{
			printf("recv failed with error: %d, closing connection\n", WSAGetLastError());
			break;
		}
		else // iResult == 0
		{
			printf("Connection %d closing...\n", (int)conn->acceptSocket);
		}
	} while (iResult > 0);
	conn->server.RemoveClient();
	closesocket(conn->acceptSocket);
	delete conn;
}

auth_server::~auth_server()
{
	WSACleanup();
}

void auth_server::init()
{
	WSADATA wsaData;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		// Something went wrong, tell the user the error id
		printf("WSAStartup failed with error: %d\n", iResult);
		exit(1);
	}
	else
	{
		printf("WSAStartup() was successful!\n");
	}

	// #1 Socket
	// Define our connection address info 
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &addrResult);
	if (iResult != 0)
	{
		printf("getaddrinfo() failed with error %d\n", iResult);
		WSACleanup();
		exit(1);
	}
	else
	{
		printf("getaddrinfo() is good!\n");
	}

	// Create a SOCKET for connecting to the server
	listenSocket = socket(
		addrResult->ai_family,
		addrResult->ai_socktype,
		addrResult->ai_protocol
	);
	if (listenSocket == INVALID_SOCKET)
	{
		// https://docs.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2
		printf("socket() failed with error %d\n", WSAGetLastError());
		freeaddrinfo(addrResult);
		WSACleanup();
		exit(1);
	}
	else
	{
		printf("socket() is created!\n");
	}

	// #2 Bind - Setup the TCP listening socket
	iResult = bind(
		listenSocket,
		addrResult->ai_addr,
		(int)addrResult->ai_addrlen
	);
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(addrResult);
		closesocket(listenSocket);
		WSACleanup();
		exit(1);
	}
	else
	{
		printf("bind() is good!\n");
	}

	// We don't need this anymore
	freeaddrinfo(addrResult);

	// #3 Listen
	iResult = listen(listenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		printf("listen() failed with error %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		exit(1);
	}
	else
	{
		printf("listen() was successful!\n");
	}

}

void auth_server::start_listening()
{
	while (true)
	{
		SOCKET acceptSocket = INVALID_SOCKET;

		// #4 Accept		(Blocking call)
		printf("Waiting for client to connect...\n");
		acceptSocket = accept(listenSocket, NULL, NULL);
		if (acceptSocket == INVALID_SOCKET)
		{
			printf("accept() failed with error: %d\n", WSAGetLastError());
			closesocket(listenSocket);
			WSACleanup();
			exit(1);
		}
		else
		{
			printf("accept() is OK!\n");
			printf("Accepted client on socket %d\n", acceptSocket, this);
		}

		connection* conn = new connection(*this, acceptSocket);
		this->client = conn;

		new std::thread(HandleAccept, conn);
	}

	// No longer need server socket
	closesocket(listenSocket);
}

void auth_server::SendMessageToClients(std::string message, connection* conn)
{
	mtx.lock();

	printf("Sending message from %d to all clients\n", (int)conn->acceptSocket);

	NetworkBuffer buf(DEFAULT_BUFLEN);
	buf.writeInt32BE(message.length());
	buf.writeStringBE(message);

	int iSendResult = send(conn->acceptSocket, buf.Data(), DEFAULT_BUFLEN, 0);
	//int iSendResult = send(clients[i]->acceptSocket, message.message.c_str(), message.message_length, 0);
	if (iSendResult == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(conn->acceptSocket);
		WSACleanup();
		exit(1);
	}

	mtx.unlock();
}

// Send a message to a specific client
//void auth_server::SendMessageToAClient(std::string message, connection* conn)
//{
//	mtx.lock();
//
//	printf("Sending message to client at socket %d\n", (int)conn->acceptSocket);
//
//	NetworkBuffer buf(DEFAULT_BUFLEN);
//	buf.writeInt32BE(message.length());
//	buf.writeStringBE(message);
//
//	for (unsigned int i = 0; i < clients.size(); ++i)
//	{
//		if (clients[i] == conn)
//		{
//			int iSendResult = send(clients[i]->acceptSocket, buf.Data(), DEFAULT_BUFLEN, 0);
//			//int iSendResult = send(clients[i]->acceptSocket, message.message.c_str(), message.message_length, 0);
//			if (iSendResult == SOCKET_ERROR)
//			{
//				printf("send failed with error: %d\n", WSAGetLastError());
//				closesocket(clients[i]->acceptSocket);
//				WSACleanup();
//				exit(1);
//			}
//		}
//	}
//
//	mtx.unlock();
//}

void auth_server::ProcessMessage(char* recvbuf, unsigned int recvbuflen, connection* conn)
{
	network_message m;
	NetworkBuffer buf(recvbuflen, recvbuf);

	// header
	m.packet_length = buf.readInt32LE();
	m.message_id = (AuthMessageTypes)buf.readInt32LE();


	switch (m.message_id)
	{
	case AuthMessageTypes::AuthenticateWeb:
	{
		m.message_length = buf.readInt32LE();
		m.message = buf.readStringBE(m.message_length);

		authentication::AuthenticateWeb web;
		web.ParseFromString(m.message);

		database* db = new database(IP, USR, PAS, SCH);

		db->Connect();

		if (db->Authenticate(web.email(), web.plaint64extpassword()))
		{
			// SEND YES TO SERVER
			m.message = "Authentication successful! Account created on ";
			m.message_length = m.message.length();
			SendMessageToClients(m.message, conn);
		}
		else
		{
			m.message = "Authentication failure!";
			m.message_length = m.message.length();
			SendMessageToClients(m.message, conn);
		}
	}
	break;
	case AuthMessageTypes::CreateAccountWeb:
	{
		m.message_length = buf.readInt32LE();
		m.message = buf.readStringBE(m.message_length);

		authentication::AuthenticateWeb web;
		web.ParseFromString(m.message);

		database* db = new database(IP, USR, PAS, SCH);

		db->Connect();

		if (db->CreateAccount(web.email(), web.plaint64extpassword()))
		{
			// SEND YES TO SERVER
			m.message = "Account creation successful! Account created on ";
			m.message_length = m.message.length();
			SendMessageToClients(m.message, conn);
		}
		else
		{
			m.message = "Account creation failure!";
			m.message_length = m.message.length();
			SendMessageToClients(m.message, conn);
		}
	}
	default:
		break;
	}

}