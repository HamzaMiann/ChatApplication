#include "client.h"
#include "console.h"
#include <mutex>

std::mutex mtx;


void ListenToServer(client* client)
{
	//--------------------
	// TO DO
	//--------------------

	// Receive a message from the server before quitting
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	int iResult;

	do
	{
		//printf("Waiting to receive data from the server...\n");
		iResult = recv(client->connectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			printf("Bytes received: %d\n", iResult);
			NetworkBuffer buf(DEFAULT_BUFLEN, recvbuf);
			int message_length = buf.readInt32LE();
			std::string message = buf.readStringBE(message_length);
			client->message_history.push_back(message);

			if (client->message_history.size() > 8u)
			{
				client->message_history.erase(client->message_history.begin());
			}
			client->display_to_screen();
		}
		else if (iResult == 0)
		{
			printf("Connection closed\n");
		}
		else
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
			//closesocket(client->connectSocket);
			//WSACleanup();
			//exit(1);
		}
	}
	while (iResult > 0);

}

client::~client()
{
	// #4 close
	closesocket(connectSocket);
	WSACleanup();
}

void client::init()
{
	

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
	iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
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

void client::listen()
{
	listen_thread = new std::thread(ListenToServer, this);
}

void client::send_message(std::string message, MessageTypes type)
{


	//--------------------
	// TO DO
	//--------------------


	NetworkBuffer buf(DEFAULT_BUFLEN);
	buf.writeInt32BE(0);

	//if ()
	buf.writeInt32BE(type);


	//buf.writeInt32BE(roomName.length());
	//buf.writeString(roomName);
	buf.writeInt32BE(message.length());
	buf.writeStringBE(message);
	/*if (type == MessageTypes::MESSAGE_ID_SEND)
	{
		buf.writeInt32BE(message.length());
		buf.writeStringBE(message);
	}
	else if (type == MessageTypes::MESSAGE_ID_NAME)
	{
		buf.writeInt32BE(message.length());
		buf.writeStringBE(message);
	}
	else if (type == MessageTypes::MESSAGE_ID_JOIN_ROOM)
	{
		buf.writeInt32BE(message.length());
		buf.writeStringBE(message);
	}*/

	// #3 write & read
	const char* buffer = "Hello server!";

	//printf("Sending a packet to the server...\n");
	iResult = send(connectSocket, buf.Data(), DEFAULT_BUFLEN, 0);
	//iResult = send(connectSocket, buffer, (int)strlen(buffer), 0);
	if (iResult == SOCKET_ERROR)
	{
		//printf("send() failed with error: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		exit(1);
	}
	//printf("Bytes sent: %d\n", iResult);

}

void client::display_to_screen()
{
	//clear_screen();
	if (inRoom == true)
	{
		mtx.lock();
		system("cls");
		printf("Use /join [RoomName] to join a room]\n");
		printf("Use /leave [RoomName] to leave a room\n");
		for (unsigned int i = 0; i < 8; ++i)
		{
			if (i < message_history.size())
			{
				printf("%s\n", message_history[i].c_str());
			}
			else
			{
				printf("\n");
			}
		}
		printf("%s", written_message.c_str());
		mtx.unlock();
	}
}
