#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>
// #include <boost/multiprecision/cpp_bin_float.hpp>
#include <vector>
#include <string>
#include <bitset>
#include <typeinfo>
// #include "cryptoMath.cpp"
#include "Blockchain.cpp"
using namespace std;
namespace mp = boost::multiprecision;





int main(){

    mp::cpp_int prime = mp::pow(mp::cpp_int(2), 256) - mp::pow(mp::cpp_int(2), 32) - mp::cpp_int(977);
    mp::cpp_int order = mp::cpp_int("115792089237316195423570985008687907852837564279074904382605163141518161494337");

    FiniteField field = FiniteField(prime);
    FiniteField signatureField = FiniteField(order);

    EllipticCurve curve = EllipticCurve(FiniteFieldElement(field, "0"), FiniteFieldElement(field, "7"), order);

    Point G = Point( FiniteFieldElement(field, "79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798"), curve);

    PrivateKey key = PrivateKey("0e74e51aba7a660e56257c4b4cadc825740cc594eebf1c6874f961e164d4fd5b");
    Point pubKey = G * key.getIntegerValue();


    // string message = "my first signed message";
    string m_h = "990a86f031e0c57e15d4fb42688e59a1a41b52d6708e91d5d826fbb80e071bc2";



    FiniteFieldElement k = FiniteFieldElement (signatureField, sha256( (FiniteFieldElement(signatureField, m_h) + key.getFiniteFieldElementValue(signatureField)).getHexValue() ) ) ;

    Point R = G * k.getValue();
    
    FiniteFieldElement h = FiniteFieldElement(signatureField, bm::hexToCpp_Int(m_h));
    FiniteFieldElement r = FiniteFieldElement(signatureField, R.getX().getValue());

    FiniteFieldElement s = k.inverse() * (h + r * key.getFiniteFieldElementValue(signatureField) );

    Point R_recovered = (G * (h * s.inverse()).getValue() + pubKey *(r*s.inverse()).getValue() );

    // cout << s.getHexValue() << endl;
    // cout << r.getHexValue() << endl;
    
    // cout << order << endl;
    // cout << s.getHexValue() << endl;
    // cout << h.getHexValue() << endl;
    // cout << r.getHexValue() << endl;
    // cout << pubKey.to_string();
    // cout << R_recovered.to_string();


    string yVal = "0" + (G.getY().getValue() % 2 + 2).str() + G.getY().getHexValue();

    // cout << Point(FiniteFieldElement(field, "-5"), curve).to_string();

    cout << yVal.substr(2,-1) << endl << G.getY().getHexValue();

    return 0;

}