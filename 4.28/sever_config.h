#pragma once
#include <WinSock2.h>
#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
#pragma comment(lib,"ws2_32.lib")
//�ͻ�����Ϣ�ṹ��
typedef struct _Client {
	SOCKET sClient;      //�ͻ����׽���
	char buf[128];       //���ݻ�����
	char userName[16];   //�ͻ����û���
	char IP[20];         //�ͻ���IP
	UINT_PTR flag;       //��ǿͻ��ˣ��������ֲ�ͬ�Ŀͻ���
	HANDLE hRecv;      //������Ϣ���߳�
	HANDLE hSend;		 //������Ϣ���߳�
	struct _Client* next; //��������һ���ͻ���
}Client,ClientList;

#define maxChatNum 2						//������������������
#define minChatNum 2						//������������������
#define currentChatNum 3                     //��ǰ������������
#define SEND_OVER 1                          //�Ѿ�ת����Ϣ
#define SEND_YET  0                          //��ûת����Ϣ
int iStatus = SEND_YET;
SOCKET ServerSocket = INVALID_SOCKET;      //������׽���
SOCKADDR_IN ClientAddr = { 0 };            //�ͻ��˵�ַ
int iClientAddrLen = sizeof(ClientAddr);

//Client client[currentChatNum] = { 0 };                  //����һ���ͻ��˽ṹ��
ClientList* clientHead; //����һ���ͻ��˽ṹ������ͷ