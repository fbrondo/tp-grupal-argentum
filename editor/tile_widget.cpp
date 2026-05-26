#include "tile_widget.h"

#include <QIcon>
#include <QListWidget>
#include <QTabWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <algorithm>

#include "sprite.h"

static constexpr std::array<const char*, layer_count> LAYER_TAB_NAMES = {"Background", "Details",
                                                                         "Object", "Roof"};

TileWidget::TileWidget(QWidget* parent): QWidget(parent) {
    tab_widget_ = new QTabWidget(this);
    tab_widget_->setDocumentMode(true);
    batch_idx_.fill(0);

    for (int li = 0; li < static_cast<int>(layer_count); ++li) {
        auto* lw = new QListWidget(this);
        lw->setViewMode(QListView::IconMode);
        lw->setIconSize(QSize(64, 64));
        lw->setGridSize(QSize(76, 92));
        lw->setResizeMode(QListView::Adjust);
        lw->setSpacing(4);
        lw->setMovement(QListView::Static);
        lw->setWordWrap(true);
        list_widgets_[li] = lw;
        tab_widget_->addTab(lw, LAYER_TAB_NAMES[li]);

        connect(lw, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
            emit spriteSelected(item->data(Qt::UserRole).toInt());
        });
    }

    connect(tab_widget_, &QTabWidget::currentChanged, this,
            [this](int idx) { emit layerChangeRequested(static_cast<Layer>(idx)); });

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(tab_widget_);
}

void TileWidget::setLayerSprite(Layer layer, Sprite* sprite) {
    int li = static_cast<int>(layer);
    sprites_[li] = sprite;
    populateLayer(li);
}

void TileWidget::setCurrentLayer(Layer layer) {
    tab_widget_->blockSignals(true);
    tab_widget_->setCurrentIndex(static_cast<int>(layer));
    tab_widget_->blockSignals(false);
}

void TileWidget::populateLayer(int li) {
    list_widgets_[li]->clear();
    pending_ids_[li].clear();
    batch_idx_[li] = 0;

    const Sprite* sprite = sprites_[li];
    if (!sprite || !sprite->hasDefinitions())
        return;

    for (const auto& [id, def]: sprite->definitions()) pending_ids_[li].push_back(id);
    std::sort(pending_ids_[li].begin(), pending_ids_[li].end());

    // Add all items with text labels first (fast), then load icons lazily
    for (int id: pending_ids_[li]) {
        auto* item = new QListWidgetItem(QString::number(id));
        item->setData(Qt::UserRole, id);
        item->setSizeHint(QSize(76, 92));
        list_widgets_[li]->addItem(item);
    }

    QTimer::singleShot(0, this, [this, li]() { loadNextBatch(li); });
}

void TileWidget::loadNextBatch(int li) {
    constexpr int PALETTE_ICON_BATCH_SIZE = 50;
    const int end = std::min(batch_idx_[li] + PALETTE_ICON_BATCH_SIZE,
                             static_cast<int>(pending_ids_[li].size()));

    for (int i = batch_idx_[li]; i < end; ++i) {
        QPixmap pm = sprites_[li]->getPaletteIcon(pending_ids_[li][i]);
        if (!pm.isNull()) {
            QListWidgetItem* item = list_widgets_[li]->item(i);
            if (item)
                item->setIcon(QIcon(pm));
        }
    }

    batch_idx_[li] = end;
    if (batch_idx_[li] < static_cast<int>(pending_ids_[li].size()))
        QTimer::singleShot(0, this, [this, li]() { loadNextBatch(li); });
}
