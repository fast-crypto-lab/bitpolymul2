project(bitpolymul2)

get_directory_property(hasParent PARENT_DIRECTORY)


# add the source files 
set(SRCS 
    "bc.c" 
    "bitpolymul.c" 
    "encode.c" 
    "butterfly_net.c" 
    "gf2128_cantor_iso.c" 
    "btfy.c" 
    "trunc_btfy_tab.c" 
    "gf264_cantor_iso.c" 
    "trunc_btfy_tab_64.c")

add_library(bitpolymul2 STATIC ${SRCS})

option(DEBUG "Turn on debugging" OFF)
message(STATUS "Option: DEBUG = ${DEBUG}")

option(SSE "Enable SSE" ON)
message(STATUS "Option: SSE = ${SSE}")

#option(K "Enable K" OFF)
#message(STATUS "Option: K = ${K}")

option(AVX "Enable AVX" ON)
message(STATUS "Option: AVX = ${AVX}")

option(AVX2 "Enable AVX2" ON)
message(STATUS "Option: AVX2 = ${AVX2}")

if(DEBUG)
    target_compile_options(bitpolymul2 PUBLIC "-D_DEBUG_")
endif()

if(NOT SSE)
    target_compile_options(bitpolymul2 PUBLIC "-D_NO_SSE_")
endif()

if(AVX)
    target_compile_options(bitpolymul2 PUBLIC "-D_USE_AVX_")
endif()

if(AVX2)
    target_compile_options(bitpolymul2 PUBLIC "-D_USE_AVX2_")
endif()
#ifdef K
#	CFLAGS += -DK=$(K)
#	CXXFLAGS += -DK=$(K)
#endif


#CFLAGS= -O3 -funroll-loops -mavx2 -mpclmul -std=gnu99                            -Wextra -Wall
target_compile_options(bitpolymul2 PUBLIC "-O3" "-funroll-loops" "-mavx2" "-mpclmul" "-std=gnu99" "-Wall")




# make projects that include bitpolymul2 use this as an include folder
target_include_directories(bitpolymul2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(bitpolymul-test "${CMAKE_CURRENT_SOURCE_DIR}/bitpolymul-test.cpp")

target_link_libraries(bitpolymul-test bitpolymul2)
