#include "sprite.h"

#include "common/includes/sprite_loader.h"

void Sprite::setGraphicsDir(const QString& path) {
    graficos_dir_ = path;
    sprite_cache_.clear();
    png_cache_.clear();
}

void Sprite::setSpritesConfig(const QString& toml_path) {
    sprites_toml_ = toml_path;
    sprite_cache_.clear();
    reloadDefinitions();
}

const QString& Sprite::graphicsDir() const { return graficos_dir_; }

bool Sprite::hasDefinitions() const { return !defs_.empty(); }

const std::unordered_map<int, SpriteDefinition>& Sprite::definitions() const { return defs_; }

QPixmap Sprite::get(int sprite_id) {
    if (sprite_cache_.contains(sprite_id))
        return sprite_cache_[sprite_id];

    QPixmap result;
    auto it = defs_.find(sprite_id);
    if (it != defs_.end())
        result = cropSprite(it->second);

    if (!result.isNull())
        result = result.scaled(kTileSize, kTileSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    sprite_cache_.insert(sprite_id, result);
    return result;
}

void Sprite::reloadDefinitions() {
    defs_.clear();
    if (sprites_toml_.isEmpty())
        return;
    try {
        defs_ = SpriteLoader::load(sprites_toml_.toStdString());
    } catch (...) {
        // Leave defs_ empty; UI will show blank tiles
    }
}

QPixmap Sprite::cropSprite(const SpriteDefinition& def) {
    if (!png_cache_.contains(def.png_id)) {
        QPixmap full;
        QString path = graficos_dir_ + "/" + QString::number(def.png_id) + ".png";
        if (!full.load(path)) {
            path = graficos_dir_ + "/" + QString::number(def.png_id) + ".PNG";
            full.load(path);
        }
        png_cache_.insert(def.png_id, full);
    }

    const QPixmap& full = png_cache_[def.png_id];
    if (full.isNull())
        return {};

    return full.copy(def.x, def.y, def.width, def.height);
}
