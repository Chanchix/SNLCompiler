#ifndef PARSER_H_
#define PARSER_H_
#include<stack>
#include"token.h"
#include"lex.h"
#include"lr1driver.h"
#include"exalgorithm.h"
#include"tree.h"
#include"parser_state.h"
#include"corrector.h"


template<class Driver>
class LR1Parser {
	Driver &driver;
    void reduce(const grammer::Rule *, tree<Token> * ,parser_state_view<Driver> &);
public:
	LR1Parser():driver(_Singleton(Driver)) { }
	tree<Token>* getSyntaxTree(TokenList *);
};

template<class Driver>
tree<Token>* LR1Parser<Driver>::getSyntaxTree(TokenList *token_list) {
	assert(token_list != nullptr);
    tree<Token>* syntax_tree = new tree<Token>();
    auto &state_machine = driver.getStateMachine();
	parser_state_view<Driver> pstate(token_list, syntax_tree);
    corrector_dispatcher<Driver> syntax_corrector(pstate);
	pstate.pushState(state_machine.getInitState());
    
	while(true){
		Token &token = pstate.topToken();
        typename Driver::State *state = pstate.topState();
        auto action = state_machine.getAction(state, token.getcode());
		switch(action.getAction()){
            case Action::Accept :{
                tree<Token>::iterator iter = pstate.topNode();
                syntax_tree->insert_subtree(syntax_tree->begin(), iter);
                return syntax_tree;
            }
			case Action::Error	:
                syntax_corrector.process();
                break;
			case Action::Shift	:
				pstate.pushState(action.getState());
                pstate.pushNode(new tree_node(token));
				pstate.popToken();
				break;
			case Action::Reduce :
				auto *rule = action.getRule();
                reduce(rule, syntax_tree, pstate);
				break;
		}
	}
	return nullptr;     //unreachable
}
template<class Driver>
void LR1Parser<Driver>::reduce(const grammer::Rule *rule, tree<Token> *syntax_tree, parser_state_view<Driver> &pstate){
    size_t size = rule->size();
    GrammerCode left = rule->getLeft();
    tree_node *father = new tree_node();
    tree<Token>::iterator_base iter = father;
    unsigned int reduced_row = 0;
    bool first_append = true;
    while(!pstate.empty_state() && size-- > 0) {
        tree<Token>::iterator_base new_node_iterator = pstate.topNode();
        if(first_append){
            iter = syntax_tree->append_child(iter, new_node_iterator);
            first_append = false;
        }
        else iter = syntax_tree->insert_subtree(iter, new_node_iterator);
        reduced_row = new_node_iterator.node->data.getrow();
        pstate.popNode();
        pstate.popState();
    }
    assert(!pstate.empty_state());
    if(first_append) reduced_row = pstate.topToken().getrow();
    father->data = Token(left, reduced_row);
    auto &state_machine = driver.getStateMachine();
    auto *reduced_state = state_machine.getAction(pstate.topState(), left).getState();
    pstate.pushState(reduced_state);
    pstate.pushNode(father);
}
#endif
