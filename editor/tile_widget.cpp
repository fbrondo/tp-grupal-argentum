#include "tile_widget.h"

#include <QIcon>
#include <QListWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <algorithm>

#include "sprite.h"

TileWidget::TileWidget(QWidget* parent): QWidget(parent) {
    list_widget_ = new QListWidget(this);
    list_widget_->setViewMode(QListView::IconMode);
    list_widget_->setIconSize(QSize(64, 64));
    list_widget_->setResizeMode(QListView::Adjust);
    list_widget_->setSpacing(4);
    list_widget_->setMovement(QListView::Static);
    list_widget_->setWordWrap(true);

    connect(list_widget_, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
        emit spriteSelected(item->data(Qt::UserRole).toInt());
    });

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(list_widget_);
}

void TileWidget::setSprite(Sprite* sprite) {
    sprite_ = sprite;
    populate();
}

void TileWidget::populate() {
    list_widget_->clear();
    pending_ids_.clear();
    batch_idx_ = 0;

    if (!sprite_ || !sprite_->hasDefinitions())
        return;

    for (const auto& [id, def]: sprite_->definitions()) pending_ids_.push_back(id);

    std::sort(pending_ids_.begin(), pending_ids_.end());

    // Add all items with text labels first (fast), then load icons lazily
    for (int id: pending_ids_) {
        auto* item = new QListWidgetItem(QString::number(id));
        item->setData(Qt::UserRole, id);
        list_widget_->addItem(item);
    }

    QTimer::singleShot(0, this, &TileWidget::loadNextBatch);
}

void TileWidget::loadNextBatch() {
    constexpr int PALETTE_ICON_BATCH_SIZE = 50;
    const int end =
            std::min(batch_idx_ + PALETTE_ICON_BATCH_SIZE, static_cast<int>(pending_ids_.size()));

    for (int i = batch_idx_; i < end; ++i) {
        QPixmap pm = sprite_->getPaletteIcon(pending_ids_[i]);
        if (!pm.isNull()) {
            QListWidgetItem* item = list_widget_->item(i);
            if (item)
                item->setIcon(QIcon(pm));
        }
    }

    batch_idx_ = end;
    if (batch_idx_ < static_cast<int>(pending_ids_.size()))
        QTimer::singleShot(0, this, &TileWidget::loadNextBatch);
}
