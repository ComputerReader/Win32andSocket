#pragma once
#include <WinSock2.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include "sever_config.h"
#pragma comment(lib,"ws2_32.lib")
//当聊天室至少用户人数都连接上服务器后才进行消息转发
int isMsgEnable(Client* client) {
	for (int i = 0; i < currentChatNum; i++) {
		if (client[i].flag == 0) {
			printf("error:当聊天室用户人数:%d\n", i);
			return 0;
		}
	}
	printf("success:当聊天室用户人数:%d\n", currentChatNum);
	return 1;
}