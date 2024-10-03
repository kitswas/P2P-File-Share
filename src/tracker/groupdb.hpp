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
	std::unordered_map<group_id_t, std::shared_ptr<Group>> groups;

public:
	GroupDB() = default;

	/**
	 * @return true if group was created, false if group already exists
	 */
	bool createGroup(group_id_t group_id, std::shared_ptr<User> owner);
	/**
	 * @return true if group was deleted, false if group does not exist
	 */
	bool deleteGroup(group_id_t group_id);
	/**
	 * @return nullptr if group does not exist
	 */
	std::shared_ptr<Group> getGroup(group_id_t group_id);
	std::vector<group_id_t> getGroups() const;
};
