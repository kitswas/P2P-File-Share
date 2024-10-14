#pragma once

#include "file.hpp"
#include "user.hpp"

class Group
{
private:
	std::string group_id;
	std::shared_ptr<User> owner;
	std::unordered_set<std::shared_ptr<User>> users;
	std::unordered_set<std::shared_ptr<User>> join_requests;
	std::unordered_set<std::shared_ptr<File>> files;

public:
	Group(std::string const &group_id, std::shared_ptr<User> owner) : group_id(group_id), owner(owner) {};

	std::string get_group_id() const { return group_id; }
	std::shared_ptr<User> get_owner() const { return owner; }
	std::unordered_set<std::shared_ptr<File>> get_files() const { return files; }
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

	bool has_member(std::shared_ptr<User> user)
	{
		return (user == owner ||
				users.find(user) != users.end());
	}

	bool remove_user(std::shared_ptr<User> user)
	{
		return users.erase(user);
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

	bool add_file(std::shared_ptr<File> file)
	{
		if (files.find(file) != files.end())
		{
			return false;
		}
		files.insert(file);
		return true;
	}
};
