#include <cstring>
#include <cstdio>

#if defined _WIN32 || _WIN64
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#endif

#include "SocketTransfer.h"

SocketTransfer::SocketTransfer(SOCKET sock)
{
    this->sock = sock;
    this->pos = 0;
    this->len = 0;
}

SocketTransfer::~SocketTransfer()
{
#if defined _WIN32 || _WIN64
    closesocket(this->sock);
#else
    close(this->sock);
#endif
}

void SocketTransfer::send(const char *line)
{
    int l = strlen(line);
#if defined _WIN32 || _WIN64
    ::send(sock, line, l, 0);
    if (line[l - 1] != '\n') // not terminated by enter
        ::send(sock, "\n", 1, 0);
#else
    write(sock, line, l);
    if (line[l - 1] != '\n') // not terminated by enter
        write(sock, "\n", 1);
#endif
//    printf("SEND: %s\n", line);
}

void SocketTransfer::send(std::string line)
{
	send(line.c_str());
}

void SocketTransfer::receive(char *line)
{
    for (int i = 0;; i++)
    {
        while (pos == len || buffer[pos] == 0)
        {
            len = recv(sock, buffer, BUFFER_LEN, 0);
            pos = 0;
        }
        if (buffer[pos] == '\n')
        {
            pos++;
            line[i] = 0;
            break;
        }
        else
            line[i] = buffer[pos++];
    }
//    printf("RECV: %s\n", line);
}

std::string SocketTransfer::receive()
{
	char buffer[BUFFER_LEN];
	receive(buffer);
	return std::string(buffer);
}

void SocketTransfer::check(const char *line)
{
    char buffer[BUFFER_LEN];
    receive(buffer);
    if (strcmp(buffer, line))
        ; // TODO
}

void SocketTransfer::init()
{
#if defined _WIN32 || _WIN64
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0)
    {
        printf("WSAStartup failed with error: %d\n", err);
        exit(1);
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        printf("Could not find a usable version of Winsock.dll\n");
        WSACleanup();
        exit(1);
    }
#endif
}

void SocketTransfer::cleanup()
{
#if defined _WIN32 || _WIN64
    WSACleanup();
#endif
}

