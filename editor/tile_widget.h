#pragma once
#include <QWidget>
#include <array>
#include <vector>

#include "common/includes/map/layer.h"

class Sprite;
class QListWidget;
class QListWidgetItem;
class QTabWidget;

class TileWidget: public QWidget {
    Q_OBJECT
public:
    explicit TileWidget(QWidget* parent = nullptr);

    // Load sprites for a specific layer into its palette tab.
    void setLayerSprite(Layer layer, Sprite* sprite);
    // Sync the active tab to match the given layer (does not emit layerChangeRequested).
    void setCurrentLayer(Layer layer);

signals:
    void spriteSelected(int sprite_id);
    void layerChangeRequested(Layer layer);

private:
    void populateLayer(int layer_idx);
    void loadNextBatch(int layer_idx);

    QTabWidget* tab_widget_;
    std::array<QListWidget*, layer_count> list_widgets_{};
    std::array<Sprite*, layer_count> sprites_{};
    std::array<std::vector<int>, layer_count> pending_ids_{};
    std::array<int, layer_count> batch_idx_{};
};
