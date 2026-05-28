#pragma once
#include <QDialog>
#include <memory>

#include "common/includes/map/map.h"

class QLineEdit;
class QSpinBox;

class NewMapDialog: public QDialog {
    Q_OBJECT
public:
    explicit NewMapDialog(QWidget* parent = nullptr);

    std::unique_ptr<Map> makeMap() const;

private:
    QLineEdit* name_edit_;
    QSpinBox* width_spin_;
    QSpinBox* height_spin_;
};
