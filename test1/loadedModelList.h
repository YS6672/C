#ifndef LOADED_MODEL_LIST_H
#define LOADED_MODEL_LIST_H

#include "Vector.hpp"
#include "loadedModel.h"
#include "ui_mainwindow.h"
#include <qvector3d.h>
#include <vector>

class LoadedModelList {
  public:
    static LoadedModelList &getInstance() {
        static LoadedModelList instance; // Guaranteed to be destroyed.
                                         // Instantiated on first use.
        return instance;
    }

    void Initialize(Ui::MainWindow *_ui);

    bool isEnabled(int i);
    void changeEnabled(QListWidgetItem *item);

    QVector3D &getOffset(int i);
    void setOffset(int i, const QVector3D &offset);

    float &getScale(int i);
    void setScale(int i, float scale);

    void insertModel(std::string name);
    void clear();

  private:
    LoadedModelList() {} // Constructor? (the {} brackets) are needed here.
    std::vector<LoadedModel> loadedModels;

    Ui::MainWindow *ui;

  public:
    LoadedModelList(LoadedModelList const &) = delete;
    void operator=(LoadedModelList const &) = delete;
};

#endif
