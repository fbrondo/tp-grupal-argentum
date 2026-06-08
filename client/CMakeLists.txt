target_sources(taller_client
    PRIVATE
    # .cpp files
        src/main.cpp
        src/client.cpp
        src/client_protocol.cpp
        src/client_sender.cpp
        src/client_receiver.cpp
        src/menu_handler.cpp
        src/command_processor.cpp

        src/commands/command_create_character.cpp
        src/commands/command_login.cpp
        src/commands/command_move.cpp
        src/sound_manager.cpp

    PUBLIC
    # .h files
        includes/client.h
        includes/client_protocol.h
        includes/client_receiver.h
        includes/client_sender.h
        includes/command_processor.h
        includes/menu_handler.h

        includes/commands/command_client.h
        includes/commands/command_move.h
        includes/commands/command_login.h
        includes/commands/command_create_character.h
        includes/sound_manager.h
    )
