#pragma once
#include <QHash>
#include <QPixmap>
#include <unordered_map>

#include "common/includes/sprite_definition.h"

class Sprite {
public:
    // Fixed pixel size of every icon shown in the sprite palette.
    static constexpr int PALETTE_ICON_SIZE = 64;

    // Set directory containing Graficos/<png_id>.png files.
    void setGraphicsDir(const QString& path);
    // Load sprite definitions from a sprites.toml file.
    void setSpritesConfig(const QString& toml_path);

    const QString& graphicsDir() const;
    bool hasDefinitions() const;
    const std::unordered_map<int, SpriteDefinition>& definitions() const;

    // Returns the raw pixmap at source pixel dimensions (no scaling).
    // Use this for canvas rendering
    QPixmap get(int sprite_id);

    // Returns the sprite scaled to exactly PALETTE_ICON_SIZE x PALETTE_ICON_SIZE.
    // Use this for palette icons.
    QPixmap getPaletteIcon(int sprite_id);

private:
    void reloadDefinitions();
    QPixmap cropSprite(const SpriteDefinition& def);

    QString graficos_dir_;
    QString sprites_toml_;
    std::unordered_map<int, SpriteDefinition> defs_;
    QHash<int, QPixmap> sprite_cache_;
    QHash<int, QPixmap> palette_icon_cache_;
    QHash<int, QPixmap> png_cache_;
};
