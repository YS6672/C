/**
 * @file Model.hpp
 * @author Christodoulos Sotiriou
 * @brief Header File: Loads a model in the proprietary UoN 3D model format
 * @version 0.1
 * @date 2021-11-15
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef MODEL_HPP
#define MODEL_HPP

#if (defined(WIN32) | defined(_WIN32)) && defined(BUILD_SHARED_LIBS)
#if defined(UoNModelLoader_EXPORTS)
#define MODELLIB_API __declspec(dllexport)
#else
#define MODELLIB_API __declspec(dllimport)
#endif
#else
#define MODELLIB_API
#endif

#include <map>
#include <string>

#include "Cell.hpp"
#include "Material.hpp"

/*
 * Model Class Overview:
 * Reads model entries from a VTK style datafile
 * Loads points and cell from the datafile and creates lists of the objects
 * Saves the loaded model in a new datafile
 * Performs the following computations
 * Displays the number of verticies
 * Displays the number and type of cells
 * Computes model's center (based on node position)
 */
class Model {
    friend class ModelRenderer;

  public:
    /* Constructors */
    MODELLIB_API Model();
    MODELLIB_API Model(std::string filename);

    MODELLIB_API ~Model(); /* Destructor */

    /* ENUMS */
    MODELLIB_API enum ERRNO {
        ERR_NONE = 0,
        ERR_FILE_NOT_FOUND,
        ERR_SYNTAX_ERROR,
        ERR_SYMBOL_NOT_FOUND,
        ERR_MODEL_NOT_LOADED
    };

    typedef std::pair<int, Vector> VertexPair;
    typedef std::map<int, Vector> VertexDictionary;

    typedef std::pair<int, Cell &> CellPair;
    typedef std::map<int, Cell &> CellDictionary;

    typedef std::pair<int, Material> MaterialPair;
    typedef std::map<int, Material> MaterialDictionary;

    /*
     * Parses a file with name <filename> as a proprietary model object. Returns
     * 0 if the parsing has been completed without error
     */
    MODELLIB_API int parseFile(const std::string &filename);

    /**
     * Saves the current loaded model to a file
     */
    MODELLIB_API int saveModel(const std::string &filename);

    /**
     * Sets the vertices of the model
     */
    MODELLIB_API void setVertices(VertexDictionary &vertices);

    /**
     * Sets the cells of the model
     */
    MODELLIB_API void setCells(CellDictionary &cells);

    /**
     * Sets the materials of the model
     */
    MODELLIB_API void setMaterials(MaterialDictionary &materials_new);

    /**
     * Print the Vertex Count of the current model
     * Requires a model to be loaded
     * Returns 0 if the value has been printed successfully
     */
    MODELLIB_API int printVertexCount();

    /**
     * Print the number of cells and the type of each one
     * Requires a model to be loaded
     * Returns 0 if the value has been printed successfully
     */
    MODELLIB_API int printCellInfo();

    /**
     * Returns the location of the model's center
     * based on the provided nodes.
     */
    MODELLIB_API Vector getModelCenter();

    MODELLIB_API Model &operator=(Model &_model);

    /**
     * Check if a model is loaded
     */
    MODELLIB_API bool isModelLoaded();

    /**
     * Set ModelLoaded to true
     */
    MODELLIB_API void setModelValid();

    /**
     * Get model cells
     */
    MODELLIB_API CellDictionary getCellDictionary();

    /**
     * Get model vertices
     */
    MODELLIB_API VertexDictionary getVertexDictionary();

    /**
     * Get model materials
     */
    MODELLIB_API MaterialDictionary getMaterialDictionary();

  private:
    bool modelLoaded;
    std::string modelFilename;

    /* Command Size Constants */
    const int MATERIAL_ARGS_COUNT = 5;
    const int VERTEX_ARGS_COUNT = 5;

  protected:
    /* Dictionaries to store the vertices, materials and cells */
    VertexDictionary vertices;
    CellDictionary cells;
    MaterialDictionary materials;
};

#endif
