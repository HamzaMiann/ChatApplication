
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

UserInfo database::CreateAccount(std::string email, std::string hash)
{
	UserInfo info;
	info.error = NONE;

	try
	{
		if (!Connect())
		{
			info.error = DB_ERROR;
			return info;
		}

		sql::PreparedStatement* pstmt = con->prepareStatement(
			"select * from web_auth where email = '" + username + "'"
		);

		sql::ResultSet* res = pstmt->executeQuery();

		if (res->rowsCount() > 0)
		{
			info.error = ALREADY_EXISTS;
			return info;
		}

		pstmt = con->prepareStatement(
			"insert into user (last_login, creation_date) values (curdate(), curdate())"
		);

		if (pstmt->execute() == 0)
		{
			info.error = DB_ERROR;
			return info;
		}


		pstmt = con->prepareStatement(
			"insert into web_auth (email, salt, hashed_password, userId) values (?, ?, ?, (select max(id) from user))"
		);

		pstmt->setString(1, username);
		pstmt->setString(2, "salty_gamer");
		pstmt->setString(3, hash);
		if (pstmt->execute() == 0)
		{
			info.error = DB_ERROR;
			return info;
		}

		pstmt = con->prepareStatement("select max(id) from user");
		res = pstmt->executeQuery();
		if (res->rowsCount() == 0)
		{
			info.error = DB_ERROR;
			return info;
		}

		res->next();
		info.user_id = res->getInt(1);

		res->close();
		pstmt->close();
		con->close();

		this->isConnected = false;

	}
	catch (sql::SQLException& ex)
	{
		info.error = DB_ERROR;
	}

	return info;
}

UserInfo database::Authenticate(std::string email, std::string hash)
{
	UserInfo info;
	info.error = NONE;

	try
	{
		if (!Connect())
		{
			info.error = DB_ERROR;
			return info;
		}

		sql::PreparedStatement* pstmt = con->prepareStatement(
			"select salt, hashed_password, userid, email from web_auth where email = ?"
		);

		pstmt->setString(1, email);

		sql::ResultSet* res = pstmt->executeQuery();
		if (res->rowsCount() >= 1)
		{

			res->next();

			std::string salt = res->getString(1);
			std::string pass = res->getString(2);
			int userid = res->getInt(3);

			if (salt != "salty_gamer" || pass != hash)
			{
				info.error = NO_MATCH;
			}
			else
			{
				info.email = email;
				info.user_id = userid;
			}

		}
		else
		{
			info.error = NO_MATCH;
		}

		res->close();
		pstmt->close();
		con->close();

		this->isConnected = false;
	}
	catch (sql::SQLException& ex)
	{
		info.error = DB_ERROR;
	}
	
	return info;
}
