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

#include "../common/load_tracker_info.cpp"
#include "../models/endpoint.hpp"
#include "../network/network_errors.hpp"
#include "../network/tcp_socket.hpp"
#include "../network/tcp_server.hpp"
#include "downloadmanager.hpp"
#include "filesdb.hpp"
#include "peerdb.hpp"
#include "process_input.hpp"
#include "process_request.hpp"

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

void loop(const Endpoint &client_endpoint, TCPSocket &tracker, const EndpointID &my_id)
{
	FilesDB my_files;
	PeerDB peer_db;
	DownloadManager download_manager(my_files, peer_db, client_endpoint, my_id);

	std::function<void(std::shared_ptr<TCPSocket>)> onConnect = [](std::shared_ptr<TCPSocket> client)
	{
		std::cout << "Client connected " << client->get_peer_ip() << ":" << client->get_peer_port() << std::endl;
	};
	std::function<void(std::shared_ptr<TCPSocket>)> onDisconnect = [](std::shared_ptr<TCPSocket> client)
	{
		std::cout << "Client disconnected " << client->get_peer_ip() << ":" << client->get_peer_port() << std::endl;
	};
	std::function<void(std::shared_ptr<TCPSocket>, std::string &)> onData = [&peer_db, &my_files, my_id](std::shared_ptr<TCPSocket> client, std::string &data)
	{
		std::cout << "Data received from " << client->get_peer_ip() << ":" << client->get_peer_port() << " : " << data << std::endl;
		process_request(client, peer_db, my_files, data, my_id);
	};

	TCPServer server(100);
	server.setOnConnect(onConnect);
	server.setOnDisconnect(onDisconnect);
	server.setOnData(onData);
	server.start(client_endpoint.ip, client_endpoint.port);

	// Parse user input
	while (true)
	{
		std::string input;
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
			process_input(input, tracker, my_id, client_endpoint, download_manager, my_files, peer_db);
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

	std::cout << "Shutting down server..." << std::endl;
	server.stop();
	std::cout << "Stopping downloads..." << std::endl;
	download_manager.pause_downloads();
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
		my_id = Endpoint::generate_id(client_endpoint);
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

	try
	{
		loop(client_endpoint, tracker, my_id);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
