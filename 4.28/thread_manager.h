#pragma once
#include <WinSock2.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include "client_init.h"
#include "sever_config.h"
#pragma comment(lib,"ws2_32.lib")
//��������
unsigned __stdcall ThreadManager(void* param) {
	while (1) {
		ClientList *p = clientHead;
		while (p->next != NULL) {
			if (send(p->next->sClient, "", sizeof(""), 0) == SOCKET_ERROR) {
				if (p->next->sClient != 0) {
						//if (p->next->hRecv)
							CloseHandle(p->next->hRecv);
						//if (p->next->hSend)
							CloseHandle(p->next->hSend);
					printf("Disconnect from IP: %s,UserName: %s\n", p->next->IP, p->next->userName);
					closeClient(p->next);   //����򵥵��жϣ���������Ϣʧ�ܣ�����Ϊ�����ж�(��ԭ���ж���)���رո��׽���
					//��Client������ɾ��
					p->next = p->next->next;
					//����ر����߳̾�������ǲ��Խ���Ͽ���C/S�Ӻ�CPU��Ȼ����
				}
			}
			p = p->next;
		}
		Sleep(2000); //2s���һ��
	}
	return 0;
}