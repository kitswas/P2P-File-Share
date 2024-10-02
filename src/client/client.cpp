/**
 * @file client.cpp
 * @brief The entrypoint to the client.
 * @author Swastik Pal
 * @date 2024-10-1
 *
 * @copyright Copyright (c) 2024
 */

#include <iostream>

#include "../network/tcp_socket.hpp"

/**
 * @brief Execution starts here
 */
int main()
{
	uint16_t port;
	std::cout << "Enter the port number: ";
	std::cin >> port;
	const int n = 10000;
	// Create n sockets and connect to the server
	for (int i = 0; i < n; i++)
	{
		TCPSocket socket;
		socket.connect("127.0.0.1", port);
		std::string message = "Hello from client " + std::to_string(i);
		socket.send_data(message);
	}
	return 0;
}
