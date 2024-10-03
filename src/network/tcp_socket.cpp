#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#include "network_errors.hpp"
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
			throw NetworkError(strerror(errno));
		}
	}

	local_address = {};
	local_address.sin_family = PF_INET;

	peer_address = {};
}

TCPSocket::~TCPSocket()
{
	// std::clog << "Destructor called for socket "
	// 		  << this->get_local_ip() << ":" << this->get_local_port() << std::endl;
	if (socket_fd != -1)
		close(socket_fd);
}

TCPSocket::TCPSocket(TCPSocket &&src) noexcept
{
	// Move the local file descriptor
	socket_fd = src.socket_fd;
	src.socket_fd = -1;

	// Move the local address
	local_address = src.local_address;
	src.local_address = {};

	// Move the peer address
	peer_address = src.peer_address;
	src.peer_address = {};

	// std::clog << "Move constructor called for socket "
	// 		  << this->get_local_ip() << ":" << this->get_local_port() << std::endl;
}

TCPSocket &TCPSocket::operator=(TCPSocket &&src)
{
	// std::clog << "Move assignment called for socket "
	// 		  << this->get_local_ip() << ":" << this->get_local_port() << std::endl;

	// Move the local file descriptor
	socket_fd = src.socket_fd;
	src.socket_fd = -1;

	// Move the local address
	local_address = src.local_address;
	src.local_address = {};

	// Move the peer address
	peer_address = src.peer_address;
	src.peer_address = {};

	return *this;
}

bool TCPSocket::bind(uint16_t port)
{
	local_address.sin_addr.s_addr = INADDR_ANY;
	local_address.sin_port = htons(port);
	socklen_t size = sizeof(local_address);

	if (::bind(socket_fd, (struct sockaddr *)&local_address, size) == -1)
	{
		switch (errno)
		{
		case 0:
			break;
		default:
			throw NetworkError(strerror(errno));
		}
	}

	if (getsockname(socket_fd, (struct sockaddr *)&local_address, &size) == -1)
	{
		switch (errno)
		{
		case 0:
			break;
		default:
			throw NetworkError(strerror(errno));
		}
	}

	return true;
}

bool TCPSocket::listen(int listen_backlog)
{
	if (::listen(socket_fd, listen_backlog) == -1)
	{
		switch (errno)
		{
		case 0:
			break;
		default:
			throw NetworkError(strerror(errno));
		}
	}

	return true;
}

std::unique_ptr<TCPSocket> TCPSocket::accept()
{
	auto client = std::make_unique<TCPSocket>();
	socklen_t size;
	client->socket_fd = ::accept(socket_fd, (struct sockaddr *)&client->peer_address, &size);

	if (client->socket_fd == -1)
	{
		switch (errno)
		{
		case 0:
			break;
		// case EBADF:
		// 	throw NetworkError("The socket argument is not a valid file descriptor");
		// case ENOTSOCK:
		// 	throw NetworkError("The descriptor socket argument is not a socket");
		// case EOPNOTSUPP:
		// 	throw NetworkError("The descriptor socket does not support this operation");
		case EWOULDBLOCK:
			throw WouldBlockError(strerror(errno));
		default:
			throw NetworkError(strerror(errno));
		}
	}

	return client;
}

bool TCPSocket::connect(const std::string &ip, uint16_t port)
{
	peer_address.sin_family = PF_INET;
	peer_address.sin_addr.s_addr = inet_addr(ip.c_str());
	peer_address.sin_port = htons(port);

	if (::connect(socket_fd, (struct sockaddr *)&peer_address, sizeof(peer_address)) == -1)
	{
		switch (errno)
		{
		case 0:
			break;
		case ECONNREFUSED:
			throw ConnectionClosedError(strerror(errno));
		case EINPROGRESS: // Non-blocking mode
			std::cout << "Connecting..." << std::endl;
			break;
		default:
			throw NetworkError(strerror(errno));
		}
	}

	return true;
}

bool TCPSocket::disconnect()
{
	if (socket_fd != -1)
		close(socket_fd);
	socket_fd = -1;

	return true;
}

ssize_t TCPSocket::send_data(const std::string &data)
{
	ssize_t sent = ::send(socket_fd, data.c_str(), data.size(), MSG_NOSIGNAL);
	if (sent == -1)
	{
		std::cerr << "Error: " << strerror(errno) << std::endl;
		switch (errno)
		{
		case EPIPE:
			throw ConnectionClosedError("Peer disconnected");
		case EWOULDBLOCK:
			throw WouldBlockError(strerror(errno));
		default:
			throw NetworkError(strerror(errno));
		}
	}
	return sent;
}

std::string TCPSocket::receive_data(bool peek)
{
	constexpr size_t buffer_size = 1024;
	char buffer[buffer_size] = {0};
	ssize_t valread = ::recv(socket_fd, buffer, buffer_size, peek ? MSG_PEEK : 0);
	if (valread == -1)
	{
		std::cerr << "Error: " << strerror(errno) << std::endl;
		if (errno == EWOULDBLOCK)
		{
			throw WouldBlockError(strerror(errno));
		}
		throw NetworkError(strerror(errno));
	}
	else if (valread == 0)
	{
		throw ConnectionClosedError("Peer disconnected");
	}

	return std::string(buffer, valread);
}

std::string TCPSocket::get_peer_ip() const
{
	return inet_ntoa(peer_address.sin_addr);
}

int TCPSocket::get_peer_port() const
{
	return ntohs(peer_address.sin_port);
}

std::string TCPSocket::get_local_ip() const
{
	return inet_ntoa(local_address.sin_addr);
}

int TCPSocket::get_local_port() const
{
	return ntohs(local_address.sin_port);
}

bool TCPSocket::set_non_blocking(bool non_blocking)
{
	int flags = fcntl(socket_fd, F_GETFL, 0);
	if (flags == -1)
	{
		return false;
	}

	if (non_blocking)
	{
		flags |= O_NONBLOCK;
	}
	else
	{
		flags &= ~O_NONBLOCK;
	}

	return fcntl(socket_fd, F_SETFL, flags) != -1;
}
