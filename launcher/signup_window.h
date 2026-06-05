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
    void loadRaceFiles();
    void loadSprites();
    void updateSelectorLimits();
    bool runClient(const QStringList& args, QString& out_stdout);
    void setStatus(const QString& msg);
    void setBusy(bool busy);

    QString raceDir() const;

    static constexpr int PROCESS_TIMEOUT_MS = 5000;
    static constexpr int HEAD_W = 26;
    static constexpr int HEAD_H = 64;
    static constexpr int BODY_W = 25;
    static constexpr int BODY_H = 45;

    Ui::SignupWindow* ui_;
    QString host_;
    QString port_;
    QWidget* login_parent_;

    int race_idx_ = 0;
    int clase_idx_ = 0;
    int head_id_ = 1;
    int body_id_ = 1;

    QStringList head_files_;
    QStringList body_files_;
    QPixmap head_pixmap_;
    QPixmap body_pixmap_;
};
