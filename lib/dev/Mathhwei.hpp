#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include "Miscmath.hpp"


using namespace std;
//***************************************************
//function for quart by  hwei



// find max value of a[] ,and it's position
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
            
            double tmp=pow(Qx[0],2)+pow(Qx[1],2)+pow(Qx[2],2)+pow(Qx[3],2);
            
            if (fabs((tmp-1.0))>1.0e-8)
            {
                  cout<<"Qx power 2 is "<<tmp<<" shouble be  1"<<endl;
                  cout<<"Qx error more than 1.0E-8"<<endl;
                  
                  //  x into unitx
                  Qx[0]=Qx[0]/sqrt(tmp);
                  Qx[1]=Qx[1]/sqrt(tmp);
                  Qx[2]=Qx[2]/sqrt(tmp);
                  Qx[3]=Qx[3]/sqrt(tmp);
                  
            }
      }
      else
      {
            
      }
      
      return 0.0;
      
}//end of Qx2Unix

// translate quartern to rotation matrix in RX[9]

void Q2Rotation(double *Q, double Rx[9])
{
      
      
      double Rmat[3][3];
      
      
      Rmat[0][0]=pow(Q[0],2)-pow(Q[1],2)-pow(Q[2],2)+pow(Q[3],2);
      // =1.D0-2*Q2**2-2*Q3**2
      Rmat[0][1]=2.0*(Q[0]*Q[1]-Q[2]*Q[3]);
      Rmat[0][2]=2.0*(Q[0]*Q[2]+Q[1]*Q[3]);
      Rmat[1][0]=2.0*(Q[0]*Q[1]+Q[2]*Q[3]);
      Rmat[1][1]=-pow(Q[0],2)+pow(Q[1],2)-pow(Q[2],2)+pow(Q[3],2);
      //=1.D0-2*Q1**2-2*Q3**2
      Rmat[1][2]=2.0*(Q[1]*Q[2]-Q[0]*Q[3]);
      Rmat[2][0]=2.0*(Q[0]*Q[2]-Q[1]*Q[3]);
      Rmat[2][1]=2.0*(Q[1]*Q[2]+Q[0]*Q[3]);
      Rmat[2][2]=-pow(Q[0],2)-pow(Q[1],2)+pow(Q[2],2)+pow(Q[3],2);
      //=1.D0-2*Q1**2-2*Q2**2
      for (int i=0 ;i<3;i++)
      {
            Rx[i]=Rmat[0][i];
            Rx[i+3]=Rmat[1][i];
            Rx[i+6]=Rmat[2][i];
      }
      
      
      
}// end of translate quartern to rotation matrix


// RX[9] translate quartern to rotation matrix

void Rotation2Q(double Rx[9],double *Q)
{
      
      double R[3][3];
      double QR[4],R1,R2,R3,R4,tmp;
      int CR;
      
      for (int i=0 ;i<3;i++)
      {
            R[0][i]=Rx[i];
            R[1][i]=Rx[i+3];
            R[2][i]=Rx[i+6];
      }
      
      QR[3]=R[0][0]+R[1][1]+R[2][2];
      QR[0]=R[0][0];
      QR[1]=R[1][1];
      QR[2]=R[2][2];
      maxaloc(QR,4,CR,tmp);
      
      
      if (CR==0)
      {
            R1=1.0+R[0][0]-R[1][1]-R[2][2];
            Q[0]=0.5*sqrt(R1);
            Q[1]=0.25*(R[0][1]+R[1][0])/Q[0];
            Q[2]=0.25*(R[0][2]+R[2][0])/Q[0];
            Q[3]=0.25*(R[2][1]-R[1][2])/Q[0];
      }
      
      if (CR==1)
      {
            R2=1.0-R[0][0]+R[1][1]-R[2][2];
            Q[1]=0.5*sqrt(R2);
            Q[0]=0.25*(R[0][1]+R[1][0])/Q[1];
            Q[2]=0.25*(R[1][2]+R[2][1])/Q[1];
            Q[3]=0.25*(R[0][2]-R[2][0])/Q[1];
      }
      if (CR==2)
      {
            R3=1.0-R[0][0]-R[1][1]+R[2][2];
            Q[2]=0.5*sqrt(R3);
            Q[0]=0.25*(R[0][2]+R[2][0])/Q[2];
            Q[1]=0.25*(R[1][2]+R[2][1])/Q[2];
            Q[3]=0.25*(R[1][0]-R[0][1])/Q[2];
      }
      if (CR==3)
      {
            R4=1.0+R[0][0]+R[1][1]+R[2][2];
            Q[3]=0.5*sqrt(R4);
            Q[0]=0.25*(R[2][1]-R[1][2])/Q[3];
            Q[1]=0.25*(R[0][2]-R[2][0])/Q[3];
            Q[2]=0.25*(R[1][0]-R[0][1])/Q[3];
      }
      
      
      
      
}// end of translate quartern to rotation matrix


void MatrixMultiplication(double *matrixA,double *matrixB,double *matrixAB,int i,int j,int k)
{
      int m,n,s;
      for (m=0;m<i;m++)
      {
            for (n=0;n<k;n++)
            {
                  matrixAB[m*k+n]=0;
                  for (s=0;s<j;s++)
                  {
                        matrixAB[m*k+n]+=matrixA[m*j+s]*matrixB[s*k+n];
                  }
            }
      }
}






