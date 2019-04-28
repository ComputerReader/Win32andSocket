#pragma once
#include <WinSock2.h>
#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
#pragma comment(lib,"ws2_32.lib")
//客户端信息结构体
typedef struct _Client {
	SOCKET sClient;      //客户端套接字
	char buf[128];       //数据缓冲区
	char userName[16];   //客户端用户名
	char IP[20];         //客户端IP
	UINT_PTR flag;       //标记客户端，用来区分不同的客户端
	HANDLE hRecv;      //接收消息的线程
	HANDLE hSend;		 //发送消息的线程
	struct _Client* next; //链表里下一个客户端
}Client,ClientList;

#define maxChatNum 2						//设置聊天室至多人数
#define minChatNum 2						//设置聊天室至少人数
#define currentChatNum 3                     //当前聊天人数限制
#define SEND_OVER 1                          //已经转发消息
#define SEND_YET  0                          //还没转发消息
int iStatus = SEND_YET;
SOCKET ServerSocket = INVALID_SOCKET;      //服务端套接字
SOCKADDR_IN ClientAddr = { 0 };            //客户端地址
int iClientAddrLen = sizeof(ClientAddr);

//Client client[currentChatNum] = { 0 };                  //创建一个客户端结构体
ClientList* clientHead; //创建一个客户端结构体链表头