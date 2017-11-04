#ifndef PARSER_H_
#define PARSER_H_
#include<stack>
#include"token.h"
#include"lex.h"
#include"LR1Driver.h"
#include"exAlgorithm.h"
#include"tree.h"

template<class Driver>
class LR1Parser {
	class parser_state;
	Driver &driver;
public:
	LR1Parser():driver(_Singleton(Driver)) { }
	tree<Token>* getSyntaxTree(TokenList *);
};

template<class Driver>
class LR1Parser<Driver>::parser_state{
    std::stack< typename Driver::State* > states_stack;
	TokenList *token_list;
    tree<Token> *stree;
	TokenList::iterator token_iterator;
public:
	parser_state(TokenList *tl, tree<Token> *_stree):token_list(tl),token_iterator(tl->begin()),stree(_stree) {}
    inline void pushState(typename Driver::State* state) {
		states_stack.push(state);
	}
	inline typename Driver::State* topState(){
	 	return  states_stack.top();
	}
	inline void popState(){
		states_stack.pop();
	}
	inline Token& topToken(){
		return *token_iterator;
	}
	inline void popToken(){
		token_iterator++;
	}
	bool empty_token()const {return token_iterator == token_list->end(); }
	bool empty_state()const { return states_stack.empty();}
};

template<class Driver>
tree<Token>* LR1Parser<Driver>::getSyntaxTree(TokenList *token_list) {
	assert(token_list != nullptr);
    tree<Token>* syntax_tree = new tree<Token>();
	auto state_machine = driver.getStateMachine();
	parser_state pstate(token_list, syntax_tree);
	pstate.pushState(state_machine.getInitState());
	while(true){
		Token &token = pstate.topToken();
        typename Driver::State *state = pstate.topState();
        auto action = state_machine.getAction(state, token.getcode());
		switch(action.getAction()){
			case Action::Accept :
				std::cout<<"Success.\n";
				return nullptr;
			case Action::Error	:
				return nullptr;
			case Action::Shift	:
				pstate.pushState(action.getState());
				pstate.popToken();
				break;
			case Action::Reduce :
				auto *rule = action.getRule();
				size_t size = rule->size();
				GrammerCode left = rule->getLeft();
                while(!pstate.empty_state() && size > 0) { pstate.popState(); --size;}
				if(pstate.empty_state()) {
					return nullptr;
				}
				auto *reduced_state = state_machine.getAction(pstate.topState(), left).getState();
				pstate.pushState(reduced_state);
				break;
		}
	}
	return nullptr;
}

#endif
