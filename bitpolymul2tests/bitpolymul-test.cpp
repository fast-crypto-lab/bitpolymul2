
#include <cryptoTools/Crypto/PRNG.h>

#include "benchmark.h"
#include "bitpolymul2/byte_inline_func.h"

#include "bitpolymul2/bitpolymul.h"

#include "bitpolymul2/config_profile.h"

#include "bitpolymul2/defines.h"
#include <cryptoTools/Common/CLP.h>

#define TEST_RUN 20

//#define _HAVE_GF2X_

#ifdef _HAVE_GF2X_
#include "gf2x.h"
void polymul_gf2x(uint64_t * c, const uint64_t * a, const uint64_t * b, unsigned terms)
{
    gf2x_mul(c, a, terms, b, terms);
}
#endif


#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)


#define bm_func1 bitpolymul
#define n_fn1 "fn:" TOSTRING(bm_func1) "()"

#define bm_func2 bitpolymul_2_128
#define n_fn2 "fn:" TOSTRING(bm_func2) "()"





void mul(uint64_t * c, const uint64_t * a, const uint64_t * b, unsigned _n_64)
{

}


int main(int argc, char ** argv)
{
    oc::CLP cmd(argc, argv);

    uint64_t len = (1ull << cmd.getOr("n", 16));
    //if (2 == argc) {
    //    int log = atoi(argv[1]);
    //    if (log > 30 || 0 == log) {
    //        printf("Benchmark binary polynomial multiplications.\nUsage: exe [log2_len]\n\n");
    //        exit(-1);
    //    }
    //    len = 1 << log;
    //}

    printf("Multiplication test:\ninput poly len: 2^%d x 64 bit. Benchmark in micro seconds.\n", oc::log2ceil(len));

    std::vector<uint64_t>
        poly1(len),
        poly2(len),
        poly3(len * 2),
        poly4(len * 2),
        poly5(len * 2);


    benchmark bm1;
    bm_init(&bm1);
    benchmark bm2;
    bm_init(&bm2);

    bool random = true;
    oc::PRNG prng(oc::ZeroBlock);

    if (random)
    {
        prng.get(poly1.data(), poly1.size());
        prng.get(poly2.data(), poly2.size());
    }
    else
    {
        poly1[0] = 2;
        poly2[0] = 4;
    }


    bm_func1(poly5.data(), poly2.data(), poly1.data(), len);
    bm_func1(poly3.data(), poly1.data(), poly2.data(), len);

    if (32 >= len) {
        printf("poly1 :"); u64_dump(poly1.data(), len); puts("");
        printf("poly2 :"); u64_dump(poly2.data(), len); puts("");

        printf("poly3 :"); u64_dump(poly3.data(), len * 2); puts("");

        if (poly3 != poly5) {
            printf("consistency fail: \n");
            printf("poly5 :"); u64_fdump(stdout, poly5.data(), len * 2); puts("");
        }
    }


    bpm::FFTPoly fft1, fft2, fft3, fft4, fft5;


    uint64_t fail_count = 0;
    uint64_t chk = 0;
    for (uint64_t i = 0; i < TEST_RUN; i++)
    {
        prng.get(poly1.data(), poly1.size());
        prng.get(poly2.data(), poly2.size());

        auto back1 = poly1;
        auto back2 = poly2;

        BENCHMARK(bm1, {
                bm_func1(poly3.data() , poly2.data() , poly1.data() , len);
            });


        BENCHMARK(bm2, {
                bm_func2(poly4.data() , poly2.data() , poly1.data() , len);
            });



        if (poly3 != poly4) {
            fail_count++;
            if (cmd.isSet("v"))
            {

                printf("consistency fail: %d.\n", i);
                printf("res1:"); u64_fdump(stdout, poly3.data(), len * 2); puts("");
                printf("res2:"); u64_fdump(stdout, poly4.data(), len * 2); puts("");
                printf("diff:"); u64_fdump(stdout, poly5.data(), len * 2); puts("");

                bitpolymul_2_64(poly5.data(), poly2.data(), poly1.data(), len);

                printf("res3:"); u64_fdump(stdout, poly3.data(), len * 2); puts("");

                if (back1 != poly1) printf("back 1 failed");
                if (back2 != poly2) printf("back 2 failed");
                printf("\n");
            }
        }

        bm_func2(poly5.data(), poly1.data(), poly3.data(), len);
        byte_xor(poly5.data(), poly4.data(), len * 2);

        fft1.encode(poly1);
        fft2.encode(poly2);
        fft3.encode(poly2);

        fft4.mult(fft1, fft2);
        fft5.mult(fft1, fft3);

        fft5.addEq(fft4);

        fft5.decode(poly4);

        if (poly4 != poly5)
        {
            chk++;
        }

    }


    printf("fail count: %d.\n", fail_count);
    printf("add failed: %d\n", chk);
    char msg[256];
    bm_dump(msg, 256, &bm1);
    printf("benchmark (%s) :\n%s\n", n_fn1, msg);

    bm_dump(msg, 256, &bm2);
    printf("benchmark (%s) :\n%s\n\n", n_fn2, msg);



    return 0;
}
