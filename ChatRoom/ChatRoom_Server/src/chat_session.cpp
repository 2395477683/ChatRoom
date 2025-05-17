#include"chat_session.h"
#include"chat_room.h"

void chat_session::start(){
    room_.join(shared_from_this());
    do_read_header();
}

void chat_session::deliver(const chat_message &msg){
    bool write_in_progress=!write_msgs_.empty();
    write_msgs_.push_back(msg);
    if(!write_in_progress){
        do_write();
    }
}

void chat_session::do_read_header(){
    auto self(shared_from_this());
    boost::asio::async_read(
    socket_,                                                                                 //async_read必须用mutable_buffer
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

void chat_session::do_read_body(){
    auto self(shared_from_this());
    boost::asio::async_read(
    socket_,
    boost::asio::buffer(read_msgs_.body(),read_msgs_.body_length()),
    [this,self](boost::system::error_code ec,std::size_t /*length*/){
        if(!ec){
            room_.deliver(read_msgs_);
            do_read_header();
        }else{
            room_.leave(shared_from_this());
        }
    });
}


void chat_session::do_write(){
    auto self(shared_from_this());
    boost::asio::async_write(
    socket_,                                                                                //async_write必须用const_buffer
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

