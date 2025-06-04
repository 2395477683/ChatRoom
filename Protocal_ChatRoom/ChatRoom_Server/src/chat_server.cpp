#include"chat_server.h"
#include"chat_session.h"


void chat_server::do_accept(){                                                          //获取客户端的连接请求
    acceptor_.async_accept(
        socket_,
        [this](boost::system::error_code ec){
        if(!ec){
            auto session=std::make_shared<chat_session>(std::move (socket_),room_);
            session->start();
        }
        do_accept();
    });
}


int main(int argc,char* argv[]){
    try{
         GOOGLE_PROTOBUF_VERIFY_VERSION;                                                //在程序启动时检查编译时使用的Protobuf头文件版本与运行时链接的库版本是否一致
        if(argc<2){
            std::cerr<<"Usage:: chat_server <port> ...\n";
            return 1;
        }

        boost::asio::io_context io_service;

        std::list<chat_server> servers;
        for(int i=1;i<argc;++i){
            tcp::endpoint endpoint(tcp::v4(),std::atoi(argv[i]));                       //绑定到所有本地网络接口（如 0.0.0.0:8080），监听来自任意 IP 的连接请求。
            servers.emplace_back(io_service,endpoint);                                  //endpoint是一个核心概念，用于表示网络连接的地址和端口信息
        }

        io_service.run();
    }catch(std::exception &e){
        std::cerr<<"Server Exception: "<<e.what()<<"\n";
    }
    google::protobuf::ShutdownProtobufLibrary();                                        //释放由Google Protocol Buffers（Protobuf）库分配的全局资源
    return 0;
}