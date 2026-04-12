#include <immintrin.h>
#include <stdexcept>
#include <string>

#include "partition_avx2.hpp"



void partition_with_mask_hashing_avx2(const uint64_t* in, uint32_t* out, uint32_t P, std::size_t N) {

    // Build the mask vector to be applied to 4 progressive uint64_t keys
    const uint64_t P_mask = static_cast<uint64_t>(P - 1);
    const __m256i mask_vec = _mm256_set1_epi64x(static_cast<long long>(P_mask));

    // Select 32-bit lanes 0,2,4,6 = low 32 bits of each 64-bit lane
    const __m256i perm_idx = _mm256_setr_epi32(0, 2, 4, 6, 0, 0, 0, 0);

    // For each step are processed 4 uint64_t keys (= 1 lane)
    std::size_t i = 0;
    const std::size_t simd_width = 4;
    for (; i + simd_width <= N; i += simd_width) {
        // Build the vector of 4 progressive uint64_t keys (from i to i+3)
        const __m256i key_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(in + i));

        // Apply the AND with the mask
        const __m256i part_vec = _mm256_and_si256(key_vec, mask_vec);

        // Move low 32 bits of each 64-bit lane into the low 128 bits
        const __m256i packed32 = _mm256_permutevar8x32_epi32(part_vec, perm_idx);

        // Store 4 x uint32_t results
        _mm_storeu_si128(reinterpret_cast<__m128i*>(out + i), _mm256_castsi256_si128(packed32));
    }
    
    // Cycle done for the last possible positions remained (< 4)
    for (; i < N; ++i) {
        out[i] = static_cast<uint32_t>(in[i] & P_mask);
    }
}


/**
inline void store_low32_lanes(uint32_t* out, __m256i values, __m256i perm_idx) {
    // Stores the low 32 bits of the 4 64-bit lanes of the values vector into the out array (from 256 bits to 128 bits)
    const __m256i packed = _mm256_permutevar8x32_epi32(values, perm_idx);
    _mm_storeu_si128(reinterpret_cast<__m128i*>(out), _mm256_castsi256_si128(packed));
}

inline __m256i mul_lo_u64x4_by_const(__m256i keys, __m256i mul_lo_vec, __m256i mul_hi_vec) {
    const __m256i cross_prod = _mm256_add_epi64(
        _mm256_mul_epu32(_mm256_srli_epi64(keys, 32), mul_lo_vec),
        _mm256_mul_epu32(keys, mul_hi_vec)
    );
    return _mm256_add_epi64(_mm256_mul_epu32(keys, mul_lo_vec), _mm256_slli_epi64(cross_prod, 32));
}


static void partition_with_mul_hashing(const std::vector<uint64_t>& keys, std::vector<uint32_t>& part_id, uint32_t P) {
    // P validation check
    if (P == 0 || (P & (P - 1)) != 0) {
        throw std::invalid_argument("P must be a power of two");
    }

    constexpr uint64_t kMulHashConstant = 11400714819323198485ull;
    const uint32_t bits = __builtin_ctz(P);
    const std::size_t n_keys = keys.size();
    const uint64_t* in = keys.data();
    uint32_t* out = part_id.data();

    if (bits == 0) {
        const __m256i zero = _mm256_setzero_si256();
        std::size_t j = 0;
        for (; j + 8 <= n_keys; j += 8) {
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(out + j), zero);
        }
        for (; j < n_keys; ++j) {
            out[j] = 0;
        }
        return;
    }

    const uint32_t shift = 64u - bits;
    const __m128i shift_vec = _mm_set1_epi64x(static_cast<long long>(shift));
    const __m256i mul_lo_vec = _mm256_set1_epi64x(static_cast<long long>(static_cast<uint32_t>(kMulHashConstant)));
    const __m256i mul_hi_vec = _mm256_set1_epi64x(static_cast<long long>(kMulHashConstant >> 32));
    const __m256i perm_idx = _mm256_setr_epi32(0, 2, 4, 6, 0, 0, 0, 0); // Select 32-bit lanes 0,2,4,6 = low 32 bits of each 64-bit lane

    std::size_t i = 0;
    for (; i + 16 <= n_keys; i += 16) {
        const __m256i key_vec0 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(in + i));
        const __m256i key_vec1 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(in + i + 4));
        const __m256i key_vec2 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(in + i + 8));
        const __m256i key_vec3 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(in + i + 12));

        const __m256i hash_vec0 = _mm256_srl_epi64(mul_lo_u64x4_by_const(key_vec0, mul_lo_vec, mul_hi_vec), shift_vec);
        const __m256i hash_vec1 = _mm256_srl_epi64(mul_lo_u64x4_by_const(key_vec1, mul_lo_vec, mul_hi_vec), shift_vec);
        const __m256i hash_vec2 = _mm256_srl_epi64(mul_lo_u64x4_by_const(key_vec2, mul_lo_vec, mul_hi_vec), shift_vec);
        const __m256i hash_vec3 = _mm256_srl_epi64(mul_lo_u64x4_by_const(key_vec3, mul_lo_vec, mul_hi_vec), shift_vec);

        store_low32_lanes(out + i, hash_vec0, perm_idx);
        store_low32_lanes(out + i + 4, hash_vec1, perm_idx);
        store_low32_lanes(out + i + 8, hash_vec2, perm_idx);
        store_low32_lanes(out + i + 12, hash_vec3, perm_idx);
    }

    for (; i + 4 <= n_keys; i += 4) {
        const __m256i key_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(in + i));
        const __m256i hash_vec = _mm256_srl_epi64(mul_lo_u64x4_by_const(key_vec, mul_lo_vec, mul_hi_vec), shift_vec);
        store_low32_lanes(out + i, hash_vec, perm_idx);
    }

    for (; i < n_keys; ++i) {
        out[i] = static_cast<uint32_t>((in[i] * kMulHashConstant) >> shift);
    }
}
*/

