#include <iostream>
#include "cryptoMath.cpp"

using namespace std;

int main(){

    mp::cpp_int prime = mp::pow(mp::cpp_int(2), 256) - mp::pow(mp::cpp_int(2), 32) - mp::cpp_int(977);

    FiniteField cryptoField = FiniteField(17);
    EllipticCurve cryptoCurve = EllipticCurve( FiniteFieldElement(cryptoField, 0), FiniteFieldElement(cryptoField, 7));
    Point G = Point(FiniteFieldElement(cryptoField, 15), FiniteFieldElement(cryptoField, 13), cryptoCurve);


    cout << (G * FiniteFieldElement(cryptoField, 6) ).to_stirng();

    return 0;
}