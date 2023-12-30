cmake_minimum_required(VERSION 2.8.12)
project(multi_threading_sat_solver)

find_package(Threads)
if (EXISTS "${CMAKE_SOURCE_DIR}/minisat")
  add_subdirectory(minisat)
else()
  message (FATAL_ERROR "MiniSat not found.")
endif()


## ECE650 Specific configuration start here
## Project specific configurations go here

# enable c++11 support
set (CMAKE_CXX_FLAGS "-std=c++11 -Wall ${CMAKE_CXX_FLAGS}")
set (CMAKE_BUILD_TYPE Debug)


# expose minisat header files
include_directories(${CMAKE_SOURCE_DIR}/minisat)

option(WITH_ASAN "Enabled AddressSanitizer to detect memory faults" OFF)
if(WITH_ASAN)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined" )
endif()

# create the main executable
## add additional .cpp files if needed
add_executable(main main.cpp helpers.cpp BFStree.cpp MyGraph.cpp)
# link MiniSAT libraries
target_link_libraries(main minisat-lib-static)
target_link_libraries(main ${CMAKE_THREAD_LIBS_INIT})

# create the executable for tests
add_executable(test test.cpp helpers.cpp BFStree.cpp MyGraph.cpp)
# link MiniSAT libraries
target_link_libraries(test minisat-lib-static)
