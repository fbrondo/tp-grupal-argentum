target_sources(taller_server
    PRIVATE
    # .cpp files
        src/commands/command_attack.cpp
        src/commands/command_buy_item.cpp
        src/commands/command_chat.cpp
        src/commands/command_create_character.cpp
        src/commands/command_disconnect.cpp
        src/commands/command_drop_item.cpp
        src/commands/command_interact.cpp
        src/commands/command_login.cpp
        src/commands/command_move.cpp
        src/commands/command_use_item.cpp
        src/commands/command_sell_item.cpp
        src/commands/command_take_item.cpp
        
        src/npc/banker.cpp
        src/npc/criature.cpp
        src/npc/merchant.cpp
        src/npc/priest.cpp

        src/responses/response_login.cpp
        src/responses/response_snapshot.cpp
        
        src/acceptor.cpp
        src/character.cpp
        src/client_handler.cpp
        src/client_receiver.cpp
        src/client_sender.cpp
        src/equipment.cpp
        src/game_config.cpp
        src/game_formulas.cpp
        src/gameloop.cpp
        src/main.cpp
        src/monitor_queues.cpp
        src/persistence_manager.cpp
        src/player.cpp
        src/response_builder.cpp
        src/server_protocol.cpp
        src/server.cpp
        src/world.cpp
        
    PUBLIC
    # .h files
        includes/commands/command.h
        includes/commands/command_attack.h
        includes/commands/command_buy_item.h
        includes/commands/command_chat.h
        includes/commands/command_disconnect.h
        includes/commands/command_drop_item.h
        includes/commands/command_interact.h
        includes/commands/command_login.h
        includes/commands/command_move.h
        includes/commands/command_use_item.h
        includes/commands/command_sell_item.h
        includes/commands/command_take_item.h
        includes/commands/command_create_character.h

        includes/core/clase.h
        includes/core/config.h
        includes/core/data.h
        includes/core/instances.h
        includes/core/inventory.h
        includes/core/item.h
        includes/core/map.h
        includes/core/race.h
        includes/core/snapshot.h
       
        includes/npc/banker.h
        includes/npc/citynpc.h
        includes/npc/criature.h
        includes/npc/merchant.h
        includes/npc/npc.h
        includes/npc/priest.h

        includes/responses/response.h
        includes/responses/response_login.h
        includes/responses/response_snapshot.h

        includes/acceptor.h
        includes/character.h
        includes/client_handler.h
        includes/client_receiver.h
        includes/client_sender.h
        includes/definitions.h
        includes/equipment.h
        includes/game_config.h
        includes/game_formulas.h
        includes/gameloop.h
        includes/monitor_queues.h
        includes/persistence_manager.h
        includes/player.h
        includes/response_builder.h
        includes/server_protocol.h
        includes/server.h
        includes/world.h

    )

target_compile_definitions(taller_server PRIVATE 
"CONFIG_PATH=\"${CMAKE_SOURCE_DIR}/server/config\""
)
