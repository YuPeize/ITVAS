// test.cpp : �������̨Ӧ�ó������ڵ㡣
//
/************************DLL_test*********************************/
/*#include "stdafx.h"
#include "reid_persondetection.h"
#include <iostream>
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	int a = 1;
	int b = 2;
	cout << add(a, b) << endl;
	return 0;
}

*/

/************************opencv_test******************************/
/*#include <stdafx.h>
#include <opencv2\opencv.hpp>  
#include <iostream>  
#include <string>  
using namespace cv;
using namespace std;
int main()
{    
	Mat img = imread("C://Users//admin//Desktop//����.jpg");    
	if (img.empty())    
	{        
		cout << "error";        
		return -1;    
	}    
	imshow("mypic", img);    
	waitKey();    
	return 0;
} 
*/

#include "stdafx.h"  
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

/*
void Train()
{
	string FILEPATH = "";
	////////////////////////////////����ѵ������ͼƬ·�������///////////////////////////////////////////////////
	//ͼ��·�������
	vector<string> imagePath;
	vector<int> imageClass;
	int numberOfLine = 0;
	string buffer;
	ifstream trainingData(string(FILEPATH) + "TrainData.txt");
	unsigned long n;

	while (!trainingData.eof())
	{
		getline(trainingData, buffer);
		if (!buffer.empty())
		{
			++numberOfLine;
			if (numberOfLine % 2 == 0)
			{
				//��ȡ�������
				imageClass.push_back(atoi(buffer.c_str()));
			}
			else
			{
				//��ȡͼ��·��
				imagePath.push_back(buffer);
			}
		}
	}

	//�ر��ļ�  
	trainingData.close();


	////////////////////////////////��ȡ������HOG����///////////////////////////////////////////////////
	//����������������
	int numberOfSample = numberOfLine / 2;
	Mat featureVectorOfSample(numberOfSample, 3780, CV_32FC1);//������ÿ��Ϊһ������

	//���������
	Mat classOfSample(numberOfSample, 1, CV_32SC1);

	Mat convertedImage;
	Mat trainImage;

	// ����HOG����
	for (vector<string>::size_type i = 0; i <= imagePath.size() - 1; ++i)
	{
		//����ͼƬ
		Mat src = imread(imagePath[i], -1);
		if (src.empty())
		{
			cout << "can not load the image:" << imagePath[i] << endl;
			continue;
		}
		//cout << "processing:" << imagePath[i] << endl;

		// ��һ��
		resize(src, trainImage, Size(64, 128));

		// ��ȡHOG����
		HOGDescriptor hog(cvSize(64, 128), cvSize(16, 16), cvSize(8, 8), cvSize(8, 8), 9);
		vector<float> descriptors;
		double time1 = getTickCount();
		hog.compute(trainImage, descriptors);//����������ü�ⴰ�ڲ��������ͼƬ��С����64��128����������winStride
		double time2 = getTickCount();
		double elapse_ms = (time2 - time1) * 1000 / getTickFrequency();
		//cout << "HOG dimensions:" << descriptors.size() << endl;
		//cout << "Compute time:" << elapse_ms << endl;


		//���浽��������������
		for (vector<float>::size_type j = 0; j <= descriptors.size() - 1; ++j)
		{
			featureVectorOfSample.at<float>(i, j) = descriptors[j];
		}

		//�������������
		//!!ע���������һ��Ҫ��int ���͵�
		classOfSample.at<int>(i, 0) = imageClass[i];
	}


	///////////////////////////////////ʹ��SVM������ѵ��///////////////////////////////////////////////////    
	//���ò�����ע��Ptr��ʹ��
	Ptr<SVM> svm = SVM::create();
	svm->setType(SVM::C_SVC);
	svm->setKernel(SVM::LINEAR);//ע�����ʹ������SVM����ѵ������ΪHogDescriptor��⺯��ֻ֧�����Լ�⣡����
	svm->setTermCriteria(TermCriteria(CV_TERMCRIT_ITER, 1000, FLT_EPSILON));

	//ʹ��SVMѧϰ         
	svm->train(featureVectorOfSample, ROW_SAMPLE, classOfSample);

	//���������(���������SVM�Ĳ�����֧������,����rho)
	svm->save(string(FILEPATH) + "Classifier.xml");

	
	//SVMѵ����ɺ�õ���XML�ļ����棬��һ�����飬����support vector������һ�����飬����alpha,��һ��������������rho;
	//��alpha����ͬsupport vector��ˣ�ע�⣬alpha*supportVector,���õ�һ������������������ǰ�����-1��֮���ٸ���������������һ��Ԫ��rho��
	//��ˣ���õ���һ�������������ø÷�������ֱ���滻opencv�����˼��Ĭ�ϵ��Ǹ���������cv::HOGDescriptor::setSVMDetector()����
	
	//��ȡ֧��������������Ĭ����CV_32F
	Mat supportVector = svm->getSupportVectors();//

	//��ȡalpha��rho
	Mat alpha;//ÿ��֧��������Ӧ�Ĳ�����(�������ճ���)��Ĭ��alpha��float64��
	Mat svIndex;//֧���������ڵ�����
	float rho = svm->getDecisionFunction(0, alpha, svIndex);

	//ת������:����һ��Ҫע�⣬��Ҫת��Ϊ32��
	Mat alpha2;
	alpha.convertTo(alpha2, CV_32FC1);

	//������������������
	Mat result(1, 3780, CV_32FC1);
	result = alpha2*supportVector;

	//����-1������Ϊʲô�����-1��
	//ע����Ϊsvm.predictʹ�õ���alpha*sv*another-rho�����Ϊ���Ļ�����Ϊ������������HOG�ļ�⺯���У�ʹ��rho+alpha*sv*another(anotherΪ-1)
	for (int i = 0; i < 3780; ++i)
		result.at<float>(0, i) *= -1;

	//�����������浽�ļ�������HOGʶ��
	//��������������б����Ĳ���(��)��HOG����ֱ��ʹ�øò�������ʶ��
	FILE *fp = fopen((string(FILEPATH) + "HOG_SVM.txt").c_str(), "wb");
	for (int i = 0; i<3780; i++)
	{
		fprintf(fp, "%f \n", result.at<float>(0, i));
	}
	fprintf(fp, "%f", rho);

	fclose(fp);

}
*/
// ʹ��ѵ���õķ�����ʶ��
void Detect()
{
	string FILEPATH = "";
	Mat img;
	FILE* f = 0;
	char _filename[1024];

	// ��ȡ����ͼƬ�ļ�·��
	/*f = fopen((string(FILEPATH) + "TestData.txt").c_str(), "rt");
	if (!f)
	{
		fprintf(stderr, "ERROR: the specified file could not be loaded\n");
		return;
	}

	//����ѵ���õ��б����Ĳ���(ע�⣬��svm->save����ķ�������ͬ)
	vector<float> detector;
	ifstream fileIn(string(FILEPATH) + "HOG_SVM.txt", ios::in);
	float val = 0.0f;
	while (!fileIn.eof())
	{
		fileIn >> val;
		detector.push_back(val);
	}
	fileIn.close();
	*/
	HOGDescriptor hog;
	//hog.setSVMDetector(detector);// ʹ���Լ�ѵ���ķ�����
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());//����ֱ��ʹ��05 CVPR��ѵ���õķ�����,�����Ͳ���Train()���������
	namedWindow("people detector", 1);

	// ���ͼƬ
	for (;;)
	{
		// ��ȡ�ļ���
		char* filename = _filename;
		if (f)
		{
			if (!fgets(filename, (int)sizeof(_filename)-2, f))
				break;
			//while(*filename && isspace(*filename))
			//  ++filename;
			if (filename[0] == '#')
				continue;

			//ȥ���ո�
			int l = (int)strlen(filename);
			while (l > 0 && isspace(filename[l - 1]))
				--l;
			filename[l] = '\0';
			img = imread(filename);
		}
		printf("%s:\n", filename);
		if (!img.data)
			continue;

		fflush(stdout);
		vector<Rect> found, found_filtered;
		double t = (double)getTickCount();
		// run the detector with default parameters. to get a higher hit-rate
		// (and more false alarms, respectively), decrease the hitThreshold and
		// groupThreshold (set groupThreshold to 0 to turn off the grouping completely).
		//��߶ȼ��
		hog.detectMultiScale(img, found, 0, Size(8, 8), Size(32, 32), 1.05, 2);
		t = (double)getTickCount() - t;
		printf("detection time = %gms\n", t*1000. / cv::getTickFrequency());
		size_t i, j;

		//ȥ���ռ��о������������ϵ�����򣬱������
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
			r.width = cvRound(r.width*0.8);
			r.y += cvRound(r.height*0.07);
			r.height = cvRound(r.height*0.8);
			rectangle(img, r.tl(), r.br(), cv::Scalar(0, 255, 0), 3);
		}
		imshow("people detector", img);
		int c = waitKey(0) & 255;
		if (c == 'q' || c == 'Q' || !f)
			break;
	}
	if (f)
		fclose(f);
	return;
}
/*
int main()
{
	//HOG_SVM2();
	Mat img;
//<<<<<<< .mine
//	//img = imread("C:\\Users\\admin\\Desktop\\QQ��ͼ20160414130948.png");
//	img = imread("E:\\RE-ID\\���ݼ�\\USCPedestrianSetA\\9Cup10.bmp");
//||||||| .r24
//	img = imread("C:\\Users\\admin\\Desktop\\QQ��ͼ20160414130948.png");
//=======
//	//img = imread("E:\\RE-ID\\pedestrians128x64\\pedestrians128x64\\per00099.ppm");
//	//img = imread("C:\\Users\\admin\\Desktop\\QQ��ͼ20160414130948.png");
//>>>>>>> .r25
	HOGDescriptor hog; 
	//hog.setSVMDetector(detector);// ʹ���Լ�ѵ���ķ�����
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());//����ֱ��ʹ��05 CVPR��ѵ���õķ�����,�����Ͳ���Train()���������
	namedWindow("people detector", 1);
	fflush(stdout);
	vector<Rect> found, found_filtered;
	double t = (double)getTickCount();
	// run the detector with default parameters. to get a higher hit-rate
	// (and more false alarms, respectively), decrease the hitThreshold and
	// groupThreshold (set groupThreshold to 0 to turn off the grouping completely).
	//��߶ȼ��
	/*
	hog.detectMultiScale(img, found, 0, Size(8, 8), Size(32, 32), 1.05, 2);
	t = (double)getTickCount() - t;
	printf("detection time = %gms\n", t*1000. / cv::getTickFrequency());
	size_t i, j;

	//ȥ���ռ��о������������ϵ�����򣬱������
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
		r.width = cvRound(r.width*0.8);
		r.y += cvRound(r.height*0.07);
		r.height = cvRound(r.height*0.8);
		rectangle(img, r.tl(), r.br(), cv::Scalar(0, 255, 0), 3);
	}
	imshow("people detector", img);
	waitKey(30000);*/
/*
	//��Ƶ
	VideoCapture CaptureFile("E:\\RE-ID\\dataset\\AVSS_AB_Easy_Divx.mp4");
	//VideoWriter Writer("E:\\RE-ID\\dataset\\result_AVSS_AB_Easy_Divx.avi", CV_FOURCC('M', 'J', 'P', 'G'), 25.0, Size(720, 576));
	int cnt = 0;
	while (CaptureFile.isOpened())
	{
		cnt++;
		CaptureFile >> img;
		if (img.empty())
		{
			CaptureFile.release();
			return 0;
		}
		fflush(stdout);
		vector<Rect> found, found_filtered;
		double t = (double)getTickCount();
		hog.detectMultiScale(img, found, 0.47, Size(8, 8), Size(32, 32), 1.1, 2);
		t = (double)getTickCount() - t;
		printf("detection time = %gms\n", t*1000. / cv::getTickFrequency());
		size_t i, j;

		//ȥ���ռ��о������������ϵ�����򣬱������
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
			r.width = cvRound(r.width*0.8);
			r.y += cvRound(r.height*0.07);
			r.height = cvRound(r.height*0.8);
			rectangle(img, r.tl(), r.br(), cv::Scalar(0, 255, 0), 3);
		}
		imshow("people detector", img);
		//Writer << img;
		waitKey(10);
	}
	
	return 0;
}
*/