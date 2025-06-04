# ChatRoom

## 框架
ChatRoom文件夹保存的是聊天室的基本框架，其他消息协议的聊天室都基于该框架实现。

linux中使用apt安装boost无法生成cmake所需配置文件,请手动安装所需要的boost库组件,需要修改cmake中boots库所在路径

该框架基于Boost库实现,使用前确保已经安装Boost库及相关组件,Boost版本使用1.88以上.

使用了C++11以上版本的新特性.

虚拟机使用ubuntu

编译过程：

1. 在虚拟机的客户端或服务器端目录下运行命令 cmake .

2. 运行命令 make

3. 启动客户端 ./clientTest 服务器ip地址 服务器端口 或 启动服务器端  ./serverTest 服务器端口

# Original_ChatRoom

使用boost实现的最常见的消息协议

# Serilization_ChatRoom

使用boost库中的Serilization组件对消息进行序列化,提高了消息通信的安全性.

# Json_ChatRoom 

使用json




