#pragma once
// RE-IDDlg.h : ͷ�ļ�
#include "EasySize.h"

// CREIDDlg �Ի���
class CREIDDlg : public CDialogEx
{
private:
	static bool m_cameraThreadFlag;//����ͷ��ʾ�̱߳�ʶ
	static bool m_fileThreadFlag;//�ļ���ʾ�̱߳�ʶ
protected:
	CWinThread* m_pThreadCamera; //����ͷ�߳�ָ��
	CWinThread* m_pThreadFile;//�ļ��߳�ָ��
	DECLARE_EASYSIZE//���ڴ��ڴ�С
// ����
public:
	CREIDDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_REID_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnBnClickedButtonCameraon();
	afx_msg void OnBnClickedButtonCameraoff();
	afx_msg void OnBnClickedButtonFileon();
	afx_msg void OnBnClickedButtonFileoff();
	afx_msg void OnBnClickedButtonExit();

	static bool getCamerathread();//��ȡ����ͷ��ʾ�̱߳�ʶ
	static void camerathreadOn();//����ͷ��ʾ�̱߳�ʶtrue
	static void camerathreadOff();//����ͷ��ʾ�̱߳�ʶfalse
	static bool getFilethread();//��ȡ�ļ���ʾ�̱߳�ʶ
	static void filethreadOn();//�ļ���ʾ�̱߳�ʶtrue
	static void filethreadOff();//�ļ���ʾ�̱߳�ʶfalse
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnClose();
};
