#include"chat_session.h"
#include"chat_room.h"

void chat_session::start(){                                                                  //将客户端加入聊天室
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

void chat_session::do_read_header(){                                                        //读取消息头部
    auto self(shared_from_this());
    boost::asio::async_read(                                                                //异步读取数据直到缓冲区填满
    socket_,                                                                                //async_read必须用mutable_buffer
    boost::asio::buffer(read_msgs_.data(),chat_message::header_length),
    [this,self](boost::system::error_code ec,std::size_t /*length*/){
        if(!ec && read_msgs_.decode_header()){                                              //确保信息无误
            do_read_body();
        }
        else{
            room_.leave(shared_from_this());
        }
    });
}

void chat_session::do_read_body(){                                                          //读取消息体
    auto self(shared_from_this());
    boost::asio::async_read(
    socket_,
    boost::asio::buffer(read_msgs_.body(),read_msgs_.body_length()),
    [this,self](boost::system::error_code ec,std::size_t /*length*/){
        if(!ec){
            handleMessage();
            do_read_header();
        }else{
            room_.leave(shared_from_this());
        }
    });
}


void chat_session::do_write(){                                                             //向客户端广播消息
    auto self(shared_from_this());
    boost::asio::async_write(                                                              //异步发送数据直到所有数据写入完成
    socket_,                                                                               //async_write必须用const_buffer
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


void chat_session::handleMessage(){
    std::cout<<"type: "<<read_msgs_.type()<<"\n";
    if(read_msgs_.type()==MT_BIND_NAME){
        const BindName* bind=reinterpret_cast<const BindName*>(read_msgs_.body());
        m_name.assign(bind->name,bind->name+bind->nameLen);
        std::cout<<m_name<<"用户创建成功"<<"\n";
    }
    else if(read_msgs_.type()==MT_CHAT_INFO){
        const ChatInformation *chat=reinterpret_cast<const ChatInformation *>(read_msgs_.body());
        m_chatInformation.assign(chat->information,chat->information+chat->infoLen);
        auto rinfo=buildinformation();
        chat_message msg;
        msg.setMessage(MT_SERVER_INFO,&rinfo,sizeof(rinfo));
        room_.deliver(msg);
    }
    else{
        std::cout<<"无效的代码"<<"\n";
    }
}

RoomInformation chat_session::buildinformation(){
    RoomInformation info;
    info.name.nameLen=m_name.size();
    std::memcpy(info.name.name,m_name.data(),m_name.size());
    info.chat.infoLen=m_chatInformation.size();
    std::memcpy(info.chat.information,m_chatInformation.data(),m_chatInformation.size());
    return info;
}
