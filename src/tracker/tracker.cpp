/**
 * @file tracker.cpp
 * @brief The entrypoint to the tracker.
 * @author Swastik Pal
 * @date 2024-10-1
 *
 * @copyright Copyright (c) 2024
 */

#include <iostream>
#include <memory>
#include <thread>

#include "../network/tcp_server.hpp"

void loop()
{
	TCPServer server(2);
	std::function<void(std::shared_ptr<TCPSocket>)> onConnect = [](std::shared_ptr<TCPSocket> client)
	{
		std::cout << "Client connected " << client->get_peer_ip() << ":" << client->get_peer_port() << std::endl;
	};
	std::function<void(std::shared_ptr<TCPSocket>)> onDisconnect = [](std::shared_ptr<TCPSocket> client)
	{
		std::cout << "Client disconnected " << client->get_peer_ip() << ":" << client->get_peer_port() << std::endl;
	};
	std::function<void(std::shared_ptr<TCPSocket>, std::string &)> onData = [](std::shared_ptr<TCPSocket> client, std::string &data)
	{
		std::cout << "Data received from " << client->get_peer_ip() << ":" << client->get_peer_port() << " : " << data << std::endl;
	};
	server.setOnConnect(onConnect);
	server.setOnDisconnect(onDisconnect);
	server.setOnData(onData);

	server.start(0);
	while (true)
	{
		if (std::cin.peek() != EOF) // check if there is input
		{
			std::string input;
			std::cin >> input;
			std::cin.ignore(1000, '\n'); // ignore the rest of the line
			if (input == "quit")
			{
				break;
			}
		}
	}
}

/**
 * @brief Execution starts here
 */
int main()
{
	std::cout << "Hello from tracker!" << std::endl;
	loop();
	return 0;
}
