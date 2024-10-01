#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#include "tcp_socket.hpp"

TCPSocket::TCPSocket()
{
	socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (socket_fd == -1)
	{
		switch (errno)
		{
		case 0:
			break;
		default:
			throw std::runtime_error(strerror(errno));
		}
	}

	socket_address.sin_family = PF_INET;

	client_fd = -1;
}

TCPSocket::~TCPSocket()
{
	close(socket_fd);
	if (client_fd != -1)
	{
		close(client_fd);
	}
}

bool TCPSocket::bind(int port)
{
	socket_address.sin_addr.s_addr = INADDR_ANY;
	socket_address.sin_port = htons(port);

	if (::bind(socket_fd, (struct sockaddr *)&socket_address, sizeof(socket_address)) == -1)
	{
		switch (errno)
		{
		case 0:
			break;
		default:
			throw std::runtime_error(strerror(errno));
		}
	}

	return true;
}

bool TCPSocket::listen()
{
	if (::listen(socket_fd, listen_backlog) == -1)
	{
		switch (errno)
		{
		case 0:
			break;
		default:
			throw std::runtime_error(strerror(errno));
		}
	}

	return true;
}

TCPSocket TCPSocket::accept()
{
	TCPSocket client;
	size_t size;
	client.client_fd = ::accept(socket_fd, (struct sockaddr *)&client.client_address, (socklen_t *)&size);

	if (client.client_fd == -1)
	{
		switch (errno)
		{
		case 0:
			break;
		default:
			throw std::runtime_error(strerror(errno));
		}
	}

	return client;
}

bool TCPSocket::connect(const std::string &ip, int port)
{
	socket_address.sin_addr.s_addr = inet_addr(ip.c_str());
	socket_address.sin_port = htons(port);

	if (::connect(socket_fd, (struct sockaddr *)&socket_address, sizeof(socket_address)) == -1)
	{
		switch (errno)
		{
		case 0:
			break;
		default:
			throw std::runtime_error(strerror(errno));
		}
	}

	return true;
}

bool TCPSocket::disconnect()
{
	close(client_fd);
	client_fd = -1;

	return true;
}

int TCPSocket::send_data(const std::string &data)
{
	return ::send(client_fd, data.c_str(), data.size(), 0);
}

std::string TCPSocket::receive_data()
{
	constexpr int buffer_size = 1024;
	char buffer[buffer_size] = {0};
	ssize_t valread = ::read(client_fd, buffer, buffer_size);
	if (valread == -1)
	{
		throw std::runtime_error(strerror(errno));
	}

	return std::string(buffer, valread);
}
