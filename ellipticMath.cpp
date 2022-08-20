#include <cmath>
#include <iostream>
 
using namespace std;
 

class Point{
    double x;
    double y;

    public:

        Point(){

        }

        Point(double x, double y){
            this->x = x;
            this->y = y;
        }

        double getX(){
            return this->x;
        }

        double getY(){
            return this->y;
        }

        string to_stirng(){
            return "( " + to_string(this->x) +", " + to_string(this->y) + " )\n";
        }
};


class EllipticCurve{
    // Eliptic Curve
    // y^2 = x^2 + a * x + b;

    double a,b;

    // Point startingPoint;

    public:
        EllipticCurve(double a, double b){
            this->a = a;
            this->b = b;

        }

        Point Double(Point point){
            double temp_x, temp_y;
            
            double s = ( 3 * pow( point.getX(), 2 ) + this->a ) / (2*point.getY());
            
            cout << s;

            temp_x = pow( s , 2) - 2 * point.getX();
            temp_y = s * ( point.getX() - temp_x ) - point.getY();
        
            Point result = Point(temp_x, temp_y);

            return result;
        }
};

class FiniteField{
    int p;

    public:

        int getPrime(){
            return this->p;
        }

        FiniteField(){

        }

        FiniteField(int p){
            this->p = p;
        }
};

class FiniteFieldElement{
    FiniteField field;
    int value;


    public:
        FiniteFieldElement(){

        }

        FiniteFieldElement(FiniteField field, int value){
            this->field = field;
            this->value = value;
        }

        int getValue(){
            return this->value;
        }

        FiniteFieldElement add(FiniteFieldElement el){
            FiniteFieldElement newFiniteFieldElement;
            int newValue;

            newValue = (this->getValue() + el.getValue()) % this->field.getPrime() ;
            newFiniteFieldElement = FiniteFieldElement(this->field, newValue);

            return newFiniteFieldElement;
        }
        FiniteFieldElement substract(FiniteFieldElement el){
            FiniteFieldElement newFiniteFieldElement;
            int newValue;

            int temp = (this->getValue() - el.getValue()) % this->field.getPrime() ;

            
            if ( temp < 0 ) {
                newValue = temp + this->field.getPrime();
            } else {
                newValue = temp;
            }

            newFiniteFieldElement = FiniteFieldElement(this->field, newValue);

            return newFiniteFieldElement;
        }

        FiniteFieldElement mulpiply(FiniteFieldElement el){
            FiniteFieldElement newFiniteFieldElement;
            int newValue;

            newValue = ( this->getValue() * el.getValue() ) % this->field.getPrime();
            newFiniteFieldElement = FiniteFieldElement(this->field, newValue);

            return newFiniteFieldElement;
        }
        FiniteFieldElement divide(FiniteFieldElement el){
            FiniteFieldElement newValue;
            FiniteFieldElement inverse = el.exponent(this->field.getPrime()-2);
            
            newValue = *this * inverse;

            return newValue;
        }
        FiniteFieldElement exponent(int power){
            long long value = (long long) pow(this->getValue(), power) ;
            cout << value << endl;
            return FiniteFieldElement(this->field, 3);
        }

        // overloading basic operations

        FiniteFieldElement operator + (FiniteFieldElement el){
            return this->add(el);
        }

        FiniteFieldElement operator - (FiniteFieldElement el){
            return this->substract(el);
        }

        FiniteFieldElement operator * (FiniteFieldElement el){
            return this->mulpiply(el);
        }

        FiniteFieldElement operator / (FiniteFieldElement el){
            return this->divide(el);
        }
};


int main(){
    // EllipticCurve bitcoinCurve = EllipticCurve(2.0 , 3.0);

    // double start_x = 3;
    // double start_y = 6; //pow(pow( start_x, 3 ) + 7, 0.5 );

    // Point startingPoint = Point(start_x , start_y);

    // cout << startingPoint.to_stirng();
    // cout << bitcoinCurve.Double(startingPoint).to_stirng();

    FiniteField myField = FiniteField(19);

    FiniteFieldElement num1 = FiniteFieldElement(myField, 2);
    FiniteFieldElement num2 = FiniteFieldElement(myField, 7);

    cout << (num1/num2).getValue();

}