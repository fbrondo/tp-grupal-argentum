target_sources(taller_server
    PRIVATE
    # .cpp files
        src/commands/command_attack.cpp
        src/commands/command_buy_item.cpp
        src/commands/command_chat.cpp
        src/commands/command_chat.cpp
        src/commands/command_deposit.cpp
        src/commands/command_deposit_gold.cpp
        src/commands/command_disconnect.cpp
        src/commands/command_drop_item.cpp
        src/commands/command_heal.cpp
        src/commands/command_interact.cpp
        src/commands/command_list_items.cpp
        src/commands/command_login.cpp
        src/commands/command_meditate.cpp
        src/commands/command_move.cpp
        src/commands/command_resurrect.cpp
        src/commands/command_sell_item.cpp
        src/commands/command_signup.cpp
        src/commands/command_take_item.cpp
        src/commands/command_use_item.cpp
        src/commands/command_withdraw.cpp
        src/commands/command_withdraw_gold.cpp

        src/entity/combat_entity.cpp
        src/npc/banker.cpp
        src/npc/creature.cpp
        src/npc/citizen_npc.cpp
        src/npc/merchant.cpp
        src/npc/priest.cpp
        src/npc/trader.cpp

        src/responses/response_action_error.cpp
        src/responses/response_bank_content.cpp
        src/responses/response_change_map.cpp
        src/responses/response_chat_msg.cpp
        src/responses/response_inventory_update.cpp
        src/responses/response_login.cpp
        src/responses/response_map.cpp
        src/responses/response_player_stats.cpp
        src/responses/response_map.cpp
        src/responses/response_signup.cpp
        src/responses/response_snapshot.cpp
        src/responses/response_trader_catalog.cpp

        src/acceptor.cpp
        src/character.cpp
        src/client_handler.cpp
        src/client_receiver.cpp
        src/client_sender.cpp
        src/equipment.cpp
        src/game_config_loader.cpp
        src/game_formulas.cpp
        src/gameloop.cpp
        src/inventory.cpp
        src/main.cpp
        src/items_positions.cpp
        src/monitor_queues.cpp
        src/npcs_positions.cpp
        src/persistence.cpp
        src/data_storage.cpp
        src/player.cpp
        src/response_builder.cpp
        src/server_protocol.cpp
        src/server.cpp
        src/slot.cpp
        src/spawn_manager.cpp
        src/world.cpp

        print.cpp
        
    PUBLIC
    # .h files
        includes/commands/command.h
        includes/commands/command_attack.h
        includes/commands/command_buy_item.h
        includes/commands/command_chat.h
        includes/commands/command_deposit.h
        includes/commands/command_deposit_gold.h
        includes/commands/command_disconnect.h
        includes/commands/command_drop_item.h
        includes/commands/command_heal.h
        includes/commands/command_interact.h
        includes/commands/command_login.h
        includes/commands/command_meditate.h
        includes/commands/command_move.h
        includes/commands/command_resurrect.h
        includes/commands/command_sell_item.h
        includes/commands/command_signup.h
        includes/commands/command_take_item.h
        includes/commands/command_use_item.h
        includes/commands/command_withdraw.h
        includes/commands/command_withdraw_gold.h

        includes/core/bank.h
        includes/core/clase.h
        includes/core/config.h
        includes/core/data.h
        includes/core/instances.h
        includes/core/inventory.h
        includes/core/item.h
        includes/core/map.h
        includes/core/race.h
        includes/core/region.h
        includes/core/snapshot.h
        includes/core/treasure.h

        includes/entity/entity.h
        includes/entity/combat_entity.h

        includes/npc/banker.h
        includes/npc/citizen_npc.h
        includes/npc/creature.h
        includes/npc/merchant.h
        includes/npc/citizen_npc.h
        includes/npc/priest.h

        includes/responses/response.h
        includes/responses/response_action_error.h
        includes/responses/response_bank_content.h
        includes/responses/response_change_map.h
        includes/responses/response_chat_msg.h
        includes/responses/response_inventory_update.h
        includes/responses/response_login.h
        includes/responses/response_map.h
        includes/responses/response_player_stats.h
        includes/responses/response_signup.h
        includes/responses/response_snapshot.h
        includes/responses/response_trader_catalog.h

        includes/acceptor.h
        includes/character.h
        includes/client_handler.h
        includes/client_receiver.h
        includes/client_sender.h
        includes/definitions.h
        includes/equipment.h
        includes/game_config_loader.h
        includes/game_formulas.h
        includes/gameloop.h
        includes/items_positions.h
        includes/monitor_queues.h
        includes/npcs_positions.h
        includes/persistence.h
        includes/data_storage.h
        includes/player.h
        includes/response_builder.h
        includes/server.h
        includes/server_protocol.h
        includes/slot.h
        includes/spawn_manager.h
        includes/world.h

        print.h

    )

if(NOT DEFINED ARGENTUM_CONFIG_PATH)
    set(ARGENTUM_CONFIG_PATH "${CMAKE_SOURCE_DIR}/server/config")
endif()
target_compile_definitions(taller_server PRIVATE
    "CONFIG_PATH=\"${ARGENTUM_CONFIG_PATH}\""
)
