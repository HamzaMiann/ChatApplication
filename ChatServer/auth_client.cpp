#include "auth_client.h"

#define DEFAULT_AUTH_BUFLEN 512
#define DEFAULT_AUTH_PORT "5151"

auth_client::~auth_client()
{
}

void auth_client::init()
{
	// Should be blocking call
}

void auth_client::listen()
{
	// Should be non-blocking (spawn a new thread)
}

void auth_client::send_message(AuthMessageTypes type, std::string message)
{

}
