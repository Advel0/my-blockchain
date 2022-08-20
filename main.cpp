#include <iostream>
#include <vector>
#include "sha256.h"


using namespace std;

class Transaction{
    string from;
    string to;
    string hash;

    double amount;

    string token;

    public:

        Transaction(){

        }

        string getHash(){
            return this->hash;
        }
        
        Transaction(string token){
            this->token = token;
            cout << "< ----  - new transaction - ---- >\n";
            
            
            cout << "from: ";
            cin >> this->from;
            cout << "to: ";
            cin >> this->to;

            cout << "amount: ";
            cin >> this->amount;

            this->hash = sha256(this->toString());
        }

        string toString(){

            string stringTrans = " ------------------------------------------------------------------------------\n";
            stringTrans += " | transaction | from: " + this->from + ", to: " + this->to + " | " + to_string(this->amount) + " " + token +"\n";
            stringTrans += " | trans hash  : " + this->getHash() + "\n" ;
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

        void addTransaction(string transaction){
            if (transactionsAmount < 15){
                transactions[transactionsAmount] = Transaction(transaction);
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

    public :
        string getChain(){
            string output = "";
            for(Block block : chain){
                output += block.toString();
            }
            return output;
        }

        void writeBlock(Block& block){
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

int main(){

    Blockchain chain;


    Block firstBlock = Block();

    firstBlock.addTransaction("AC");

    Block secondBlock = Block();
    chain.writeBlock(firstBlock);
    chain.writeBlock(secondBlock);

    cout << chain.getChain();

    
    return 0;
    
}

