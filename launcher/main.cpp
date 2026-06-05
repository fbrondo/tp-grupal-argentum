#include <QApplication>
#include <QFile>

#include "login_window.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QFile qss(":/style/main.qss");
    if (qss.open(QFile::ReadOnly))
        app.setStyleSheet(qss.readAll());

    LoginWindow window;
    window.show();
    return QApplication::exec();
}
