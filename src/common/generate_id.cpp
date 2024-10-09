#pragma once

#include <cstdint>
#include <string>

#include "../models/endpoint.hpp"

EndpointID generate_id(const Endpoint &endpoint)
{
	return std::hash<std::string>{}(endpoint.to_string());
}
