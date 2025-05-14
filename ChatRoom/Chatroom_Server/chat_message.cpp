#include"chat_message.h"

#include<boost/asio.hpp>

#include<deque>
#include<iostream>
#include<list>
#include<memory>
#include<set>
#include<utility>

using boost::asio::ip::tcp;
using chat_message_queue = std::deque<chat_message>;
using chat_message_list =std::list<chat_message>;

class chat_session;

using chat_session_ptr=std::shared_ptr<chat_session>;

class chat_room{                                                                 //聊天室
    public:
    void join(chat_session_ptr);                                                 //加入聊天室
    void leave(chat_session_ptr);                                                //离开聊天室
    void deliver(const chat_message&);                                           //聊天室的信息传递
    private:
    std::set<chat_session_ptr> participants_;                                //聊天室的成员
    enum{max_recent_msg=100};
    chat_message_queue recent_msgs_;
};

class chat_session: public std::enable_shared_from_this<chat_session>
{
    public:
        chat_session(tcp::socket socket,chat_room &room):socket_(std::move(socket)),
        room_(room){}

        void start(){
            room_.join(shared_from_this());
            do_read_header();
        }

        void deliver(const chat_message &msg){
            bool write_in_progress=!write_msgs_.empty();
            write_msgs_.push_back(msg);
            if(!write_in_progress){
                do_write();
            }
        }

    private:
    void do_read_header(){
        auto self(shared_from_this());
        boost::asio::async_read(socket_,                                                        //async_read必须用mutable_buffer
        boost::asio::buffer(read_msgs_.data(),chat_message::header_length),
        [this,self](boost::system::error_code ec,std::size_t /*length*/){
            if(!ec && read_msgs_.decode_header()){
                do_read_body();
            }
            else{
                room_.leave(shared_from_this());
            }
        });
    }

    void do_read_body(){
        auto self(shared_from_this());
        boost::asio::async_read(socket_,
        boost::asio::buffer(read_msgs_.body(),read_msgs_.body_length()),
        [this,self](boost::system::error_code ec,std::size_t /*length*/){
            if(!ec){
                room_.deliver(read_msgs_);
                do_read_body();
            }else{
                room_.leave(shared_from_this());
            }
        });
    }


    void do_write(){
        auto self(shared_from_this());
        boost::asio::async_write(socket_,                                                       //async_write必须用const_buffer
        boost::asio::buffer(write_msgs_.front().data(),write_msgs_.front().length()),
        [this,self](boost::system::error_code ec,std::size_t /*length*/){
            if(!ec){
                write_msgs_.pop_front();
                if(!write_msgs_.empty()){
                    do_write();
                }
            }
            else{
                room_.leave(shared_from_this());
            }
        });
    }

    tcp::socket socket_;
    chat_room &room_;
    chat_message read_msgs_;
    chat_message_queue write_msgs_;
};


class chat_server{
    public:
    chat_server(boost::asio::io_service &io_service ,const tcp::endpoint &endpoint):acceptor_(io_service,endpoint),
    socket_(io_service){
        do_accept();
    }

    private:
    void do_accept(){
        acceptor_.async_accept(socket_,[this](boost::system::error_code ec){
            if(!ec){
                auto session=std::make_shared<chat_session>(std::move (socket_),room_);
                session->start();
            }

            do_accept();
        });
    }

    tcp::acceptor acceptor_;
    tcp::socket socket_;
    chat_room room_;
};

void chat_session::join(chat_session_ptr participant){                                  //加入聊天室
    participants_.insert(participant);
    for(const auto& msg : recent_msgs_){
        participant->deliver(msg);
    }
}
void chat_session::leave(chat_session_ptr participant){                                     //离开聊天室
    participants_.erase(participant);
}
void chat_session::deliver(const chat_message &msg){                                    //聊天室的信息传递
    recent_msgs_.push_back(msg);
    while(recent_msgs_.size()>max_recent_msg){
        recent_msgs_.pop_front();
    }
    for(auto& participant : participants_){
        participant->deliver(msg);
    }
}

int main(int argc,char* argv[]){
    try{
        if(argc<2){
            std::cerr<<"Usage:: chat_server <port> ...\n";
            return 1;
        }

        boost::asio::io_service io_service;

        std::list<chat_server> servers;
        for(int i=1;i<argc;++i){
            tcp::endpoint endpoint(tcp::v4(),std::atoi(argv[i]));
            servers.emplace_back(io_service,endpoint);
        }

        io_service.run();
    }catch(std::exception &e){
        std::cerr<<"Exception: "<<e.what()<<"\n";
    }
}