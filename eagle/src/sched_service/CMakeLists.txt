cmake_minimum_required (VERSION 3.16)
project (sched_service C)
set ( CMAKE_INCLUDE_CURRENT_DIR ON)

# Add head directory
include_directories ("${PROJECT_SOURCE_DIR}")
include_directories ("${PROJECT_SOURCE_DIR}/../../inc/shared")
include_directories ("${PROJECT_SOURCE_DIR}/../pwrapi_adaptor")

# Load source file
set(SCHED_SRC ${PROJECT_SOURCE_DIR}/sched_service.c)

# Set compile policy
set (PG_NAME ${PROJECT_NAME})
add_library(${PG_NAME} SHARED ${SCHED_SRC})
set_target_properties(${PG_NAME} PROPERTIES LINKER_LANGUAGE C)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(BUILD_LLT)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
endif(BUILD_LLT)

# set installation path
set ( CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "Install path prefix" FORCE)
install (TARGETS ${PG_NAME} DESTINATION lib64)

# Release compile mode
#set(CMAKE_BUILD_TYPE "Release")
#set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

# Debug compile mode
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")

