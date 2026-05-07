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

	while (true)
	{
		ClientAddrSize = sizeof(ClientAddr);
		ClientSocket = accept(ServerSocket, (SOCKADDR*)&ClientAddr, &ClientAddrSize);

		if (ClientSocket == INVALID_SOCKET)
		{
			std::cout << "Accept Error " << WSAGetLastError() << std::endl;
			exit(-1);
		}

		SendFile(ClientSocket, "carnation.png");

		closesocket(ClientSocket);
	}

	closesocket(ServerSocket);
	WSACleanup();
}