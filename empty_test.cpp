#include <iostream>
// #include "cryptoMath.cpp"
#include "Blockchain.cpp"
#include <sstream>


using namespace std;

class test{

};

int main(){
    
    mp::cpp_int prime = mp::pow(mp::cpp_int(2), 256) - mp::pow(mp::cpp_int(2), 32) - mp::cpp_int(977);
     mp::cpp_int order = mp::cpp_int("115792089237316195423570985008687907852837564279074904382605163141518161494337");

    string seedPhrase[12] = {"name", "village", "axe", "auto", "joker", "slice", "power", "sence", "chicken", "toask", "ninja", "card"};

    FiniteField field = FiniteField(prime);
    EllipticCurve curve = EllipticCurve(FiniteFieldElement(field, (mp::cpp_int)"0"), FiniteFieldElement(field, "7"), order);

    FiniteFieldElement x = FiniteFieldElement(field, mp::cpp_int("55066263022277343669578718895168534326250603453777594175500187360389116729240"));
    FiniteFieldElement y = FiniteFieldElement(field, mp::cpp_int("32670510020758816978083085130507043184471273380659243275938904335757337482424"));


    Point G = Point(x, y, curve);

    Blockchain chain = Blockchain(field, curve, G);
    Wallet myWallet = Wallet(chain, seedPhrase);
               
    Account acc0 = myWallet.getAccount(0);

    cout << "current account : " << endl;
    cout << acc0.to_string() << endl;


    Node n = Node(chain);

    Transaction trans;
    string reciever;
    double amount;

    cout << "--new transaction--" << endl;
    cout << "enter reciever adress : ";
    cin >> reciever;
    cout << "enter amount : ";
    cin >> amount;

    trans = acc0.newTransaction(reciever, "ETH", amount);

    cout << trans.toString();
    
    cout << "transaction verifeid: " << (n.validateTransaction(trans) ? "yes" : "no") << endl;
    
    return 0;
}