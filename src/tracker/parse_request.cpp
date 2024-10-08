#include <sstream>

#include "parse_request.hpp"

std::shared_ptr<Transaction> parse_request(std::shared_ptr<TCPSocket> client, const std::string &data)
{
	std::stringstream ss(data);
	std::string request;
	ss >> request;
	std::shared_ptr<Transaction> transaction;
	Request req;
	if (request == "create_user")
	{
		req = UserRequest::CREATE;
	}
	else if (request == "delete_user")
	{
		req = UserRequest::DELETE;
	}
	else if (request == "list_users")
	{
		req = UserRequest::LIST;
	}
	else if (request == "create_group")
	{
		req = GroupRequest::CREATE;
	}
	else if (request == "join_group")
	{
		req = GroupRequest::JOIN;
	}
	else if (request == "leave_group")
	{
		req = GroupRequest::LEAVE;
	}
	else if (request == "list_groups")
	{
		req = GroupRequest::LIST;
	}
	else if (request == "list_join_requests")
	{
		req = GroupRequest::LIST_JOIN_REQUESTS;
	}
	else if (request == "accept_join_request")
	{
		req = GroupRequest::ACCEPT_JOIN_REQUEST;
	}
	else
	{
		throw UnknownRequest("Unknown request");
	}
	transaction = std::make_shared<Transaction>(Transaction{
		req,
		Endpoint{client->get_peer_ip(), client->get_peer_port()},
		data,
		Result{false, ""}});
	return transaction;
}