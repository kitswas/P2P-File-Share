#pragma once

#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

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
	struct sockaddr_in socket_address;
	/**
	 * @brief The file descriptor for the client socket
	 */
	int client_fd;
	/**
	 * @brief The details of the client socket
	 */
	struct sockaddr_in client_address;
	/**
	 * @brief Listen backlog size
	 */
	const int listen_backlog = 3;

public:
	TCPSocket();
	~TCPSocket();

	/**
	 * @brief Bind the socket to a port
	 * @return true on success, false on failure
	 */
	bool bind(int port);

	/**
	 * @brief Listen for incoming connections
	 * @return true on success, false on failure
	 */
	bool listen();

	/**
	 * @brief Accept incoming connection
	 * @return A new TCPSocket object representing the connection
	 */
	TCPSocket accept();

	/**
	 * @brief Connect to a server at a given ip and port
	 * @return true on success, false on failure
	 */
	bool connect(const std::string &ip, int port);

	/**
	 * @brief Disconnect from whatever the socket is connected to
	 * @return true on success, false on failure
	 */
	bool disconnect();

	/**
	 * @brief Send data over the socket
	 * @return The number of bytes sent
	 */
	int send_data(const std::string &data);

	/**
	 * @brief Receive data from the socket
	 * @return The received data
	 */
	std::string receive_data();
};
