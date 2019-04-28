#include <WinSock2.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include "client_init.h"
#include "sever_config.h"
#include "thread_accept.h"

#pragma comment(lib,"ws2_32.lib")
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

	//关闭套接字------------
	ClientList *p;
	p = clientHead->next;
	while (p != NULL) {
			closeClient(p);
		p = p->next;
	}
	clientHead->next = NULL;
	//-----------------------
	closesocket(ServerSocket);
	WSACleanup();
	return 0;
}