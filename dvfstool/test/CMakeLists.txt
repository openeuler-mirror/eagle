set(TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/)

set(SOURCE_FILE
    ${TOP_DIR}/../freq_pattern.c
    ${TOP_DIR}/../freq_set.c
    ${TOP_DIR}/../multi_thread.c
)

option(DVFS_DEBUG "option fro debug" OFF)
if (DVFS_DEBUG)
        add_definitions(-DDVFS_DEBUG)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pthread -fprofile-arcs -ftest-coverage")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -pthread -fprofile-arcs -ftest-coverage")
set(CMAKE_LDFLAGS "${CMAKE_LDFLAGS}")

set(TEST_FILE
    ${TOP_DIR}/testcase/FreqSetTest.cpp
    ${TOP_DIR}/testcase/MultiThreadTest.cpp
    ${TOP_DIR}/testcase/FreqPatternTest.cpp
)

add_executable(test
    ${SOURCE_FILE}
    ${TEST_FILE}
)

target_include_directories(test PRIVATE
    ${TOP_DIR}/../include
)

target_link_libraries(test PRIVATE
    gtest
    gtest_main
    pthread
)
