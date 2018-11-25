#include "stdafx.h"
#include "UdpSocket.h"
#include "ScreenShareServer.h"
#include "ScreenShareServerDlg.h"
const float n = 0.5;
CUdpSocket::CUdpSocket()
{
	nWidth = GetSystemMetrics(SM_CXSCREEN);        //��Ļ���
	nHeight = GetSystemMetrics(SM_CYSCREEN);       //��Ļ�߶�
	DesktopWnd = ::GetDesktopWindow();             // ��ȡ������
	DesktophDC = ::GetDC(DesktopWnd);              // Ϊ��Ļ�����豸������
	MemoryhDC = CreateCompatibleDC(DesktophDC);    // Ϊ��Ļ�豸�����������ݵ��ڴ��豸������
	hbm = CreateCompatibleBitmap(DesktophDC, nWidth * n, nHeight * n);// ����һ������Ļ�豸��������ݵ�λͼ

	bits = new BYTE[nWidth * nHeight * n * n * 4];
	fileLen = nWidth * nHeight * n * n * 4;
}


CUdpSocket::~CUdpSocket()
{
	delete[]bits;
	::ReleaseDC(DesktopWnd, DesktophDC);
	::DeleteDC(MemoryhDC);
	::DeleteObject(hbm);
}
/*CString CUdpSocket::GetIp()
{
	CString m_broadCastIp;
	char name[128] = { 0 };
	gethostname(name, 128);
	CString str;
	str.Format(_T("%s"), name);
	//AfxMessageBox(str);
	struct hostent *ph = gethostbyname(name);
	if (ph == NULL)
		AfxMessageBox("��ȡ����IPʧ��");
	else
	{
		in_addr addr;
		memcpy(&addr, ph->h_addr_list[0], sizeof(in_addr));
		m_broadCastIp = inet_ntoa(addr);
		int pos = m_broadCastIp.ReverseFind('.');
		m_broadCastIp = m_broadCastIp.Left(pos) + ".255"; //ͬһ���Σ����һλΪ255
		//m_ip.SetWindowText(str);
	//	AfxMessageBox(m_broadCastIp);
	}
	return m_broadCastIp;
}*/

bool CUdpSocket::InitUdpSock()
{
//	CString m_broadCastIp = GetIp();
//	AfxMessageBox(m_broadCastIp);
	int addrLen = sizeof(m_serverAddr);
	memset(&m_serverAddr, 0, addrLen);
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	m_serverAddr.sin_family = AF_INET;
	m_serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_BROADCAST);//inet_addr(m_broadCastIp);//
	m_serverAddr.sin_port = htons(8000);
	m_udpSock = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_udpSock == INVALID_SOCKET)
	{
		WSACleanup();
		return false;
	}
	bool bOpt = true;
	//���ø��׽���Ϊ�㲥����  
	int res = setsockopt(m_udpSock, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt));

	//����200k������
	long sendBufLen=1024 * 32;
	int len = sizeof(long);
	setsockopt(m_udpSock, SOL_SOCKET, SO_SNDBUF, (char*)&sendBufLen, len);
	
	//getsockopt(m_udpSock, SOL_SOCKET, SO_SNDBUF, (char*)&sendLen, &len);
	//cout << sendLen << endl;

	if (res != 0)
		return false;
	return true;
}
/*vector<CString> SplitCString(CString strSource, CString ch)
{

	vector <CString> vecString;
	int iPos = 0;
	CString strTmp;
	strTmp = strSource.Tokenize(ch, iPos);
	while (strTmp.Trim() != _T(""))
	{
		vecString.push_back(strTmp);
		strTmp = strSource.Tokenize(ch, iPos);
	}
	return vecString;
}*/

void CUdpSocket::SendScreenShot()
{
	holdbm = (HBITMAP)SelectObject(MemoryhDC, hbm); // �����µ�λͼhbm,����MemoryhDCԭ�Ⱦɵ�λͼ��holdbm
	StretchBlt(MemoryhDC, 0, 0, nWidth * n, nHeight * n, DesktophDC, 0, 0, nWidth, nHeight, SRCCOPY);  // �������浽MemoryhDC��
	hbm = (HBITMAP)SelectObject(MemoryhDC, holdbm); // ��ԭMemoryhDCԭ�ȵ�λͼ,�����µ�λͼ��hbm

	CBitmap *pbm;
	pbm = CBitmap::FromHandle(hbm);
	pbm->GetBitmapBits(nWidth * nHeight * n * n * 4 * sizeof(BYTE), bits);	
	/*�������Ʋ���
	CScreenShareServerDlg *p = (CScreenShareServerDlg *)m_pDlg;
	CDC *pDC = p->GetDC();
	CDC *mdc = new CDC;
	CBitmap *bmp = new CBitmap;
	mdc->CreateCompatibleDC(pDC);
	bmp->CreateCompatibleBitmap(pDC, nWidth * n, nHeight * n);
	bmp->SetBitmapBits(nWidth * nHeight * n * n * 4, bits);
	mdc->SelectObject(bmp);
	pDC->StretchBlt(0, 0, nWidth * n, nHeight * n, mdc, 0, 0, nWidth * n, nHeight * n, SRCCOPY);
	pDC->DeleteDC();
	delete mdc;
	delete bmp;
*/
	
	int haveSendLen = 0;
	long sendBufLen = 1024 * 32;

	int subId = 0;
	while (haveSendLen < fileLen)
	{
		int curSendLen;//����Ҫ���͵ĳ���
		UdpHeader head;
		int headLen = sizeof(UdpHeader);
		if (fileLen - haveSendLen > sendBufLen)//���ʣ�೤�ȴ���32k������32k
		{
			curSendLen = sendBufLen;
			head.bOK = 0;
			head.mainId = m_sendId;
			head.subId = subId++;
			head.dataLen = sendBufLen;
			head.sendTime = clock();
		}
		else//������ʣ�ಿ��
		{
			curSendLen = fileLen - haveSendLen;
			head.bOK = 1;
			head.mainId = m_sendId;
			head.subId = subId++;
			head.dataLen = fileLen - haveSendLen;
			head.sendTime = clock();
		}
		byte *buf = new byte[curSendLen + headLen];
		memset(buf, 0, curSendLen + headLen);
		memcpy(buf, &head, headLen);//����ͷ��
		memcpy(buf + headLen, bits + haveSendLen, curSendLen);//������ǰ����

		int sendlen = sendto(m_udpSock, (char*)buf, curSendLen + headLen, 0, (sockaddr*)&m_serverAddr, sizeof(m_serverAddr));
		haveSendLen += curSendLen;
		Sleep(6);

		/*
		TODO ��ԭ���ƻ�����һ��ͼƬ������ɺ󣬲��Ҷ�ʧ��͸�����ˣ��������·��Ͷ�ʧ�����Ȼ���뵽udp�Ĺ㲥
				��������ͻ��˽��յ��Ĳ�һ�£�Ҳ����˵��ʧ�Ĳ�һ�£�������������˷��Ͷ�ʧ���ǻ������ˣ��������ڸĳ�
				��ÿ�η���һ�������ӳټ����룬��Ȼ��󽵵��˶����ļ��ʡ�
		����һ���뷨����δ�ֶ�ʵ�֣�����ÿ�����������飬�������㶪һ��Ҳ��Ҫ��
		if (head.bOK == 1)
		{
			struct timeval tv = { 1,0 };
			setsockopt(m_udpSock, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));//���ý��ճ�ʱ
			int addrLen = sizeof(m_serverAddr);
			char * recvBuf = new char[128];
			ZeroMemory(recvBuf, 128);
			int recvLen = recvfrom(m_udpSock, (char*)recvBuf, 128, 0, (sockaddr*)&m_serverAddr, &addrLen);
			if (recvLen > 0)
			{
				vector<CString> res = SplitCString(recvBuf, "|");
				
				for (int i = 0; i < res.size() - 1; i++)
				{
					head.bOK = 0;
					head.mainId = m_sendId;
					head.subId = atoi(res[i]);
					head.dataLen = sendBufLen;
					head.sendTime = clock();
					memset(buf, 0, sendBufLen + headLen);
					memcpy(buf, &head, headLen);//����ͷ��
					memcpy(buf + headLen, bits + atoi(res[i]) * sendBufLen, sendBufLen);//������ǰ����
					sendto(m_udpSock, (char*)buf, sendBufLen + headLen, 0, (sockaddr*)&m_serverAddr, sizeof(m_serverAddr));
				}
			}
			delete[] recvBuf;
		}
		*/

		delete[]buf;

	}

	m_sendId++;
}