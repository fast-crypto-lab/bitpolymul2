cmake_minimum_required(VERSION 3.15)
project(bitpolymul VERSION 1.0.1)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE Release)
endif()
	
include("cmake/buildOptions.cmake")

add_subdirectory(bitpolymul)

include("cmake/install.cmake")