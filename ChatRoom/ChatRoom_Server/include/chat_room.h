#ifndef CHAT_ROOM_H
#define CHAT_ROOM_H
#include"chat_message.h"

class chat_session;
using chat_session_ptr=std::shared_ptr<chat_session>;

class chat_room{                                                                    //聊天室
    public:
    void join(chat_session_ptr);                                                    //加入聊天室
    void leave(chat_session_ptr);                                                   //离开聊天室
    void deliver(const chat_message&);                                              //聊天室的信息传递
    private:
    std::set<chat_session_ptr> participants_;                                       //聊天室的成员
    enum{max_recent_msg=100};
    chat_message_queue recent_msgs_;                                                //保存聊天记录                                             
};

#endif