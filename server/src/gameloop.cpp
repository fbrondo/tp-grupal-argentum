#include "server/includes/gameloop.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include "server/includes/core/data.h"
#include "server/includes/core/snapshot.h"
#include "server/includes/responses/response_login.h"
#include "server/includes/responses/response_signup.h"
#include "server/includes/responses/response_snapshot.h"
#include "server/print.h"

#define INVALID_REGISTER "Username already taken."
#define INVALID_LOGIN "El usuario o la contraseña son incorrectos."

Gameloop::Gameloop(GameConfigLoader& loader_conf, MonitorQueues& monitor, QueueCmd& cmmds_queue):
        monitor(monitor),
        commands_queue(cmmds_queue),
        files_data(loader_conf.getFilesData()),
        game_conf(loader_conf.getdGameConfiguration()),
        world(this->files_data.map),
        persistence(this->files_data) {
    loader_conf.loadRaces(this->info_races);
    loader_conf.loadClases(this->info_clases);
    loader_conf.loadItems(this->info_items);
}

void Gameloop::handleSignup(SignupCommand* cmd) {
    auto [id, username, pass, traits] = cmd->getSignupInfo();
    Print::printNewPlayerArrived(id, username, pass, static_cast<TypeRace>(traits.race),
                                 static_cast<TypeClase>(traits.clase));
    if (this->persistence.exists(username)) {
        this->monitor.queueTheServerResponse(
                id, std::make_unique<ResponseSignup>(false, INVALID_REGISTER));
        return;
    }
    /*Creacion del jugador*/
    cmd->execute(this->world); /*Ubico al jugador en el mundo*/
    User user(std::move(username), std::move(pass));
    Character character(this->info_races.at(static_cast<TypeRace>(traits.race)),
                        this->info_clases.at(static_cast<TypeClase>(traits.clase)), traits.head,
                        traits.body);
    Pose pose(this->world.positionPlayerInTheWorld(id), UP);
    std::unique_ptr<Player> new_player = std::make_unique<Player>(
            std::move(user), std::move(pose), std::move(character), this->game_conf.player_init);
    PlayerData player_data = new_player->getPlayerData();
    this->persistence.savePlayer(player_data);
    this->players.emplace(id, std::move(new_player));

    /*Informacion que se envia al usuario*/
    this->monitor.queueTheServerResponse(id, std::make_unique<ResponseLogin>(true));
    this->executeBroacastSnapshot();
}

void Gameloop::handleLogin(LoginCommand* cmd) {
    auto [id, username, password] = cmd->getLoginInfo();
    if (!this->persistence.exists(username)) {
        this->monitor.queueTheServerResponse(
                id, std::make_unique<ResponseLogin>(false, "User not found."));
        return;
    }
    PlayerData data = this->persistence.loadPlayer(username);
    if (std::string(data.password) != password) {
        this->monitor.queueTheServerResponse(
                id, std::make_unique<ResponseLogin>(false, "Wrong password."));
        return;
    }
    if (data.level == 0) {
        this->monitor.queueTheServerResponse(id, std::make_unique<ResponseLogin>(true, "none"));
        return;
    }
    // std::ostringstream payload;
    // payload << data.username << " " << static_cast<int>(data.race) << " "
    //         << static_cast<int>(data.clase) << " " << static_cast<int>(data.level);
    // this->monitor.queueTheServerResponse(id, std::make_unique<ResponseLogin>(true,
    // payload.str()));
}

void Gameloop::executeBroacastSnapshot() {
    Snapshot snap = this->resp.buildSnapshot(this->players, this->world);
    std::shared_ptr<ResponseSnapshot> resp_snap =
            std::make_unique<ResponseSnapshot>(std::move(snap));
    this->monitor.executeBroadcast(std::move(resp_snap));
}

void Gameloop::executeMovePlayer(MoveCommand* cmd) {
    auto [player_id, direction] = cmd->getMoveInfo();
    if (this->world.isWalkable(player_id, direction)) {
        cmd->execute(this->world);
    }
}

bool Gameloop::isItPossibleToAttack(const Id& player_id, const Id& victim_id, Weapon& weapon) {
    MagicWeapon* magic_weapon = dynamic_cast<MagicWeapon*>(&weapon);
    if (magic_weapon && !this->players[player_id]->hasEnoughMana(magic_weapon->range_attack)) {
        return false;
    }
    uint16_t range = weapon.range_attack;
    int distance = this->world.distanceBetweenTheAttackerAndTheVictim(player_id, victim_id);
    if (distance > range) {
        return false;
    }
    return true;
}

CombatEntity* Gameloop::inSearchOfTheVictimAttack(const Id& id_search) {
    CombatEntity* victim = nullptr;
    if (this->players.count(id_search) > 0) {
        victim = dynamic_cast<CombatEntity*>(this->players.at(id_search).get());
    }
    if (this->info_NPC.count(id_search) > 0) {
        NPC* npc_generico = this->info_NPC.at(id_search).get();
        victim = dynamic_cast<CombatEntity*>(npc_generico);
    }
    return victim;
}

std::vector<Defense*> Gameloop::getInfoAboutThePlayerDefensiveEquipment(const Id& player_id) {
    std::vector<Defense*> info_equipment_defensive;
    std::vector<TypeItem> equipmentTypes = this->players[player_id]->getEquipment();
    for (auto Type: equipmentTypes) {
        Defense* item_defensive = dynamic_cast<Defense*>(this->info_items.at(Type).get());
        info_equipment_defensive.push_back(item_defensive);
    }
    return info_equipment_defensive;
}


void Gameloop::executeAttackPlayer(AttackCommand* cmd) {

    auto [attacker_id, victim_id] = cmd->getAttackInfo();
    /*Como ahora player desciende de otra clase es necesario usar unique_ptr*/
    Player* attacker = this->players.at(attacker_id).get();
    if (!attacker->isAlive())
        return;
    /*Vemos si tenemos un arma euipada*/
    TypeItem weapon_type = attacker->getHandItem();
    if (weapon_type == NONE)
        return;
    /*Buscamos a lo que se pide atacar*/
    CombatEntity* victim = this->inSearchOfTheVictimAttack(victim_id);
    if (!victim)
        return;

    Weapon* weapon = dynamic_cast<Weapon*>(this->info_items.at(weapon_type).get());
    if (!weapon)
        return;
    if (!this->isItPossibleToAttack(attacker_id, victim_id, *weapon)) {
        return;
    }
    bool is_critical = false;
    uint16_t damage_by_attacker = attacker->calculateDamage(is_critical, *weapon);
    if (!is_critical && victim->dodgeAttack()) {
        return;  // Registrar sonido de esquivado
    }
    if (Player* player = dynamic_cast<Player*>(victim)) { /*Si la victima es un jugador*/
        std::vector<Defense*> info_equip_defensive =
                this->getInfoAboutThePlayerDefensiveEquipment(victim_id);
        uint16_t defense_victim = player->calculateDefense(info_equip_defensive);
        damage_by_attacker =
                (damage_by_attacker > defense_victim) ? (damage_by_attacker - defense_victim) : 0;
    }
    victim->receiveDamage(damage_by_attacker);
}

void Gameloop::process_buy_item(Id player_id, Id npc_id, Id item_id) {
    NpcInstance* npc = this->world.getNpcById(npc_id);
    NPC* npc_generico = this->info_NPC.at(npc_id).get();
    TraderNPC* trader = dynamic_cast<TraderNPC*>(npc_generico);
    if (!trader) {
        return; // Enviar error como "Este NPC no vende ni compra nada."
    }

    auto& tienda = trader->getStore();
    TypeItem tipo_buscado = static_cast<TypeItem>(item_id);
    if (tienda.count(tipo_buscado) == 0) {
        return;  // Enviar error como "El NPC no vende ese ítem."
    }

    Item* item_template = tienda.at(tipo_buscado).get();

    if (!this->players[player_id]->canBuy(item_template)) {
        return; // Enviar error como "Oro insuficiente o inventario lleno."
    }

    Id id_instance = this->next_item_instance_id++;
    this->players[player_id]->buyItem(item_template, id_instance);
}

void Gameloop::process_sell_item(Id player_id, Id npc_id, Id instance_id) {
    NpcInstance* npc = this->world.getNpcById(npc_id);
    NPC* npc_generico = this->info_NPC.at(npc_id).get();
    TraderNPC* trader = dynamic_cast<TraderNPC*>(npc_generico);
    if (!trader) {
        return; // Enviar error como "Este NPC no vende ni compra nada."
    }

    Player *player = this->players[player_id].get();
    if (!player->canSell(instance_id)) {
        return; // Error: "No tenés ese ítem en el inventario"
    }

    auto& tienda = trader->getStore();
    
    ItemInstance *itemInstance = player->getItemInstance(instance_id);
    TypeItem tipo_buscado = itemInstance->type;
    if (tienda.count(tipo_buscado) == 0) {
        return;  // Enviar error como "El NPC no compra ese ítem."
    }

    Item* item_template = tienda.at(tipo_buscado).get();
    uint32_t sell_price = item_template->selling_price;

    player->sellItem(instance_id, sell_price);
}


void Gameloop::execuetRequest() {
    std::unique_ptr<Command> cmd;
    while (this->commands_queue.try_pop(cmd)) {
        try {
            cmd->execute(*this); //Cambiar world en command a gameloop (cuando termine los comandos cambio todos)
        } catch (const std::exception& e) {
            std::cerr << "[Gameloop] execuetRequest() exited: " << e.what() << std::endl;
        }
    }
}

void Gameloop::run() {
    std::cerr << "[Gameloop] run() started" << std::endl;
    try {
        while (should_keep_running()) {
            this->execuetRequest();
            //world.update();
            this->executeBroacastSnapshot();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    } catch (const ClosedQueue&) {
        std::cerr << "[Gameloop] run() exited: ClosedQueue" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[Gameloop] run() exited: " << e.what() << std::endl;
    }
    std::cerr << "[Gameloop] run() finished" << std::endl;
}
