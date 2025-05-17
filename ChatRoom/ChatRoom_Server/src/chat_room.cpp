#include"chat_room.h"
#include"chat_session.h"



void chat_room::join(chat_session_ptr participant){                                         //加入聊天室
    participants_.insert(participant);
    for(const auto& msg : recent_msgs_){                                                    
        participant->deliver(msg);                                                          //广播聊天记录
    }
}

void chat_room::leave(chat_session_ptr participant){                                        //离开聊天室
    participants_.erase(participant);
}

void chat_room::deliver(const chat_message &msg){                                           //聊天室的信息传递
    recent_msgs_.push_back(msg);
    while(recent_msgs_.size()>max_recent_msg){                                              //超过记录存储上线，删除最开始的记录
        recent_msgs_.pop_front();
    }
    for(auto& participant : participants_){
        participant->deliver(msg);
    }
}