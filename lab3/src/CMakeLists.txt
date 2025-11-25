# Compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Parent process executable
add_executable(parent parent.cpp)
target_link_libraries(parent common)

# Child process executable
add_executable(child child.cpp)
target_link_libraries(child common)

# Ensure executables are placed in the same directory
set_target_properties(parent child PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/src"
)

