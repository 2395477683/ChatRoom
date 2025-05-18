#ifndef CHAT_CLINENT_H
#define CHAT_CLINENT_H
#include"chat_message.h"


class chat_client{
    public:
    chat_client(boost::asio::io_service &io_service,tcp::resolver::iterator endpoint_iterator):
    io_service_(io_service),socket_(io_service){                                //创建 TCP 套接字socket_,socke_关联到 io_service，使socket_的异步操作由该事件循环管理
        do_connect(endpoint_iterator);
    }

    void write(const chat_message &msg);

    void close(){io_service_.post([this](){socket_.close();});}

    private:
    void do_connect(tcp::resolver::iterator endpoint_iterator);                 //建立一个TCP连接到由endpoint_iterator(迭代器)指定的网络端点

    void do_read_header();                                                      //异步读取TCP连接中的数据头部

    void do_read_body();                                                        //异步读取TCP连接中的数据

    void do_write();                                                            //异步往TCP连接发送数据

    boost::asio::io_service &io_service_;                                       //管理和调度异步 I/O 操作
    tcp::socket socket_;                                                        //客户端套接字
    chat_message read_msgs_;                                                    //读取的消息
    chat_message_queue write_msgs_;                                             //写入的消息队列
};
#endif