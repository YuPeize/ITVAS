#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp> 
#include <iostream>
#include <cstring>
#include <fstream> 
#include <queue>
using namespace std;
using namespace cv;
#ifdef _DEBUG
#pragma comment(lib,"..\\x64\\Debug\\reid_tracking.lib")
#else
#pragma comment(lib,"..\\x64\\Release\\reid_tracking.lib")
#endif

const double detectionResizeCol = 2; //���ͼ��������ϵ��
const double detectionResizeRow = 2; //���ͼ��������ϵ��
const double trackingResizeCol = 5; //����ͼ��������ϵ��
const double trackingResizeRow = 5; //����ͼ��������ϵ��
const int detectionSkip = 30; //�����
const int trackingSkip = 1; //���ټ�� 
const int lineThickness = 8; //��ע���ϸ
/***************************************************************************
�������ܣ��ж�����boundingbox�Ƿ�Ϊһ������
����˵�����߽�1���ϵ㣬�߽�1���µ㣬�߽�2���ϵ㣬�߽�2���µ㣬��������ֵ
���أ�����boundingbox�Ƿ�Ϊһ������
���ߣ�jipeng huang
ʱ�䣺2016-10-10
***************************************************************************/
bool isOverlap(Point tl1, Point br1, Point tl2, Point br2, double threshold=0.5)
{
	if (tl1.x > br2.x) return false; //���ֲ��ཻ�����
	if (tl1.y > br2.y) return false;
	if (tl2.x > br1.x) return false;
	if (tl2.y > br1.y) return false;
	float colInt = (float)min(br1.x, br2.x) - max(tl1.x, tl2.x);
	float rowInt = (float)min(br1.y, br2.y) - max(tl1.y, tl2.y);
	float intersection = colInt * rowInt;
	float area1 = (float)(br1.x - tl1.x)*(br1.y - tl1.y);
	float area2 = (float)(br2.x - tl2.x)*(br2.y - tl2.y);
	if (intersection / (area1 + area2 - intersection) > threshold) return true;
	return false;
}
/***************************************************************************
�������ܣ����ɿ�����ɫ
����˵����ԭɫ�仯����
���أ�������ɫ����
���ߣ�jipeng huang
ʱ�䣺2016-10-09
***************************************************************************/
queue<Scalar> generateColor(int step=63)
{
	queue<Scalar> q;
	for (int red = 0; red < 256; red += step)
		for (int green = 0; green < 256; green += step)
			for (int blue = 0; blue < 256; blue += step)
				q.push(Scalar(red,green,blue));
	return q;
}
/***************************************************************************
�������ܣ���֡�������
����˵�������ͼ��HOG�����
���أ�����boundingbox������
���ߣ�jipeng huang
ʱ�䣺2016-10-08
***************************************************************************/
vector<Rect> detection(Mat &src,HOGDescriptor hog)
{
	Mat dst;
	resize(src, dst, Size((int)(src.cols / detectionResizeCol), (int)(src.rows / detectionResizeRow)));
	fflush(stdout);
	vector<Rect> found, found_filtered;
	double t = (double)getTickCount();
	//��߶ȼ��
	hog.detectMultiScale(dst, found, 1.3, Size(16, 16), Size(0, 0), 1.3, 5);
	t = (double)getTickCount() - t;
	printf("detection time = %gms\n", t*1000. / cv::getTickFrequency());
	//ȥ���ռ��о������������ϵ�����򣬱������
	vector<Rect>::iterator it = found.begin();
	for (; it != found.end();)
	{
		if ((*it).height > 500) //ɸ��̫�������
			found.erase(it);
		else
			it++;
	}
	for (int i = 0, j; i < found.size(); i++)
	{
		for (j = i+1; j < found.size(); j++)
		if (isOverlap(found[i].tl(), found[i].br(), found[j].tl(), found[j].br(), 0.1))
			break;
		if (j == found.size())
			found_filtered.push_back(found[i]);
	}
	/*for (int i = 0, j ; i < found.size(); i++)
	{
		Rect r = found[i];
		for (j = 0; j < found.size(); j++) 
			if (j != i && (r & found[j]) == r) 
				break;
		if (j == found.size())
			found_filtered.push_back(r);
	}*/
	// �ʵ���С����,����ԭ��ԭ����
	for (int i = 0; i < found_filtered.size(); i++)
	{
		//found_filtered[i].x += cvRound(found_filtered[i].width*0.1);
		found_filtered[i].x = cvRound(found_filtered[i].x*detectionResizeCol);
		//found_filtered[i].width = cvRound(found_filtered[i].width*0.8*detectionResizeCol);
		found_filtered[i].width = cvRound(found_filtered[i].width*detectionResizeCol);
		//found_filtered[i].y += cvRound(found_filtered[i].height*0.07);
		found_filtered[i].y = cvRound(found_filtered[i].y*detectionResizeRow);
		//found_filtered[i].height = cvRound(found_filtered[i].height*0.8*detectionResizeRow);
		found_filtered[i].height = cvRound(found_filtered[i].height*detectionResizeRow);	
	}
	return found_filtered;
}
/***************************************************************************
�������ܣ����+����
����˵�����ļ�·��
���أ�
���ߣ�jipeng huang
ʱ�䣺2016-10-08
***************************************************************************/
void detection_tracking(string path)
{
	VideoWriter writer("E:\\RE-ID\\���ݼ�\\����\\video\\20160719\\demo.avi", ('M', 'J', 'P', 'G'), 29, Size(3840, 2160));

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
	Mat src, dst;
	vector<Rect> cars;//�����ĳ���
	vector<Rect2d> preCars; //֮ǰ�ĳ���
	vector<Scalar> nowColor, preColor;//���ڵ���ɫ��֮ǰ����ɫ
	queue<Scalar> colorQueue = generateColor(127);//���ɾ��ο���ɫ
	for (int frameCnt = 0;; frameCnt++)
	{
		cap >> src;
		if (src.empty()) break; //�������һ֡s
		if (frameCnt%detectionSkip == 0)//���
		{
			nowColor.clear();
			cars = detection(src, hog);
			for (int i = 0,j; i < cars.size(); i++)
			{
				for (j = 0; j < preCars.size(); j++)
					if (isOverlap(cars[i].tl(), cars[i].br(), preCars[j].tl(), preCars[j].br(), 0.1)) break;
				if (j == preCars.size()) //û���غϵģ��³������·�����ɫ
				{
					nowColor.push_back(colorQueue.front());
					colorQueue.pop();
				}
				else//�ɳ�	
					nowColor.push_back(preColor[j]);
				rectangle(src, cars[i], nowColor[i], lineThickness);
			}
			imshow("car detector",src);
			writer << src;
			waitKey(1);
			//�ͷ�preColor,preCars
			preCars.clear();
			for (int i = 0, j; i < preColor.size(); i++)
			{
				for (j = 0; j < nowColor.size(); j++)
					if (nowColor[j] == preColor[i]) break;
				if (j == nowColor.size()) //�ɳ�����ͷ������δ�õ�����ɫ
					colorQueue.push(preColor[i]);
			}
			preColor.clear();
		}
		else //����
		{
			vector<Rect2d> temp;
			MultiTracker tracker("KCF");
			resize(src, dst, Size((int)(src.cols / trackingResizeCol), (int)(src.rows / trackingResizeRow)));
			for (int i = 0; i < cars.size(); i++) //boundingbox����
			{
				Rect2d now;
				now.x = cvRound(cars[i].x / trackingResizeCol);
				now.width = cvRound(cars[i].width / trackingResizeCol);
				now.y = cvRound(cars[i].y / trackingResizeRow);
				now.height = cvRound(cars[i].height / trackingResizeRow);
				temp.push_back(now);
			}
			tracker.add(dst,temp);//boundingbox��ʼ��
			for (; frameCnt%detectionSkip != detectionSkip - 1; frameCnt++)
			{
				cap >> src;
				if (src.empty()) break;
				if (frameCnt%trackingSkip == 0) //��Ҫtracking��֡
				{
					double t = (double)getTickCount();
					resize(src, dst, Size((int)(src.cols / trackingResizeCol), (int)(src.rows / trackingResizeRow)));
					tracker.update(dst); //KCF
					t = (double)getTickCount() - t;
					printf("tracking time = %gms\n", t*1000. / cv::getTickFrequency());
				}
				preColor = nowColor;
				for (int i = 0; i < tracker.objects.size(); i++)//��ԭ����ѡ
				{
					Rect2d display;
					display.x = tracker.objects[i].x * trackingResizeCol;
					display.width = tracker.objects[i].width * trackingResizeCol;
					display.y = tracker.objects[i].y * trackingResizeRow;
					display.height = tracker.objects[i].height * trackingResizeRow;
					if (frameCnt%detectionSkip == detectionSkip - 2)//�������һ֡����preCar
						preCars.push_back(display);
					rectangle(src, display, preColor[i], lineThickness);
				}
				imshow("car detector", src);
				writer << src;
				waitKey(1);
			}
		}
	}
}

int main()
{
	detection_tracking("E:\\RE-ID\\���ݼ�\\����\\video\\20160719\\DJI_0006.avi");
	return 0;
}