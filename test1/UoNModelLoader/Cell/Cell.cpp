#include "Cell.hpp"

#include <iostream>

/* Parent Cell Functions */
Cell::Cell() {}

void Cell::addVertex(Vector v) { verticies.push_back(v); }

Vector Cell::getVertex(int index) { return verticies[index]; }

std::vector<Vector> Cell::getVertices() { return verticies; }

// Sets the density
void Cell::setDensity(double den) { density = den; }

// Returns the density
double Cell::getDensity() { return density; }

// Prints and returns the weight
double Cell::getWeight() {
    double mass = density * getVolume(); // Mass = Density * Volume
    const double gfs = 9.81;             // Gravitational Field Strength
    weight = mass * gfs; // Weight = Mass * Gravitational Field Strength
    return weight;
}

// Prints and returns the centre of gravity
void Cell::calculateCentreOfGravity() {
    centre = centre * 0;
    for (Vector vertex : verticies) {
        centre = centre + vertex;
    }

    centre = centre * (1.0 / verticies.size());
}

void Cell::printCellInfo() {
    std::cout << "vertex count: " << VERTEX_COUNT() << std::endl;
    std::cout << "density: " << getDensity() << std::endl;
    std::cout << "weight: " << getWeight() << std::endl;
    std::cout << "volume: " << getVolume() << std::endl;
    for (Vector vertex : verticies) {
        vertex.show();
    }
}

void Cell::setMaterialId(int _materialId) { materialId = _materialId; }
int Cell::getMaterialId() { return materialId; }

// Returns the volume
Tetrahedron::Tetrahedron() {}
Tetrahedron::~Tetrahedron() {}

int Tetrahedron::VERTEX_COUNT() const { return 4; }

double Tetrahedron::getVolume() {
    Vector AB(verticies[1] - verticies[0]);
    Vector AC(verticies[2] - verticies[0]);
    Vector AD(verticies[3] - verticies[0]);

    double volume =
        ((AB.x * AC.y * AD.z) + (AB.y * AC.z * AD.x) + (AB.z * AC.x * AD.y) -
         (AB.z * AC.y * AD.x) - (AB.y * AC.x * AD.z) - (AB.x * AC.z * AD.y));
    // If statement below returns absolute value
    if (volume < 0) {
        volume = volume * -1;
    } else {
    }
    volume = volume * (1.0 / 6.0);
    return volume;
}

// Returns the volume
Pyramid::Pyramid() {}
Pyramid::~Pyramid() {}

int Pyramid::VERTEX_COUNT() const { return 5; };

double Pyramid::getVolume() {
    Vector AB(verticies[1] - verticies[0]);
    Vector AC(verticies[2] - verticies[0]);
    Vector AD(verticies[3] - verticies[0]);
    Vector AE(verticies[4] - verticies[0]);

    // Half the volume of the pyramid, calculated from vectors AB, AC, AE
    double halfVolume1 = ((AB.x * AC.y * AE.z) + (AB.y * AC.z * AE.x) +

                          (AB.z * AC.x * AE.y) - (AB.z * AC.y * AE.x) -

                          (AB.y * AC.x * AE.z) - (AB.x * AC.z * AE.y));

    // Half the volume of the pyramid, calculated from vectors AC, AD, AE */
    double halfVolume2 = ((AC.x * AD.y * AE.z) + (AC.y * AD.z * AE.x) +

                          (AC.z * AD.x * AE.y) - (AC.z * AD.y * AE.x) -

                          (AC.y * AD.x * AE.z) - (AC.x * AD.z * AE.y));

    // If statement returns absolute value */
    if (halfVolume1 < 0) {
        halfVolume1 = halfVolume1 * -1;
    } else {
    }

    halfVolume1 = halfVolume1 * (1.0 / 6.0);
    // If statement returns absolute value
    if (halfVolume2 < 0) {
        halfVolume2 = halfVolume2 * -1;
    } else {
    }

    halfVolume2 = halfVolume2 * (1.0 / 6.0);

    double volume = halfVolume1 + halfVolume2;

    return volume;
}

Hexahedron::Hexahedron() {}
Hexahedron::~Hexahedron() {}

// Hexahedron volume function
double Hexahedron::tetVolume(double x1, double y1, double z1, double x2,
                             double y2, double z2, double x3, double y3,
                             double z3) {
    double volume = ((x1 * y2 * z3) + (y1 * z2 * x3) + (z1 * x2 * y3) -
                     (z1 * y2 * x3) - (y1 * x2 * z3) - (x1 * z2 * y3));
    // If statement below returns absolute value
    if (volume < 0) {
        volume = volume * -1;
    } else {
    }
    volume = volume * (1.0 / 6.0);
    return volume;
}

// Returns the volume
double Hexahedron::getVolume() {
    Vector AO(centre - verticies[0]);
    Vector AB(verticies[1] - verticies[0]);
    Vector AC(verticies[2] - verticies[0]);
    Vector AD(verticies[3] - verticies[0]);
    Vector AE(verticies[4] - verticies[0]);
    Vector AF(verticies[5] - verticies[0]);
    Vector AH(verticies[7] - verticies[0]);

    Vector CO(centre - verticies[2]);
    Vector CB(verticies[1] - verticies[2]);
    Vector CF(verticies[5] - verticies[2]);
    Vector CG(verticies[6] - verticies[2]);

    Vector DO(centre - verticies[3]);
    Vector DC(verticies[2] - verticies[3]);
    Vector DG(verticies[6] - verticies[3]);
    Vector DH(verticies[7] - verticies[3]);

    Vector EO(centre - verticies[4]);
    Vector EF(verticies[5] - verticies[4]);
    Vector EG(verticies[6] - verticies[4]);
    Vector EH(verticies[7] - verticies[4]);

    // Bottom pyramid
    // Vectors AC AD AO
    double var1 = tetVolume(AC.x, AC.y, AC.z, AD.x, AD.y, AD.z, AO.x,

                            AO.y, AO.z);
    // Vectors AB AC AO
    double var2 = tetVolume(AB.x, AB.y, AB.z, AC.x, AC.y, AC.z, AO.x,

                            AO.y, AO.z);
    // Top pyramid
    // Vectors EG EH EO
    double var3 = tetVolume(EG.x, EG.y, EG.z, EH.x, EH.y, EH.z, EO.x,

                            EO.y, EO.z);
    // Vectors EF EG EO
    double var4 = tetVolume(EF.x, EF.y, EF.z, EG.x, EG.y, EG.z, EO.x,

                            EO.y, EO.z);
    // Front pyramid
    // Vectors AH AE AO
    double var5 = tetVolume(AH.x, AH.y, AH.z, AE.x, AE.y, AE.z, AO.x,

                            AO.y, AO.z);
    // Vectors AD AH AO
    double var6 = tetVolume(AD.x, AD.y, AD.z, AH.x, AH.y, AH.z, AO.x,

                            AO.y, AO.z);
    // Back pyramid
    // Vectors CF CG CO
    double var7 = tetVolume(CF.x, CF.y, CF.z, CG.x, CG.y, CG.z, CO.x,

                            CO.y, CO.z);
    // Vectors CB CF CO
    double var8 = tetVolume(CB.x, CB.y, CB.z, CF.x, CF.y, CF.z, CO.x,

                            CO.y, CO.z);
    // Left pyramid
    // Vectors DG DH DO
    double var9 = tetVolume(DG.x, DG.y, DG.z, DH.x, DH.y, DH.z, DO.x,

                            DO.y, DO.z);
    // Vectors DC DG DO
    double var10 = tetVolume(DC.x, DC.y, DC.z, DG.x, DG.y, DG.z, DO.x,

                             DO.y, DO.z);
    // Right pyramid
    // Vectors AF AE AO
    double var11 = tetVolume(AF.x, AF.y, AF.z, AE.x, AE.y, AE.z, AO.x,

                             AO.y, AO.z);
    // Vectors AB AF AO
    double var12 = tetVolume(AB.x, AB.y, AB.z, AF.x, AF.y, AF.z, AO.x,

                             AO.y, AO.z);
    double volume = var1 + var2 + var3 + var4 + var5 + var6 + var7 + var8 +

                    var9 + var10 + var11 + var12;
    return volume;
}

int Hexahedron::VERTEX_COUNT() const { return 8; };
