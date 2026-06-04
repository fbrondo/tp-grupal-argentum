#pragma once

#include <QMainWindow>
#include <QPixmap>
#include <QProcess>
#include <QString>
#include <QStringList>

QT_BEGIN_NAMESPACE
namespace Ui {
class SignupWindow;
}
QT_END_NAMESPACE

class SignupWindow: public QMainWindow {
    Q_OBJECT

public:
    SignupWindow(const QString& host, const QString& port, QWidget* login_parent = nullptr);
    ~SignupWindow() override;

private slots:
    void onPrevRace();
    void onNextRace();
    void onPrevClase();
    void onNextClase();
    void onPrevHead();
    void onNextHead();
    void onPrevBody();
    void onNextBody();
    void onCrear();
    void onVolver();

private:
    void updatePreview();
    bool runClient(const QStringList& args, QString& out_stdout);
    void setStatus(const QString& msg);
    void setBusy(bool busy);

    static constexpr int PROCESS_TIMEOUT_MS = 5000;
    static constexpr int MAX_HEAD_ID = 9;
    static constexpr int MAX_BODY_ID = 9;

    Ui::SignupWindow* ui_;
    QString host_;
    QString port_;
    QWidget* login_parent_;

    int race_idx_ = 0;
    int clase_idx_ = 0;
    int head_id_ = 1;
    int body_id_ = 1;

    QPixmap head_pixmap_;
    QPixmap body_pixmap_;
};
