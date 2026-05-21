#include "common/includes/toml_config.h"

#include "gtest/gtest.h"

namespace {

class TomlConfigTest: public ::testing::Test {
protected:
    TomlConfig config{TOML_TEST_CONFIG_PATH};
};

// --- [server] ---

TEST_F(TomlConfigTest, ServerPort) {
    auto val = config.get<int64_t>("server.port");
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, 8080);
}

TEST_F(TomlConfigTest, ServerMaxPlayers) {
    auto val = config.get<int64_t>("server.max_players");
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, 100);
}

TEST_F(TomlConfigTest, ServerTickRate) {
    auto val = config.get<int64_t>("server.tick_rate_ms");
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, 50);
}

// --- [game] ---

TEST_F(TomlConfigTest, GameInventorySize) {
    auto val = config.get<int64_t>("game.inventory_size");
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, 20);
}

TEST_F(TomlConfigTest, GameClanMaxMembers) {
    auto val = config.get<int64_t>("game.clan_max_members");
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, 16);
}

// --- [balance] ---

TEST_F(TomlConfigTest, BalanceExpLimitBase) {
    auto val = config.get<int64_t>("balance.exp_limit_base");
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, 1000);
}

TEST_F(TomlConfigTest, BalanceOroMaxBase) {
    auto val = config.get<int64_t>("balance.gold_max_base");
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, 100);
}

TEST_F(TomlConfigTest, BalanceOroExcessFactor) {
    auto val = config.get<double>("balance.gold_excess_factor");
    ASSERT_TRUE(val.has_value());
    EXPECT_DOUBLE_EQ(*val, 0.5);
}

// --- [npc] ---

TEST_F(TomlConfigTest, NpcSpawnInterval) {
    auto val = config.get<int64_t>("npc.spawn_interval_s");
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, 30);
}

TEST_F(TomlConfigTest, NpcMaxPopulation) {
    auto val = config.get<int64_t>("npc.max_population");
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, 200);
}

// --- comportamiento ante claves ausentes/erróneas ---

TEST_F(TomlConfigTest, MissingKeyReturnsNullopt) {
    EXPECT_EQ(config.get<int64_t>("server.nonexistent"), std::nullopt);
}

TEST_F(TomlConfigTest, TypeMismatchReturnsNullopt) {
    // port is an integer; requesting it as string must return nullopt
    EXPECT_EQ(config.get<std::string>("server.port"), std::nullopt);
}

TEST_F(TomlConfigTest, HasReturnsTrueForExistingKey) { EXPECT_TRUE(config.has("server.port")); }

TEST_F(TomlConfigTest, HasReturnsFalseForMissingKey) {
    EXPECT_FALSE(config.has("server.nonexistent"));
}

TEST_F(TomlConfigTest, GetOrReturnsDefaultValueForMissingKey) {
    EXPECT_EQ(config.get_or<int64_t>("server.missing", 9999), 9999);
}

TEST(TomlConfigErrorTest, ThrowsOnMissingFile) {
    EXPECT_THROW(TomlConfig{"/nonexistent/path/argentum.toml"}, toml::parse_error);
}

}  // namespace
