#include <iostream>
#include<string.h>
#include "SBB_date.h"
#include "SBB_io.h"


class Match
 {
     public:
     int x,k;
     int period_of_treasury[];
     int length;

     void sort_treasury(SBB_instrument_fields *treasury_records,int length, SBB_date setDate, SBB_date matDate)
       {

           while(x<length)
                {
                    int _sdate = treasury_records[x].SettlementDate;
                    int _mdate = treasury_records[x].MaturityDate;
                    int frequency= treasury_records[x].Frequency;

                    setDate.set_from_yyyymmdd(_sdate);
                    matDate.set_from_yyyymmdd(_mdate);
                    period_of_treasury[x]=get_number_of_periods( setDate, matDate, frequency);

                    x++;

                }



              int temp, i,j;
              double holder;

              for( int i = 0; i < length; i++)
               {
                 for (int j = 0;j < (length-1); j++)
                 {
                        if (period_of_treasury[j+1] < period_of_treasury[j])
                        {
                           temp = period_of_treasury[j];
                           period_of_treasury[j] = period_of_treasury[j+1];
                           period_of_treasury[j+1] = temp;

                           holder = treasury_records[j].Rate;
                           treasury_records[j].Rate = treasury_records[j+1].Rate;
                           treasury_records[j+1].Rate = holder;
                        }
                 }
               }



      }

        double find_correct_treasury(int period,SBB_instrument_fields *treasury_records)
        {
            for(int i=0;i<(length);i++)
               {
               if(period_of_treasury[i-1]<period && period<=period_of_treasury[i])
                       {
                         return treasury_records[i].Rate;

                       }

            }


        }
 };
