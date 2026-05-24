#pragma once
#include <QGraphicsScene>
#include <vector>

#include "common/includes/map/layer.h"

class Map;
class Sprite;
class QGraphicsSceneMouseEvent;
class QGraphicsPixmapItem;
class QGraphicsRectItem;

class MapScene: public QGraphicsScene {
    Q_OBJECT
public:
    static constexpr int kTileSize = 64;

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
    void updateTileVisual(int x, int y);
    void paintAt(QPointF scene_pos);
    int tileIndex(int x, int y) const;

    Map* map_{nullptr};
    Sprite* sprite_{nullptr};
    Layer current_layer_{Layer::Background};
    int current_sprite_id_{0};
    bool current_walkable_{true};
    bool painting_{false};

    struct TileVisual {
        QGraphicsRectItem* bg{nullptr};
        QGraphicsPixmapItem* pix{nullptr};
        QGraphicsRectItem* overlay{nullptr};
    };
    std::vector<TileVisual> visuals_;
};
