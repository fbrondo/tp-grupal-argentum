target_sources(taller_client
    PRIVATE
    # .cpp files
        src/main.cpp
        src/client_protocol.cpp
    PUBLIC
    # .h files
        includes/client_protocol.h
    )
