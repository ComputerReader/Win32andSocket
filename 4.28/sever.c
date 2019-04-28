/*
#include <WinSock2.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include "client_init.h"
#pragma comment(lib,"ws2_32.lib")
#define maxChatNum 2						//������������������
#define minChatNum 2						//������������������
#define currentChatNum 3                     //��ǰ������������
#define SEND_OVER 1                          //�Ѿ�ת����Ϣ
#define SEND_YET  0                          //��ûת����Ϣ
int iStatus = SEND_YET;
SOCKET ServerSocket = INVALID_SOCKET;      //������׽���
SOCKADDR_IN ClientAddr = { 0 };            //�ͻ��˵�ַ
int iClientAddrLen = sizeof(ClientAddr);
Client client[currentChatNum] = { 0 };                  //����һ���ͻ��˽ṹ��


//�������������û������������Ϸ�������Ž�����Ϣת��
int isMsgEnable(Client* g_Client) {
	for (int i = 0; i < currentChatNum; i++) {
		if (g_Client[i].flag == 0) {
			printf("error:���������û�����:%d\n", i);
			return 0;
		}
	}
	printf("success:���������û�����:%d\n", currentChatNum);
	return 1;
}

//���������߳�
unsigned __stdcall ThreadSend(void* param) {
	int flagSend = *(int*)param;
	int ret = 0;
	char temp[128] = { 0 };                         //����һ����ʱ�����ݻ�������������Ž��յ�������
	memcpy(temp, client[flagSend].buf, sizeof(temp));

	//���Ҫ���ã����������Լ����Լ�����Ϣ��BUG
	for (int flagRecv = 0; flagRecv < currentChatNum; flagRecv++) {

		if (flagRecv == flagSend)//��Ҫ�����Լ�
			continue;

		sprintf(client[flagRecv].buf, "%s: %s", client[flagSend].userName, temp);//���һ���û���ͷ

		if (strlen(temp) != 0 && iStatus == SEND_YET) //������ݲ�Ϊ���һ�ûת����ת��
			ret = send(client[flagRecv].sClient, client[flagRecv].buf, sizeof(client[flagRecv].buf), 0);
		if (ret == SOCKET_ERROR)
			return 1;
	}
	iStatus = SEND_OVER;   //ȫ���û�ת���ɹ�������״̬Ϊ��ת��
	return 0;
}

//��������
unsigned __stdcall ThreadRecv(void* param) {
	SOCKET tempClient = INVALID_SOCKET;
	char temp[128] = { 0 };  //��ʱ���ݻ�����
	int flagSend = 0;

	//�ж����ĸ��ͻ��˷�������Ϣ
	for (; flagSend < currentChatNum; flagSend++) {
		if (*(int*)param == client[flagSend].flag) {
			tempClient = client[flagSend].sClient;
			break;
		}
	}
	while (1) {
		//���Ͻ���ת����Ϣ
		memset(temp, 0, sizeof(temp));
		int ret = recv(tempClient, temp, sizeof(temp), 0); //��������
		if (ret != SOCKET_ERROR) {
			iStatus = SEND_YET;//����ת��״̬Ϊδת��
			memcpy(client[flagSend].buf, temp, sizeof(client[flagSend].buf));

			client[flagSend].hSend = _beginthreadex(NULL, 0, ThreadSend, &flagSend, 0, NULL); //����һ��ת���߳�,flagflagSend�����Ҫת���Ŀͻ���
			//����Ҳ�����ǵ���CPUʹ����������ԭ��

		}
	}

	return 0;
}

//��������
unsigned __stdcall ThreadManager(void* param) {
	while (1) {
		for (int index = 0; index < currentChatNum; index++) {
			if (send(client[index].sClient, "", sizeof(""), 0) == SOCKET_ERROR) {
				if (client[index].sClient != 0) {
					
					//��Client������ɾ��
					
					for (int i = 0; i < currentChatNum; i++) {//����ر����߳̾�������ǲ��Խ���Ͽ���C/S�Ӻ�CPU��Ȼ����
						if (client[i].hRecv)
							CloseHandle(client[i].hRecv);
						if (client[i].hSend)
							CloseHandle(client[i].hSend);
					}
					printf("Disconnect from IP: %s,UserName: %s\n", client[index].IP, client[index].userName);
					closeClient(&client[index]);   //����򵥵��жϣ���������Ϣʧ�ܣ�����Ϊ�����ж�(��ԭ���ж���)���رո��׽���
				}
			}
		}
		Sleep(2000); //2s���һ��
	}
	return 0;
}

//��������
unsigned __stdcall ThreadAccept(void* param) {

	int temp[currentChatNum] = { 0 };
	_beginthreadex(NULL, 0, ThreadManager, NULL, 0, NULL);

	while (1) {
		for (int i = 0; i < currentChatNum; i++) {
			if (client[i].flag != 0) {
				continue;
			}
			//����пͻ����������Ӿͽ�������
			if ((client[i].sClient = accept(ServerSocket, (SOCKADDR*)&ClientAddr, &iClientAddrLen)) == INVALID_SOCKET) {
				printf("accept failed with error code: %d\n", WSAGetLastError());
				closesocket(ServerSocket);
				WSACleanup();
				return -1;
			}
			//�����û���
			recv(client[i].sClient, client[i].userName, sizeof(client[i].userName), 0);
			printf("Successfuuly got a connection from IP:%s ,Port: %d,UerName: %s\n",
				inet_ntoa(ClientAddr.sin_addr), htons(ClientAddr.sin_port), client[i].userName);
			//��¼�ͻ���IP
			memcpy(client[i].IP, inet_ntoa(ClientAddr.sin_addr), sizeof(client[i].IP));
			client[i].flag = client[i].sClient; //��ͬ��socket�в�ͬUINT_PTR���͵���������ʶ
		}

		//����--------------------------------------------------------------------------------------
		//�����ӽ���
		if (isMsgEnable(client)) {	//�������������û������������Ϸ�������Ž�����Ϣת��
			for (int i = 0; i < currentChatNum; i++) {
				if (client[i].flag != temp[i]) {    //ÿ�ζϿ�һ�����Ӻ��ٴ����ϻ��¿�һ���̣߳�����cpuʹ��������,����Ҫ�ص��ɵ�
					if (client[i].hRecv) {//����ر����߳̾�������ǲ��Խ���Ͽ���C/S�Ӻ�CPU��Ȼ����
						CloseHandle(client[i].hRecv);
					}
					//����currentChatNum��������Ϣ���߳�
					client[i].hRecv = (HANDLE)_beginthreadex(NULL, 0, ThreadRecv, &client[i].flag, 0, NULL);
				}
			}

		}
		//��ֹThreadRecv�̶߳�ο���
		for (int i = 0; i < currentChatNum; i++) {
			temp[i] = client[i].flag;
		}

		Sleep(3000);
	}

	return 0;
}

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

	//�ر��׽���
	for (int j = 0; j < currentChatNum; j++) {
		if (client[j].sClient != INVALID_SOCKET)
			closeClient(&client[j]);
	}
	closesocket(ServerSocket);
	WSACleanup();
	return 0;
}

int main() {
	StartServer(); //����������

	return 0;
}

*/