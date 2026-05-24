#include "map_scene.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsSceneEvent>

#include "common/includes/map/map.h"

#include "sprite.h"

MapScene::MapScene(QObject* parent): QGraphicsScene(parent) {}

void MapScene::loadMap(Map* map, Sprite* sprite) {
    map_ = map;
    sprite_ = sprite;
    rebuildScene();
}

void MapScene::setCurrentLayer(Layer layer) {
    current_layer_ = layer;
    if (map_)
        rebuildScene();
}

void MapScene::setCurrentSpriteId(int sprite_id) { current_sprite_id_ = sprite_id; }

void MapScene::setCurrentWalkable(bool walkable) { current_walkable_ = walkable; }

void MapScene::rebuildScene() {
    clear();
    visuals_.clear();
    if (!map_)
        return;

    int w = map_->width();
    int h = map_->height();
    visuals_.resize(static_cast<size_t>(w * h));

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int idx = tileIndex(x, y);
            qreal rx = x * kTileSize;
            qreal ry = y * kTileSize;

            auto* bg = addRect(rx, ry, kTileSize, kTileSize, QPen(QColor(80, 80, 80)),
                               QBrush(QColor(40, 40, 40)));
            bg->setZValue(0);
            bg->setAcceptedMouseButtons(Qt::NoButton);
            visuals_[idx].bg = bg;

            auto* pix = new QGraphicsPixmapItem();
            pix->setZValue(1);
            pix->setAcceptedMouseButtons(Qt::NoButton);
            addItem(pix);
            visuals_[idx].pix = pix;

            auto* overlay = addRect(rx, ry, kTileSize, kTileSize, QPen(Qt::NoPen),
                                    QBrush(QColor(255, 0, 0, 80)));
            overlay->setZValue(2);
            overlay->setAcceptedMouseButtons(Qt::NoButton);
            overlay->setVisible(false);
            visuals_[idx].overlay = overlay;

            updateTileVisual(x, y);
        }
    }

    setSceneRect(0, 0, w * kTileSize, h * kTileSize);
}

void MapScene::updateTileVisual(int x, int y) {
    int idx = tileIndex(x, y);
    const Tile& tile = map_->tile_at(x, y, current_layer_);

    if (tile.sprite_id != 0 && sprite_) {
        QPixmap pm = sprite_->get(tile.sprite_id);
        visuals_[idx].pix->setPixmap(pm);
        qreal px_x = x * kTileSize + (kTileSize - pm.width()) / 2.0;
        qreal px_y = y * kTileSize + (kTileSize - pm.height()) / 2.0;
        visuals_[idx].pix->setPos(px_x, px_y);
    } else {
        visuals_[idx].pix->setPixmap(QPixmap());
    }

    visuals_[idx].overlay->setVisible(!tile.walkable);
}

void MapScene::paintAt(QPointF scene_pos) {
    if (!map_)
        return;
    int x = static_cast<int>(scene_pos.x()) / kTileSize;
    int y = static_cast<int>(scene_pos.y()) / kTileSize;
    if (x < 0 || y < 0 || x >= map_->width() || y >= map_->height())
        return;

    Tile& tile = map_->tile_at(x, y, current_layer_);
    tile.sprite_id = current_sprite_id_;
    tile.walkable = current_walkable_;
    updateTileVisual(x, y);
    emit tileModified(x, y);
}

void MapScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        painting_ = true;
        paintAt(event->scenePos());
        event->accept();
    }
}

void MapScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (painting_) {
        paintAt(event->scenePos());
        event->accept();
    }
}

void MapScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        painting_ = false;
        event->accept();
    }
}

int MapScene::tileIndex(int x, int y) const { return y * map_->width() + x; }
