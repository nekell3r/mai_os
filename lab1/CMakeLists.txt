cmake_minimum_required(VERSION 3.10)

project(lab1_variant10)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add subdirectories
add_subdirectory(common)
add_subdirectory(src)

# Output build info
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  message("Building lab1 for Windows")
endif()

