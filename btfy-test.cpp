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

#include <stdio.h>

#include "benchmark.h"
#include "byte_inline_func.h"
#include "immintrin.h"

#include "btfy.h"

#define TEST_RUN 100


#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define bm_func1 btfy_64
#define n_fn1 "fn:" TOSTRING(bm_func1) "()"

#define bm_func2 i_btfy_64
#define n_fn2 "fn:" TOSTRING(bm_func2) "()"


typedef uint64_t sto_t;
//typedef __m128i sto_t;
//typedef __m256i sto_t;


//#define _EXIT_WHILE_FAIL_

#define LEN 256




int main()
{
	//unsigned char seed[32] = {0};

	sto_t poly1[LEN] __attribute__((aligned(32)));
	sto_t poly2[LEN] __attribute__((aligned(32)));

	benchmark bm1;
	bm_init(&bm1);
	benchmark bm2;
	bm_init(&bm2);


	u64_rand( (uint64_t*)poly1 , (sizeof(sto_t)/sizeof(uint64_t))*LEN/2 );
	for(unsigned i=LEN/2;i<LEN;i++) poly1[i] ^= poly1[i];

	memcpy( poly2 , poly1 , LEN*sizeof(sto_t) );
	if(1024>LEN) u64_dump( (uint64_t*)poly2 , (sizeof(sto_t)/sizeof(uint64_t))*LEN ); puts("");
	bm_func1( (uint64_t*) poly2 , LEN , 0 );
	if(1024>LEN) u64_dump( (uint64_t*)poly2 , (sizeof(sto_t)/sizeof(uint64_t))*LEN ); puts("");


	for(unsigned i=0;i<TEST_RUN;i++) {
		u64_rand( (uint64_t*)poly1 , (sizeof(sto_t)/sizeof(uint64_t))*LEN/2 );
		memcpy( poly2 , poly1 , LEN*sizeof(sto_t) );
BENCHMARK( bm1 , {
		bm_func1( (uint64_t*)poly2 , LEN , 128 );
} );
BENCHMARK( bm2 , {
		bm_func2( (uint64_t*)poly2 , LEN , 128 );
} );

		byte_xor( (uint64_t*)poly2 , (uint64_t*)poly1 , (sizeof(sto_t)/sizeof(uint64_t))*LEN );
		if( ! byte_is_zero( (uint64_t*)poly2 , (sizeof(sto_t)/sizeof(uint64_t))*LEN ) ) {
			printf("consistency fail: %d.\n", i);
			printf("diff:"); u64_fdump(stdout,(uint64_t*)poly2,LEN);
			printf("\n");
#define _EXIT_WHILE_FAIL_
#ifdef _EXIT_WHILE_FAIL_
			exit(-1);
#endif
		}
	}

	printf("check: %x\n", *((unsigned *)poly2) );
	char msg[256];
	bm_dump( msg , 256 , &bm1 );
	printf("benchmark (%s) :\n%s\n\n", n_fn1 , msg );

	bm_dump( msg , 256 , &bm2 );
	printf("benchmark (%s) :\n%s\n\n", n_fn2 , msg );

	return 0;
}
