
#include "server.h"

void server::init()
{
	//WSADATA wsaData;

	//// Initialize Winsock
	//iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	//if (iResult != 0)
	//{
	//	// Something went wrong, tell the user the error id
	//	printf("WSAStartup failed with error: %d\n", iResult);
	//	return 1;
	//}
	//else
	//{
	//	printf("WSAStartup() was successful!\n");
	//}

	//// #1 Socket
	//SOCKET listenSocket = INVALID_SOCKET;
	//SOCKET acceptSocket = INVALID_SOCKET;

	//struct addrinfo* addrResult = NULL;
	//struct addrinfo hints;

	//// Define our connection address info 
	//ZeroMemory(&hints, sizeof(hints));
	//hints.ai_family = AF_INET;
	//hints.ai_socktype = SOCK_STREAM;
	//hints.ai_protocol = IPPROTO_TCP;
	//hints.ai_flags = AI_PASSIVE;

	//// Resolve the server address and port
	//iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &addrResult);
	//if (iResult != 0)
	//{
	//	printf("getaddrinfo() failed with error %d\n", iResult);
	//	WSACleanup();
	//	return 1;
	//}
	//else
	//{
	//	printf("getaddrinfo() is good!\n");
	//}

	//// Create a SOCKET for connecting to the server
	//listenSocket = socket(
	//	addrResult->ai_family,
	//	addrResult->ai_socktype,
	//	addrResult->ai_protocol
	//);
	//if (listenSocket == INVALID_SOCKET)
	//{
	//	// https://docs.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2
	//	printf("socket() failed with error %d\n", WSAGetLastError());
	//	freeaddrinfo(addrResult);
	//	WSACleanup();
	//	return 1;
	//}
	//else
	//{
	//	printf("socket() is created!\n");
	//}

}

void server::start()
{

}
