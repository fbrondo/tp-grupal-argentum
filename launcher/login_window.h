#pragma once

#include <QApplication>
#include <QMainWindow>
#include <QProcess>
#include <QString>
#include <QStringList>

QT_BEGIN_NAMESPACE
namespace Ui {
class LoginWindow;
}
QT_END_NAMESPACE

class LoginWindow: public QMainWindow {
    Q_OBJECT

public:
    explicit LoginWindow(QWidget* parent = nullptr);
    ~LoginWindow() override;

private slots:
    void onLogin();
    void onSignup();

private:
    static constexpr int PROCESS_TIMEOUT_MS = 5000;

    bool runClient(const QStringList& args, QString& out_stdout);
    bool tryLogin(const QString& host, const QString& port, const QString& user,
                  const QString& pass, QString& out_payload);
    void launchGame(const QString& host, const QString& port);
    void setStatus(const QString& msg);
    void setBusy(bool busy);

    Ui::LoginWindow* ui_;
};
