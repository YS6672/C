#include <QColorDialog>
#include <QRegularExpression>
#include <cstdio>
#include <iostream>
#include <math.h>
#include <qcheckbox.h>
#include <qcolordialog.h>
#include <qdialog.h>
#include <qlistwidget.h>
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
#include <vtkCollectionIterator.h>
#include <vtkColor.h>
#include <vtkCubeSource.h>
#include <vtkDataSetAlgorithm.h>
#include <vtkDataSetMapper.h>
#include <vtkDelaunay3D.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkGeometryFilter.h>
#include <vtkHexahedron.h>
#include <vtkLight.h>
#include <vtkLightCollection.h>
#include <vtkLinearSubdivisionFilter.h>
#include <vtkMarchingCubes.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOutlineFilter.h>
#include <vtkPlane.h>
#include <vtkPointDataToCellData.h>
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
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridGeometryFilter.h>
#include <vtkWarpTo.h>

#include "Cell.hpp"
#include "loadedModel.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    // standard call to setup Qt UI (same as previously)
    ui->setupUi(this);

    /* SLOTS */
    connect(ui->actionFileOpen, &QAction::triggered, this,
            &MainWindow::handleFileOpen);

    connect(ui->actionFileAdd, &QAction::triggered, this,
            &MainWindow::handleFileAdd);

    connect(ui->actionFileSave, &QAction::triggered, this,
            &MainWindow::handleFileSave);

    connect(ui->actionPrint, &QAction::triggered, this,
            &MainWindow::handlePrint);

    connect(ui->modelColorBtn, &QPushButton::pressed, this,
            &MainWindow::handleChangeModelColor);

    connect(ui->backgroundColorBtn, &QPushButton::pressed, this,
            &MainWindow::handleChangeBackgroundColor);

    connect(ui->resetCameraBtn, &QPushButton::pressed, this,
            &MainWindow::handleResetCamera);

    connect(ui->lightIntensitySlide, &QSlider::valueChanged, this,
            &MainWindow::handleLightIntensityChange);

    connect(ui->shrinkChk, &QCheckBox::toggled, this,
            &MainWindow::handleShrinkFilter);

    connect(ui->shrinkSlide, &QSlider::valueChanged, this,
            &MainWindow::handleShrinkFactor);

    connect(ui->clipChk, &QCheckBox::toggled, this,
            &MainWindow::handleClipFilter);

    connect(ui->boundingBoxChk, &QCheckBox::toggled, this,
            &MainWindow::handleBoundingBoxFilter);

    /* Warp Filter */
    connect(ui->warpChk, &QCheckBox::toggled, this,
            &MainWindow::handleWarpFilter);
    connect(ui->warpXslide, &QSlider::valueChanged, this,
            &MainWindow::handleWarpXslide);
    connect(ui->warpYslide, &QSlider::valueChanged, this,
            &MainWindow::handleWarpYslide);
    connect(ui->warpZslide, &QSlider::valueChanged, this,
            &MainWindow::handleWarpZslide);

    connect(ui->modelList, &QListWidget::itemChanged, this,
            &MainWindow::handleModelListChange);

    connect(ui->modelList, &QListWidget::doubleClicked, this,
            &MainWindow::handleModelListDoubleClick);

    /* SIGNALS */
    connect(this, &MainWindow::statusUpdateMessage, ui->statusbar,
            &QStatusBar::showMessage);

    shrinkFactor = (float)ui->shrinkSlide->value() / 100.0;

    // Now need to create a VTK render window and link it to the QtVTK
    // widget
    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
    ui->qvtkWidget->SetRenderWindow(renderWindow);
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow->AddRenderer(renderer);

    vtkSmartPointer<vtkLight> light = vtkSmartPointer<vtkLight>::New();
    light->SetLightTypeToCameraLight();
    light->SetConeAngle(160);
    light->SetFocalPoint(0, 0, 0);
    light->SetDiffuseColor(1, 1, 1);
    light->SetAmbientColor(1, 1, 1);
    light->SetSpecularColor(1, 1, 1);
    light->SetIntensity(lightIntensity);

    renderer->AddLight(light);

    modelColor.setRgb(255, 255, 255);
    backgroundColor.setRgb(192, 192, 192);

    LoadedModelList::getInstance().Initialize(ui);

    redrawModel();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::redrawModel() {
    /* Clean Window */
    vtkSmartPointer<vtkRenderer> renderer =
        ui->qvtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
    vtkSmartPointer<vtkNamedColors> colors =
        vtkSmartPointer<vtkNamedColors>::New();

    renderer->RemoveAllViewProps();
    vtkNew<vtkDataSetMapper> mapper;

    if (currentLoadedFileType == NONE) {
        vtkNew<vtkTextActor> txt;
        txt->SetInput("Model Not Loaded");
        vtkTextProperty *txtprop = txt->GetTextProperty();
        txtprop->SetFontFamilyToArial();
        txtprop->BoldOn();
        txtprop->SetFontSize(36);
        txtprop->ShadowOn();
        txtprop->SetShadowOffset(4, 4);
        txt->SetDisplayPosition(20, 30);
        renderer->AddActor(txt);
    }

    /* Filters */
    if (currentLoadedFileType != NONE) {
        vtkNew<vtkAppendFilter> appendFilter;
        vtkNew<vtkGeometryFilter> geometryFilter;
        vtkNew<vtkCleanPolyData> cleanFilter;

        LoadedModelList &loadedModelList = LoadedModelList::getInstance();

        globalPD->InitTraversal();
        vtkPolyData *pd;
        int index = 0;
        int visibleCount = 0;

        while ((pd = globalPD->GetNextItem())) {
            if (loadedModelList.isEnabled(index)) {
                QVector3D offset = loadedModelList.getOffset(index);
                vtkNew<vtkTransform> transform;
                transform->Translate(offset.x(), offset.y(), offset.z());
                transform->Scale(loadedModelList.getScale(index),
                                 loadedModelList.getScale(index),
                                 loadedModelList.getScale(index));

                vtkNew<vtkTransformPolyDataFilter> transformFilter;
                transformFilter->SetTransform(transform);

                transformFilter->SetInputData(pd);
                appendFilter->AddInputConnection(
                    transformFilter->GetOutputPort());

                visibleCount++;
            }
            index++;
        }
        if (visibleCount != 0) {

            geometryFilter->SetInputConnection(appendFilter->GetOutputPort());
            cleanFilter->SetInputConnection(geometryFilter->GetOutputPort());

            mapper->SetInputConnection(cleanFilter->GetOutputPort());
            vtkNew<vtkActor> actor;

            if (clipFilterEnabled) {
                // this will apply a clipping plane whose normal is the x-axis
                // that crosses the x-axis at x=0
                vtkNew<vtkGeometryFilter> geometryFilter;

                auto planeLeft = vtkSmartPointer<vtkPlane>::New();
                planeLeft->SetOrigin(0.0, 0.0, 0.0);
                planeLeft->SetNormal(-1.0, 0.0, 0.0);
                auto clipFilter = vtkSmartPointer<vtkClipDataSet>::New();
                clipFilter->SetInputConnection(
                    mapper->GetInputAlgorithm()->GetOutputPort(0));
                clipFilter->SetClipFunction(planeLeft.Get());
                geometryFilter->SetInputConnection(clipFilter->GetOutputPort());
                mapper->SetInputConnection(geometryFilter->GetOutputPort());
            }
            if (shrinkFilterEnabled) {
                vtkNew<vtkShrinkFilter> shrinkFilter;
                shrinkFilter->SetShrinkFactor(shrinkFactor);
                vtkNew<vtkGeometryFilter> geometryFilter;

                shrinkFilter->SetInputConnection(
                    mapper->GetInputAlgorithm()->GetOutputPort(0));
                geometryFilter->SetInputConnection(
                    shrinkFilter->GetOutputPort());
                mapper->SetInputConnection(geometryFilter->GetOutputPort());
            }
            if (warpFilterEnabled) {
                vtkNew<vtkWarpTo> warpFilter;
                warpFilter->SetInputConnection(
                    mapper->GetInputAlgorithm()->GetOutputPort(0));
                warpFilter->SetPosition(warpFilterX, warpFilterY, warpFilterZ);
                warpFilter->AbsoluteOn();
                mapper->SetInputConnection(warpFilter->GetOutputPort());
            }
            if (boundingBoxFilterEnabled) {
                vtkNew<vtkPolyDataMapper> boundingBoxMapper;

                boundingBoxMapper->SetInputConnection(
                    mapper->GetInputAlgorithm()->GetOutputPort(0));

                auto outlineFilter = vtkSmartPointer<vtkOutlineFilter>::New();

                outlineFilter->SetInputConnection(
                    boundingBoxMapper->GetInputAlgorithm()->GetOutputPort(0));
                boundingBoxMapper->SetInputConnection(
                    0, outlineFilter->GetOutputPort());

                vtkNew<vtkActor> boundingBoxActor;
                boundingBoxActor->SetMapper(boundingBoxMapper);
                renderer->AddActor(boundingBoxActor);
            }

            actor->SetMapper(mapper);
            renderer->AddActor(actor);
        }
    }

    ui->qvtkWidget->GetRenderWindow()->StereoUpdate();

    renderer->SetBackground(backgroundColor.redF(), backgroundColor.greenF(),
                            backgroundColor.blueF());

    ui->qvtkWidget->GetRenderWindow()->Render();

    changeModelColor();
    updateModelInfo();
}

void MainWindow::addLoadedUoNModel(Model &model) {
    Model::CellDictionary cells = model.getCellDictionary();
    Model::VertexDictionary vertices = model.getVertexDictionary();

    vtkNew<vtkPoints> points;

    for (const auto &iter : vertices) {
        int id = iter.first;
        Vector vec = iter.second;

        points->InsertPoint(vec.getId(), vec.x, vec.y, vec.z);
    }

    vtkNew<vtkUnstructuredGrid> ug;
    vtkNew<vtkCellArray> vtkCells;

    ug->SetPoints(points);

    for (const auto &iter : cells) {
        int id = iter.first;
        Cell &cell = iter.second;

        if (cell.VERTEX_COUNT() == 4) { /* Tetrahedral */
            vtkNew<vtkTetra> tetra;

            int i = 0;
            for (auto vec : cell.getVertices()) {
                tetra->GetPointIds()->SetId(i, vec.getId());
                i++;
            }
            vtkCells->InsertNextCell(tetra);
            ug->InsertNextCell(tetra->GetCellType(), tetra->GetPointIds());
        } else if (cell.VERTEX_COUNT() == 5) { /* Pyramid */
            vtkNew<vtkPyramid> pyramid;

            int i = 0;
            for (auto vec : cell.getVertices()) {
                pyramid->GetPointIds()->SetId(i, vec.getId());
                i++;
            }
            vtkCells->InsertNextCell(pyramid);
            ug->InsertNextCell(pyramid->GetCellType(), pyramid->GetPointIds());
        } else if (cell.VERTEX_COUNT() == 8) { /* Hexahedron */
            vtkNew<vtkHexahedron> hexahedron;

            int i = 0;
            for (auto vec : cell.getVertices()) {
                hexahedron->GetPointIds()->SetId(i, vec.getId());
                i++;
            }
            vtkCells->InsertNextCell(hexahedron);
            ug->InsertNextCell(hexahedron->GetCellType(),
                               hexahedron->GetPointIds());
        }
    }

    /* Convert ug -> pd */
    vtkNew<vtkGeometryFilter> geometryFilter;
    vtkNew<vtkTriangleFilter> triangleFilter;
    vtkNew<vtkCleanPolyData> cleanFilter;

    geometryFilter->SetInputData(ug);
    triangleFilter->SetInputConnection(geometryFilter->GetOutputPort());

    cleanFilter->SetInputConnection(triangleFilter->GetOutputPort());

    cleanFilter->Update();
    globalPD->AddItem(cleanFilter->GetOutput());
}

void MainWindow::addLoadedSTLModel(vtkSTLReader *stlReader) {
    stlReader->Update();
    vtkNew<vtkPolyData> pd;
    pd->DeepCopy(stlReader->GetOutput());
    globalPD->AddItem(pd);
}

void MainWindow::generateUoNModelFromRenderer(Model *model, vtkPolyData *pd) {
    /* Generate Tetrahedral mesh */
    vtkNew<vtkDelaunay3D> delaunay3D;

    delaunay3D->SetInputData(pd);
    delaunay3D->Update();

    vtkUnstructuredGrid *ug = delaunay3D->GetOutput();

    vtkPoints *points = ug->GetPoints();

    Model::VertexDictionary modVertices;
    Model::CellDictionary modCells;
    Model::MaterialDictionary modMaterials;

    /* Add generic plastic material */
    Material material(0, "PLA", 1250);
    int r, g, b;
    modelColor.getRgb(&r, &g, &b);
    material.col.setcolour((uint8_t)r, (uint8_t)g, (uint8_t)b);
    modMaterials.insert(Model::MaterialPair(0, material));

    /* Loop through points */
    for (int i = 0; i < points->GetNumberOfPoints(); i++) {
        double point[3];
        points->GetPoint(i, point);
        Vector v(point[0], point[1], point[2]);
        v.setId(i);
        modVertices.insert(Model::VertexPair(i, v));
    }

    for (int i = 0; i < ug->GetNumberOfCells(); i++) {
        vtkCell *cell = ug->GetCell(i);
        int vertexCount = cell->GetNumberOfPoints();

        Cell *modCell;
        switch (vertexCount) {
        case 4:
            modCell = new Tetrahedron();
            break;
        case 5:
            modCell = new Pyramid();
            break;
        case 8:
            modCell = new Hexahedron();
            break;
        default:
            return;
        }

        for (int i = 0; i < vertexCount; i++) {
            modCell->addVertex(modVertices.at(cell->GetPointId(i)));
        }

        modCell->setMaterialId(0);
        modCells.insert(Model::CellPair(i, *modCell));
    }

    model->setVertices(modVertices);
    model->setCells(modCells);
    model->setMaterials(modMaterials);
}

void MainWindow::changeModelColor() {
    vtkSmartPointer<vtkRenderer> renderer =
        ui->qvtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

    vtkActorCollection *actorCollection = renderer->GetActors();
    vtkActor *actor;
    actorCollection->InitTraversal();
    while (actor = actorCollection->GetNextActor()) {
        actor->GetProperty()->SetColor(modelColor.redF(), modelColor.greenF(),
                                       modelColor.blueF());
    }

    ui->qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::updateModelInfo() {
    if (currentLoadedFileType == NONE) {
        ui->sizelbl->setText("Size: ");
        ui->verticeslbl->setText("#Vertices:");
        ui->cellslbl->setText("#Cells:");
    } else {
        vtkSmartPointer<vtkRenderer> renderer =
            ui->qvtkWidget->GetRenderWindow()
                ->GetRenderers()
                ->GetFirstRenderer();
        vtkSmartPointer<vtkActorCollection> actors = renderer->GetActors();

        actors->InitTraversal();
        vtkSmartPointer<vtkActor> actor;

        vtkNew<vtkAppendPolyData> appendFilter;
        int numberOfActors = 0;
        while ((actor = actors->GetNextActor())) {
            vtkPolyData *pd = vtkPolyData::SafeDownCast(
                actor->GetMapper()->GetInputAsDataSet());
            appendFilter->AddInputData(pd);
            numberOfActors++;
        }
        appendFilter->Update();

        vtkPolyData *pd = appendFilter->GetOutput();
        double *bounds = pd->GetBounds();

        QString sizeTxt = QString("Size: %1 x %2 x %3")
                              .arg(abs(bounds[0] - bounds[1]), 0, 'f', 2)
                              .arg(abs(bounds[2] - bounds[3]), 0, 'f', 2)
                              .arg(abs(bounds[4] - bounds[5]), 0, 'f', 2);

        ui->sizelbl->setText(sizeTxt);
        ui->verticeslbl->setText("#Vertices: " +
                                 QString::number(pd->GetNumberOfPoints()));
        ui->cellslbl->setText("#Cells: " +
                              QString::number(pd->GetNumberOfCells()));
    }
}
