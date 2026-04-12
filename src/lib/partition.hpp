#ifndef PARTITION_HPP
#define PARTITION_HPP

#include <cstdint>
#include <string>
#include <vector>

void partition_with_mask_hashing(const uint64_t* __restrict__ in, uint32_t* __restrict__ out, uint32_t P, std::size_t N);
void partition_with_xorshift_hashing(const uint64_t* __restrict__ in, uint32_t* __restrict__ out, uint32_t P, std::size_t N);
void partition_with_fmix32fold_hashing(const uint64_t* __restrict__ in, uint32_t* __restrict__ out, uint32_t P, std::size_t N);

#endif
