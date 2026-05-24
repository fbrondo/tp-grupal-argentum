#pragma once
#include <QWidget>
#include <vector>

class Sprite;
class QListWidget;

class TileWidget: public QWidget {
    Q_OBJECT
public:
    explicit TileWidget(QWidget* parent = nullptr);

    // Repopulates the palette from the sprite definitions.
    void setSprite(Sprite* sprite);

signals:
    void spriteSelected(int sprite_id);

private:
    void populate();
    void loadNextBatch();

    QListWidget* list_widget_;
    Sprite* sprite_{nullptr};
    std::vector<int> pending_ids_;
    int batch_idx_{0};
};
