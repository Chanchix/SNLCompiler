#ifndef LR1DRIVER_H_ 
#define LR1DRIVER_H_ 

#include<tuple>
#include<set>
#include<queue>
#include<algorithm>
#include<cassert>
#include<exception>
#include <iostream>
#include"grammer.h"
#include"exAlgorithm.h"

#define RulePtr 0
#define Position 1
#define LR1TEMPLATE template<class SpecifiedGrammer, bool isLA, SafeMode mode>
#define LR1DriverInst LR1Driver<SpecifiedGrammer, isLA, mode>

typedef  std::tuple<const grammer::Rule*, size_t> Item;
using FirstSet = std::map<int, std::set<LexCode>>;
enum class Action { Shift, Reduce, Accept, Error };
enum SafeMode { safe, no_conflict_check };

template<class SpecifiedGrammer, bool isLA = true, SafeMode mode = safe>
class LR1Driver {
private:
	friend class exAlgorithm::Singleton<LR1DriverInst>;
	class non_duplicate_comp;
	FirstSet first;
	SpecifiedGrammer &_grammer;

	LR1Driver() :_grammer(_Singleton(SpecifiedGrammer)){ get_first(); }
	std::set<LexCode> get_first(const grammer::Rule &);
	const FirstSet& get_first();
	size_t last_empty(const grammer::Rule &);
	void merge(FirstSet&, FirstSet &, GrammerCode, GrammerCode);
	static std::set<LexCode> get_prospect(const grammer::Rule &, size_t, const std::set<LexCode>& = {});
public:
	class State;
	class StateMachine;
	class TableItem;
    class ReduceCheckException;
    
    
	LR1Driver& operator=(const LR1Driver&) = delete;
	LR1Driver(const LR1Driver&) = delete;
	StateMachine getStateMachine() { return StateMachine(); }
};

LR1TEMPLATE
class LR1DriverInst::ReduceCheckException : std::exception{
public:
    enum ErrorCode{ RR, SR};
private:
    ErrorCode err_code;
    const State &state;
    std::string message;
public:
    ReduceCheckException(ErrorCode ec, const State& _state, const std::string &msg):
    err_code(ec), message(std::move(msg)),state(_state){}
    ErrorCode getErrorCode(){ return err_code; }
    const State& getConflictState(){ return state; }
    virtual const char* what(){ return message.c_str();}
};

//返回产生式含有空递推非终极符的个数
LR1TEMPLATE
size_t LR1DriverInst::last_empty(const grammer::Rule &rule) {
	size_t pos = 0;
	for (; pos < rule.length(); pos++) {
		int code = rule[pos];
		if (_grammer.IsLexCode(code)) return pos;
		auto iter = first[code].find(_grammer.getEmpty());
		if (iter == first[code].end()) return pos;
	}
	return pos;
}
LR1TEMPLATE
void LR1DriverInst::merge(FirstSet &first, FirstSet &result, GrammerCode father, GrammerCode child) {
	if (_grammer.IsLexCode(child) || child == _grammer.getEmpty())
		result[father].insert(child);
	else if(!first[child].empty())
		result[father].insert(first[child].begin(), first[child].end());	//合并	
	
}
LR1TEMPLATE
const FirstSet& LR1DriverInst::get_first() {
	if (!first.empty()) return first;
	auto &_result = first;
	auto expand = [&](FirstSet &result, FirstSet &mid)->FirstSet* {
		FirstSet *next_mid = new FirstSet();
		for (auto &rule_list : _grammer.getRules()) {		
			GrammerCode code = rule_list.first;							//产生式左部
			bool has_empty = false;
			for (const auto &rule : rule_list.second) {
				bool merged = false;							//是否做过并集
				size_t empty_num = last_empty(rule);				//查询改产生式可导空的非终极符数量
				for (int i = 0; i < empty_num; i++) {
					GrammerCode right_code = rule[i];					//可导空的产生式右部非终极符代码
					auto iter = mid.find(right_code);			//检查该非终极符的first集合上次是否生成
					if ((iter != mid.end())) {
						merge(mid, *next_mid, code, right_code);
						merged = true;
					}
				}
				if (empty_num < rule.length() && merged) (*next_mid)[code].erase(_grammer.getEmpty());
				if (empty_num == rule.size()) has_empty = true;
				else merge(mid, *next_mid, code, rule[empty_num]);
			}
			if(has_empty) merge(result, *next_mid, code, _grammer.getEmpty());
		}
		return next_mid;
	};
	exAlgorithm::map_set_closure(_result, expand, false);
	return _result;
}

//已经获取的first集合，求一个句型的first集合
LR1TEMPLATE
std::set<LexCode> LR1DriverInst::get_first(const grammer::Rule &rule) {
	get_first();
	std::set<LexCode> result;
	if (rule.empty()) {
		result.insert(_grammer.getEmpty());
		return result;
	}
	size_t empty_num = last_empty(rule);
	for (size_t i = 0; i < empty_num; i++) {
		auto _set = first[rule[i]];
		result.insert(_set.begin(), _set.end());
	}
	if (empty_num < rule.length()) {
		auto right = rule.getRight();
		GrammerCode right_code = right[empty_num];
		if (_grammer.IsLexCode(right_code)) result.insert(right_code);
		else result.insert(first[right_code].begin(), first[right_code].end());	//合并
		if (empty_num > 0) result.erase(result.find(_grammer.getEmpty()));
	}
	return result;
}


LR1TEMPLATE
class LR1DriverInst::TableItem {
	Action action;
	union dependent_union {			//存储规约用的产生式或转入的状态
		grammer::Rule* pRule;
		State* nextState;
	};
	dependent_union dependent;
public:
	TableItem(Action = Action::Error, const void* = nullptr);
	Action getAction() { return action; }
	const grammer::Rule* getRule() {
		assert(action == Action::Reduce);
		return dependent.pRule;
	}
	State* getState() {
		assert(action == Action::Shift);
		return dependent.nextState;
	}
};

LR1TEMPLATE
LR1DriverInst::TableItem::TableItem(Action act, const void *ptr) :action(act) {
	switch (act) {
	case(Action::Reduce): dependent.pRule = (grammer::Rule*)ptr; break;
	case(Action::Shift): dependent.nextState = (State*)ptr; break;
	default: break;
	}
}

LR1TEMPLATE
class LR1DriverInst::State {
	friend class StateMachine;
	std::map<Item, std::set<LexCode>> items;
	std::map<GrammerCode, TableItem> gotoAct;
	std::set<Item> cores;
	void _lookahead(const Item&, const std::set<LexCode>&);
	void _add_item(const grammer::Rule *, size_t, std::set<LexCode>&&);
	void _add_item(const Item &item, std::set<LexCode>&& forward) {
		_add_item(std::get<RulePtr>(item), std::get<Position>(item), std::move(forward));
	}
public:
	std::map<GrammerCode, std::list<Item>> allMarks();
	void merge_lalr_state(const State *);
	void conflict_check() const;
	void addItem(const Item &item, std::set<LexCode>&& forward);
	void addItem(const Item &item, const std::set<LexCode>& forward) { addItem(item, std::set<LexCode>(forward)); }
	void addAction(GrammerCode, const TableItem &);
	const std::map<Item, std::set<LexCode>>& getProjects() const { return items; }
	const std::map<GrammerCode, TableItem>& getAction() const { return gotoAct; }
	const std::set<Item> getCores()const { return cores; }
    static bool isShiftItem(const Item &);
    static bool isReduceItem(const Item &);
	void print()const {
		auto print_item = [](const Item &item)->void {
			auto *rule = std::get<RulePtr>(item);
			size_t position = std::get<Position>(item);
			std::cout << rule->getLeft() << " -> ";
			const auto &right = rule->getRight();
			for (size_t i = 0; i < right.size(); i++) {
				if (i == position) std::cout << ". ";
				std::cout << right[i] << " ";
			}
			if (position == right.size()) std::cout << ". ";
			if (right.size() == 0) std::cout << "e";            //产程式为空的输出
		};
		for (const auto &pair : items) {
			print_item(pair.first);
			std::cout << " , ";
			for (auto pro : pair.second) std::cout << pro << '/';
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
};
LR1TEMPLATE
bool LR1DriverInst::State::isShiftItem(const Item &item) {
    auto *rule = std::get<RulePtr>(item);
    size_t position = std::get<Position>(item);
    assert(rule->size() >= position);
    if (rule->size() > position) return true;
    else return false;
}
LR1TEMPLATE
bool LR1DriverInst::State::isReduceItem(const Item &item) {
    return !isShiftItem(item);
}

LR1TEMPLATE
void LR1DriverInst::State::merge_lalr_state(const State *other) {
	non_duplicate_comp comp;
	//断言两个状态有同心核
	assert(isLA);
	assert(!comp(this, other));
	assert(!comp(other, this));
	const auto &projects = other->getProjects();
	for (const auto &item : other->cores) {
		auto forward_iter = projects.find(item);
		assert(forward_iter != projects.end());
		_lookahead(item, forward_iter->second);
	}
}

LR1TEMPLATE
void LR1DriverInst::State::conflict_check()const {
	std::set<LexCode> shift, reduce;
	for (const auto &item_pair : items) {
		const auto &item = item_pair.first;
		if (isShiftItem(item)) shift.insert((*std::get<RulePtr>(item))[std::get<Position>(item)]);
		else {
			auto oldsize = reduce.size();
			auto item_iter = items.find(item);
			auto forward_size = item_iter->second.size();
			reduce = exAlgorithm::set_union(reduce, item_iter->second);
			if (reduce.size() < oldsize + forward_size) {
                std::string errorMessage = "Reduce-Reduce conflict detected.";
                throw ReduceCheckException(ReduceCheckException::SR, *this, errorMessage);
			}
		}
	}
	std::set<LexCode> allMarks = exAlgorithm::set_union(reduce, shift);
	if (allMarks.size() < reduce.size() + shift.size()){
        std::string errorMessage = "Shift-Reduce conflict detected.";
        throw ReduceCheckException(ReduceCheckException::SR, *this, errorMessage);
	}
}

//求出所有项目A->γ.αβ(α∈Vn∪Vt)中α的部分和返回该项目,并将所有规约项填入Action表中
LR1TEMPLATE
std::map<GrammerCode, std::list<Item>> LR1DriverInst::State::allMarks()
{
	using ActionValue = typename decltype(gotoAct)::value_type;
	std::map<GrammerCode, std::list<Item>> result;
	for (auto &pair : items) {
		auto *rule = std::get<RulePtr>(pair.first);
		size_t position = std::get<Position>(pair.first);
		assert(position <= rule->size());
		if (position == rule->size()) {
            auto reduceItem = TableItem(Action::Reduce, (const void *)rule);//项目已经探索到结尾
			for (LexCode prospect : pair.second) {
				auto actValue = ActionValue(prospect, reduceItem);
				gotoAct.insert(actValue);										//添加规约项目
			}
			continue;
		}
		else result[(*rule)[position]].push_back(Item(rule, position));
	}
	return result;
}

LR1TEMPLATE
void LR1DriverInst::State::_lookahead(const Item &item, const std::set<LexCode>& forward){
	assert(isLA);
	assert(cores.find(item) != cores.end());
	assert(items.find(item) != items.end());
	static auto &_grammer = _Singleton(SpecifiedGrammer);
	std::set<LexCode> diff = exAlgorithm::set_difference(forward, items.find(item)->second);
	if (diff.empty()) return;
	
	using ProjectSet = std::map<Item, std::set<LexCode>>;
	ProjectSet *first_mid = new ProjectSet();
    first_mid->insert(ProjectSet::value_type(item, diff));
	auto expand = [this](ProjectSet &result, ProjectSet &mid)->ProjectSet* {
		ProjectSet *next_mid = new ProjectSet();
		for (auto &pair : mid) {
			const Item &item = pair.first;
			auto &lookAhead = pair.second;
            result[item].insert(lookAhead.begin(), lookAhead.end());
            size_t position = std::get<Position>(item);
			auto *rule = std::get<RulePtr>(item);
			size_t ruleSize = (int)rule->size();
			assert(position <= ruleSize);
            
            if (position == ruleSize){
                for(LexCode code : lookAhead) addAction(code, TableItem(Action::Reduce, rule));
                continue;    //项目点位置在最后则不会将展望符传播下去
            }
			GrammerCode next_mark = (*rule)[position];			//传播到下一个状态对应的输入符号
            auto table_item_iter = gotoAct.find(next_mark);
            if (table_item_iter != gotoAct.end()) {
                State *state = table_item_iter->second.getState();
                Item propagated_item = Item(rule, position + 1);
                state->_lookahead(propagated_item, lookAhead);
            }
            if(_grammer.IsGrammerCode(next_mark)){
                auto &rule_right = rule->getRight();
                std::vector<GrammerCode> right_for_pros(rule_right.begin() + position + 1, rule_right.end());            //项目未探索部分β
                auto right_first = _Singleton(LR1DriverInst).get_first(grammer::Rule(0, std::move(right_for_pros)));
                if (right_first.find(_grammer.getEmpty()) != right_first.end()) {
                    const auto &rules = _grammer.getRules();
                    auto rule_list_pair = rules.find(next_mark);
                    for (const grammer::Rule &rule : rule_list_pair->second) {
                        Item item(&rule, 0);
                        next_mid->insert(ProjectSet::value_type(item, lookAhead));
                    }
                }
            }
		}
		return next_mid;
	};
	exAlgorithm::map_set_closure(items, expand, false, first_mid);
}

LR1TEMPLATE
void LR1DriverInst::State::_add_item(
	const grammer::Rule* pRule, size_t position, std::set<LexCode>&& last_prospect) {
	assert(pRule->size() >= position);
	static auto &_grammer = _Singleton(SpecifiedGrammer);

	Item item(pRule, position);
	if (items.find(item) == items.end()) items[item] = std::move(last_prospect);
	else {
		auto &forward = items[item];
		size_t oldsize = forward.size();
		forward.insert(last_prospect.begin(), last_prospect.end());
		if (oldsize == forward.size()) return;
	}
	auto &forward = items[item];
	if (pRule->size() == position) {
		for(LexCode f : forward) gotoAct.insert(std::make_pair(f, TableItem(Action::Reduce, pRule)));
		return;
	}
	GrammerCode lcode = (*pRule)[position];

	auto &rules = _grammer.getRules();//递归求closure闭包
	auto find_iter = rules.find(lcode);
	if (find_iter == rules.end()) return;
	const auto &ruleList = find_iter->second;
	for (auto &rule : ruleList) {
		std::set<LexCode> prospect = get_prospect(*pRule, position + 1, forward);	//求出展望符集first(βa)
		_add_item(&rule, 0, std::move(prospect));
	}
}
LR1TEMPLATE
void LR1DriverInst::State::addItem(const Item &item, std::set<LexCode>&& forward) {
	cores.insert(item);
	_add_item(item, std::move(forward));
}

LR1TEMPLATE
void LR1DriverInst::State::addAction(GrammerCode input, const TableItem &tableItem) {
	gotoAct[input] = tableItem;
}
/******************
比较原则：先比较项目个数，相同再比较LR(0)项目，最后比较展望集合
******************/
LR1TEMPLATE
class LR1DriverInst::non_duplicate_comp {

public:
	bool operator ()(const State *a, const State *b)const {
		const auto &aProjects = a->getProjects(), &bProjects = b->getProjects();
		const auto &aCores = a->getCores(), &bCores = b->getCores();
		if (aCores.size() < bCores.size()) return true;
		if (aCores.size() > bCores.size()) return false;
		for (auto a_iter = aCores.begin(), b_iter = bCores.begin();
			a_iter != aCores.end();
			a_iter++, b_iter++) {
			if (*a_iter < *b_iter) return true;
			if (*a_iter > *b_iter) return false;
			if (!isLA) {
				auto find_iter_a = aProjects.find(*a_iter);
				auto find_iter_b = bProjects.find(*b_iter);
				assert(find_iter_a != aProjects.end());
				assert(find_iter_b != bProjects.end());
				if (find_iter_a->second < find_iter_b->second) return true;
				if (find_iter_b->second < find_iter_a->second) return false;
			}
		}
		return false;					//默认返回值，其实代表了相等的情况
	}
};
LR1TEMPLATE
std::set<LexCode> LR1DriverInst::get_prospect(
	const grammer::Rule &rule, size_t position, const std::set<LexCode> &last_prospect)
{
	assert(position <= rule.size());
	static auto &_grammer = _Singleton(SpecifiedGrammer);

	std::vector<GrammerCode> right = rule.getRight();
	std::vector<GrammerCode> right_for_pros(right.begin() + position, right.end());			//项目未探索部分β
	using Driver = LR1DriverInst;
	auto rfp_first = _Singleton(Driver).get_first(grammer::Rule(0, std::move(right_for_pros)));
	auto find_iter = rfp_first.find(_grammer.getEmpty());
	if (find_iter != rfp_first.end()) {
		rfp_first.erase(find_iter);
		rfp_first.insert(last_prospect.begin(), last_prospect.end());
	}
	return rfp_first;
}

LR1TEMPLATE
class LR1DriverInst::StateMachine {
	State *initState;
	State *finalState;
	std::set<State*> states;												//以指针为比较基础的map
	std::set<State *, non_duplicate_comp> non_duplicate_states;			//以状态值为比较基础的map
	std::set<State*> expandState(State *old);
public:
	StateMachine();
	~StateMachine();
	StateMachine(StateMachine &&);
	State *getInitState() const { return initState; }
	const std::set<State*>& getStates()const { return states; }
	TableItem getAction(State *, GrammerCode) const;
	void print() const{
		std::cout << "States:" << std::endl << std::endl;
		for (State *state : states) {
			state->print();
		}
		std::cout << std::endl;
	}
};

//填好Action和goto表并返回新生成状态
LR1TEMPLATE
std::set<typename LR1DriverInst::State* >
LR1DriverInst::StateMachine::expandState(State *old)
{
	std::set<State *> result;
	const auto &marks = old->allMarks();					//填好Reduce部分
	auto &projects = old->getProjects();
	for (auto &pair : marks) {	
		GrammerCode mark = pair.first;
		State *to_state = new State();
		for (const auto &item : pair.second) {
			auto *pRule = std::get<RulePtr>(item);
			size_t position = std::get<Position>(item) + 1;
			const std::set<LexCode> &last_prospect = projects.find(item)->second;
			to_state->addItem(Item(pRule, position), last_prospect);			
		}		
		auto find_iter = non_duplicate_states.find(to_state);	
		if (find_iter != non_duplicate_states.end()) {
			State *existed_state = *find_iter;
			if (isLA) existed_state->merge_lalr_state(to_state);
			old->addAction(mark, TableItem(Action::Shift, existed_state));
			delete to_state;
		}
		else {
			result.insert(to_state);
			old->addAction(mark, TableItem(Action::Shift, to_state));
		}
	}
	return result;
}
LR1TEMPLATE
typename LR1DriverInst::TableItem
LR1DriverInst::StateMachine::getAction(State *state, GrammerCode input) const {
	auto state_iter = states.find(state);
	assert(state_iter != states.end());
	auto action_iter = state->gotoAct.find(input);
	if (action_iter == state->gotoAct.end()) return TableItem(Action::Error);
	else return action_iter->second;
}
LR1TEMPLATE
LR1DriverInst::StateMachine::StateMachine():finalState(new State()), initState(new State()){
	auto &_grammer = _Singleton(SpecifiedGrammer);
	State *currentState = initState;
	initState->addItem(Item(&_grammer.getExRule(), 0), { _grammer.getLexEnd() });
    initState->addAction(_grammer.getStart(), TableItem(Action::Accept));
	finalState->addItem(Item(&_grammer.getExRule(), 1), { _grammer.getLexEnd() });
	finalState->addAction(SpecifiedGrammer::getLexEnd(), TableItem(Action::Accept));

	std::queue<State*> active;
	active.push(initState);
	states.insert(initState);
	states.insert(finalState);
	non_duplicate_states.insert(initState);
	non_duplicate_states.insert(finalState);
	while (!active.empty()) {
		currentState = active.front();
		active.pop();
		const auto &all_states = expandState(currentState);
		for (const auto &state : all_states) {
			active.push(state);
			states.insert(state);
			non_duplicate_states.insert(state);	
		}
	}
    if(mode < no_conflict_check){
        for (const State *state : states) state->conflict_check();
    }
}
LR1TEMPLATE
LR1DriverInst::StateMachine::StateMachine(StateMachine &&stm):
	initState(stm.initState),
	finalState(stm.finalState),
	states(std::move(stm.states)), 
	non_duplicate_states(std::move(stm.non_duplicate_states))
{
	stm.initState = nullptr;
	stm.finalState = nullptr;
}

LR1TEMPLATE
LR1DriverInst::StateMachine::~StateMachine() {
	for (State *s : states) delete s;
}
#endif
