
// ScreenShareClientDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "UdpSocket.h"
#define WM_CUSTOM_NETWORK_MSG (WM_USER + 100) 

//TODO �ļ�����ʱ���ı���������
class CScreenShareClientDlg : public CDialogEx
{
// ����
public:
	CScreenShareClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SCREENSHARECLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonTcp();//�Ͽ� �� ����TCP������
	afx_msg void OnClose();
	virtual void OnOK();
	afx_msg void OnBnClickedSharedScreen();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	CListBox m_listChat;
	CProgressCtrl m_progress;

	void OutputFormatError(int);
	void initSock();
	void setStatus(int t);//����tcp״̬��0Ϊδ���ӣ�1Ϊ������
	void setUdpStatus();
	
	bool SendData(const char*, int len,bool isfile=false);
	bool handleHeaderPack(char buf[],int);
	void handleRecvFile(char buf[], int);
	SOCKET m_tcpSock;
	bool m_bIsRecvFile;//�˱�־�ж��Ƿ����ڽ����ļ�
	CString m_recvFilePath;//�����ļ��ľ���·��

	long m_totalSize;//��ǰ������ļ����ܴ�С
	long m_haveGetSize;//��ǰ�����ļ��Ѵ���С
	long m_currentPackSize;//�ļ��зְ��Ĵ�С
	long m_currentPackGetSize;//�ļ��ְ�ʵ���ѵõ��Ĵ�С

	CString m_startSendAttackMsg = "###START_HEAD###";//��ʼ�����ļ�ʱ�ĵ�һ����
	CString m_recvPackAttackMsg = "###SUB_PACK###";//�ļ��ְ�ʱ�ĵİ�ȷ��
	CString m_recvCurrentPackEndMsg = "###SUB_PACK_END###";//��ǰ���������ȷ��
	
public:
	UdpSocket m_udpSocket;
	HANDLE m_hThread=0;//���Ͻ���udp���͵Ľ�ͼ�߳̾��
	static DWORD WINAPI ReceiveScreenShotProc(LPVOID p);//���Ͻ���udp���͵Ľ�ͼ�߳�

	bool m_bSharedScreen = false; //��ǰ�Ƿ��ڷ�����Ļ
	bool m_bFullScreen = false;//TODO ��ǰ�Ƿ�ȫ��
	CRect m_oldClientRect;
	bool m_bConnectTcp = false; //��ǰ��û�����ӵ�TCP������
	void SetFullScreen();

	CDC *m_pDC,*m_picDC;
};
