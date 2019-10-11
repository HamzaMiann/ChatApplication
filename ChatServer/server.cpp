
#include "server.h"
#include <mutex>
#include <thread>
#include <algorithm>

std::mutex mtx;

void HandleAccept(connection* conn)
{
	int iResult;

	// #5 recv & send	(Blocking calls)
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	int iSendResult;

	do
	{
		printf("Waiting to receive data from the client...\n");
		iResult = recv(conn->acceptSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			// We have received data successfully!
			// iResult is the number of bytes received
			printf("Bytes received: %d\n", iResult);
			network_message m;
			m.message = recvbuf;
			m.message_length = iResult;
			conn->Server.SendMessageToAllClients(m, conn);

			// Send data to the client
			/*iSendResult = send(conn->acceptSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR)
			{
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(conn->acceptSocket);
				WSACleanup();
				exit(1);
			}
			printf("Bytes sent: %d\n", iSendResult);*/
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
	conn->Server.RemoveClient(conn);
	closesocket(conn->acceptSocket);
	delete conn;
}

server::~server()
{
	WSACleanup();
}

void server::init()
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

void server::start_listening()
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
		AddClient(conn);

		new std::thread(HandleAccept, conn);
	}

	// No longer need server socket
	closesocket(listenSocket);
}

void server::SendMessageToAllClients(network_message message, connection* conn)
{
	mtx.lock();

	printf("Sending message from %d to all clients\n", (int)conn->acceptSocket);

	for (unsigned int i = 0; i < clients.size(); ++i)
	{
		printf("client = %d\n", i);
		int iSendResult = send(clients[i]->acceptSocket, message.message, message.message_length, 0);
		if (iSendResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(clients[i]->acceptSocket);
			WSACleanup();
			exit(1);
		}
	}


	mtx.unlock();
}

void server::RemoveClient(connection* conn)
{
	mtx.lock();

	std::vector<connection*>::iterator it = std::find(clients.begin(), clients.end(), conn);
	if (it != clients.end())
	{
		clients.erase(it);
	}

	mtx.unlock();
}

void server::AddClient(connection* conn)
{
	mtx.lock();

	this->clients.push_back(conn);

	mtx.unlock();
}
