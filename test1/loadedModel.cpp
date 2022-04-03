#include "loadedModel.h"
#include <iostream>

LoadedModel::LoadedModel(const std::string _name, const bool _enabled)
    : name(_name), enabled(_enabled), offset(0, 0, 0), scale(1) {}

LoadedModel::LoadedModel(const LoadedModel &_LoadedModel) {
    name = _LoadedModel.name;
    enabled = _LoadedModel.enabled;
    offset = _LoadedModel.offset;
    scale = _LoadedModel.scale;
}

LoadedModel::~LoadedModel() {}

LoadedModel &LoadedModel::operator=(const LoadedModel &_LoadedModel) {
    if (this == &_LoadedModel) {
        return *this;
    }

    name = _LoadedModel.name;
    enabled = _LoadedModel.enabled;
    offset = _LoadedModel.offset;
    scale = _LoadedModel.scale;

    return *this;
}
