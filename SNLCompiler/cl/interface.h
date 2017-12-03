//
//  interface.h
//  SNLCompiler
//
//  Created by 王振吉 on 2017/11/5.
//  Copyright © 2017年 王振吉. All rights reserved.
//
#ifndef interface_h
#define interface_h
#include<string>
#include<utility>
#include<exception>
#include"exalgorithm.h"

class Decodable{
public:
    virtual const std::string& code_to_string(unsigned int)const = 0;
    class NoSuchCodeException;
};
class Decodable::NoSuchCodeException : public std::exception{
    unsigned int code;
public:
    NoSuchCodeException(unsigned int c):code(c){}
    unsigned int getCode() { return code; }
    const char* what(){
        return exAlgorithm::string_format("Can not convert code({0}) to string.", code).c_str();
    }
};
class Encodable{
public:
    virtual unsigned int string_to_code(const std::string&)const = 0;
    class NoSuchStringException;
};
class Encodable::NoSuchStringException : public std::exception{
    std::string _string;
public:
    NoSuchStringException(std::string&& str):_string(std::move(str)){}
    NoSuchStringException(const std::string& str):_string(str){}
    std::string getString() { return _string; }
    const char* what(){
        return exAlgorithm::string_format("Can not convert string\"{0}\" to code.", _string).c_str();
    }
};
#endif /* interface_h */
