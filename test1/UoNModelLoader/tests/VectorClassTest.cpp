#include "Vector.hpp"
#include <iostream>

using namespace std;

int main() {
    Vector u(2, 2, 0);
    Vector v(2, 2, 2);

    Vector ans1 = u + v; // Addition
    ans1.show();

    Vector ans2 = u - v; // Subtraction
    ans2.show();

    double ans3 = u * v; // scalar product
    cout << ans3 << endl;

    Vector ans4 = u ^ v; // vector product
    ans4.show();

    return 0;
}
