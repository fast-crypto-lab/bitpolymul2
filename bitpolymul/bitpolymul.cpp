#include "bitpolymul.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <memory>

#include "bitpolymul/bc.h"
#include "bitpolymul/gfext_aesni.h"
#include "bitpolymul/bpmDefines.h"
#include "bitpolymul/btfy.h"
#include "bitpolymul/encode.h"


namespace bpm
{

    struct Aligned
    {
        std::unique_ptr<u64[]> uPtr;
        u64* ptr;
        u64* data()
        {
            return ptr;
        }

        u64& operator[](u64 i)
        {
            return data()[i];
        }
    };

    Aligned alignedNew(u64 n)
    {
        u64 aligment = sizeof(__m256);
        std::unique_ptr<u64[]> uPtr(new u64[n + aligment]);
        u8* u8ptr = (u8*)uPtr.get();

        auto offset = u64(u8ptr) % aligment;
        if (offset)
            u8ptr += aligment - offset;
        
        return Aligned{ std::move(uPtr), (u64*)u8ptr };
    }

    const int tab64[64] = {
    63,  0, 58,  1, 59, 47, 53,  2,
    60, 39, 48, 27, 54, 33, 42,  3,
    61, 51, 37, 40, 49, 18, 28, 20,
    55, 30, 34, 11, 43, 14, 22,  4,
    62, 57, 46, 52, 38, 26, 32, 41,
    50, 36, 17, 19, 29, 10, 13, 21,
    56, 45, 25, 31, 35, 16,  9, 12,
    44, 24, 15,  8, 23,  7,  6,  5 };


    u64 log2floor(u64 value)
    {
        value |= value >> 1;
        value |= value >> 2;
        value |= value >> 4;
        value |= value >> 8;
        value |= value >> 16;
        value |= value >> 32;
        return tab64[((uint64_t)((value - (value >> 1)) * 0x07EDD5E59A4E28C2)) >> 58];
    }

    u64 log2ceil(u64 value)
    {
        auto floor = log2floor(value);
        return floor + (value > (1ull << floor));
    }

    void bitpolymul_2_128(uint64_t* c, const uint64_t* a, const uint64_t* b, u64 _n_64)
    {
        if (0 == _n_64) return;
        u64 n_64 = 0;
        if (1 == _n_64)
            n_64 = _n_64;
        else {
            n_64 = 1ull << log2ceil(_n_64);
        }

        if (256 > n_64) n_64 = 256;

        auto a_bc = alignedNew(n_64);
        auto b_bc = alignedNew(n_64);

        memcpy(a_bc.data(), a, sizeof(uint64_t) * _n_64);
        for (u64 i = _n_64; i < n_64; i++) a_bc[i] = 0;
        bc_to_lch_2_unit256(a_bc.data(), n_64);

        memcpy(b_bc.data(), b, sizeof(uint64_t) * _n_64);
        for (u64 i = _n_64; i < n_64; i++) b_bc[i] = 0;
        bc_to_lch_2_unit256(b_bc.data(), n_64);


        u64 n_terms = n_64;
        u64 log_n = __builtin_ctzll(n_terms);
        auto a_fx = alignedNew(2 * n_terms);
        auto b_fx = alignedNew(2 * n_terms);

        encode_128_half_input_zero(a_fx.data(), a_bc.data(), n_terms);
        encode_128_half_input_zero(b_fx.data(), b_bc.data(), n_terms);

        btfy_128(b_fx.data(), n_terms, 64 + log_n + 1);
        btfy_128(a_fx.data(), n_terms, 64 + log_n + 1);

        for (u64 i = 0; i < n_terms; i++)
        {
            gf2ext128_mul_sse(
                (uint8_t*)& a_fx[i * 2],
                (uint8_t*)& a_fx[i * 2],
                (uint8_t*)& b_fx[i * 2]);
        }

        i_btfy_128(a_fx.data(), n_terms, 64 + log_n + 1);

        decode_128(b_fx.data(), a_fx.data(), n_terms);

        bc_to_mono_2_unit256(b_fx.data(), 2 * n_64);

        for (u64 i = 0; i < (2 * _n_64); i++) {
            c[i] = b_fx[i];
        }

    }






    ///////////////////////////////////////////////////


    void bitpolymul_2_64(uint64_t* c, const uint64_t* a, const uint64_t* b, u64 _n_64)
    {
        if (0 == _n_64) return;
        if (_n_64 > (1 << 26)) { printf("un-supported length of polynomials."); exit(-1); }
        u64 n_64 = 0;
        if (1 == _n_64) n_64 = _n_64;
        else {
            n_64 = 1ull << log2ceil(_n_64);
        }

        if (256 > n_64) n_64 = 256;



        auto a_bc_ = alignedNew(n_64);
        auto b_bc_ = alignedNew(n_64);
        uint64_t* a_bc = a_bc_.data();
        uint64_t* b_bc = b_bc_.data();

        memcpy(a_bc, a, sizeof(uint64_t) * _n_64);
        for (u64 i = _n_64; i < n_64; i++) a_bc[i] = 0;
        bc_to_lch_2_unit256(a_bc, n_64);

        memcpy(b_bc, b, sizeof(uint64_t) * _n_64);
        for (u64 i = _n_64; i < n_64; i++) b_bc[i] = 0;
        bc_to_lch_2_unit256(b_bc, n_64);


        u64 n_terms = n_64 * 2;
        u64 log_n = __builtin_ctzll(n_terms);

        auto a_fx_ = alignedNew(n_terms);
        auto b_fx_ = alignedNew(n_terms);
        uint64_t* a_fx = a_fx_.data();
        uint64_t* b_fx = b_fx_.data();

        encode_64_half_input_zero(a_fx, a_bc, n_terms);
        encode_64_half_input_zero(b_fx, b_bc, n_terms);

        btfy_64(b_fx, n_terms, 32 + log_n + 1);
        btfy_64(a_fx, n_terms, 32 + log_n + 1);

        for (u64 i = 0; i < n_terms; i += 4) {
            cache_prefetch(&a_fx[i + 4], _MM_HINT_T0);
            cache_prefetch(&b_fx[i + 4], _MM_HINT_T0);
            gf2ext64_mul_4x4_avx2((uint8_t*)& a_fx[i], (uint8_t*)& a_fx[i], (uint8_t*)& b_fx[i]);
        }
        i_btfy_64(a_fx, n_terms, 32 + log_n + 1);
        decode_64(b_fx, a_fx, n_terms);

        bc_to_mono_2_unit256(b_fx, n_terms);

        for (u64 i = 0; i < (2 * _n_64); i++) {
            c[i] = b_fx[i];
        }
    }

}
