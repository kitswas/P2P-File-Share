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

std::vector<std::shared_ptr<Peer>> PeerDB::getPeers(const std::string &group_id, const std::string &file_name)
{
	std::scoped_lock lock(mutex);
	std::vector<std::shared_ptr<Peer>> result;
	for (const auto &[id, peer] : peers)
	{
		if (peer->files.find({group_id, file_name}) != peer->files.end())
		{
			result.push_back(peer);
		}
	}
	return result;
}
