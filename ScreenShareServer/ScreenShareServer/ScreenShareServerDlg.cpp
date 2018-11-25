
// ScreenShareServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ScreenShareServer.h"
#include "ScreenShareServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CScreenShareServerDlg �Ի���



CScreenShareServerDlg::CScreenShareServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SCREENSHARESERVER_DIALOG, pParent),
	m_bIsShareScreen(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CScreenShareServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_listCtrl);
	DDX_Control(pDX, IDC_LIST4, m_listChat);
	DDX_Control(pDX, IDC_LIST5, m_listLog);
}

BEGIN_MESSAGE_MAP(CScreenShareServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON3, &CScreenShareServerDlg::OnBnClickedButtonSendMsg)
	ON_BN_CLICKED(IDC_BUTTON2, &CScreenShareServerDlg::OnBnClickedButtonSendFile)
	ON_BN_CLICKED(IDC_BTN_SHARE_SCREEN, &CScreenShareServerDlg::OnBnClickedButtonShareScreen)
END_MESSAGE_MAP()


// CScreenShareServerDlg ��Ϣ�������

BOOL CScreenShareServerDlg::OnInitDialog()
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

	
	initUI();
	Log::CreateLog();
	if(!InitSock())
		AddLog("��ʼ��tcpʧ��");
	if (!m_udpSocket.InitUdpSock())
		AddLog("��ʼ��udpʧ��");
	m_udpSocket.setHandle(this);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CScreenShareServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CScreenShareServerDlg::OnPaint()
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
HCURSOR CScreenShareServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CScreenShareServerDlg::OnOK()
{
	//CDialogEx::OnOK();
}

void CScreenShareServerDlg::initUI()
{
	m_listCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);//������չ���
	m_listCtrl.InsertColumn(0, _T("socketId"), LVCFMT_LEFT, 100);//���õ�һ��
	m_listCtrl.InsertColumn(1, _T("IP"), LVCFMT_LEFT, 100);//���õڶ���
	m_listCtrl.InsertColumn(2, _T("�ļ��������"), LVCFMT_LEFT, 200);//...

}


void CScreenShareServerDlg::OutPutErrorMsg(int line, DWORD dw)
{
	char szBuf[128];
	LPVOID lpMsgBuf;
	if (!dw)
		dw = GetLastError();
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL,
		dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	sprintf(szBuf, "��:%d (������=%d): %s", line, dw, lpMsgBuf);
	LocalFree(lpMsgBuf);
	//MessageBox(szBuf);
	AddLog(szBuf);
}
bool CScreenShareServerDlg::InitSock()
{
	SOCKADDR_IN serverAddr;
	int addrLen = sizeof(serverAddr);
	memset(&serverAddr, 0, addrLen);
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;//inet_addr("127.0.0.1");
	serverAddr.sin_port = htons(m_port);
	m_listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_listenSocket == INVALID_SOCKET)
	{
		OutPutErrorMsg(__LINE__);
		WSACleanup();
		return false;
	}
	if (::bind(m_listenSocket, (sockaddr*)&serverAddr, addrLen) == SOCKET_ERROR)
	{
		OutPutErrorMsg(__LINE__);
		WSACleanup();
		return false;
	}
	listen(m_listenSocket, 5);
	//CString log;
	//log.Format("<Listening..> ip: %s port:%d", inet_ntoa(serverAddr.sin_addr), m_port);
	//AddLog(log);

	/* WSAAsyncSelect���Զ����׽�������Ϊ������ģʽ*/

	WSAAsyncSelect(m_listenSocket, m_hWnd, WM_CUSTOM_NETWORK_MSG, FD_ACCEPT);
	return true;
}

BOOL CScreenShareServerDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_CUSTOM_NETWORK_MSG)
	{
		SOCKET socket = (SOCKET)pMsg->wParam;//��������Ϣ��socket
		CClient *currentSocket;
		for (auto &p : m_allClientSocket)
		{
			if (p->m_socket == socket)
				currentSocket = p;
		}
		long event = WSAGETSELECTEVENT(pMsg->lParam);
		int error = WSAGETSELECTERROR(pMsg->lParam); //����WSAAsyncSelectֻ�����������ô�����ΪGetLastError���ܸ��������¼���Ϣ�����Դ�
		/*if (error)
		{
			OutPutErrorMsg(__LINE__, error);
		}*/
		CString strLog;
		int row;
		vector<CClient*>::iterator pos;//����ָ��ɾ��m_allClientSocketĳһ����λ��
		switch (event)
		{
		case FD_ACCEPT:
			AcceptConnect();
			break;
		case FD_READ:
			//ע��ÿ��δ���껺������recv()���ã��������´���һ��FD_READ��Ϣ,���������Ҫѭ����ȡ�Ļ�����Ҫ�ȹرն�FD_READ�ļ���
			//WSAAsyncSelect(socket, m_hWnd, WM_CUSTOM_NETWORK_MSG, FD_WRITE | FD_CLOSE);
			currentSocket->RecvMsg();
			//WSAAsyncSelect(socket, m_hWnd, WM_CUSTOM_NETWORK_MSG, FD_READ | FD_WRITE | FD_CLOSE);
			break;
		case FD_WRITE:

			break;
		case FD_CLOSE:
			currentSocket->CloseSocket(m_allClientSocket.size() - 1);
			pos = remove_if(m_allClientSocket.begin(), m_allClientSocket.end(), [=](CClient* c) {return c->m_socket == socket; });	
		
			row = GetListCtrlRow(currentSocket->m_socket);
			if (row != -1)
				m_listCtrl.DeleteItem(row);
			else
				AddLog("δ���ҵ��رյ�socket����");
			m_allClientSocket.erase(pos, m_allClientSocket.end());
			SetBottomRightString();
			break;
		default:
			break;
		}
	}
	else if (pMsg->message == WM_KEYDOWN&&pMsg->hwnd == GetDlgItem(IDC_EDIT1)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		OnBnClickedButtonSendMsg();
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
void CScreenShareServerDlg::AcceptConnect()//�յ�������
{
	SOCKADDR_IN sockAddr;
	int len = sizeof(SOCKADDR_IN);
	ZeroMemory(&sockAddr, len);
	SOCKET client = accept(m_listenSocket, (sockaddr*)&sockAddr, &len);
	if (client == INVALID_SOCKET)
	{
		OutPutErrorMsg(__LINE__);
		return;
	}
	//���÷��ͻ�������С��Ĭ��8k���������64k��
	int sendBufSize = 1024 * 64 - 1;
	setsockopt(client, SOL_SOCKET, SO_SNDBUF, (char*)&sendBufSize, sizeof(sendBufSize));

	m_allClientSocket.push_back(new CClient(client, this));
	WSAAsyncSelect(client, m_hWnd, WM_CUSTOM_NETWORK_MSG, FD_READ | FD_WRITE | FD_CLOSE);

	CString strLog;
	strLog.Format("%d �����ӣ���ַ:%s ,��ǰ������: %d", client, inet_ntoa(sockAddr.sin_addr), m_allClientSocket.size());
	AddLog(strLog);
	SetBottomRightString();

	int num = m_listCtrl.GetItemCount();
	strLog.Format("%d", client);
	m_listCtrl.InsertItem(num, strLog);
	m_listCtrl.SetItemText(num, 1, inet_ntoa(sockAddr.sin_addr));
	m_listCtrl.SetItemText(num, 2, "");

}
void CScreenShareServerDlg::SendAllMsg(const char *msg)
{
	for (auto client : m_allClientSocket)
	{
		client->SendData(msg, strlen(msg), false);
	}
}
void CScreenShareServerDlg::AddLog(const CString &str)
{
	m_listLog.AddString(str);
	m_listLog.SetTopIndex(m_listLog.GetCount() - 1);//��ListBox��Զ���������·�
}
void CScreenShareServerDlg::AddChat(const CString &str)
{
	m_listChat.AddString(str);
	m_listChat.SetTopIndex(m_listChat.GetCount() - 1);//��ListBox��Զ���������·�
}
void CScreenShareServerDlg::OnBnClickedButtonSendMsg()
{
	CString strMsg;
	GetDlgItemText(IDC_EDIT1, strMsg);
	if (strMsg.IsEmpty())
		return;
	else
	{
		SendAllMsg("Server: " + strMsg);
		AddChat(CString("��:")+ strMsg);
		SetDlgItemText(IDC_EDIT1, "");
	}
}

void CScreenShareServerDlg::OnBnClickedButtonSendFile()
{
	CString strPath;
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("All Files (*.*)|*.*||"), NULL);
	if (dlg.DoModal() == IDOK)
	{
		strPath = dlg.GetPathName(); //�ļ�·������������FilePathName��
		AddLog("�����ļ�:" + strPath);
		for (auto client : m_allClientSocket)
		{
			//TODO ����ڴ˷��͹����У��пͻ��˶Ͽ����ӻ���ʲô�����

			client->setFileName(strPath);
			client->StartSendFile();
		}
	}
	else
	{
		return;
	}
}
int CScreenShareServerDlg::GetListCtrlRow(SOCKET fd)
{
	for (int i = 0; i < m_listCtrl.GetItemCount(); i++)
	{
		if (atoi(m_listCtrl.GetItemText(i, 0)) == fd)
		{
			return i;
		}
	}
	return -1;
}
void CScreenShareServerDlg::SetBottomRightString()
{
	CString str;
	str.Format("��ǰ������:%d", m_allClientSocket.size());
	SetDlgItemText(IDC_Info, str);
}
void CScreenShareServerDlg::ChangeTranferPercent(SOCKET s, int percent)
{
	int row = GetListCtrlRow(s);
	if (row != -1)
	{
		CString strPercent;
		strPercent.Format("%d%%", percent);
		m_listCtrl.SetItemText(row, 2, strPercent);
	}
}

DWORD WINAPI shareScreenThread(LPVOID p)
{
	CScreenShareServerDlg * pDlg = (CScreenShareServerDlg*)p;
	pDlg->AddLog("��ʼ����");
	while (1)
	{
		if (!pDlg->m_bIsShareScreen)
			break;
		pDlg->m_udpSocket.SendScreenShot();
		
	}
	pDlg->AddLog("�رչ���");
	return 0;
}

void CScreenShareServerDlg::OnBnClickedButtonShareScreen()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bIsShareScreen)
	{
		m_bIsShareScreen = true;
		m_hThreadHandle = CreateThread(NULL, 0, shareScreenThread, this, 0, 0);
		GetDlgItem(IDC_BTN_SHARE_SCREEN)->SetWindowText("�رչ���");
	}
	else
	{
		m_bIsShareScreen = false;
		CloseHandle(m_hThreadHandle);
		GetDlgItem(IDC_BTN_SHARE_SCREEN)->SetWindowText("�򿪹���");
	}
}
