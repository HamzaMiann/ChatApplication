
#include <conio.h>
#include <iostream>
#include <string>
#include "client.h"

#define ESCAPE 27
#define RETURN 13

std::string getNewRoom()
{
	std::string result;
	system("cls");
	std::cout << "Please enter the room you would like to join: ";
	std::cin >> result;
	return result;
}

void registerEmail()
{


}

int main(int argc, char** argv)
{
	std::string room, name;
	std::cout << "Please enter your name: ";
	std::cin >> name;
	//std::cout << "Please enter the room you would like to join: ";
	//std::cin >> room;

	client* c = new client();

	c->init();

	c->listen();
	c->send_message(name, MESSAGE_ID_NAME);
	//c->send_message(room, MESSAGE_ID_JOIN_ROOM);
	//while (c->authenticated == false)
	//{
	//	system("cls");
	//	printf("type /register to register an email\n");
	//	printf("type /authenticate to login to an email\n");
	//	c->display_to_screen();
	//	std::string result = "";
	//	std::cin >> result;
	//	if (result.size() >= 9)
	//	{
	//		if (result.substr(0, 9) == "/register")	//If the user wants to register an email
	//		{
	//			system("cls");
	//			std::string email;
	//			std::string password;
	//			std::cout << "Enter the email account name: ";
	//			std::cin >> email;
	//			std::cout << "\n Enter the password for your account: ";
	//			std::cin >> password;

	//			c->email_authentication(email, password, REGISTER_EMAIL);
	//		}
	//	}
	//	if (result.size() >= 13)	//If the user wants to authenticate an email
	//	{
	//		if (result.substr(0, 13) == "/authenticate")
	//		{
	//			system("cls");
	//			std::string email;
	//			std::string password;
	//			std::cout << "Enter the email account name: ";
	//			std::cin >> email;
	//			std::cout << "\n Enter the password for your account: ";
	//			std::cin >> password;

	//			c->email_authentication(email, password, AUTHENTICATE_EMAIL);
	//		}
	//	}
	//}
	c->inRoom = true;
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
				bool usedSpecialChar = false;
				/*if (c->written_message == "/leave ")
					{

						c->inRoom = false;
						c->written_message = "";
						c->send_message("", MESSAGE_ID_LEAVE_ROOM);
						room = getNewRoom();
						c->inRoom = true;
						c->send_message(room, MESSAGE_ID_JOIN_ROOM);
					}*/
				if (c->written_message.size() >= 7)
				{
					if (c->written_message.substr(0, 7) == "/leave ")
					{
						usedSpecialChar = true;
						c->send_message(c->written_message.substr(7), MESSAGE_ID_LEAVE_ROOM);
					}
				}
				if (c->written_message.size() >= 6)
				{
					if (c->written_message.substr(0, 6) == "/join ")
					{
						usedSpecialChar = true;
						c->send_message(c->written_message.substr(6), MESSAGE_ID_JOIN_ROOM);
					}
				}
				if (c->written_message.size() >= 9)
				{
					if (c->written_message.substr(0, 9) == "/register")	//If the user wants to register an email
					{
						system("cls");
						std::string email;
						std::string password;
						std::cout << "Enter the email account name: ";
						std::cin >> email;
						std::cout << "\n Enter the password for your account: ";
						std::cin >> password;

						c->email_authentication(email, password, REGISTER_EMAIL);
					}
				}
				if (c->written_message.size() >= 13)	//If the user wants to authenticate an email
				{
					if (c->written_message.substr(0, 13) == "/authenticate")
					{
						system("cls");
						std::string email;
						std::string password;
						std::cout << "Enter the email account name: ";
						std::cin >> email;
						std::cout << "\n Enter the password for your account: ";
						std::cin >> password;

						c->email_authentication(email, password, AUTHENTICATE_EMAIL);
					}
				}
				if (usedSpecialChar == false)
				{
					c->send_message(c->written_message, MESSAGE_ID_SEND);
				}
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

			if (c->inRoom == true)
			{
				c->display_to_screen();
			}

			
		}
	}

	delete c;

	return 0;
}