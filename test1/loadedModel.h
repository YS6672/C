#ifndef LOADED_MODEL_H
#define LOADED_MODEL_H

#include <QVector3D>
#include <qvector3d.h>
#include <string.h>
#include <string>

class LoadedModelList;

class LoadedModel {
    friend LoadedModelList;

  public:
    LoadedModel(const std::string _name, const bool _enabled);
    LoadedModel(const LoadedModel &_LoadedModel); /* Copy Constructor */
    ~LoadedModel();                               /* Default Destructor */
    LoadedModel &operator=(const LoadedModel &_LoadedModel);

  private:
    std::string name;
    bool enabled;
    QVector3D offset;
    float scale;
};

#endif
