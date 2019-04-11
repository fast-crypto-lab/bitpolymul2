/*
Copyright (C) 2017 Ming-Shing Chen

This file is part of BitPolyMul.

BitPolyMul is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

BitPolyMul is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with BitPolyMul.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _BITPOLYMUL_H_
#define _BITPOLYMUL_H_


#include <stdint.h>
#include <cryptoTools/Common/Defines.h>
#include <vector>


void bitpolymul_simple(uint64_t * c, const uint64_t * a, const uint64_t * b, unsigned n_64);


void bitpolymul_2_128(uint64_t * c, const uint64_t * a, const uint64_t * b, unsigned n_64);

void bitpolymul_2_64(uint64_t * c, const uint64_t * a, const uint64_t * b, unsigned n_64);



void bitpolymul(uint64_t * c, const uint64_t * a, const uint64_t * b, uint64_t n_64);

namespace bpm
{
    template<typename T>
    using span = oc::span<T>;

    using u64 = oc::u64;
    using u32 = oc::u32;
    using u8 = oc::u8;

    class FFTPoly
    {
    public:

        FFTPoly() = default;
        FFTPoly(const FFTPoly&) = default;
        FFTPoly(FFTPoly&&) = default;

        FFTPoly(span<const u64> data)
        {
            encode(data);
        }

        u64 mN = 0, mNPow2 = 0;
        std::vector<u64> mPoly;
        std::vector<u64> temp;

        void resize(u64 n);

        void encode(span<const u64> data);


        void mult(const FFTPoly& a, const FFTPoly& b);
        void multEq(const FFTPoly& b);

        void add(const FFTPoly& a, const FFTPoly& b);
        void addEq(const FFTPoly& b);


        void decode(span<u64> dest);


    };
}

#endif
