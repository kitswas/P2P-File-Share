#pragma once

#include <cstdint>
#include <string>

struct Endpoint
{
	std::string ip;
	uint16_t port;
	
	friend bool operator==(const Endpoint &lhs, const Endpoint &rhs) noexcept
	{
		return lhs.ip == rhs.ip && lhs.port == rhs.port;
	}
	
	std::string to_string() const
	{
		return ip + ":" + std::to_string(port);
	}
};
