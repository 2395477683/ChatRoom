#ifndef FND_JSON_OBJECT_H
#define FND_JSON_OBJECT_H

#include<sstream>
#include<boost/property_tree/ptree.hpp>
#include<boost/property_tree/json_parser.hpp>
#include<string>

using ptree = boost::property_tree::ptree;

inline std::string ptreeToJsonString(const ptree& tree){            //将 Boost 库中的 property_tree（属性树）对象转换为紧凑的 JSON 格式字符串
    std::stringstream ss;                                           // 创建字符串流对象
    boost::property_tree::write_json(ss,tree,false);                // 将属性树写入流
    return ss.str();                                                // 返回流的字符串内容
}
#endif