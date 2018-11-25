#pragma once
struct UdpHeader
{
	long mainId;
	int subId;
	clock_t  sendTime;
	int bOK;
	long dataLen;
};
class CUdpSocket
{
public:
	CUdpSocket();
	~CUdpSocket();
	bool InitUdpSock();
	void SendScreenShot();
	SOCKADDR_IN m_serverAddr;
	void setHandle(CWnd *p) { m_pDlg = p; }
	CWnd *m_pDlg;//������ָ��
	SOCKET m_udpSock;//udp���͵�socket
	long m_sendId = 0; //��Ϊ���͵�����֡�����汾��

	//CString GetIp();
	
private:
	//����ı������ǽ�ͼ���
	int nWidth, nHeight;
	HWND DesktopWnd;
	HDC DesktophDC, MemoryhDC;
	HBITMAP hbm, holdbm;
	BYTE *bits;//��ͼ���ͼ������
	int fileLen;
};

