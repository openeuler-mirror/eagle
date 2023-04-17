cmake_minimum_required (VERSION 3.12)
project(dvfs C)
set(TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR})

option(DVFS_DEBUG "option fro debug" OFF)
if (DVFS_DEBUG)
	add_definitions(-DDVFS_DEBUG)
endif()

set(CMAKE_C_FLAGS "-std=gnu11 -Wall -s -O2 -fstack-protector-strong -fpic \
    -fpie -Wl,-z,relro -Wl,-z,noexecstack -Werror=format")

set(SOURCE_FILE
    ${TOP_DIR}/dvfs.c
    ${TOP_DIR}/freq_pattern.c
    ${TOP_DIR}/freq_set.c
    ${TOP_DIR}/multi_thread.c
)

add_executable(dvfs
    ${SOURCE_FILE}
)

target_link_libraries(dvfs PRIVATE
    pthread
)
