#pragma once

#define WIN32_LEAN_AND_MEAN			// Strip rarely used calls

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <vector>
#include "NetworkBuffer.hpp"

#pragma comment (lib, "Ws2_32.lib")

class server;

enum error_type
{
	NONE,
	DB_ERROR,
	NO_MATCH,
	INVALID_CREDENTIALS,
	ALREADY_EXISTS
};

enum AuthMessageTypes
{
	CreateAccountWeb,
	AuthenticateWeb,
	CreateAccountWebSuccess,
	CreateAccountWebFailure,
	AuthenticateWebSuccess,
	AuthenticateWebFailure
};

class auth_client
{
private:
	WSADATA wsaData;
	int iResult;


public:
	SOCKET connectSocket = INVALID_SOCKET;
	std::thread* listen_thread;
	server* connected_server;

	auth_client(server* s) { this->connected_server = s; }
	~auth_client();

	void init();
	void listen();
	void send_message(AuthMessageTypes type, std::string message);
};
