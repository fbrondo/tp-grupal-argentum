#include "signup_window.h"

#include <QCoreApplication>
#include <QPainter>
#include <QPushButton>

#include "character_window.h"
#include "ui_signup_window.h"

static const QStringList RACES = {"Humano", "Elfo", "Enano", "Gnomo"};
static const QStringList CLASES = {"Mago", "Clérigo", "Paladín", "Guerrero"};

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

    const QString assets = QCoreApplication::applicationDirPath() + "/client/assets/";
    head_pixmap_.load(assets + "head.png");
    body_pixmap_.load(assets + "body.png");

    updatePreview();
}

SignupWindow::~SignupWindow() { delete ui_; }

void SignupWindow::onPrevRace() {
    race_idx_ = (race_idx_ - 1 + RACES.size()) % RACES.size();
    ui_->raceLabel->setText(RACES[race_idx_]);
}

void SignupWindow::onNextRace() {
    race_idx_ = (race_idx_ + 1) % RACES.size();
    ui_->raceLabel->setText(RACES[race_idx_]);
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
        ui_->headLabel->setText(QString::number(head_id_));
        updatePreview();
    }
}

void SignupWindow::onNextHead() {
    if (head_id_ < MAX_HEAD_ID) {
        ++head_id_;
        ui_->headLabel->setText(QString::number(head_id_));
        updatePreview();
    }
}

void SignupWindow::onPrevBody() {
    if (body_id_ > 1) {
        --body_id_;
        ui_->bodyLabel->setText(QString::number(body_id_));
        updatePreview();
    }
}

void SignupWindow::onNextBody() {
    if (body_id_ < MAX_BODY_ID) {
        ++body_id_;
        ui_->bodyLabel->setText(QString::number(body_id_));
        updatePreview();
    }
}

void SignupWindow::updatePreview() {
    constexpr int PREVIEW_W = 128;
    constexpr int PREVIEW_H = 128;

    QPixmap canvas(PREVIEW_W, PREVIEW_H);
    canvas.fill(Qt::transparent);
    QPainter painter(&canvas);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    if (!body_pixmap_.isNull()) {
        const int bx = (PREVIEW_W - body_pixmap_.width() * 3) / 2;
        const int by = (PREVIEW_H - body_pixmap_.height() * 3) / 2 + 10;
        painter.drawPixmap(bx, by, body_pixmap_.width() * 3, body_pixmap_.height() * 3,
                           body_pixmap_);
    }

    if (!head_pixmap_.isNull()) {
        const int hx = (PREVIEW_W - head_pixmap_.width() * 3) / 2;
        const int hy =
                (PREVIEW_H - body_pixmap_.height() * 3) / 2 + 10 - head_pixmap_.height() * 3 + 3;
        painter.drawPixmap(hx, hy, head_pixmap_.width() * 3, head_pixmap_.height() * 3,
                           head_pixmap_);
    }

    painter.end();
    ui_->previewLabel->setPixmap(canvas);
}

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

    QString ignored;
    if (!runClient(
                {host_, port_, "--signup", user, pass, QString::number(race_idx_),
                 QString::number(clase_idx_), QString::number(head_id_), QString::number(body_id_)},
                ignored)) {
        setBusy(false);
        return;
    }

    setStatus("Iniciando sesión...");

    QString payload;
    if (!runClient({host_, port_, "--login", user, pass}, payload)) {
        setBusy(false);
        return;
    }

    auto* win = new CharacterWindow(host_, port_, user, pass, payload);
    win->show();
    close();
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
