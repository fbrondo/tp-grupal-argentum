#ifndef BANK_H
#define BANK_H

#include <map>
#include <string>
#include <cstdint>
#include "server/includes/slot.h"

struct  Account {
    uint32_t golden{0};
    uint32_t size_items{0};
    std::vector<Slot> safe_box;
    Account() = default;
};

struct Bank {
    std::map<std::string, Account> accounts;
};
#endif //TALLER_TP_BANK_H
