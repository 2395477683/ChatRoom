#ifndef CHAT_MESSAGE_HPP
#define CHAT_MESSAGE_HPP

#include"structHeader.h"

#include<iostream>

#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cassert>

class chat_message{
    public:
    enum{header_length=sizeof(Header)};
    enum{max_body_length=512};

    chat_message(){}

    const char* data() const {return data_;}

    char* data(){return data_;}

    std::size_t length() const {return header_length+m_header.bodySize;}

    const char* body() const {return data_+header_length;}

    char* body(){return data_+header_length;}

    int type() const {
        return m_header.type;
    }

    std::size_t body_length() const {return m_header.bodySize;}

    void setMessage(int messageType,const void *buffer,size_t bufferSize){         //设置消息的类型、消息体的内容和长度
        assert(bufferSize<=max_body_length);
        m_header.bodySize =bufferSize;
        m_header.type=messageType;
        std::memcpy(body(),buffer,bufferSize);
        std::memcpy(data(),&m_header,sizeof(m_header));
    }


    bool decode_header(){                                                          //解码从网络连接中读取的数据头部，以获取数据主体的长度
        std::memcpy(&m_header,data(),header_length);
        if(m_header.bodySize>max_body_length){
            std::cout<<"数据过多"<<"\n";
            return false;
        }
        return true;
    }

    private:
    char data_[header_length+max_body_length];
    Header m_header;

};
#endif