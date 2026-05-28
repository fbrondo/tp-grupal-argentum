#pragma once

#include <QMainWindow>
#include <array>
#include <memory>

#include "common/includes/map/layer.h"
#include "common/includes/map/map.h"

class MapScene;
class MapView;
class Sprite;
class TileWidget;
class QComboBox;
class QCheckBox;

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

private:
    void onNew();
    void onOpen();
    void onSave();
    void onSaveAs();
    void onZoomIn();
    void onZoomOut();
    void onToggleFullscreen();
    void onSelectGraficosDir();
    void setupEditor();
    void setupToolBar();
    void loadMapIntoScene();
    void loadLayerSprites(const QString& maps_base_dir);
    void updateTitle();
    void tryDefaultGraficosDir();

    Ui::MainWindow* ui_;
    std::unique_ptr<Map> map_;
    QString file_path_;
    std::array<Sprite*, layer_count> sprites_{};
    MapScene* scene_;
    MapView* view_;
    TileWidget* tile_widget_;
    QComboBox* layer_combo_;
    QCheckBox* walkable_check_;
};
