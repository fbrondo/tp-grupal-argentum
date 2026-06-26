target_sources(taller_tests
    PRIVATE
    # .cpp files
    toml_config_test.cpp
    map_test.cpp
    sprite_loader_test.cpp
    protocol_test.cpp
    ../client/src/client_protocol.cpp
    PUBLIC
    # .h files
    )
