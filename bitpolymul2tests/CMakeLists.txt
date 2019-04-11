

include("${CMAKE_CURRENT_SOURCE_DIR}/../msvc_runtime.cmake")
configure_msvc_runtime()

add_executable(bitpolymul-test "${CMAKE_CURRENT_SOURCE_DIR}/bitpolymul-test.cpp")

target_link_libraries(bitpolymul-test bitpolymul2)
