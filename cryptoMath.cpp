#include <cmath>
#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>
#include "BigMath.cpp"
#include <sstream>

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

        bool operator == (FiniteField field){
            if (this->p == field.getPrime()){
                return true;
            } 
            return false;
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

            mp::cpp_int result = bm::power_by_squaring(this->getValue(), power, this->field.getPrime());
            
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
            // cout << "test";
            return this->mulpiply(el);
        }

        FiniteFieldElement operator / (FiniteFieldElement el){
            return this->divide(el);
        }

        bool operator == (FiniteFieldElement el){
            if (this->getValue() == el.getValue() && this->getField() == el.getField()){
                return true;
            }
            return false;
        }
};


class EllipticCurve{
    // Eliptic Curve
    // y^2 = x^2 + a * x + b;

    FiniteFieldElement a,b;
    FiniteField field;

    // Point startingPoint;

    public:

        EllipticCurve(){

        }

        EllipticCurve(FiniteFieldElement a, FiniteFieldElement b){
            if ( a.getField() == b.getField() ){
                this->a = a;
                this->b = b;
                this->field = a.getField();
            } else {
                throw invalid_argument( "received negative value" );
            }
        }

        FiniteField getField(){
            return this->field;
        }

        FiniteFieldElement getA(){
            return this->a;
        }
        FiniteFieldElement getB(){
            return this->b;
        }

        bool operator == (EllipticCurve curve){
            if( this->getA() == curve.getA() && this->getB() == curve.getB() && this->getField() == curve.getField() ){
                return true;
            }
            return false;
        }
};

class Point{
    FiniteFieldElement x;
    FiniteFieldElement y;
    EllipticCurve curve;
    bool isDefined = false;;

    public:
        Point(){

        }

        Point(FiniteFieldElement x, FiniteFieldElement y, EllipticCurve curve){
            this->x = x;
            this->y = y;
            this->curve = curve;
            this->isDefined = true;
        }

        FiniteFieldElement getX(){
            return this->x;
        }

        FiniteFieldElement getY(){
            return this->y;
        }

        string to_string(){
            return "( " + this->x.getValue().str() +", " + this->y.getValue().str() + " )\n";
        }

        EllipticCurve getCurve(){
            return this->curve;
        }

        bool getIsDefined(){
            return this->isDefined;
        }

        Point operator * (mp::cpp_int n){
            return this->multiply_by_doubling(n);
        }

        Point operator + (Point p){
            return Add(*this, p);
        }

        Point operator * (string el){

            mp::cpp_int n;
            stringstream stream;
            stream << hex << el;
            stream >> n;

            Point result = *this * n;

            return result;
        }

        bool operator == (Point p){
            if (this->getX() == p.getX() && this->getY() == p.getY() && this->getCurve() == p.getCurve()){
                return true;
            }
            return false;
        }

        Point multiply_by_doubling(mp::cpp_int n){
            Point p = *this;
            Point result;

            if (n==1){
                return *this;
            }

            while( n > 1 ){
                
                if ( n%2 == 1){
                    if (!result.getIsDefined()){
                        result = p;
                    } else {
                        result = result + p;
                    }
                }

                p = p.Double();

                n/=2;
            }


            return (result.getIsDefined() ? result + p : p);
        }   

        Point Double(){

            FiniteFieldElement temp_x, temp_y;
            
            FiniteFieldElement s = ( FiniteFieldElement(this->curve.getA().getField(), 3) * (*this).getX().exponent(2) + this->curve.getA() ) / (FiniteFieldElement(this->curve.getA().getField(), 2)*(*this).getY());

            temp_x = s * s - FiniteFieldElement(this->curve.getA().getField(), 2) * (*this).getX();
            temp_y = s * ( (*this).getX() - temp_x ) - (*this).getY();
        
            Point result = Point(temp_x, temp_y, this->curve);

            return result;
        }

        Point Add(Point A, Point B){

            if ( A == B ) return A.Double();

            FiniteFieldElement temp_x, temp_y;
            
            FiniteFieldElement s = (B.getY() - A.getY()) / (B.getX() - A.getX()) ;
            
            temp_x = s * s - A.getX() - B.getX();
            temp_y = s * (A.getX() - temp_x) - A.getY();

            Point result = Point(temp_x, temp_y, this->curve);

            return result;
        }
};
