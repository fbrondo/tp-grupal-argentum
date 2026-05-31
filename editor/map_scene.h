#pragma once
#include <QGraphicsScene>
#include <array>
#include <utility>
#include <vector>

#include "common/includes/map/layer.h"
#include "common/includes/map/tile.h"

class Map;
class Sprite;
class QGraphicsSceneMouseEvent;
class QGraphicsPixmapItem;
class QGraphicsRectItem;

constexpr int BASE_TILE_PX = 32;
class MapScene: public QGraphicsScene {
    Q_OBJECT
public:
    explicit MapScene(QObject* parent = nullptr);

    void loadMap(Map* map, std::array<Sprite*, layer_count> sprites);
    void setCurrentLayer(Layer layer);
    void setCurrentSpriteId(int sprite_id);
    void setCurrentWalkable(bool walkable);
    void setCurrentRegion(Region region);
    void undo();
    void redo();

signals:
    void tileModified(int x, int y);
    void undoAvailable(bool available);
    void redoAvailable(bool available);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    struct TileChange {
        Layer layer;
        int ax;
        int ay;
        Tile old_tile;
        Tile new_tile;
    };

    void rebuildScene();
    void updateAllLayersVisual(int x, int y);
    void updateTileVisual(int x, int y);
    void paintAt(QPointF scene_pos);
    void eraseAt(QPointF scene_pos);
    void applyTileChange(int ax, int ay, Layer layer, const Tile& old_tile, const Tile& new_tile);
    // Returns (cells_wide, cells_tall) that sprite_id occupies based on pixel dimensions.
    std::pair<int, int> spriteCellSize(int sprite_id) const;
    // Returns all (x,y) cells in the footprint of sprite anchored at (ax, ay).
    std::vector<std::pair<int, int>> occupiedCells(int ax, int ay, int sprite_id) const;
    int tileIndex(int x, int y) const;

    Map* map_{nullptr};
    std::array<Sprite*, layer_count> sprites_{};
    Layer current_layer_{Layer::Background};
    int current_sprite_id_{0};
    bool current_walkable_{true};
    Region current_region_{Region::Field};
    bool painting_{false};
    bool erasing_{false};

    struct TileVisual {
        QGraphicsRectItem* bg{nullptr};
        std::array<QGraphicsPixmapItem*, layer_count> layer_pix{};
        QGraphicsRectItem* walkable_overlay{nullptr};
    };
    std::vector<TileVisual> visuals_;
    std::vector<int> anchor_at_;
    std::vector<TileChange> undo_stack_;
    std::vector<TileChange> redo_stack_;
};
