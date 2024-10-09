#pragma once

#include <stdexcept>
#include <string>
#include <variant>

#include "../models/endpoint.hpp"
#include "../network/tcp_socket.hpp"

enum class RequestType
{
	USER,
	GROUP,
};

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

enum class TrackerRequest
{
	SYNC,
};

using Request = std::variant<UserRequest, GroupRequest, TrackerRequest>;

class UnknownRequest : public std::runtime_error
{
public:
	using runtime_error::runtime_error;
};

struct Result
{
	bool success = false;
	std::string message = "Not yet processed";
};

/**
 * @brief Workaround for static_assert(false, ...)
 */
template <typename T>
struct always_false : std::false_type
{
};

/**
 * @brief Every request is a transaction.
 */
struct Transaction
{
	const EndpointID origin;
	const Request request;
	const std::string data;

	Result outcome;

	std::string to_string() const
	{
		return std::visit([](auto &&arg) -> std::string
						  {
			// Magic: https://en.cppreference.com/w/cpp/utility/variant/visit
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, UserRequest>)
			{
				return "UserRequest " +  std::to_string(static_cast<int>(arg));
			}
			else if constexpr (std::is_same_v<T, GroupRequest>)
			{
				return "GroupRequest " +  std::to_string(static_cast<int>(arg));
			}
			else if constexpr (std::is_same_v<T, TrackerRequest>)
			{
				return "TrackerRequest " +  std::to_string(static_cast<int>(arg));
			}
			else // Programmer error, should never happen
			{
				static_assert(always_false<T>::value, "non-exhaustive visitor!");
			} }, request) +
			   " from " + std::to_string(origin) +
			   " with data: " + data;
	}
};
