#include "TcpConnect.h"
#include <unistd.h>
#include <netinet/tcp.h>

TcpConnect::TcpConnect() {
    m_localAddr = "127.0.0.1";
}   

TcpConnect::~TcpConnect() {

}

int TcpConnect::Init(const std::string& serverIp, int serverPort, int localPort) {
    m_localPort = localPort;
    m_serverPort = serverPort;
    m_serverAddr = serverIp;
    m_socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if(m_socketFd < 0) {
        cout<<"Create socket error!";
        return -1;
    }    
    int enable = 1;
    int ret = setsockopt(m_socketFd, IPPROTO_TCP, TCP_NODELAY, (void*)&enable, sizeof(enable));
    if(ret < 0) {
        cout<<"setsockopt IPPROTO_TCP error!";
        close(m_socketFd);
        return -1;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_aton(m_localAddr.c_str(), &addr.sin_addr);
    addr.sin_port = ::htons(localPort);  
    ret = bind(m_socketFd, (sockaddr*)&addr, sizeof addr);
    if(ret < 0) {
        cout<<"WebServer::Init bind failed. ret="<<ret<<endl;
        close(m_socketFd);
        return ret;
    }
    inet_aton(serverIp.c_str(), &addr.sin_addr);
    addr.sin_port = ::htons(serverPort);    
    ret = connect(m_socketFd, (sockaddr*)&addr, sizeof addr);
    if(ret != 0) {
        std::cout<<"connect failed. ret = "<<ret<<endl;
        close(m_socketFd);
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
        close(m_socketFd);
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
        // epoll注册
        epoll_event event;
        event.data.fd = m_socketFd;
        event.events = EPOLLIN | EPOLLET;
        ret = epoll_ctl(m_epollFd, EPOLL_CTL_DEL, m_socketFd, &event);
        if(ret < 0) {
            cout<<"Client:epoll_ctl failed. ret="<<ret<<endl;
            close(m_socketFd);
            return ret;
        }    
    }
    return ret;
}