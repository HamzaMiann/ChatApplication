

#include "server.h"


int main()
{
	server* s = new server;

	s->init();
	s->start();

	delete s;
}