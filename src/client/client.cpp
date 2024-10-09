/**
 * @file client.cpp
 * @brief The entrypoint to the client.
 * @author Swastik Pal
 * @date 2024-10-1
 *
 * @copyright Copyright (c) 2024
 */

#include <cstring>
#include <iostream>

#include "../common/generate_id.cpp"
#include "../common/load_tracker_info.cpp"
#include "../models/endpoint.hpp"
#include "../network/network_errors.hpp"
#include "../network/tcp_socket.hpp"

// List of trackers
static std::vector<Endpoint> trackers;

/**
 * @brief Attempt to connect to any of the trackers
 */
bool connect_to_tracker(TCPSocket &tracker)
{
	for (auto const &t : trackers)
	{
		try
		{
			if (tracker.connect(t.ip, t.port))
			{
				std::clog << "Connected to tracker " << t.ip << ":" << t.port << std::endl;
				return true;
			}
		}
		catch (const ConnectionClosedError &e)
		{
			std::cerr << t.ip << ":" << t.port << " " << e.what() << std::endl;
		}
	}
	return false;
}

/**
 * @brief Execution starts here
 */
int main(int argc, char *argv[])
{
	const char *file_path = nullptr;
	Endpoint client_endpoint;
	EndpointID my_id;

	// Parse args
	if (argc != 3)
	{
		std::cerr << "Usage: ../client <IP>:<PORT> tracker_info.txt" << std::endl;
		exit(1);
	}

	// Parse client endpoint
	std::string client_endpoint_str(argv[1]);
	try
	{
		client_endpoint = Endpoint::from_string(client_endpoint_str);
		my_id = generate_id(client_endpoint);
	}
	catch (const std::invalid_argument &e)
	{
		std::cerr << e.what() << std::endl;
		exit(1);
	}

	file_path = argv[2];

	// Load tracker info
	trackers = load_tracker_info(file_path);

	// Connect to tracker
	TCPSocket tracker;
	if (!connect_to_tracker(tracker))
	{
		std::cerr << "Failed to connect to trackers." << std::endl;
		exit(1);
	}

	std::cout << "My ID: " << my_id << std::endl;

	// Parse user input
	while (true)
	{
		std::string input;
		std::string response;
		if (std::cin.peek() != EOF) // check if there is input
		{
			std::getline(std::cin >> std::ws, input);
			if (input == "quit")
			{
				break;
			}
		}
		try
		{
			std::string request = "1" + std::to_string(my_id) + " " + input;
			tracker.send_data(request);
			response = tracker.receive_data();
			std::cout << response << std::endl;
		}
		catch (const ConnectionClosedError &e)
		{
			std::cerr << e.what() << '\n';
			std::cerr << "Failed to send or receive data. Reconnecting..." << std::endl;
			tracker = TCPSocket(); // Create a new socket
			if (!connect_to_tracker(tracker))
			{
				std::cerr << "Failed to reconnect" << std::endl;
				exit(1);
			}
		}
	}

	return 0;
}
