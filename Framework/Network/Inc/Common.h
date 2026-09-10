#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _WINSOCK_DEPRECATED_NO_WARNINGS

//windows network lib
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

//shared lib
#include <Core/Inc/Core.h>
#include <Math/Inc/VMath.h>
#include <Graphics/Inc/Graphics.h>

#define LATENCY_CHECK 5
#define LATENCT_DEV 150
#define WM_SOCKET (WM_USER + 1)
#define DEFAULT_PORT 8000
#define RECIEVE_BUFFER_SIZE 65535 //max 16 value


