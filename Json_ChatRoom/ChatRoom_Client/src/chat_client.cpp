#include"chat_client.h"

void chat_client::write(const chat_message &msg){
    boost::asio::post(
        io_service_,   
        [this,msg](){                                                                       //将一个任务投递到 io_service 的事件队列中.post() 方法会立即返回，不会等待任务执行完毕
        bool write_in_progress=!write_msgs_.empty();
        write_msgs_.push_back(msg);
        if(!write_in_progress){                                                             //检查 write_msgs_ 队列是否为空，如果为空，表示当前没有正在进行的写操作。
            do_write();
        } 
    });
}


void chat_client::do_connect(tcp::resolver::results_type endpoint_iterator){                //建立一个TCP连接到由endpoint_iterator(迭代器)指定的网络端点
    boost::asio::async_connect(
    socket_,
    endpoint_iterator,                                                                
    [this](boost::system::error_code ec,tcp::endpoint){
        if(!ec){
            std::cout<<"成功加入聊天室，你可以使用以下指令："<<"\n";
            std::cout<<"1.BindName 创建你的用户名。"<<"\n";
            std::cout<<"2.Chat 向聊天室中发送你的消息。"<<"\n";
            do_read_header();
        }
    });
}

void chat_client::do_read_header(){                                                         //异步读取TCP连接中的数据头部
    boost::asio::async_read(
    socket_,
    boost::asio::buffer(read_msgs_.data(),chat_message::header_length),                     //将read_msgs_的前header_length字节包装为一个缓冲区，供 Asio 读取或写入消息头部
    [this](boost::system::error_code ec, std::size_t /*length*/){
    if(!ec && read_msgs_.decode_header()){
        do_read_body();
    }else{
        socket_.close();
    }
    });
}

void chat_client::do_read_body(){                                                          //异步读取TCP连接中的数据体
    boost::asio::async_read(
    socket_,
    boost::asio::buffer(read_msgs_.body(),read_msgs_.body_length()),
    [this](boost::system::error_code ec,std::size_t /*length*/){                           //ec 和 length 是异步操作执行结果的元信息，它们的值由底层 I/O 操作的状态决定
        if(!ec){
            if(read_msgs_.type()==MT_SERVER_INFO){
                std::stringstream ss(std::string(read_msgs_.body(),read_msgs_.body()+read_msgs_.body_length()));     //创建字符串流并用原始数据初始化   
                ptree tree;                                                                                          //创建空的属性树容器
                boost::property_tree::read_json(ss,tree);                                                            //从字符串流解析 JSON 数据到属性树

                std::cout<<"用户: ";
                std::cout<<tree.get<std::string>("name");
                std::cout<<"  发送: ";
                 std::cout<<tree.get<std::string>("information");
                std::cout<<"\n";
            }
            do_read_header();
        }else{
            socket_.close();
        }
    });
}

void chat_client::do_write(){                                                            //异步往TCP连接写入数据
    boost::asio::async_write(
    socket_,
    boost::asio::buffer(write_msgs_.front().data(),write_msgs_.front().length()),
    [this](boost::system::error_code ec,std::size_t /*length*/){
        if(!ec){
            write_msgs_.pop_front();
            if(!write_msgs_.empty()){
                do_write();
            }
        }else{
            socket_.close();
        }
    });
}



int main(int argc,char *argv[]){
try{
    if(argc!=3){
        std::cerr<<"Usage: chat_client <host> <port>\n";
        return 1;
    }

    boost::asio::io_context io_service;                                           //创建异步调用核心

    tcp::resolver resolver(io_service);                                           //创建TCP解析器对象
    auto endpoint_iterator=resolver.resolve(argv[1],argv[2]);                     //解析网络地址和端口号，以获取可以用于网络连接的端点信息,返回一个endpoint类型的迭代器

    chat_client c(io_service,endpoint_iterator);                                  //创建客户端

    std::thread t([&io_service](){io_service.run();});

    char line[chat_message::max_body_length+1];
    while(std::cin.getline(line,chat_message::max_body_length+1)){
        chat_message msg;
        auto type=0;
        std::string input(line,line+std::strlen(line));
        std::string output;
        if(parseMessage(input,&type,output)){
            msg.setMessage(type,output.data(),output.size());
            c.write(msg);
            std::cout<<""<<"\n";
        }  
    }
    c.close();
    t.join();
}catch(std::exception &e){
    std::cerr<<"Client Exception:"<<e.what()<<"\n";
    }
    return 0;
}