#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>

using EndpointID = size_t;

struct Endpoint
{
	std::string ip;
	uint16_t port;

	friend bool operator==(const Endpoint &lhs, const Endpoint &rhs) noexcept
	{
		return lhs.ip == rhs.ip && lhs.port == rhs.port;
	}

	static Endpoint from_string(const std::string &str)
	{
		size_t colon_pos = str.find(':');
		if (colon_pos == std::string::npos)
		{
			throw std::invalid_argument("Invalid endpoint string");
		}
		return {str.substr(0, colon_pos), static_cast<uint16_t>(std::stoi(str.substr(colon_pos + 1)))};
	}

	std::string to_string() const
	{
		return ip + ":" + std::to_string(port);
	}

	static EndpointID generate_id(const Endpoint &endpoint)
	{
		return std::hash<std::string>{}(endpoint.to_string());
	}
};
