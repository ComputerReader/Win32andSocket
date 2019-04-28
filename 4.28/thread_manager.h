#pragma once
#include <WinSock2.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include "client_init.h"
#include "sever_config.h"
#pragma comment(lib,"ws2_32.lib")
//管理连接
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
					closeClient(p->next);   //这里简单的判断：若发送消息失败，则认为连接中断(其原因有多种)，关闭该套接字
					//从Client链表中删除
					p->next = p->next->next;
					//这里关闭了线程句柄，但是测试结果断开连C/S接后CPU仍然疯涨
				}
			}
			p = p->next;
		}
		Sleep(2000); //2s检查一次
	}
	return 0;
}