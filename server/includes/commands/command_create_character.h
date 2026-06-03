#pragma once

#include <string>

#include "../definitions.h"
#include "../world.h"
#include "common/includes/types.h"

#include "command.h"

class CreateCharacterCommand: public Command {
private:
    std::string username;
    std::string password;
    TypeRace race;
    TypeClase clase;

public:
    CreateCharacterCommand(Id client_id, std::string&& name, std::string&& pass, TypeRace race,
                           TypeClase clase);
    RegisterInfo getRegistrationInfo();
    void execute(World& world) override;
};
