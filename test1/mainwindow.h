#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "loadedModel.h"
#include "loadedModelList.h"

#include <Model.hpp>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <qlistwidget.h>
#include <qobjectdefs.h>
#include <vtkDataSetAlgorithm.h>
#include <vtkDataSetMapper.h>
#include <vtkNamedColors.h>
#include <vtkPolyData.h>
#include <vtkPolyDataCollection.h>
#include <vtkSTLReader.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

  public slots:
    /**
     * @brief open file
     */
    void handleFileOpen(bool checked);
    void handleFileAdd(bool checked);
    /**
     * @brief save file
     */
    void handleFileSave(bool checked);
    void handlePrint(bool checked);
    void handleChangeModelColor();
    void handleChangeBackgroundColor();
    void handleResetCamera();
    void handleLightIntensityChange(int value);
    void handleShrinkFilter(bool checked);
    void handleShrinkFactor(int value);
    void handleClipFilter(bool checked);
    void handleBoundingBoxFilter(bool checked);
    void handleWarpFilter(bool checked);
    void handleWarpXslide(int value);
    void handleWarpYslide(int value);
    void handleWarpZslide(int value);
    void handleModelListChange(QListWidgetItem *item);
    void handleModelListDoubleClick(const QModelIndex &index);

    void handleRedrawModel();

  signals:
    void statusUpdateMessage(const QString &message, int timeout);

  private:
    enum FILE_TYPES { NONE, STL, MOD };

    vtkNew<vtkPolyDataCollection> globalPD;

    QColor modelColor;
    QColor backgroundColor;
    float lightIntensity = 0.5;

    bool warpFilterEnabled = false;
    float warpFilterX = 0, warpFilterY = 0, warpFilterZ = 0;

    bool boundingBoxFilterEnabled = false;

    bool clipFilterEnabled = false;

    bool shrinkFilterEnabled = false;
    float shrinkFactor;

    Ui::MainWindow *ui;

    void addLoadedUoNModel(Model &model);
    void addLoadedSTLModel(vtkSTLReader *stlReader);

    void redrawModel();
    void updateModelInfo();
    void changeModelColor();
    void generateUoNModelFromRenderer(Model *model, vtkPolyData *pd);

    FILE_TYPES currentLoadedFileType = NONE;
};

#endif // MAINWINDOW_H
