#pragma once

#include <QMainWindow>
#include <memory>

#include "common/includes/map/layer.h"
#include "common/includes/map/map.h"

class MapScene;
class Sprite;
class TileWidget;
class QComboBox;
class QCheckBox;
class QGraphicsView;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow: public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    // private slots:
private:
    void onNew();
    void onOpen();
    void onSave();
    void onSaveAs();
    void onZoomIn();
    void onZoomOut();
    void onSelectGraficosDir();
    void setupEditor();
    void setupToolBar();
    void loadMapIntoScene();
    void updateTitle();
    void tryDefaultGraficosDir();

    Ui::MainWindow* ui_;
    std::unique_ptr<Map> map_;
    QString file_path_;
    Sprite* sprite_;
    MapScene* scene_;
    QGraphicsView* view_;
    TileWidget* tile_widget_;
    QComboBox* layer_combo_;
    QCheckBox* walkable_check_;
};
