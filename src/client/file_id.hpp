#pragma once

#include <string>

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
