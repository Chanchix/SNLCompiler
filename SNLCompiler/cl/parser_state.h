//
//  parser_state.h
//  SNLCompiler
//
//  Created by 王振吉 on 2017/11/5.
//  Copyright © 2017年 王振吉. All rights reserved.
//

#ifndef parser_state_h
#define parser_state_h
#include"tree.h"
typedef tree_node_<Token> tree_node;

template<class Driver>
class parser_state_view{
    std::stack< typename Driver::State* > states_stack;
    std::stack< tree_node* > nodes_stack;
    TokenList *token_list;
    tree<Token> *stree;
    TokenList::iterator token_iterator;
public:
    parser_state_view(TokenList *tl, tree<Token> *_stree):token_list(tl),token_iterator(tl->begin()),stree(_stree) {}
    
    inline void pushState(typename Driver::State* state) { states_stack.push(state); }
    inline typename Driver::State* topState() { return  states_stack.top(); }
    inline void popState() { states_stack.pop(); }
    
    inline Token& topToken(){ return *token_iterator; }
    inline void popToken(){ token_iterator++; }
    
    inline void popNode(){ nodes_stack.pop(); }
    inline void pushNode(tree_node *node){ nodes_stack.push(node); }
    inline tree_node* topNode(){ return nodes_stack.top(); }
    
    bool empty_token()const {return token_iterator == token_list->end(); }
    bool empty_state()const { return states_stack.empty();}
    bool empty_node()const { return nodes_stack.empty();}
};
#endif /* parser_state_h */
