#pragma once

#include <utility>
#include "server/includes/core/map.h"

class Entity {
protected:
    Pose pose;

public:
    explicit Entity(Pose&& pose_): pose(std::move(pose_)) {}
    virtual ~Entity() = default;
    [[nodiscard]] const Position& getPosition() const { return this->pose.position; }
    virtual void updatePosition(Pose&& new_pose) { this->pose = std::move(new_pose); }
    virtual bool isAttackable() const { return false; }
};
