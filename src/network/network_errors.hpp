#pragma once

#include <stdexcept>

class NetworkError : public std::runtime_error
{
public:
	using runtime_error::runtime_error;
};

class WouldBlockError : public NetworkError
{
public:
	using NetworkError::NetworkError;
};

class ConnectionClosedError : public NetworkError
{
public:
	using NetworkError::NetworkError;
};
