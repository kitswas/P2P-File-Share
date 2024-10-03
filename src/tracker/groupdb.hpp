#pragma once

#include <memory>
#include <string>
#include <set>
#include <unordered_map>
#include <vector>

#include "../models/group.hpp"

class GroupDB
{
private:
	std::unordered_map<std::string, std::shared_ptr<Group>> groups;

public:
	GroupDB() = default;

	/**
	 * @return true if group was created, false if group already exists
	 */
	bool createGroup(std::string const &group_id, std::shared_ptr<User> owner);
	/**
	 * @return true if group was deleted, false if group does not exist
	 */
	bool deleteGroup(std::string const &group_id);
	/**
	 * @return nullptr if group does not exist
	 */
	std::shared_ptr<Group> getGroup(std::string const &group_id);
	std::vector<std::string> getGroups() const;
};
