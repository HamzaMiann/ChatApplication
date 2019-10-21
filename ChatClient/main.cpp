
#include <conio.h>
#include <iostream>
#include <string>
#include "client.h"

#define ESCAPE 27
#define RETURN 13



int main(int argc, char** argv)
{
	std::string room, name;
	std::cout << "Please enter your name: ";
	std::cin >> name;
	std::cout << "Please enter the room you would like to join: ";
	std::cin >> room;

	client* c = new client();

	c->init();

	c->listen();

	std::string message;
	bool quit = false;
	while (!quit)
	{
		// Conintuously print to the console to demonstrate 
		// kbhit is non-blocking.

		if (_kbhit())
		{
			char ch = _getch();

			if (ch == ESCAPE)
			{
				quit = true;
				break;
			}
			else if (ch == RETURN)
			{
				c->send_message(room, name, c->written_message);
				c->written_message = "";
			}
			else if (ch == '\b')
			{
				if (c->written_message.size() > 0)
				{
					c->written_message.pop_back();
				}
			}
			else
			{
				c->written_message.push_back(ch);
			}


			c->display_to_screen();

			
		}
	}

	delete c;

	return 0;
}