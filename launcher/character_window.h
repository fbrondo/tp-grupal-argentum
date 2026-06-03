#pragma once

#include <QMainWindow>
#include <QString>

class CharacterWindow: public QMainWindow {
    Q_OBJECT

public:
    CharacterWindow(const QString& host, const QString& port, const QString& user,
                    const QString& pass, const QString& payload, QWidget* parent = nullptr);
};
