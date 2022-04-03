/**
 * @file Model.cpp
 * @author Christodoulos Sotiriou
 * @brief Source File: Loads a model in the proprietary UoN 3D model format
 * @version 0.1
 * @date 2021-11-15
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "Cell.hpp"
#include "Material.hpp"
#include "Model.hpp"

#include <cctype>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdio.h> /* sscanf */
#include <vector>

/* Constructors */
/* Basic */
Model::Model() { modelLoaded = false; }
/* Open and parse file */
Model::Model(std::string filename) : Model() { parseFile(filename); }

Model::~Model() {
    /* Free dynamic allocations */
    for (CellDictionary::iterator iter = cells.begin(); iter != cells.end();
         ++iter) {
        delete &iter->second;
    }
}

void Model::setVertices(VertexDictionary &vertices_new) {
    vertices = vertices_new;
}

void Model::setCells(CellDictionary &cells_new) { cells = cells_new; }

void Model::setMaterials(MaterialDictionary &materials_new) {
    materials = materials_new;
}

/* File Parse Helpers */
/***********************************
 * Stripping string (like python)
 * Source:
 *https://stackoverflow.com/questions/9358718/similar-function-in-c-to-pythons-strip
 ***********************************/
template <std::ctype_base::mask mask> class IsNot {
    std::locale myLocale; // To ensure lifetime of facet...
    std::ctype<char> const *myCType;

  public:
    IsNot(std::locale const &l = std::locale())
        : myLocale(l), myCType(&std::use_facet<std::ctype<char>>(l)) {}
    bool operator()(char ch) const { return !myCType->is(mask, ch); }
};

typedef IsNot<std::ctype_base::space> IsNotSpace;

std::string stripString(std::string const &original) {
    std::string::const_iterator right =
        std::find_if(original.rbegin(), original.rend(), IsNotSpace()).base();
    std::string::const_iterator left =
        std::find_if(original.begin(), right, IsNotSpace());
    return std::string(left, right);
}

/***********************************
 * Split a string based on a delimeter
 * Source:
 *https://stackoverflow.com/questions/236129/how-do-i-iterate-over-the-words-of-a-string
 ***********************************/
template <typename Out>
void splitString(const std::string &s, char delim, Out result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        *result++ = item;
    }
}

std::vector<std::string> splitString(const std::string &s, char delim) {
    std::vector<std::string> elems;
    splitString(s, delim, std::back_inserter(elems));
    return elems;
}

int Model::parseFile(const std::string &filename) {
    modelLoaded = false;

    /* Clear Dictrionaries */
    cells.clear();
    vertices.clear();
    materials.clear();

    /* Open file */
    std::ifstream datafile(filename, std::ios::in);

    /* Check if file has been opened successfully */
    if (!datafile.is_open())
        return ERR_FILE_NOT_FOUND;

    /* Parse Line by line */
    std::string currentLine;
    while (std::getline(datafile, currentLine)) {

        /* Remove whitespaces from start and end */
        currentLine = stripString(currentLine);

        /* Skip Empty Lines */
        if (currentLine.length() == 0)
            continue;

        const char command = currentLine[0];
        if (command == '#') { /* Comment */
            continue;
        } else if (command == 'm') { /* material */

            std::vector<std::string> args = splitString(currentLine, ' ');
            if (args.size() != MATERIAL_ARGS_COUNT)
                return ERR_SYNTAX_ERROR;

            /* Arg0 command name (ignore) */
            /* Arg1 ID */
            unsigned int id;
            if (sscanf(args[1].c_str(), "%u", &id) != 1)
                return ERR_SYNTAX_ERROR;
            /* Arg2 Density */
            int density;
            if (sscanf(args[2].c_str(), "%d", &density) != 1)
                return ERR_SYNTAX_ERROR;
            /* Arg3 Color (HEX) */
            int color;
            if (sscanf(args[3].c_str(), "%x", &color) != 1)
                return ERR_SYNTAX_ERROR;
            /* Arg4 Name */

            Material material(id, args[4], (double)density);
            material.col.setcolour(color);

            materials.insert(MaterialPair(id, material));

        } else if (command == 'v') { /* vertex */

            std::vector<std::string> args = splitString(currentLine, ' ');
            if (args.size() != VERTEX_ARGS_COUNT)
                return ERR_SYNTAX_ERROR;

            /* Arg0 command name (ignore) */
            /* Arg1 ID */
            int id;
            if (sscanf(args[1].c_str(), "%d", &id) != 1)
                return ERR_SYNTAX_ERROR;
            /* Arg2 x coord (float) */
            /* Arg3 y coord (float) */
            /* Arg4 z coord (float) */
            float vertex_x, vertex_y, vertex_z;
            if (sscanf(args[2].c_str(), "%f", &vertex_x) != 1)
                return ERR_SYNTAX_ERROR;
            if (sscanf(args[3].c_str(), "%f", &vertex_y) != 1)
                return ERR_SYNTAX_ERROR;
            if (sscanf(args[4].c_str(), "%f", &vertex_z) != 1)
                return ERR_SYNTAX_ERROR;

            Vector vertex(vertex_x, vertex_y, vertex_z);
            vertex.setId(id);

            vertices.insert(VertexPair(id, vertex));

        } else if (command == 'c') { /* cell */

            std::vector<std::string> args = splitString(currentLine, ' ');
            if (args.size() < 4)
                return ERR_SYNTAX_ERROR;

            int args_count_expected = 4; /* cmd, id, type, meterial */

            if (args[2] == "h") { /* hexahedral */
                args_count_expected += 8;
            } else if (args[2] == "p") { /* pyramid */
                args_count_expected += 5;
            } else if (args[2] == "t") { /* tetrahedral */
                args_count_expected += 4;
            } else {
                return ERR_SYNTAX_ERROR;
            }

            if (args.size() != args_count_expected)
                return ERR_SYNTAX_ERROR;

            int id;
            if (sscanf(args[1].c_str(), "%d", &id) != 1)
                return ERR_SYNTAX_ERROR;

            int materialId;
            if (sscanf(args[3].c_str(), "%d", &materialId) != 1)
                return ERR_SYNTAX_ERROR;

            auto it = materials.find(materialId);
            if (it == materials.end())
                return ERR_SYNTAX_ERROR;

            Material material = it->second;

            Cell *cell = nullptr;
            if (args[2] == "h") { /* hexahedral */
                cell = new Hexahedron();
            } else if (args[2] == "p") { /* pyramid */
                cell = new Pyramid();
            } else if (args[2] == "t") { /* tetrahedral */
                cell = new Tetrahedron();
            }

            cell->setDensity(material.getDensity());
            cell->setMaterialId(materialId);
            for (int i = 4; i < args_count_expected; i++) {
                int currentVertexID;
                if (sscanf(args[i].c_str(), "%d", &currentVertexID) != 1) {
                    delete cell;
                    return ERR_SYNTAX_ERROR;
                }

                auto it = vertices.find(currentVertexID);
                if (it == vertices.end()) {
                    delete cell;
                    return ERR_SYNTAX_ERROR;
                }

                cell->addVertex(it->second);
            }

            cells.insert(CellPair(id, *cell));

        } else { /* Unknown Command */
            return ERR_SYNTAX_ERROR;
        }
    }

    modelFilename = filename;
    modelLoaded = true;
    return ERR_NONE;
}

int Model::saveModel(const std::string &filename) {
    if (!modelLoaded)
        return ERR_MODEL_NOT_LOADED;

    /* Open file to write */
    std::ofstream outfile(filename, std::ios::out);

    if (!outfile.is_open())
        return ERR_FILE_NOT_FOUND;

    outfile << "###### Generated with UoN Model Library ######\n\n";

    /* 1. Write materials */
    outfile << "###### Materials ######\n";
    for (MaterialDictionary::iterator iter = materials.begin();
         iter != materials.end(); ++iter) {
        int id = iter->first;
        Material &material = iter->second;

        outfile << "m " << id << " " << material.getDensity() << " " << std::hex
                << material.col.getColour() << std::dec << " "
                << material.getName() << std::endl;
    }

    /* 2. Write vertices */
    outfile << "\n###### Vertices ######\n";
    for (VertexDictionary::iterator iter = vertices.begin();
         iter != vertices.end(); ++iter) {
        int id = iter->first;
        Vector &vertex = iter->second;

        outfile << "v " << id << " " << vertex.x << " " << vertex.y << " "
                << vertex.z << std::endl;
    }

    /* 3. Write cells */
    outfile << "\n###### Cells ######\n";
    for (CellDictionary::iterator iter = cells.begin(); iter != cells.end();
         ++iter) {
        int id = iter->first;
        Cell &cell = iter->second;

        outfile << "c " << id << " ";

        switch (cell.VERTEX_COUNT()) {
        case 4:
            outfile << "t ";
            break;
        case 5:
            outfile << "p ";
            break;
        case 8:
            outfile << "h ";
            break;
        }

        outfile << cell.getMaterialId() << " ";

        for (Vector vertex : cell.getVertices()) {
            outfile << vertex.getId() << " ";
        }
        outfile << std::endl;
    }

    outfile.close();

    return ERR_NONE;
}

int Model::printVertexCount() {
    if (modelLoaded) {
        std::cout << "Model " << modelFilename
                  << " Vertex Count: " << vertices.size() << std::endl;
        return vertices.size();
    } else {
        std::cerr << "No model has been loaded\n";
        return 0;
    }
}

int Model::printCellInfo() {
    if (modelLoaded) {

        std::cout << "Model " << modelFilename
                  << " Cell Count: " << cells.size() << std::endl;
        /* Source:
         * https://stackoverflow.com/questions/1443793/iterate-keys-in-a-c-map
         */
        for (CellDictionary::iterator iter = cells.begin(); iter != cells.end();
             ++iter) {
            int id = iter->first;
            Cell &cell = iter->second;
            std::cout << "Cell #" << id << std::endl;
            iter->second.printCellInfo();
        }

        return 0;

    } else {
        std::cerr << "No model has been loaded\n";
        return 1;
    }
}

Vector Model::getModelCenter() {
    Vector vec(0, 0, 0);

    int totalVertices = 0;
    /* Loop through all vertices and calculate the "average" vertex position */
    for (VertexDictionary::iterator iter = vertices.begin();
         iter != vertices.end(); iter++) {

        totalVertices++;
        vec = vec + iter->second;
    }

    return vec * (1.0f / (float)totalVertices);
}

Model &Model::operator=(Model &_model) { return *this; }

bool Model::isModelLoaded() { return modelLoaded; }

void Model::setModelValid() { modelLoaded = true; }

/**
 * Get model cells
 */
Model::CellDictionary Model::getCellDictionary() { return cells; }

/**
 * Get model vertices
 */
Model::VertexDictionary Model::getVertexDictionary() { return vertices; }

/**
 * Get model materials
 */
Model::MaterialDictionary Model::getMaterialDictionary() { return materials; }
