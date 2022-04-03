#include "editmodeldialog.h"
#include "mainwindow.h"
#include "ui_editmodeldialog.h"
#include <qspinbox.h>

EditModelDialog::EditModelDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::EditModelDialog) {
    ui->setupUi(this);

    connect(ui->scaleBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &EditModelDialog::handleScaleValueChange);
    connect(ui->offsetXBox,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &EditModelDialog::handleOffsetXValueChange);
    connect(ui->offsetYBox,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &EditModelDialog::handleOffsetYValueChange);
    connect(ui->offsetZBox,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &EditModelDialog::handleOffsetZValueChange);
}

EditModelDialog::~EditModelDialog() { delete ui; }

bool EditModelDialog::runDialog(QVector3D &offset, float &scale) {
    scaleReference = &scale;
    offsetReference = &offset;

    ui->scaleBox->setValue(scale);
    ui->offsetXBox->setValue(offset.x());
    ui->offsetYBox->setValue(offset.y());
    ui->offsetZBox->setValue(offset.z());

    if (this->exec() == QDialog::Accepted) {
        scale = ui->scaleBox->value();
        offset.setX(ui->offsetXBox->value());
        offset.setY(ui->offsetYBox->value());
        offset.setZ(ui->offsetZBox->value());
        return true;
    }

    return false;
}

void EditModelDialog::handleScaleValueChange(double d) {
    *scaleReference = d;
    emit signalRedrawScene();
}
void EditModelDialog::handleOffsetXValueChange(double d) {
    offsetReference->setX(d);
    emit signalRedrawScene();
}
void EditModelDialog::handleOffsetYValueChange(double d) {
    offsetReference->setY(d);
    emit signalRedrawScene();
}
void EditModelDialog::handleOffsetZValueChange(double d) {
    offsetReference->setZ(d);
    emit signalRedrawScene();
}
