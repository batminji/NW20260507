#include "stdafx.h"

void ReceiveFile(SOCKET InServerSocket, const char* InFileName)
{
	FILE* File = fopen(InFileName, "wb");
	if (File == NULL) 
	{
		printf("파일을 생성할 수 없습니다.\n");
		return;
	}

	long TotalSize;
	int Result = recv(InServerSocket, (char*)&TotalSize, sizeof(TotalSize), 0);

	if (Result <= 0) 
	{
		printf("File Total Size Receive Error\n");
		fclose(File);
		return;
	}

	char* ReceiveBuffer = (char*)malloc(RECEIVE_BUFFER_SIZE);

	long TotalReceivedBytes = 0;
	int ReadBytes;
	int BytesToRequest;

	printf("수신 시작\n");

	while (TotalReceivedBytes < TotalSize) 
	{
		if (TotalSize - TotalReceivedBytes < RECEIVE_BUFFER_SIZE)
		{
			BytesToRequest = (int)(TotalSize - TotalReceivedBytes);
		}
		else
		{
			BytesToRequest = RECEIVE_BUFFER_SIZE;
		}

		ReadBytes = recv(InServerSocket, ReceiveBuffer, BytesToRequest, 0);

		if (ReadBytes <= 0) 
		{
			printf("Receive Error\n");
			break;
		}

		fwrite(ReceiveBuffer, 1, ReadBytes, File);
		TotalReceivedBytes += ReadBytes;
	}

	if (TotalReceivedBytes == TotalSize) 
	{
		std::cout << "파일 수신 완료: " << InFileName << std::endl;
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