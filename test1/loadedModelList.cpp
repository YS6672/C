#include "loadedModelList.h"
#include "ui_mainwindow.h"
#include <QListWidget>
#include <qlistwidget.h>
#include <qnamespace.h>

void LoadedModelList::Initialize(Ui::MainWindow *_ui) { ui = _ui; }

bool LoadedModelList::isEnabled(int i) { return loadedModels[i].enabled; }

void LoadedModelList::changeEnabled(QListWidgetItem *item) {
    int index = ui->modelList->row(item);
    loadedModels[index].enabled = item->checkState() == Qt::Checked;
}

void LoadedModelList::setOffset(int i, const QVector3D &offset) {
    loadedModels[i].offset = offset;
}

QVector3D &LoadedModelList::getOffset(int i) { return loadedModels[i].offset; }

float &LoadedModelList::getScale(int i) { return loadedModels[i].scale; }

void LoadedModelList::setScale(int i, float scale) {
    loadedModels[i].scale = scale;
}

void LoadedModelList::insertModel(const std::string name) {
    loadedModels.push_back(LoadedModel(name, true));

    QListWidgetItem *item = new QListWidgetItem;
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Checked);
    item->setText(QString::fromStdString(name));

    ui->modelList->addItem(item);
}

void LoadedModelList::clear() {
    for (int i = 0; i < ui->modelList->count(); ++i) {
        delete ui->modelList->item(i);
    }

    ui->modelList->clear();
}
