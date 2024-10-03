#pragma once

#include <memory>
#include <string>
#include <unordered_set>

#include "user.hpp"

using group_id_t = u_int16_t;

class Group
{
private:
	u_int16_t group_id;
	std::shared_ptr<User> owner;
	std::unordered_set<std::shared_ptr<User>> users;
	std::unordered_set<std::shared_ptr<User>> join_requests;

public:
	Group(u_int16_t group_id, std::shared_ptr<User> owner) : group_id(group_id), owner(owner) {};

	u_int16_t get_group_id() const { return group_id; }
	std::shared_ptr<User> get_owner() const { return owner; }
	std::unordered_set<std::shared_ptr<User>> get_users() const { return users; }
	std::unordered_set<std::shared_ptr<User>> get_join_requests() const { return join_requests; }

	bool add_user(std::shared_ptr<User> user)
	{
		if (users.find(user) != users.end())
		{
			return false;
		}
		users.insert(user);
		return true;
	}

	bool remove_user(std::shared_ptr<User> user)
	{
		if (users.find(user) == users.end())
		{
			return false;
		}
		users.erase(user);
		return true;
	}

	bool add_join_request(std::shared_ptr<User> user)
	{
		if (join_requests.find(user) != join_requests.end())
		{
			return false;
		}
		join_requests.insert(user);
		return true;
	}

	bool remove_join_request(std::shared_ptr<User> user)
	{
		if (join_requests.find(user) == join_requests.end())
		{
			return false;
		}
		join_requests.erase(user);
		return true;
	}
};
