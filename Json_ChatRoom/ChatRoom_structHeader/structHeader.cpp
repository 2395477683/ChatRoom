#include"structHeader.h"
#include"SerilizationObject.h"
#include"JsonObject.h"

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
        ptree tree;
        tree.put("name",name);
        outbuffer=ptreeToJsonString(tree);                                       
        return true;
    }else if(command=="Chat"){
        std::string Chat_msgs=input.substr(pos+1);
        if(Chat_msgs.size()>256){
            return false;
        }
        if(type){
            *type=MT_CHAT_INFO;
        }
        ptree tree;
        tree.put("information",Chat_msgs);
        outbuffer=ptreeToJsonString(tree);                                       
        return true;
    }else{
        std::cout<<"没有这个指令"<<"\n";
        return false;
    }
}
