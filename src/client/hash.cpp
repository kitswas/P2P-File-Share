#include "hash.hpp"
#include <openssl/sha.h>

std::string get_sha1(const int8_t *input, size_t length)
{
	unsigned char hash[SHA_DIGEST_LENGTH];
	SHA1(reinterpret_cast<const unsigned char *>(input), length, hash);
	std::string hash_str;
	hash_str.reserve(SHA_DIGEST_LENGTH * 2);
	for (size_t i = 0; i < SHA_DIGEST_LENGTH; ++i)
	{
		char out[3];
		snprintf(out, sizeof(out), "%02x", hash[i]);
		hash_str += out;
	}
	return hash_str;
}
