#include "process_request.hpp"
#include "groupdb.hpp"
#include "userdb.hpp"

#include <sstream>

static GroupDB groupDB;

static std::unordered_map<std::shared_ptr<TCPSocket>, std::shared_ptr<User>> logged_in_users;
static UserDB userDB;

bool is_logged_in(std::shared_ptr<TCPSocket> client)
{
	return logged_in_users.find(client) != logged_in_users.end();
}

Result process_user_request(std::shared_ptr<TCPSocket> client, UserRequest request, std::stringstream &datastream)
{
	Result result;
	if (request == UserRequest::CREATE)
	{
		std::string username;
		std::string password;
		datastream >> username >> password;

		// Create user
		if (userDB.createUser(username, password))
		{
			result.message = "User created\n";
		}
		else
		{
			result.message = "User already exists\n";
		}
	}
	else if (request == UserRequest::DELETE)
	{
		std::string username;
		datastream >> username;

		// Delete user
		if (userDB.deleteUser(username))
		{
			result.message = "User deleted\n";
		}
		else
		{
			result.message = "User does not exist\n";
		}
	}
	else if (request == UserRequest::LIST)
	{
		// Get usernames
		auto usernames = userDB.getUsernames();
		std::string response = "";
		for (auto const &username : usernames)
		{
			response += username + "\n";
		}
		result.message = (response.empty() ? "No users found\n" : response);
	}
	else if (request == UserRequest::LOGIN)
	{
		std::string username;
		std::string password;
		datastream >> username >> password;
		auto user = userDB.getUser(username);
		if (user && !is_logged_in(client) && user->checkPassword(password))
		{
			logged_in_users.try_emplace(client, user);
			result.message = "Login successful\n";
		}
		else
		{
			result.message = "Login failed\n";
		}
	}
	else if (request == UserRequest::LOGOUT)
	{
		if (logged_in_users.erase(client))
		{
			result.message = "Logout successful\n";
		}
		else
		{
			result.message = "Logout failed\n";
		}
	}
	else
	{
		result.success = false;
		result.message = "Not a user request\n";
		return result;
	}

	result.success = true;
	return result;
}

Result process_group_request(std::shared_ptr<TCPSocket> client, GroupRequest request, std::stringstream &datastream)
{
	Result result;
	const char *const login_required_warning = "You must be logged in to perform this action\n";

	if (request == GroupRequest::LIST)
	{
		auto groups = groupDB.getGroups();
		std::string response = "";
		for (auto const &group : groups)
		{
			response += group + "\n";
		}
		result.message = (response.empty() ? "No users found\n" : response);
		result.success = true;
		return result;
	}

	std::shared_ptr<User> user = nullptr;
	try
	{
		std::string group_id;
		datastream >> group_id;
		// The following require user login
		if (request == GroupRequest::CREATE)
		{
			user = logged_in_users.at(client);
			if (groupDB.createGroup(group_id, user))
			{
				result.message = "Group created\n";
			}
			else
			{
				result.message = "Group already exists\n";
			}
		}
		else if (request == GroupRequest::JOIN)
		{
			user = logged_in_users.at(client);
			std::shared_ptr<Group> group = groupDB.getGroup(group_id);
			if (group && !group->has_member(user))
			{
				group->add_join_request(user);
				result.message = "Join request sent\n";
			}
			else
			{
				result.message = "Invalid request\n";
			}
		}
		else if (request == GroupRequest::LEAVE)
		{
			user = logged_in_users.at(client);
			std::shared_ptr<Group> group = groupDB.getGroup(group_id);
			if (group && group->remove_user(user))
			{
				result.message = "Left group\n";
			}
			else
			{
				result.message = "Invalid request\n";
			}
		}
		else if (request == GroupRequest::LIST_JOIN_REQUESTS)
		{
			user = logged_in_users.at(client);
			std::shared_ptr<Group> group = groupDB.getGroup(group_id);
			if (group)
			{
				auto join_requests = group->get_join_requests();
				std::string response = "";
				for (auto const &join_request : join_requests)
				{
					response += join_request->getUsername() + "\n";
				}
				result.message = (response.empty() ? "No pending join_requests\n" : response);
			}
			else
			{
				result.message = "Group does not exist\n";
			}
		}
		else if (request == GroupRequest::ACCEPT_JOIN_REQUEST)
		{
			user = logged_in_users.at(client);
			std::string username;
			datastream >> username;
			std::shared_ptr<Group> group = groupDB.getGroup(group_id);
			if (group)
			{
				if (group->get_owner() != user)
				{
					result.message = "You are not the owner of the group\n";
				}
				else
				{
					auto join_requests = group->get_join_requests();
					bool found = false;
					for (auto const &join_request : join_requests)
					{
						if (join_request->getUsername() == username)
						{
							group->add_user(join_request);
							group->remove_join_request(join_request);
							result.message = "Request accepted\n";
							found = true;
							break;
						}
					}
					if (!found)
					{
						result.message = "This user has not sent a request\n";
					}
				}
			}
			else
			{
				result.message = "Group does not exist\n";
			}
		}
		else
		{
			result.success = false;
			result.message = "Not a group request\n";
			return result;
		}
	}
	catch (std::out_of_range &_)
	{
		result.message = login_required_warning;
	}
	result.success = true;
	return result;
}

void process_request(std::shared_ptr<Transaction> transaction, std::shared_ptr<TCPSocket> client, bool should_respond)
{
	std::stringstream ss(transaction->data);
	std::string request;
	ss >> request;
	Result result;
	if (std::holds_alternative<UserRequest>(transaction->request))
	{
		result = process_user_request(client, std::get<UserRequest>(transaction->request), ss);
	}
	else if (std::holds_alternative<GroupRequest>(transaction->request))
	{
		result = process_group_request(client, std::get<GroupRequest>(transaction->request), ss);
	}
	else
	{
		result.success = false;
		result.message = "Invalid request\n";
	}
	if (should_respond)
	{
		client->send_data(result.message);
	}
}
