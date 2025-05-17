#ifndef CHAT_MESSAGE_H
#define CHAT_MESSAGE_H
#include<boost/asio.hpp>



#include<iostream>
#include<list>
#include<deque>
#include<memory>
#include<set>
#include<utility>
#include<thread>

#include<cstdio>
#include<cstdlib>
#include<cstring>


class chat_message;

using boost::asio::ip::tcp;
using chat_message_queue = std::deque<chat_message>;
using chat_message_list =std::list<chat_message>;

class chat_message{
    public:
    enum{header_length=4};
    enum{max_body_length=512};

    chat_message() : body_length_(0){}

    const char* data() const {return data_;}

    char* data(){return data_;}

    std::size_t length() const {return header_length+body_length_;}             //返回信息长度

    const char* body() const {return data_+header_length;}

    char* body(){return data_+header_length;}                                   //返回指向消息体（body）起始位置的指针

    std::size_t body_length() const {return body_length_;}

    void body_length(std::size_t new_length);                                   //设置消息体长度

    bool decode_header();                                                       //解码从网络连接中读取的数据头部，以获取数据主体的长度

    void encode_header();                                                       //将消息体长度编码为固定4字节的字符串头，并拷贝到数据缓冲区(data_);
    
    private:
    char data_[header_length+max_body_length];                                  //将消息头（header）和消息体（body）存储在同一块连续内存中
    std::size_t body_length_;                                                   //用于记录消息体的实际长度
};
#endif