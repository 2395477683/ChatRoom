#ifndef FND_SERI_ROOMINFO
#define FND_SERI_ROOMINFO
#include"SerilizationChatInfo.h"
#include"SerilizationBindName.h"

#include<boost/archive/text_oarchive.hpp>
#include<boost/archive/text_iarchive.hpp>

class SRoomInfo{
    private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar,const unsigned int version){
        ar & m_bind;
        ar & m_chat;
    }

    SBindName m_bind;
    SChatInfo m_chat;

    public:
    SRoomInfo(std::string name,std::string info):m_bind(std::move(name)),m_chat(std::move(info)){}
    SRoomInfo(){}
    const std::string &name()const {return  m_bind.bindName();}
    const std::string &information()const {return  m_chat.ChatInformation();}
};

#endif