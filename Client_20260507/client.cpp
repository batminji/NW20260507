#include "stdafx.h"

static int ReceiveCnt = 0;

void ReceiveFile(SOCKET InServerSocket, const char* InFileName)
{
	FILE* File = fopen(InFileName, "rb");

	char Buffer[RECEIVE_BUFFER_SIZE] = { 0, };
	size_t RecvBytes = 0;
	do
	{
		RecvBytes = fread(Buffer, sizeof(char), RECEIVE_BUFFER_SIZE, File);
		int Result = send(InServerSocket, Buffer, (int)RecvBytes, 0);

		if (Result <= 0)
		{
			break;
		}
	} while (RecvBytes > 0);
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