#include "TcpConnect.h"

TcpConnect::TcpConnect() {

}   

TcpConnect::~TcpConnect() {

}

int TcpConnect::Init(const std::string& serverIp, int serverPort, int localPort) {
    m_localPort = localPort;
    m_serverPort = serverPort;
    m_serverAddr = serverIp;
    m_socketFd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_aton(serverIp.c_str(), &addr.sin_addr);
    addr.sin_port = ::htons(localPort);  
    int ret = bind(m_socketFd, (sockaddr*)&addr, sizeof addr);
    if(ret < 0) {
        cout<<"WebServer::Init bind failed. ret="<<ret<<endl;
        return ret;
    }
    addr.sin_port = ::htons(serverPort);    
    ret = connect(m_socketFd, (sockaddr*)&addr, sizeof addr);
    if(ret != 0) {
        std::cout<<"connect failed. ret = "<<ret<<endl;
        return ret;
    }

    m_epollFd = epoll_create(1);
    // epoll注册
    epoll_event event;
    event.data.fd = m_socketFd;
    event.events = EPOLLIN | EPOLLET;
    ret = epoll_ctl(m_epollFd, EPOLL_CTL_ADD, m_socketFd, &event);
    if(ret < 0) {
        cout<<"Client:epoll_ctl failed. ret="<<ret<<endl;
        return ret;
    }    
    return ret;
}

int TcpConnect::ReceiveMsg(std::function<void (const std::string&)>& func) {
    epoll_event events[1024];
    int nums = epoll_wait(m_epollFd, events, 1, -1);
    for(int i = 0; i < nums; i++) {
        if(events[i].events != EPOLLIN) {
            continue;
        }
        char buffer[1024];
        memset(buffer, 0, 1024);
        int ret = recv(events[i].data.fd, buffer, 1024, MSG_NOSIGNAL);
        if(ret < 0) {
            cout<< "Client::recv failed, ret="<<ret<<endl;
        }        
        func(buffer);        
    }
    return nums;    
}

int TcpConnect::SendMsg(const std::string& msg) {
    int ret = send(m_socketFd, msg.c_str(), msg.length(), MSG_NOSIGNAL);
    if(ret < 0) {
        cout<<"Client::send failed, ret="<<ret<<endl;
    }
    return ret;
}