//
//  snl_corrector.h
//  SNLCompiler
//
//  Created by 王振吉 on 2017/11/11.
//  Copyright © 2017年 王振吉. All rights reserved.
//

#ifndef snl_corrector_h
#define snl_corrector_h

#include"SNLGrammer.h"
#include"corrector.h"
#include"LR1Driver.h"
using Driver = LR1Driver<SNLGrammer, true, safe>;

template<>
class corrector_dispatcher<Driver> : public corrector<Driver>{
public:
    corrector_dispatcher(parser_state_view<Driver> &_pstate):corrector<Driver>(_pstate){}
    void process() override{
        std::cout<<"error occurred.";
        exit(-1);
    }
    virtual std::string get_problem_message() override{
        return "A new problem.";
    }
};
#endif /* snl_corrector_h */
