target_sources(taller_server
    PRIVATE
    # .cpp files
        src/main.cpp
        src/server_protocol.cpp
    PUBLIC
    # .h files
        includes/server_protocol.h
    )
