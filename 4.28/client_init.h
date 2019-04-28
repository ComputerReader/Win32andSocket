#pragma once
#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include "sever_config.h"
#pragma comment(lib,"ws2_32.lib")


void closeClient(Client* client) {
	closesocket(client->sClient);
	memset(client->buf, 0, sizeof(client->buf));
	memset(client->userName, 0, sizeof(client->userName));
	memset(client->IP, 0, sizeof(client->IP));
	client->flag = 0;
}