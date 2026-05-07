#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <WinSock2.h>

constexpr int SERVERPORT = 9000;
constexpr int RECEIVE_BUFFER_SIZE = 1024 * 64;