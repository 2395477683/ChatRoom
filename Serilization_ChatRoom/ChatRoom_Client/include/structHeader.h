#ifndef STRUCT_HEADER_H
#define STRUCT_HEADER_H

#include<string>

struct Header{                                          //存储消息类型和消息体长度
    int bodySize;
    int type;
};

enum{                                                   //不同的消息类型
    MT_BIND_NAME=1,
    MT_CHAT_INFO=2,
    MT_SERVER_INFO=3
};

struct BindName{                                        //存储用户名
    char name[32];
    int nameLen;

};

struct ChatInformation{                                 //存储聊天信息
    char information[256];
    int infoLen;
};

struct RoomInformation{                                 //存储消息
    BindName name;
    ChatInformation chat;
};

bool parseMessage(const std::string &input,int* type,std::string &outbuffer);  //检查消息是否合法，处理消息
#endif