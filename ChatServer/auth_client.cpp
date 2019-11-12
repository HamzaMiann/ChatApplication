#include "auth_client.h"
#include "server.h"
#include <protobuf/AuthenticationProtocol.pb.h>

#define DEFAULT_AUTH_BUFLEN 512
#define DEFAULT_AUTH_PORT "5151"

void ListenToServer(auth_client* client)
{
	// Receive a message from the server before quitting
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	int iResult;

	//printf("Waiting to receive data from the server...\n");
	iResult = recv(client->connectSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0)
	{
		printf("Bytes received: %d\n", iResult);

		// Spawn a new thread to listen to requests while the server processes the request
		client->listen_thread = new std::thread(ListenToServer, client);

		// Process the request
		client->connected_server->ProcessAuthMessage(recvbuf, recvbuflen);

	}
	else if (iResult == 0)
	{
		printf("Connection closed\n");
		delete client;
		printf("\nLost connection to auth server...\n");
		exit(1);
	}
	else
	{
		printf("recv failed with error: %d\n", WSAGetLastError());
		delete client;
		printf("\nLost connection to auth server...\n");
		exit(1);
	}
}

auth_client::~auth_client()
{
	// #4 close
	closesocket(connectSocket);
	WSACleanup();
}

void auth_client::init()
{
	// Should be blocking call
	//Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		//Something went wrong, tell the user the error id
		printf("WSAStartup failed with error: %d\n", iResult);
		exit(1);
	}
	else
	{
		printf("WSAStartup() was successful\n");
	}

	// #1 socket

	struct addrinfo* result = NULL;
	struct addrinfo* ptr = NULL;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// resolve the server address and port
	iResult = getaddrinfo("127.0.0.1", DEFAULT_AUTH_PORT, &hints, &result);
	if (iResult != 0)
	{
		printf("getaddrinfo() failed with error: %d\n", iResult);
		WSACleanup();
		exit(1);
	}
	else
	{
		printf("getaddrinfo() successful!\n");
	}

	// #2 connect
	// Attempt to connect to the server until a socket succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		// Create a SOCKET for connecting to the server
		connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (connectSocket == INVALID_SOCKET)
		{
			printf("socket() failed with error code %d\n", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
			exit(1);
		}

		// Attempt to connect to the server
		iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			printf("connect() failed with error code %d\n", WSAGetLastError());
			closesocket(connectSocket);
			connectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	freeaddrinfo(result);

	if (connectSocket == INVALID_SOCKET)
	{
		printf("Unable to connect to the server!\n");
		WSACleanup();
		exit(1);
	}
	printf("Successfully connected to the server on socket %d!\n", connectSocket);
}

void auth_client::listen()
{
	// Should be non-blocking (spawn a new thread)
	listen_thread = new std::thread(ListenToServer, this);
}

void auth_client::send_message(AuthMessageTypes type, std::string message)
{
	// TODO
}

void auth_client::verify_email(AuthMessageTypes type, std::string email, std::string password, unsigned int clientId)
{
	NetworkBuffer buf(DEFAULT_BUFLEN);
	buf.writeInt32BE(0);
	buf.writeInt32BE(type);


	if (type == AuthMessageTypes::AuthenticateWeb)
	{
		authentication::AuthenticateWeb web;

		web.set_email(email);
		web.set_plaintextpassword(password);
		web.set_requestid(clientId);

		std::string message;
		message = web.SerializeAsString();
		buf.writeInt32BE(message.length());
		buf.writeStringBE(message);

		buf.writeInt32BE(clientId);
		//buf.writeStringBE(password);

		iResult = send(connectSocket, buf.Data(), DEFAULT_BUFLEN, 0);
		if (iResult == SOCKET_ERROR)
		{
			printf("send() failed with error: %d\n", WSAGetLastError());
			closesocket(connectSocket);
			WSACleanup();
			exit(1);
		}
	}
	else if (type == AuthMessageTypes::CreateAccountWeb)
	{
		authentication::CreateAccountWeb web;
		web.set_email(email);
		web.set_plaintextpassword(password);
		web.set_requestid(clientId);

		std::string message;
		message = web.SerializeAsString();
		buf.writeInt32BE(message.length());
		buf.writeStringBE(message);

		buf.writeInt32BE(clientId);
		//buf.writeStringBE(password);

		iResult = send(connectSocket, buf.Data(), DEFAULT_BUFLEN, 0);
		if (iResult == SOCKET_ERROR)
		{
			printf("send() failed with error: %d\n", WSAGetLastError());
			closesocket(connectSocket);
			WSACleanup();
			exit(1);
		}

	}
}
