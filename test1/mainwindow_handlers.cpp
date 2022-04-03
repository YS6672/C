#include <QColorDialog>
#include <QRegularExpression>
#include <iostream>
#include <qcheckbox.h>
#include <qcolordialog.h>
#include <qdialog.h>
#include <qprinter.h>
#include <qslider.h>

#include <qvector3d.h>
#include <vtkActor.h>
#include <vtkActorCollection.h>
#include <vtkAlgorithm.h>
#include <vtkAppendFilter.h>
#include <vtkAppendPolyData.h>
#include <vtkCamera.h>
#include <vtkCellArrayIterator.h>
#include <vtkCellType.h>
#include <vtkCleanPolyData.h>
#include <vtkClipDataSet.h>
#include <vtkColor.h>
#include <vtkCubeSource.h>
#include <vtkDataSetAlgorithm.h>
#include <vtkDataSetMapper.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkGeometryFilter.h>
#include <vtkHexahedron.h>
#include <vtkJPEGWriter.h>
#include <vtkLight.h>
#include <vtkLightCollection.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPNGWriter.h>
#include <vtkPlane.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkPyramid.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSTLWriter.h>
#include <vtkShrinkFilter.h>
#include <vtkSmartPointer.h>
#include <vtkTetra.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridGeometryFilter.h>
#include <vtkWindowToImageFilter.h>

#include "editmodeldialog.h"
#include "loadedModel.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::handleChangeModelColor() {
    modelColor = QColorDialog::getColor(modelColor);
    changeModelColor();
}

void MainWindow::handleChangeBackgroundColor() {
    backgroundColor = QColorDialog::getColor(backgroundColor);

    vtkSmartPointer<vtkRenderer> renderer =
        ui->qvtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
    renderer->SetBackground(backgroundColor.redF(), backgroundColor.greenF(),
                            backgroundColor.blueF());
    ui->qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::handleLightIntensityChange(int value) {
    vtkSmartPointer<vtkRenderer> renderer =
        ui->qvtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

    vtkLightCollection *lights = renderer->GetLights();
    vtkLight *light;
    lights->InitTraversal();
    while (light = lights->GetNextItem()) {
        light->SetIntensity((double)value / 1000);
    }

    ui->qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::handleResetCamera() {
    vtkSmartPointer<vtkRenderer> renderer =
        ui->qvtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
    renderer->ResetCamera();
    renderer->ResetCameraClippingRange();

    ui->qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::handleShrinkFilter(bool checked) {
    shrinkFilterEnabled = checked;
    ui->shrinkSlide->setEnabled(checked);
    redrawModel();
}

void MainWindow::handleShrinkFactor(int value) {
    shrinkFactor = (float)value / 100.0;
    redrawModel();
}

void MainWindow::handleClipFilter(bool checked) {
    clipFilterEnabled = checked;
    redrawModel();
}

void MainWindow::handleBoundingBoxFilter(bool checked) {
    boundingBoxFilterEnabled = checked;
    redrawModel();
}

void MainWindow::handleWarpFilter(bool checked) {
    warpFilterEnabled = checked;
    ui->warpXslide->setEnabled(checked);
    ui->warpYslide->setEnabled(checked);
    ui->warpZslide->setEnabled(checked);
    redrawModel();
}

void MainWindow::handleWarpXslide(int value) {
    warpFilterX = value;
    redrawModel();
}

void MainWindow::handleWarpYslide(int value) {
    warpFilterY = value;
    redrawModel();
}

void MainWindow::handleWarpZslide(int value) {
    warpFilterZ = value;
    redrawModel();
}

void MainWindow::handleFileOpen(bool checked) {
    QString filename = QFileDialog::getOpenFileName(
        this, tr("Open Model File"), "./", tr("3D Model File (*.stl *.mod)"));

    if (filename.isEmpty())
        return;

    /* All Files SHOULD be drawn as PolyData! */
    QString extention = QFileInfo(filename).suffix();
    QString modelName = QFileInfo(filename).baseName();

    globalPD->RemoveAllItems();
    LoadedModelList::getInstance().clear();

    currentLoadedFileType = NONE;

    if (extention.toLower() == "mod") {
        Model model;
        int ret = model.parseFile(filename.toStdString());
        if (ret == Model::ERR_NONE)
            currentLoadedFileType = MOD;

        addLoadedUoNModel(model);
    } else if (extention.toLower() == "stl") {
        vtkNew<vtkSTLReader> stlReader;
        stlReader->SetFileName(filename.toStdString().c_str());
        currentLoadedFileType = STL;

        addLoadedSTLModel(stlReader);
    }

    if (currentLoadedFileType == NONE) {
        emit statusUpdateMessage("File could not be opened.", 0);
    }

    LoadedModelList::getInstance().insertModel(modelName.toStdString());

    redrawModel();
    handleResetCamera();
}

void MainWindow::handleFileAdd(bool checked) {
    QString filename = QFileDialog::getOpenFileName(
        this, tr("Add Model File"), "./", tr("3D Model File (*.stl *.mod)"));

    if (filename.isEmpty())
        return;

    /* All Files SHOULD be drawn as PolyData! */
    QString extention = QFileInfo(filename).suffix();
    QString modelName = QFileInfo(filename).baseName();

    if (extention.toLower() == "mod") {
        Model model;
        int ret = model.parseFile(filename.toStdString());

        if (ret == Model::ERR_NONE)
            addLoadedUoNModel(model);
        else
            goto __fileAddError__;

    } else if (extention.toLower() == "stl") {
        vtkNew<vtkSTLReader> stlReader;

        stlReader->SetFileName(filename.toStdString().c_str());
        addLoadedSTLModel(stlReader);
    }

    LoadedModelList::getInstance().insertModel(modelName.toStdString());

    redrawModel();
    handleResetCamera();
    return;

__fileAddError__:
    emit statusUpdateMessage("File could not be added.", 0);
}

void MainWindow::handleFileSave(bool checked) {
    if (currentLoadedFileType == NONE) {
        emit statusUpdateMessage("No Model is Loaded", 0);
        return;
    }

    QString selfilter;
    QString filename = QFileDialog::getSaveFileName(
        this, tr("Save Model"), "./", tr("MOD File (*.mod);;STL File (*.stl)"),
        &selfilter);

    /* Parse selected filter */
    QRegularExpression re("\\(\\*\\.(.*)\\)");
    QString extention = re.match(selfilter).captured(1);

    if (filename.isEmpty())
        return;

    /* Add extention if required */
    if (!filename.contains(extention))
        filename += "." + extention;

    /* Get Current Model from renderer */
    // Iterate over all actors in the renderer
    vtkSmartPointer<vtkRenderer> renderer =
        ui->qvtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
    vtkSmartPointer<vtkActorCollection> actors = renderer->GetActors();

    actors->InitTraversal();
    vtkSmartPointer<vtkActor> actor;

    vtkNew<vtkAppendPolyData> appendFilter;
    vtkNew<vtkCleanPolyData> cleanFilter;
    int numberOfActors = 0;
    while ((actor = actors->GetNextActor())) {
        vtkPolyData *pd =
            vtkPolyData::SafeDownCast(actor->GetMapper()->GetInputAsDataSet());
        appendFilter->AddInputData(pd);
        numberOfActors++;
    }
    cleanFilter->SetInputConnection(appendFilter->GetOutputPort());
    cleanFilter->Update();

    if (numberOfActors == 0)
        goto __modelSaveError__;

    /* Save file */
    if (extention.toLower() == "mod") {
        Model outModel;
        generateUoNModelFromRenderer(&outModel, cleanFilter->GetOutput());
        outModel.setModelValid();
        if (outModel.saveModel(filename.toStdString())) {
            goto __modelSaveError__;
            return;
        }
    } else if (extention.toLower() == "stl") {
        vtkNew<vtkSTLWriter> stlWriter;
        stlWriter->SetFileName(filename.toStdString().c_str());
        stlWriter->SetFileTypeToASCII();
        stlWriter->SetInputConnection(cleanFilter->GetOutputPort());
        stlWriter->Write();
    }

    emit statusUpdateMessage("Model Saved to " + filename, 0);
    return;

__modelSaveError__:
    emit statusUpdateMessage("Model couldn't be saved", 0);
}

void MainWindow::handlePrint(bool checked) {
    if (currentLoadedFileType == NONE) {
        emit statusUpdateMessage("No Model is Loaded", 0);
        return;
    }

    QString selfilter;
    QString filename = QFileDialog::getSaveFileName(
        this, tr("Scene Screen Capture"), "./", tr("PNG (*.png);;JPG (*.jpg)"),
        &selfilter);

    /* Parse selected filter */
    QRegularExpression re("\\(\\*\\.(.*)\\)");
    QString extention = re.match(selfilter).captured(1);

    if (filename.isEmpty())
        return;

    // Screenshot
    // SRC:
    // https://kitware.github.io/vtk-examples/site/Cxx/Utilities/Screenshot/
    vtkNew<vtkWindowToImageFilter> windowToImageFilter;
    windowToImageFilter->SetInput(ui->qvtkWidget->GetRenderWindow());

#if VTK_MAJOR_VERSION >= 8 || VTK_MAJOR_VERSION == 8 && VTK_MINOR_VERSION >= 90
    windowToImageFilter->SetScale(2); // image quality
#else
    windowToImageFilter->SetMagnification(2); // image quality
#endif

    windowToImageFilter->SetInputBufferTypeToRGBA(); // also record the alpha
                                                     // (transparency) channel
    windowToImageFilter->ReadFrontBufferOff(); // read from the back buffer
    windowToImageFilter->Update();

    if (extention.toLower() == "png") {
        vtkNew<vtkPNGWriter> writer;
        writer->SetFileName(filename.toStdString().c_str());
        writer->SetInputConnection(windowToImageFilter->GetOutputPort());
        writer->Write();
    } else if (extention.toLower() == "jpg") {
        vtkNew<vtkJPEGWriter> writer;
        writer->SetFileName(filename.toStdString().c_str());
        writer->SetInputConnection(windowToImageFilter->GetOutputPort());
        writer->Write();
    }
}

void MainWindow::handleModelListChange(QListWidgetItem *item) {
    LoadedModelList::getInstance().changeEnabled(item);
    redrawModel();
}

void MainWindow::handleModelListDoubleClick(const QModelIndex &index) {
    EditModelDialog dialog;

    connect(&dialog, &EditModelDialog::signalRedrawScene, this,
            &MainWindow::handleRedrawModel);

    LoadedModelList &loadedModelList = LoadedModelList::getInstance();

    QVector3D &offset = loadedModelList.getOffset(index.row());
    QVector3D offsetBackup = QVector3D(offset);
    float &scale = loadedModelList.getScale(index.row());
    float scaleBackup = float(scale);

    if (!dialog.runDialog(offset, scale)) {
        loadedModelList.setOffset(index.row(), offsetBackup);
        loadedModelList.setScale(index.row(), scaleBackup);
    }

    redrawModel();
}

void MainWindow::handleRedrawModel() { redrawModel(); }
