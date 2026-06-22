#pragma once
#include <filesystem>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "common/includes/core/position.h"

// Result structure for clan operations
struct ClanOpResult {
    std::string msg;
    std::string target_nick;
    std::string target_msg;
};

class ClanManager {
private:
    struct ClanEntry {
        std::string name;
        std::string founder;
        std::vector<std::string> members;
        std::vector<std::string> pending;
        // we use std::set for banned
        std::set<std::string> banned;
    };

    std::map<std::string, ClanEntry> clans;
    std::map<std::string, std::string> player_clan;
    std::filesystem::path data_path;

    // check if is necessary move this methods to other class
    static void writeString(std::ostream& out, const std::string& s);
    static std::string readString(std::istream& in);
    void rebuildIndex();

public:
    explicit ClanManager(std::filesystem::path path);

    // Clan operations
    ClanOpResult foundClan(const std::string& founder, const std::string& clan_name, uint8_t level,
                           uint8_t min_level);
    ClanOpResult requestJoin(const std::string& username, const std::string& clan_name);
    ClanOpResult reviewClan(const std::string& founder) const;
    ClanOpResult acceptMember(const std::string& founder, const std::string& nick,
                              uint8_t max_members);
    ClanOpResult rejectMember(const std::string& founder, const std::string& nick);
    ClanOpResult banMember(const std::string& founder, const std::string& nick);
    ClanOpResult kickMember(const std::string& founder, const std::string& nick);
    ClanOpResult leaveClan(const std::string& username);

    // other operations
    bool areInSameClan(const std::string& a, const std::string& b) const;
    std::string getClanOf(const std::string& username) const;
    std::vector<std::string> getMembersOf(const std::string& clan_name) const;
    std::vector<std::string> listClans() const;
    uint8_t countNearbyMembers(const std::string& username, const Position& pos,
                               const std::map<std::string, Position>& online_positions,
                               uint32_t range) const;

    void load();
    void save() const;
};
