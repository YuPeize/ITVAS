//2016-3-16 hjp �����̺߳��������ڹ��̹���
#include "stdafx.h"
#include "DlgThread.h"
#include "RE-ID.h"
#include "RE-IDDlg.h"
 
#include <cv.h>
#include <highgui.h>
#include <cvaux.h>
#include <ml.h>
#include <opencv2\highgui\highgui.hpp>

using namespace cv;

/***************************************************************************
�������ܣ�����ͷ��ʾ�̺߳���
����˵����

���أ�
���ߣ�jipeng huang
ʱ�䣺2016-3-14
�޸ģ�
***************************************************************************/
UINT threadFunCamera(LPVOID pParam)
{
	Mat Temp;
	VideoCapture CaptureCamera(0);
	//VideoWriter Writer("E://VideoTest.avi", CV_FOURCC('M', 'J', 'P', 'G'), 25.0, Size(640, 480));
	if (!CaptureCamera.isOpened())//�쳣����
	{
		AfxMessageBox(_T("����ͷ����ʧ��"));
		return -1;
	}
	CREIDDlg::camerathreadOn();
	while (CaptureCamera.isOpened() && CREIDDlg::getCamerathread())
	//while (CaptureCamera.isOpened())
	{
		CaptureCamera >> Temp;//����ͷ-�ڴ�-Ӳ��
		imshow("view", Temp);
	//	Writer << Temp;
		Sleep(20);
	}
	return 0;
}

/***************************************************************************
�������ܣ��ļ���ʾ�̺߳���
����˵����
���أ�
���ߣ�jipeng huang
ʱ�䣺2016-3-15
�޸ģ�
***************************************************************************/
UINT threadFunFile(LPVOID pParam)
{
	String Path;
	CFileDialog Openfiledlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Video Files (*.rmvb;*.avi)|*.rmvb;*.avi||"));
	if (Openfiledlg.DoModal() == IDOK) //����ģ̬�Ի���  
	{
		//CString����ת��Ϊstring����  
		CString  filepath;
		filepath = Openfiledlg.GetPathName();
		CStringA temp(filepath.GetBuffer(0));
		filepath.ReleaseBuffer();
		Path = temp.GetBuffer(0);
		temp.ReleaseBuffer();
	}
	else
	{
		return 0;
	}
		
	Mat Temp;
	/*VideoCapture CaptureFile(Path);
	if (!CaptureFile.isOpened())//�쳣����
	{
		AfxMessageBox(_T("�ļ���ʧ��"));
		return -1;
	}
	CREIDDlg::filethreadOn();
	while (CaptureFile.isOpened() && CREIDDlg::getFilethread())
	{
		CaptureFile >> Temp;//����ͷ-�ڴ�-Ӳ��
		if (Temp.empty())
		{
			CaptureFile.release();
			return 0;
		}
		imshow("view", Temp);
		Sleep(50);
	}*/
	return 0;
}