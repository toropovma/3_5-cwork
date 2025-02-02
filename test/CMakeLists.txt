include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.x
)
FetchContent_MakeAvailable(googletest)

set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(BUILD_GTEST ON CACHE BOOL "" FORCE)

add_executable(
    tests
    main.cpp

    ${CMAKE_SOURCE_DIR}/src/data.cpp
    ${CMAKE_SOURCE_DIR}/src/database.cpp
    ${CMAKE_SOURCE_DIR}/src/binarydatabase.cpp

    db/binary.cpp
)

target_include_directories(
    tests
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(
    tests
    PRIVATE
    gtest
    spdlog
)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(
        tests
        PRIVATE
        -O2
    )
endif()
