#include <iostream>
#include <vector>
#include "sha256.cpp"
// #include <boost/multiprecision/cpp_int.hpp>
#include <sstream>
#include "cryptoMath.cpp"

using namespace std;
namespace mp = boost::multiprecision;

class Signature{
    string s;
    string r;

    public:
        Signature(){

        }

        Signature(string s, string r){
            this->s = s;
            this->r=  r;
        }

        string getS(){
            return this->s;
        }
        
        string getR(){
            return this->r;
        }

        string to_string(){
            return "( s: " + this->s  + " , r: " + this->r + " )" ;
        }
};

class PrivateKey {
    string hexValue;

    public:
        PrivateKey(){

        }

        PrivateKey(string value){
            this->hexValue = value;
        }

        string getHexValue(){
            return this->hexValue;
        }
        
        mp::cpp_int getIntegerValue(){
            return bm::hexToCpp_Int(this->hexValue);
        }
        FiniteFieldElement getFiniteFieldElementValue( FiniteField field ){
            return FiniteFieldElement(field, this->getIntegerValue());
        }
};

class Transaction{
    string from;
    string to;
    string hash;

    double amount;

    string token;

    Signature signature;

    private:

        void setHash(string hash){
            this->hash = hash;
        }

    public:

        Transaction(){

        }

        string getSender(){
            return this->from;
        }

        string hashTransaction(){
            return sha256(from+to+token+to_string(amount));
        }

        void setSignature(Signature signature){
            this->signature = signature;
        }

        Signature getSignature(){
            return this->signature;
        }

        string getHash(){
            return this->hash;
        }
        
        Transaction(string from, string to, string token, double amount){
            this->from = from;
            this->to = to;
            this->token = token;
            this->amount = amount;
            setHash(this->hashTransaction());
        }

        string toString(){

            string stringTrans = " ------------------------------------------------------------------------------\n";
            stringTrans += " | transaction | from: " + this->from + ", to: " + this->to + " | " + to_string(this->amount) + " " + token +"\n";
            stringTrans += " | trans hash  : " + this->getHash() + "\n" ;
            stringTrans += " | signature   : " + this->getSignature().to_string() + "\n" ;
            stringTrans += " ------------------------------------------------------------------------------\n";

            return stringTrans;
        }
};

class Block{


    // Block Data
    int index;
    Transaction transactions[15];


    string prevHash;
    string hash;
    
    
    int transactionsAmount = 0;

    public: 

        int getIndex(){
            return this->index;
        }

        void setIndex(int index){
            this->index = index;
        }

        string getPrevHash(){
            return this->prevHash;
        }

        void setPrevHash(string prevHash){
            this->prevHash = prevHash;
        } 

        string getHash(){
            return this->hash;
        }

        string toString(){
            string stringBlock = "------------------------------------------------------------------------------------\n";
            stringBlock += "Block Index       : " + to_string(this->index) +'\n'; 
            stringBlock += "prevHash          : " + this->prevHash + "\n";
            stringBlock += "Hash              : " + this->hash + "\n";
            stringBlock += "------------------------------------------------------------------------------------\n";        
            stringBlock += "transactions      : \n" ;

            for(int i = 0; i < transactionsAmount; i++){

                stringBlock += transactions[i].toString();

            }
            for(int i = transactionsAmount; i < 15; i++){
                stringBlock += "           0x00000000000000000000000000000000000000000000000000000000000000\n";
            }   

            stringBlock += "------------------------------------------------------------------------------------\n";

            return stringBlock;
        }

        void addTransaction(Transaction transaction){
            if (transactionsAmount < 15){
                transactions[transactionsAmount] = transaction;
                transactionsAmount++;
            }

        }

        void createHash(){

            //index
            string toHash = to_string(this->index);
            
            // transactions info
            for(int i = 0; i < transactionsAmount; i++){
                toHash += transactions[i].toString();
            }
            for(int i = transactionsAmount; i < 15; i++){
                toHash += "0x00000000000000000000000000000000000000000000000000000000000000";
            }

            toHash += prevHash;
            
            this->hash = sha256(toHash);
        }

};

class Blockchain{
    vector<Block> chain;
    FiniteField field;
    EllipticCurve curve;
    Point G;

    public :
        Blockchain(){

        }

        Blockchain(FiniteField field, EllipticCurve curve, Point G){
            this->field = field;
            this->curve = curve;
            this->G = G;
        }

        EllipticCurve getCurve(){
            return this->curve;
        }

        string getChain(){
            string output = "";
            for(Block block : chain){
                output += block.toString();
            }
            return output;
        }

        Point getG(){
            return this->G;
        }

        void writeBlock(Block& block){
            // verify Block

            if (!this->chain.empty()) {
                block.getPrevHash() = this->chain.front().getHash();
                block.setIndex(this->chain.front().getIndex() + 1);
            } else {
                block.setPrevHash("0x00000000000000000000000000000000000000000000000000000000000000");
                block.setIndex(0);
            }
            block.createHash();
            this->chain.push_back(block);
        }

        
};

bool confirmTransaction( Transaction transaction ){
    return true;
}

class Account {
    PrivateKey privateKey;
    string publicKey;
    Blockchain chain;
    
    private:
        void generatePublicKey(){
            Point p = (this->chain.getG() * this->privateKey.getHexValue() );

            this->publicKey = "0" + (p.getY().getValue()%2 +2).str() + p.getXHex();
        }

    public:
            Account(PrivateKey privateKey, Blockchain chain){
                this->privateKey = privateKey;
                this->chain = chain;
                generatePublicKey();
            }

            Transaction newTransaction(string to, string token, double amount){
                Transaction newTransaction = Transaction(this->publicKey, to, token, amount);
                this->signTransaction(newTransaction);

                return newTransaction;
            }

            void signTransaction(Transaction &transaction){

                FiniteField signatureField = FiniteField(this->chain.getCurve().getOrder());

                FiniteFieldElement h = FiniteFieldElement(signatureField, bm::hexToCpp_Int(transaction.getHash()));
                FiniteFieldElement k = FiniteFieldElement (signatureField, sha256( (FiniteFieldElement(signatureField, transaction.getHash()) + this->privateKey.getFiniteFieldElementValue(signatureField)).getHexValue() ) ) ;
                Point R = this->chain.getG() * k.getValue();
                FiniteFieldElement r = FiniteFieldElement(signatureField, R.getX().getValue());

                FiniteFieldElement s = k.inverse() * (h + r * this->privateKey.getFiniteFieldElementValue(signatureField) );

                Signature signature = Signature(s.getHexValue(), r.getHexValue());

                // cout << R.to_string() << endl;

                transaction.setSignature(signature);
            }

            string to_string(){
                string stringAccount = " ------------------------------------------------------------------------------\n";
                stringAccount += " | account | \n";
                stringAccount += " | privKey     : " + this->privateKey.getHexValue() + "\n" ;
                stringAccount += " | publicKey   : " +  this->publicKey + "\n" ;
                stringAccount += " ------------------------------------------------------------------------------\n";

            return stringAccount;
            }


            string getPublicKey(){
                return this->publicKey;
            }
};

class Wallet {
    Blockchain chain;
    string *seedPhrase;
    int seedLength = 12;


    private:
        PrivateKey generateKey(int walletNumber){
            string toHash = to_string(walletNumber);
            PrivateKey Key;

            for(int i = 0  ; i < this->seedLength; i++){
                toHash += this->seedPhrase[i];
            }

            Key = PrivateKey(sha256(toHash));
            return Key;
        }

    public:
        Wallet(Blockchain chain, string seedPhrase[12]){
            (*this).seedPhrase = seedPhrase;
            this->chain = chain;
        }

        Account getAccount( int number ){
            PrivateKey privKey = this->generateKey(number);
            Account acc = Account(privKey, this->chain);

            return acc;
        }
};

PrivateKey generateRandomKey(){
    
    string nums;
    mp::cpp_int num;
    string Key;

    srand(time(0));


    for (int i =0 ; i<77;i++){
        nums += to_string(rand()%10);
        num = mp::cpp_int(nums);
    }

    
    stringstream stream;

    stream << hex << num;

    Key =  stream.str();

    return PrivateKey(Key);
}

class Node{
    Blockchain chain;

    public:
        Node(Blockchain chain){
            this->chain = chain;
        }

        bool validateTransaction(Transaction transaction){
            FiniteField signatureField = FiniteField(this->chain.getCurve().getOrder());

            FiniteFieldElement s = FiniteFieldElement(signatureField, bm::hexToCpp_Int(transaction.getSignature().getS()));
            FiniteFieldElement h = FiniteFieldElement(signatureField, bm::hexToCpp_Int(transaction.hashTransaction()));
            FiniteFieldElement r = FiniteFieldElement(signatureField, bm::hexToCpp_Int(transaction.getSignature().getR()));

            Point senderPubKey = Point( transaction.getSender() ,this->chain.getCurve());

            Point R_recovered = (this->chain.getG() * (h * s.inverse()).getValue() + senderPubKey *(r*s.inverse()).getValue() );

            if (R_recovered.getX().getValue() == r.getValue()){
                return true;
            }
            return false;
        }

};