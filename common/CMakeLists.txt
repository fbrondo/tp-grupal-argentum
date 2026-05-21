target_sources(taller_common
    PRIVATE
    # .cpp files
        src/toml_config.cpp
        src/liberror.cpp
        src/resolver.cpp
        src/resolvererror.cpp
        src/socket.cpp
        src/map/map.cpp
        src/map/map_serializer.cpp
    PUBLIC
    # .h files
        includes/protocol.h
        includes/toml_config.h
        includes/liberror.h
        includes/resolver.h
        includes/resolvererror.h
        includes/socket.h
        includes/map/tile.h
        includes/map/layer.h
        includes/map/map.h
        includes/map/map_serializer.h
    )
