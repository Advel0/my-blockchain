#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>
// #include <boost/multiprecision/cpp_bin_float.hpp>
#include <vector>
#include <string>
#include <bitset>
#include <typeinfo>
#include "cryptoMath.cpp"
using namespace std;
namespace mp = boost::multiprecision;





int main(){

    FiniteField field = FiniteField(17);
    EllipticCurve curve = EllipticCurve(FiniteFieldElement(field, 0), FiniteFieldElement(field, 7));

    Point p = Point(FiniteFieldElement(field, 15), FiniteFieldElement(field, 13), curve);

    cout << p.Double().Double().to_string();
    cout << p.Double().to_string();


    // p = (p.Double().Double() + p.Double() );
    p = p * 6;
    // p = p.Double().Double() + p.Double();

    cout << p.to_string() << endl;

    // cout << bm::power_by_squaring(2,6,1000000000);

 


    return 0;

}