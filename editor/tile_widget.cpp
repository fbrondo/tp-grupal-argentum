#include "tile_widget.h"

#include <QDir>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QListView>
#include <QVBoxLayout>

TileWidget::TileWidget(QWidget* parent): QWidget(parent) {
    model_ = new QFileSystemModel(this);
    model_->setFilter(QDir::Files);
    model_->setNameFilters({"*.png", "*.PNG"});
    model_->setNameFilterDisables(false);

    list_view_ = new QListView(this);
    list_view_->setModel(model_);
    list_view_->setViewMode(QListView::IconMode);
    list_view_->setIconSize(QSize(64, 64));
    list_view_->setResizeMode(QListView::Adjust);
    list_view_->setSpacing(4);
    list_view_->setMovement(QListView::Static);

    connect(list_view_, &QListView::clicked, this, [this](const QModelIndex& index) {
        QString filename = model_->fileName(index);
        bool ok;
        int id = QFileInfo(filename).completeBaseName().toInt(&ok);
        if (ok)
            emit spriteSelected(id);
    });

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(list_view_);
}

void TileWidget::setGraphicsDir(const QString& path) {
    model_->setRootPath(path);
    list_view_->setRootIndex(model_->index(path));
}
