#include "groupdb.hpp"

bool GroupDB::createGroup(std::string const &group_id, std::shared_ptr<User> owner)
{
	if (groups.find(group_id) != groups.end())
	{
		return false;
	}
	groups[group_id] = std::make_shared<Group>(group_id, owner);
	return true;
}

bool GroupDB::deleteGroup(std::string const &group_id)
{
	auto it = groups.find(group_id);
	if (it == groups.end())
	{
		return false;
	}
	groups.erase(it);
	return true;
}

std::shared_ptr<Group> GroupDB::getGroup(std::string const &group_id)
{
	auto it = groups.find(group_id);
	if (it == groups.end())
	{
		return nullptr;
	}
	return it->second;
}

std::vector<std::string> GroupDB::getGroups() const
{
	std::vector<std::string> group_ids;
	for (auto const &[group_id, _] : groups)
	{
		group_ids.push_back(group_id);
	}
	return group_ids;
}
