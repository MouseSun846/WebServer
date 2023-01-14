rm -rf ./Output
mkdir Output
cd ./Output
cd ../Server
g++ main.cpp WebServer.cpp WebServer.h -o ../Output/webserver
cd ../Client
g++ main.cpp TcpConnect.cpp TcpConnect.h -pthread -o ../Output/client
cd ../Output