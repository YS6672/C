#include "Material.hpp"
#include <cmath>
#include <iostream>
using namespace std;

// Constructor
Material::Material(int _ID, string _Name, double _Density)
    : ID(_ID), Name(_Name), Density(_Density) {}

// Copy Constructor
Material::Material(const Material &m)
    : ID(m.ID), Name(m.Name), Density(m.Density), col(m.col) {}

// Assignment Operator
// -----------------------------------------------------------------------------
Material &Material::operator=(const Material &m) {
    if (&m == this)
        return (*this);

    this->ID = m.ID;
    this->Name = m.Name;
    this->Density = m.Density;
    this->col = m.col;

    return (*this);
}

// Destructor
Material::~Material() {}

void Material::setID(unsigned int ID) { this->ID = ID; }

void Material::setDensity(double Density) { this->Density = Density; }

void Material::setName(string Name) { this->Name = Name; }

unsigned int Material::getID() { return this->ID; }

double Material::getDensity() { return this->Density; }

string Material::getName() { return this->Name; }

// Constructor (Colour)
Colour::Colour() {}

// Constructor (Colour)
Colour::~Colour() {}
Colour::Colour(const Colour &c) {
    this->r = c.r;
    this->g = c.g;
    this->b = c.b;
}

Colour &Colour::operator=(const Colour &c) {
    if (&c == this)
        return (*this);

    this->r = c.r;
    this->g = c.g;
    this->b = c.b;

    return (*this);
}

void Colour::setcolour(unsigned char r, unsigned char g, unsigned char b) {
    this->r = r;
    this->g = g;
    this->b = b;
}
void Colour::setcolour(unsigned int rgb) {
    this->r = (rgb >> 16) & 0xff;
    this->g = (rgb >> 8) & 0xff;
    this->b = rgb & 0xff;
}

void Colour::setcolour(float r, float g, float b) {
    this->r = round(r);
    this->g = round(g);
    this->b = round(b);
}

unsigned int Colour::getColour() {
    return ((this->r & 0xff) << 16) + ((this->g & 0xff) << 8) +
           (this->b & 0xff);
}

int HexadecimalToDecimal(string code) {
    int len = code.length();
    double dec = 0;
    for (int i = 0; i < len; i++) {
        char c = code[i];
        if (c >= 48 && c <= 57)
            c -= 48;
        else if (c >= 65 && c <= 70)
            c -= 55;
        dec += c * pow(16, ((len - i) - 1));
    }
    return (int)dec;
}

void Colour::HexadecimalToRGB(string hex) {
    this->r = (unsigned char)HexadecimalToDecimal(hex.substr(0, 2));
    this->g = (unsigned char)HexadecimalToDecimal(hex.substr(2, 2));
    this->b = (unsigned char)HexadecimalToDecimal(hex.substr(4, 2));
}
