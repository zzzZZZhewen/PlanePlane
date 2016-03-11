#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <string>
#pragma comment(lib, "WS2_32.lib")
#include <list>

#define HOSTNAME "127.0.0.1"
#define DEFAULT_PORT "27123"
#define DEFAULT_BUFLEN  1024

bool UDP_Host_Init();
bool UDP_Host_Shutdown();
bool UDP_Host_Recv();
bool UDP_Host_Send(std::string);

bool UDP_Client_Init();
bool UDP_Client_Shutdown();
bool UDP_Client_Recv();
bool UDP_Client_Send(std::string);


extern char recvbuf[DEFAULT_BUFLEN];
extern int recvbuflen;
extern int recvlen;
extern sockaddr_in remoteAddr;
extern int nAddrLen;