add_executable(modern_sfm_tests
    core/test_log.cpp
    core/test_types.cpp
)

target_link_libraries(modern_sfm_tests PRIVATE
    modern_sfm_core
    doctest::doctest
)

target_compile_features(modern_sfm_tests PRIVATE cxx_std_23)

# Register with CTest
add_test(NAME modern_sfm_tests COMMAND modern_sfm_tests)
