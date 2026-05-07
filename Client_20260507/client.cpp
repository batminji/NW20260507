#include "stdafx.h"

static int ReceiveCnt = 0;

char Operator[4] = { '+', '-', '*', '/' };

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

	while (true)
	{
		char SendBuffer[9] = { 0, };

		int LeftNum = rand() % 10000;
		int RightNum = rand() % 9999 + 1;
		int OperatorIndex = rand() % 4;

		SendBuffer[0] = (LeftNum / 1000) + '0';
		SendBuffer[1] = ((LeftNum / 100) % 10) + '0';
		SendBuffer[2] = ((LeftNum / 10) % 10) + '0';
		SendBuffer[3] = (LeftNum % 10) + '0';

		SendBuffer[4] = Operator[OperatorIndex];

		SendBuffer[5] = (RightNum / 1000) + '0';
		SendBuffer[6] = ((RightNum / 100) % 10) + '0';
		SendBuffer[7] = ((RightNum / 10) % 10) + '0';
		SendBuffer[8] = (RightNum % 10) + '0';

		int SendResult = send(ServerSocket, SendBuffer, 9, 0);
		if (SendResult <= 0)
		{
			printf("Send Error\n");
			exit(-1);
		}

		char RecvBuffer[9] = { 0, };
		int RecvResult = recv(ServerSocket, RecvBuffer, 9, 0);
		if (RecvResult <= 0)
		{
			printf("Recv Error\n");
			exit(-1);
		}

		int FinalResult = 0;
		bool bIsNegative = false;
		int Index = 0;

		if (RecvBuffer[0] == '-')
		{
			bIsNegative = true;
			Index = 1; 
		}

		for (int i = Index; i < 9; ++i)
		{
			if (RecvBuffer[i] >= '0' && RecvBuffer[i] <= '9')
			{
				FinalResult = FinalResult * 10 + (RecvBuffer[i] - '0');
			}
		}

		if (bIsNegative)
		{
			FinalResult *= -1;
		}

		printf("%d %c %d = %d\n", LeftNum, Operator[OperatorIndex], RightNum, FinalResult);
	}

	closesocket(ServerSocket);
	WSACleanup();
}