#ifndef SOCKETTRANSFER_H
#define SOCKETTRANSFER_H
#define BUFFER_LEN 4096

#include <string>

#if defined _WIN32 || _WIN64
#include <WinSock2.h>
#define socklen_t int
#else
#define SOCKET int
#endif

class SocketTransfer
{
public:
    SocketTransfer(SOCKET sock);
    ~SocketTransfer();

    void send(const char *line);
    void send(std::string line);
    void receive(char *line);
    std::string receive();
    void check(const char *line);

    static void init();
    static void cleanup();

private:
    SOCKET sock;
    char buffer[BUFFER_LEN];
    int pos, len;
};

#endif
