#pragma once
#include <WinSock2.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include "sever_config.h"
#pragma comment(lib,"ws2_32.lib")
//�������������û������������Ϸ�������Ž�����Ϣת��
int isMsgEnable(Client* client) {
	for (int i = 0; i < currentChatNum; i++) {
		if (client[i].flag == 0) {
			printf("error:���������û�����:%d\n", i);
			return 0;
		}
	}
	printf("success:���������û�����:%d\n", currentChatNum);
	return 1;
}