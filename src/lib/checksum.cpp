#include <cstdint>
#include <vector>

#include "checksum.hpp"

uint64_t compute_checksum(const std::vector<uint32_t>& data) {
    // Offset basis
    uint64_t hash = 1469598103934665603ULL;
    // Constant with which "shuffle" the hash value multipling it
    const uint64_t mul_const = 1099511628211ULL;

    // Generate checksum
    for (uint32_t x : data) {
        hash ^= static_cast<uint64_t>(x); // logical XOR to incorporate x into the hash value
        hash *= mul_const;
    }

    return hash;
}