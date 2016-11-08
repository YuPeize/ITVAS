/***************************************************************************
���ܣ�С����-��ȡͼƬ����Ƶ�еĶ������򲢱���
���ߣ�jipeng huang
ʱ�䣺2016-7-12
�޸ģ�
ʹ��˵����
����ѡ��ָ�ģʽ������1Ϊ��ȡͼ������2Ϊ��ȡ��Ƶ��
��ס���������϶�ѡ���ȡ���򣬲�������ظ�ѡ��
���س���ȷ�Ͻ�ȡ�������򱣴�Ϊ.jpg��ʽ��ͼ��
���һ��ͼ��Ľ�ȡ�󣬰�ESC��������һ��ͼ��������Ƶ֡��
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

class BoxExtractor 
{
public:
	int key;
	Rect2d extract(const std::string& windowName, Mat img);
	struct handlerT
	{
		bool isDrawing;
		Rect2d box;
		Mat image;
		handlerT() : isDrawing(false) {};
	}params;

private:
	static void mouseHandler(int event, int x, int y, int flags, void *param);
	void opencv_mouse_callback(int event, int x, int y, void *param);
};
/***************************************************************************
�������ܣ�����¼���ʼ��
����˵����ʱ�䣬�����꣬�����꣬flags(��δʹ�ã�ֻ������opencv�ĵ�����ʽ)��thisָ��
���أ�
���ߣ�jipeng huang
ʱ�䣺2016-7-12
�޸ģ�
***************************************************************************/
void BoxExtractor::mouseHandler(int event, int x, int y, int flags, void *param)
{
	BoxExtractor *self = static_cast<BoxExtractor*>(param);
	self->opencv_mouse_callback(event, x, y, param);
}
/***************************************************************************
�������ܣ�����¼���Ӧ
����˵�����¼��������꣬�����꣬thisָ��
���أ�
���ߣ�jipeng huang
ʱ�䣺2016-7-12
�޸ģ�
***************************************************************************/
void BoxExtractor::opencv_mouse_callback(int event, int x, int y, void *param)
{
	handlerT * data = (handlerT*)param;
	switch (event)
	{
		//�ƶ�-������
	case EVENT_MOUSEMOVE:
		if (data->isDrawing)
		{
			data->box.width = x - data->box.x;
			data->box.height = y - data->box.y;
		}
		break;
		//�������
	case EVENT_LBUTTONDOWN:
		data->isDrawing = true;
		data->box = cvRect(x, y, 0, 0);
		break;
		//����ɿ�
	case EVENT_LBUTTONUP:
		data->isDrawing = false;
		if (data->box.width < 0)
		{
			data->box.x += data->box.width;
			data->box.width *= -1;
		}
		if (data->box.height < 0)
		{
			data->box.y += data->box.height;
			data->box.height *= -1;
		}
		break;
	}
}
/***************************************************************************
�������ܣ���ȡͼƬ����
����˵�����������ƣ�����ͼ��
���أ���ȡ�����boundingbox
���ߣ�jipeng huang
ʱ�䣺2016-7-12
�޸ģ�
***************************************************************************/
Rect2d BoxExtractor::extract(const std::string& windowName, Mat img)
{
	imshow(windowName, img);
	params.image = img.clone();
	setMouseCallback(windowName, mouseHandler, (void *)&params);
	while (!(key == 27 || key == 13))
	{
		rectangle(params.image, params.box, Scalar(0, 255, 0), 2, 1);
		imshow(windowName, params.image);
		params.image = img.clone();
		key = waitKey(1);
	}
	return params.box;
}

void segImage()
{
	BoxExtractor box;
	freopen("E:\\RE-ID\\ѵ������\\����\\�������\\FileNameList.txt","r",stdin);//�������ļ����ɵ��ļ�·���б�
	string fileNameList;
	while (cin >> fileNameList)
	{
		string fileName = fileNameList.substr(fileNameList.length()-14,10);
		//ofstream boundingBoxFile("E:\\RE-ID\\ѵ������\\����\\�������\\"+fileName+".txt");//����boundingbox���ļ�·��
		Mat frame=imread(fileNameList);
		//imwrite("E:\\RE-ID\\ѵ������\\����\\�������\\" + fileName + ".jpg", frame);
		for (int objectCnt = 1;; objectCnt++)
		{
			box.key = -1;
			Rect2d roi = box.extract("Origin", frame);
			if (roi.width == 0 || roi.height == 0) 
			{
				objectCnt--;
				continue;
			}
			Mat saveRoi = frame(roi);
			imshow("Extract", saveRoi);

			stringstream ss;
			ss << objectCnt;
			string objectId = ss.str();
			imwrite("E:\\RE-ID\\ѵ������\\����\\�������\\"+fileName+"_"+objectId+".jpg",saveRoi);//������ȡͼ���·��
			//boundingBoxFile << roi.x << " " << roi.y << " " << roi.height << " " << roi.width << endl;
			box.params.isDrawing = false;//���ǰһ��ͼ��ѡȡ�ı߿�
			box.params.box.height = 0;
			box.params.box.width = 0;
			if (box.key == 27) break;
		}
		//boundingBoxFile.close();
	}
}
void segVideo()
{
	const int fps = 50;
	BoxExtractor box;
	string videoPath = "E:\\RE-ID\\���ݼ�\\����\\video\\20160711\\";
	string videoName = "DJI_0005.MOV";
	VideoCapture cap(videoPath+videoName);
	Mat frame;
	for (int frameCnt = 1;; frameCnt++)
	{
		cap >> frame;
		if (frame.empty()) break;
		if (frameCnt%fps) continue;
		imshow("Origin",frame);
		if (waitKey(1000000) != 13) continue;
		for (int objectCnt = 1;; objectCnt++)
		{
			box.key = -1;
			Rect2d roi = box.extract("Origin", frame);
			if (roi.width == 0 || roi.height == 0)
			{
				objectCnt--;
				continue;
			}
			Mat saveRoi = frame(roi);
			imshow("Extract", saveRoi);

			stringstream ss;
			ss << objectCnt;
			string objectId = ss.str();
			ss.str("");
			ss << frameCnt;
			string frameId = ss.str();
			//cout << (videoPath + videoName + "_pos_" + frameId + string("_") + objectId + ".jpg") << endl;
			imwrite(videoPath + videoName + "_pos_" + frameId + string("_") + objectId + ".jpg", saveRoi);//������ȡͼ���·��
			box.params.isDrawing = false;//���ǰһ��ͼ��ѡȡ�ı߿�
			box.params.box.height = 0;
			box.params.box.width = 0;
			if (box.key == 27) break;
		}
	}
}
/*
int main(int argc, char** argv)
{
	cout << "��ȡͼƬ������1" << endl << "��ȡ��Ƶ������2" << endl;
	int choice;
	cin >> choice;
	cout << "���س�ȷ��ѡȡĿ��" << endl << "��ESC������һ��ͼƬ�������Ƶ֡" << endl;
	namedWindow("Origin", WINDOW_KEEPRATIO);
	switch (choice)
	{
	case 1: segImage(); break;
	case 2: segVideo(); break;
	}
	return 0;
}
*/