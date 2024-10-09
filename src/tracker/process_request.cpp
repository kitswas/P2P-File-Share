#include "process_request.hpp"
#include "groupdb.hpp"
#include "userdb.hpp"
#include "transactionsrecord.hpp"

#include <sstream>

static GroupDB groupDB;

static std::unordered_map<EndpointID, std::shared_ptr<User>> logged_in_users;
static UserDB userDB;

extern TransactionsRecord successful_transactions;

bool is_logged_in(EndpointID client)
{
	return logged_in_users.find(client) != logged_in_users.end();
}

Result process_user_request(EndpointID origin, UserRequest request, std::stringstream &datastream)
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
			result.success = true;
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
			result.success = true;
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
		if (user && !is_logged_in(origin) && user->checkPassword(password))
		{
			logged_in_users.try_emplace(origin, user);
			result.message = "Login successful\n";
			result.success = true;
		}
		else
		{
			result.message = "Login failed\n";
		}
	}
	else if (request == UserRequest::LOGOUT)
	{
		if (logged_in_users.erase(origin))
		{
			result.message = "Logout successful\n";
			result.success = true;
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

	return result;
}

Result process_group_request(EndpointID origin, GroupRequest request, std::stringstream &datastream)
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
		result.message = (response.empty() ? "No groups found\n" : response);
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
			user = logged_in_users.at(origin);
			if (groupDB.createGroup(group_id, user))
			{
				result.message = "Group created\n";
				result.success = true;
			}
			else
			{
				result.message = "Group already exists\n";
			}
		}
		else if (request == GroupRequest::JOIN)
		{
			user = logged_in_users.at(origin);
			std::shared_ptr<Group> group = groupDB.getGroup(group_id);
			if (group && !group->has_member(user))
			{
				group->add_join_request(user);
				result.message = "Join request sent\n";
				result.success = true;
			}
			else
			{
				result.message = "Invalid request\n";
			}
		}
		else if (request == GroupRequest::LEAVE)
		{
			user = logged_in_users.at(origin);
			std::shared_ptr<Group> group = groupDB.getGroup(group_id);
			if (group && group->remove_user(user))
			{
				result.message = "Left group\n";
				result.success = true;
			}
			else
			{
				result.message = "Invalid request\n";
			}
		}
		else if (request == GroupRequest::LIST_JOIN_REQUESTS)
		{
			user = logged_in_users.at(origin);
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
			user = logged_in_users.at(origin);
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
							result.success = true;
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
	return result;
}

void process_request(std::shared_ptr<Transaction> transaction, std::shared_ptr<TCPSocket> client, bool do_mirror)
{
	std::stringstream ss(transaction->data);
	EndpointID _id;
	ss >> _id; // Discard the id
	std::string _request;
	ss >> _request; // Discard the request

	Result result;
	if (std::holds_alternative<UserRequest>(transaction->request))
	{
		result = process_user_request(transaction->origin, std::get<UserRequest>(transaction->request), ss);
	}
	else if (std::holds_alternative<GroupRequest>(transaction->request))
	{
		result = process_group_request(transaction->origin, std::get<GroupRequest>(transaction->request), ss);
	}
	else if (std::holds_alternative<TrackerRequest>(transaction->request))
	{
		do_mirror = false;
		transaction->outcome.success = false; // We do not want to store tracker requests
		client->set_non_blocking(false);	  // We want to block until all data is sent
		if (std::get<TrackerRequest>(transaction->request) == TrackerRequest::SYNC)
		{
			for (auto const &t : successful_transactions.getTransactions())
			{
				client->send_data(t.data);
				client->receive_data(); // Wait for the tracker to process the data
			}
			client->send_data("done");
		}
		client->set_non_blocking(true); // Set back to non-blocking
	}
	else
	{
		result.success = false;
		result.message = "Invalid request\n";
	}
	transaction->outcome = result;

	if (do_mirror)
	{
		client->send_data(result.message);
	}
}
