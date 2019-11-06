#pragma once

#include <jdbc/cppconn/driver.h>
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <string>

class database
{
private:
	sql::Driver* driver;
	sql::Connection* con;

	std::string ip;
	std::string schema;
	std::string username;
	std::string password;

	bool isConnected = false;

public:
	database(std::string ip, std::string schema, std::string username, std::string password);

	bool Connect();

	bool CreateAccount(std::string email, std::string hash);
	bool Authenticate(std::string email, std::string hash);
	
};