#ifndef FND_STRUCT_HEADER_H
#define FND_STRUCT_HEADER_H

#include<string>

struct Header{
    int bodySize;
    int type;
};

enum{
    MT_BIND_NAME=1,
    MT_CHAT_INFO=2,
    MT_SERVER_INFO=3
};

struct BindName{
    char name[32];
    int nameLen;
};

struct ChatInformation{
    char information[256];
    int infoLen;
};

struct RoomInformation{
    BindName name;
    ChatInformation chat;
};

bool parseMessage(const std::string &input,int* type,std::string &outbuffer);
#endif
