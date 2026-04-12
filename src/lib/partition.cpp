#include <stdexcept>

#include "partition.hpp"



void partition_with_mask_hashing(const uint64_t* __restrict__ in, uint32_t* __restrict__ out, uint32_t P, std::size_t N) {
    const uint32_t mask = P - 1u;
    for (std::size_t i = 0; i < N; ++i) {
        out[i] = in[i] & mask;
    }
}



uint64_t kMul = 11400714819323198485ull;

void partition_with_xorshift_hashing(const uint64_t* __restrict__ in, uint32_t* __restrict__ out, uint32_t P, std::size_t N) {
    const uint32_t mask = P - 1u;
    for (std::size_t i = 0; i < N; ++i) {
        uint64_t k = in[i];
        k ^= k >> 33;
        k ^= k >> 17;
        k ^= k >> 9;
        out[i] = static_cast<uint32_t>(k) & mask;
    }
}



constexpr std::uint64_t C1 = 0xff51afd7ed558ccdull;
constexpr std::uint64_t C2 = 0xc4ceb9fe1a85ec53ull;

void partition_with_fmix32fold_hashing(const uint64_t* __restrict__ in, uint32_t* __restrict__ out, uint32_t P, std::size_t N) {
    const uint32_t mask = P - 1u;
    for (std::size_t i = 0; i < N; ++i) {
        uint32_t x = static_cast<uint32_t>(in[i]) ^ static_cast<uint32_t>(in[i] >> 32);
        x ^= x >> 16;
        x *= 0x85ebca6bu;
        x ^= x >> 13;
        x *= 0xc2b2ae35u;
        x ^= x >> 16;
        out[i] = x & mask;
    }
}

