#ifndef FND_SERI_BINDNAME
#define FND_SERI_BINDNAME
#include<boost/archive/text_oarchive.hpp>
#include<boost/archive/text_iarchive.hpp>

class SBindName{
    private:
    friend class boost::serialization::access;

    template<class Archive>                                 
    void serialize(Archive &ar,const unsigned int version){                                 //序列化函数 序列化和反序列化时成员顺序必须一致
        ar & m_bindName;                                                                    //将m_bindName写入流
    }

    std::string m_bindName;

    public:
    SBindName(std::string name): m_bindName(std::move(name)){}
    SBindName(){}
    const std::string &bindName()const {return  m_bindName;}
};
#endif