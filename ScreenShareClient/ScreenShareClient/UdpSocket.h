#pragma once
#include<vector>
using namespace std;
struct UdpHeader
{
	long mainId;
	int subId;
	clock_t sendTime;
	int bOK;
	long dataLen;
};
const int headLen = sizeof(UdpHeader);
class UdpSocket
{
public:
	UdpSocket();
	~UdpSocket();
	bool initSocket();
	void OutputFormatError(int);
	UdpHeader paraseHeader(char *data, int len);
	void ReceiveSreenShot();
	SOCKET m_listenSocket;
	SOCKADDR_IN sin_from;

	float m_losePercent;//������
	int m_ping=0;//�ӳ�ms
	clock_t m_lastPackTime = 0;//��һ���յ�����ʱ��

	long m_totalPackNum = 0;//�ܹ��յ��İ��ĸ���
	long m_recvPackNum = 0;//���ð��ĸ���
	
	void setDrawDC(CDC *dc) { m_drawDC = dc; }

	void DrawScreenShot();//���ƽ�ͼ
	void DeleteCache(vector<UdpHeader>&,int);//���յ�һ�����һ������ɾ������
	void CalcQos(long);//���㴫������
	void ConcatData(const vector<UdpHeader>&recvHeader, const UdpHeader &curHead);//��ͼƬ����ƴ��
	//vector<int> DetectLoseItem(const vector<UdpHeader>&recvHeader, const UdpHeader &curHead);

private:
	CDC *m_drawDC; //��ͼ��DC
	char *m_picData; //ƴ�ӵ�ͼƬ���ݣ�Ϊ�˲������ظ������ͷ��ڴ棬���Ե�����Ա������һ��������
	CDC *mdc;//�ڴ�DC
	CBitmap *m_rcvBmp;//�յ�������ת��CBitmap
	vector<char*>recvData;//���յ��ķְ����ݴ����������ƴ������
	int nWidth, nHeight;
};

