// ScreenShareServerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "ListCtrlEx.h"
#include "Client.h"
#include "UdpSocket.h"
#define WM_CUSTOM_NETWORK_MSG (WM_USER + 100) 
// CScreenShareServerDlg �Ի���
class CScreenShareServerDlg : public CDialogEx
{
// ����
public:
	CScreenShareServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SCREENSHARESERVER_DIALOG };
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
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonSendMsg();
	virtual void OnOK();

public:
	void initUI();
	void OutPutErrorMsg(int line, DWORD dw = 0);
	bool InitSock();//��ʼ��SOCKET
	//bool InitUdpSock();//��ʼ��udpSocket
	void AcceptConnect();//���տͻ��������¼�
	void SendAllMsg(const char *);//�������Ѿ����ӵĿͻ��˷����ı���Ϣ
	void AddLog(const CString &);
	void AddChat(const CString &);
	int GetListCtrlRow(SOCKET fd);//���ݸ�����socket�õ����б��е��е�λ��
	void SetBottomRightString();//�޸����·������ӵ���Ϣ
	void ChangeTranferPercent(SOCKET, int);//�޸�listCtrl�еĴ����ļ�����

	HANDLE m_hThreadHandle;//���ͽ�ͼ�ľ��
	bool m_bIsShareScreen;//��ǰ�Ƿ��ڷ�����Ŀ

public:
	CListCtrlEx m_listCtrl;
	CListBox m_listChat;
	CListBox m_listLog;

	SOCKET m_listenSocket;//TCP�ļ���socket
	
	CUdpSocket m_udpSocket;
	int m_port = 8000;
	vector<CClient*> m_allClientSocket; //��ǰtcp���ӵ�socket���


	afx_msg void OnBnClickedButtonSendFile();
	afx_msg void OnBnClickedButtonShareScreen();
};
