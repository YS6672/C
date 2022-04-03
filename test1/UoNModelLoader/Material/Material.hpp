#ifndef MATERIAL_H_INCLUDED
#define MATERIAL_H_INCLUDED

#if (defined(WIN32) | defined(_WIN32)) && defined(BUILD_SHARED_LIBS)
#if defined(UoNModelLoader_EXPORTS)
#define MATERIALLIB_API __declspec(dllexport)
#else
#define MATERIALLIB_API __declspec(dllimport)
#endif
#else
#define MATERIALLIB_API
#endif

#include <iostream>
#include <string>

using namespace std;

class Colour {
  protected:
    unsigned char r, g, b;

  public:
    MATERIALLIB_API Colour();
    MATERIALLIB_API Colour(const Colour &c);
    MATERIALLIB_API Colour &operator=(const Colour &c);

    MATERIALLIB_API void setcolour(unsigned char r, unsigned char g,
                                   unsigned char b);
    MATERIALLIB_API void setcolour(float r, float g, float b);
    MATERIALLIB_API void setcolour(unsigned int rgb);

    MATERIALLIB_API unsigned int getColour();

    MATERIALLIB_API void HexadecimalToRGB(string hex);
    MATERIALLIB_API ~Colour();
};

class Material {
  private:
    unsigned int ID;
    double Density;
    string Name;

  public:
    MATERIALLIB_API Material(int _ID, string _Name, double _Density);
    MATERIALLIB_API Material(const Material &m);
    MATERIALLIB_API Material &operator=(const Material &m);

    MATERIALLIB_API void setID(unsigned int ID);
    MATERIALLIB_API void setDensity(double ID);
    MATERIALLIB_API void setName(string Name);

    MATERIALLIB_API unsigned int getID();
    MATERIALLIB_API double getDensity();
    MATERIALLIB_API string getName();

    Colour col;

    MATERIALLIB_API ~Material();
};

#endif // MATERIAL_H_INCLUDED
