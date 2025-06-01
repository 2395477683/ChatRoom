#ifndef CHAT_MESSAGE_H
#define CHAT_MESSAGE_H
#include"structHeader.h"
#include"SerilizationRoomInfo.h"


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
    enum{header_length=sizeof(Header)};                                                 //重要！！！消息头的大小
    enum{max_body_length=512};

    chat_message(){}

    const char* data() const {return data_;}                                            //返回指向data_数组起始位置的指针

    char* data(){return data_;}

    std::size_t length() const {return header_length+m_header.bodySize;}                //返回信息长度

    const char* body() const {return data_+header_length;}

    char* body(){return data_+header_length;}                                           //返回指向消息体（body）起始位置的指针

    std::size_t body_length() const {return  m_header.bodySize;}

    int type() const {return m_header.type;}

    bool decode_header();                                                               //解码从网络连接中读取的数据头部，以获取数据主体的长度

    void setMessage(int messageType,const void *buffer,size_t bufferSize);              //设置消息的类型、消息体的内容和长度
    
    void setMessage(int messageType,const std::string &buffer){
    setMessage(messageType,buffer.data(),buffer.size());
    }

    private:
    char data_[header_length+max_body_length];                                          //将消息头（header）和消息体（body）存储在同一块连续内存中
    Header m_header;                                                                    //用于记录消息体的实际长度
};
#endif