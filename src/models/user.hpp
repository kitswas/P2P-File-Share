#pragma once

#include <string>

class User
{
private:
	std::string username;
	std::string password;

public:
	User(std::string const &username, std::string const &password) : username(username), password(password) {};
	std::string getUsername() const { return username; }
	bool checkPassword(std::string_view password) const { return this->password == password; }
};
