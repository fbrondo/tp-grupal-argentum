#pragma once
#include <QHash>
#include <QPixmap>

class Sprite {
public:
    static constexpr int kTileSize = 64;

    void setGraphicsDir(const QString& path);
    const QString& graficosDir() const;
    QPixmap get(int sprite_id);

private:
    QString graficos_dir_;
    QHash<int, QPixmap> cache_;
};
