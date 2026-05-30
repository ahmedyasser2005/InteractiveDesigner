if(NOT TARGET app_core)
    message(FATAL_ERROR "The test suite requires the app_core target to be defined first.")
endif()

find_package(Catch2 CONFIG REQUIRED)

add_executable(unit_tests)

target_sources(unit_tests
    PRIVATE
        MainTest.cpp
)

target_link_libraries(unit_tests
    PRIVATE
        app_core
        Catch2::Catch2WithMain
)

include(CTest)
include(Catch)
catch_discover_tests(unit_tests)