//
//  corrector.h
//  SNLCompiler
//
//  Created by 王振吉 on 2017/11/4.
//  Copyright © 2017年 王振吉. All rights reserved.
//

#ifndef corrector_h
#define corrector_h
#include<iostream>

#include"parser_state.h"

template<class Driver>
class corrector{
protected:
    parser_state_view<Driver> &pstate;
public:
    corrector(parser_state_view<Driver> &_pstate):pstate(_pstate){}
    virtual void process() = 0;
};
template<class Driver>
class corrector_dispatcher : public corrector<Driver>{
public:
    corrector_dispatcher(parser_state_view<Driver> &_pstate):corrector<Driver>(_pstate){}
    void process(){
        std::cout<<"error occurred.";
        exit(-1);
    }
};

#endif /* corrector_h */
