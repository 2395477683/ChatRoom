#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H
#include"chat_message.h"
#include"chat_room.h"

class chat_server{
    public:
    chat_server(boost::asio::io_service &io_service ,const tcp::endpoint &endpoint):acceptor_(io_service,endpoint),
    socket_(io_service){
        do_accept();
    }

    private:
    void do_accept();

    tcp::acceptor acceptor_;
    tcp::socket socket_;
    chat_room room_;
};
#endif