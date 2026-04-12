#ifndef PARTITION_AVX2_HPP
#define PARTITION_AVX2_HPP

#include <cstdint>
#include <string>
#include <vector>

void partition_with_mask_hashing_avx2(const uint64_t* in, uint32_t* out, uint32_t P, std::size_t N);

#endif
