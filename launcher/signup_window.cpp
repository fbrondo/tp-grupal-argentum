#include "signup_window.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPushButton>

#include "ui_signup_window.h"

static const QStringList RACES = {"Humano", "Elfo", "Enano", "Gnomo"};
static const QStringList CLASES = {"Mago", "Clérigo", "Paladín", "Guerrero"};
static QString unitsBase() { return QString(ARGENTUM_SHARE_PATH) + "/common/assets/units"; }

// --- Race helpers ---

QString SignupWindow::raceDir() const {
    switch (race_idx_) {
        case 0:
            return "humanos";
        case 1:
            return "elfos";
        case 2:
            return "enanos";
        default:
            return "gnomos";
    }
}

// --- Constructor / destructor ---

SignupWindow::SignupWindow(const QString& host, const QString& port, QWidget* login_parent):
        QMainWindow(login_parent ? login_parent->parentWidget() : nullptr),
        ui_(new Ui::SignupWindow),
        host_(host),
        port_(port),
        login_parent_(login_parent) {
    ui_->setupUi(this);

    connect(ui_->prevRaceBtn, &QPushButton::clicked, this, &SignupWindow::onPrevRace);
    connect(ui_->nextRaceBtn, &QPushButton::clicked, this, &SignupWindow::onNextRace);
    connect(ui_->prevClaseBtn, &QPushButton::clicked, this, &SignupWindow::onPrevClase);
    connect(ui_->nextClaseBtn, &QPushButton::clicked, this, &SignupWindow::onNextClase);
    connect(ui_->prevHeadBtn, &QPushButton::clicked, this, &SignupWindow::onPrevHead);
    connect(ui_->nextHeadBtn, &QPushButton::clicked, this, &SignupWindow::onNextHead);
    connect(ui_->prevBodyBtn, &QPushButton::clicked, this, &SignupWindow::onPrevBody);
    connect(ui_->nextBodyBtn, &QPushButton::clicked, this, &SignupWindow::onNextBody);
    connect(ui_->crearBtn, &QPushButton::clicked, this, &SignupWindow::onCrear);
    connect(ui_->volverBtn, &QPushButton::clicked, this, &SignupWindow::onVolver);

    loadRaceFiles();
    loadSprites();
    updatePreview();

    auto* previewShadow = new QGraphicsDropShadowEffect(this);
    previewShadow->setBlurRadius(18);
    previewShadow->setColor(QColor(0, 0, 0, 100));
    previewShadow->setOffset(0, 4);
    ui_->previewLabel->setGraphicsEffect(previewShadow);
}

SignupWindow::~SignupWindow() { delete ui_; }

// --- Sprite loading ---

void SignupWindow::loadRaceFiles() {
    const QString base = unitsBase();
    const QString race = raceDir();

    QDir hDir(base + "/heads/" + race);
    head_files_ = hDir.entryList({"*.png"}, QDir::Files, QDir::Name);
    for (auto& f: head_files_) f = hDir.filePath(f);

    QDir bDir(base + "/bodies/" + race);
    body_files_ = bDir.entryList({"*.png"}, QDir::Files, QDir::Name);
    for (auto& f: body_files_) f = bDir.filePath(f);

    if (head_id_ > head_files_.size())
        head_id_ = 1;
    if (body_id_ > body_files_.size())
        body_id_ = 1;
}

void SignupWindow::loadSprites() {
    if (!head_files_.isEmpty()) {
        QPixmap sheet(head_files_[head_id_ - 1]);
        if (!sheet.isNull())
            head_pixmap_ = sheet.copy(0, 0, HEAD_W, HEAD_H);
    }
    if (!body_files_.isEmpty()) {
        QPixmap sheet(body_files_[body_id_ - 1]);
        if (!sheet.isNull())
            body_pixmap_ = sheet.copy(0, 0, BODY_W, BODY_H);
    }
}

void SignupWindow::updateSelectorLimits() {
    ui_->headLabel->setText(QString::number(head_id_));
    ui_->bodyLabel->setText(QString::number(body_id_));
}

// --- Selector slots ---

void SignupWindow::onPrevRace() {
    race_idx_ = (race_idx_ - 1 + RACES.size()) % RACES.size();
    ui_->raceLabel->setText(RACES[race_idx_]);
    head_id_ = 1;
    body_id_ = 1;
    loadRaceFiles();
    loadSprites();
    updateSelectorLimits();
    updatePreview();
}

void SignupWindow::onNextRace() {
    race_idx_ = (race_idx_ + 1) % RACES.size();
    ui_->raceLabel->setText(RACES[race_idx_]);
    head_id_ = 1;
    body_id_ = 1;
    loadRaceFiles();
    loadSprites();
    updateSelectorLimits();
    updatePreview();
}

void SignupWindow::onPrevClase() {
    clase_idx_ = (clase_idx_ - 1 + CLASES.size()) % CLASES.size();
    ui_->claseLabel->setText(CLASES[clase_idx_]);
}

void SignupWindow::onNextClase() {
    clase_idx_ = (clase_idx_ + 1) % CLASES.size();
    ui_->claseLabel->setText(CLASES[clase_idx_]);
}

void SignupWindow::onPrevHead() {
    if (head_id_ > 1) {
        --head_id_;
        loadSprites();
        updateSelectorLimits();
        updatePreview();
    }
}

void SignupWindow::onNextHead() {
    if (head_id_ < head_files_.size()) {
        ++head_id_;
        loadSprites();
        updateSelectorLimits();
        updatePreview();
    }
}

void SignupWindow::onPrevBody() {
    if (body_id_ > 1) {
        --body_id_;
        loadSprites();
        updateSelectorLimits();
        updatePreview();
    }
}

void SignupWindow::onNextBody() {
    if (body_id_ < body_files_.size()) {
        ++body_id_;
        loadSprites();
        updateSelectorLimits();
        updatePreview();
    }
}

// --- Preview rendering ---

void SignupWindow::updatePreview() {
    constexpr int PREVIEW_W = 128;
    constexpr int PREVIEW_H = 128;

    QPixmap canvas(PREVIEW_W, PREVIEW_H);
    canvas.fill(Qt::black);
    QPainter painter(&canvas);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    const QPixmap bg(QString(ARGENTUM_SHARE_PATH) + "/common/assets/maps/background/121.png");
    if (!bg.isNull())
        painter.drawPixmap(0, 0, PREVIEW_W, PREVIEW_H, bg);

    // gnomos (3) and enanos (2) need more overlap due to shorter stature
    const int OVERLAP = (race_idx_ == 2 || race_idx_ == 3) ? 52 : 44;
    const int totalH = HEAD_H + BODY_H - OVERLAP;
    const int startY = (PREVIEW_H - totalH) / 2;

    if (!body_pixmap_.isNull()) {
        const int bx = (PREVIEW_W - BODY_W) / 2;
        const int by = startY + HEAD_H - OVERLAP;
        painter.drawPixmap(bx, by, BODY_W, BODY_H, body_pixmap_);
    }

    if (!head_pixmap_.isNull()) {
        const int hx = (PREVIEW_W - HEAD_W) / 2;
        painter.drawPixmap(hx, startY, HEAD_W, HEAD_H, head_pixmap_);
    }

    painter.end();
    ui_->previewLabel->setPixmap(canvas);
}

// --- Signup / launch ---

bool SignupWindow::runClient(const QStringList& args, QString& out_stdout) {
    const QString binary = QCoreApplication::applicationDirPath() + "/taller_client";
    QProcess proc;
    proc.start(binary, args);

    if (!proc.waitForFinished(PROCESS_TIMEOUT_MS)) {
        proc.kill();
        setStatus("Timeout: no se pudo conectar al servidor.");
        return false;
    }

    if (proc.exitCode() != 0) {
        const QString err = proc.readAllStandardError().trimmed();
        setStatus(err.isEmpty() ? "Error desconocido." : err);
        return false;
    }

    out_stdout = proc.readAllStandardOutput().trimmed();
    return true;
}

void SignupWindow::onCrear() {
    const QString user = ui_->userEdit->text().trimmed();
    const QString pass = ui_->passEdit->text();

    if (user.isEmpty() || pass.isEmpty()) {
        setStatus("Complete todos los campos.");
        return;
    }

    setBusy(true);
    setStatus("Registrando...");

    const int headSpriteId = head_id_ - 1;
    const int bodySpriteId = body_id_ - 1;

    QString ignored;
    if (!runClient({host_, port_, "--signup", user, pass, QString::number(race_idx_ + 1),
                    QString::number(clase_idx_ + 1), QString::number(headSpriteId),
                    QString::number(bodySpriteId)},
                   ignored)) {
        setBusy(false);
        return;
    }

    const QString binary = QCoreApplication::applicationDirPath() + "/taller_client";
    QProcess::startDetached(binary, {host_, port_, user, pass});
    QApplication::quit();
}

void SignupWindow::onVolver() {
    if (login_parent_)
        login_parent_->show();
    close();
}

void SignupWindow::setStatus(const QString& msg) { ui_->statusLabel->setText(msg); }

void SignupWindow::setBusy(bool busy) {
    ui_->crearBtn->setEnabled(!busy);
    ui_->volverBtn->setEnabled(!busy);
    ui_->userEdit->setEnabled(!busy);
    ui_->passEdit->setEnabled(!busy);
}
