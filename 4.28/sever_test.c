#include <WinSock2.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include "sever_star.h"
#pragma comment(lib,"ws2_32.lib")



int main() {
	
	clientHead = (ClientList*)malloc(sizeof(ClientList)); 
	clientHead->next = NULL;
	printf("192.168.229.1\n");
	StartServer(); //Æô¶¯·şÎñÆ÷

	return 0;
}
