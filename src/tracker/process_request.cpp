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

bool process_user_request(std::shared_ptr<TCPSocket> client, std::string_view request, std::stringstream &datastream)
{
	if (request == "create_user")
	{
		std::string username;
		std::string password;
		datastream >> username >> password;

		// Create user
		if (userDB.createUser(username, password))
		{
			client->send_data("User created\n");
		}
		else
		{
			client->send_data("User already exists\n");
		}
	}
	else if (request == "delete_user")
	{
		std::string username;
		datastream >> username;

		// Delete user
		if (userDB.deleteUser(username))
		{
			client->send_data("User deleted\n");
		}
		else
		{
			client->send_data("User does not exist\n");
		}
	}
	else if (request == "list_users")
	{
		// Get usernames
		auto usernames = userDB.getUsernames();
		std::string response = "";
		for (auto const &username : usernames)
		{
			response += username + "\n";
		}
		client->send_data(response.empty() ? "No users found\n" : response);
	}
	else if (request == "login")
	{
		std::string username;
		std::string password;
		datastream >> username >> password;
		auto user = userDB.getUser(username);
		if (user && !is_logged_in(client) && user->checkPassword(password))
		{
			logged_in_users.try_emplace(client, user);
			client->send_data("Login successful\n");
		}
		else
		{
			client->send_data("Login failed\n");
		}
	}
	else if (request == "logout")
	{
		if (logged_in_users.erase(client))
		{
			client->send_data("Logout successful\n");
		}
		else
		{
			client->send_data("Logout failed\n");
		}
	}
	else
	{
		return false;
	}
	return true;
}

bool process_group_request(std::shared_ptr<TCPSocket> client, std::string_view request, std::stringstream &datastream)
{
	const char *const login_required_warning = "You must be logged in to perform this action\n";

	if (request == "list_groups")
	{
		auto groups = groupDB.getGroups();
		std::string response = "";
		for (auto const &group : groups)
		{
			response += group + "\n";
		}
		client->send_data(response.empty() ? "No users found\n" : response);
		return true;
	}

	std::shared_ptr<User> user = nullptr;
	try
	{
		user = logged_in_users.at(client);
		std::string group_id;
		datastream >> group_id;
		// The following require user login
		if (request == "create_group")
		{
			if (groupDB.createGroup(group_id, user))
			{
				client->send_data("Group created\n");
			}
			else
			{
				client->send_data("Group already exists\n");
			}
		}
		else if (request == "join_group")
		{
			std::shared_ptr<Group> group = groupDB.getGroup(group_id);
			if (group && !group->has_user(user))
			{
				group->add_join_request(user);
				client->send_data("Join request sent\n");
			}
			else
			{
				client->send_data("Invalid request\n");
			}
		}
		else if (request == "leave_group")
		{
			std::shared_ptr<Group> group = groupDB.getGroup(group_id);
			if (group && group->remove_user(user))
			{
				client->send_data("Left group\n");
			}
			else
			{
				client->send_data("Invalid request\n");
			}
		}
		else if (request == "list_requests")
		{
			std::shared_ptr<Group> group = groupDB.getGroup(group_id);
			if (group)
			{
				auto requests = group->get_join_requests();
				std::string response = "";
				for (auto const &request : requests)
				{
					response += request->getUsername() + "\n";
				}
				client->send_data(response.empty() ? "No pending requests\n" : response);
			}
			else
			{
				client->send_data("Group does not exist\n");
			}
		}
		else if (request == "accept_request")
		{
			std::string username;
			datastream >> username;
			std::shared_ptr<Group> group = groupDB.getGroup(group_id);
			if (group)
			{
				if (group->get_owner() != user)
				{
					client->send_data("You are not the owner of the group\n");
				}
				else
				{
					auto requests = group->get_join_requests();
					bool found = false;
					for (auto const &request : requests)
					{
						if (request->getUsername() == username)
						{
							group->add_user(request);
							group->remove_join_request(request);
							client->send_data("Request accepted\n");
							found = true;
							break;
						}
					}
					if (!found)
					{
						client->send_data("This user has not sent a request\n");
					}
				}
			}
			else
			{
				client->send_data("Group does not exist\n");
			}
		}
		else
		{
			return false;
		}
	}
	catch (std::out_of_range &_)
	{
		client->send_data(login_required_warning);
	}
	return true;
}

void process_request(std::shared_ptr<TCPSocket> client, std::string &data)
{
	std::stringstream ss(data);
	std::string request;
	ss >> request;
	if (!process_user_request(client, request, ss) && !process_group_request(client, request, ss))
	{
		client->send_data("Unknown request\n");
	}
}
