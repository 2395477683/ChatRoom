#ifndef FND_SERI_BINDNAME
#define FND_SERI_BINDNAME
#include<boost/archive/text_oarchive.hpp>
#include<boost/archive/text_iarchive.hpp>

class SBindName{
    private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar,const unsigned int version){
        ar & m_bindName;
    }

    std::string m_bindName;

    public:
    SBindName(std::string name): m_bindName(std::move(name)){}
    SBindName(){}
    const std::string &bindName()const {return  m_bindName;}
};
#endif