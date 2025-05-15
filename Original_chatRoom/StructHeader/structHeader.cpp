#include"structHeader.h"

#include<cstdlib>
#include<cstring>

#include<iostream>

bool parseMessage(const std::string &input,int *type,std::string &outbuffer){
    auto pos=input.find_first_of(" ");
    if(pos==std::string::npos||pos==0){
        return false;
    }
    auto command =input.substr(0,pos);
    if(command=="BindName"){
        std::string name=input.substr(pos+1);
        if(name.size()>32){
            return false;
        }
        if(type){
            *type=MT_BIND_NAME;
        }
        BindName bindInfo;
        bindInfo.nameLen=name.size();
        std::memcpy(&bindInfo.name,name.data(),name.size());
        auto buffer =reinterpret_cast<const char*>(&bindInfo);
        outbuffer.assign(buffer,buffer+sizeof(bindInfo));                                       //将outbuffer的内容替换sizeof(bindInfo)个字节的数据
        return true;
    }else if(command=="Chat"){
        std::string Chat_msgs=input.substr(pos+1);
        if(Chat_msgs.size()>256){
            return false;
        }
        if(type){
            *type=MT_CHAT_INFO;
        }
        ChatInformation chatInfo;
        chatInfo.infoLen=Chat_msgs.size();
        std::memcpy(&chatInfo.information,Chat_msgs.data(),Chat_msgs.size());
        auto buffer =reinterpret_cast<const char*>(&chatInfo);
        outbuffer.assign(buffer,buffer+sizeof(chatInfo));                                       //将outbuffer的内容替换sizeof(bindInfo)个字节的数据
        return true;
    }else{
        std::cout<<"没有这个指令"<<"\n";
        return false;
    }
}
