#include"structHeader.h"
#include"SerilizationObject.h"
#include"JsonObject.h"
#include"Protocal.pb.h"

#include<cstdlib>
#include<cstring>

#include<iostream>

template <typename T>std::string seriliaze(const T &obj){
    std::stringstream ss;
    boost::archive::text_oarchive oa(ss);
    oa & obj;
    return ss.str();
}

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
        PBindName BindName;
        BindName.set_name(name);
        auto ok=BindName.SerializeToString(&outbuffer);                                
        return ok;
    }else if(command=="Chat"){
        std::string Chat_msgs=input.substr(pos+1);
        if(Chat_msgs.size()>256){
            return false;
        }
        if(type){
            *type=MT_CHAT_INFO;
        }
        PChat chat;
        chat.set_information(Chat_msgs);
        auto ok=chat.SerializeToString(&outbuffer);                      
        return ok;
    }else{
        std::cout<<"没有这个指令"<<"\n";
        return false;
    }
}
