#pragma once
#include <WinSock2.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include "is_msg_enable.h"
#include "sever_config.h"
#include "thread_manager.h"
#include "thread_recv.h"
#pragma comment(lib,"ws2_32.lib")

//��������
unsigned __stdcall ThreadAccept(void* param) {


	_beginthreadex(NULL, 0, ThreadManager, NULL, 0, NULL);

	while (1) {
		int lenList = 0;
		while(1){
			lenList = 0;
			ClientList *p = clientHead->next;
			while (p != NULL) {
				lenList++;
				p = p->next;
			}
			if (lenList == currentChatNum) {
				continue;
			}

			ClientList *temp = (ClientList*)malloc(sizeof(ClientList));
			temp->next = NULL;
			//����пͻ����������Ӿͽ�������
			if ((temp->sClient = accept(ServerSocket, (SOCKADDR*)&ClientAddr, &iClientAddrLen)) == INVALID_SOCKET) {
				printf("accept failed with error code: %d\n", WSAGetLastError());
				closesocket(ServerSocket);
				WSACleanup();
				return -1;
			}
			//�����û���
			recv(temp->sClient, temp->userName, sizeof(temp->userName), 0);
			printf("Successfuuly got a connection from IP:%s ,Port: %d,UerName: %s\n",
				inet_ntoa(ClientAddr.sin_addr), htons(ClientAddr.sin_port), temp->userName);
			//��¼�ͻ���IP
			memcpy(temp->IP, inet_ntoa(ClientAddr.sin_addr), sizeof(temp->IP));
			temp->flag = temp->sClient; //��ͬ��socket�в�ͬUINT_PTR���͵���������ʶ
			temp->next = clientHead->next;
			clientHead->next = temp;

			lenList++;
			if (lenList == currentChatNum) {//�����������ټ�������
				break;
			}
		}
		//����--------------------------------------------------------------------------------------
		if (lenList == currentChatNum) {
			ClientList *p = clientHead->next;
			while (p != NULL) {
				p->hRecv = (HANDLE)_beginthreadex(NULL, 0, ThreadRecv, &p->flag, 0, NULL);
				p = p->next;
			}
		}

		Sleep(3000);
	}

	return 0;
}