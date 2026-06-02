#include "map_scene.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsSceneEvent>
#include <algorithm>

#include "common/includes/map/map.h"

#include "sprite.h"

MapScene::MapScene(QObject* parent): QGraphicsScene(parent) {}

void MapScene::loadMap(Map* map, std::array<Sprite*, layer_count> sprites) {
    map_ = map;
    sprites_ = sprites;
    undo_stack_.clear();
    redo_stack_.clear();
    emit undoAvailable(false);
    emit redoAvailable(false);
    rebuildScene();
}

void MapScene::setCurrentLayer(Layer layer) {
    current_layer_ = layer;
    if (map_)
        rebuildScene();
}

void MapScene::setCurrentSpriteId(int sprite_id) { current_sprite_id_ = sprite_id; }

void MapScene::setCurrentWalkable(bool walkable) { current_walkable_ = walkable; }

void MapScene::setCurrentRegion(Region region) { current_region_ = region; }

void MapScene::setCurrentTool(Tool tool) { current_tool_ = tool; }

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

            for (int li = 0; li < static_cast<int>(layer_count); ++li) {
                auto* pix = new QGraphicsPixmapItem();
                pix->setZValue((li + 1) * 10);
                pix->setAcceptedMouseButtons(Qt::NoButton);
                addItem(pix);
                visuals_[idx].layer_pix[li] = pix;
            }

            auto* overlay = addRect(rx, ry, BASE_TILE_PX, BASE_TILE_PX, QPen(Qt::NoPen),
                                    QBrush(QColor(255, 0, 0, 80)));
            overlay->setZValue((layer_count + 1) * 10);
            overlay->setAcceptedMouseButtons(Qt::NoButton);
            overlay->setVisible(false);
            visuals_[idx].walkable_overlay = overlay;

            updateAllLayersVisual(x, y);
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

void MapScene::updateAllLayersVisual(int x, int y) {
    int idx = tileIndex(x, y);
    int current_idx = static_cast<int>(current_layer_);

    for (int li = 0; li < static_cast<int>(layer_count); ++li) {
        auto* pix = visuals_[idx].layer_pix[li];
        Sprite* layer_sprite = sprites_[li];
        const Tile& tile = map_->tile_at(x, y, static_cast<Layer>(li));

        if (tile.sprite_id != 0 && layer_sprite) {
            pix->setPixmap(layer_sprite->get(tile.sprite_id));
            pix->setPos(x * BASE_TILE_PX, y * BASE_TILE_PX);
        } else {
            pix->setPixmap(QPixmap());
        }

        if (li < current_idx) {
            pix->setVisible(true);
            pix->setOpacity(0.4);
        } else if (li == current_idx) {
            pix->setVisible(true);
            pix->setOpacity(1.0);
        } else {
            pix->setVisible(false);
        }
    }

    visuals_[idx].walkable_overlay->setVisible(!map_->tile_at(x, y, current_layer_).walkable);
}

void MapScene::updateTileVisual(int x, int y) {
    int idx = tileIndex(x, y);
    int current_idx = static_cast<int>(current_layer_);
    Sprite* layer_sprite = sprites_[current_idx];
    const Tile& tile = map_->tile_at(x, y, current_layer_);

    auto* pix = visuals_[idx].layer_pix[current_idx];
    if (tile.sprite_id != 0 && layer_sprite) {
        pix->setPixmap(layer_sprite->get(tile.sprite_id));
        pix->setPos(x * BASE_TILE_PX, y * BASE_TILE_PX);
    } else {
        pix->setPixmap(QPixmap());
    }

    visuals_[idx].walkable_overlay->setVisible(!tile.walkable);
}

std::pair<int, int> MapScene::spriteCellSize(int sprite_id) const {
    int current_idx = static_cast<int>(current_layer_);
    Sprite* layer_sprite = sprites_[current_idx];
    if (!layer_sprite || sprite_id == 0)
        return {1, 1};
    const auto& defs = layer_sprite->definitions();
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

void MapScene::applyTileChange(int ax, int ay, Layer layer, const Tile& old_tile,
                               const Tile& new_tile) {
    if (layer == current_layer_) {
        if (old_tile.sprite_id != 0) {
            for (auto [nx, ny]: occupiedCells(ax, ay, old_tile.sprite_id))
                anchor_at_[tileIndex(nx, ny)] = -1;
        }
    }

    map_->tile_at(ax, ay, layer) = new_tile;

    if (layer == current_layer_) {
        if (new_tile.sprite_id != 0) {
            int anchor_idx = tileIndex(ax, ay);
            for (auto [nx, ny]: occupiedCells(ax, ay, new_tile.sprite_id))
                anchor_at_[tileIndex(nx, ny)] = anchor_idx;
        }
    }

    updateAllLayersVisual(ax, ay);
    emit tileModified(ax, ay);
}

void MapScene::paintAt(QPointF scene_pos) {
    if (!map_ || current_sprite_id_ == 0)
        return;
    int x = static_cast<int>(scene_pos.x()) / BASE_TILE_PX;
    int y = static_cast<int>(scene_pos.y()) / BASE_TILE_PX;
    if (x < 0 || y < 0 || x >= map_->width() || y >= map_->height())
        return;

    // Block paint if any cell in the sprite's footprint is already occupied.
    for (auto [nx, ny]: occupiedCells(x, y, current_sprite_id_)) {
        if (anchor_at_[tileIndex(nx, ny)] != -1)
            return;
    }

    Tile old_tile = map_->tile_at(x, y, current_layer_);
    Tile new_tile{current_sprite_id_, current_walkable_, current_region_};

    redo_stack_.clear();
    undo_stack_.push_back({current_layer_, x, y, old_tile, new_tile});
    emit undoAvailable(true);
    emit redoAvailable(false);

    applyTileChange(x, y, current_layer_, old_tile, new_tile);
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

    Tile old_tile = map_->tile_at(ax, ay, current_layer_);
    Tile new_tile{0, true};

    redo_stack_.clear();
    undo_stack_.push_back({current_layer_, ax, ay, old_tile, new_tile});
    emit undoAvailable(true);
    emit redoAvailable(false);

    applyTileChange(ax, ay, current_layer_, old_tile, new_tile);
}

void MapScene::undo() {
    if (undo_stack_.empty())
        return;
    TileChange change = undo_stack_.back();
    undo_stack_.pop_back();
    redo_stack_.push_back(change);

    applyTileChange(change.ax, change.ay, change.layer, change.new_tile, change.old_tile);

    emit undoAvailable(!undo_stack_.empty());
    emit redoAvailable(true);
}

void MapScene::redo() {
    if (redo_stack_.empty())
        return;
    TileChange change = redo_stack_.back();
    redo_stack_.pop_back();
    undo_stack_.push_back(change);

    applyTileChange(change.ax, change.ay, change.layer, change.old_tile, change.new_tile);

    emit undoAvailable(true);
    emit redoAvailable(!redo_stack_.empty());
}

void MapScene::selectAt(QPointF scene_pos) {
    if (!map_)
        return;
    int x = static_cast<int>(scene_pos.x()) / BASE_TILE_PX;
    int y = static_cast<int>(scene_pos.y()) / BASE_TILE_PX;
    if (x < 0 || y < 0 || x >= map_->width() || y >= map_->height())
        return;

    int anchor_idx = anchor_at_[tileIndex(x, y)];
    if (anchor_idx != -1) {
        int ax = anchor_idx % map_->width();
        int ay = anchor_idx / map_->width();
        emit tileSelected(map_->tile_at(ax, ay, current_layer_));
    } else {
        emit tileSelected(map_->tile_at(x, y, current_layer_));
    }
}

void MapScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (current_tool_ == Tool::Select) {
        if (event->button() == Qt::LeftButton) {
            selectAt(event->scenePos());
            event->accept();
        }
        return;
    }
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
    if (current_tool_ == Tool::Select) {
        if (event->buttons() & Qt::LeftButton) {
            selectAt(event->scenePos());
            event->accept();
        }
        return;
    }
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
