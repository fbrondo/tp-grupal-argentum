#include "sprite.h"

#include <QDir>
#include <QFileInfo>

void Sprite::setGraphicsDir(const QString& path) {
    graficos_dir_ = path;
    cache_.clear();
}

const QString& Sprite::graficosDir() const { return graficos_dir_; }

QPixmap Sprite::get(int sprite_id) {
    if (cache_.contains(sprite_id))
        return cache_[sprite_id];

    QPixmap pixmap;
    QString path = graficos_dir_ + "/" + QString::number(sprite_id) + ".png";
    if (!pixmap.load(path)) {
        path = graficos_dir_ + "/" + QString::number(sprite_id) + ".PNG";
        pixmap.load(path);
    }

    if (!pixmap.isNull())
        pixmap = pixmap.scaled(kTileSize, kTileSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    cache_.insert(sprite_id, pixmap);
    return pixmap;
}
