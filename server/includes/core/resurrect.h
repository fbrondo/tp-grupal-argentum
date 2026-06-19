#ifndef RESURRECT_H
#define RESURRECT_H

#include "common/includes/types.h"

struct ResurrectPending {
    uint32_t time_left_ms;
    Id healer_id;
};
#endif
