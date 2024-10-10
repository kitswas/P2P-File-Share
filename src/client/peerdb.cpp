#include "peerdb.hpp"

bool PeerDB::addPeer(EndpointID id, const Endpoint &endpoint)
{
	std::scoped_lock lock(mutex);
	if (peers.find(id) != peers.end())
	{
		return false;
	}

	peers[id] = std::make_shared<Peer>(endpoint, id);
	return true;
}

bool PeerDB::removePeer(EndpointID id)
{
	std::scoped_lock lock(mutex);
	if (peers.find(id) == peers.end())
	{
		return false;
	}

	peers.erase(id);
	return true;
}

bool PeerDB::addFile(EndpointID peer_id, const std::string &group_id, const std::string &file_name)
{
	std::scoped_lock lock(mutex);
	if (peers.find(peer_id) == peers.end())
	{
		return false;
	}

	peers[peer_id]->files.emplace(group_id, file_name);
	return true;
}

bool PeerDB::removeFile(EndpointID peer_id, const std::string &group_id, const std::string &file_name)
{
	std::scoped_lock lock(mutex);
	if (peers.find(peer_id) == peers.end())
	{
		return false;
	}

	peers[peer_id]->files.erase({group_id, file_name});
	return true;
}
