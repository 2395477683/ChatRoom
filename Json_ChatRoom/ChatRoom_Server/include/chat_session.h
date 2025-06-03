#ifndef CHAT_SESSION_H
#define CHAT_SESSION_H
#include"chat_message.h"

class chat_room;


class chat_session: public std::enable_shared_from_this<chat_session>
{
    public:
        chat_session(tcp::socket socket,chat_room &room):
        socket_(std::move(socket)),
        room_(room){}

        void start();                                                               //将客户端加入聊天室

        void deliver(const chat_message &msg);

        template<typename T>
        T toObeject();

    private:
    void do_read_header();                                                          

    void do_read_body();

    void do_write(); 

    void handleMessage();

    ptree toPtree();
    
    std::string buildinformation();

    tcp::socket socket_;                                                            //socket套接字
    chat_room &room_;                                                               //聊天室
    chat_message read_msgs_;
    chat_message_queue write_msgs_;
    std::string m_name;                                                             //发送消息的用户名
    std::string m_chatInformation;                                                  //用户发送的消息
};

using chat_session_ptr=std::shared_ptr<chat_session>;


#endif