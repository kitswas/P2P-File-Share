#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "../models/endpoint.hpp"
#include "../models/file.hpp"
#include "../models/fileinfo.hpp"
#include "file_id.hpp"

class Peer
{
public:
	const EndpointID id;
	const Endpoint endpoint;
	/**
	 * @brief The files that the peer has. (partially or completely)
	 */
	std::unordered_set<file_id> files;

	Peer(const Endpoint &endpoint, const EndpointID &id) : id(id), endpoint(endpoint) {}
};

/**
 * @brief A thread-safe database of peers.
 *
 */
class PeerDB
{
private:
	std::mutex mutex;
	std::unordered_map<EndpointID, std::shared_ptr<Peer>> peers;

public:
	bool addPeer(EndpointID id, const Endpoint &endpoint);
	bool removePeer(EndpointID id);
	bool addFile(EndpointID peer_id, const std::string &group_id, const std::string &file_name);
	bool removeFile(EndpointID peer_id, const std::string &group_id, const std::string &file_name);
	
	/**
	 * @brief Get a list of peers that have the file.
	 *
	 * @param group_id The group id of the file.
	 * @param file_name The name of the file.
	 * @return std::vector<std::shared_ptr<Peer>> A list of peers that have the file.
	 */
	std::vector<std::shared_ptr<Peer>> getPeers(const std::string &group_id, const std::string &file_name);
};
