#pragma once

#include <atomic>
#include <functional>
#include <list>
#include <mutex>
#include <thread>

#include "tcp_socket.hpp"

#define HARD_CONNECTION_LIMIT 256

class TCPServer
{
private:
	/**
	 * @brief The server socket.
	 */
	TCPSocket socket;

	/**
	 * @brief A separate thread to accept incoming connections.
	 */
	std::thread service_thread;

	/**
	 * @brief A flag to indicate if the server is running.
	 * The service_thread observes this flag to determine if it should continue running.
	 */
	std::atomic<bool> running;

	/**
	 * @brief The maximum number of clients the server can handle.
	 */
	const size_t maxConnections;

	/**
	 * @brief The function to handle incoming connections and data.
	 * This function is run in a separate thread.
	 * It uses the select system call to wait for incoming data on the server socket.
	 * When data is received, it calls the appropriate callback function.
	 * This function is responsible for managing the clients list.
	 *
	 * @param wait_time When running is set to false, this function exits after wait_time seconds.
	 */
	void serveRequests(const int wait_time);

	/**
	 * @brief The list of connected clients.
	 */
	std::list<std::shared_ptr<TCPSocket>> clients;

	/**
	 * @brief A mutex to protect the clients list from concurrent access.
	 */
	std::mutex clients_list_mutex;

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
	 * Will be capped at HARD_CONNECTION_LIMIT.
	 */
	explicit TCPServer(size_t maxConnections);

	TCPServer(const TCPServer &) = delete;
	TCPServer &operator=(const TCPServer &) = delete;
	TCPServer(TCPServer &&) = delete;
	~TCPServer();

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
	 * @param listen_backlog The maximum number of pending connections.
	 * @return true on success, false on failure.
	 */
	bool start(uint16_t port, int listen_backlog = 1);

	/**
	 * @brief Stop the server.
	 */
	void stop();
};
