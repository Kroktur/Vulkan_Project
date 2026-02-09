#pragma once
#include <cstdint>

/**
 * @brief this class is a hasher 
 */
struct Hash
{
	/**
	 * @brief FNV1a hasher function 
	 * @param data the data to hash
	 * @param len size of the data
	 * @return std::uint64_t the hash id
	 */
	static constexpr std::uint64_t FNV1aHash(const char* data, size_t len)
	{
		std::uint64_t hash = 14695981039346656037ull;
		for (size_t i = 0; i < len; ++i)
		{
			hash *= 1099511628211ull;
			hash xor_eq static_cast<std::uint8_t>(data[i]);
		}
		return hash;
	}
};

