#include "../../includes/commands/command_create_character.h"

CreateCharacterCommand::CreateCharacterCommand(Id client_id, std::string&& name, std::string&& pass, TypeRace race, TypeClase clase) :
        Command(client_id),
        username(std::move(name)),
        password(std::move(pass)),
        race(race),
        clase(clase) {}
        
RegisterInfo CreateCharacterCommand::getRegistrationInfo() {
    return std::make_tuple(client_id, this->username, this->password, this->race, this->clase);
}  
void CreateCharacterCommand::execute(World& world) {
    world.spawnPlayer(client_id);
}