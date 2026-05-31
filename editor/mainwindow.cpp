#include "mainwindow.h"

#include <QActionGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QMessageBox>
#include <QSplitter>
#include <QToolBar>
#include <algorithm>

#include "common/includes/map/map_serializer.h"

#include "map_scene.h"
#include "map_view.h"
#include "new_map_dialog.h"
#include "sprite.h"
#include "tile_widget.h"
#include "ui_mainwindow.h"

// Folder name for each layer inside the maps base directory.
static constexpr std::array<const char*, layer_count> LAYER_FOLDERS = {"background", "details",
                                                                       "objets", "roof"};

// TOML filename for each layer inside the data directory.
static constexpr std::array<const char*, layer_count> LAYER_TOMLS = {
        "background_sprites.toml", "details_sprites.toml", "objects_sprites.toml",
        "roof_sprites.toml"};

MainWindow::MainWindow(QWidget* parent):
        QMainWindow(parent), ui_(new Ui::MainWindow), scene_(new MapScene(this)) {
    std::generate(sprites_.begin(), sprites_.end(), []() { return new Sprite(); });

    ui_->setupUi(this);

    setupEditor();
    setupToolBar();

    connect(ui_->actionNuevo, &QAction::triggered, this, &MainWindow::onNew);
    connect(ui_->actionAbrir, &QAction::triggered, this, &MainWindow::onOpen);
    connect(ui_->actionGuardar, &QAction::triggered, this, &MainWindow::onSave);
    connect(ui_->actionGuardar_Como, &QAction::triggered, this, &MainWindow::onSaveAs);

    ui_->actionDeshacer->setShortcut(QKeySequence::Undo);
    ui_->actionDeshacer->setEnabled(false);
    connect(ui_->actionDeshacer, &QAction::triggered, this, &MainWindow::onUndo);
    connect(scene_, &MapScene::undoAvailable, ui_->actionDeshacer, &QAction::setEnabled);

    ui_->actionRehacer->setShortcut(QKeySequence::Redo);
    ui_->actionRehacer->setEnabled(false);
    connect(ui_->actionRehacer, &QAction::triggered, this, &MainWindow::onRedo);
    connect(scene_, &MapScene::redoAvailable, ui_->actionRehacer, &QAction::setEnabled);

    connect(ui_->actionZoom, &QAction::triggered, this, &MainWindow::onZoomIn);
    connect(ui_->actionZoom_2, &QAction::triggered, this, &MainWindow::onZoomOut);
    connect(ui_->actionSeleccionar_graficos, &QAction::triggered, this,
            &MainWindow::onSelectGraficosDir);

    ui_->actionPantalla_Completa->setCheckable(true);
    ui_->actionPantalla_Completa->setShortcut(QKeySequence("F11"));
    connect(ui_->actionPantalla_Completa, &QAction::triggered, this,
            &MainWindow::onToggleFullscreen);

    connect(scene_, &MapScene::tileModified, this, [this](int x, int y) {
        statusBar()->showMessage(QString("Tile (%1, %2) modificado").arg(x).arg(y), 2000);
    });

    connect(scene_, &MapScene::tileSelected, this, &MainWindow::onTileSelected);

    tryDefaultGraficosDir();
    updateTitle();
    showMaximized();
}

MainWindow::~MainWindow() {
    for (auto* s: sprites_) delete s;
    delete ui_;
}

void MainWindow::setupEditor() {
    auto* splitter = new QSplitter(Qt::Horizontal, this);

    tile_widget_ = new TileWidget(this);
    tile_widget_->setFixedWidth(220);

    connect(tile_widget_, &TileWidget::spriteSelected, scene_, &MapScene::setCurrentSpriteId);
    connect(tile_widget_, &TileWidget::layerChangeRequested, this, [this](Layer layer) {
        layer_combo_->blockSignals(true);
        layer_combo_->setCurrentIndex(static_cast<int>(layer));
        layer_combo_->blockSignals(false);
        scene_->setCurrentLayer(layer);
        region_combo_->setEnabled(layer == Layer::Background);
    });

    view_ = new MapView(scene_, this);
    view_->setDragMode(QGraphicsView::NoDrag);
    view_->setBackgroundBrush(QBrush(QColor(20, 20, 20)));
    view_->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    view_->setResizeAnchor(QGraphicsView::AnchorUnderMouse);

    splitter->addWidget(tile_widget_);
    splitter->addWidget(view_);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    setCentralWidget(splitter);
}

void MainWindow::setupToolBar() {
    auto* toolbar = addToolBar("Herramientas");
    toolbar->setMovable(false);

    auto* tool_group = new QActionGroup(this);
    auto* paint_action = toolbar->addAction("Insertar");
    paint_action->setCheckable(true);
    paint_action->setChecked(true);
    tool_group->addAction(paint_action);
    auto* select_action = toolbar->addAction("Seleccionar");
    select_action->setCheckable(true);
    tool_group->addAction(select_action);

    connect(paint_action, &QAction::triggered, this, [this]() {
        scene_->setCurrentTool(MapScene::Tool::Paint);
        view_->unsetCursor();
    });
    connect(select_action, &QAction::triggered, this, [this]() {
        scene_->setCurrentTool(MapScene::Tool::Select);
        view_->setCursor(Qt::PointingHandCursor);
    });

    toolbar->addSeparator();
    toolbar->addWidget(new QLabel("  Capa: ", this));
    layer_combo_ = new QComboBox(this);
    layer_combo_->addItem("Background", static_cast<int>(Layer::Background));
    layer_combo_->addItem("Details", static_cast<int>(Layer::Details));
    layer_combo_->addItem("Object", static_cast<int>(Layer::Object));
    layer_combo_->addItem("Roof", static_cast<int>(Layer::Roof));
    toolbar->addWidget(layer_combo_);

    toolbar->addSeparator();
    toolbar->addWidget(new QLabel("  Caminable: ", this));
    walkable_check_ = new QCheckBox(this);
    walkable_check_->setChecked(true);
    toolbar->addWidget(walkable_check_);

    toolbar->addSeparator();
    toolbar->addWidget(new QLabel("  Región: ", this));
    region_combo_ = new QComboBox(this);
    region_combo_->addItem("Campo", static_cast<int>(Region::Field));
    region_combo_->addItem("Ciudad", static_cast<int>(Region::City));
    region_combo_->addItem("Pueblo", static_cast<int>(Region::Town));
    region_combo_->addItem("Bosque", static_cast<int>(Region::Forest));
    region_combo_->addItem("Desierto", static_cast<int>(Region::Desert));
    region_combo_->addItem("Caverna", static_cast<int>(Region::Cavern));
    region_combo_->addItem("Mazmorra", static_cast<int>(Region::Dungeon));
    toolbar->addWidget(region_combo_);

    connect(layer_combo_, &QComboBox::currentIndexChanged, this, [this](int idx) {
        Layer layer = static_cast<Layer>(layer_combo_->itemData(idx).toInt());
        scene_->setCurrentLayer(layer);
        tile_widget_->setCurrentLayer(layer);
        region_combo_->setEnabled(layer == Layer::Background);
    });
    connect(walkable_check_, &QCheckBox::toggled, scene_, &MapScene::setCurrentWalkable);
    connect(region_combo_, &QComboBox::currentIndexChanged, this, [this](int idx) {
        scene_->setCurrentRegion(static_cast<Region>(region_combo_->itemData(idx).toInt()));
    });

    toolbar->addSeparator();
    toolbar->addAction(ui_->actionPantalla_Completa);
}

void MainWindow::onNew() {
    NewMapDialog map_dialog(this);
    if (map_dialog.exec() != QDialog::Accepted)
        return;
    map_ = map_dialog.makeMap();
    file_path_.clear();
    loadMapIntoScene();
    updateTitle();
}

void MainWindow::onOpen() {
    QString path = QFileDialog::getOpenFileName(this, "Abrir mapa", {}, "Mapas TOML (*.toml)");
    if (path.isEmpty())
        return;
    try {
        map_ = std::make_unique<Map>(MapSerializer::load(path.toStdString()));
        file_path_ = path;
        loadMapIntoScene();
        updateTitle();
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error",
                              QString("No se pudo abrir el mapa:\n%1").arg(e.what()));
    }
}

void MainWindow::onSave() {
    if (!map_)
        return;
    if (file_path_.isEmpty()) {
        onSaveAs();
        return;
    }
    try {
        MapSerializer::save(*map_, file_path_.toStdString());
        statusBar()->showMessage("Mapa guardado.", 2000);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString("No se pudo guardar:\n%1").arg(e.what()));
    }
}

void MainWindow::onSaveAs() {
    if (!map_)
        return;
    QString path =
            QFileDialog::getSaveFileName(this, "Guardar mapa como", {}, "Mapas TOML (*.toml)");
    if (path.isEmpty())
        return;
    if (!path.endsWith(".toml"))
        path += ".toml";
    file_path_ = path;
    onSave();
    updateTitle();
}

void MainWindow::onUndo() { scene_->undo(); }
void MainWindow::onRedo() { scene_->redo(); }

void MainWindow::onTileSelected(Tile tile) {
    walkable_check_->blockSignals(true);
    walkable_check_->setChecked(tile.walkable);
    walkable_check_->blockSignals(false);

    for (int i = 0; i < region_combo_->count(); ++i) {
        if (region_combo_->itemData(i).toInt() == static_cast<int>(tile.region)) {
            region_combo_->blockSignals(true);
            region_combo_->setCurrentIndex(i);
            region_combo_->blockSignals(false);
            break;
        }
    }

    statusBar()->showMessage(QString("Caminable: %1 - Región: %2")
                                     .arg(tile.walkable ? "Si" : "No")
                                     .arg(region_combo_->currentText()),
                             3000);
}

void MainWindow::onZoomIn() { view_->scale(1.25, 1.25); }
void MainWindow::onZoomOut() { view_->scale(0.8, 0.8); }

void MainWindow::onToggleFullscreen() {
    if (isFullScreen()) {
        showNormal();
        ui_->actionPantalla_Completa->setChecked(false);
    } else {
        showFullScreen();
        ui_->actionPantalla_Completa->setChecked(true);
    }
}

void MainWindow::onSelectGraficosDir() {
    QString base = QFileDialog::getExistingDirectory(
            this, "Seleccionar carpeta de mapas (common/assets/maps)");
    if (base.isEmpty())
        return;
    loadLayerSprites(base);
    if (map_)
        loadMapIntoScene();
}

void MainWindow::loadLayerSprites(const QString& maps_base_dir) {
    QDir base(maps_base_dir);
    // common/data/ lives two levels above common/assets/maps/
    QString data_dir = base.filePath("../../data");

    for (int li = 0; li < static_cast<int>(layer_count); ++li) {
        QString layer_dir = base.filePath(LAYER_FOLDERS[li]);
        if (!QDir(layer_dir).exists())
            continue;
        sprites_[li]->setGraphicsDir(layer_dir);

        QString toml = QDir(data_dir).filePath(LAYER_TOMLS[li]);
        if (QFileInfo::exists(toml))
            sprites_[li]->setSpritesConfig(QFileInfo(toml).absoluteFilePath());

        tile_widget_->setLayerSprite(static_cast<Layer>(li), sprites_[li]);
    }
}

void MainWindow::loadMapIntoScene() { scene_->loadMap(map_.get(), sprites_); }

void MainWindow::updateTitle() {
    QString title = "Argentum Editor";
    if (map_)
        title += " — " + QString::fromStdString(map_->name());
    if (!file_path_.isEmpty())
        title += " [" + file_path_ + "]";
    setWindowTitle(title);
}

void MainWindow::tryDefaultGraficosDir() {
    const QString app_dir = QCoreApplication::applicationDirPath();
    const QStringList candidates = {
            app_dir + "/../../common/assets/maps",
            app_dir + "/../common/assets/maps",
            app_dir + "/common/assets/maps",
    };
    auto it = std::find_if(candidates.begin(), candidates.end(),
                           [](const QString& c) { return QDir(c).exists(); });
    if (it != candidates.end())
        loadLayerSprites(QDir(*it).absolutePath());
}
