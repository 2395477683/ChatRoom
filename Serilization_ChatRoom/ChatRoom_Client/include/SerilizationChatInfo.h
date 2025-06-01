#ifndef FND_SERI_CHATINFO
#define FND_SERI_CHATINFO
#include<boost/archive/text_oarchive.hpp>
#include<boost/archive/text_iarchive.hpp>

class SChatInfo{
    private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar,const unsigned int version){
        ar & m_ChatInformation;
    }

    std::string m_ChatInformation;

    public:
    SChatInfo(std::string info): m_ChatInformation(std::move(info)){}
    SChatInfo(){}
    const std::string &ChatInformation()const {return  m_ChatInformation;}
};

#endif