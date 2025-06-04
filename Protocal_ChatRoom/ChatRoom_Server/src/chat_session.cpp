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
    socket_,                                                                                
    boost::asio::buffer(read_msgs_.data(),chat_message::header_length),                     //async_read必须用mutable_buffer
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
    socket_,                                                                               
    boost::asio::buffer(write_msgs_.front().data(),write_msgs_.front().length()),           //async_write必须用const_buffer
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

bool chat_session::fillProtobuf(::google::protobuf::Message* msg){                      //将接收到的网络数据反序列化为 Protocol Buffers 消息对象
    std::string ss(read_msgs_.body(),read_msgs_.body()+read_msgs_.body_length());
    auto ok=msg->ParseFromString(ss);                                                   //将二进制字符串解析为 protobuf 对象
    return ok;
}

void chat_session::handleMessage(){                                                     //处理客户端发送的信息
    if(read_msgs_.type()==MT_BIND_NAME){
        PBindName BindName;
        if(fillProtobuf(&BindName)){
            m_name=BindName.name();
        }                                  
        std::cout<<m_name<<"用户创建成功"<<"\n";
    }
    else if(read_msgs_.type()==MT_CHAT_INFO){
        PChat chat;
        if(!fillProtobuf(&chat)){return;}
        m_chatInformation=chat.information();

        auto rinfo=buildinformation();                                       

        chat_message msg;
        msg.setMessage(MT_SERVER_INFO,rinfo);
        room_.deliver(msg);
    }
    else{
        std::cout<<"无效的代码"<<"\n";
    }
}

std::string chat_session::buildinformation(){                                           //构建并序列化消息信息                                     
        ProomInformation roomInfo;                                                      //构建
        roomInfo.set_name(m_name);
        roomInfo.set_information(m_chatInformation);
        std::string out;
        auto ok=roomInfo.SerializeToString(&out);                                       //序列化
        return out;
}
