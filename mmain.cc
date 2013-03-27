/* This code parses over a portfolio of Fixed income bonds and liaises with Yield Curve file and Ratings book to price each security.
   A number of parameters are calculated including DV01 (first order risk) for each security. 
   These are printed out to a file results.txt which holds 4 values: Biggest long and short positions, The biggest Risk and Aggregated Risk for     portfolio.

*/



#include <fstream>
#include <iostream>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <iomanip>

#include "SBB_ratings.h"
#include "SBB_util.h"

#include "Match.h"
#include "Bond.h"


int
get_number_of_periods( SBB_date from_dt, SBB_date to_dt, int frequency);

int main(int argc, char* argv[])
{
   SBB_util *test_obj= new SBB_util();
   test_obj->start_clock();

  //Reading from tradingbook.txt

    SBB_instrument_input_file *obj1= new SBB_instrument_input_file(argv[1]);
    int length;

    SBB_instrument_fields *instrument_values=obj1->get_records(length);
    int x=0;
    SBB_date setDate, matDate, Frequency;
    int period;

 // Creating Bond class objects
    Bond* b =new Bond();
    Bond* coupon_bearing_bond= new Coupon_Bearing();
    Bond* zero_coupon_bond=new Zero_Coupon();

// defining variables for taking data from tradingbook
    double amount;
    double yield;
    int frequency;
    char ratetype[32] ;
    char quality[32] ;


//defining variables for largest entries
       double Total_Risk=0.0;
       double maximum_Risk;
       int maximum_Amount;
       int maximum_shortest_Amount;

//Creating object of Ratings class
    SBB_bond_ratings ratings;


//Reading yieldcurve
	SBB_instrument_input_file *treasury= new SBB_instrument_input_file(argv[2]);
    int no_of_records;
    SBB_instrument_fields *treasury_records=treasury->get_records(no_of_records);


//Sorting records from yieldcurve
    Match *match_record=new Match();
    match_record->sort_treasury(treasury_records,no_of_records,setDate,matDate);

//Processing each record
	  while(x<length) {

    //Reading data stored in the object
      int _sdate = instrument_values[x].SettlementDate;
      int _mdate = instrument_values[x].MaturityDate;
      yield=instrument_values[x].Rate;
      strcpy(ratetype,instrument_values[x].RateType);
      strcpy(quality,instrument_values[x].Quality);

    //Calculating periods
      setDate.set_from_yyyymmdd(_sdate);
      matDate.set_from_yyyymmdd(_mdate);
      period=get_number_of_periods( setDate, matDate, instrument_values[x].Frequency);


     //Processing for SPREAD type

      if (strcmp(ratetype,"SPREAD")==0)
      {
           double yield_to_add=match_record->find_correct_treasury(period,treasury_records);
           yield=(instrument_values[x].Rate/100)+yield_to_add;

      }

     //Processing Coupon Bearing bonds

    if (instrument_values[x].CouponRate!=0)
          {

        //Getting Present Value from the values in data.txt file
             coupon_bearing_bond->Set_Values( instrument_values[x].CouponRate, period,instrument_values[x].Frequency);
             instrument_values[x].NPV=coupon_bearing_bond->Present_Value(yield);


        //Calculating dv01
            instrument_values[x].DV01= coupon_bearing_bond->calculate_dv01(yield, instrument_values[x].NPV);


        //Calculating Risk
             instrument_values[x].Risk=(instrument_values[x].DV01/100)*instrument_values[x].Amount;


        }

    else
        {

        //Getting Present Value from the values in data.txt file
              zero_coupon_bond->Set_Values(instrument_values[x].CouponRate,period,2);
              instrument_values[x].NPV=zero_coupon_bond->Present_Value(yield);

       //Calculating DV01
              instrument_values[x].DV01= zero_coupon_bond->calculate_dv01(instrument_values[x].Rate,instrument_values[x].NPV);

       //Calculating Risk
             instrument_values[x].Risk=(instrument_values[x].DV01/100)*instrument_values[x].Amount;


        }

        Total_Risk=Total_Risk+instrument_values[x].Risk;

       //Getting instrument_values[x].LGD for bonds
        instrument_values[x].LGD=ratings.LGD_given_SnP_Fitch(quality)*instrument_values[x].Amount;

	   //Finding maximum Amount,Risk and LGD
	     if (maximum_Amount<instrument_values[x].Amount)
          {
          maximum_Amount=instrument_values[x].Amount;
          }

          if (maximum_shortest_Amount>instrument_values[x].Amount)
          {
              maximum_shortest_Amount=instrument_values[x].Amount;
          }

         if(maximum_Risk<fabs(instrument_values[x].Risk))
         {
             maximum_Risk=fabs(instrument_values[x].Risk);
         }


      x++;


  }


 FILE* pfile;

 //Printing to a file


 ofstream output("results.txt");
  output<<"This biggest long position is: "<<maximum_Amount
                <<"\nThe biggest short position is: "<<maximum_shortest_Amount<<"\n Risk: "<<maximum_Risk
                <<"\nThe total risk of the portfolio is: "<<Total_Risk;

  output.close();




 pfile=fopen (argv[1],"w");
 for(x=0;x<length;x++)
  {
    fprintf(pfile,"\n %s %s %d %.1f %d %d %s %.1f %s %d %.3f %.3f %.3f %.3f ",
            instrument_values[x].SecurityID,
            instrument_values[x].Ticker,
            instrument_values[x].SettlementDate
            ,instrument_values[x].CouponRate,
            instrument_values[x].MaturityDate,
            instrument_values[x].Frequency
             ,instrument_values[x].RateType,
             instrument_values[x].Rate,
             instrument_values[x].Quality,
             instrument_values[x].Amount
             ,instrument_values[x].NPV,
             instrument_values[x].DV01,
             instrument_values[x].Risk,
             instrument_values[x].LGD);

 }
fclose(pfile);

    //Displaying time
    test_obj->end_clock();
    delete b;
    delete match_record;


    return 0;



}




