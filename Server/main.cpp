#include "WebServer.h"
using namespace std;
int main() {
    WebServer webServer;
    int ret = webServer.Init("127.0.0.1", 4567);
    if(ret != 0) {
        cout<<"Server: init failed. "<<endl;
    }
    std::function<void (const std::string&)> func = [](const std::string& msg)->void {
        cout<<"Receive from client: "<<msg<<" ";
    };
    int count = 0;
    while(true) {
        if(webServer.ReceiveMsg(func) != -1) {
            count++;
            std::cout<<"Server:Receive times="<<count<<endl;
        }
    }
    return 0;
}