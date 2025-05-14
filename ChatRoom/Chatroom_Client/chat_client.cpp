#include"chat_message.h"

#include<boost/asio.hpp>

#include<deque>
#include<iostream>
#include<thread>

#include<cstdlib>

using boost::asio::ip::tcp;

using chat_message_queue=std::deque<chat_message>;

class chat_client{
    public:
    chat_client(boost::asio::io_service &io_service,tcp::resolver::iterator endpoint_iterator):
    io_service_(io_service),socket_(io_service){
        do_connect(endpoint_iterator);
    }

    void write(const chat_message &msg){
        io_service_.post([this,msg](){                                          //将一个任务投递到 io_service 的事件队列中
            bool write_in_progress=!write_msgs_.empty();
            write_msgs_.push_back(msg);
            if(!write_in_progress){
                do_write();
            } 
        });
    }

    void close(){
        io_service_.post([this](){socket_.close();});
    }

    private:
    void do_connect(tcp::resolver::iterator endpoint_iterator){                  //建立一个TCP连接到由endpoint_iterator(迭代器)指定的网络端点
        boost::asio::async_connect(socket_,endpoint_iterator,
        [this](boost::system::error_code ec,tcp::resolver::iterator){
            if(!ec){
                do_read_header();}
        });
    }

    void do_read_header(){                                                      //异步读取TCP连接中的数据头部
        boost::asio::async_read(socket_,
        boost::asio::buffer(read_msgs_.data(),chat_message::header_length),
        [this](boost::system::error_code ec, std::size_t /*length*/){
        if(!ec && read_msgs_.decode_header()){
            do_read_body();
        }else{
            socket_.close();
        }
        });
    }

    void do_read_body(){                                                        //异步读取TCP连接中的数据体
        boost::asio::async_read(socket_,
        boost::asio::buffer(read_msgs_.body(),read_msgs_.body_length()),
        [this](boost::system::error_code ec,std::size_t /*length*/){
            if(!ec){
                std::cout.write(read_msgs_.body(),read_msgs_.body_length());
                std::cout<<"\n";
                do_read_header();
            }else{
                socket_.close();
            }
        });
    }

    void do_write(){                                                            //异步往TCP连接写入数据
        boost::asio::async_write(socket_,
        boost::asio::buffer(
        write_msgs_.front().data(),
        write_msgs_.front().length()),
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

    boost::asio::io_service &io_service_;
    tcp::socket socket_;
    chat_message read_msgs_;
    chat_message_queue write_msgs_;
};

int main(int argc,char *argv[]){
try{
    if(argc!=3){
        std::cerr<<"Usage: chat_client <host> <port>\n";
        return 1;
    }

    boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);                                           //创建TCP解析器对象
    auto endpoint_iterator=resolver.resolve({argv[1],argv[2]});                   //解析网络地址和端口号，以获取可以用于网络连接的端点信息,返回一个迭代器

    chat_client c(io_service,endpoint_iterator);

    std::thread t([&io_service](){io_service.run();});

    char line[chat_message::max_body_length+1];
    while(std::cin.getline(line,chat_message::max_body_length+1)){
        chat_message msg;
        msg.body_length(std::strlen(line));
        std::memcpy(msg.body(),line,msg.body_length());
        msg.encode_header();
        c.write(msg);
    }
    c.close();
    t.join();
}catch(std::exception &e){
    std::cerr<<"Exception: "<<e.what()<<"\n";
    }
    return 0;
}