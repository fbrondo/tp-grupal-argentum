#pragma once
#include <QWidget>

class QFileSystemModel;
class QListView;

class TileWidget: public QWidget {
    Q_OBJECT
public:
    explicit TileWidget(QWidget* parent = nullptr);

    void setGraphicsDir(const QString& path);

signals:
    void spriteSelected(int sprite_id);

private:
    QFileSystemModel* model_;
    QListView* list_view_;
};
