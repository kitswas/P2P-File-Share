#pragma once

#include <stdexcept>
#include <string>
#include <vector>

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
	 * Each piece is should be 512KB
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
		std::string str = "File: " + name + "\n";
		str += "Size: " + std::to_string(size) + " bytes\n";
		str += "Pieces: ";
		for (const auto &piece : pieces)
		{
			str += piece + " ";
		}
		str += "\nHash: " + hash;
		return str;
	}

	/**
	 * @brief Bencoding format available at:
	 * https://www.bittorrent.org/beps/bep_0003.html#bencoding
	 * @details
	 * - Strings are length-prefixed base ten followed by a colon and the string. For example 4:spam corresponds to 'spam'.
	 * - Integers are represented by an 'i' followed by the number in base 10 followed by an 'e'.
	 * For example i3e corresponds to 3 and i-3e corresponds to -3. Integers have no size limitation. i-0e is invalid.
	 * All encodings with a leading zero, such as i03e, are invalid, other than i0e, which of course corresponds to 0.
	 * - Lists are encoded as an 'l' followed by their elements (also bencoded) followed by an 'e'.
	 * For example l4:spam4:eggse corresponds to ['spam', 'eggs'].
	 * - Dictionaries are encoded as a 'd' followed by a list of alternating keys and their corresponding values
	 * followed by an 'e'. For example, d3:cow3:moo4:spam4:eggse corresponds to {'cow': 'moo', 'spam': 'eggs'}
	 * and d4:spaml1:a1:bee corresponds to {'spam': ['a', 'b']}.
	 * Keys must be strings and appear in sorted order (sorted as raw strings, not alphanumerics).
	 *
	 * @return A bencoded string representing the file info.
	 * e.g. `d4:name8:filename4:sizei1024e6:piecesl20:hash1...e4:hash20:hash2...e`
	 */
	std::string bencoded() const
	{
		std::string str = "d4:name" + std::to_string(name.size()) + ":" + name + "4:sizei" + std::to_string(size) + "e6:piecesl";
		for (const auto &piece : pieces)
		{
			str += "20:" + piece;
		}
		str += "e";
		return str;
	}

	/**
	 * @brief Parse a bencoded string to create a FileInfo object
	 * @param bencode The bencoded string
	 * @return The FileInfo object
	 * @see FileInfo::bencoded
	 */
	static FileInfo from_bencoded(std::string bencode)
	{
		std::string name;
		size_t size;
		std::vector<std::string> pieces;
		std::string hash = "";
		const std::invalid_argument error("Invalid bencoded string");
		size_t i = 0;
		try
		{
			// Skip d4:name
			i += 7;
			// Read size of name
			size_t name_size = std::stoi(bencode.substr(i, bencode.find(':', i) - i));
			i += std::to_string(name_size).size() + 1; // +1 for :
			// Read name
			name = bencode.substr(i, name_size);
			i += name_size;
			// Skip 4:sizei
			i += 7;
			// Read size
			size = std::stoi(bencode.substr(i, bencode.find('e', i) - i));
			i += std::to_string(size).size() + 1; // +1 for e
			// Skip 6:piecesl
			i += 9;
			// Read pieces list, each piece is 20:hash
			while (bencode[i] != 'e')
			{
				// Skip 20:
				i += 3;
				std::string piece = bencode.substr(i, 20);
				pieces.push_back(piece);
				hash += piece;
				i += 20;
			}
			// Now bencode[i] should be e
			if (bencode[i] != 'e')
			{
				throw error;
			}
		}
		catch (const std::out_of_range &_)
		{
			throw error;
		}

		return FileInfo(name, size, pieces, hash);
	}

	friend bool operator==(const FileInfo &lhs, const FileInfo &rhs) noexcept
	{
		return lhs.name == rhs.name;
	}
};
