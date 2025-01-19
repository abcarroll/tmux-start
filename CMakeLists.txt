cmake_minimum_required(VERSION 3.10)
project(tmux-start VERSION 1.0.0 LANGUAGES C)

# Enable strict compilation flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra") # -Wpedantic -Werror

# Set the source directory
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

# Add the executable
add_executable(tmux-start ${SRC_DIR}/tmux-start.c)

# Installation directories
install(TARGETS tmux-start DESTINATION /usr/local/bin)
install(FILES LICENSE DESTINATION /usr/share/licenses/tmux-start)
install(FILES README.md DESTINATION /usr/share/doc/tmux-start)

# Enable clang-tidy if available
find_program(CLANG_TIDY_EXECUTABLE NAMES clang-tidy)
if(CLANG_TIDY_EXECUTABLE)
    message(STATUS "clang-tidy found: ${CLANG_TIDY_EXECUTABLE}")
    set(CMAKE_C_CLANG_TIDY "${CLANG_TIDY_EXECUTABLE}")
else()
    message(WARNING "clang-tidy not found, skipping tidy checks")
endif()

# Custom tidy target
add_custom_target(tidy
    COMMAND ${CLANG_TIDY_EXECUTABLE} ${SRC_DIR}/*.c
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running clang-tidy on source files"
    VERBATIM
)
