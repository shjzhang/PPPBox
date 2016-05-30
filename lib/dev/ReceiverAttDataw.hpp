#pragma ident "$Id: ReceiverAttData.hpp 2897 2011-09-14 20:08:46Z shjzhang $"
//only for GOCE ATTITUDE
/**
 * @file ReceiverAttData.hpp
 * Read receiver attitude data  
 */

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  
//  Shoujian Zhang, Wuhan University, 2012
//
//============================================================================

#ifndef GPSTK_RECEIVER_ATT_DATA_HPP
#define GPSTK_RECEIVER_ATT_DATA_HPP

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>


#include "Miscmath.hpp"
#include "Mathhwei.hpp"
// data structures
#include "DataStructures.hpp"

using namespace std;




namespace gpstk
{

      class ReceiverAttDataw
   {
   
   public:
      
      
         
         typedef struct GOCEatt
         {
               double second;
               double q1;
               double q2;
               double q3;
               double q4;
               

               GOCEatt()
               {
                     second = q1 = q2 = q3 =q4 = 0.0;
               }
               
               
               
         } GOCEatt;
         

         typedef struct GOCEprdposition
         {
               double second;
               double x;
               double y;
               double z;
               double vx;
               double vy;
               double vz;
               double tag;
               
               
               GOCEprdposition()
               {
                     second = x = y = z = vx = vy =vz = 0.0;
                     tag=0.0;
               }
               
               
               
         } GOCEprdposition;
         
         
         //read GOCE position file
         void ReadGOCEposition(const char *filename, vector<GOCEprdposition> &vGOCEprdpositions)
         {
               
               GOCEprdposition gocepx;
               
               vGOCEprdpositions.clear();
               FILE *fp = fopen(filename, "r");
               if(!fp)
               {
                     printf("File error!\n");
                     exit(0);
               }
               
               char buf[512];
               
               while(1)
               {
                     if(feof(fp))
                           break;
                     fgets(buf, 512,fp);
                     sscanf(buf, "%lf%lf%lf%lf%lf%lf%lf", &gocepx.second,
                                          &gocepx.x, &gocepx.y,&gocepx.z,
                                          &gocepx.vx,&gocepx.vy,&gocepx.vz);
                     vGOCEprdpositions.push_back(gocepx);
                     
               }
               
               cout<<"The length of file "<<filename<<" is "
               << vGOCEprdpositions.size()<<endl;
               
               fclose(fp);
               
         }//end of read GOCE attitude file
         
         //read GOCE position file
         void ReadGOCEposition2(double t1, double t2,const char *filename, vector<GOCEprdposition> &vGOCEprdpositionsnew)
         {
               vector<GOCEprdposition>  vGOCEprdpositions;
               vGOCEprdpositionsnew.clear();
               
               // read GOCE position file
               ReadGOCEposition(filename,vGOCEprdpositions);
               
               int length;
               length=vGOCEprdpositions.size();
               
               for (int i=0; i<length-1;i++)
               {
                     
                     if((vGOCEprdpositions[i].second > (t1-100))
                        &&(vGOCEprdpositions[i].second < (t2+100)))
                           
                           vGOCEprdpositionsnew.push_back(vGOCEprdpositions[i]);
                     
               }
               
               
               
         
         }//end of read GOCE attitude file
         
         //get GOCEposition value at time ttag   vGOCEattag
         void GetGOCEpostime(double ttag, vector<GOCEprdposition> vGOCEprdpositions,
                            GOCEprdposition &vGOCEptag)
         {
               
               //*******************
               // choose 9 point for lagrangepoly ,change it as you need;
               int N=9;
               int mid=floor(N/2);
               //*******************
               
                vector<double> times(N), x(N), y(N), z(N);
               double err;
               
               // compute dt for GOCE prd data
               //compute  line use to interpoly
               double dt=vGOCEprdpositions[1].second-vGOCEprdpositions[0].second;
               int line,lengtht;
               line=floor((ttag-vGOCEprdpositions[0].second)/round(dt));
               
               lengtht=vGOCEprdpositions.size();
               
               if ((ttag<vGOCEprdpositions[N].second )||(ttag > vGOCEprdpositions[lengtht-N].second ) )
               {
               
                 cout<<setprecision(16);
                 cout<<"The GOCE prd position inpoly time  "<<ttag<<" out of file "
                     <<"the time should in "<<vGOCEprdpositions[N].second
                     << " and " <<vGOCEprdpositions[lengtht-N].second<<endl;
                     return ;
               }
               
              // cout<<"ttag"<<ttag<<endl;
               
               
               
               for(int i = 0; i < N; i ++)
               {
                     times[i]=vGOCEprdpositions[line-mid+i].second;
                     x[i]=vGOCEprdpositions[line-mid+i].x;
                     y[i]=vGOCEprdpositions[line-mid+i].y;
                     z[i]=vGOCEprdpositions[line-mid+i].z;
               }
               vGOCEptag.second=ttag;
               vGOCEptag.x=LagrangeInterpolation(times,x,ttag,err);
               vGOCEptag.y=LagrangeInterpolation(times,y,ttag,err);
               vGOCEptag.z=LagrangeInterpolation(times,z,ttag,err);

               
         
         }

         
         
         
         
         //read GOCE attitude file
         void ReadGOCEatt(const char *filename, vector<GOCEatt> &vGOCEatts)
         {
               
               GOCEatt goceattx;
               
               vGOCEatts.clear();
               FILE *fp = fopen(filename, "r");
               if(!fp)
               {
                     printf("File error!\n");
                     exit(0);
               }
               
               char buf[512];
               
               while(1)
               {
                     if(feof(fp))
                           break;
                     fgets(buf, 512,fp);
                     sscanf(buf, "%lf%lf%lf%lf%lf", &goceattx.second,
                            &goceattx.q1, &goceattx.q2, &goceattx.q3,
                            &goceattx.q4);
                     vGOCEatts.push_back(goceattx);
                     
               }
               
               cout<<"The length of file "<<filename<<" is "
                   <<vGOCEatts.size()<<endl;
               
               fclose(fp);
         } //end of read GOCEattitude file
         
         //read GOCE attitude file
         void ReadGOCEatt2(double t1,double t2,const char *filename, vector<GOCEatt> &vGOCEattsnew)
         {
               
               
               vector<GOCEatt> vGOCEatts;
               vGOCEattsnew.clear();
               //read file
               ReadGOCEatt(filename,vGOCEatts);
               
               int length;
               length=vGOCEatts.size();
               
               for (int i=0; i<length-1;i++)
               {
               
                     if((vGOCEatts[i].second > (t1-100))&&(vGOCEatts[i].second < (t2+100)))
                  vGOCEattsnew.push_back(vGOCEatts[i]);
                     
               }
               
         
         } //end of read GOCEattitude file
         
         
         
         
         
         
         //get GOCE attitude value at time ttag   vGOCEattag
         void GetGOCEattime(double ttag, vector<GOCEatt> vGOCEatts,
                          GOCEatt &vGOCEattag)
         
         {

               //*******************
               // choose 9 point for lagrangepoly ,change it as you need;
               int N=9;
               int mid=floor(N/2);
               //*******************
               

               vector<double> times(N), Q1(N), Q2(N), Q3(N), Q4(N);
               double dt,err,Qx[4];
               int line,lengtht;
               
               
               //cout<<"ttag"<<ttag<<endl;
               dt=vGOCEatts[1].second-vGOCEatts[0].second;
               line=floor((ttag-vGOCEatts[0].second)/round(dt));
               
               lengtht=vGOCEatts.size();
               
               if ((ttag<vGOCEatts[N].second )||(ttag > vGOCEatts[lengtht-N].second ) )
               {
                     cout<<setprecision(16);
                     cout<<"The GOCE attitude inpoly time  "<<ttag<<" out of file "
                     <<"the time should in "<<vGOCEatts[N].second
                     << " and " <<vGOCEatts[lengtht-N].second<<endl;
                     return ;
               }
               
               
               // read data for interpoly
               
               for(int i = 0; i < N; i ++)
               {
                     
                     times[i]=vGOCEatts[line-mid+i].second;
                        Q1[i]=vGOCEatts[line-mid+i].q1;
                        Q2[i]=vGOCEatts[line-mid+i].q2;
                        Q3[i]=vGOCEatts[line-mid+i].q3;
                        Q4[i]=vGOCEatts[line-mid+i].q4;
                     
                     
                     // comtinue quater
                     //        if  Qtn(i,1:3)*Qtn(i-1,1:3)'<0
                     //        Qtn(i,1:4)=-Qtn(i,1:4);
                     
                     if(i> 0)
                     {
                           err=Q1[i]*Q1[i-1]+Q2[i]*Q2[i-1]+Q3[i]*Q3[i-1];
                           if(err<0)
                           {
                                 Q1[i]=-Q1[i];
                                 Q2[i]=-Q2[i];
                                 Q3[i]=-Q3[i];
                                 Q4[i]=-Q4[i];
                           }
                           err=0;
                     }

                     
                     
               }
               //cout<<"ttag"<<ttag<<endl;


               
               
               Qx[0]=LagrangeInterpolation(times,Q1,ttag,err);
               Qx[1]=LagrangeInterpolation(times,Q2,ttag,err);
               Qx[2]=LagrangeInterpolation(times,Q3,ttag,err);
               Qx[3]=LagrangeInterpolation(times,Q4,ttag,err);
               // unit Quart at time ttag
                Qx2Unitx(4 ,Qx);
               
               vGOCEattag.second=ttag;
               vGOCEattag.q1=Qx[0];
               vGOCEattag.q2=Qx[1];
               vGOCEattag.q3=Qx[2];
               vGOCEattag.q4=Qx[3];

               
              
         
         }//end of get GOCE attitude value at time ttag   vGOCEattag

         
         //get GOCE reciver offset value at in ICEF
         
         void GOCEroffsetvt(double ttag,vector<GOCEatt> vGOCEatts1,vector<GOCEatt> vGOCEatts2,
                            Triple offsetReciver,Triple &offsetRecivert)
         {
               //get GOCE attitude value at time ttag   vGOCEattag
               GOCEatt         vGOCEattag1, vGOCEattag2;
               
               GetGOCEattime(ttag,vGOCEatts1,vGOCEattag1);
               GetGOCEattime(ttag,vGOCEatts2,vGOCEattag2);
               
               
               double Qx[4];
               double offset1[3],offset2[3];
               double offtmp[3];
               double Rmat[9]={0.0};
               double Rmat1[9]={0.0};
               double Rmat2[9]={0.0};
               //prapare data
                  //  get QX from file1 ,and translate to Rmat1[9]=[3][3]
               Qx[0]=vGOCEattag1.q1;
               Qx[1]=vGOCEattag1.q2;
               Qx[2]=vGOCEattag1.q3;
               Qx[3]=vGOCEattag1.q4;
               Q2Rotation(Qx, Rmat1);
                  //  get QX from file2 ,and translate to Rmat2[9]=[3][3]
               Qx[0]=vGOCEattag2.q1;
               Qx[1]=vGOCEattag2.q2;
               Qx[2]=vGOCEattag2.q3;
               Qx[3]=vGOCEattag2.q4;
               Q2Rotation(Qx, Rmat2);
               
               MatrixMultiplication(Rmat1,Rmat2,Rmat,3,3,3);
               offtmp[0]=offsetReciver[0];
               offtmp[1]=offsetReciver[1];
               offtmp[2]=offsetReciver[2];

               // end of prepare data
               
               
//                 // as Qx character,  choose leftMul or rightMul of Rmat
//               MatrixMultiplication(offtmp,Rmat,offset1,1,3,3);
//               offsetRecivert[0]=offset1[0];
//               offsetRecivert[1]=offset1[1];
//               offsetRecivert[2]=offset1[2];
               
               
               MatrixMultiplication(Rmat,offtmp,offset2,3,3,1);
               offsetRecivert[0]=offset2[0];
               offsetRecivert[1]=offset2[1];
               offsetRecivert[2]=offset2[2];

//
         
         
         }//end of get GOCE reciver offset value at in ICEF
         
         //***********
         //get GOCE reciver offset value at in ICEF
         //to get quick read
         
         void GOCEroffsetvt2(double ttag,vector<GOCEatt> vGOCEatts1,vector<GOCEatt> vGOCEatts2,
                            Triple offsetReciver,Triple &offsetRecivert)
         {
               //*******************
               // choose 9 point for lagrangepoly ,change it as you need;
               int N=9;
               int mid=floor(N/2);
               //*******************
               
               
               vector<double> times(N), Q1(N), Q2(N), Q3(N), Q4(N);
               vector<double> times2(N), Q12(N), Q22(N), Q32(N), Q42(N);
               double err,Qx1[4],Qx2[4];
               
               int line,lengtht;
               int line2,lengtht2;
               double dt,dt2;

               
               
//      clock_t start = clock();
//      cout<< "start time :"<< start/double(CLOCKS_PER_SEC)  <<endl;

               
               //cout<<"ttag"<<ttag<<endl;
               dt=vGOCEatts1[1].second-vGOCEatts1[0].second;
               dt2=vGOCEatts2[1].second-vGOCEatts2[0].second;
               
               line=floor((ttag-vGOCEatts1[0].second)/round(dt));
               line2=floor((ttag-vGOCEatts2[0].second)/round(dt2));
//               
               lengtht=vGOCEatts1.size();
               lengtht2=vGOCEatts2.size();
               
               if ((ttag<vGOCEatts1[N].second )||(ttag > vGOCEatts1[lengtht-N].second ) )
               {
                     
                     cout<<"The GOCE attitude inpoly time  "<<ttag<<" out of file "
                     <<"the time should in "<<vGOCEatts1[N].second
                     << " and " <<vGOCEatts1[lengtht-N].second<<endl;
                     return ;
               }
               
               if ((ttag<vGOCEatts2[N].second )||(ttag > vGOCEatts2[lengtht-N].second ) )
               {
                     
                     cout<<"The GOCE attitude inpoly time  "<<ttag<<" out of file "
                     <<"the time should in "<<vGOCEatts2[N].second
                     << " and " <<vGOCEatts2[lengtht-N].second<<endl;
                     return ;
               }
//      clock_t end = clock();
//      cout<< "Time check ttag:"<< (end-start)/double(CLOCKS_PER_SEC) <<endl;
        
               
               // read data for interpoly
               
               for(int i = 0; i < N; i ++)
               {
                     
                     times[i]=vGOCEatts1[line-mid+i].second;
                     Q1[i]=vGOCEatts1[line-mid+i].q1;
                     Q2[i]=vGOCEatts1[line-mid+i].q2;
                     Q3[i]=vGOCEatts1[line-mid+i].q3;
                     Q4[i]=vGOCEatts1[line-mid+i].q4;
                     
                     
                     times2[i]=vGOCEatts2[line-mid+i].second;
                     Q12[i]=vGOCEatts2[line-mid+i].q1;
                     Q22[i]=vGOCEatts2[line-mid+i].q2;
                     Q32[i]=vGOCEatts2[line-mid+i].q3;
                     Q42[i]=vGOCEatts2[line-mid+i].q4;
                     
                     if(i> 0)
                     {
                           err=Q1[i]*Q1[i-1]+Q2[i]*Q2[i-1]+Q3[i]*Q3[i-1];
                           if(err<0)
                           {
                                 Q1[i]=-Q1[i];
                                 Q2[i]=-Q2[i];
                                 Q3[i]=-Q3[i];
                                 Q4[i]=-Q4[i];
                           }
                           err=0;
                           
                           err=Q12[i]*Q12[i-1]+Q22[i]*Q22[i-1]+Q32[i]*Q32[i-1];
                           if(err<0)
                           {
                                 Q12[i]=-Q12[i];
                                 Q22[i]=-Q22[i];
                                 Q32[i]=-Q32[i];
                                 Q42[i]=-Q42[i];
                           }
                           err=0;

                     }
                     
                     
               }
               
//      clock_t end1 = clock();
//      cout<< "Time parepare for lagrange:"<< (end1-end)/double(CLOCKS_PER_SEC) <<endl;
               //cout<<"ttag"<<ttag<<endl;
               // comtinue quater
               //        if  Qtn(i,1:3)*Qtn(i+1,1:3)'<0
               //        Qtn(i+1,1:4)=-Qtn(i+1,1:4);
               
               Qx1[0]=LagrangeInterpolation(times,Q1,ttag,err);
               Qx1[1]=LagrangeInterpolation(times,Q2,ttag,err);
               Qx1[2]=LagrangeInterpolation(times,Q3,ttag,err);
               Qx1[3]=LagrangeInterpolation(times,Q4,ttag,err);
               Qx2[0]=LagrangeInterpolation(times,Q12,ttag,err);
               Qx2[1]=LagrangeInterpolation(times,Q22,ttag,err);
               Qx2[2]=LagrangeInterpolation(times,Q32,ttag,err);
               Qx2[3]=LagrangeInterpolation(times,Q42,ttag,err);
               // unit Quart at time ttag
               
//      clock_t end2 = clock();
//      cout<< "Time Lagrange:"<< (end2-end1)/double(CLOCKS_PER_SEC) <<endl;
               
               Qx2Unitx(4 ,Qx1);
               Qx2Unitx(4 ,Qx2);
               
//      clock_t end3 = clock();
//      cout<< "Time Qx2Unitx:"<< (end3-end2)/double(CLOCKS_PER_SEC) <<endl;
               
       
               
               
       //        GetGOCEattime(ttag,vGOCEatts1,vGOCEattag1);
       //        GetGOCEattime(ttag,vGOCEatts2,vGOCEattag2);
               
               
               double Qx[4];
               double offset1[3],offset2[3];
               double offtmp[3];
               double Rmat[9]={0.0};
               double Rmat1[9]={0.0};
               double Rmat2[9]={0.0};
               //prapare data
               //  get QX from file1 ,and translate to Rmat1[9]=[3][3]
               Q2Rotation(Qx1, Rmat1);
               //  get QX from file2 ,and translate to Rmat2[9]=[3][3]
               Q2Rotation(Qx2, Rmat2);
//
//      clock_t end4 = clock();
//      cout<< "Time Q2rot:"<< (end4-end3)/double(CLOCKS_PER_SEC) <<endl;
               

               
               
               MatrixMultiplication(Rmat1,Rmat2,Rmat,3,3,3);
               offtmp[0]=offsetReciver[0];
               offtmp[1]=offsetReciver[1];
               offtmp[2]=offsetReciver[2];
               
               // end of prepare data
               
               
               // as Qx character,  choose leftMul or rightMul of Rmat
               MatrixMultiplication(offtmp,Rmat,offset1,1,3,3);
               
               offsetRecivert[0]=offset1[0];
               offsetRecivert[1]=offset1[1];
               offsetRecivert[2]=offset1[2];
               
//      clock_t end5 = clock();
//      cout<< "Time R1*R2:"<< (end5-end4)/double(CLOCKS_PER_SEC) <<endl;

               
               //               MatrixMultiplication(Rmat,offtmp,offset2,3,3,1);
               //               offsetRecivert[0]=offset2[0];
               //               offsetRecivert[1]=offset2[1];
               //               offsetRecivert[2]=offset2[2];
               //
               //
               
               
         }//end of get GOCE reciver offset value at in ICEF
         


   };// end of class

} // namespace gpstk









#endif // GPSTK_RECEIVER_ATT_DATA_HPP
