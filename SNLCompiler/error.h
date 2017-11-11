//
//  error.h
//  SNLCompiler
//
//  Created by 王振吉 on 2017/11/7.
//  Copyright © 2017年 王振吉. All rights reserved.
//

#ifndef error_h
#define error_h
#include<string>
#include<exception>
#include"exAlgorithm.h"

enum class Period{
    Lex, Syntax, Semantic, Link
};
class HasErrorsException : public std::exception{
    Period time;
public:
    ErrorException(Period _time) :time(_time){}
    Period getPeriod()const { return time; }
    const char* what()const {
        return "Compiler encountered errors and cannot process any longer";
    }
};

#endif /* error_h */
