#include "stdafx.h"

void ReceiveFile(SOCKET InServerSocket, const char* InFileName)
{
	// 1. 파일 생성 (바이너리 쓰기 모드)
	FILE* File = fopen(InFileName, "wb");
	if (File == NULL)
	{
		printf("파일을 생성할 수 없습니다.\n");
		return;
	}

	// 2. 전체 파일 크기 수신 (Header)
	long TotalExpectedSize;
	int Result = recv(InServerSocket, (char*)&TotalExpectedSize, sizeof(TotalExpectedSize), 0);

	if (Result <= 0)
	{
		printf("파일 크기 정보를 수신하지 못했습니다.\n");
		fclose(File);
		return;
	}

	// 3. 수신용 대형 버퍼 할당
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

	int RecvSize;
	int SendSize;

	char Buffer[1024] = "Hello";
	SendSize = send(ServerSocket, Buffer, sizeof(Buffer), 0);
	if (SendSize == 0)
	{
		std::cout << "Client Disconnected" << std::endl;
		exit(-1);
	}
	else if (SendSize < 0)
	{
		std::cout << "Send Error " << WSAGetLastError() << std::endl;
		exit(-1);
	}

	/*RecvSize = recv(ServerSocket, Buffer, sizeof(Buffer), 0);
	if (RecvSize == 0)
	{
		std::cout << "Client Disconnected" << std::endl;
		exit(-1);
	}
	else if (RecvSize < 0)
	{
		std::cout << "Receive Error " << WSAGetLastError() << std::endl;
		exit(-1);
	}
	std::cout << "Received Data : " << Buffer << std::endl;*/\
	
	ReceiveFile(ServerSocket, "ReceivedFile.png");

	closesocket(ServerSocket);
	WSACleanup();
}