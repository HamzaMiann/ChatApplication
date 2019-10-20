

#include "server.h"

/*
	Main (Program Entry Point)
*/
int main(int argc, char** argv)
{
	server* s = new server;

	s->init();
	s->start_listening();

	delete s;
}