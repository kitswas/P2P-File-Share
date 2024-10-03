#pragma once

#include <memory>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

/**
 * @brief A class representing a TCP socket
 *
 * This class is used to create and manage TCP sockets.
 * A TCPSocket object can only be moved, not copied.
 *
 */
class TCPSocket
{
	friend class TCPServer;

private:
	/**
	 * @brief The file descriptor for this socket
	 */
	int socket_fd;
	/**
	 * @brief The details of this socket
	 */
	struct sockaddr_in local_address;
	/**
	 * @brief The details of the connected peer
	 */
	struct sockaddr_in peer_address;

public:
	TCPSocket();
	~TCPSocket();
	TCPSocket &operator=(const TCPSocket &) = delete;
	TCPSocket(const TCPSocket &) = delete;
	TCPSocket(TCPSocket &&src) noexcept;
	TCPSocket &operator=(TCPSocket &&src);

	/**
	 * @brief Bind the socket to a port
	 * @return true on success, false on failure
	 */
	bool bind(uint16_t port);

	/**
	 * @brief Listen for incoming connections
	 * @param listen_backlog The maximum number of pending connections
	 * @return true on success, false on failure
	 */
	bool listen(int listen_backlog);

	/**
	 * @brief Accept incoming connection
	 * @return A new TCPSocket object representing the connection
	 */
	std::unique_ptr<TCPSocket> accept();

	/**
	 * @brief Connect to a server at a given ip and port
	 * @return true on success, false on failure
	 */
	bool connect(const std::string &ip, uint16_t port);

	/**
	 * @brief Disconnect from whatever the socket is connected to
	 * @return true on success, false on failure
	 */
	bool disconnect();

	/**
	 * @brief Send data over the socket
	 * @return The number of bytes sent
	 */
	ssize_t send_data(const std::string &data);

	/**
	 * @brief Receive data from the socket
	 * @return The received data
	 */
	std::string receive_data(bool peek = false);

	/**
	 * @brief Get the IP address of the connected peer
	 * @return The IP address of the connected peer
	 */
	std::string get_peer_ip() const;

	/**
	 * @brief Get the port of the connected peer
	 * @return The port of the connected peer
	 */
	int get_peer_port() const;

	/**
	 * @brief Get the IP address of the local socket
	 * @return The IP address of the local socket
	 */
	std::string get_local_ip() const;

	/**
	 * @brief Get the port of the local socket
	 * @return The port of the local socket
	 */
	int get_local_port() const;

	/**
	 * @brief Set the socket non-blocking mode
	 * @return true on success, false on failure
	 */
	bool set_non_blocking(bool non_blocking);

	friend bool operator==(const TCPSocket &lhs, const TCPSocket &rhs) noexcept
	{
		return lhs.socket_fd == rhs.socket_fd;
	}
};
