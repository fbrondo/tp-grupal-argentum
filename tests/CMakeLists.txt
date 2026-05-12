target_sources(taller_tests
    PRIVATE
    # .cpp files
    toml_config_test.cpp
    PUBLIC
    # .h files
    )

# Exposes the absolute path to the test config file so tests don't depend
# on the working directory when the binary is executed.
target_compile_definitions(taller_tests PRIVATE
    TOML_TEST_CONFIG_PATH="${CMAKE_CURRENT_SOURCE_DIR}/config_test.toml"
    )
