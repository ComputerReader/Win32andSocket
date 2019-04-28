#include <WinSock2.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include "client_init.h"
#include "sever_config.h"
#include "thread_accept.h"

#pragma comment(lib,"ws2_32.lib")
//����������
int StartServer() {
	//����׽�����Ϣ�Ľṹ
	WSADATA wsaData = { 0 };
	SOCKADDR_IN ServerAddr = { 0 };             //����˵�ַ
	USHORT uPort = 18000;                       //�����������˿�

	//��ʼ���׽���
	if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		printf("WSAStartup failed with error code: %d\n", WSAGetLastError());
		return -1;
	}
	//�жϰ汾
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		printf("wVersion was not 2.2\n");
		return -1;
	}
	//�����׽���
	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ServerSocket == INVALID_SOCKET) {
		printf("socket failed with error code: %d\n", WSAGetLastError());
		return -1;
	}

	//���÷�������ַ
	ServerAddr.sin_family = AF_INET;//���ӷ�ʽ
	ServerAddr.sin_port = htons(uPort);//�����������˿�
	ServerAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//�κοͻ��˶����������������

	//�󶨷�����
	if (SOCKET_ERROR == bind(ServerSocket, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr))) {
		printf("bind failed with error code: %d\n", WSAGetLastError());
		closesocket(ServerSocket);
		return -1;
	}
	//���ü����ͻ����������� max = 20000 ��
	if (SOCKET_ERROR == listen(ServerSocket, 20000)) {
		printf("listen failed with error code: %d\n", WSAGetLastError());
		closesocket(ServerSocket);
		WSACleanup();
		return -1;
	}

	_beginthreadex(NULL, 0, ThreadAccept, NULL, 0, 0);
	for (int k = 0; k < 100; k++) //�����߳����ߣ��������ر�TCP����.
		Sleep(10000000);

	//�ر��׽���------------
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