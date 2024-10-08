#pragma once

#include <string>
#include <variant>

#include "../models/endpoint.hpp"
#include "../network/tcp_socket.hpp"

enum class UserRequest
{
	CREATE,
	DELETE,
	LIST
};

enum class GroupRequest
{
	CREATE,
	JOIN,
	LEAVE,
	LIST,
	LIST_JOIN_REQUESTS,
	ACCEPT_JOIN_REQUEST,
};

using Request = std::variant<UserRequest, GroupRequest>;

struct Result
{
	bool success;
	std::string message;
};

/**
 * @brief Every request is a transaction.
 */
struct Transaction
{
	const Request request;
	const Endpoint source;
	const std::string data;

	Result outcome;
};
