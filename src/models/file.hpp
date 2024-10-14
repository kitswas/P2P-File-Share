#pragma once

#include <memory>
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

	std::string to_string() const
	{
		std::stringstream ss;
		ss << seeders.size() << " ";
		for (auto const &seeder : seeders)
		{
			ss << seeder->to_string() << " ";
		}
		ss << group_id << " ";
		ss << file_info->to_string();
		return ss.str();
	}

	static File from_string(std::string const &str)
	{
		std::stringstream ss(str);
		size_t seeder_count;
		ss >> seeder_count;
		std::unordered_set<std::shared_ptr<Endpoint>> seeders;
		for (size_t i = 0; i < seeder_count; ++i)
		{
			std::string seeder_str;
			ss >> seeder_str;
			seeders.insert(std::make_shared<Endpoint>(Endpoint::from_string(seeder_str)));
		}
		std::string group_id;
		ss >> group_id;
		FileInfo file_info = FileInfo::from_string(ss.str().substr(ss.tellg()));
		File file(std::make_shared<FileInfo>(file_info), group_id);
		file.seeders = seeders;
		return file;
	}
};
