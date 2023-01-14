#include "sys/socket.h"
#include "bits/stdc++.h"

class WebServer{
public:
    WebServer();
    ~WebServer();
    int Init(const std::string& ip, int port);
    int ReceiveMsg(std::function<void (const std::string&)>& func);
private:    
    int m_listenFd;
    int m_epollFd;
    std::string m_ipAddr;
    int m_port;
};