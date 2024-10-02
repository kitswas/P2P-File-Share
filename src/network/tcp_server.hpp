#pragma once

#include <functional>
#include <list>

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
	const size_t maxConnections;

	/**
	 * @brief Accept incoming connections.
	 */
	void acceptConnections();

	/**
	 * @brief The list of connected clients.
	 */
	std::list<std::shared_ptr<TCPSocket>> clients;

	/**
	 * @brief The function to call when a new client connects.
	 */
	std::function<void(std::shared_ptr<TCPSocket>)> onConnect;

	/**
	 * @brief The function to call when a client disconnects.
	 */
	std::function<void(std::shared_ptr<TCPSocket>)> onDisconnect;

	/**
	 * @brief The function to call when data is received from a client.
	 */
	std::function<void(std::shared_ptr<TCPSocket>, std::string &)> onData;

public:
	/**
	 * @brief Construct a new TCPServer object
	 *
	 * @param maxConnections The maximum number of clients the server can handle.
	 */
	explicit TCPServer(size_t maxConnections);

	/**
	 * @brief Set the function to call when a new client connects.
	 *
	 * @param onConnect The function to call when a new client connects.
	 */
	void setOnConnect(std::function<void(std::shared_ptr<TCPSocket>)> const &onConnect);

	/**
	 * @brief Set the function to call when a client disconnects.
	 *
	 * @param onDisconnect The function to call when a client disconnects.
	 */
	void setOnDisconnect(std::function<void(std::shared_ptr<TCPSocket>)> const &onDisconnect);

	/**
	 * @brief Set the function to call when data is received from a client.
	 *
	 * @param onData The function to call when data is received from a client.
	 */
	void setOnData(std::function<void(std::shared_ptr<TCPSocket>, std::string &)> const &onData);

	/**
	 * @brief Start the server.
	 *
	 * @param port The port to listen on.
	 * @return true on success, false on failure.
	 */
	bool start(uint16_t port);

	/**
	 * @brief Stop the server.
	 */
	void stop();
};
