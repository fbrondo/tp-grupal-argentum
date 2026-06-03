#include "character_window.h"

CharacterWindow::CharacterWindow(const QString& host, const QString& port, const QString& user,
                                 const QString& pass, const QString& payload, QWidget* parent):
        QMainWindow(parent) {
    Q_UNUSED(host)
    Q_UNUSED(port)
    Q_UNUSED(user)
    Q_UNUSED(pass)
    Q_UNUSED(payload)
    setWindowTitle("Argentum Online");
    resize(500, 400);
}
