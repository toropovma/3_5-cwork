cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME cwork)
set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS FALSE)

project(
    ${PROJECT_NAME}
    LANGUAGES CXX
)

include(FetchContent)

FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
    GIT_TAG        v5.0.0
)
FetchContent_MakeAvailable(ftxui)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v2.x
)
FetchContent_MakeAvailable(spdlog)

add_subdirectory(src)

if (${BUILD_WITH_TESTS})
    add_subdirectory(test)
endif()
