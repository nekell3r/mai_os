cmake_minimum_required(VERSION 3.10)

# Put directory name to COMPONENT_NAME variable
get_filename_component(COMPONENT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# Set component name
project(${COMPONENT_NAME})


# Set warnings flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
endif()

# Set optimization + debug info flags
if(CMAKE_BUILD_TYPE MATCHES Debug)
    if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Od /Zi")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g3")
    endif()
endif()
if(CMAKE_BUILD_TYPE MATCHES Release)
    if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -g0")
    endif()
endif()


# Add source files
set(SOURCE_LIB 
    errors.cpp
)

# Make lib
add_library(${COMPONENT_NAME} STATIC ${SOURCE_LIB})


# Output out system
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  message("Build for Linux")
endif ()


if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  message("Build for Windows")
endif ()

