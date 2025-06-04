#include"structHeader.h"
#include"Protocal.pb.h"

#include<cstdlib>
#include<cstring>

#include<iostream>

#include <cstring>

bool parseMessage(const std::string &input,int *type,std::string &outbuffer){
    auto pos=input.find_first_of(" ");
    if(pos==std::string::npos||pos==0){
        std::cout<<"输入格式错误"<<"\n";
        return false;
    }
    auto command=input.substr(0,pos);
    if(command=="BindName"){
        std::string name=input.substr(pos+1);
        if(name.size()>32){
            std::cout<<"你的名字太长了！"<<"\n";
            return false;
        }
        *type=MT_BIND_NAME;
        PBindName BindName;
        BindName.set_name(name);
        auto ok=BindName.SerializeToString(&outbuffer);                                
        return ok;
    }
    else if(command=="Chat"){
        std::string chat=input.substr(pos+1);
        if(chat.size()>256){
            std::cout<<"你发送的消息太长了！"<<"\n";
            return false;
        }
        *type=MT_CHAT_INFO;
        PChat chats;
        chats.set_information(chat);
        auto ok=chats.SerializeToString(&outbuffer);                      
        return ok;
    }
    else{
        std::cout<<"没有这个指令！"<<"\n";
        return false;
    }

}