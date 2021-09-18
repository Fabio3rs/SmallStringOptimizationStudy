set(apiname smallstringoptimization)

cmake_minimum_required(VERSION 3.6.0)
project(${apiname} VERSION 0.1.0)

set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-header-filter=.;-extra-arg=-std=c++17;-export-fixes;-checks=read*,modern*,hicpp*,performance*")

include(CTest)
enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

add_subdirectory(${CMAKE_SOURCE_DIR}/tests)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(CPack)

find_package(PkgConfig)

# setando todos arquivos fontes que serão compilados
file(
    GLOB SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# setando todos arquivos include
set(INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/"
)

include_directories(${INCLUDE_DIRS})

add_executable(${apiname} ${SOURCES})

# Compiler-specific compile flags
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    message(STATUS "Setting G++ flags")
    # GCC/G++
    target_compile_options(${apiname} PRIVATE -g -Wall -Werror -Wextra -O2 -std=c++17 -Wformat-security -Wconversion -Wsign-conversion)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    message(STATUS "Setting MSVC flags")
    # MSVC
    target_compile_options(${apiname} PRIVATE /EHsc /W2 /c)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    message(STATUS "Setting Clang flags")
    # LLVM Clang/Clang++
    target_compile_options(${apiname} PRIVATE -g -Weverything -Werror -Wno-disabled-macro-expansion -Wno-global-constructors -std=c++17 -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-exit-time-destructors)
endif()


