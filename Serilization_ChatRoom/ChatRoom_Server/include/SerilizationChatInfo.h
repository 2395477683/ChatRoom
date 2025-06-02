#ifndef FND_SERI_CHATINFO
#define FND_SERI_CHATINFO
#include<boost/archive/text_oarchive.hpp>
#include<boost/archive/text_iarchive.hpp>

class SChatInfo{
    private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar,const unsigned int version){                             //序列化函数 序列化和反序列化时成员顺序必须一致
        ar & m_ChatInformation;                                                         //将m_ChatInformation写入流
    }

    std::string m_ChatInformation;

    public:
    SChatInfo(std::string info): m_ChatInformation(std::move(info)){}
    SChatInfo(){}
    const std::string &ChatInformation()const {return  m_ChatInformation;}
};

#endif