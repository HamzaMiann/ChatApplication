
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

	do
	{
		printf("Waiting to receive data from the client...\n");
		iResult = recv(conn->acceptSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			// We have received data successfully!
			// iResult is the number of bytes received
			printf("Bytes received: %d\n", iResult);

			conn->Server.ProcessMessage(recvbuf, recvbuflen, conn);
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
	conn->Server.RemoveClient(conn);
	closesocket(conn->acceptSocket);
	delete conn;
}

bool containsRoom(connection* c, std::string room)
{
	bool connected = false;
	for (std::string r : c->rooms)
	{
		if (r == room)
		{
			connected = true;
			break;
		}
	}
	return connected;
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

void server::SendMessageToClients(std::string message, connection* conn)
{
	mtx.lock();

	printf("Sending message from %d to all clients\n", (int)conn->acceptSocket);

	NetworkBuffer buf(DEFAULT_BUFLEN);
	buf.writeInt32BE(message.length());
	buf.writeStringBE(message);

	for (unsigned int i = 0; i < clients.size(); ++i)
	{

		int iSendResult = send(clients[i]->acceptSocket, buf.Data(), DEFAULT_BUFLEN, 0);
		//int iSendResult = send(clients[i]->acceptSocket, message.message.c_str(), message.message_length, 0);
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

void server::SendMessageToRoom(std::string room, std::string message, connection* conn)
{
	mtx.lock();

	printf("Sending message from %d to room %s\n", (int)conn->acceptSocket, room.c_str());

	NetworkBuffer buf(DEFAULT_BUFLEN);
	buf.writeInt32BE(message.length());
	buf.writeStringBE(message);

	for (unsigned int i = 0; i < clients.size(); ++i)
	{
		//if (clients[i]->room != room) continue;
		if (containsRoom(clients[i], room.c_str()) == false)
		{
			continue;
		}
		//if (clients[i]->rooms.data()->find)

		int iSendResult = send(clients[i]->acceptSocket, buf.Data(), DEFAULT_BUFLEN, 0);
		//int iSendResult = send(clients[i]->acceptSocket, message.message.c_str(), message.message_length, 0);
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

// Send a message to a specific client
void server::SendMessageToAClient(std::string message, connection* conn)
{
	mtx.lock();

	printf("Sending message to client at socket %d\n", (int)conn->acceptSocket);

	NetworkBuffer buf(DEFAULT_BUFLEN);
	buf.writeInt32BE(message.length());
	buf.writeStringBE(message);

	for (unsigned int i = 0; i < clients.size(); ++i)
	{
		if (clients[i] == conn)
		{
			int iSendResult = send(clients[i]->acceptSocket, buf.Data(), DEFAULT_BUFLEN, 0);
			//int iSendResult = send(clients[i]->acceptSocket, message.message.c_str(), message.message_length, 0);
			if (iSendResult == SOCKET_ERROR)
			{
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(clients[i]->acceptSocket);
				WSACleanup();
				exit(1);
			}
		}
	}

	mtx.unlock();
}

void server::ProcessMessage(char* recvbuf, unsigned int recvbuflen, connection* conn)
{
	network_message m;
	NetworkBuffer buf(recvbuflen, recvbuf);

	// header
	m.packet_length = buf.readInt32LE();
	m.message_id = (MessageTypes)buf.readInt32LE();


	switch (m.message_id)
	{
	case MessageTypes::MESSAGE_ID_JOIN_ROOM:
	{
		mtx.lock();

		m.room_length = buf.readInt32LE();
		m.room = buf.readString(m.room_length);

		mtx.unlock();
		//conn->room = m.room;
		if (!containsRoom(conn, m.room.c_str()))
		{
			conn->rooms.push_back(m.room.c_str());


			// notify all clients that the client has left the room
			SendMessageToClients(conn->client_name + " has joined '" + m.room + "'", conn);
			printf("%s has joined %s\n", conn->client_name.c_str(), m.room.c_str());
		}
		break;
	}
	case MessageTypes::MESSAGE_ID_LEAVE_ROOM:
	{
		// store room variable
		//std::string room = conn->room;

		mtx.lock();

		m.room_length = buf.readInt32LE();
		m.room = buf.readString(m.room_length);

		bool leftRoom = false;
		for (std::vector<std::string>::iterator index = conn->rooms.begin(); index != conn->rooms.end(); index++)
		{
			if (*index == m.room)
			{
				conn->rooms.erase(index);
				leftRoom = true;
				break;
			}
		}

		//conn->room = "";

		mtx.unlock();

		// notify all clients that the client has left the room
		if (leftRoom == true)
		{
			SendMessageToClients(conn->client_name + " has left '" + m.room + "'", conn);
			printf("%s has left %s\n", conn->client_name.c_str(), m.room.c_str());
		}
		break;
	}
	case MessageTypes::MESSAGE_ID_SEND:
	{
		m.message_length = buf.readInt32LE();
		m.message = buf.readString(m.message_length);

		for (std::string s : conn->rooms)
		{
			SendMessageToRoom(s.c_str(),  "["+ s + "] " + conn->client_name + ": " + m.message, conn);
			printf("%s sent %s to %s\n", conn->client_name.c_str(), m.message.c_str(), s.c_str());
		}
		break;
	}
	case MessageTypes::MESSAGE_ID_NAME:
	{
		m.name_length = buf.readInt32LE();
		m.client_name = buf.readString(m.name_length);
		//printf("Client name: %s \n", m.client_name);

		mtx.lock();

		conn->client_name = m.client_name;

		mtx.unlock();
	}
	default:
		break;
	}

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
