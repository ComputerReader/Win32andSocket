#pragma once
#include <WinSock2.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include "sever_config.h"
#include "thread_send.h"
#pragma comment(lib,"ws2_32.lib")

//��������

unsigned __stdcall ThreadRecv(void* param) {
	SOCKET tempClient = INVALID_SOCKET;
	char temp[128] = { 0 };  //��ʱ���ݻ�����
	int flagSend = 0;

	ClientList *p;
	p = clientHead->next;
	while (p != NULL) {//�ж����ĸ��ͻ��˷�������Ϣ
		if (*(int*)param == p->flag) {
			tempClient = p->sClient;
			flagSend = p->flag;
			break;
		}
		p = p->next;
	}

	while (1) {
		//���Ͻ���ת����Ϣ
		memset(temp, 0, sizeof(temp));
		int ret = recv(tempClient, temp, sizeof(temp), 0); //��������
		if (ret != SOCKET_ERROR) {
			iStatus = SEND_YET;//����ת��״̬Ϊδת��
			memcpy(p->buf, temp, sizeof(p->buf));

			p->hSend = (HANDLE)_beginthreadex(NULL, 0, ThreadSend, &flagSend, 0, NULL); //����һ��ת���߳�,flagflagSend�����Ҫת���Ŀͻ���
			//����Ҳ�����ǵ���CPUʹ����������ԭ��

		}
	}
	return 0;
}