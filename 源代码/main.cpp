#include<iostream>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/opencv.hpp>
#include<facedetect-dll.h>
#include<math.h>
#include<string>
#include<fstream>
#include<sstream>
#include<vector>
#include"tps.h"

using namespace cv;
using namespace std;

//���建������С
#define DETECT_BUFFER_SIZE 0x20000

int main() 
{  

	cout<<"**************��ӭ������������ϵͳ***************"<<endl<<endl;
	cout<<"�����밴1���˳��밴��������"<<endl;
	string flag;
	cin>>flag;
	if(flag!="1")
		exit(0);
	while(1){

		int*pResults=NULL;
		unsigned char*pBuffer=(unsigned char*)malloc(DETECT_BUFFER_SIZE);
		if(!pBuffer)
		{
			fprintf(stderr,"Cannotallocbuffer.\n");
			return-1;
		}



		//ͼƬ���ֺ͵������ļ�������
		string original_jpg;
		string control_point_txt;
		string target_jpg;
		string target_point_txt;


		//���Ƶ���Ŀ���
		double control_point[68][2];
		double target_point[68][2];


		string filenum;//���

		cout<<"������ԭͼ���(1-9)��";
		cin>>filenum;

		original_jpg=filenum+".jpg";
		target_point_txt=filenum+".txt";//����Ŀ������˼������Ҫ�任��ȥ�ĵ�,֮ǰ�Ĵ�����ǰ�����㷴��

		Mat orig_img=imread(original_jpg);
		Mat gray;//�Ҷ�ͼ����ʶ��

		cvtColor(orig_img,gray,CV_BGR2GRAY);
		//�ҹؼ���
		pResults=facedetect_multiview_reinforce(pBuffer,(unsigned char*)(gray.ptr(0)),gray.cols,gray.rows,(int)gray.step,1.2f,2,48,0,1);
		short*p=((short*)(pResults+1));//�ؼ�����Ϣ


		//�洢�ؼ�����Ϣ
		for(int i=0;i<68;i++){
			target_point[i][0]=(double)p[6+2*i+1];
			target_point[i][1]=(double)p[6+2*i];
		}
		cout<<"������Ŀ��ͼ���(1-9)��";
		cin>>filenum;

		target_jpg=filenum+".jpg";
		control_point_txt=filenum+".txt";

		Mat target_img=imread(target_jpg);
		cvtColor(target_img,gray,CV_BGR2GRAY);

		pResults=facedetect_multiview_reinforce(pBuffer,(unsigned char*)(gray.ptr(0)),gray.cols,gray.rows,(int)gray.step,1.2f,2,48,0,1);
		p=((short*)(pResults+1));


		for(int i=0;i<68;i++){
			control_point[i][0]=(double)p[6+2*i+1];
			control_point[i][1]=(double)p[6+2*i];
		}

		//��ʼ��tps���εĺ���
		//���Ƚ�������L
		double **matrix_L=new double*[71];
		for(int i=0; i<71; i++)
		{
			matrix_L[i] = new double[71];
		}

		matrix_L=build_matrix_L(control_point,target_point);

		//��������Y
		double **matrix_Y=new double*[71];
		for(int i=0; i<71; i++)
		{
			matrix_Y[i] = new double[2];
		}

		matrix_Y=build_matrix_Y(target_point);


		//��ⷽ��
		double **matrix_soultion=new double*[71];
		for(int i=0; i<71; i++)
		{
			matrix_soultion[i] = new double[2];
		}
		matrix_soultion=soultion(matrix_L,matrix_Y);


		//���洦��ͼƬ��Ϣ
		//�й�ԭͼ
		int original_row,original_col;

		original_row=orig_img.rows;
		original_col=orig_img.cols;

		vector<Mat> channels;
		//�洢ԭͼ��RGB��Ϣ
		split(orig_img,channels);
		Mat orig_R, orig_G, orig_B;

		orig_B=channels.at(0);
		orig_G=channels.at(1);
		orig_R=channels.at(2);


		//��ȡĿ��ͼƬ,������Ϣ
		//Mat target_img=imread(target_jpg);
		int target_row,target_col;
		target_row=target_img.rows;
		target_col=target_img.cols;

		//�����ϳ�ͼƬ
		Mat out_img(target_row,target_col,CV_8UC3);
		vector<Mat>channels2;
		split(out_img,channels2);

		Mat out_R,out_G,out_B;
		out_B=channels2.at(0);
		out_G=channels2.at(1);
		out_R=channels2.at(2);
		for(int i=0;i<target_row;i++){
			for(int j=0;j<target_col;j++){
				out_B.at<unsigned char>(i,j)=0;
				out_G.at<unsigned char>(i,j)=0;
				out_R.at<unsigned char>(i,j)=0;
			}
		}
		//���в�ֵ
		int kind;//��ֵ��ʽ
		double tpsx=0,tpsy=0;//�任������
		double u,v;
		int I,J;//���ڲ�ֵ


		//˫���εĲ�ֵ��ʽ
		double A[4]={0};
		double C[4]={0};
		double BC_B[4]={0};
		double BC_G[4]={0};
		double BC_R[4]={0};
		int result[3]={0};

		cout<<"ѡ���ֵ��ʽ"<<endl<<"1.����ڲ�ֵ"<<endl<<"2.˫���Բ�ֵ"<<endl<<"3.˫���β�ֵ"<<endl;
		cin>>kind;

		//��ֵ
		for(int i=0;i<target_row;i++){
			for(int j=0;j<target_col;j++){

				tpsx=0,tpsy=0;
				TPS_f(i,j,&tpsx,&tpsy,matrix_soultion,control_point);

				if((tpsx>0)&&(tpsx<original_row-1)&&(tpsy>0)&&(tpsy<original_col-1)){

					I=floor(tpsx);
					u=tpsx-I;
					J=floor(tpsy);
					v=tpsy-J;

					if(kind==1){//�����
						if(u>0.5)
							I++;
						if(v>0.5)
							J++;
						out_B.at<unsigned char>(i,j) = orig_B.at<unsigned char>(I,J);
						out_G.at<unsigned char>(i,j) = orig_G.at<unsigned char>(I,J);
						out_R.at<unsigned char>(i,j) = orig_R.at<unsigned char>(I,J);
					}

					else if(kind==2){//˫����

						out_B.at<unsigned char>(i,j)=(orig_B.at<unsigned char>(I,J)*(1-v)+v*orig_B.at<unsigned char>(I,J+1))*(1-u)+((1-v)*orig_B.at<unsigned char>(I+1,J)+v*orig_B.at<unsigned char>(I+1,J+1))*u;
						out_G.at<unsigned char>(i,j)=(orig_G.at<unsigned char>(I,J)*(1-v)+v*orig_G.at<unsigned char>(I,J+1))*(1-u)+((1-v)*orig_G.at<unsigned char>(I+1,J)+v*orig_G.at<unsigned char>(I+1,J+1))*u;
						out_R.at<unsigned char>(i,j)=(orig_R.at<unsigned char>(I,J)*(1-v)+v*orig_R.at<unsigned char>(I,J+1))*(1-u)+((1-v)*orig_R.at<unsigned char>(I+1,J)+v*orig_R.at<unsigned char>(I+1,J+1))*u;
					}
					else{//˫����


						result[0]=0;
						result[1]=0;
						result[2]=0;
						for(int p=0;p<4;p++){
							BC_B[p]=0;
							BC_G[p]=0;
							BC_R[p]=0;
						}

						if((I-1)>=0&&I<(original_row-2)&&(J-1)>0&&J<(original_col-2)){

							C[0]=S(1+u);
							C[1]=S(u);
							C[2]=S(1-u);
							C[3]=S(2-u);

							A[0]=S(1+v);
							A[1]=S(v);
							A[2]=S(1-v);
							A[3]=S(2-v);
							
							for(int p=0;p<4;p++){
								for(int q=0;q<4;q++){
									BC_B[p]+=orig_B.at<unsigned char>(I-1+p,J-1+q)*C[q];
									BC_G[p]+=orig_G.at<unsigned char>(I-1+p,J-1+q)*C[q];
									BC_R[p]+=orig_R.at<unsigned char>(I-1+p,J-1+q)*C[q];
								}

							}

							for(int p=0;p<4;p++){
								result[0]+=BC_B[p]*A[p];
								result[1]+=BC_G[p]*A[p];
								result[2]+=BC_R[p]*A[p];
							}
							//cout<<(int)out_B.at<unsigned char>(i,j)<<" ";
							//�п���������RGBֵ����255��������������Ϲ淶���͵��ڸ����ĵ�
							//������˫���������Ȩ����Ӧ�ò��ᳬ��255������Ӧ������������������֮���ʹ�䳬����Χ
							if(result[0]>=0&&result[0]<=255&&result[1]>=0&&result[1]<=255&&result[2]>=0&&result[2]<=255){
								out_B.at<unsigned char>(i,j)=result[0];
								out_G.at<unsigned char>(i,j)=result[1];
								out_R.at<unsigned char>(i,j)=result[2];
							}
							else{//������Χ������ڣ�����ʵ��Ч����û����ô�ã��������ڱ߽紦
								if(u>0.5)
									I++;
								if(v>0.5)
									J++;
								out_B.at<unsigned char>(i,j) = orig_B.at<unsigned char>(I,J);
								out_G.at<unsigned char>(i,j) = orig_G.at<unsigned char>(I,J);
								out_R.at<unsigned char>(i,j) = orig_R.at<unsigned char>(I,J);
							}
						}
					}
				}
			}
		}

		//ɾ��ָ��
		for(int i=0;i<71;i++){
			delete matrix_L[i];
			delete matrix_Y[i];
			delete matrix_soultion[i];
		}

		delete matrix_L;
		delete matrix_Y;
		delete matrix_soultion;

		cout<<"���γɹ�"<<endl;

		//���ͼƬ
		cvNamedWindow("ԭͼ");
		imshow("ԭͼ",orig_img);

		cvNamedWindow("Ŀ��ͼ");
		imshow("Ŀ��ͼ",target_img);


		channels2.at(0)=out_B;
		channels2.at(1)=out_G;
		channels2.at(2)=out_R;
		merge(channels2,out_img);
		cvNamedWindow("�ϳ�ͼ");
		imshow("�ϳ�ͼ",out_img);
		waitKey(0);

		cout<<"�Ƿ񱣴棿��1���棬�����水��������";
		cin>>flag;
		if(flag=="1"){
			string imgname;

			cout<<"������ͼƬ����";
			cin>>imgname;
			imgname=imgname+".jpg";
			imwrite(imgname,out_img);
		}
		cout<<"�Ƿ��뱣�������ؼ�����Ϣ�������밴1���������밴��������";
		cin>>flag;
		if(flag=="1"){
			ofstream output;
			output.open(target_point_txt,std::ios::out|std::ios::app);//��д������ļ�ĩβ��ӵķ�ʽ��.txt�ļ���û�еĻ��ʹ������ļ���
			if(!output.is_open())
				return 0;
			fstream file(target_point_txt,ios::out);//�����������

			for(int i=0;i<68;i++){
				output<<target_point[i][1]<<" "<<target_point[i][0]<<endl;
				//�������ؼ�����
				//circle(src,Point((int)p[6+2*i],(int)p[6+2*i+1]),1,Scalar(0,0,255),2);
			}
			output.close();

			output.open(control_point_txt,std::ios::out|std::ios::app);//��д������ļ�ĩβ��ӵķ�ʽ��.txt�ļ���û�еĻ��ʹ������ļ���
			if(!output.is_open())
				return 0;
			fstream file1(control_point_txt,ios::out);//�����������

			for(int i=0;i<68;i++){
				output<<control_point[i][1]<<" "<<control_point[i][0]<<endl;
				//�������ؼ�����
				//circle(src,Point((int)p[6+2*i],(int)p[6+2*i+1]),1,Scalar(0,0,255),2);
			}
			output.close();


		}

		cout<<"����������1���˳�����������"<<endl;
		cin>>flag;
		if(flag!="1")
			break;


	}
	return 0;
}

