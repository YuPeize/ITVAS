#include "cv.h"  
#include <opencv2\opencv.hpp>  
#include "highgui.h"  
#include <ml.h>  
#include <iostream>  
#include <fstream>  
#include <string>  
#include <vector> 
#include <windows.h>

using namespace cv;
using namespace std;
using namespace ml;


/***************************************************************************
�������ܣ�ѵ��ͼ��
����˵����
���أ�
���ߣ�jipeng huang
ʱ�䣺2016-9-29
�޸ģ�
***************************************************************************/
void Train()
{
	vector<string> imagePath;
	vector<int> imageClass;
	int numberOfLine = 0;
	string buffer;
	ifstream trainingData(string("E:\\RE-ID\\ѵ������\\����\\") + "trainFileNameLIst.txt");
	unsigned long n;

	while (!trainingData.eof())
	{
		getline(trainingData, buffer);
		if (!buffer.empty())
		{
			++numberOfLine;
			if (numberOfLine % 2 == 0)
				imageClass.push_back(atoi(buffer.c_str()));//��ȡ�������
			else
				imagePath.push_back(buffer);//��ȡͼ��·��
		}
	} 
	trainingData.close();
	//����������������
	int numberOfSample = numberOfLine / 2;
	Mat featureVectorOfSample(numberOfSample, 1764, CV_32FC1);//������ÿ��Ϊһ������
	Mat classOfSample(numberOfSample, 1, CV_32SC1);//���������
	Mat convertedImage;
	Mat trainImage;
	// ����HOG����
	HOGDescriptor hog(cvSize(64, 64), cvSize(16, 16), cvSize(8, 8), cvSize(8, 8), 9);
	for (vector<string>::size_type i = 0; i <= imagePath.size() - 1; i++)
	{
		Mat src = imread(imagePath[i], -1);
		if (src.empty())
		{
			cout << "can not load the image:" << imagePath[i] << endl;
			continue;
		}
		// ��һ��
		resize(src, trainImage, Size(64, 64));
		// ��ȡHOG����
		vector<float> descriptors;
		double time1 = getTickCount();
		hog.compute(trainImage, descriptors);//����������ü�ⴰ�ڲ��������ͼƬ��С����64��128����������winStride
		double time2 = getTickCount();
		double elapse_ms = (time2 - time1) * 1000 / getTickFrequency();
		//���浽��������������
		for (vector<float>::size_type j = 0; j < descriptors.size(); j++)
		{
			//cout << j << " " << descriptors.size() << endl;
			//cout << descriptors[j] << endl;
			featureVectorOfSample.at<float>(i, j) = descriptors[j];
			
		}
		//�������������
		//!!ע���������һ��Ҫ��int ���͵�
		classOfSample.at<int>(i, 0) = imageClass[i];
	}    
	Ptr<SVM> svm = SVM::create();
	svm->setType(SVM::C_SVC);
	svm->setKernel(SVM::LINEAR);//ע�����ʹ������SVM����ѵ������ΪHogDescriptor��⺯��ֻ֧�����Լ�⣡����
	svm->setTermCriteria(TermCriteria(CV_TERMCRIT_ITER, 1000, FLT_EPSILON));        
	svm->train(featureVectorOfSample, ROW_SAMPLE, classOfSample);
	//���������(���������SVM�Ĳ�����֧������,����rho)
	svm->save(string("E:\\RE-ID\\ѵ������\\����\\") + "Classifier3.xml");
	/*
	SVMѵ����ɺ�õ���XML�ļ����棬��һ�����飬����support vector������һ�����飬����alpha,��һ��������������rho;
	��alpha����ͬsupport vector��ˣ�ע�⣬alpha*supportVector,���õ�һ������������������ǰ�����-1��֮���ٸ���������������һ��Ԫ��rho��
	��ˣ���õ���һ�������������ø÷�������ֱ���滻opencv�����˼��Ĭ�ϵ��Ǹ���������cv::HOGDescriptor::setSVMDetector()����
	*/
	//��ȡ֧��������������Ĭ����CV_32F
	Mat supportVector = svm->getSupportVectors();
	//��ȡalpha��rho
	Mat alpha;//ÿ��֧��������Ӧ�Ĳ�����(�������ճ���)��Ĭ��alpha��float64��
	Mat svIndex;//֧���������ڵ�����
	float rho = svm->getDecisionFunction(0, alpha, svIndex);
	//ת������:����һ��Ҫע�⣬��Ҫת��Ϊ32��
	Mat alpha2;
	alpha.convertTo(alpha2, CV_32FC1);
	//������������������
	const int col = 1764;
	Mat result(1, col, CV_32FC1);
	result = alpha2*supportVector;
	//ע����Ϊsvm.predictʹ�õ���alpha*sv*another-rho�����Ϊ���Ļ�����Ϊ������������HOG�ļ�⺯���У�ʹ��rho+alpha*sv*another(anotherΪ-1)
	for (int i = 0; i < col; i++)
		result.at<float>(0, i) *= -1;
	//�����������浽�ļ�������HOGʶ��
	//��������������б����Ĳ���(��)��HOG����ֱ��ʹ�øò�������ʶ��
	FILE *fp = fopen((string("E:\\RE-ID\\ѵ������\\����\\") + "HOG_SVM3.txt").c_str(), "wb");
	for (int i = 0; i < col; i++)
	{
		fprintf(fp, "%f \n", result.at<float>(0, i));
	}
	fprintf(fp, "%f", rho);
	fclose(fp);
}

void DetectionAndTracking(string path)
{
	Mat dst;
	VideoCapture cap(path);
	//����ѵ���õ��б����Ĳ���(ע�⣬��svm->save����ķ�������ͬ)
	vector<float> detector;
	ifstream fileIn(string("E:\\RE-ID\\ѵ������\\����\\") + "HOG_SVM.txt", ios::in);
	float val = 0.0f;
	while (!fileIn.eof())
	{
		fileIn >> val;
		detector.push_back(val);
	}
	fileIn.close();
	//����HOG
	HOGDescriptor hog(cvSize(128, 128), cvSize(16, 16), cvSize(8, 8), cvSize(8, 8), 9);
	hog.setSVMDetector(detector);// ʹ���Լ�ѵ���ķ�����
	namedWindow("car detector", WINDOW_NORMAL);
	Mat src;
	for (int frameCnt = 1;; frameCnt++)
	{
		cap >> src;
		if (src.empty())break; //�������һ֡
		resize(src, dst, Size(src.cols, src.rows));//1.5 3.75
		fflush(stdout);
		vector<Rect> found, found_filtered;
		double t = (double)getTickCount();
		//��߶ȼ��
		hog.detectMultiScale(dst, found, 1.2, Size(16, 16), Size(0, 0), 2, 5);//1.1 1.3 2
		t = (double)getTickCount() - t;
		printf("detection time = %gms\n", t*1000. / cv::getTickFrequency());
		size_t i, j;

		//ȥ���ռ��о������������ϵ�����򣬱������
		vector<Rect>::iterator it = found.begin();
		for (; it != found.end();)
		{
			if ((*it).height > 250)
				found.erase(it);
			else
				it++;
		}
		for (i = 0; i < found.size(); i++)
		{
			Rect r = found[i];
			for (j = 0; j < found.size(); j++)
			if (j != i && (r & found[j]) == r)
				break;
			if (j == found.size())
				found_filtered.push_back(r);
		}
		// �ʵ���С����
		for (i = 0; i < found_filtered.size(); i++)
		{
			Rect r = found_filtered[i];
			r.x += cvRound(r.width*0.1);
			//r.x *= 3.75;
			r.width = cvRound(r.width*0.8);
			//r.width *= 1.5;
			r.y += cvRound(r.height*0.07);
			r.height = cvRound(r.height*0.8);
			//cout << r.height << " " << r.width << endl;
			rectangle(dst, r.tl(), r.br(), cv::Scalar(0, 255, 0), 3);
			//rectangle(dst, r.tl(), r.br(), cv::Scalar(0, 255, 0), 3);
		}
		imshow("car detector", dst);
		waitKey(1);
		//int c = waitKey(0) & 255;
		//if (c == 'q' || c == 'Q' )
			//break;
		//writer << img;
		
	}
}
/*
int main()
{
	//Train();
	//cout << "train finish!" << endl;
	DetectionAndTracking("E:\\RE - ID\\���ݼ�\\����\\video\\20160713\\DJI_0003.mov");
	return 0;
}
*/