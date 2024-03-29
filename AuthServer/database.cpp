
#include "database.h"
#include "bcrypt/BCrypt.hpp"

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

UserInfo database::CreateAccount(std::string email, std::string pass)
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
			"select * from web_auth where email = '" + email + "'"
		);

		sql::ResultSet* res = pstmt->executeQuery();

		if (res->rowsCount() > 0)
		{
			info.error = ALREADY_EXISTS;
			return info;
		}

		pstmt = con->prepareStatement(
			"insert into user (last_login, creation_date) values (now(), now())"
		);

		if (pstmt->executeUpdate() == 0)
		{
			info.error = DB_ERROR;
			return info;
		}

		pstmt->close();

		pstmt = con->prepareStatement(
			"insert into web_auth (email, salt, hashed_password, userId) values (?, ?, ?, (select max(id) from user))"
		);

		pstmt->setString(1, email);
		pstmt->setString(2, "salty_gamer");
		pstmt->setString(3, BCrypt::generateHash(pass));
		if (pstmt->executeUpdate() == 0)
		{
			info.error = DB_ERROR;
			return info;
		}

		pstmt = con->prepareStatement("select id, creation_date from user where id = (select max(id) from user)");
		res = pstmt->executeQuery();
		if (res->rowsCount() == 0)
		{
			info.error = DB_ERROR;
			return info;
		}

		res->next();
		info.user_id = res->getInt(1);
		info.creation_date = res->getString(2);

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

UserInfo database::Authenticate(std::string email, std::string pass)
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
			"select web_auth.salt, web_auth.hashed_password, web_auth.userid, user.creation_date from web_auth inner join user on web_auth.userid = user.id where web_auth.email = ? "
		);

		pstmt->setString(1, email);

		sql::ResultSet* res = pstmt->executeQuery();
		if (res->rowsCount() >= 1)
		{

			res->next();

			std::string salt = res->getString(1);
			std::string hash = res->getString(2);
			int userid = res->getInt(3);
			std::string creation = res->getString(4);

			if (salt != "salty_gamer" || !BCrypt::validatePassword(pass, hash))
			{
				info.error = NO_MATCH;
			}
			else
			{
				info.email = email;
				info.user_id = userid;
				info.creation_date = creation;

				pstmt = con->prepareStatement("update user set last_login = now() where id = ?");
				pstmt->setInt(1, userid);
				pstmt->executeUpdate();
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
