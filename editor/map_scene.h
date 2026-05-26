#pragma once
#include <QGraphicsScene>
#include <array>
#include <utility>
#include <vector>

#include "common/includes/map/layer.h"

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

    void loadMap(Map* map, Sprite* sprite);
    void setCurrentLayer(Layer layer);
    void setCurrentSpriteId(int sprite_id);
    void setCurrentWalkable(bool walkable);

signals:
    void tileModified(int x, int y);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    void rebuildScene();
    void updateAllLayersVisual(int x, int y);
    void updateTileVisual(int x, int y);
    void paintAt(QPointF scene_pos);
    void eraseAt(QPointF scene_pos);
    // Returns (cells_wide, cells_tall) that sprite_id occupies based on pixel dimensions.
    std::pair<int, int> spriteCellSize(int sprite_id) const;
    // Returns all (x,y) cells in the footprint of sprite anchored at (ax, ay).
    std::vector<std::pair<int, int>> occupiedCells(int ax, int ay, int sprite_id) const;
    int tileIndex(int x, int y) const;

    Map* map_{nullptr};
    Sprite* sprite_{nullptr};
    Layer current_layer_{Layer::Background};
    int current_sprite_id_{0};
    bool current_walkable_{true};
    bool painting_{false};
    bool erasing_{false};

    struct TileVisual {
        QGraphicsRectItem* bg{nullptr};
        std::array<QGraphicsPixmapItem*, layer_count> layer_pix{};
        QGraphicsRectItem* walkable_overlay{nullptr};
    };
    std::vector<TileVisual> visuals_;
    std::vector<int> anchor_at_;
};
