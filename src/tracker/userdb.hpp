#pragma once

#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

#include "../models/user.hpp"

class UserDB
{
private:
	std::unordered_map<std::string, std::shared_ptr<User>> users;

public:
	UserDB() = default;

	/**
	 * @return true if user was created, false if user already exists
	 */
	bool createUser(std::string const &username, std::string const &password);
	/**
	 * @return true if user was deleted, false if user does not exist
	 */
	bool deleteUser(std::string const &username);
	/**
	 * @return nullptr if user does not exist
	 */
	std::shared_ptr<User> getUser(std::string const &username);
	std::vector<std::string> getUsernames() const;
};
