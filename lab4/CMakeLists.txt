cmake_minimum_required(VERSION 3.10)
project(lab4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Common library
add_subdirectory(common)

# DLL: rectangles.dll (Rectangle method + Bubble Sort)
add_library(rectangles SHARED
    src/rectangles.cpp
    src/integral_interface.h
    src/sort_interface.h
)
set_target_properties(rectangles PROPERTIES
    PREFIX ""
    OUTPUT_NAME "rectangles"
)
target_include_directories(rectangles PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/common
)
target_compile_definitions(rectangles PRIVATE BUILD_DLL)
target_link_libraries(rectangles common)

# DLL: trapezoids.dll (Trapezoidal method + QuickSort)
add_library(trapezoids SHARED
    src/trapezoids.cpp
    src/integral_interface.h
    src/sort_interface.h
)
set_target_properties(trapezoids PROPERTIES
    PREFIX ""
    OUTPUT_NAME "trapezoids"
)
target_include_directories(trapezoids PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/common
)
target_compile_definitions(trapezoids PRIVATE BUILD_DLL)
target_link_libraries(trapezoids common)

# Program 1: Static linking (uses rectangles implementation)
add_executable(program1
    src/program1.cpp
    src/rectangles.cpp
)
target_include_directories(program1 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/common
)
target_compile_definitions(program1 PRIVATE USE_STATIC_LINK)
target_link_libraries(program1 common)

# Program 2: Dynamic loading (uses both DLLs)
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
    $<TARGET_FILE:rectangles>
    $<TARGET_FILE_DIR:program2>/
    COMMENT "Copying rectangles.dll to program2 directory"
)

add_custom_command(TARGET program2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:trapezoids>
    $<TARGET_FILE_DIR:program2>/
    COMMENT "Copying trapezoids.dll to program2 directory"
)
