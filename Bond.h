#include <math.h>
#include <iostream>

using namespace std;

class Bond{

public:
double value;
double frequency,coupon,period,yield,adj_yield;
double coupon_factor,pv_factor;
double price1,price2;
double dv01,bond_price;


public:

   void Set_Values(double CouponRate,int Period,int Frequency)
     {
      frequency=Frequency;
      coupon=CouponRate/frequency;
      period=Period;

      }

   double calculate_dv01(double Yield,double bond_price)
     {
     //Calculating when yield is increased
      yield=Yield+0.01;
      price1=this->Present_Value(yield);
     
     //Calculating change when yield is decreased
      yield=yield-0.02;
      price2=this->Present_Value(yield);
    

     //Calcuting DVO1
     dv01=((bond_price-price1)+(price2-bond_price))/2;
     return dv01;
     }

     virtual double Present_Value(double Yield){};


};

class Coupon_Bearing: public Bond{

public:

    double Present_Value(double YieldRate)
     {
       adj_yield=(YieldRate/100)/frequency;
       pv_factor=1/pow((1+adj_yield),period);
       coupon_factor=(1-pv_factor)/adj_yield;
       value=coupon*coupon_factor + 100*pv_factor;
       return value;
     }


};

class Zero_Coupon: public Bond{


public:

   double Present_Value(double YieldRate)
    {
      adj_yield=(YieldRate/100)/frequency;
      value=100/pow((1+adj_yield),period);
      return value;

    }



};

































