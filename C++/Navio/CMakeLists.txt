cmake_minimum_required(VERSION 3.10)
project(Navio2 VERSION 0.0.1)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release") # default

  set(CMAKE_BUILD_TYPE
      "${CMAKE_BUILD_TYPE}"
      CACHE
        STRING
        "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
                                               "MinSizeRel" "RelWithDebInfo")
endif(NOT CMAKE_BUILD_TYPE)

file(GLOB_RECURSE SOURCES 
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     "Common/*.cpp" "Navio2/*.cpp")
file(GLOB_RECURSE HEADERS
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     "Common/*.h" "Navio2/*.h")     
message(STATUS "Headers: ${HEADERS}")
add_library(navio2 SHARED ${SOURCES} ${HEADERS})     
#set_target_properties(navio2 PROPERTIES PUBLIC_HEADER "${HEADERS}")
target_include_directories(navio2 PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/>
    $<INSTALL_INTERFACE:include/Navio>)

export(PACKAGE Navio2)

include(CMakePackageConfigHelpers)

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR
    lib
    CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR
    bin
    CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR
    include/Navio
    CACHE PATH "Installation directory for header files")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR share/Navio2/cmake)
endif()
set(INSTALL_CMAKE_DIR
    ${DEF_INSTALL_CMAKE_DIR}
    CACHE PATH "Installation directory for CMake files")

# Create the Navio2Config.cmake and Navio2ConfigVersion files
if(NOT IS_ABSOLUTE "${INSTALL_INCLUDE_DIR}")
  set(ABS_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/${INSTALL_INCLUDE_DIR}")
endif()
if(NOT IS_ABSOLUTE "${INSTALL_CMAKE_DIR}")
  set(ABS_INSTALL_CMAKE_DIR "${CMAKE_INSTALL_PREFIX}/${INSTALL_CMAKE_DIR}")
endif()
file(RELATIVE_PATH REL_INCLUDE_DIR "${ABS_INSTALL_CMAKE_DIR}"
     "${ABS_INSTALL_INCLUDE_DIR}")
# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
configure_file(cmake/Navio2Config.cmake.in
               "${PROJECT_BINARY_DIR}/Navio2Config.cmake" @ONLY)
# ... for the install tree
set(CONF_INCLUDE_DIRS "\${Navio2_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(
  cmake/Navio2Config.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/Navio2Config.cmake"
  @ONLY)
# ... version file for both
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/Navio2ConfigVersion.cmake
                                 VERSION ${PROJECT_VERSION}
                                 COMPATIBILITY SameMajorVersion )

install(
    TARGETS navio2
    EXPORT Navio2Targets
    LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
    PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/"
                COMPONENT dev)            
                
# Add all targets to the build-tree export set
export(TARGETS navio2
       FILE "${PROJECT_BINARY_DIR}/Navio2Targets.cmake")
                       
install(
    FILES
        "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/Navio2Config.cmake"
        "${PROJECT_BINARY_DIR}/Navio2ConfigVersion.cmake"
    DESTINATION "${INSTALL_CMAKE_DIR}"
    COMPONENT dev)
    install(
        DIRECTORY Common Navio2
        DESTINATION ${INSTALL_INCLUDE_DIR}
        FILES_MATCHING
        PATTERN "*.h")    

# Install the export set for use with the install-tree
install(
  EXPORT Navio2Targets
  DESTINATION "${INSTALL_CMAKE_DIR}"
  COMPONENT dev)        