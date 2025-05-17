#ifndef CHAT_SESSION_H
#define CHAT_SESSION_H
#include"chat_message.h"

class chat_room;


class chat_session: public std::enable_shared_from_this<chat_session>
{
    public:
        chat_session(tcp::socket socket,chat_room &room):socket_(std::move(socket)),
        room_(room){}

        void start();

        void deliver(const chat_message &msg);

    private:
    void do_read_header();

    void do_read_body();

    void do_write();

    tcp::socket socket_;
    chat_room &room_;
    chat_message read_msgs_;
    chat_message_queue write_msgs_;
};

using chat_session_ptr=std::shared_ptr<chat_session>;


#endif