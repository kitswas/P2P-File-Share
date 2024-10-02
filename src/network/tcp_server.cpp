#include <cstring>
#include <iostream>
#include <memory>
#include <sys/time.h>
#include <thread>

#include "network_errors.hpp"
#include "tcp_server.hpp"

TCPServer::TCPServer(size_t maxConnections) : maxConnections(maxConnections)
{
}

void TCPServer::setOnConnect(std::function<void(std::shared_ptr<TCPSocket>)> const &onConnect)
{
	this->onConnect = onConnect;
}

void TCPServer::setOnDisconnect(std::function<void(std::shared_ptr<TCPSocket>)> const &onDisconnect)
{
	this->onDisconnect = onDisconnect;
}

void TCPServer::setOnData(std::function<void(std::shared_ptr<TCPSocket>, std::string &)> const &onData)
{
	this->onData = onData;
}

void TCPServer::serveRequests(const int wait_time)
{
	//! TODO: Refactor this beast

	fd_set active_fd_set;
	fd_set read_fd_set;
	FD_ZERO(&active_fd_set);
	FD_SET(socket.local_fd, &active_fd_set);

	// This is somewhat similar to Node.js's event loop
	while (running)
	{
		read_fd_set = active_fd_set;
		// Block until input arrives on one or more active sockets
		struct timeval timeout;
		timeout.tv_sec = wait_time;
		timeout.tv_usec = 0;

		if (select(FD_SETSIZE, &read_fd_set, nullptr, nullptr, &timeout) < 0)
		{
			throw NetworkError(strerror(errno));
		}

		// Serve all the sockets with input pending
		for (int i = 0; i < FD_SETSIZE; ++i)
		{
			if (FD_ISSET(i, &read_fd_set))
			{
				if (i == socket.local_fd)
				{
					// Connection request on original socket
					auto client = socket.accept();
					if (client)
					{
						client->set_non_blocking(true); // Set the client socket to non-blocking
						std::scoped_lock<std::mutex> lock(clients_list_mutex);
						if (clients.size() < maxConnections)
						{
							FD_SET(client->peer_fd, &active_fd_set);
							clients.push_back(std::move(client));
							if (onConnect)
							{
								onConnect(clients.back());
							}
						}
						else
						{
							client->disconnect();
						}
					}
				}
				else // Data arriving on an already-connected socket
				{
					std::scoped_lock<std::mutex> lock(clients_list_mutex);
					auto it = std::find_if(clients.begin(), clients.end(), [i](std::shared_ptr<TCPSocket> const &client)
										   { return client->peer_fd == i; });
					if (it != clients.end())
					{
						auto client = *it;
						try
						{
							std::string data = client->receive_data();
							if (onData && !data.empty())
							{
								onData(client, data);
							}
						}
						catch (WouldBlockError &_)
						{
							// Ignore and continue, this is a regular occurrence
						}
						catch (ConnectionClosedError &_)
						{
							FD_CLR(client->peer_fd, &active_fd_set);
							clients.erase(it);
							client->disconnect();
							if (onDisconnect)
							{
								onDisconnect(client);
							}
						}
						catch (NetworkError &e)
						{
							std::cerr << "Error: " << e.what() << std::endl;
							FD_CLR(client->peer_fd, &active_fd_set);
							clients.erase(it);
							client->disconnect();
							if (onDisconnect)
							{
								onDisconnect(client);
							}
						}
					}
				}
			}
		}
	}
}

bool TCPServer::start(uint16_t port)
{
	if (!socket.bind(port))
	{
		return false;
	}

	if (!socket.listen())
	{
		return false;
	}

	std::cout << "Server started on port " << socket.get_local_port() << std::endl;

	running = true;
	service_thread = std::thread(&TCPServer::serveRequests, this, 1);

	return true;
}

void TCPServer::stop()
{
	running = false;
	service_thread.join();
	socket.disconnect();
	for (auto const &client : clients)
	{
		client->disconnect();
	}
}
