#pragma once
#include <WinSock2.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include "sever_config.h"
#include "thread_send.h"
#pragma comment(lib,"ws2_32.lib")

//接受数据

unsigned __stdcall ThreadRecv(void* param) {
	SOCKET tempClient = INVALID_SOCKET;
	char temp[128] = { 0 };  //临时数据缓冲区
	int flagSend = 0;

	ClientList *p;
	p = clientHead->next;
	while (p != NULL) {//判断是哪个客户端发来的消息
		if (*(int*)param == p->flag) {
			tempClient = p->sClient;
			flagSend = p->flag;
			break;
		}
		p = p->next;
	}

	while (1) {
		//不断接受转发消息
		memset(temp, 0, sizeof(temp));
		int ret = recv(tempClient, temp, sizeof(temp), 0); //接收数据
		if (ret != SOCKET_ERROR) {
			iStatus = SEND_YET;//设置转发状态为未转发
			memcpy(p->buf, temp, sizeof(p->buf));

			p->hSend = (HANDLE)_beginthreadex(NULL, 0, ThreadSend, &flagSend, 0, NULL); //开启一个转发线程,flagflagSend标记着要转发的客户端
			//这里也可能是导致CPU使用率上升的原因。

		}
	}
	return 0;
}