#include "server/includes/core/item.h"

#include "server/includes/player.h"

bool Item::use(Player& user) const {
    if (this->type == LIFE_POTION) {
        user.restoreAllHp();
        return true;
    } else if (this->type == MANA_POTION) {
        user.restoreAllMana();
        return true;
    }
    return false;
}
