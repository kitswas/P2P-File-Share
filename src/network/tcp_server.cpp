#include <iostream>
#include <memory>
#include <thread>

#include "network_error.hpp"
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

__attribute__((noreturn)) void connectionHandler(
	std::shared_ptr<TCPSocket> client,
	std::function<void(std::shared_ptr<TCPSocket>)> const &onConnect,
	std::function<void(std::shared_ptr<TCPSocket>, std::string &)> const &onData,
	std::function<void(std::shared_ptr<TCPSocket>)> const &onDisconnect)
{
	if (onConnect)
	{
		onConnect(client);
	}

	while (true)
	{
		try
		{
			std::string data = client->receive_data();
			if (onData && !data.empty())
			{
				onData(client, data);
			}
		}
		catch (NetworkError &_)
		{
			break;
		}
	}
	client->disconnect();

	if (onDisconnect)
	{
		onDisconnect(client);
	}

	exit(EXIT_SUCCESS);
}

__attribute__((noreturn)) void TCPServer::acceptConnections()
{
	while (true)
	{
		if (clients.size() < maxConnections)
		{
			std::shared_ptr<TCPSocket> client = socket.accept();
			clients.push_back(client);

			std::thread(connectionHandler, client, onConnect, onData,
						[this](std::shared_ptr<TCPSocket> client)
						{
							if (onDisconnect)
							{
								onDisconnect(client);
							}
							clients.remove(client);
						})
				.detach();
		}
	}
	exit(EXIT_SUCCESS);
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

	std::thread(&TCPServer::acceptConnections, this).detach();

	return true;
}

void TCPServer::stop()
{
	socket.disconnect();
	for (auto const &client : clients)
	{
		client->disconnect();
	}
}
