#include "login_window.h"

#include <QCoreApplication>
#include <QProcess>
#include <QPushButton>

#include "signup_window.h"
#include "ui_login_window.h"

LoginWindow::LoginWindow(QWidget* parent): QMainWindow(parent), ui_(new Ui::LoginWindow) {
    ui_->setupUi(this);
    connect(ui_->loginBtn, &QPushButton::clicked, this, &LoginWindow::onLogin);
    connect(ui_->signupBtn, &QPushButton::clicked, this, &LoginWindow::onSignup);
}

LoginWindow::~LoginWindow() { delete ui_; }

bool LoginWindow::runClient(const QStringList& args, QString& out_stdout) {
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

bool LoginWindow::tryLogin(const QString& host, const QString& port, const QString& user,
                           const QString& pass, QString& out_payload) {
    return runClient({host, port, "--login", user, pass}, out_payload);
}

void LoginWindow::launchGame(const QString& host, const QString& port) {
    const QString binary = QCoreApplication::applicationDirPath() + "/taller_client";

    // TODO: Review this
    QProcess::startDetached(binary, {host, port});
    QApplication::quit();
}

void LoginWindow::onLogin() {
    const QString host = ui_->hostEdit->text().trimmed();
    const QString port = ui_->portEdit->text().trimmed();
    const QString user = ui_->loginUserEdit->text().trimmed();
    const QString pass = ui_->loginPassEdit->text();

    if (host.isEmpty() || port.isEmpty() || user.isEmpty() || pass.isEmpty()) {
        setStatus("Complete todos los campos.");
        return;
    }

    setStatus("Conectando...");
    setBusy(true);

    QString payload;
    if (!tryLogin(host, port, user, pass, payload)) {
        setBusy(false);
        return;
    }

    launchGame(host, port);
}

void LoginWindow::onSignup() {
    const QString host = ui_->hostEdit->text().trimmed();
    const QString port = ui_->portEdit->text().trimmed();
    auto* sw = new SignupWindow(host, port, this);
    sw->show();
    hide();
}

void LoginWindow::setStatus(const QString& msg) { ui_->statusLabel->setText(msg); }

void LoginWindow::setBusy(bool busy) {
    ui_->loginBtn->setEnabled(!busy);
    ui_->signupBtn->setEnabled(!busy);
    ui_->hostEdit->setEnabled(!busy);
    ui_->portEdit->setEnabled(!busy);
}
