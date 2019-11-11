#pragma once

#include <jdbc/cppconn/driver.h>
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <string>

enum error_type
{
	NONE,
	DB_ERROR,
	NO_MATCH,
	INVALID_CREDENTIALS,
	ALREADY_EXISTS
};


struct UserInfo
{
	int user_id;
	std::string email;
	error_type error;
};

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

	UserInfo CreateAccount(std::string email, std::string hash);
	UserInfo Authenticate(std::string email, std::string hash);
	
};