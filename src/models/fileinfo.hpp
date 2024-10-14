#pragma once

#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// 512KB
constexpr size_t block_size = 512 * 1024;

class FileInfo
{
public:
	/**
	 * @brief The filename
	 */
	const std::string name;
	/**
	 * @brief The size of the file (in bytes)
	 */
	const size_t size;
	/**
	 * @brief The list of SHA1 hashes of the pieces of the file
	 * Each piece is should be @link block_size @endlink bytes long
	 */
	const std::vector<std::string> pieces;
	/**
	 * @brief The hash of the file
	 * Equivalent to the hash of the concatenation of the pieces
	 */
	const std::string hash;

	FileInfo(const std::string &name, size_t size, const std::vector<std::string> &pieces, const std::string &hash)
		: name(name), size(size), pieces(pieces), hash(hash)
	{
	}

	std::string to_string() const
	{
		std::string str = "File: " + name + "\t";
		str += "Size: " + std::to_string(size) + " bytes\t";
		str += "Pieces: ";
		for (const auto &piece : pieces)
		{
			str += piece + " ";
		}
		str += "\n";
		return str;
	}

	static FileInfo from_string(const std::string &str)
	{
		std::string name;
		size_t size;
		std::vector<std::string> pieces;
		std::string hash = "";
		std::string piece;
		std::string temp;
		std::stringstream ss(str);
		ss >> temp >> name >> temp >> size >> temp >> temp;
		while (ss >> piece)
		{
			pieces.push_back(piece);
			hash += piece;
		}
		return FileInfo(name, size, pieces, hash);
	}

	friend bool operator==(const FileInfo &lhs, const FileInfo &rhs) noexcept
	{
		return lhs.name == rhs.name;
	}
};
