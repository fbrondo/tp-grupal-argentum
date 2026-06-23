#ifndef BANK_H
#define BANK_H

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "server/includes/core/account.h"

struct Bank {
    std::map<std::string, Account> accounts;
};
#endif
