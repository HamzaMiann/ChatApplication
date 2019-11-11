#pragma once

#define WIN32_LEAN_AND_MEAN			// Strip rarely used calls


#include <vector>
#include <string>
#include <map>
#include "NetworkBuffer.hpp"
#include "auth_client.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "5150"

enum MessageTypes
{
	MESSAGE_ID_SEND,
	MESSAGE_ID_JOIN_ROOM,
	MESSAGE_ID_LEAVE_ROOM,
	MESSAGE_ID_NAME,
	REGISTER_EMAIL,
	AUTHENTICATE_EMAIL
};

struct network_message
{
	INT32			packet_length;
	INT32			message_id;
	INT32			room_length;
	std::string		room;
	INT32			message_length;
	std::string		message;
	INT32			name_length;
	std::string		client_name;
};

class connection;

class server
{
private:
	SOCKET listenSocket = INVALID_SOCKET;
	int iResult;
	struct addrinfo hints;
	struct addrinfo* addrResult = NULL;

	auth_client* auth_server;

	std::vector<connection*> clients;

public:

	server() { auth_server = new auth_client(this); }
	~server();

	void init();
	void start_listening();

	void SendMessageToClients(std::string message, connection* conn);
	void SendMessageToRoom(std::string room, std::string message, connection* conn);
	void SendMessageToAClient(std::string message, connection* conn); // Send a message to a specific client
	void ProcessMessage(char* recvbuf, unsigned int recvbuflen, connection* conn);
	void ProcessAuthMessage(char* recvbuf, unsigned int recvbuflen);
	void RemoveClient(connection* conn);
	void AddClient(connection* conn);
};

class connection
{
public:
	server& Server;
	std::string client_name;
	//std::string room;
	std::vector<std::string> rooms;
	SOCKET acceptSocket;
	connection(server& _server, SOCKET socket) : Server(_server), acceptSocket(socket) {}
	inline bool isConnected() { return acceptSocket != INVALID_SOCKET; }
	std::string email;
};