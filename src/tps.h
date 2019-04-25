#pragma once
#include<math.h>
#include<string>
#include<iostream>

using namespace std;

double tps_Ur(double x1,double y1,double x2,double y2){
	double r=((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
	if(r==0)
		return 0;
	else
		return r*log(r);
}

double **build_matrix_L(double control_point[68][2],double target_point[68][2]){
	int i,j;
	double **matrix_L=new double*[71];
	for(i=0; i<71; i++)
	{
		matrix_L[i] = new double[71];
	}


	//U_r������
	for(i=0;i<68;i++){
		for(j=0;j<68;j++){
			matrix_L[i][j]=tps_Ur(control_point[i][0],control_point[i][1],control_point[j][0],control_point[j][1]);
		}
	}
	//P����ת��д��L
	for(i=68;i<71;i++){
		for(j=0;j<68;j++){
			if(i==68)
				matrix_L[i][j]=1;
			else if(i==69)
				matrix_L[i][j]=control_point[j][0];
			else
				matrix_L[i][j]=control_point[j][1];
		}
	}
	//Pд��L
	for(i=0;i<68;i++){
		for(j=68;j<71;j++){
			if(j==68)
				matrix_L[i][j]=1;
			else if(j==69)
				matrix_L[i][j]=control_point[i][0];
			else
				matrix_L[i][j]=control_point[i][1];
		}
	}
	//д0����
	for(i=68;i<71;i++){
		for(j=68;j<71;j++){
			matrix_L[i][j]=0;
		}
	}

	for(i=0;i<68;i++)
		matrix_L[i][i]=1;
	return matrix_L;
}

double ** build_matrix_Y(double target_point[68][2]){
	int i;
	//����Y����
	double **matrix_Y=new double*[71];
	for(i=0; i<71; i++)
	{
		matrix_Y[i] = new double[2];
	}
	//����Ŀ���
	for(i=0;i<71;i++){
		if(i<68){
			matrix_Y[i][0]=target_point[i][0];
			matrix_Y[i][1]=target_point[i][1];
		}
		else{
			matrix_Y[i][0]=0;
			matrix_Y[i][1]=0;
		}
	}
	return matrix_Y;
}


//����LU�ֽ�������
double ** soultion(double** matrix_L,double** matrix_Y){
	double sum,total;
	//���������
	double **matrix_soultion=new double*[71];
	for(int i=0; i<71; i++)
	{
		matrix_soultion[i]=new double[2];
	}
	//����L����
	double **L=new double*[71];
	for(int i=0; i<71; i++)
	{
		L[i]=new double[71];
	}
	//����U����
	double **U=new double*[71];
	for(int i=0; i<71; i++)
	{
		U[i]=new double[71];
	}

	//U�����һ��
	for(int i=0;i<71;i++)
	{
		U[0][i]=matrix_L[0][i];
	}
	//L�����һ��
	for(int i=1;i<71;i++)
	{
		L[i][0]=matrix_L[i][0]/U[0][0];
	}
	//��LU�ֽ�
	for(int i=1;i<71;i++)
	{
		for(int j=i;j<71;j++)
		{
			sum=0;
			for(int k=0;k<i;k++)
			{
				sum+=L[i][k]*U[k][j];
			}
			U[i][j]=matrix_L[i][j]-sum;
		}
		for(int j=i+1;j<71;j++)
		{
			total=0;
			for(int k=0;k<i;k++)
			{
				total+=L[j][k]*U[k][i];
			}
			L[j][i]=(matrix_L[j][i]-total)/U[i][i];
		}
	}

	for(int i=0;i<71;i++)
	{
		L[i][i]=1;
	}

	//��US������S�����ǵĽ����,LUS=Y
	double **US=new double*[71];
	for(int i=0;i<71;i++)
	{
		US[i]=new double[2];
	}

	for(int i=0;i<71;i++)
	{
		sum=0;
		for(int j=0;j<i;j++)
		{
			sum+=L[i][j]*US[j][0];
		}
		US[i][0]=matrix_Y[i][0]-sum;

		sum=0;
		for(int j=0;j<i;j++)
		{
			sum+=L[i][j]*US[j][1];
		}
		US[i][1]=matrix_Y[i][1]-sum;
	}
	//U*S=US������

	for(int i=70;i>=0;i--)
	{
		sum=0;
		for(int j=i+1;j<71;j++)
		{
			sum+=U[i][j]*matrix_soultion[j][0];
		}
		matrix_soultion[i][0]=(US[i][0]-sum)/U[i][i];

		sum=0;
		for(int j=i+1;j<71;j++)
		{
			sum+=U[i][j]*matrix_soultion[j][1];
		}
		matrix_soultion[i][1]=(US[i][1]-sum)/U[i][i];
	}
	/*for(int i=0;i<71;i++){
		cout<<matrix_soultion[i][0]<<" "<<matrix_soultion[i][1]<<endl;
	}*/
	return matrix_soultion;
}





//�������˽ⷽ�̣�����Ҫ��tps���κ�����Ӧ���ڱ��κ���ǰ�����������ѽⷽ��������������Ų���ÿһ�ζ�����LU�ֽ�֮�� ��

void TPS_f(int x,int y,double *xtps,double *ytps,double** matrix_soultion,double control_point[68][2]){//������Դ���꣬���κ����꣬�ⷽ��
	int i;
	double sum=0;
	for(i=0;i<68;i++){
		sum+=(matrix_soultion[i][0]*tps_Ur(control_point[i][0],control_point[i][1],x,y));
	}
	*xtps=sum+matrix_soultion[68][0]+x*matrix_soultion[69][0]+y*matrix_soultion[70][0];
	//cout<<*xtps<<" ";
	//��y�仯
	sum=0;
	for(i=0;i<68;i++){
		sum+=(matrix_soultion[i][1]*tps_Ur(control_point[i][0],control_point[i][1],x,y));
	}
	*ytps=sum+matrix_soultion[68][1]+x*matrix_soultion[69][1]+y*matrix_soultion[70][1];
	//cout<<*ytps<<endl;
}


//˫���β�ֵ��S����
double S(double x){
	double s;
	double a=abs(x);
	if(a<=1){
		s=1-2*a*a+a*a*a;
	}
	else if(a>1&&a<2){
		s=4-8*a+5*a*a-a*a*a;
	}
	else
		s=0;
	return s;
}