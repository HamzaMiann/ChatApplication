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
	MESSAGE_ID_LEAVE_ROOM
};



class client
{
private:
	WSADATA wsaData;
	int iResult;
	std::thread* listen_thread;
	std::vector<std::string> message_history;

public:
	SOCKET connectSocket = INVALID_SOCKET;
	std::string written_message = "";

	client() {}
	~client();

	void init();
	void listen();
	void send_message(std::string roomName, std::string message);
	void display_to_screen();
};
