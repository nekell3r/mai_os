cmake_minimum_required(VERSION 3.10)
project(lab4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Common library
add_subdirectory(common)

# DLL: Bubble Sort
add_library(bubble_sort SHARED
    src/bubble_sort.cpp
    src/sort_interface.h
)
set_target_properties(bubble_sort PROPERTIES
    PREFIX ""
    OUTPUT_NAME "bubble_sort"
)
target_include_directories(bubble_sort PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/common
)
target_compile_definitions(bubble_sort PRIVATE BUILD_DLL)
target_link_libraries(bubble_sort common)

# DLL: QuickSort
add_library(quicksort SHARED
    src/quicksort.cpp
    src/sort_interface.h
)
set_target_properties(quicksort PROPERTIES
    PREFIX ""
    OUTPUT_NAME "quicksort"
)
target_include_directories(quicksort PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/common
)
target_compile_definitions(quicksort PRIVATE BUILD_DLL)
target_link_libraries(quicksort common)

# Program 1: Static linking (uses bubble_sort)
add_executable(program1
    src/program1.cpp
    src/bubble_sort.cpp
)
target_include_directories(program1 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/common
)
target_compile_definitions(program1 PRIVATE USE_STATIC_LINK)
target_link_libraries(program1 common)

# Program 2: Dynamic loading
add_executable(program2
    src/program2.cpp
)
target_include_directories(program2 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/common
)
target_link_libraries(program2 common)

# Copy DLLs to output directory for program2
add_custom_command(TARGET program2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:bubble_sort>
    $<TARGET_FILE_DIR:program2>/
    COMMENT "Copying bubble_sort.dll to program2 directory"
)

add_custom_command(TARGET program2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:quicksort>
    $<TARGET_FILE_DIR:program2>/
    COMMENT "Copying quicksort.dll to program2 directory"
)

