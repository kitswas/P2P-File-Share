#pragma once

#include <memory>
#include <string>
#include <unordered_set>

#include "endpoint.hpp"
#include "fileinfo.hpp"
#include "user.hpp"

class File
{
public:
	std::unordered_set<std::shared_ptr<Endpoint>> seeders;
	const std::shared_ptr<FileInfo> file_info;
	const std::string group_id;

	File(std::shared_ptr<FileInfo> file_info, std::string const &group_id) : file_info(file_info), group_id(group_id) {}

	friend bool operator==(const File &lhs, const File &rhs)
	{
		return lhs.file_info->hash == rhs.file_info->hash && lhs.group_id == rhs.group_id;
	}
};

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
