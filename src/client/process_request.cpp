#include "process_request.hpp"

#include <sstream>

void process_request(std::shared_ptr<TCPSocket> client, PeerDB &peer_db, const std::string &request, const EndpointID &my_id)
{
	Endpoint client_endpoint = {client->get_peer_ip(), client->get_peer_port()};
	std::stringstream ss(request);
	std::string command;
	ss >> command;

	if (command == "get")
	{
		std::string group_id;
		ss >> group_id;
		std::string file_name;
		ss >> file_name;
	}
	else
	{
		client->send_data("Invalid command");
	}
}
