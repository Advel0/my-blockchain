#include <iostream>
#include <vector>
#include "sha256.cpp"
// #include <boost/multiprecision/cpp_int.hpp>
#include <sstream>
#include "cryptoMath.cpp"

using namespace std;
namespace mp = boost::multiprecision;

class Transaction{
    string from;
    string to;
    string hash;

    double amount;

    string token;

    string signature;

    private:
        void Hash(){
            this->hash = sha256(from+to+token+to_string(amount));
        }

    public:

        Transaction(){

        }

        void setSignature(string signature){
            this->signature = signature;
        }

        string getSignature(){
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
            Hash();
        }

        string toString(){

            string stringTrans = " ------------------------------------------------------------------------------\n";
            stringTrans += " | transaction | from: " + this->from + ", to: " + this->to + " | " + to_string(this->amount) + " " + token +"\n";
            stringTrans += " | trans hash  : " + this->getHash() + "\n" ;
            stringTrans += " | signature   : " + this->getSignature() + "\n" ;
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
    string privateKey;
    string publicKey;
    Blockchain chain;
    
    private:
        void generatePublicKey(){
            Point p = (this->chain.getG() * this->privateKey );
            
            stringstream stream;

            stream << hex << p.getX().getValue();

            stream >> this->publicKey;
        }

    public:
            Account(string privateKey, Blockchain chain){
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
                string signature;



                transaction.setSignature(signature);
            }

            string to_string(){
                string stringAccount = " ------------------------------------------------------------------------------\n";
                stringAccount += " | account | \n";
                stringAccount += " | privKey     : " + this->privateKey + "\n" ;
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
        string generateKey(int walletNumber){
            string toHash = to_string(walletNumber);
            string Key;

            for(int i = 0  ; i < this->seedLength; i++){
                toHash += this->seedPhrase[i];
            }

            Key = sha256(toHash);
            return Key;
        }

    public:
        Wallet(Blockchain chain, string seedPhrase[12]){
            (*this).seedPhrase = seedPhrase;
            this->chain = chain;
        }

        Account getAccount( int number ){
            string privKey = this->generateKey(number);
            Account acc = Account(privKey, this->chain);

            return acc;
        }
};

string generateRandomKey(){
    
    string nums;
    mp::cpp_int num;
    string Key;

    srand(time(0));


    for (int i =0 ; i<77;i++){
        nums += to_string(rand()%10);
        num = mp::cpp_int(nums);
    }

    
    stringstream stream;
    cout << num << endl;
    stream << hex << num;

    Key =  stream.str();

    return Key;
}


