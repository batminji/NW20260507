#include "stdafx.h"

void SendFile(SOCKET InClientSocket, const char* InFileName)
{
	FILE* File = fopen(InFileName, "rb");
	if (File == NULL)
	{
		printf("파일을 찾을 수 없습니다.\n");
		return;
	}

	fseek(File, 0, SEEK_END);
	long TotalFileSize = ftell(File);
	fseek(File, 0, SEEK_SET);

	send(InClientSocket, (char*)&TotalFileSize, sizeof(TotalFileSize), 0);

	char* SendBuffer = (char*)malloc(SEND_BUFFER_SIZE);

	long TotalSentBytes = 0;
	int BytesToRead;

	while (TotalSentBytes < TotalFileSize)
	{
		if (TotalFileSize - TotalSentBytes < SEND_BUFFER_SIZE)
		{
			BytesToRead = (int)(TotalFileSize - TotalSentBytes);
		}
		else
		{
			BytesToRead = SEND_BUFFER_SIZE;
		}

		int ReadBytes = (int)fread(SendBuffer, 1, BytesToRead, File);

		if (ReadBytes > 0)
		{
			send(InClientSocket, SendBuffer, ReadBytes, 0);
			TotalSentBytes += ReadBytes;
		}
		else
		{
			break;
		}
	}

	printf("파일 전송 완료\n");

	free(SendBuffer);
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
	int RecvSize;
	int SendSize;

	char Buffer[1024] = { 0, };

	while (true)
	{
		ClientAddrSize = sizeof(ClientAddr);
		ClientSocket = accept(ServerSocket, (SOCKADDR*)&ClientAddr, &ClientAddrSize);

		if (ClientSocket == INVALID_SOCKET)
		{
			std::cout << "Accept Error " << WSAGetLastError() << std::endl;
			exit(-1);
		}

		memset(Buffer, 0, sizeof(Buffer));
		RecvSize = recv(ClientSocket, Buffer, sizeof(Buffer), 0);
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
		std::cout << "Received Data: " << Buffer << std::endl;

		/*SendSize = send(ClientSocket, Buffer, sizeof(Buffer), 0);
		if (SendSize == 0)
		{
			std::cout << "Client Disconnected" << std::endl;
			exit(-1);
		}
		else if (SendSize < 0)
		{
			std::cout << "Send Error " << WSAGetLastError() << std::endl;
			exit(-1);
		}*/

		SendFile(ClientSocket, "carnation.png");

		closesocket(ClientSocket);
	}

	closesocket(ServerSocket);
	WSACleanup();
}