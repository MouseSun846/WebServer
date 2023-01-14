#include "WebServer.h"
#include <cerrno>
#include <functional>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;;
WebServer::WebServer() {

}

WebServer::~WebServer() {
    close(m_listenFd);
    close(m_epollFd);
}

int WebServer::Init(const std::string& ip, int port) {
    m_ipAddr = ip;
    m_port = port;
    m_listenFd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = ::htons(port);
    inet_aton(ip.c_str(), &addr.sin_addr);

    int ret = bind(m_listenFd, (sockaddr*)&addr, sizeof addr);
    if(ret < 0) {
        cout<<"WebServer::Init bind failed. ret="<<ret<<endl;
        return ret;
    }
    ret = listen(m_listenFd, 128);

    m_epollFd = epoll_create(10);
    // epoll注册
    epoll_event event;
    event.data.fd = m_listenFd;
    event.events = EPOLLIN | EPOLLET;
    ret = epoll_ctl(m_epollFd, EPOLL_CTL_ADD, m_listenFd, &event);
    if(ret < 0) {
        cout<<"epoll_ctl failed. ret="<<ret<<endl;
        return ret;
    }
    return ret;
}

int WebServer::ReceiveMsg(std::function<void (const std::string&)>& func) {
    epoll_event events[1024];
    int nums = epoll_wait(m_epollFd, events, 10, -1);
    for(int i = 0; i < nums; i++) {
        if(events[i].data.fd == m_listenFd) {
            if(events[i].events != EPOLLIN) {
                continue;
            }
            sockaddr_in clientAddr;
            clientAddr.sin_family = AF_INET;
            socklen_t clinetLen = sizeof(clientAddr);

            int connFd = accept(m_listenFd, (sockaddr*)&clientAddr, &clinetLen);
            epoll_event event;
            event.data.fd = connFd;
            event.events = EPOLLIN | EPOLLET;
            int ret = epoll_ctl(m_epollFd, EPOLL_CTL_ADD, connFd, &event);
            if(ret < 0) {
                cout<<"WebServer::ReceiveMsg epoll_ctl failed. ret="<<ret<<endl;
            } else {
                cout<< "WebServer::accept success, addr="<<std::string(inet_ntoa(clientAddr.sin_addr))<<" clientPort="<< ntohs(clientAddr.sin_port)<<endl;
            }         
        } else {
            char buffer[1024];
            memset(buffer, 0, 1024);
            int ret = recv(events[i].data.fd, buffer, 1024, MSG_NOSIGNAL);
            if(ret < 0) {
                 cout<< "WebServer::recv failed, ret="<<ret<<endl;
            }
            ret = send(events[i].data.fd, buffer, 1024, MSG_NOSIGNAL);
            if(ret < 0) {
                 cout<< "WebServer::send failed, ret="<<ret<<endl;
            }            
            func(buffer);
        }
    }
    return nums;
}

