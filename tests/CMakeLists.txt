# Configuração dos testes no CMake/GoogleTest

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
set(CMAKE_CXX_STANDARD 17)

set(srcdir "${CMAKE_CURRENT_SOURCE_DIR}")
set(msrcdir "${CMAKE_CURRENT_SOURCE_DIR}/../src")
set(mainbinary "${CMAKE_CURRENT_SOURCE_DIR}/../bin")

set(TEST_COMPILE_FLAGS -Weverything -Werror -Wno-disabled-macro-expansion -Wno-global-constructors -std=c++17 -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-exit-time-destructors -Wno-global-constructors)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Werror -std=c++17 -Wno-c++98-compat -Wno-padded -Wno-date-time -Wno-c++98-compat-pedantic -Wno-exit-time-destructors -Wno-global-constructors")

add_executable(teststring test_string.cpp)
target_compile_options(teststring PUBLIC ${TEST_COMPILE_FLAGS})

target_link_libraries(teststring GTest::GTest GTest::Main)

gtest_discover_tests(teststring)

