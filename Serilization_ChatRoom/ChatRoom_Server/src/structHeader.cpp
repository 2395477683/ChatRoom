#include"structHeader.h"
#include"SerilizationBindName.h"
#include"SerilizationChatInfo.h"

#include<cstdlib>
#include<cstring>

#include<iostream>

template <typename T>
std::string seriliaze(const T &obj){
    std::stringstream ss;                                                               //创建一个 std::stringstream 对象 ss（字符串流）
    boost::archive::text_oarchive oa(ss);                                               //创建一个 Boost 文本输出归档对象 oa。
    oa & obj;                                                                           //使用 & 操作符将对象 obj 序列化到归档 oa 中
    return ss.str();                                                                    //获取文本格式的序列化结果
}

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
        outbuffer=seriliaze(SBindName(std::move(name)));       
        return true;
    }
    else if(command=="Chat"){
        std::string chat=input.substr(pos+1);
        if(chat.size()>256){
            std::cout<<"你发送的消息太长了！"<<"\n";
            return false;
        }
        *type=MT_CHAT_INFO;
        outbuffer=seriliaze(SChatInfo(std::move(chat)));    
        return true;
    }
    else{
        std::cout<<"没有这个指令！"<<"\n";
        return false;
    }

}