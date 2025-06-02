#include"chat_message.h"


bool chat_message::decode_header(){                                                 //解码从网络连接中读取的数据头部，以获取数据主体的长度
    std::memcpy(&m_header,data(),header_length);                                    //通用内存块的复制,当需要复制任意二进制数据(如结构体、字节流)或非字符串内存块时。
    if(m_header.bodySize>max_body_length){
        std::cout<<"body size"<<m_header.bodySize<<" "<<m_header.type<<std::endl;
        return false;
    }
    return true;
}

void chat_message::setMessage(int messageType,const void * buffer,size_t bufferSize){
    m_header.bodySize=bufferSize;
    m_header.type=messageType;
    std::memcpy(body(),buffer,bufferSize);
    std::memcpy(data(),&m_header,sizeof(m_header));
}

void chat_message::setMessage(int messageType,const std::string &buffer){
    setMessage(messageType,buffer.data(),buffer.size());
}