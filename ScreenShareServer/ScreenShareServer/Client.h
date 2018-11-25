#ifndef CLIENT_H
#define CLIENT_H
#include"stdafx.h"
#include"FileOperate.h"
#include<vector>
using namespace std;
#define BUF_SIZE 32767
class CClient
{
public:
	CClient();
	CClient(SOCKET,CWnd *);
	~CClient();
	//CClient operator=(const CClient&);
	void CloseSocket(int);
	void RecvMsg();//������Ϣ�����յ�FD_READʱ����
	void StartSendFile();//����get�������ã���ʱ����һ�������ļ���С���ļ����Ƶİ�ͷ
	void ContinueSendFile();//���յ��ͻ��˵�ȷ��ʱ�ļ�
	void SendPackHeader();//ÿ�η����ļ���ǰ�ķ��͵İ���������һ�����Ĵ�С

	//��������ַ��������������������������ڲ��������ģ���m_commandList��ƥ��
	void SplitCommandAndParam(const CString &, CString &, CString &);
	//��send�ķ�װ�����ڷ�ֹ���͵����ݰ��ͻ���һ��ֻ������һ���ֵ��������Ҫ����send����ֵ����ʣ�ಿ��
	void SendData(const char *data, int len,bool isFile=true);
	Header MakeHead(const CString& msg, const CString & fileName, int len, MSGTYPE msgType);//�����ͷ�����ذ��Ľṹ��
	bool HandleAttackMsg(const CString &,int len);//�����յ��ͻ���Ӧ����Ϣ
	//����ͻ��˷��͵������get cd ls pwd��,��Ϊ��һ���汾�ͻ����ǿ���̨ʱ���������˰���δʹ��
	//bool HandleCommmandMsg(const CString &);
	void setFileName(const CString &file) { m_fileName = file; }
	

	SOCKET m_socket; //��ǰ�ͻ��˵�socket
private:	
	CWnd *m_pDlg;//������ָ��
	
	CString m_fileName;//��ǰ���͵��ļ���
	CString m_currentDir;//�ͻ���ͨ�� cd �õ��ĵ�ǰ·��
	vector<CString>m_commandList;
	CString m_startSendAttackMsg = "###START_HEAD###";//��ʼ�����ļ�ʱ�ĵ�һ����
	CString m_recvPackAttackMsg = "###SUB_PACK###";//�ļ��ְ�ʱ�ĵİ�ȷ��
	CString m_recvCurrentPackEndMsg = "###SUB_PACK_END###";//��ǰ���������ȷ��
	long m_curTotalLen;//��ǰ���͵��ļ�����
	long m_curHaveSendLen;//��ǰ�ѷ��͵��ļ�����

};
#endif
