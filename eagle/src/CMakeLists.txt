cmake_minimum_required (VERSION 3.16)
project (eagle C)
set ( CMAKE_INCLUDE_CURRENT_DIR ON)
# Add head file directory
include_directories (
    "${PROJECT_SOURCE_DIR}/../inc"
    "${PROJECT_SOURCE_DIR}/../inc/shared"
    "${PROJECT_SOURCE_DIR}/pwrapi_adaptor"
    "${PROJECT_SOURCE_DIR}/pwrapi_adaptor/pwrapi"
    #"${PROJECT_SOURCE_DIR}/sched_service"
    )

# Add library directory
link_directories (
    "${CMAKE_CURRENT_BINARY_DIR}/pwrapi_adaptor"
) 

# Load source file
aux_source_directory(${PROJECT_SOURCE_DIR} EAGLE_SRC_DIR)
#aux_source_directory(${PROJECT_SOURCE_DIR}/sched_service EAGLE_SRC_DIR)

# Set compiling policy
set (PG_NAME ${PROJECT_NAME})
add_executable (${PG_NAME} ${EAGLE_SRC_DIR})
target_link_libraries(${PG_NAME} -lpwrapi_adaptor -lpwrapi -ldl -lpthread -lnuma -lm)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set installaltion path
#set ( CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}")
set ( CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "Install path prefix" FORCE)

set(CMAKE_SKIP_RPATH TRUE)
install (TARGETS ${PG_NAME} DESTINATION sbin)

# Install default config files
install (FILES "${PROJECT_SOURCE_DIR}/../conf/eagle_config.ini" DESTINATION /etc/eagle)
install (FILES "${PROJECT_SOURCE_DIR}/../conf/eagle_policy.ini" DESTINATION /etc/eagle)
install (FILES "${PROJECT_SOURCE_DIR}/../eagle.service" DESTINATION /usr/lib/systemd/system)

# release compile mode
#set(CMAKE_BUILD_TYPE "Release")
#set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

# debug compile mode
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wshadow -Wstrict-prototypes -Wmissing-prototypes -Werror")