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
            this->value = value  % this->field.getPrime();
            if (this->value < 0){
                this->value = this->value + this->field.getPrime();
            }
        }

        FiniteFieldElement(FiniteField field, string value){
            
            this->field = field;
            this->value = bm::hexToCpp_Int(value)  % this->field.getPrime();
            if (this->value < 0){
                this->value = this->value + this->field.getPrime();
            }
        }
        

        mp::cpp_int getValue(){
            return this->value;
        }

        string getHexValue(){
            return bm::cpp_intToHex(this->value);
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
            return *this * el.inverse();;
        }

        FiniteFieldElement inverse(){
            return this->exponent(this->field.getPrime()-2);
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
    mp::cpp_int order;
    FiniteField field;

    // Point startingPoint;

    public:

        EllipticCurve(){

        }

        EllipticCurve(FiniteFieldElement a, FiniteFieldElement b, mp::cpp_int order){
            if ( a.getField() == b.getField() ){
                this->a = a;
                this->b = b;
                this->field = a.getField();
                this->order = order;
            } else {
                throw invalid_argument( "received negative value" );
            }
        }

        mp::cpp_int getOrder(){
            return this->order;
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
            if (this->isZero(x,y)){
                this->x=x;
                this->y=y;
            } else {
                if ( !this->belongsToCurve(x,y,curve)){
                throw std::invalid_argument( "point not on a curve" ); 
            }
            }
            this->curve = curve;
            this->isDefined = true;
            this->x = x;
            this->y = y;
        }

        Point(string x, EllipticCurve curve){

            this->curve = curve;
            this->isDefined = true;
            this->x = FiniteFieldElement(this->curve.getField(), x.substr(2,-1));
            if (x.substr(0,2) == "03"){
                this->y = this->recoverYFromX();
            } else if (x.substr(0,2) == "02"){
                this->y = FiniteFieldElement(this->curve.getField(), -1 * this->recoverYFromX().getValue());// this->recoverYFromX();
            } else{
                throw std::invalid_argument( "impossible to recover point" ); 
            }
            if ( !this->belongsToCurve(this->getX(),y,curve)){
                throw std::invalid_argument( "point not on a curve" ); 
            }
        }

        bool isZero(FiniteFieldElement x, FiniteFieldElement y){
            if (x.getValue() == 0 && y.getValue() == 1){
                return true;
            }
            return false;
        }

        bool belongsToCurve(FiniteFieldElement x, FiniteFieldElement y, EllipticCurve curve){
            if ( y.exponent(2) == x.exponent(3) + x * curve.getA() + curve.getB() ){
                return true;
            }
            return false;
        }

        FiniteFieldElement recoverYFromX(){

            FiniteFieldElement y2= FiniteFieldElement(this->curve.getField(), ( x.exponent(3) + this->curve.getA() * x + this->curve.getB() ).getValue() );
            mp::cpp_int y = y2.exponent( FiniteFieldElement(this->curve.getField(), 4).inverse().getValue() ).getValue();
                
            return FiniteFieldElement(this->curve.getField(), y);
    }

        FiniteFieldElement getX(){
            return this->x;
        }
        string getXHex(){
            return bm::cpp_intToHex(this->getX().getValue());
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

        Point operator + (Point p){
            return Add(*this, p);
        }

        Point operator * (mp::cpp_int n){
            return this->multiply_by_doubling(n);
        }

        Point operator * (string el){
            Point result = *this * bm::hexToCpp_Int(el);
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
            if (A.getX().getValue() == B.getX().getValue() && A.getY().getValue() != B.getY().getValue()){
                return Point(FiniteFieldElement(this->getCurve().getField(), "0"), FiniteFieldElement(this->getCurve().getField(),"1"), this->getCurve());
            }
            if (A.isZero(A.getX(), A.getY())){
                return B;
            }
            if (B.isZero(B.getX(), B.getY())){
                return A;
            }
            if ( A == B ) return A.Double();

            FiniteFieldElement temp_x, temp_y;
            
            FiniteFieldElement s = (B.getY() - A.getY()) / (B.getX() - A.getX()) ;
            
            temp_x = s * s - A.getX() - B.getX();
            temp_y = s * (A.getX() - temp_x) - A.getY();

            Point result = Point(temp_x, temp_y, this->curve);

            return result;
        }
};
