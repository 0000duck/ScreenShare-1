
// ScreenShareClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ScreenShareClient.h"
#include "ScreenShareClientDlg.h"
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


// CScreenShareClientDlg �Ի���



CScreenShareClientDlg::CScreenShareClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SCREENSHARECLIENT_DIALOG, pParent),
	m_bIsRecvFile(false),
	m_totalSize(0),
	m_haveGetSize(0),
	m_currentPackSize(0),
	m_currentPackGetSize(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
}

void CScreenShareClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listChat);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
}

BEGIN_MESSAGE_MAP(CScreenShareClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CScreenShareClientDlg::OnBnClickedButtonTcp)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON2, &CScreenShareClientDlg::OnBnClickedSharedScreen)
//	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CScreenShareClientDlg ��Ϣ�������

BOOL CScreenShareClientDlg::OnInitDialog()
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

	Log::CreateLog();

	initSock();
	m_pDC = GetDC();
	m_picDC = GetDlgItem(IDC_PIC)->GetDC();
	m_udpSocket.setDrawDC(m_picDC); //�˴�����Ҫ��initSocket֮ǰ����Ϊ��initSocket����Ҫ���û�ͼ��DC
	if (!m_udpSocket.initSocket())
		AfxMessageBox("��ʼ��updʧ��");
	
	m_progress.SetRange(0, 100);
	
	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CScreenShareClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CScreenShareClientDlg::OnPaint()
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
HCURSOR CScreenShareClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CScreenShareClientDlg::OutputFormatError(int line)
{
	TCHAR szBuf[128];
	LPVOID lpMsgBuf;
	DWORD dw = GetLastError();
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	sprintf(szBuf, "(�к�:%d ������=%d): %s", line, dw, lpMsgBuf);
	LocalFree(lpMsgBuf);
	MessageBox(szBuf);

}
void CScreenShareClientDlg::setStatus(int s)
{
	if (s)
		SetDlgItemText(IDC_Status, "tcp����״̬:������");
	else
		SetDlgItemText(IDC_Status, "tcp����״̬:δ����");
}
void CScreenShareClientDlg::setUdpStatus()
{
	CString str;
	str.Format("udp�ӳ�:%dms", m_udpSocket.m_ping);
	SetDlgItemText(IDC_TEXT_Ping, str);
	str.Format("udp������:%.1f%%", m_udpSocket.m_losePercent);
	SetDlgItemText(IDC_TEXT_LOSE, str);
}
void CScreenShareClientDlg::initSock()
{
	WSAData wsa;
	ZeroMemory(&wsa, sizeof(WSAData));
	WSAStartup(MAKEWORD(2, 2), &wsa);
	SOCKADDR_IN sockAddr;
	int sockAddrLen = sizeof(sockAddr);
	ZeroMemory(&sockAddr, sockAddrLen);
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//inet_addr("172.20.10.5");//inet_addr("192.168.137.1");
	sockAddr.sin_port = htons(8000);

	m_tcpSock = socket(AF_INET, SOCK_STREAM, 0);


	if (m_tcpSock == INVALID_SOCKET)
	{
		OutputFormatError(__LINE__);
		return;
	}
	if (connect(m_tcpSock, (sockaddr*)&sockAddr, sockAddrLen) == SOCKET_ERROR)
	{
		m_bConnectTcp = false;
		OutputFormatError(__LINE__);
		return;
	}
	else
	{
		m_bConnectTcp = true;
		GetDlgItem(IDC_BUTTON1)->SetWindowTextA("�Ͽ�����");
	}
	int recvBufSize = 1024 * 64 - 1;
	setsockopt(m_tcpSock, SOL_SOCKET, SO_RCVBUF, (char*)&recvBufSize, sizeof(recvBufSize));
	//AfxMessageBox("")
	setStatus(1);
	WSAAsyncSelect(m_tcpSock, m_hWnd, WM_CUSTOM_NETWORK_MSG, FD_READ | FD_WRITE | FD_CLOSE);
	m_bIsRecvFile = false;
}

void CScreenShareClientDlg::SetFullScreen()
{
	int cx, cy;
	cx = GetSystemMetrics(SM_CXSCREEN);
	cy = GetSystemMetrics(SM_CYSCREEN);
	if (!m_bFullScreen)
	{
		GetClientRect(m_oldClientRect);
		CRect rcTemp;
		rcTemp.BottomRight() = CPoint(cx, cy);
		rcTemp.TopLeft() = CPoint(0, 0);
		MoveWindow(&rcTemp);
		LONG Style = ::GetWindowLong(this->m_hWnd, GWL_STYLE);
		::SetWindowLong(this->m_hWnd, GWL_STYLE, Style&~WS_CAPTION);
		::SetWindowPos(this->m_hWnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
		m_bFullScreen = true;
		
		m_udpSocket.setDrawDC(m_pDC);
	//	GetDlgItem(IDC_PIC)->MoveWindow(&rcTemp);
	}
	else
	{
		CRect rcTemp;
		rcTemp.TopLeft() = CPoint(150,100);
		rcTemp.BottomRight() = CPoint(m_oldClientRect.Width() + 170 /*+ (m_oldClientRect.Width() - cx) >> 2*/,
			m_oldClientRect.Height() + 140 /*+ (m_oldClientRect.Height() - cy) >> 2*/);
		MoveWindow(&rcTemp);
		LONG Style = ::GetWindowLong(this->m_hWnd, GWL_STYLE);
		::SetWindowLong(this->m_hWnd, GWL_STYLE, Style | WS_CAPTION);
		::SetWindowPos(this->m_hWnd, NULL,0,0, 0,0,SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
		m_bFullScreen = false;
		m_udpSocket.setDrawDC(m_picDC);
		Invalidate();
	}
}

BOOL CScreenShareClientDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_CUSTOM_NETWORK_MSG)
	{
		long event = WSAGETSELECTEVENT(pMsg->lParam);
		if (event == FD_READ)
		{
			char buf[32767] = { 0 };
			int len = recv(m_tcpSock, buf, 32767, 0);
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				AfxMessageBox("����");
				return false;
			}
			else if (len <= 0)
			{
				AfxMessageBox("�����˶Ͽ�����");
				::closesocket(m_tcpSock);
				m_bConnectTcp = false;
				GetDlgItem(IDC_BUTTON1)->SetWindowTextA("����TCP����");
				setStatus(0);
			}
			else
			{
				//����պ��ļ����͵ĳ����ǰ�ͷ�ĳ��ȣ��ǲ��ܱ��󵼽����ɰ�ͷ��
				if (len == HEADLEN && handleHeaderPack(buf, len))
					return true;
				if (m_bIsRecvFile)
					handleRecvFile(buf, len);
				else
				{
					buf[len] = '\0';
					m_listChat.AddString(buf);
				}

			}
		}
		if (event == FD_WRITE)
		{

		}
		if (event == FD_CLOSE)
		{
			AfxMessageBox("�����˶Ͽ�����");
			::closesocket(m_tcpSock);
			m_bConnectTcp = false;
			GetDlgItem(IDC_BUTTON1)->SetWindowTextA("����TCP����");
			setStatus(0);
		}
	}
	else if (pMsg->message == WM_KEYDOWN&&pMsg->hwnd == GetDlgItem(IDC_EDIT1)->m_hWnd && pMsg->wParam==VK_RETURN)
	{
		//OnBnClickedButtonTcp();
		CString strMsg;
		GetDlgItemText(IDC_EDIT1, strMsg);
		if (!strMsg.IsEmpty())
		{
			SendData(strMsg, strMsg.GetLength());
			SetDlgItemText(IDC_EDIT1, "");
		}
	}
	else if (pMsg->message == WM_LBUTTONDBLCLK /*&& pMsg->hwnd == GetDlgItem(IDC_PIC)->m_hWnd*/)
	{
		SetFullScreen();
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
bool CScreenShareClientDlg::handleHeaderPack(char buf[],int len)
{
	Header * p = reinterpret_cast<Header*>(buf);
	if (strcmp(p->msg, "firstPack") == 0)//�˴��Ƿ����ļ��ĵ�һ����������������ļ������ļ����ܴ�С
	{
		m_bIsRecvFile = true;
		m_progress.ShowWindow(SW_SHOW);

		m_recvFilePath.Format("E:\\%s", p->fileName);
		m_totalSize = p->totalLen;
		m_haveGetSize = 0;
		FILE * fp = fopen(m_recvFilePath, "w");//�½��ļ���Ȼ�������׷�ӷ�ʽд���ļ�
		fclose(fp);
		SendData(m_startSendAttackMsg, m_startSendAttackMsg.GetLength());
		return true;
	}
	else if (strcmp(p->msg, "SubPack") == 0)//�յ������ļ��İ�
	{
		m_currentPackGetSize = 0;
		m_currentPackSize = p->totalLen;
		//writeQueue.push_back("###SUB_PACK###");
		SendData(m_recvPackAttackMsg, m_recvPackAttackMsg.GetLength());
		return true;
	}
	return false;
}
void CScreenShareClientDlg::handleRecvFile(char buf[], int len)
{
	FILE * fp = fopen(m_recvFilePath, "ab+");
	if (fp)
	{
		fwrite(buf, 1, len, fp);
		fclose(fp);
	}
	else
	{
		OutputFormatError(__LINE__);
	}
	m_haveGetSize += len;
	m_currentPackGetSize += len;
	int pos = m_haveGetSize / (float)m_totalSize *100;
	m_progress.SetPos(pos);
	if (m_currentPackSize == m_currentPackGetSize)
		SendData(m_recvCurrentPackEndMsg, m_recvCurrentPackEndMsg.GetLength());
	//printf("Have download : %.2f%%\n", haveGetSize / (float)totalSize *100.0);

	if (m_haveGetSize == m_totalSize)
	{
		m_haveGetSize = 0;
		m_totalSize = 0;
		m_currentPackSize = 0;
		m_currentPackGetSize = 0;
		//writeQueue.push_back("���ճɹ�");
		m_bIsRecvFile = false;
		//m_progress.ShowWindow(SW_HIDE);
	}
}
bool CScreenShareClientDlg::SendData(const char * data, int len,bool isfile)
{
	int totalLen = len;
	int sendLen = 0;
	int haveSendLen = 0;
	while (haveSendLen != len)
	{
		//send����ֵ����0����Ϊ���͵ĳ��ȣ�=0��Ϊ�ر���sokcet�� С��0��Ϊ�쳣
		sendLen = send(m_tcpSock, data + haveSendLen, totalLen, 0);
		if (sendLen>0)
		{
			haveSendLen += sendLen;
			//�˴����ڶԷ����ղ�ȫ����������緢��10k����ֻ������3k����sendLen����3k
			totalLen -= sendLen;
		}
		else if (sendLen == 0)
		{
			//�ر���Socket
			::closesocket(m_tcpSock);
		}
		else
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				sendLen = 0;
			}
			else
			{
				return false;
			//	break;
			}
		}

	}
	return true;
}

void CScreenShareClientDlg::OnBnClickedButtonTcp()
{
	if (!m_bConnectTcp)
	{
		initSock();
	}
	else
	{
		::closesocket(m_tcpSock);
		WSACleanup();
		m_bConnectTcp = false;
		GetDlgItem(IDC_BUTTON1)->SetWindowTextA("����TCP����");
	}

/*	CString strMsg;
	GetDlgItemText(IDC_EDIT1, strMsg);
	if (strMsg.IsEmpty())
		return;
	else
	{
		SendData(strMsg, strMsg.GetLength());
		SetDlgItemText(IDC_EDIT1, "");
	}
	*/
}


void CScreenShareClientDlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���

	//CDialogEx::OnOK();
}


void CScreenShareClientDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_hThread)
	{
		CloseHandle(m_hThread);
		m_hThread = 0;
	}
	if (m_bSharedScreen)
		OnBnClickedSharedScreen();
	CDialogEx::OnClose();
	ReleaseDC(m_pDC);
	ReleaseDC(m_picDC);
}

UdpHeader paraseHeader(char *data, int len)
{
	UdpHeader tmp;
	if (len < headLen)
		;// cout << "error";
	else
	{
		char * buf = new char[headLen];
		memcpy(buf, data, headLen);
		UdpHeader *header = reinterpret_cast<UdpHeader*>(buf);
		tmp = *header;
		delete[]buf;
	}
	return tmp;
}
DWORD WINAPI CScreenShareClientDlg::ReceiveScreenShotProc(LPVOID p)
{
	CScreenShareClientDlg * pDlg = (CScreenShareClientDlg*)p;
	
	while (1)
	{
		if (!pDlg->m_bSharedScreen)
			break;
		pDlg->m_udpSocket.ReceiveSreenShot();
		pDlg->setUdpStatus();
		Sleep(10);
			
	}

	return 0;
}

void CScreenShareClientDlg::OnBnClickedSharedScreen()
{
	if (!m_bSharedScreen)
	{
		m_bSharedScreen = true;
		m_hThread = CreateThread(NULL, 0, ReceiveScreenShotProc, this, 0, 0);
		SetDlgItemText(IDC_BUTTON2, "�رչ�����Ļ");
		//m_udpSocket.setDrawDC(GetDlgItem(IDC_PIC)->GetDC());
	}
	else
	{
		m_bSharedScreen = false;
		if (m_hThread)
		{
			CloseHandle(m_hThread);
			m_hThread = 0;
		}
		SetDlgItemText(IDC_BUTTON2, "�򿪹�����Ļ");
	}
}
