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

ptree chat_session::toPtree(){                                                          //转换原始数据到属性树
    ptree obj;
    std::stringstream ss( std::string(read_msgs_.body(),read_msgs_.body()+read_msgs_.body_length()));
    boost::property_tree::read_json(ss,obj);
    return obj;
}

void chat_session::handleMessage(){                                                     //处理客户端发送的信息
    if(read_msgs_.type()==MT_BIND_NAME){
        auto nameTree=toPtree();
        m_name=nameTree.get<std::string>("name");                                       //获取用户姓名
        std::cout<<m_name<<"用户创建成功"<<"\n";
    }
    else if(read_msgs_.type()==MT_CHAT_INFO){
        auto chatPtree=toPtree();                                                       
        m_chatInformation=chatPtree.get<std::string>("information");                    //获取客户端发送的消息
        auto rinfo=buildinformation();                                          

        chat_message msg;
        msg.setMessage(MT_SERVER_INFO,rinfo);
        room_.deliver(msg);
    }
    else{
        std::cout<<"无效的代码"<<"\n";
    }
}

std::string chat_session::buildinformation(){                                            //将用户的消息转换为文本形式的字符串
        ptree tree;
        tree.put("name",m_name);
        tree.put("information",m_chatInformation);
        return ptreeToJsonString(tree);
}
