#include <thread>

#include "tcp_server.hpp"

TCPServer::TCPServer(int maxConnections) : maxConnections(maxConnections)
{
}

void TCPServer::setOnConnect(std::function<void(TCPSocket)> onConnect)
{
	this->onConnect = onConnect;
}

void TCPServer::setOnDisconnect(std::function<void(TCPSocket)> onDisconnect)
{
	this->onDisconnect = onDisconnect;
}

void TCPServer::setOnData(std::function<void(TCPSocket, std::string)> onData)
{
	this->onData = onData;
}

int connectionHandler(TCPSocket client,
					  std::function<void(TCPSocket)> onConnect,
					  std::function<void(TCPSocket, std::string)> onData,
					  std::function<void(TCPSocket)> onDisconnect)
{
	if (onConnect)
	{
		onConnect(client);
	}

	while (true)
	{
		try
		{
			thread_local std::string data = client.receive_data();
			if (onData)
			{
				onData(client, data);
			}
		}
		catch (...)
		{
			break;
		}
	}
	client.disconnect();

	if (onDisconnect)
	{
		onDisconnect(client);
	}

	return 0;
}

bool TCPServer::start(int port)
{
	if (!socket.bind(port))
	{
		return false;
	}

	if (!socket.listen())
	{
		return false;
	}

	while (clients.size() < maxConnections)
	{
		TCPSocket client = socket.accept();
		clients.push_back(client);

		std::thread(connectionHandler, client, onConnect, onData, onDisconnect).detach();
	}

	return true;
}

void TCPServer::stop()
{
	socket.disconnect();
	for (auto &client : clients)
	{
		client.disconnect();
	}
}
