#include "new_map_dialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QVBoxLayout>

NewMapDialog::NewMapDialog(QWidget* parent): QDialog(parent) {
    setWindowTitle("Nuevo mapa");

    name_edit_ = new QLineEdit("Mapa", this);

    width_spin_ = new QSpinBox(this);
    width_spin_->setRange(1, MAX_MAP_WIDTH);
    width_spin_->setValue(20);

    height_spin_ = new QSpinBox(this);
    height_spin_->setRange(1, MAX_MAP_HEIGHT);
    height_spin_->setValue(20);

    auto* form = new QFormLayout;
    form->addRow("Nombre:", name_edit_);
    form->addRow("Ancho:", width_spin_);
    form->addRow("Alto:", height_spin_);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);
}

std::unique_ptr<Map> NewMapDialog::makeMap() const {
    return std::make_unique<Map>(name_edit_->text().toStdString(), width_spin_->value(),
                                 height_spin_->value());
}
