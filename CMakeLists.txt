# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(BStarTreeProject)

# Set the C++ standard to C++11 (needed for stoll, lambdas, etc.)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable target and list all .cpp source files
add_executable(bstree_main
    main.cpp
    customer.cpp
    treenode.cpp
    bstartree.cpp
)

# Tell the executable where to find header files (in the current directory)
target_include_directories(bstree_main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})