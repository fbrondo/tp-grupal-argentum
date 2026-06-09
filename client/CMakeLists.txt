target_sources(taller_client
    PRIVATE
    # .cpp files
        src/main.cpp
        src/client.cpp
        src/client_protocol.cpp
        src/client_sender.cpp
        src/client_receiver.cpp
        src/commands/command_move.cpp
        src/menu_handler.cpp
        src/command_processor.cpp
    PUBLIC
    # .h files
        includes/client_protocol.h
        includes/menu_handler.h
        includes/command_processor.h
        includes/client.h
        includes/client_sender.h
        includes/client_receiver.h
        includes/commands/command_client.h
        includes/commands/command_move.h
    )

if(NOT DEFINED ARGENTUM_SHARE_PATH)
    set(ARGENTUM_SHARE_PATH "${CMAKE_SOURCE_DIR}")
endif()
target_compile_definitions(taller_client PRIVATE
    "ARGENTUM_SHARE_PATH=\"${ARGENTUM_SHARE_PATH}\"")
