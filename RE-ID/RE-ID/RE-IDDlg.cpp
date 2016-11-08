// RE-IDDlg.cpp : ʵ���ļ�
//
#include "stdafx.h"
#include "RE-ID.h"
#include "RE-IDDlg.h"
#include "afxdialogex.h"

#include "DlgThread.h"
#include "EasySize.h"
#include <cv.h>
#include <highgui.h>
#include <cvaux.h>
#include <ml.h>
#include <opencv2\highgui\highgui.hpp>

using namespace cv;

bool CREIDDlg::m_cameraThreadFlag = false;
bool CREIDDlg::m_fileThreadFlag = false;
/***************************************************************************
�������ܣ��������ڴ�С,ʹ��easysize
����˵����
���أ�
���ߣ�jipeng huang
ʱ�䣺2016-3-30
�޸ģ�
***************************************************************************/
BEGIN_EASYSIZE_MAP(CREIDDlg)
	//EASYSIZE(Control_ID,left,top,right,bottom,options)
	//EASYSIZE(IDC_COMBO_COMMON, ES_BORDER, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_STATIC_ShowVideo , ES_BORDER, ES_BORDER, ES_BORDER           , ES_BORDER, 0)
	EASYSIZE(IDC_BUTTON_CameraOn, IDC_STATIC_ShowVideo, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_CameraOff, IDC_STATIC_ShowVideo, IDC_BUTTON_CameraOn, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_FileOn, IDC_STATIC_ShowVideo, IDC_BUTTON_CameraOff, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_FileOff, IDC_STATIC_ShowVideo, IDC_BUTTON_FileOn, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_Exit, IDC_STATIC_ShowVideo, IDC_BUTTON_FileOff, ES_BORDER, ES_KEEPSIZE, 0)
END_EASYSIZE_MAP

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CREIDDlg �Ի���
CREIDDlg::CREIDDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CREIDDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CREIDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CREIDDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CameraOn, &CREIDDlg::OnBnClickedButtonCameraon)
	ON_BN_CLICKED(IDC_BUTTON_CameraOff, &CREIDDlg::OnBnClickedButtonCameraoff)
	ON_BN_CLICKED(IDC_BUTTON_FileOn, &CREIDDlg::OnBnClickedButtonFileon)
	ON_BN_CLICKED(IDC_BUTTON_FileOff, &CREIDDlg::OnBnClickedButtonFileoff)
	ON_BN_CLICKED(IDC_BUTTON_Exit, &CREIDDlg::OnBnClickedButtonExit)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CREIDDlg ��Ϣ�������
BOOL CREIDDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	//2016-3-30 hjp �������ڴ�С
	INIT_EASYSIZE;
	//2016-3-14 hjp ��opencv��ʾ��Ƕ��Pic�ؼ�
	Mat BlackScreen(1000, 1000, CV_8UC3, Scalar(0, 0, 0));
	namedWindow("view", WINDOW_AUTOSIZE);
	HWND hWnd = (HWND)cvGetWindowHandle("view");
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd, GetDlgItem(IDC_STATIC_ShowVideo)->m_hWnd);
	::ShowWindow(hParent, SW_HIDE);
	imshow("view",BlackScreen);//�ñ���Ϊ��ɫ

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CREIDDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CREIDDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CREIDDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
/***************************************************************************
�������ܣ���ȡ����ͷ��ʾ�̱߳�ʶ
����˵����
���أ�����ͷ��ʾ�̱߳�ʶ
���ߣ�jipeng huang
ʱ�䣺2016-3-29
�޸ģ�
***************************************************************************/
bool CREIDDlg::getCamerathread()
{
	return m_cameraThreadFlag;
}
void CREIDDlg::camerathreadOn()
{
	m_cameraThreadFlag = true;
	return;
}
void CREIDDlg::camerathreadOff()
{
	m_cameraThreadFlag = false;
	return;
}
/***************************************************************************
�������ܣ���ȡ�ļ���ʾ�̱߳�ʶ
����˵����
���أ��ļ���ʾ�̱߳�ʶ
���ߣ�jipeng huang
ʱ�䣺2016-3-29
�޸ģ�
***************************************************************************/
bool CREIDDlg::getFilethread()
{
	return m_fileThreadFlag;
}
void CREIDDlg::filethreadOn()
{
	m_fileThreadFlag = true;
}
void CREIDDlg::filethreadOff()
{
	m_fileThreadFlag = false;
}

void CREIDDlg::OnBnClickedButtonCameraon()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//2016-3-14 hjp ����ͷ����ť�����߼�
	m_pThreadCamera = ::AfxBeginThread(threadFunCamera, 0, THREAD_PRIORITY_NORMAL, 0, 0);
	GetDlgItem(IDC_BUTTON_CameraOn)->EnableWindow(false);//��ť״̬����
	GetDlgItem(IDC_BUTTON_CameraOff)->EnableWindow(true);
	return;
}

void CREIDDlg::OnBnClickedButtonCameraoff()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//2016-3-14 hjp ����ͷ�ذ�ť�����߼�
	m_cameraThreadFlag = false;
	GetDlgItem(IDC_BUTTON_CameraOn)->EnableWindow(true);//��ť״̬����
	GetDlgItem(IDC_BUTTON_CameraOff)->EnableWindow(false);
	Mat BlackScreen(1000, 1000, CV_8UC3, Scalar(0, 0, 0));//��ԭ��ʾ����
	imshow("view", BlackScreen);
	return;
}

void CREIDDlg::OnBnClickedButtonFileon()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//2016-3-15 hjp �ļ�����ť�����߼�
	m_pThreadFile = ::AfxBeginThread(threadFunFile, 0, THREAD_PRIORITY_NORMAL, 0, 0);
	GetDlgItem(IDC_BUTTON_FileOn)->EnableWindow(false);//��ť״̬����
	GetDlgItem(IDC_BUTTON_FileOff)->EnableWindow(true);
	return;
}


void CREIDDlg::OnBnClickedButtonFileoff()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//2016-3-15 hjp �ļ��ذ�ť�����߼�
	m_fileThreadFlag = false;
	GetDlgItem(IDC_BUTTON_FileOn)->EnableWindow(true);//��ť״̬����
	GetDlgItem(IDC_BUTTON_FileOff)->EnableWindow(false);
	//Mat BlackScreen(1000, 1000, CV_8UC3, Scalar(0, 0, 0));//��ԭ��ʾ����
	//imshow("view", BlackScreen);
	return;
}


void CREIDDlg::OnBnClickedButtonExit()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	exit(0);
	return;
}

/***************************************************************************
�������ܣ��������ڴ�С
����˵����
���أ�
���ߣ�jipeng huang
ʱ�䣺2016-3-30
�޸ģ�
***************************************************************************/
void CREIDDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO:  �ڴ˴������Ϣ����������
	UPDATE_EASYSIZE;
}
void CREIDDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialogEx::OnSizing(fwSide, pRect);

	// TODO:  �ڴ˴������Ϣ����������
	//�趨��Сֵ
	EASYSIZE_MINSIZE(600, 400, fwSide, pRect);
}

/***************************************************************************
�������ܣ��رճ������н��̣�����رմ��ڰ�ť�����ú���
����˵����
���أ�
���ߣ�lei qi
ʱ�䣺2016-4-7
�޸ģ�
***************************************************************************/
void CREIDDlg::OnClose()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	int a;
	if (m_cameraThreadFlag == true)
	{
		//OnBnClickedButtonFileoff();
		camerathreadOff();
		//a = TerminateThread(m_pThreadCamera->m_hThread, 0);//�ر�����ͷ����
	}
	if (m_fileThreadFlag == true)
	{
		TerminateThread(m_pThreadFile->m_hThread, 0);//�ر�����ͷ����
	}
	CDialogEx::OnClose();
	//exit(0);//
	return;
}
