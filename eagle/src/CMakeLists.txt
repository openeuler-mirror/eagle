cmake_minimum_required (VERSION 3.19)
project (eagle C)
set ( CMAKE_INCLUDE_CURRENT_DIR ON)
# Add head file directory
include_directories ("${PROJECT_SOURCE_DIR}/../inc")

# Load source file
aux_source_directory(${PROJECT_SOURCE_DIR} EAGLE_SRC_DIR)

# Set compiling policy
set (PG_NAME ${PROJECT_NAME})
add_executable (${PG_NAME} ${EAGLE_SRC_DIR})
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set installaltion path
#set ( CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}")
set ( CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "Install path prefix" FORCE)
#install (TARGETS ${PG_NAME} DESTINATION bin)
install (TARGETS ${PG_NAME} DESTINATION sbin)

# Install default config files
install (FILES "${PROJECT_SOURCE_DIR}/../conf/eagle_config.ini" DESTINATION /etc/sysconfig)
install (FILES "${PROJECT_SOURCE_DIR}/../eagle.service" DESTINATION /usr/lib/systemd/system)

# release complile mode
#set(CMAKE_BUILD_TYPE "Release")
#set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

# debug complile mode
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
