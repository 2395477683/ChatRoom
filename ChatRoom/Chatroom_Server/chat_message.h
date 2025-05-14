#ifndef CHAT_MESSAGE_HPP
#define CHAT_MESSAGE_HPP

#include<cstdio>
#include<cstdlib>
#include<cstring>

class chat_message{
    public:
    enum{header_length=4};
    enum{max_body_length=512};

    chat_message() : body_length_(0){}

    const char* data() const {return data_;}

    char* data(){return data_;}

    std::size_t length() const {return header_length+body_length_;}

    const char* body() const {return data_+header_length;}

    char* body(){return data_+header_length;}

    std::size_t body_length() const {return body_length_;}

    void body_length(std::size_t new_length){
        body_length_=new_length;
        if(body_length_>max_body_length){
            body_length_=max_body_length;
        }
    }

    bool decode_header(){                                               //解码从网络连接中读取的数据头部，以获取数据主体的长度
        char header[header_length+1]="";
        std::strncat(header,data_,header_length);                       //将data_（通常是之前通过async_read读取到缓冲区中的数据的前header_length个字符复制到header数组中
        body_length_=std::atoi(header);                                 //从头部获取数据体的长度
        if(body_length_>max_body_length){
            body_length_=0;
            return false;
        }
        return true;
    }

    void encode_header(){
        char header[header_length+1]="";
        std::sprintf(header,"%4d",static_cast<int>(body_length_));
        std::memcpy(data_,header,header_length);
    }
    

    private:
    char data_[header_length+max_body_length];
    std::size_t body_length_;

};
#endif