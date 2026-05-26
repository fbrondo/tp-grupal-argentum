#include "common/includes/sprite_loader.h"

#include <filesystem>

#include "gtest/gtest.h"

namespace {

// Paths are resolved relative to this source file so they work regardless
// of the working directory when the test binary is executed.
const std::filesystem::path test_dir = std::filesystem::path(__FILE__).parent_path();
const std::filesystem::path sprites_test_path = test_dir / "sprites_test.toml";
const std::filesystem::path sprites_mis_test_path = test_dir / "sprites_mismatch_test.toml";

// --- Load valid file ---

TEST(SpriteLoaderTest, LoadsCorrectCount) {
    auto sprites = SpriteLoader::load(sprites_test_path);
    EXPECT_EQ(sprites.size(), 2u);
}

TEST(SpriteLoaderTest, Sprite1Fields) {
    auto sprites = SpriteLoader::load(sprites_test_path);
    ASSERT_EQ(sprites.count(1), 1u);
    const auto& s = sprites.at(1);
    EXPECT_EQ(s.png_id, 41);
    EXPECT_EQ(s.x, 0);
    EXPECT_EQ(s.y, 0);
    EXPECT_EQ(s.width, 32);
    EXPECT_EQ(s.height, 32);
}

TEST(SpriteLoaderTest, Sprite2Fields) {
    auto sprites = SpriteLoader::load(sprites_test_path);
    ASSERT_EQ(sprites.count(2), 1u);
    const auto& s = sprites.at(2);
    EXPECT_EQ(s.png_id, 42);
    EXPECT_EQ(s.x, 16);
    EXPECT_EQ(s.y, 0);
    EXPECT_EQ(s.width, 16);
    EXPECT_EQ(s.height, 32);
}

TEST(SpriteLoaderTest, MissingKeyReturnsEmptyMap) {
    auto sprites = SpriteLoader::load(sprites_test_path);
    EXPECT_EQ(sprites.count(99), 0u);
}

// --- Error cases ---

TEST(SpriteLoaderTest, MismatchedArraysThrows) {
    EXPECT_THROW(SpriteLoader::load(sprites_mis_test_path), std::runtime_error);
}

TEST(SpriteLoaderTest, MissingFileThrows) {
    EXPECT_THROW(SpriteLoader::load("nonexistent.toml"), std::runtime_error);
}

}  // namespace
