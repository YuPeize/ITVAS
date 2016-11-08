/***************************************************************************
���ܣ�С����-��ȡͼƬ����Ƶ�еĸ�����������
���ߣ�jipeng huang
ʱ�䣺2016-7-13
�޸ģ�
ʹ��˵�������س���ȷ�ϴӵ�ǰ֡�л�ȡ��������
����10s�������򰴳��س��������������֡��
���ɵĸ�������Ҫ�ֹ��޳�����������
***************************************************************************/
#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;
using namespace cv;
/***************************************************************************
�������ܣ��Զ��ָ�ͼ��
����˵�����ָ�ͼ�񣬵�ǰ֡����Ƶ�е�λ��
���أ�
���ߣ�jipeng huang
ʱ�䣺2016-7-13
�޸ģ�
***************************************************************************/
void autoSeg(Mat frame,int frameCnt)
{
	int rowSegNum = 16;
	int colSegNum = 30;
	int winHeight = frame.rows / rowSegNum;
	int winWidth = frame.cols / colSegNum;

	for (int i = 0; i < rowSegNum; i++)
	{
		for (int j = 0; j < colSegNum; j++)
		{
			Rect2d window( j*winWidth, i*winHeight, winWidth, winHeight);
			Mat segFrame = frame(window);	
			stringstream sr,sc,sid;
			sr << i;
			sc << j;
			sid << frameCnt;
			string srString = sr.str();
			string scString = sc.str();
			string sidString = sid.str();
			//���渺������·��
			imwrite("E:\\RE-ID\\���ݼ�\\����\\video\\20160711\\DJI_0001_"+sidString+"_"+srString+"_"+scString+".jpg",segFrame);
		}
	}
}

/*int main(int argc, char** argv)
{
	cout << "���س���ȷ�ϴӵ�ǰ֡�л�ȡ������"<<endl;
	cout << "����10s�������򰴳��س��������������֡" << endl;
	const int fps = 30;
	string videoPath = "E:\\RE-ID\\���ݼ�\\����\\video\\20160711\\DJI_0001.MOV";//��Ƶ·��
	VideoCapture cap(videoPath);
	Mat frame;
	namedWindow("frame", 0);
	for (int frameCnt=1;;frameCnt++)
	{
		cap >> frame;
		if (frame.empty())break;
		if (frameCnt%fps)continue;
		imshow("frame", frame);
		int key = waitKey(10000);
		if (key == 13) autoSeg(frame,frameCnt);
	}
	return 0;
}
*/