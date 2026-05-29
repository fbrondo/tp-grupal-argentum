#include "common/includes/map/map.h"

#include <fstream>

#include "common/includes/map/map_serializer.h"
#include "gtest/gtest.h"

namespace {

// --- Constructor Tests ---

TEST(MapConstructorTest, Valid) {
    Map map("test", 10, 5);
    EXPECT_EQ(map.name(), "test");
    EXPECT_EQ(map.width(), 10);
    EXPECT_EQ(map.height(), 5);
}

TEST(MapConstructorTest, ZeroWidthTest) { EXPECT_THROW(Map("test", 0, 5), std::invalid_argument); }

TEST(MapConstructorTest, ZeroHeightTest) {
    EXPECT_THROW(Map("test", 10, 0), std::invalid_argument);
}

TEST(MapConstructorTest, NegativeDimensionsTest) {
    EXPECT_THROW(Map("test", -1, 5), std::invalid_argument);
    EXPECT_THROW(Map("test", 10, -1), std::invalid_argument);
}

TEST(MapConstructorTest, TooLargeTest) {
    EXPECT_THROW(Map("test", MAX_MAP_WIDTH + 1, 5), std::invalid_argument);
    EXPECT_THROW(Map("test", 5, MAX_MAP_HEIGHT + 1), std::invalid_argument);
}

// --- Tiles and layers ---

TEST(MapLayerTest, LayerTilesHasCorrectSize) {
    Map map("test", 4, 3);
    EXPECT_EQ(map.layer_tiles(Layer::Background).size(), 12u);
    EXPECT_EQ(map.layer_tiles(Layer::Roof).size(), 12u);
}

TEST(MapLayerTest, TileAtDefaultValues) {
    Map map("test", 3, 3);
    const Tile& t = map.tile_at(1, 1, Layer::Background);
    EXPECT_EQ(t.sprite_id, 0);
    EXPECT_TRUE(t.walkable);
}

TEST(MapLayerTest, TileAtModify) {
    Map map("test", 3, 3);
    map.tile_at(1, 2, Layer::Object).sprite_id = 7;
    map.tile_at(1, 2, Layer::Object).walkable = false;

    EXPECT_EQ(map.tile_at(1, 2, Layer::Object).sprite_id, 7);
    EXPECT_FALSE(map.tile_at(1, 2, Layer::Object).walkable);
}

}  // namespace
