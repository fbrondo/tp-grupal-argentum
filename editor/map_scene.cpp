#include "map_scene.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsSceneEvent>
#include <algorithm>

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
    anchor_at_.clear();
    if (!map_)
        return;

    int w = map_->width();
    int h = map_->height();
    visuals_.resize(static_cast<size_t>(w * h));
    anchor_at_.assign(static_cast<size_t>(w * h), -1);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int idx = tileIndex(x, y);
            qreal rx = x * BASE_TILE_PX;
            qreal ry = y * BASE_TILE_PX;

            auto* bg = addRect(rx, ry, BASE_TILE_PX, BASE_TILE_PX, QPen(QColor(80, 80, 80)),
                               QBrush(QColor(40, 40, 40)));
            bg->setZValue(0);
            bg->setAcceptedMouseButtons(Qt::NoButton);
            visuals_[idx].bg = bg;

            auto* pix = new QGraphicsPixmapItem();
            pix->setZValue(1);
            pix->setAcceptedMouseButtons(Qt::NoButton);
            addItem(pix);
            visuals_[idx].pix = pix;

            auto* overlay = addRect(rx, ry, BASE_TILE_PX, BASE_TILE_PX, QPen(Qt::NoPen),
                                    QBrush(QColor(255, 0, 0, 80)));
            overlay->setZValue(2);
            overlay->setAcceptedMouseButtons(Qt::NoButton);
            overlay->setVisible(false);
            visuals_[idx].overlay = overlay;

            updateTileVisual(x, y);
        }
    }

    // Populate anchor_at_ from the map data for the current layer.
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            const Tile& tile = map_->tile_at(x, y, current_layer_);
            if (tile.sprite_id != 0) {
                int anchor_idx = tileIndex(x, y);
                for (auto [nx, ny]: occupiedCells(x, y, tile.sprite_id)) {
                    int cell_idx = tileIndex(nx, ny);
                    if (anchor_at_[cell_idx] == -1)
                        anchor_at_[cell_idx] = anchor_idx;
                }
            }
        }
    }

    setSceneRect(0, 0, w * BASE_TILE_PX, h * BASE_TILE_PX);
}

void MapScene::updateTileVisual(int x, int y) {
    int idx = tileIndex(x, y);
    const Tile& tile = map_->tile_at(x, y, current_layer_);

    if (tile.sprite_id != 0 && sprite_) {
        QPixmap pm = sprite_->get(tile.sprite_id);
        visuals_[idx].pix->setPixmap(pm);
        visuals_[idx].pix->setPos(x * BASE_TILE_PX, y * BASE_TILE_PX);
    } else {
        visuals_[idx].pix->setPixmap(QPixmap());
    }

    visuals_[idx].overlay->setVisible(!tile.walkable);
}

std::pair<int, int> MapScene::spriteCellSize(int sprite_id) const {
    if (!sprite_ || sprite_id == 0)
        return {1, 1};
    const auto& defs = sprite_->definitions();
    auto it = defs.find(sprite_id);
    if (it == defs.end())
        return {1, 1};
    int cw = std::max(1, (it->second.width + BASE_TILE_PX - 1) / BASE_TILE_PX);
    int ch = std::max(1, (it->second.height + BASE_TILE_PX - 1) / BASE_TILE_PX);
    return {cw, ch};
}

std::vector<std::pair<int, int>> MapScene::occupiedCells(int ax, int ay, int sprite_id) const {
    auto [cw, ch] = spriteCellSize(sprite_id);
    std::vector<std::pair<int, int>> cells;
    cells.reserve(static_cast<size_t>(cw * ch));
    for (int dy = 0; dy < ch; ++dy) {
        for (int dx = 0; dx < cw; ++dx) {
            int nx = ax + dx;
            int ny = ay + dy;
            if (map_ && nx < map_->width() && ny < map_->height())
                cells.push_back({nx, ny});
        }
    }
    return cells;
}

void MapScene::paintAt(QPointF scene_pos) {
    if (!map_ || current_sprite_id_ == 0)
        return;
    int x = static_cast<int>(scene_pos.x()) / BASE_TILE_PX;
    int y = static_cast<int>(scene_pos.y()) / BASE_TILE_PX;
    if (x < 0 || y < 0 || x >= map_->width() || y >= map_->height())
        return;

    // Block paint if any cell in the sprite's footprint is already occupied.
    auto cells = occupiedCells(x, y, current_sprite_id_);
    for (auto [nx, ny]: cells) {
        if (anchor_at_[tileIndex(nx, ny)] != -1)
            return;
    }

    Tile& tile = map_->tile_at(x, y, current_layer_);
    tile.sprite_id = current_sprite_id_;
    tile.walkable = current_walkable_;

    int anchor_idx = tileIndex(x, y);
    for (auto [nx, ny]: cells) anchor_at_[tileIndex(nx, ny)] = anchor_idx;

    updateTileVisual(x, y);
    emit tileModified(x, y);
}

void MapScene::eraseAt(QPointF scene_pos) {
    if (!map_)
        return;
    int x = static_cast<int>(scene_pos.x()) / BASE_TILE_PX;
    int y = static_cast<int>(scene_pos.y()) / BASE_TILE_PX;
    if (x < 0 || y < 0 || x >= map_->width() || y >= map_->height())
        return;

    int anchor_idx = anchor_at_[tileIndex(x, y)];
    if (anchor_idx == -1)
        return;  // cell is free — nothing to erase

    int ax = anchor_idx % map_->width();
    int ay = anchor_idx / map_->width();

    Tile& anchor_tile = map_->tile_at(ax, ay, current_layer_);
    int sprite_id = anchor_tile.sprite_id;

    // Release all cells in the sprite's footprint before clearing the anchor.
    for (auto [nx, ny]: occupiedCells(ax, ay, sprite_id)) anchor_at_[tileIndex(nx, ny)] = -1;

    anchor_tile.sprite_id = 0;
    anchor_tile.walkable = true;
    updateTileVisual(ax, ay);
    emit tileModified(ax, ay);
}

void MapScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        painting_ = true;
        paintAt(event->scenePos());
        event->accept();
    } else if (event->button() == Qt::RightButton) {
        erasing_ = true;
        eraseAt(event->scenePos());
        event->accept();
    }
}

void MapScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (painting_) {
        paintAt(event->scenePos());
        event->accept();
    } else if (erasing_) {
        eraseAt(event->scenePos());
        event->accept();
    }
}

void MapScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        painting_ = false;
        event->accept();
    } else if (event->button() == Qt::RightButton) {
        erasing_ = false;
        event->accept();
    }
}

int MapScene::tileIndex(int x, int y) const { return y * map_->width() + x; }
