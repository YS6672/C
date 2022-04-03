#include "Vector.hpp"
#include <iostream>

void Vector::show() { // display
    std::cout << "(" << x << ", " << y << ", " << z << ")" << std::endl;
}

Vector::Vector() : x(0), y(0), z(0) {}
Vector::Vector(double x, double y, double z) : x(x), y(y), z(z) {}

Vector &Vector::operator=(const Vector &T) {
    if (this == &T)
        return *this;

    this->x = T.x;
    this->y = T.y;
    this->z = T.z;

    return *this;
}

double Vector::operator*(const Vector &T) { // scalar product
    return this->x * T.x + this->y * T.y + this->z * T.z;
}

Vector Vector::operator*(const double n) { // Scale by n
    return Vector(this->x * n, this->y * n, this->z * n);
}

Vector Vector::operator+(const Vector &T) { // Addition
    return Vector(this->x + T.x, this->y + T.y, this->z + T.z);
}

Vector Vector::operator-(const Vector &T) { // Subtraction
    return Vector(this->x - T.x, this->y - T.y, this->z - T.z);
}

Vector Vector::operator^(const Vector &T) { // vector product
    return Vector(this->y * T.z - this->z * T.y,
                  (this->x * T.z - this->z * T.x),
                  this->x * T.y - this->y * T.x);
}

int Vector::getId() { return id; }
void Vector::setId(int _id) { id = _id; }
