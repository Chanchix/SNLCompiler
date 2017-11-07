//
//  error.h
//  SNLCompiler
//
//  Created by 王振吉 on 2017/11/7.
//  Copyright © 2017年 王振吉. All rights reserved.
//

#ifndef error_h
#define error_h
#include"interface.h"

class SyntaxError{
public:
    enum syntax_error{
        
    };
    static std::string toString(syntax_error)const ;
}
class SyntaxWarning{
public:
    enum syntax_warning{
        
    };
    static std::string toString(syntax_error)const ;
};
#endif /* error_h */
