#pragma once

#include <functional>

#include "tcp_socket.hpp"

class TCPServer
{
private:
	/**
	 * @brief The server socket.
	 */
	TCPSocket socket;

	/**
	 * @brief The maximum number of clients the server can handle.
	 */
	const int maxConnections;

	/**
	 * @brief The list of connected clients.
	 */
	std::vector<TCPSocket> clients;

	/**
	 * @brief The function to call when a new client connects.
	 */
	std::function<void(TCPSocket)> onConnect;

	/**
	 * @brief The function to call when a client disconnects.
	 */
	std::function<void(TCPSocket)> onDisconnect;

	/**
	 * @brief The function to call when data is received from a client.
	 */
	std::function<void(TCPSocket, std::string)> onData;

public:
	/**
	 * @brief Construct a new TCPServer object
	 *
	 * @param maxConnections The maximum number of clients the server can handle.
	 */
	TCPServer(int maxConnections);

	/**
	 * @brief Set the function to call when a new client connects.
	 *
	 * @param onConnect The function to call when a new client connects.
	 */
	void setOnConnect(std::function<void(TCPSocket)> onConnect);

	/**
	 * @brief Set the function to call when a client disconnects.
	 *
	 * @param onDisconnect The function to call when a client disconnects.
	 */
	void setOnDisconnect(std::function<void(TCPSocket)> onDisconnect);

	/**
	 * @brief Set the function to call when data is received from a client.
	 *
	 * @param onData The function to call when data is received from a client.
	 */
	void setOnData(std::function<void(TCPSocket, std::string)> onData);

	/**
	 * @brief Start the server.
	 *
	 * @param port The port to listen on.
	 * @return true on success, false on failure.
	 */
	bool start(int port);

	/**
	 * @brief Stop the server.
	 */
	void stop();
};
