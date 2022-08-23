#include <cmath>
#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>
#include "BigMath.cpp"


using namespace std;
namespace mp = boost::multiprecision; 
namespace bm = bigmath;


class FiniteField{
    mp::cpp_int p;

    public:

        mp::cpp_int getPrime(){
            return this->p;
        }

        FiniteField(){

        }

        FiniteField(mp::cpp_int p){
            this->p = p;
        }
};

class FiniteFieldElement{
    FiniteField field;
    mp::cpp_int value;


    public:
        FiniteFieldElement(){

        }

        FiniteField getField(){
            return this->field;
        }

        FiniteFieldElement(FiniteField field, mp::cpp_int value){
            this->field = field;
            this->value = value;
        }

        mp::cpp_int getValue(){
            return this->value;
        }

        FiniteFieldElement add(FiniteFieldElement el){
            FiniteFieldElement newFiniteFieldElement;
            mp::cpp_int newValue;

            newValue = (this->getValue() + el.getValue()) % this->field.getPrime() ;
            newFiniteFieldElement = FiniteFieldElement(this->field, newValue);

            return newFiniteFieldElement;
        }
        FiniteFieldElement substract(FiniteFieldElement el){
            FiniteFieldElement newFiniteFieldElement;
            mp::cpp_int newValue;

            mp::cpp_int temp = (this->getValue() - el.getValue()) % this->field.getPrime() ;

            
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
            mp::cpp_int newValue;

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
        FiniteFieldElement exponent(mp::cpp_int power){
            bm::PowersAndReminder par = bm::getPowersAndRemainder(power);

            mp::cpp_int result = bm::powerByStruct(this->getValue(), par, this->field.getPrime());

            return FiniteFieldElement(this->field, result);
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


class EllipticCurve{
    // Eliptic Curve
    // y^2 = x^2 + a * x + b;

    FiniteFieldElement a,b;

    // Point startingPoint;

    public:

        EllipticCurve(){

        }

        EllipticCurve(FiniteFieldElement a, FiniteFieldElement b){
            this->a = a;
            this->b = b;

        }

        FiniteFieldElement getA(){
            return this->a;
        }
        FiniteFieldElement getB(){
            return this->b;
        }

};

class Point{
    FiniteFieldElement x;
    FiniteFieldElement y;
    EllipticCurve curve;

    public:
        Point(){

        }

        Point(FiniteFieldElement x, FiniteFieldElement y, EllipticCurve curve){
            this->x = x;
            this->y = y;
            this->curve = curve;
        }

        FiniteFieldElement getX(){
            return this->x;
        }

        FiniteFieldElement getY(){
            return this->y;
        }

        string to_stirng(){
            return "( " + this->x.getValue().str() +", " + this->y.getValue().str() + " )\n";
        }


        Point operator * (FiniteFieldElement el){
            Point result = *this;
            if (el.getValue() > 1){
                result = this->Double(*this);
            }


            for(mp::cpp_int i = 0; i < (el.getValue()-2); i++ ){
                result = this->Add(result, *this);
            }

            return result;
        }


        Point Double(Point point){

            FiniteFieldElement temp_x, temp_y;
            
            FiniteFieldElement s = ( FiniteFieldElement(this->curve.getA().getField(), 3) * point.getX().exponent(2) + this->curve.getA() ) / (FiniteFieldElement(this->curve.getA().getField(), 2)*point.getY());
            
            // cout << s;

            temp_x = s * s - FiniteFieldElement(this->curve.getA().getField(), 2) * point.getX();
            temp_y = s * ( point.getX() - temp_x ) - point.getY();
        
            Point result = Point(temp_x, temp_y, this->curve);

            return result;
        }

        Point Add(Point A, Point B){
            FiniteFieldElement temp_x, temp_y;
            
            FiniteFieldElement s = (B.getY() - A.getY()) / (B.getX() - A.getX()) ;
            
            temp_x = s * s - A.getX() - B.getX();
            temp_y = s * (A.getX() - temp_x) - A.getY();

            Point result = Point(temp_x, temp_y, this->curve);

            return result;
        }
};

// int main(){

//     FiniteField myField = FiniteField(19);

//     FiniteFieldElement num1 = FiniteFieldElement(myField, 5);
//     FiniteFieldElement num2 = FiniteFieldElement(myField, 5);

//     cout << (num1*num2).getValue() << endl;

// }