#include "server/includes/clan_manager.h"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <stdexcept>

ClanManager::ClanManager(std::filesystem::path path): data_path(std::move(path)) {}

void ClanManager::writeString(std::ostream& out, const std::string& s) {
    auto len = static_cast<uint8_t>(s.size());
    out.write(reinterpret_cast<const char*>(&len), sizeof(len));
    out.write(s.data(), len);
}

std::string ClanManager::readString(std::istream& in) {
    uint8_t len = 0;
    in.read(reinterpret_cast<char*>(&len), sizeof(len));
    std::string s(len, '\0');
    in.read(s.data(), len);
    return s;
}

void ClanManager::rebuildIndex() {
    player_clan.clear();
    for (const auto& [clan_name, entry]: clans) {
        for (const auto& member: entry.members) {
            player_clan[member] = clan_name;
        }
    }
}

void ClanManager::load() {
    if (!std::filesystem::exists(data_path))
        return;
    std::ifstream in(data_path, std::ios::binary);
    if (!in.is_open())
        return;
    uint32_t num_clans = 0;
    in.read(reinterpret_cast<char*>(&num_clans), sizeof(num_clans));
    for (uint32_t i = 0; i < num_clans; ++i) {
        ClanEntry entry;
        entry.name = readString(in);
        entry.founder = readString(in);
        uint32_t n = 0;
        in.read(reinterpret_cast<char*>(&n), sizeof(n));
        entry.members.resize(n);
        for (auto& m: entry.members) m = readString(in);
        in.read(reinterpret_cast<char*>(&n), sizeof(n));
        entry.pending.resize(n);
        for (auto& p: entry.pending) p = readString(in);
        in.read(reinterpret_cast<char*>(&n), sizeof(n));
        for (uint32_t j = 0; j < n; ++j) entry.banned.insert(readString(in));
        clans[entry.name] = std::move(entry);
    }
    rebuildIndex();
}

void ClanManager::save() const {
    std::filesystem::create_directories(data_path.parent_path());
    std::ofstream out(data_path, std::ios::binary | std::ios::trunc);
    if (!out.is_open())
        throw std::runtime_error("No se pudo guardar el archivo de clanes");
    auto num_clans = static_cast<uint32_t>(clans.size());
    out.write(reinterpret_cast<const char*>(&num_clans), sizeof(num_clans));
    for (const auto& [clan_name, entry]: clans) {
        writeString(out, entry.name);
        writeString(out, entry.founder);
        auto n = static_cast<uint32_t>(entry.members.size());
        out.write(reinterpret_cast<const char*>(&n), sizeof(n));
        for (const auto& m: entry.members) writeString(out, m);
        n = static_cast<uint32_t>(entry.pending.size());
        out.write(reinterpret_cast<const char*>(&n), sizeof(n));
        for (const auto& p: entry.pending) writeString(out, p);
        n = static_cast<uint32_t>(entry.banned.size());
        out.write(reinterpret_cast<const char*>(&n), sizeof(n));
        for (const auto& b: entry.banned) writeString(out, b);
    }
}

// --------------------
// Main clan operations
// --------------------

ClanOpResult ClanManager::foundClan(const std::string& founder, const std::string& clan_name,
                                    uint8_t level, uint8_t min_level) {
    if (clan_name.empty())
        return {"El nombre del clan no puede estar vacio.", "", ""};
    if (level < min_level)
        return {"Necesitas nivel " + std::to_string(min_level) + " para fundar un clan.", "", ""};
    if (player_clan.contains(founder))
        return {"Ya perteneces a un clan.", "", ""};
    if (clans.contains(clan_name))
        return {"Ya existe un clan con ese nombre.", "", ""};

    ClanEntry entry;
    entry.name = clan_name;
    entry.founder = founder;
    entry.members.push_back(founder);
    clans[clan_name] = std::move(entry);
    player_clan[founder] = clan_name;

    save();
    return {"Clan \"" + clan_name + "\" fundado exitosamente.", "", ""};
}

ClanOpResult ClanManager::requestJoin(const std::string& username, const std::string& clan_name) {
    if (player_clan.contains(username))
        return {"Ya perteneces a un clan.", "", ""};
    if (!clans.contains(clan_name))
        return {"No existe el clan \"" + clan_name + "\".", "", ""};

    ClanEntry& entry = clans.at(clan_name);
    if (entry.banned.contains(username))
        return {"Tu solicitud fue rechazada.", "", ""};

    const auto it = std::find(entry.pending.begin(), entry.pending.end(), username);
    if (it != entry.pending.end())
        return {"Ya enviaste una solicitud a ese clan.", "", ""};
    entry.pending.push_back(username);

    save();
    return {"Solicitud enviada al clan \"" + clan_name + "\".", "", ""};
}

ClanOpResult ClanManager::reviewClan(const std::string& founder) const {
    if (!player_clan.contains(founder))
        return {"No perteneces a ningun clan.", "", ""};

    const std::string& clan_name = player_clan.at(founder);
    const ClanEntry& entry = clans.at(clan_name);

    if (entry.founder != founder)
        return {"Solo el fundador puede revisar el clan.", "", ""};

    std::ostringstream ss;
    ss << "=== Clan: " << clan_name << " ===\n";
    ss << "Miembros (" << entry.members.size() << "):\n";
    for (const auto& m: entry.members)
        ss << "  - " << m << (m == entry.founder ? " [fundador]" : "") << "\n";

    if (entry.pending.empty()) {
        ss << "Solicitudes pendientes: ninguna.";
    } else {
        ss << "Solicitudes pendientes (" << entry.pending.size() << "):\n";
        for (const auto& p: entry.pending) ss << "  - " << p << "\n";
    }

    return {ss.str(), "", ""};
}

ClanOpResult ClanManager::acceptMember(const std::string& founder, const std::string& nick,
                                       uint8_t max_members) {
    if (!player_clan.contains(founder))
        return {"No perteneces a ningun clan.", "", ""};

    const std::string& clan_name = player_clan.at(founder);
    ClanEntry& entry = clans.at(clan_name);
    if (entry.founder != founder)
        return {"Solo el fundador puede aceptar miembros.", "", ""};
    if (static_cast<uint32_t>(entry.members.size()) >= max_members)
        return {"El clan alcanzo el limite de " + std::to_string(max_members) + " miembros.", "",
                ""};

    const auto it = std::find(entry.pending.begin(), entry.pending.end(), nick);
    if (it == entry.pending.end())
        return {nick + " no tiene una solicitud pendiente.", "", ""};
    if (player_clan.contains(nick))
        return {nick + " ya pertenece a otro clan.", "", ""};

    entry.pending.erase(it);
    entry.members.push_back(nick);
    player_clan[nick] = clan_name;

    save();
    return {"Aceptaste a " + nick + " en el clan.", nick,
            "Fuiste aceptado en el clan \"" + clan_name + "\"."};
}

ClanOpResult ClanManager::rejectMember(const std::string& founder, const std::string& nick) {
    if (!player_clan.contains(founder))
        return {"No perteneces a ningun clan.", "", ""};

    const std::string& clan_name = player_clan.at(founder);
    ClanEntry& entry = clans.at(clan_name);
    if (entry.founder != founder)
        return {"Solo el fundador puede rechazar solicitudes.", "", ""};

    const auto it = std::find(entry.pending.begin(), entry.pending.end(), nick);
    if (it == entry.pending.end())
        return {nick + " no tiene una solicitud pendiente.", "", ""};

    entry.pending.erase(it);

    save();
    return {"Rechazaste la solicitud de " + nick + ".", nick,
            "Tu solicitud para unirte al clan \"" + clan_name + "\" fue rechazada."};
}

ClanOpResult ClanManager::banMember(const std::string& founder, const std::string& nick) {
    if (!player_clan.contains(founder))
        return {"No perteneces a ningun clan.", "", ""};

    const std::string& clan_name = player_clan.at(founder);
    ClanEntry& entry = clans.at(clan_name);
    if (entry.founder != founder)
        return {"Solo el fundador puede banear jugadores.", "", ""};
    if (nick == founder)
        return {"No puedes banearte a ti mismo.", "", ""};

    const auto pit = std::find(entry.pending.begin(), entry.pending.end(), nick);
    if (pit != entry.pending.end())
        entry.pending.erase(pit);
    const auto mit = std::find(entry.members.begin(), entry.members.end(), nick);
    if (mit != entry.members.end()) {
        entry.members.erase(mit);
        player_clan.erase(nick);
    }
    entry.banned.insert(nick);

    save();
    return {"Baneaste a " + nick + " del clan.", nick,
            "Fuiste baneado del clan \"" + clan_name + "\"."};
}

ClanOpResult ClanManager::kickMember(const std::string& founder, const std::string& nick) {
    if (!player_clan.contains(founder))
        return {"No perteneces a ningun clan.", "", ""};

    const std::string& clan_name = player_clan.at(founder);
    ClanEntry& entry = clans.at(clan_name);

    if (entry.founder != founder)
        return {"Solo el fundador puede expulsar miembros.", "", ""};
    if (nick == founder)
        return {"No puedes expulsarte a ti mismo.", "", ""};

    const auto it = std::find(entry.members.begin(), entry.members.end(), nick);
    if (it == entry.members.end())
        return {nick + " no es miembro del clan.", "", ""};

    entry.members.erase(it);
    player_clan.erase(nick);

    save();
    return {"Expulsaste a " + nick + " del clan.", nick,
            "Fuiste expulsado del clan \"" + clan_name + "\"."};
}

ClanOpResult ClanManager::leaveClan(const std::string& username) {
    if (!player_clan.contains(username))
        return {"No perteneces a ningun clan.", "", ""};

    const std::string& clan_name = player_clan.at(username);
    const ClanEntry& entry = clans.at(clan_name);

    if (entry.founder == username)
        return {"El fundador no puede abandonar el clan.", "", ""};
    ClanEntry& editable = clans.at(clan_name);
    const auto it = std::find(editable.members.begin(), editable.members.end(), username);

    if (it != editable.members.end())
        editable.members.erase(it);
    player_clan.erase(username);

    save();
    return {"Abandonaste el clan \"" + clan_name + "\".", "", ""};
}

bool ClanManager::areInSameClan(const std::string& a, const std::string& b) const {
    const auto ia = player_clan.find(a);
    if (ia == player_clan.end())
        return false;
    const auto ib = player_clan.find(b);
    if (ib == player_clan.end())
        return false;
    return ia->second == ib->second;
}

std::string ClanManager::getClanOf(const std::string& username) const {
    const auto it = player_clan.find(username);
    if (it == player_clan.end())
        return "";
    return it->second;
}

std::vector<std::string> ClanManager::getMembersOf(const std::string& clan_name) const {
    const auto it = clans.find(clan_name);
    if (it == clans.end())
        return {};
    return it->second.members;
}

uint8_t ClanManager::countNearbyMembers(const std::string& username, const Position& pos,
                                        const std::map<std::string, Position>& online_positions,
                                        uint32_t range) const {
    const auto it = player_clan.find(username);
    if (it == player_clan.end())
        return 0;
    const std::string& clan_name = it->second;
    const auto cit = clans.find(clan_name);
    if (cit == clans.end())
        return 0;
    uint8_t count = 0;
    for (const auto& member: cit->second.members) {
        if (member == username)
            continue;
        const auto pit = online_positions.find(member);
        if (pit == online_positions.end())
            continue;
        const uint32_t dx =
                (pos.x > pit->second.x) ? (pos.x - pit->second.x) : (pit->second.x - pos.x);
        const uint32_t dy =
                (pos.y > pit->second.y) ? (pos.y - pit->second.y) : (pit->second.y - pos.y);
        if (dx + dy <= range)
            ++count;
    }
    return count;
}

std::vector<std::string> ClanManager::listClans() const {
    std::vector<std::string> result;
    for (const auto& [name, entry]: clans) {
        result.push_back(name + " (fundador: " + entry.founder +
                         ", miembros: " + std::to_string(entry.members.size()) + ")");
    }
    return result;
}
