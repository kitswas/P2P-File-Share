#pragma once

#include <memory>
#include <string>
#include <unordered_set>

#include "endpoint.hpp"
#include "fileinfo.hpp"

class File
{
public:
	std::unordered_set<std::shared_ptr<Endpoint>> seeders;
	const std::shared_ptr<FileInfo> file_info;
	const std::string group_id;

	File(std::shared_ptr<FileInfo> file_info, std::string const &group_id) : file_info(file_info), group_id(group_id) {}

	friend bool operator==(const File &lhs, const File &rhs)
	{
		return lhs.file_info->hash == rhs.file_info->hash && lhs.group_id == rhs.group_id;
	}
};
