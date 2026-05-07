#include "stdafx.h"

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
	ServerSockAddr.sin_addr.s_addr = inet_addr("192.168.0.95");
	ServerSockAddr.sin_port = htons(SERVERPORT);

	Result = connect(ServerSocket, (SOCKADDR*)&ServerSockAddr, sizeof(ServerSockAddr));
	if (Result == SOCKET_ERROR)
	{
		std::cout << "Connect Error " << WSAGetLastError() << std::endl;
		exit(-1);
	}

	int RecvSize;
	int SendSize;

	char Buffer[1024] = "";
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

	RecvSize = recv(ServerSocket, Buffer, sizeof(Buffer), 0);
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
	std::cout << "Received Data : " << Buffer << std::endl;

	closesocket(ServerSocket);
	WSACleanup();
}