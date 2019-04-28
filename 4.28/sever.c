/*
#include <WinSock2.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include "client_init.h"
#pragma comment(lib,"ws2_32.lib")
#define maxChatNum 2						//设置聊天室至多人数
#define minChatNum 2						//设置聊天室至少人数
#define currentChatNum 3                     //当前聊天人数限制
#define SEND_OVER 1                          //已经转发消息
#define SEND_YET  0                          //还没转发消息
int iStatus = SEND_YET;
SOCKET ServerSocket = INVALID_SOCKET;      //服务端套接字
SOCKADDR_IN ClientAddr = { 0 };            //客户端地址
int iClientAddrLen = sizeof(ClientAddr);
Client client[currentChatNum] = { 0 };                  //创建一个客户端结构体


//当聊天室至少用户人数都连接上服务器后才进行消息转发
int isMsgEnable(Client* g_Client) {
	for (int i = 0; i < currentChatNum; i++) {
		if (g_Client[i].flag == 0) {
			printf("error:当聊天室用户人数:%d\n", i);
			return 0;
		}
	}
	printf("success:当聊天室用户人数:%d\n", currentChatNum);
	return 1;
}

//发送数据线程
unsigned __stdcall ThreadSend(void* param) {
	int flagSend = *(int*)param;
	int ret = 0;
	char temp[128] = { 0 };                         //创建一个临时的数据缓冲区，用来存放接收到的数据
	memcpy(temp, client[flagSend].buf, sizeof(temp));

	//这个要设置，否则会出现自己给自己发消息的BUG
	for (int flagRecv = 0; flagRecv < currentChatNum; flagRecv++) {

		if (flagRecv == flagSend)//不要发给自己
			continue;

		sprintf(client[flagRecv].buf, "%s: %s", client[flagSend].userName, temp);//添加一个用户名头

		if (strlen(temp) != 0 && iStatus == SEND_YET) //如果数据不为空且还没转发则转发
			ret = send(client[flagRecv].sClient, client[flagRecv].buf, sizeof(client[flagRecv].buf), 0);
		if (ret == SOCKET_ERROR)
			return 1;
	}
	iStatus = SEND_OVER;   //全部用户转发成功后设置状态为已转发
	return 0;
}

//接受数据
unsigned __stdcall ThreadRecv(void* param) {
	SOCKET tempClient = INVALID_SOCKET;
	char temp[128] = { 0 };  //临时数据缓冲区
	int flagSend = 0;

	//判断是哪个客户端发来的消息
	for (; flagSend < currentChatNum; flagSend++) {
		if (*(int*)param == client[flagSend].flag) {
			tempClient = client[flagSend].sClient;
			break;
		}
	}
	while (1) {
		//不断接受转发消息
		memset(temp, 0, sizeof(temp));
		int ret = recv(tempClient, temp, sizeof(temp), 0); //接收数据
		if (ret != SOCKET_ERROR) {
			iStatus = SEND_YET;//设置转发状态为未转发
			memcpy(client[flagSend].buf, temp, sizeof(client[flagSend].buf));

			client[flagSend].hSend = _beginthreadex(NULL, 0, ThreadSend, &flagSend, 0, NULL); //开启一个转发线程,flagflagSend标记着要转发的客户端
			//这里也可能是导致CPU使用率上升的原因。

		}
	}

	return 0;
}

//管理连接
unsigned __stdcall ThreadManager(void* param) {
	while (1) {
		for (int index = 0; index < currentChatNum; index++) {
			if (send(client[index].sClient, "", sizeof(""), 0) == SOCKET_ERROR) {
				if (client[index].sClient != 0) {
					
					//从Client链表中删除
					
					for (int i = 0; i < currentChatNum; i++) {//这里关闭了线程句柄，但是测试结果断开连C/S接后CPU仍然疯涨
						if (client[i].hRecv)
							CloseHandle(client[i].hRecv);
						if (client[i].hSend)
							CloseHandle(client[i].hSend);
					}
					printf("Disconnect from IP: %s,UserName: %s\n", client[index].IP, client[index].userName);
					closeClient(&client[index]);   //这里简单的判断：若发送消息失败，则认为连接中断(其原因有多种)，关闭该套接字
				}
			}
		}
		Sleep(2000); //2s检查一次
	}
	return 0;
}

//接受请求
unsigned __stdcall ThreadAccept(void* param) {

	int temp[currentChatNum] = { 0 };
	_beginthreadex(NULL, 0, ThreadManager, NULL, 0, NULL);

	while (1) {
		for (int i = 0; i < currentChatNum; i++) {
			if (client[i].flag != 0) {
				continue;
			}
			//如果有客户端申请连接就接受连接
			if ((client[i].sClient = accept(ServerSocket, (SOCKADDR*)&ClientAddr, &iClientAddrLen)) == INVALID_SOCKET) {
				printf("accept failed with error code: %d\n", WSAGetLastError());
				closesocket(ServerSocket);
				WSACleanup();
				return -1;
			}
			//接收用户名
			recv(client[i].sClient, client[i].userName, sizeof(client[i].userName), 0);
			printf("Successfuuly got a connection from IP:%s ,Port: %d,UerName: %s\n",
				inet_ntoa(ClientAddr.sin_addr), htons(ClientAddr.sin_port), client[i].userName);
			//记录客户端IP
			memcpy(client[i].IP, inet_ntoa(ClientAddr.sin_addr), sizeof(client[i].IP));
			client[i].flag = client[i].sClient; //不同的socket有不同UINT_PTR类型的数字来标识
		}

		//链表--------------------------------------------------------------------------------------
		//创建子进程
		if (isMsgEnable(client)) {	//当聊天室至少用户人数都连接上服务器后才进行消息转发
			for (int i = 0; i < currentChatNum; i++) {
				if (client[i].flag != temp[i]) {    //每次断开一个连接后再次连上会新开一个线程，导致cpu使用率上升,所以要关掉旧的
					if (client[i].hRecv) {//这里关闭了线程句柄，但是测试结果断开连C/S接后CPU仍然疯涨
						CloseHandle(client[i].hRecv);
					}
					//开启currentChatNum个接收消息的线程
					client[i].hRecv = (HANDLE)_beginthreadex(NULL, 0, ThreadRecv, &client[i].flag, 0, NULL);
				}
			}

		}
		//防止ThreadRecv线程多次开启
		for (int i = 0; i < currentChatNum; i++) {
			temp[i] = client[i].flag;
		}

		Sleep(3000);
	}

	return 0;
}

//启动服务器
int StartServer() {
	//存放套接字信息的结构
	WSADATA wsaData = { 0 };
	SOCKADDR_IN ServerAddr = { 0 };             //服务端地址
	USHORT uPort = 18000;                       //服务器监听端口

	//初始化套接字
	if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		printf("WSAStartup failed with error code: %d\n", WSAGetLastError());
		return -1;
	}
	//判断版本
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		printf("wVersion was not 2.2\n");
		return -1;
	}
	//创建套接字
	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ServerSocket == INVALID_SOCKET) {
		printf("socket failed with error code: %d\n", WSAGetLastError());
		return -1;
	}

	//设置服务器地址
	ServerAddr.sin_family = AF_INET;//连接方式
	ServerAddr.sin_port = htons(uPort);//服务器监听端口
	ServerAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//任何客户端都能连接这个服务器

	//绑定服务器
	if (SOCKET_ERROR == bind(ServerSocket, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr))) {
		printf("bind failed with error code: %d\n", WSAGetLastError());
		closesocket(ServerSocket);
		return -1;
	}
	//设置监听客户端连接数（ max = 20000 ）
	if (SOCKET_ERROR == listen(ServerSocket, 20000)) {
		printf("listen failed with error code: %d\n", WSAGetLastError());
		closesocket(ServerSocket);
		WSACleanup();
		return -1;
	}

	_beginthreadex(NULL, 0, ThreadAccept, NULL, 0, 0);
	for (int k = 0; k < 100; k++) //让主线程休眠，不让它关闭TCP连接.
		Sleep(10000000);

	//关闭套接字
	for (int j = 0; j < currentChatNum; j++) {
		if (client[j].sClient != INVALID_SOCKET)
			closeClient(&client[j]);
	}
	closesocket(ServerSocket);
	WSACleanup();
	return 0;
}

int main() {
	StartServer(); //启动服务器

	return 0;
}

*/