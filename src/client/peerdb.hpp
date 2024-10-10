#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "../models/endpoint.hpp"
#include "../models/file.hpp"
#include "../models/fileinfo.hpp"

/**
 * @brief The first string is the group id and the second string is the file name.
 *
 */
struct file_id
{
	const std::string group_id;
	const std::string file_name;

	file_id(const std::string &group_id, const std::string &file_name) : group_id(group_id), file_name(file_name) {}

	std::string to_string() const
	{
		return group_id + " " + file_name;
	}

	friend bool operator==(const file_id &lhs, const file_id &rhs)
	{
		return lhs.group_id == rhs.group_id && lhs.file_name == rhs.file_name;
	}
};

// Hash function for file_id
namespace std
{
	template <>
	struct hash<file_id>
	{
		std::size_t operator()(const file_id &id) const
		{
			return std::hash<std::string>()(id.group_id) ^ (std::hash<std::string>()(id.file_name) << 1);
		}
	};
}

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
};
