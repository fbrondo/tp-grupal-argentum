#include "login_window.h"

#include "ui_login_window.h"

LoginWindow::LoginWindow(QWidget* parent): QMainWindow(parent), ui_(new Ui::LoginWindow) {
    ui_->setupUi(this);
}

LoginWindow::~LoginWindow() { delete ui_; }
