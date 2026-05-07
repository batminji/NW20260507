#include "stdafx.h"

void SendFile(SOCKET InClientSocket, const char* InFileName)
{
	FILE* File = fopen(InFileName, "rb");

	char Buffer[SEND_BUFFER_SIZE] = { 0, };
	size_t SentBytes = 0;
	do
	{
		SentBytes = fread(Buffer, sizeof(char), SEND_BUFFER_SIZE, File);
		int Result = send(InClientSocket, Buffer, (int)SentBytes, 0);
		
		if (Result <= 0)
		{
			break;
		}
	} while (SentBytes > 0);

	fclose(File);
}

int main()
{
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

	SOCKADDR_IN ServerAddr;
	memset(&ServerAddr, 0, sizeof(ServerAddr));
	ServerAddr.sin_family = AF_INET;
	// ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	ServerAddr.sin_port = htons(SERVERPORT);

	Result = bind(ServerSocket, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr));
	if (Result == SOCKET_ERROR)
	{
		std::cout << "Bind Error " << WSAGetLastError() << std::endl;
		exit(-1);
	}

	Result = listen(ServerSocket, SOMAXCONN);
	if (Result == SOCKET_ERROR)
	{
		std::cout << "Listen Error " << WSAGetLastError() << std::endl;
		exit(-1);
	}

	SOCKET ClientSocket;
	SOCKADDR_IN ClientAddr;
	memset(&ClientAddr, 0, sizeof(ClientAddr));

	int ClientAddrSize;

	ClientAddrSize = sizeof(ClientAddr);
	ClientSocket = accept(ServerSocket, (SOCKADDR*)&ClientAddr, &ClientAddrSize);

	if (ClientSocket == INVALID_SOCKET)
	{
		std::cout << "Accept Error " << WSAGetLastError() << std::endl;
		exit(-1);
	}

	while (true)
	{
		char RecvBuffer[9] = { 0, };
		int RecvResult = recv(ClientSocket, RecvBuffer, 9, 0);
		if (RecvResult <= 0)
		{
			printf("Recv Error\n");
			exit(-1);
		}

		int LeftNum = (RecvBuffer[0] - '0') * 1000 + (RecvBuffer[1] - '0') * 100 + (RecvBuffer[2] - '0') * 10 + (RecvBuffer[3] - '0');
		int RightNum = (RecvBuffer[5] - '0') * 1000 + (RecvBuffer[6] - '0') * 100 + (RecvBuffer[7] - '0') * 10 + (RecvBuffer[8] - '0');
		char Operation = RecvBuffer[4];

		int Result = 0;
		switch (Operation)
		{
		case '+':
			Result = LeftNum + RightNum;
			break;
		case '-':
			Result = LeftNum - RightNum;
			break;
		case '*':
			Result = LeftNum * RightNum;
			break;
		case '/':
			Result = LeftNum / RightNum;
			break;
		default:
			break;
		}

		printf("Result : %d %c %d = %d\n", LeftNum, Operation, RightNum, Result);

		char SendBuffer[9] = { 0, };
		int TempResult = Result;
		bool bIsNegative = false;
		if (TempResult < 0)
		{
			bIsNegative = true;
			TempResult *= -1;
		}

		int Index = 8;
		while (Index >= 1 && TempResult > 0)
		{
			SendBuffer[Index] = (TempResult % 10) + '0';
			TempResult /= 10;
			Index--;
		}

		if (bIsNegative)
		{
			SendBuffer[0] = '-';
		}
		else
		{
			SendBuffer[0] = '0';
		}

		int SentResult = send(ClientSocket, SendBuffer, 9, 0);
		if (SentResult <= 0)
		{
			printf("Send Error\n");
			exit(-1);
		}

		// closesocket(ClientSocket);
	}

	closesocket(ServerSocket);
	WSACleanup();
}