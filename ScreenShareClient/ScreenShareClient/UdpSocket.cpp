#include "stdafx.h"
#include "UdpSocket.h"

const float n = 0.5;
UdpSocket::UdpSocket()
{
	m_picData = new char[1024 * 1024 * 2];//һ��ʼ����2MB�ڴ�
	mdc = new CDC;
	m_rcvBmp = new CBitmap;
	nWidth = GetSystemMetrics(SM_CXSCREEN);        //��Ļ���
	nHeight = GetSystemMetrics(SM_CYSCREEN);       //��Ļ�߶�
	
}


UdpSocket::~UdpSocket()
{
	delete[]m_picData;
	delete mdc;
	delete m_rcvBmp;
	m_drawDC->DeleteDC();
	
}
void UdpSocket::OutputFormatError(int line)
{
	TCHAR szBuf[128];
	LPVOID lpMsgBuf;
	DWORD dw = GetLastError();
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	sprintf(szBuf, "(�к�:%d ������=%d): %s", line, dw, lpMsgBuf);
	LocalFree(lpMsgBuf);
	AfxMessageBox(szBuf);

}
bool UdpSocket::initSocket()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	//SOCKET m_listenSocket;
	SOCKADDR_IN serverAddr;
	int addrLen = sizeof(serverAddr);
	memset(&serverAddr, 0, addrLen);

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	serverAddr.sin_port = htons(8000);
	m_listenSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_listenSocket == INVALID_SOCKET)
	{
		OutputFormatError(__LINE__);
		WSACleanup();
		return false;
	}
	bool bOpt = true;
	setsockopt(m_listenSocket, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt));
	setsockopt(m_listenSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&bOpt, sizeof(bOpt));//���õ�ַ������
	int nRecvBuf = 1024 * 1024 * 5;//���ջ���5M
	if (setsockopt(m_listenSocket, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int)) == SOCKET_ERROR)
		;// cout << "���û�����ʧ��" << endl;
	if (::bind(m_listenSocket, (sockaddr*)&serverAddr, addrLen) == SOCKET_ERROR)
	{
		OutputFormatError(__LINE__);
		WSACleanup();
		return false;
	}

	sin_from.sin_family = AF_INET;
	sin_from.sin_port = htons(8000);
	sin_from.sin_addr.s_addr = INADDR_BROADCAST;
	//sin_from.sin_addr.S_un.S_addr = INADDR_ANY;
	
	
	//u_long mode = 1;
	//ioctlsocket(m_listenSocket, FIONBIO, &mode);
	long recvLen = 1024 * 32 + sizeof(UdpHeader);
	setsockopt(m_listenSocket, SOL_SOCKET, SO_RCVBUF, (char*)&recvLen, sizeof(long));

/*	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	setsockopt(m_listenSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&tv, sizeof(tv));//���÷��ͳ�ʱ
*/
	//��Ҫ��setDC
	mdc->CreateCompatibleDC(m_drawDC);
	m_rcvBmp->CreateCompatibleBitmap(m_drawDC, nWidth * n, nHeight * n);
	return true;
}
UdpHeader UdpSocket::paraseHeader(char *data, int len)
{
	UdpHeader tmp;
	if (len < headLen)
		;//cout << "error";
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
void UdpSocket::ReceiveSreenShot()
{
	long recvLen = 1024 * 32 + sizeof(UdpHeader);
	int addrLen = sizeof(sin_from);
	while (1)
	{
		char *buf = new char[recvLen];
		//�˴�ʹ�õ�������socket
		int len = recvfrom(m_listenSocket, buf, recvLen, 0, (SOCKADDR*)&sin_from, &addrLen);
		if (len > 0)
		{
			recvData.push_back(buf);
			auto curHead = paraseHeader(buf, len);
			
			CString log;
			log.Format("ver: %d , %d\n", curHead.mainId, curHead.subId);
			Log::WrtiteLog(log);

			if (curHead.bOK)
			{
				vector<UdpHeader>recvHeader;
				recvHeader.reserve(recvData.size());
				m_totalPackNum += recvData.size();//�ܹ����յİ�
				for (auto p : recvData)//�������е�header��������
				{
					recvHeader.push_back(paraseHeader(p, headLen + 1));
				}
				
		/* TODO ��ԭ���ƻ�����һ��ͼƬ������ɺ󣬲��Ҷ�ʧ��͸�����ˣ��������·��Ͷ�ʧ�����Ȼ���뵽udp�Ĺ㲥
				��������ͻ��˽��յ��Ĳ�һ�£�Ҳ����˵��ʧ�Ĳ�һ�£�������������˷��Ͷ�ʧ���ǻ������ˣ��������ڸĳ�
				��ÿ�η���һ�������ӳټ����룬��Ȼ��󽵵��˶����ļ��ʡ�
		����һ���뷨����δ�ֶ�ʵ�֣�����ÿ�����������飬�������㶪һ��Ҳ��Ҫ��

			vector<int> loseItem = DetectLoseItem(recvHeader, curHead);//���Ҷ�ʧͼƬ�Ĵΰ汾��
				if (!loseItem.empty())
				{
					CString strLose="";
					for (auto &ele : loseItem)
					{
						CString tmp;
						tmp.Format("%d|", ele);
						strLose += tmp;
					}
					//�˴������ý��ճ�ʱΪ1s�������궪ʧ�İ�����ȡ����ʱ
					sendto(m_listenSocket, strLose, strLose.GetLength(), 0, (SOCKADDR*)&sin_from, addrLen);
					struct timeval tv = { 1,0 };
					setsockopt(m_listenSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));//���ý��ճ�ʱ

					int loseNum = loseItem.size();
					while (loseNum--)
					{
						int recvLoseItemLen = recvfrom(m_listenSocket, buf, recvLen, 0, (SOCKADDR*)&sin_from, &addrLen);
						if (recvLoseItemLen > 0)
						{
							m_totalPackNum++;
							recvData.push_back(buf);
							recvHeader.push_back(paraseHeader(buf, headLen + 1));
						}
					}
					tv.tv_sec = 0;
					setsockopt(m_listenSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));//ȡ�����ճ�ʱ	
				}
				*/

				ConcatData(recvHeader,curHead);//ƴ��ͼƬ
				DrawScreenShot();//���ƽ�ͼ
				DeleteCache(recvHeader, curHead.mainId);//ɾ������
				CalcQos(curHead.sendTime);//���㴫������
				break;
			}
		}
		else
			delete[]buf;
	}//while(true)
}
/*vector<int> UdpSocket::DetectLoseItem(const vector<UdpHeader>&recvHeader, const UdpHeader &curHead)
{
	vector<int> loseItem;
	int mainId = curHead.mainId;
	for (int i = 0; i < curHead.subId; i++)
	{
		bool bFind = false;
		for (auto p : recvHeader)
		{
			if (p.mainId == mainId && p.subId == i)
			{
				bFind = true;
			}
		}
		if (!bFind)
		{
			loseItem.push_back(i);
		}

	}
	return loseItem;
}*/
void UdpSocket::ConcatData(const vector<UdpHeader>&recvHeader, const UdpHeader &curHead)
{
	long totalLen = 0;
	int mainId = curHead.mainId;
	int subId = curHead.subId;//�˴���subIdҲ�����id������������
	for (int j = 0; j <= subId; j++)
	{
		for (int i = 0; i < recvHeader.size(); i++)
		{//
			if (recvHeader[i].subId == j&& recvHeader[i].mainId == mainId)
			{
				m_recvPackNum++;//���õİ�
				memcpy(m_picData + totalLen, recvData[i] + headLen, recvHeader[i].dataLen);
				totalLen += recvHeader[i].dataLen;
			}
		}
	}
}
void UdpSocket::DrawScreenShot()
{
	//��������ͼƬ���Ƴ���
	CRect rc;
	m_drawDC->GetWindow()->GetWindowRect(&rc);
	m_rcvBmp->SetBitmapBits(rc.Width()*rc.Height() * 4, m_picData);
	mdc->SelectObject(m_rcvBmp);
	m_drawDC->SetStretchBltMode(HALFTONE);
	m_drawDC->StretchBlt(0, 0,rc.Width(),rc.Height(), mdc, 0, 0, nWidth * n, nHeight * n, SRCCOPY);
}
void UdpSocket::DeleteCache(vector<UdpHeader>&recvHeader,int mainId)
{
	//Ϊ��ֹ��һ��ͼ���һ��ͼ�ķ������򣬼��ڶ���ͼ�İ��ڵ�һ�����һ��������֮ǰ���ͣ�������ղ���ֱ��ȫ������
	vector<char*>tempRecvData;
	for (int i = 0; i < recvHeader.size(); i++)
	{
		if (recvHeader[i].mainId > mainId)
		{
			tempRecvData.push_back(recvData[i]);//�����°汾�Ÿߵ�����
		}
		else
			delete[]recvData[i];//��������ɾ��
	}
	recvData.swap(tempRecvData);
	tempRecvData.swap(vector<char*>());
	recvHeader.swap(vector<UdpHeader>());
//	totalLen = 0;
}
void UdpSocket::CalcQos(long curTime )
{
	m_losePercent = (1 - m_recvPackNum / (float)m_totalPackNum) * 100;//���㶪����
	m_ping = curTime - m_lastPackTime;//��������һ�������ӳ�
	m_lastPackTime = curTime;
}