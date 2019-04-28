#pragma once
#include <WinSock2.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include "sever_config.h"
#pragma comment(lib,"ws2_32.lib")

//发送数据线程
unsigned __stdcall ThreadSend(void* param) {
	int flagSend = *(int*)param;
	int ret = 0;
	char tempBuf[128] = { 0 };                         //创建一个临时的数据缓冲区，用来存放接收到的数据
	char tempUserName[16] = { 0 };
	ClientList *p;
	p = clientHead->next;
	while (p != NULL) {//判断是哪个客户端发送消息
		if (flagSend == p->flag){
			break;
		}
		p = p->next;
	}
	memcpy(tempBuf, p->buf, sizeof(tempBuf));
	memcpy(tempUserName, p->userName, sizeof(tempUserName));
	//这个要设置，否则会出现自己给自己发消息的BUG
	p = clientHead->next;
	while (p != NULL) {
		if (flagSend != p->flag) {//不要发给自己
			sprintf(p->buf, "%s: %s", tempUserName, tempBuf);//添加一个用户名头

			if (strlen(tempBuf) != 0 && iStatus == SEND_YET) //如果数据不为空且还没转发则转发
				ret = send(p->sClient, p->buf, sizeof(p->buf), 0);
			if (ret == SOCKET_ERROR)
				return 1;
		}
		p = p->next;
	}

	iStatus = SEND_OVER;   //全部用户转发成功后设置状态为已转发
	return 0;
}