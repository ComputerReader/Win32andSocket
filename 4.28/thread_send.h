#pragma once
#include <WinSock2.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include "sever_config.h"
#pragma comment(lib,"ws2_32.lib")

//���������߳�
unsigned __stdcall ThreadSend(void* param) {
	int flagSend = *(int*)param;
	int ret = 0;
	char tempBuf[128] = { 0 };                         //����һ����ʱ�����ݻ�������������Ž��յ�������
	char tempUserName[16] = { 0 };
	ClientList *p;
	p = clientHead->next;
	while (p != NULL) {//�ж����ĸ��ͻ��˷�����Ϣ
		if (flagSend == p->flag){
			break;
		}
		p = p->next;
	}
	memcpy(tempBuf, p->buf, sizeof(tempBuf));
	memcpy(tempUserName, p->userName, sizeof(tempUserName));
	//���Ҫ���ã����������Լ����Լ�����Ϣ��BUG
	p = clientHead->next;
	while (p != NULL) {
		if (flagSend != p->flag) {//��Ҫ�����Լ�
			sprintf(p->buf, "%s: %s", tempUserName, tempBuf);//���һ���û���ͷ

			if (strlen(tempBuf) != 0 && iStatus == SEND_YET) //������ݲ�Ϊ���һ�ûת����ת��
				ret = send(p->sClient, p->buf, sizeof(p->buf), 0);
			if (ret == SOCKET_ERROR)
				return 1;
		}
		p = p->next;
	}

	iStatus = SEND_OVER;   //ȫ���û�ת���ɹ�������״̬Ϊ��ת��
	return 0;
}