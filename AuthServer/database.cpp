
#include "database.h"

database::database(std::string ip, std::string schema, std::string username, std::string password)
{
	driver = get_driver_instance();
	this->ip = ip;
	this->schema = schema;
	this->username = username;
	this->password = password;
}

bool database::Connect()
{
	if (isConnected) return true;

	try
	{
		con = driver->connect(ip, username, password);
		con->setSchema(schema);
		this->isConnected = true;
	}
	catch (sql::SQLException& ex)
	{
		this->isConnected = false;
	}
	return this->isConnected;
}

bool database::CreateAccount(std::string email, std::string hash)
{
	try
	{
		if (!Connect()) return false;

		sql::PreparedStatement* pstmt = con->prepareStatement(
			"select * from web_auth where email = '" + username + "'"
		);

		sql::ResultSet* res = pstmt->executeQuery();

		if (res->rowsCount() > 0) return false;

		pstmt = con->prepareStatement(
			"insert into user (last_login, creation_date) values (curdate(), curdate())"
		);

		if (pstmt->execute() == 0) return false;



		pstmt = con->prepareStatement(
			"insert into web_auth (email, salt, hashed_password, userId) values (?, ?, ?, (select max(id) from user))"
		);

		pstmt->setString(1, username);
		pstmt->setString(2, "salty_gamer");
		pstmt->setString(3, hash);
		if (pstmt->execute() == 0)
			return false;

		res->close();
		pstmt->close();
		con->close();

		this->isConnected = false;

	}
	catch (sql::SQLException& ex)
	{
		return false;
	}
	return true;
}

bool database::Authenticate(std::string email, std::string hash)
{
	try
	{
		if (!Connect()) return false;

		sql::PreparedStatement* pstmt = con->prepareStatement(
			"select salt, hashed_password from web_auth where email = ?"
		);

		pstmt->setString(1, email);

		sql::ResultSet* res = pstmt->executeQuery();
		if (res->rowsCount() < 1) return false;

		res->next();

		std::string salt = res->getString(1);
		std::string pass = res->getString(1);

		if (salt != "salty_gamer" || pass != hash) return false;

		res->close();
		pstmt->close();
		con->close();

		this->isConnected = false;
	}
	catch (sql::SQLException& ex)
	{
		return false;
	}
	return true;
}
