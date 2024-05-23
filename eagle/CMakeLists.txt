cmake_minimum_required (VERSION 3.16)
project (eagle_entrance C)
set(CMAKE_VERBOSE_MAKEFILE on)

add_subdirectory(src/pwrapi_adaptor)
add_subdirectory(src/sched_service)
add_subdirectory(src/freq_service)
add_subdirectory(src/idle_service)
add_subdirectory(src/mpc_service)
add_subdirectory(src)

