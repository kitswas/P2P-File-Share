#include "process_request.hpp"
#include "userdb.hpp"

#include <sstream>

static UserDB userDB;

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
	else if (request == "get_user")
	{
		std::string username;
		datastream >> username;

		// Get user
		auto user = userDB.getUser(username);
		if (user)
		{
			client->send_data("User found\n");
		}
		else
		{
			client->send_data("User not found\n");
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
	else
	{
		return false;
	}
	return true;
}

void process_request(std::shared_ptr<TCPSocket> client, std::string &data)
{
	std::stringstream ss(data);
	std::string request;
	ss >> request;
	if (!process_user_request(client, request, ss))
	{
		client->send_data("Invalid request\n");
	}
}
