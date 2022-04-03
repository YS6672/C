#include "Cell.hpp"

int main() {
    Tetrahedron tet;
    tet.addVertex(Vector(-1, 2, 0));
    tet.addVertex(Vector(2, 1, -3));
    tet.addVertex(Vector(1, 0, 1));
    tet.addVertex(Vector(3, -2, 3));

    tet.setDensity(2.21);
    tet.printCellInfo();

    Pyramid pyr;
    pyr.addVertex(Vector(0, 0, 0));
    pyr.addVertex(Vector(4, 0, 0));
    pyr.addVertex(Vector(4, 4, 0));
    pyr.addVertex(Vector(0, 4, 0));
    pyr.addVertex(Vector(2, 2, 4));

    pyr.setDensity(3.37);
    pyr.printCellInfo();

    Hexahedron hex;
    hex.addVertex(Vector(0, 0, 0));
    hex.addVertex(Vector(8, 0, 0));
    hex.addVertex(Vector(8, 8, 0));
    hex.addVertex(Vector(0, 8, 0));
    hex.addVertex(Vector(0, 0, 4));
    hex.addVertex(Vector(8, 0, 4));
    hex.addVertex(Vector(8, 8, 4));
    hex.addVertex(Vector(0, 8, 4));

    hex.setDensity(4.12);
    hex.printCellInfo();

    return 0;
}
