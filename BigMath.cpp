#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>
// #include <boost/multiprecision/cpp_bin_float.hpp>
#include <vector>
#include <string>
#include <sstream>
// #include "cryptoMath.cpp"

using namespace std;
namespace mp = boost::multiprecision;

namespace bigmath{
    
    mp::cpp_int power_by_squaring(mp::cpp_int num, mp::cpp_int power, mp::cpp_int prime){
        if ( num == 0 ){
            return 1;
        }

        mp::cpp_int result = 1;

        while (power > 0){
            if (power %2 == 1) {
                result *= num;
                result %=prime;
            }

            num *= num;
            num %= prime;
        
            power /= 2;
        }

        return result;
    }

    mp::cpp_int hexToCpp_Int(string value){
        mp::cpp_int converted;
        
        stringstream stream;

        stream << hex << value;

        stream >> converted;

        return converted;
    }

    string cpp_intToHex(mp::cpp_int value){
        string converted;

        stringstream stream;

        stream << hex << value;

        stream >> converted;

        return converted;
    }



}

