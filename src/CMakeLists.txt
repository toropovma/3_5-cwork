file(GLOB_RECURSE SOURCE_FILES **.cpp)

set(BINARY_NAME ${PROJECT_NAME})

add_executable(
    ${BINARY_NAME}
    ${SOURCE_FILES}
)

target_include_directories(
    ${BINARY_NAME}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(
    ${BINARY_NAME}
    PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component
    spdlog
)
