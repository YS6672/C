#ifndef EDITMODELDIALOG_H
#define EDITMODELDIALOG_H

#include <QDialog>
#include <QVector3D>
#include <qvector3d.h>

namespace Ui {
class EditModelDialog;
}

class EditModelDialog : public QDialog {
    Q_OBJECT

  public:
    explicit EditModelDialog(QWidget *parent = nullptr);
    ~EditModelDialog();

    bool runDialog(QVector3D &offset, float &scale);

  public slots:
    void handleScaleValueChange(double d);
    void handleOffsetXValueChange(double d);
    void handleOffsetYValueChange(double d);
    void handleOffsetZValueChange(double d);

  signals:
    void signalRedrawScene();

  private:
    Ui::EditModelDialog *ui;

    QVector3D *offsetReference;
    float *scaleReference;
};

#endif // EDITMODELDIALOG_H
