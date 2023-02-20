file(GLOB_RECURSE SRCS *.cpp *.c)


add_library(bitpolymul STATIC ${SRCS})

if(MSVC)

    if(BITPOLYMUL_ASAN)
        message("Warning: ASAN is enabled for bitpolymul")
        target_compile_options(bitpolymul PUBLIC "/fsanitize=address")
    endif()

else()
    target_compile_options(bitpolymul PRIVATE -mavx2 -mpclmul -std=c++11)
    
    
    if(BITPOLYMUL_ASAN)
        message("Warning: ASAN is enabled for bitpolymul")
        target_compile_options(bitpolymul PUBLIC "-fsanitize=address")
        target_link_options(bitpolymul PUBLIC "-fsanitize=address")
    endif()
    if(BITPOLYMUL_PIC)
        target_compile_options(bitpolymul PRIVATE -fPIC)
    endif()
endif()

# make projects that include bitpolymul use this as an include folder
target_include_directories(bitpolymul PUBLIC 
                    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
                    $<INSTALL_INTERFACE:include>) 

