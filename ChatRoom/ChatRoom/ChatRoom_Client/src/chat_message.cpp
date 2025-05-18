#include"chat_message.h"

void chat_message::body_length(std::size_t new_length){                             //设置消息体长度
    body_length_=new_length;
    if(body_length_>max_body_length){                                               //如果消息体过长只接收最大消息体长度内容
        body_length_=max_body_length;
    }
}

bool chat_message::decode_header(){                                                 //解码从网络连接中读取的数据头部，以获取数据主体的长度
    char header[header_length+1]="";
    std::strncat(header,data_,header_length);                                       //将data_（通常是之前通过async_read读取到缓冲区中的数据)的前header_length个字符复制到header数组中
    body_length_=std::atoi(header);                                                 //从头部获取数据体的长度
    if(body_length_>max_body_length){
        body_length_=0;
        return false;
    }
    return true;
}

void chat_message::encode_header(){                                                 //将消息体长度编码为固定4字节的字符串头，并拷贝到数据缓冲区(data_);
    char header[header_length+1]="";
    std::sprintf(header,"%4d",static_cast<int>(body_length_));                      //将变量body_length_的值格式化为一个固定宽度为4的字符串，并存储在数组header中
    std::memcpy(data_,header,header_length);                                        //header 中的内容复制到数组 data_ 的开头部分
}
