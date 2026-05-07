#include "stdafx.h"

static int ReceiveCnt = 0;

void ReceiveFile(SOCKET InServerSocket, const char* InFileName)
{
	FILE* File = fopen(InFileName, "rb");

	char Buffer[RECEIVE_BUFFER_SIZE] = { 0, };
	size_t WriteSize = 0;
	do
	{
		int RecvBytes = recv(InServerSocket, Buffer, sizeof(Buffer), 0);
		if (RecvBytes <= 0)
		{
			break;
		}

		WriteSize = fwrite(Buffer, sizeof(char), RecvBytes, File);
	} while (WriteSize > 0);

	fclose(File);
}

int main()
{
	std::srand(std::time(NULL));

	int Result;

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return 1;
	}

	SOCKET ServerSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (ServerSocket == INVALID_SOCKET)
	{
		std::cout << "Socket Error " << WSAGetLastError() << std::endl;
		exit(-1);
	}

	SOCKADDR_IN ServerSockAddr;
	memset(&ServerSockAddr, 0, sizeof(ServerSockAddr));
	ServerSockAddr.sin_family = AF_INET;
	ServerSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	ServerSockAddr.sin_port = htons(SERVERPORT);

	Result = connect(ServerSocket, (SOCKADDR*)&ServerSockAddr, sizeof(ServerSockAddr));
	if (Result == SOCKET_ERROR)
	{
		std::cout << "Connect Error " << WSAGetLastError() << std::endl;
		exit(-1);
	}

	char SendBuffer[5] = { 0, };

	int LeftNum = rand() % 90 + 10;
	int RightNum = rand() % 90 + 10;

	SendBuffer[0] = (LeftNum / 10) + '0';
	SendBuffer[1] = (LeftNum % 10) + '0';

	SendBuffer[2] = '+';

	SendBuffer[3] = (RightNum / 10) + '0';
	SendBuffer[4] = (RightNum % 10) + '0';

	send(ServerSocket, SendBuffer, 5, 0);

	closesocket(ServerSocket);
	WSACleanup();
}