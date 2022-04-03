#ifndef CELL_HPP
#define CELL_HPP

#if (defined(WIN32) | defined(_WIN32)) && defined(BUILD_SHARED_LIBS)
#if defined(UoNModelLoader_EXPORTS)
#define CELLLIB_API __declspec(dllexport)
#else
#define CELLLIB_API __declspec(dllimport)
#endif
#else
#define CELLLIB_API
#endif

#include "Vector.hpp"

#include <iostream>
#include <vector>

class Cell {
  public:
    CELLLIB_API Cell();
    virtual ~Cell() {}

    CELLLIB_API void addVertex(Vector v);
    CELLLIB_API Vector getVertex(int index);

    CELLLIB_API std::vector<Vector> getVertices();

    CELLLIB_API void setDensity(double den); // Sets the density
    CELLLIB_API double getDensity();         // Returns the density

    // Prints and returns the weight
    CELLLIB_API double getWeight();

    // Prints and returns the centre of gravity
    CELLLIB_API void calculateCentreOfGravity();

    // Prints cell info
    CELLLIB_API void printCellInfo();

    CELLLIB_API void setMaterialId(int _materialId);
    CELLLIB_API int getMaterialId();

    // Returns the volume
    virtual double getVolume() = 0;

    virtual int VERTEX_COUNT() const = 0;

  protected:
    std::vector<Vector> verticies;
    Vector centre;  // The x,y,z coordinates of the centre of gravity;
    double density; // Density of the material
    double weight;  // Weight of the cell
    int materialId;
};

class Tetrahedron : public Cell {
  public:
    CELLLIB_API Tetrahedron();  // Constructor
    CELLLIB_API ~Tetrahedron(); // Destructor

    // Returns the volume
    CELLLIB_API double getVolume();

    CELLLIB_API int VERTEX_COUNT() const;
};

class Pyramid : public Cell {
  public:
    CELLLIB_API Pyramid();  // Constructor
    CELLLIB_API ~Pyramid(); // Destructor

    // Returns the volume
    CELLLIB_API double getVolume();

    CELLLIB_API int VERTEX_COUNT() const;
};

class Hexahedron : public Cell {
  public:
    CELLLIB_API Hexahedron();  // Constructor
    CELLLIB_API ~Hexahedron(); // Destructor

    // Returns the volume
    CELLLIB_API double getVolume();

    CELLLIB_API int VERTEX_COUNT() const;

  private:
    double tetVolume(double x1, double y1, double z1, double x2, double y2,
                     double z2, double x3, double y3, double z3);
};

#endif
