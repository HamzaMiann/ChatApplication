

#include "auth_server.h"

/*
	Main (Program Entry Point)
*/
int main(int argc, char** argv)
{
	auth_server* s = new auth_server;

	s->init();
	s->start_listening();

	delete s;
}