# Include common directory
include_directories(${CMAKE_SOURCE_DIR}/common)

# Add source files
add_executable(median_filter 
    main.cpp
    median_filter.cpp
)

# Link with common library
target_link_libraries(median_filter common)

