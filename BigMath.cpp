#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>
// #include <boost/multiprecision/cpp_bin_float.hpp>
#include <vector>
#include <string>

using namespace std;
namespace mp = boost::multiprecision;

namespace bigmath{
    
struct PowersAndReminder {
    vector<int>powers;
    mp::cpp_int remainder = 0;

    string to_string(){
        string result = " powers : [ ";
        for(int power : powers){
            result += std::to_string(power) + ", ";
        }
        result += "] | remainder : " + remainder.str() + "\n";
        return result;
    }

    mp::cpp_int restore(){
        mp::cpp_int value;

        for(int power : powers){
            value += mp::pow(mp::cpp_int(2), power);
        }

        value += remainder;
    
        return value;
    }
};

int log_2(mp::cpp_int num){
    int counter=0;

    do{
        counter++;
        num/=2;
    }while (num >= 2 );

    return counter;
}

PowersAndReminder getPowersAndRemainder(mp::cpp_int num){
    PowersAndReminder par;
    int primeLog2 = log_2(num);
    par.powers.push_back(primeLog2);


    if ( mp::pow(mp::cpp_int(2), primeLog2) != num) {

        mp::cpp_int twoPowPrimeLog2 = mp::pow(mp::cpp_int(2), primeLog2);

        mp::cpp_int toPowerAndRemainderList = num - twoPowPrimeLog2;
        
        mp::cpp_int currentValue = 0;   
        mp::cpp_int temp;
        int counter = primeLog2 - 1;


        while(currentValue < toPowerAndRemainderList && counter > 0){

            temp = mp::pow(mp::cpp_int(2), counter);
            if (currentValue + temp <  toPowerAndRemainderList){
                currentValue += temp;
                par.powers.push_back(counter);
            }

            counter--;
        }


        par.remainder = toPowerAndRemainderList - currentValue;

    }


    return par;
}

mp::cpp_int powerByStruct(mp::cpp_int num, PowersAndReminder par, mp::cpp_int prime){
    mp::cpp_int result = 1;

    mp::cpp_int powerOfNum;

    for(int power : par.powers){
        powerOfNum = num;

        for(int i = 0 ; i < power; i++){
            powerOfNum = mp::pow(powerOfNum, 2) % prime;
        }

        result *= powerOfNum;
        result %= prime;
    }

   

    if ( par.remainder != 0 ){
        powerOfNum = mp::pow(num, (int)par.remainder) % prime;
        result *= powerOfNum;
        result %= prime;
    }
    

    return result;
}

}

