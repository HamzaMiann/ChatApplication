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

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "5150"

enum MessageTypes
{
	MESSAGE_ID_SEND,
	MESSAGE_ID_JOIN_ROOM,
	MESSAGE_ID_LEAVE_ROOM,
	MESSAGE_ID_NAME
};



class client
{
private:
	WSADATA wsaData;
	int iResult;
	std::thread* listen_thread;

public:
	SOCKET connectSocket = INVALID_SOCKET;
	std::vector<std::string> message_history;
	std::string written_message = "";
	bool inRoom = false;

	client() {}
	~client();

	void init();
	void listen();
	void send_message(std::string message, MessageTypes type);
	void display_to_screen();
};
