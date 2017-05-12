// DEMInterpolation.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <math.h>
#include <iostream>
using namespace std;


struct sData 
{
	double X[10];
	double Y[10];
	double Z[10];
}Data;

int _tmain(int argc, _TCHAR* argv[])
{
	int x,y;
	int tempData;
	FILE * fp;
	if((fp  = fopen("data.txt","r")) == NULL)
	{
		printf("ERROR!!!!!!!!!!");
		exit(0);
	}
	else
	{
		for(int i = 0;i < 10;i++)
		{
			fscanf(fp,"%d",&tempData);
			Data.X[i] = tempData;
			fscanf(fp,"%d",&tempData);
			Data.Y[i] = tempData;
			fscanf(fp,"%d",&tempData);
			Data.Z[i] = tempData;
		}
		fscanf(fp,"%d %d",&x,&y);
	}
	fclose(fp);

	/*坐标系改化*/
	for(int i = 0;i < 10;i++)
	{
		Data.X[i] = Data.X[i] - x;
		Data.Y[i] = Data.Y[i] - y;
	}

	/*误差方程*/
	double M[10][6];
	double P[10][10] = {0};

	for(int i = 0;i < 10;i++)
	{
		M[i][0] = Data.X[i] * Data.X[i];
		M[i][1] = Data.X[i] * Data.Y[i];
		M[i][2] = Data.Y[i] * Data.Y[i];
		M[i][3] = Data.X[i];                 
		M[i][4] = Data.Y[i];                 
		M[i][5] = 1;  
		P[i][i] = pow( 10 - sqrt(pow(Data.X[i],2)+pow(Data.Y[i],2)),2 )/( pow(Data.X[i],2)+pow(Data.Y[i],2) );
	}

	/*组成法方程*/
	double Mt[6][10];
	double MtP[6][10];
	double MtPM[6][6];
	double MtPZ[6];

	for(int i = 0;i < 6;i++)
	{
		for(int j = 0;j < 10;j++)
		{
			Mt[i][j] = M[j][i];
		}
	}

	for(int i = 0;i < 6;i++)                //矩阵相乘
	{	
		for(int j = 0;j < 10;j++) 
		{
			MtP[i][j] = 0;
			for(int k = 0;k < 10;k++)
			{
				MtP[i][k] += Mt[i][k] * P[k][k];
			}
		}
	}

	for(int i = 0;i < 6;i++)                //矩阵相乘
	{	
		for(int j = 0;j < 6;j++) 
		{
			MtPM[i][j] = 0;
			MtPZ[i] = 0;
			for(int k = 0;k < 10;k++)
			{
				MtPM[i][j] += MtP[i][k] * M[k][j];
				MtPZ[i] += MtP[i][k] * Data.Z[k];
			}
		}
	}

	/*for(int i = 0;i<6;i++)
		{
			for(int j = 0;j<6;j++)
			{
				cout << MtM[i][j]<<"      ";
			}
			cout << endl;
		}*/


	/*解求6个系数*/
	double N_MtPM[6][6];
	double Tem[6][6];

	int k = 0;                               //矩阵求逆
	for(int i = 0;i < 6;i++)
	{
		for(int j = 0;j < 6;j++)
		{
			if(i == k &&j == k)
				Tem[i][j] = 1 / MtPM[k][k];
			if(i == k && j!= k)
				Tem[i][j] = MtPM[k][j] / MtPM[k][k];
			if(i != k && j == k)
				Tem[i][j] = -(MtPM[i][k] / MtPM[k][k]);
			if(i != k && j != k)
				Tem[i][j] = MtPM[i][j] - MtPM[i][k] * MtPM[k][j] / MtPM[k][k];
		}
	}

	for(k = 1;k < 6;k++)
	{
		for(int i = 0;i < 6;i++)
		{
			for(int j = 0;j < 6;j++)
			{
				if(i == k &&j == k)
					N_MtPM[i][j] = 1 /Tem[k][k];
				if(i == k && j!= k)
					N_MtPM[i][j] = Tem[k][j] / Tem[k][k];
				if(i != k && j == k)
					N_MtPM[i][j] = -(Tem[i][k] / Tem[k][k]);
				if(i != k && j != k)
					N_MtPM[i][j] = Tem[i][j] - Tem[i][k] * Tem[k][j] / Tem[k][k];
			}
		}
		for(int i = 0;i < 6;i++)
			for(int j = 0;j < 6;j++)
				Tem[i][j] = N_MtPM[i][j];

		
		/*for(int i = 0;i<6;i++)
		{
			for(int j = 0;j<6;j++)
			{
				cout << Tem[i][j]<<"      ";
			}
			cout << endl;
		}*/
	}

	double X[6];        //矩阵相乘
    for(int i = 0;i < 6;i++)
	{
		X[i] = 0;
		for(int j = 0;j < 6;j++)
			X[i] += N_MtPM[i][j] * MtPZ[j];
	}

	for(int i = 0;i<6;i++)
			cout << X[i] << endl;
		cout<<endl;


	if((fp  = fopen("DEM内插Result.txt","w")) == NULL)
	{
		printf("ERROR!!!!!!!!!!");
		exit(0);
	}
	else
	{
		fprintf(fp,"DEM内插结果： %lf",X[5]);
	}
	fclose(fp);

	return 0;
}

