target_sources(taller_client
    PRIVATE
    # .cpp files
        src/main.cpp
        src/client.cpp
        src/client_protocol.cpp
        src/client_sender.cpp
        src/client_receiver.cpp
        src/commands/command_move.cpp
        src/commands/command_create_character.cpp
        src/texture_manager.cpp
    PUBLIC
    # .h files
        includes/client_protocol.h
        includes/client.h
        includes/client_sender.h
        includes/client_receiver.h
        includes/commands/command_client.h
        includes/commands/command_move.h
        includes/commands/command_create_character.h
        includes/texture_manager.h
    )
