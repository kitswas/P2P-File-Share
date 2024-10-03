#include "userdb.hpp"

bool UserDB::createUser(std::string const &username, std::string const &password)
{
	if (users.find(username) != users.end())
	{
		return false;
	}
	users[username] = std::make_shared<User>(username, password);
	return true;
}

bool UserDB::deleteUser(std::string const &username)
{
	auto it = users.find(username);
	if (it == users.end())
	{
		return false;
	}
	users.erase(it);
	return true;
}

std::shared_ptr<User> UserDB::getUser(std::string const &username)
{
	auto it = users.find(username);
	if (it == users.end())
	{
		return nullptr;
	}
	return it->second;
}

std::vector<std::string> UserDB::getUsernames() const
{
	std::vector<std::string> usernames;
	for (auto const &[username, _] : users)
	{
		usernames.push_back(username);
	}
	return usernames;
}
