
#############################################
#            Install                        #
#############################################


# make cache variables for install destinations
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

configure_file("${CMAKE_CURRENT_LIST_DIR}/../cmake/configVersion.cmake.in" "bitpolymulConfigVersion.cmake" COPYONLY)
configure_file("${CMAKE_CURRENT_LIST_DIR}/../cmake/buildOptions.cmake.in"  "bitpolymulBuildOptions.cmake")

# generate the config file that is includes the exports
configure_package_config_file(
  "${CMAKE_CURRENT_LIST_DIR}/Config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/bitpolymulConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bitpolymul
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

if(NOT DEFINED bitpolymul_VERSION_MAJOR)
    message("\n\n\n\n warning, bitpolymul_VERSION_MAJOR not defined ${bitpolymul_VERSION_MAJOR}")
endif()

set_property(TARGET bitpolymul PROPERTY VERSION ${bitpolymul_VERSION})

# install the configuration file
install(FILES
          "${CMAKE_CURRENT_BINARY_DIR}/bitpolymulBuildOptions.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/bitpolymulConfig.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/bitpolymulConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bitpolymul
)

set(exportLibs 
    "bitpolymul;")
    
# install library
install(
    TARGETS ${exportLibs}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    EXPORT bitpolymulTargets)

# install headers
install(
    DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../bitpolymul/ 
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bitpolymul 
    FILES_MATCHING PATTERN "*.h")
    

# install config
install(EXPORT bitpolymulTargets
  FILE bitpolymulTargets.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bitpolymul
)
 export(EXPORT bitpolymulTargets
       FILE "${CMAKE_CURRENT_BINARY_DIR}/bitpolymulTargets.cmake"
)