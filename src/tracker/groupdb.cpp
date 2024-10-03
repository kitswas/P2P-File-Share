#include "groupdb.hpp"

bool GroupDB::createGroup(group_id_t group_id, std::shared_ptr<User> owner)
{
	if (groups.find(group_id) != groups.end())
	{
		return false;
	}
	groups[group_id] = std::make_shared<Group>(group_id, owner);
	return true;
}

bool GroupDB::deleteGroup(group_id_t group_id)
{
	auto it = groups.find(group_id);
	if (it == groups.end())
	{
		return false;
	}
	groups.erase(it);
	return true;
}

std::shared_ptr<Group> GroupDB::getGroup(group_id_t group_id)
{
	auto it = groups.find(group_id);
	if (it == groups.end())
	{
		return nullptr;
	}
	return it->second;
}

std::vector<group_id_t> GroupDB::getGroups() const
{
	std::vector<group_id_t> group_ids;
	for (auto const &[group_id, _] : groups)
	{
		group_ids.push_back(group_id);
	}
	return group_ids;
}
