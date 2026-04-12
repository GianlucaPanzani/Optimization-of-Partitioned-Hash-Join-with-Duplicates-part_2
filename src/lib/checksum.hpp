#ifndef CHECKSUM_HPP
#define CHECKSUM_HPP

#include <cstdint>
#include <vector>

uint64_t compute_checksum(const std::vector<uint32_t>& data);

#endif