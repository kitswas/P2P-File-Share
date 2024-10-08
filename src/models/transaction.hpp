#pragma once

#include <string>
#include <variant>

#include "../models/endpoint.hpp"
#include "../network/tcp_socket.hpp"

enum class UserRequest
{
	CREATE,
	DELETE,
	LIST,
	LOGIN,
	LOGOUT,
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
	
	std::string to_string() const
	{
		return "Transaction: " + std::visit([](auto &&arg) -> std::string {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, UserRequest>)
			{
				return "UserRequest";
			}
			else if constexpr (std::is_same_v<T, GroupRequest>)
			{
				return "GroupRequest";
			}
			else // Programmer error, should never happen
			{
				static_assert(always_false<T>::value, "non-exhaustive visitor!");
			}
		}, request) + " from " + source.to_string() + " with data: " + data;
	}
};
