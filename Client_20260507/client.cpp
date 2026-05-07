#include "stdafx.h"

static int ReceiveCnt = 0;

void ReceiveFile(SOCKET InServerSocket, const char* InFileName)
{
	FILE* File = fopen(InFileName, "wb");
	if (File == NULL)
	{
		printf("파일을 생성할 수 없습니다.\n");
		return;
	}

	int64_t TotalExpectedSize;
	int Result = recv(InServerSocket, (char*)&TotalExpectedSize, sizeof(TotalExpectedSize), 0);

	if (Result <= 0)
	{
		printf("파일 크기 정보를 수신하지 못했습니다.\n");
		fclose(File);
		return;
	}

	char* ReceiveBuffer = (char*)malloc(RECEIVE_BUFFER_SIZE);

	long TotalReceivedBytes = 0;
	int BytesToRequest;
	int ActualReadBytes;

	printf("수신 시작: 총 %ld bytes\n", TotalExpectedSize);

	while (TotalReceivedBytes < TotalExpectedSize)
	{
		if (TotalExpectedSize - TotalReceivedBytes < RECEIVE_BUFFER_SIZE)
		{
			BytesToRequest = (int)(TotalExpectedSize - TotalReceivedBytes);
		}
		else
		{
			BytesToRequest = RECEIVE_BUFFER_SIZE;
		}

		ActualReadBytes = recv(InServerSocket, ReceiveBuffer, BytesToRequest, 0);
		printf("%d		", ReceiveCnt++);

		if (ActualReadBytes <= 0)
		{
			break;
		}

		fwrite(ReceiveBuffer, 1, ActualReadBytes, File);
		TotalReceivedBytes += ActualReadBytes;
	}

	if (TotalReceivedBytes == TotalExpectedSize)
	{
		printf("파일 수신 완료\n");
	}
	else
	{
		printf("파일 수신 불완전\n");
	}

	free(ReceiveBuffer);
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

	ReceiveFile(ServerSocket, "ReceivedFile.png");

	closesocket(ServerSocket);
	WSACleanup();
}