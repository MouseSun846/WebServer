#include "bits/stdc++.h"
#include <chrono>
#include <netinet/in.h>
#include <string>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
using namespace std;

class TcpConnect{
public:
    TcpConnect();    
    ~TcpConnect();
    int Init(const std::string& serverIp, int serverPort, int localPort);
    int ReceiveMsg(std::function<void (const std::string&)>& func);
    int SendMsg(const std::string& msg);

private:
    int m_socketFd;
    int m_epollFd;
    int m_localPort;
    int m_serverPort;
    std::string m_serverAddr;
    std::string m_localAddr;

};