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
        src/sprite_loader.cpp
    PUBLIC
    # .h files
        includes/core/Statistics.h
        includes/core/character_traits.h
        includes/core/user.h

        includes/map/tile.h
        includes/map/layer.h
        includes/map/map.h
        includes/map/map_serializer.h

        includes/direction.h
        includes/liberror.h
        includes/opcode.h
        includes/protocol.h
        includes/queue.h
        includes/resolver.h
        includes/resolvererror.h
        includes/socket.h
        includes/sprite_definition.h
        includes/sprite_loader.h
        includes/thread.h
        includes/toml_config.h
        includes/types.h
    )
