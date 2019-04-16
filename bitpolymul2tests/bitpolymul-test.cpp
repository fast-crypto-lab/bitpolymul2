
#include <cryptoTools/Crypto/PRNG.h>

#include "benchmark.h"
#include "bitpolymul2/byte_inline_func.h"

#include "bitpolymul2/bitpolymul.h"

#include "bitpolymul2/config_profile.h"

#include "bitpolymul2/defines.h"
#include <cryptoTools/Common/CLP.h>
#include <cryptoTools/Common/Timer.h>

//#define TEST_RUN 20

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

    uint64_t TEST_RUN = cmd.getOr("t", 10);
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
        poly3(len),
        rPoly1(len * 2),
        rPoly2(len * 2);


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


    bm_func1(rPoly1.data(), poly2.data(), poly1.data(), len);
    bm_func1(rPoly2.data(), poly1.data(), poly2.data(), len);

    if (32 >= len && cmd.isSet("v")) {
        printf("poly1 :"); u64_dump(poly1); puts("");
        printf("poly2 :"); u64_dump(poly2); puts("");

        printf("rPoly1 :"); u64_dump(rPoly1); puts("");

        if (rPoly1 != rPoly2) {
            printf("consistency fail: \n");
            printf("rPoly2 :"); u64_fdump(stdout, rPoly2); puts("");
        }
    }


    bpm::FFTPoly fft1, fft2, fft3, fft_12, fft_13;
    auto vecAdd = [](std::vector<oc::u64> a, std::vector<oc::u64>& b)
    {
        for (oc::u64 i = 0; i < a.size(); ++i)
            a[i] ^= b[i];
        return a;
    };

    if (cmd.isSet("timer"))
    {
        oc::Timer timer;
        timer.setTimePoint("start");
        for (uint64_t i = 0; i < TEST_RUN; i++)
        {
            bm_func2(rPoly1.data(), poly2.data(), poly1.data(), len);
        }
        timer.setTimePoint("end");
        std::cout <<"timer " << timer << std::endl;
    }

    uint64_t fail_count = 0;
    uint64_t chk = 0;
    for (uint64_t i = 0; i < TEST_RUN; i++)
    {
        //prng.get(poly1.data(), poly1.size());
        //prng.get(poly2.data(), poly2.size());
        for (uint64_t q = 0; q < len; q++) { poly1[q] = q + 1 + i; }   // i * i + 321434123377;
        for (uint64_t q = 0; q < len; q++) { poly2[q] = q + 2 + i; }   // i * i + 463254234534;
        for (uint64_t q = 0; q < len; q++) { poly3[q] = q + 3 + i; }   // i * i + 463254234534;



        auto back1 = poly1;
        auto back2 = poly2;

        BENCHMARK(bm1, {
                bm_func1(rPoly1.data() , poly2.data() , poly1.data() , len);
            });


        BENCHMARK(bm2, {
                bm_func2(rPoly2.data() , poly2.data() , poly1.data() , len);
            });


        //fft1.encode(poly1);
        //fft2.encode(poly2);
        //fft1.addEq(fft2);
        //fft1.decode(poly5);

        //auto x = vecAdd(poly1, poly2);
        //x.resize(poly5.size());
        //if (poly5 != x)
        //{
        //    std::cout << "add plain failed." << std::endl;
        //    printf("res1:"); u64_fdump(stdout, poly5); puts("");
        //    printf("resX:"); u64_fdump(stdout, x); puts("");
        //}

        if (rPoly1 != rPoly2) {
            fail_count++;
            if (cmd.isSet("v"))
            {

                printf("consistency fail: %d.\n", i);
                printf("res1:"); u64_fdump(stdout, rPoly1); puts("");
                printf("res2:"); u64_fdump(stdout, rPoly2); puts("");
                //printf("diff:"); u64_fdump(stdout, poly5); puts("");

                bitpolymul_2_64(rPoly1.data(), poly2.data(), poly1.data(), len);

                printf("res3:"); u64_fdump(stdout, rPoly1); puts("");

                if (back1 != poly1) printf("back 1 failed");
                if (back2 != poly2) printf("back 2 failed");
                printf("\n");
            }
        }

        //bm_func2(poly5.data(), poly1.data(), poly3.data(), len);
        ////byte_xor(poly5.data(), poly4.data(), len * 2);
        //poly5 = vecAdd(poly5, poly4);



        fft1.encode(poly1);
        fft2.encode(poly2);
        fft3.encode(poly3);

        std::vector<oc::u64> 
            poly_12(len * 2),
            poly_13(len * 2),
            fft_r(len*2);
        //auto fft_12_13 = fft3;

        fft_12.mult(fft1, fft2);
        fft_13.mult(fft1, fft3);

        fft_12.decode(poly_12, false);
        fft_13.decode(poly_13, false);

        bpm::FFTPoly fft_12_13;
        fft_12_13.add(fft_13, fft_12);

        auto poly_r = vecAdd(poly_12, poly_13);
        fft_12_13.decode(fft_r);

        if (poly_r != fft_r)
        {

            std::cout << "add plain failed." << std::endl;
            printf("poly_r:"); u64_fdump(stdout, poly_r); puts("");
            printf("fft_r: "); u64_fdump(stdout, fft_r); puts("");
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
