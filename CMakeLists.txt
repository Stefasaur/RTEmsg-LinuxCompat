cmake_minimum_required(VERSION 3.10)
project(RTEmsg 
    VERSION 1.0.0
    DESCRIPTION "RTE Message Decoder"
    LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Compiler settings
if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wno-unused-parameter -Wno-return-type -std=gnu11)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
        add_compile_definitions(_DEBUG)
    else()
        add_compile_options(-O3)
        add_compile_definitions(NDEBUG)
    endif()
    # Add UTF-8 support
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/W4 /utf-8)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(/Od)
        add_compile_definitions(_DEBUG)
    else()
        add_compile_options(/O2)
        add_compile_definitions(NDEBUG)
    endif()
endif()

# Platform-specific definitions
if(WIN32)
    add_compile_definitions(WIN32 _CONSOLE _CRT_SECURE_NO_WARNINGS)
else()
    # Linux/Unix specific settings
    add_compile_definitions(_GNU_SOURCE)
endif()

# Source files
set(SOURCES
    Code/main.c
    Code/cmd_line.c
    Code/decoder.c
    Code/errors.c
    Code/files.c
    Code/format.c
    Code/messages.c
    Code/parse_directive.c
    Code/parse_directive_helpers.c
    Code/parse_directive_msg.c
    Code/parse_error_reporting.c
    Code/parse_file_handling.c
    Code/parse_fmt_string.c
    Code/pch.c
    Code/print_helper.c
    Code/print_message.c
    Code/process_bin_data.c
    Code/read_bin_data.c
    Code/statistics.c
    Code/utf8_helpers.c
)

# Add platform compatibility source for non-Windows systems
if(NOT WIN32)
    list(APPEND SOURCES Code/platform_compat.c)
endif()

# Header files
set(HEADERS
    Code/cmd_line.h
    Code/decoder.h
    Code/errors.h
    Code/files.h
    Code/format.h
    Code/main.h
    Code/messages.h
    Code/parse_directive.h
    Code/parse_directive_helpers.h
    Code/parse_directive_msg.h
    Code/parse_error_reporting.h
    Code/parse_file_handling.h
    Code/parse_fmt_string.h
    Code/pch.h
    Code/platform_compat.h
    Code/print_helper.h
    Code/print_message.h
    Code/process_bin_data.h
    Code/read_bin_data.h
    Code/rtedbg.h
    Code/rtemsg_config.h
    Code/statistics.h
    Code/text.h
    Code/timestamp.h
    Code/utf8_helpers.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE Code)

# Link libraries
if(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE m)  # Math library
endif()

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Installation rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Optional: Create different build configurations
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(STATIC_RUNTIME)
        if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
            target_link_options(${PROJECT_NAME} PRIVATE -static)
        elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
            set_property(TARGET ${PROJECT_NAME} PROPERTY
                MSVC_RUNTIME_LIBRARY "MultiThreaded")
        endif()
    endif()
endif()