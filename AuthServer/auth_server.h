#pragma once

#define WIN32_LEAN_AND_MEAN			// Strip rarely used calls

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <map>
#include "NetworkBuffer.hpp"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "5151"

enum AuthMessageTypes
{
	CreateAccountWeb,
	AuthenticateWeb,
	CreateAccountWebSuccess,
	CreateAccountWebFailure,
	AuthenticateWebSuccess,
	AuthenticateWebFailure
};

struct network_message
{
	INT32				packet_length;
	AuthMessageTypes	message_id;
	INT32				message_length;
	std::string			message;
};

class connection;

class auth_server
{
private:
	SOCKET listenSocket = INVALID_SOCKET;
	int iResult;
	struct addrinfo hints;
	struct addrinfo* addrResult = NULL;

	connection* client;

public:
	~auth_server();

	void init();
	void start_listening();

	void SendMessageToClients(AuthMessageTypes type, std::string message, connection* conn);
	//void SendMessageToAClient(std::string message, connection* conn); // Send a message to a specific client
	void ProcessMessage(char* recvbuf, unsigned int recvbuflen, connection* conn);
	void RemoveClient() { client = nullptr; }
};

class connection
{
public:
	auth_server& server;
	SOCKET acceptSocket;

	connection(auth_server& _server, SOCKET socket) : server(_server), acceptSocket(socket) {}

	inline bool isConnected() { return acceptSocket != INVALID_SOCKET; }
};