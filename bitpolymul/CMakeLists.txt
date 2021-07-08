file(GLOB_RECURSE SRCS *.cpp *.c)


add_library(bitpolymul STATIC ${SRCS})

if(MSVC)
else()
    target_compile_options(bitpolymul PRIVATE -mavx2 -mpclmul)
endif()

# make projects that include bitpolymul use this as an include folder
target_include_directories(bitpolymul PUBLIC 
                    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
                    $<INSTALL_INTERFACE:include>) 

