#include "TcpConnect.h"
#include <chrono>
#include <functional>
using namespace std;

int main() {

    TcpConnect tcpConnect;
    int ret = tcpConnect.Init("127.0.0.1", 4567, 7654);
    if(ret != 0) {
        cout<<"Client init failed"<<endl;
        return ret;
    }
    std::thread(
        [&]()->void {
            int count = 0;
            while(true) {
                std::string msg = "I'm is client, count=";
                msg += to_string(count++);
                tcpConnect.SendMsg(msg);
                std::this_thread::sleep_for(std::chrono::seconds(3));
            }    
        }
    ).detach();
    
    std::function<void(const std::string&)> func = [](const std::string& msg)->void {
        cout<<"Receive from Server:"<<msg<<" ";
    };
    int count=0;
    while(true) {
        ret = tcpConnect.ReceiveMsg(func);
        if(ret != -1) {
            count++;
            std::cout<<"Client:Receive times="<<count<<endl;
        }
    }
    return 0;
}