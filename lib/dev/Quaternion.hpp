



#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include "Miscmath.hpp"
#include "Matrix.hpp"



//***************************************************
using namespace std;
namespace gpstk
{

      // find max value of a[] ,and it's position
      //  need for quaternion compute. min error.
      void maxaloc(double a[],int length,int &m,double & maxv)
      {
            m=0;maxv=a[m];
            for(int i=0;i<length ;i++)
            {
                  if(a[i]>a[m])
                        m=i;
                  maxv=a[m];
            }
      }// end of macaloc





      //  Qx into unitx

      double Qx2Unitx(int n ,double *Qx)
      {
            
            
            if (n==4)
            {
                  
                  double tmp=std::pow(Qx[0],2)+std::pow(Qx[1],2)+std::pow(Qx[2],2)+std::pow(Qx[3],2);
                  
                  if (fabs((tmp-1.0))>1.0e-8)
                  {
                        cout<<"Qx power 2 is "<<tmp<<" shouble be  1"<<endl;
                        cout<<"Qx error more than 1.0E-8"<<endl;
                        
                        //  x into unitx
                        Qx[0]=Qx[0]/std::sqrt(tmp);
                        Qx[1]=Qx[1]/std::sqrt(tmp);
                        Qx[2]=Qx[2]/std::sqrt(tmp);
                        Qx[3]=Qx[3]/std::sqrt(tmp);
                        
                  }
            }
            else
            {
                  
            }
            
            return 0.0;
            
      }//end of Qx2Unix

      // translate quartern to rotation matrix in RX[9]

      void Q2Rotation(double *Q, Matrix<double> Rx)
      {
            
            
            Rx.resize(3,3,0.0);
            
            Rx(0,0)=std::pow(Q[0],2)-std::pow(Q[1],2)-std::pow(Q[2],2)+std::pow(Q[3],2);
            // =1.D0-2*Q2**2-2*Q3**2
            Rx(0,1)=2.0*(Q[0]*Q[1]-Q[2]*Q[3]);
            Rx(0,2)=2.0*(Q[0]*Q[2]+Q[1]*Q[3]);
            Rx(1,0)=2.0*(Q[0]*Q[1]+Q[2]*Q[3]);
            Rx(1,1)=-std::pow(Q[0],2)+std::pow(Q[1],2)-std::pow(Q[2],2)+std::pow(Q[3],2);
            //=1.D0-2*Q1**2-2*Q3**2
            Rx(1,2)=2.0*(Q[1]*Q[2]-Q[0]*Q[3]);
            Rx(2,0)=2.0*(Q[0]*Q[2]-Q[1]*Q[3]);
            Rx(2,1)=2.0*(Q[1]*Q[2]+Q[0]*Q[3]);
            Rx(2,2)=-std::pow(Q[0],2)-std::pow(Q[1],2)+std::pow(Q[2],2)+std::pow(Q[3],2);
            //=1.D0-2*Q1**2-2*Q2**2

            
      }// end of translate quartern to rotation matrix


      // RX[9] translate quartern to rotation matrix

      void Rotation2Q(Matrix<double> Rx,double *Q)
      {

            double QR[4],R1,R2,R3,R4,tmp;
            int CR;
            //Rx.size(3,3,0.0);
            
            QR[3]=Rx(0,0)+Rx(1,1)+Rx(2,2);
            QR[0]=Rx(0,0);
            QR[1]=Rx(1,1);
            QR[2]=Rx(2,2);
            
            // find max value in Quaternion for compute
            maxaloc(QR,4,CR,tmp);
            
            
            if (CR==0)
            {
                  R1=1.0+Rx(0,0)-Rx(1,1)-Rx(2,2);
                  Q[0]=0.5*std::sqrt(R1);
                  Q[1]=0.25*(Rx(0,1)+Rx(1,0))/Q[0];
                  Q[2]=0.25*(Rx(0,2)+Rx(2,0))/Q[0];
                  Q[3]=0.25*(Rx(2,1)+Rx(1,2))/Q[0];
            }
            
            if (CR==1)
            {
                  R2=1.0-Rx(0,0)+Rx(1,1)-Rx(2,2);
                  Q[1]=0.5*std::sqrt(R2);
                  Q[0]=0.25*(Rx(0,1)+Rx(1,0))/Q[1];
                  Q[2]=0.25*(Rx(2,1)+Rx(1,2))/Q[1];
                  Q[3]=0.25*(Rx(0,2)-Rx(2,0))/Q[1];
            }
            if (CR==2)
            {
                  R3=1.0-Rx(0,0)-Rx(1,1)+Rx(2,2);
                  Q[2]=0.5*std::sqrt(R3);
                  Q[0]=0.25*(Rx(0,2)+Rx(2,0))/Q[2];
                  Q[1]=0.25*(Rx(1,2)+Rx(2,1))/Q[2];
                  Q[3]=0.25*(Rx(1,0)-Rx(0,1))/Q[2];
            }
            if (CR==3)
            {
                  R4=1.0+Rx(0,0)+Rx(1,1)+Rx(2,2);
                  Q[3]=0.5*std::sqrt(R4);
                  Q[0]=0.25*(Rx(2,1)-Rx(1,2))/Q[3];
                  Q[1]=0.25*(Rx(0,2)-Rx(2,0))/Q[3];
                  Q[2]=0.25*(Rx(1,0)-Rx(0,1))/Q[3];
            }
            
      }// end of translate quartern to rotation matrix

} //end of namespace gpstk





