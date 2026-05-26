#include "mainwindow.h"

#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QGraphicsView>
#include <QLabel>
#include <QMessageBox>
#include <QSplitter>
#include <QToolBar>

#include "common/includes/map/map_serializer.h"

#include "map_scene.h"
#include "new_map_dialog.h"
#include "sprite.h"
#include "tile_widget.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent):
        QMainWindow(parent),
        ui_(new Ui::MainWindow),
        sprite_(new Sprite),
        scene_(new MapScene(this)) {
    ui_->setupUi(this);

    setupEditor();
    setupToolBar();

    connect(ui_->actionNuevo, &QAction::triggered, this, &MainWindow::onNew);
    connect(ui_->actionAbrir, &QAction::triggered, this, &MainWindow::onOpen);
    connect(ui_->actionGuardar, &QAction::triggered, this, &MainWindow::onSave);
    connect(ui_->actionGuardar_Como, &QAction::triggered, this, &MainWindow::onSaveAs);
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

    tryDefaultGraficosDir();
    updateTitle();
    showMaximized();
}

MainWindow::~MainWindow() {
    delete sprite_;
    delete ui_;
}

void MainWindow::setupEditor() {
    auto* splitter = new QSplitter(Qt::Horizontal, this);

    tile_widget_ = new TileWidget(this);
    tile_widget_->setFixedWidth(220);
    connect(tile_widget_, &TileWidget::spriteSelected, scene_, &MapScene::setCurrentSpriteId);

    view_ = new QGraphicsView(scene_, this);
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

    connect(layer_combo_, &QComboBox::currentIndexChanged, this, [this](int idx) {
        auto layer = static_cast<Layer>(layer_combo_->itemData(idx).toInt());
        scene_->setCurrentLayer(layer);
    });
    connect(walkable_check_, &QCheckBox::toggled, scene_, &MapScene::setCurrentWalkable);

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
    QString path = QFileDialog::getExistingDirectory(this, "Seleccionar directorio de gráficos");
    if (path.isEmpty())
        return;
    sprite_->setGraphicsDir(path);

    // Auto-detect sprites.toml relative to the graficos directory
    QString toml = QDir(path).filePath("../data/sprites.toml");
    if (QFileInfo::exists(toml))
        sprite_->setSpritesConfig(QFileInfo(toml).absoluteFilePath());

    tile_widget_->setSprite(sprite_);
    if (map_)
        loadMapIntoScene();
}

void MainWindow::loadMapIntoScene() { scene_->loadMap(map_.get(), sprite_); }

void MainWindow::updateTitle() {
    QString title = "Argentum Editor";
    if (map_)
        title += " — " + QString::fromStdString(map_->name());
    if (!file_path_.isEmpty())
        title += " [" + file_path_ + "]";
    setWindowTitle(title);
}

void MainWindow::tryDefaultGraficosDir() {
    // Each pair: { graficos_dir candidate, sprites.toml candidate }
    const QStringList graficos_candidates = {
            QCoreApplication::applicationDirPath() + "/../../common/Graficos",
            QCoreApplication::applicationDirPath() + "/../common/Graficos",
            QCoreApplication::applicationDirPath() + "/common/Graficos",
    };
    for (const QString& candidate: graficos_candidates) {
        QDir dir(candidate);
        if (!dir.exists())
            continue;

        sprite_->setGraphicsDir(dir.absolutePath());

        QString toml = dir.filePath("../data/sprites.toml");
        if (QFileInfo::exists(toml))
            sprite_->setSpritesConfig(QFileInfo(toml).absoluteFilePath());

        tile_widget_->setSprite(sprite_);
        return;
    }
}
