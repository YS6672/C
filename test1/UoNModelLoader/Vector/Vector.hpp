#ifndef VECTOR_HPP
#define VECTOR_HPP

#if (defined(WIN32) | defined(_WIN32)) && defined(BUILD_SHARED_LIBS)
#if defined(UoNModelLoader_EXPORTS)
#define VECTORLIB_API __declspec(dllexport)
#else
#define VECTORLIB_API __declspec(dllimport)
#endif
#else
#define VECTORLIB_API
#endif

#include <iostream>

class Vector {
  public:
    double x, y, z;
    VECTORLIB_API Vector(double x, double y, double z);
    VECTORLIB_API Vector();

    VECTORLIB_API Vector &operator=(const Vector &T);
    VECTORLIB_API double operator*(const Vector &T);
    VECTORLIB_API Vector operator*(const double n);
    VECTORLIB_API Vector operator+(const Vector &T);
    VECTORLIB_API Vector operator-(const Vector &T);
    VECTORLIB_API Vector operator^(const Vector &T);

    VECTORLIB_API void show();

    VECTORLIB_API int getId();
    VECTORLIB_API void setId(int _id);

  private:
    int id;
};

#endif
