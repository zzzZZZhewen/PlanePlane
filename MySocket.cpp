#include "MySocket.h"

WSADATA wsaData;
SOCKET ListenSocket = INVALID_SOCKET;
SOCKET SendSocket = INVALID_SOCKET;
addrinfo *result = NULL;

char recvbuf[DEFAULT_BUFLEN];
int recvbuflen = DEFAULT_BUFLEN;
int recvlen = 0;
sockaddr_in remoteAddr;
int nAddrLen = sizeof(remoteAddr);

bool UDP_Host_Init()
{		
		/// Initializing Winsock
		// Create a WSADATA object called wsaData.
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
				return false;

		/// Creating a Socket for the Server
		//1 The getaddrinfo function is used to determine the values in the sockaddr structure
		struct addrinfo *result = NULL, *ptr = NULL, hints;
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_protocol = IPPROTO_UDP;
		hints.ai_flags = AI_PASSIVE;
		// Resolve the local address and port to be used by the server
		if (getaddrinfo(NULL, DEFAULT_PORT, &hints, &result )!= 0)
		{
				WSACleanup();
				return false;
		}

		//3 Create a SOCKET for the server to listen for client connections
		ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		//4 Check for errors to ensure that the socket is a valid socket.
		if (ListenSocket == INVALID_SOCKET)
		{
				freeaddrinfo(result);
				WSACleanup();
				return false;
		}
		///Binding a Socket
		//Call the bind function, passing the created socket and sockaddr structure returned from the getaddrinfo function as parameters. Check for general errors.
		if (bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) 
		{
				freeaddrinfo(result);
				closesocket(ListenSocket);
				WSACleanup();
				return false;
		}

		ULONG nonblock = 1;
		/* set to non-blocking mode */
		if (ioctlsocket(ListenSocket, FIONBIO, &nonblock) == SOCKET_ERROR)
		{
				closesocket(ListenSocket);
				ListenSocket = INVALID_SOCKET;
				WSACleanup();
				return false;
		}

		return true;
}

bool UDP_Host_Shutdown()
{
		freeaddrinfo(result);
		if (closesocket(ListenSocket) == SOCKET_ERROR)
		{
				return false;
		}
		WSACleanup();
		return true;
}

bool UDP_Host_Recv()
{
		memset(recvbuf, 0, recvbuflen);
		if (recvlen = recvfrom(ListenSocket, recvbuf, recvbuflen, 0, (SOCKADDR *)&remoteAddr, &nAddrLen) > 0)
		{
				return true;
		}
		return false;
}
bool UDP_Host_Send(std::string sendbuf)
{
		int iResult = sendto(ListenSocket, sendbuf.c_str(), strlen(sendbuf.c_str()), 0, (SOCKADDR *) &remoteAddr, nAddrLen);
		if (iResult == SOCKET_ERROR)
		{
				return false;
		}
		return true;
}
bool UDP_Client_Init()
{
		/// Initializing Winsock
		//2 Call WSAStartup and return its value as an integer and check for errors.
		int iResult;
		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0)
				return false;
		/// Creating a Socket for the Server
		//1 The getaddrinfo function is used to determine the values in the sockaddr structure
		struct addrinfo  *ptr = NULL, hints;
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_protocol = IPPROTO_UDP;
		hints.ai_flags = AI_PASSIVE;
		// Resolve the local address and port to be used by the server
		iResult = getaddrinfo(HOSTNAME , DEFAULT_PORT, &hints, &result);
		if (iResult != 0)
		{
				WSACleanup();
				return false;
		}
		//2 Create a SOCKET object called ListenSocket for the server to listen for client connections.

		//3 Create a SOCKET for the server to listen for client connections
		SendSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		//4 Check for errors to ensure that the socket is a valid socket.
		if (SendSocket == INVALID_SOCKET)
		{
				WSACleanup();
				return false;
		}
		ULONG nonblock = 1;
		/* set to non-blocking mode */
		if (ioctlsocket(SendSocket, FIONBIO, &nonblock) == SOCKET_ERROR)
		{
				closesocket(SendSocket);
				SendSocket = INVALID_SOCKET;
				WSACleanup();
				return false;
		}

		return true;
}

bool UDP_Client_Shutdown()
{
		freeaddrinfo(result);
		if (closesocket(SendSocket) == SOCKET_ERROR)
		{
				return false;
		}
		WSACleanup();
		return true;
}

bool UDP_Client_Send(std::string sendbuf)
{
		
		int iResult = sendto(SendSocket, sendbuf.c_str(), strlen(sendbuf.c_str()), 0, result->ai_addr, result->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
				return false;
		}
		return true;
}
bool UDP_Client_Recv()
{
		memset(recvbuf, 0, recvbuflen);
		if (recvlen = recvfrom(SendSocket, recvbuf, recvbuflen, 0, (SOCKADDR *)&remoteAddr, &nAddrLen) > 0)
		{
				return true;
		}
		return false;
}
